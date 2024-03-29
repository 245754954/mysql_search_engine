#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include "search.h"
#include "util.h"
#include "token.h"
#include "mysqldatabase.h"
#include "postings.h"

/* 将类型inverted_index_hash/value和postings_list也用于检索 */
typedef inverted_index_hash query_token_hash;
typedef inverted_index_value query_token_value;
typedef postings_list token_positions_list;

typedef struct
{
  token_positions_list *documents; /* 文档编号的序列 */
  token_positions_list *current;   /* 当前的文档编号 */
} doc_search_cursor;

typedef struct
{
  const UT_array *positions; /* 位置信息 */
  int base;                  /* 词元在查询中的位置 */
  int *current;              /* 当前的位置信息 */
} phrase_search_cursor;


/**
 * 根据document size比较两条检索结果
 * @param[in] a 检索结果a的数据
 * @param[in] b 检索结果b的数据
 * @return 得分的大小关系
 */
static int search_results_body_size_desc_size(search_results *a, search_results *b)
{

  return (b->body_size > a->body_size) ? 1 : (b->body_size < a->body_size) ? -1 : 0;
}
/**
 * 根据visit time比较两条检索结果
 * @param[in] a 检索结果a的数据
 * @param[in] b 检索结果b的数据
 * @return 得分的大小关系
 */

static int search_results_score_time(search_results *a, search_results *b)
{

  time_t t1 = mktime(&a->stamp);
  time_t t2 = mktime(&b->stamp);

  double x = difftime(t1, t2);

  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

/**
 * 根据得分比较两条检索结果
 * @param[in] a 检索结果a的数据
 * @param[in] b 检索结果b的数据
 * @return 得分的大小关系
 */
static int
search_results_score_desc_sort(search_results *a, search_results *b)
{
  return (b->score > a->score) ? 1 : (b->score < a->score) ? -1 : 0;
}

/**
 * 比较出现过词元a和词元b的文档数
 * @param[in] a 词元a的数据
 * @param[in] b 词元b的数据
 * @return 文档数的大小关系
 */
static int
query_token_value_docs_count_desc_sort(query_token_value *a,
                                       query_token_value *b)
{
  return b->docs_count - a->docs_count;
}

/**
 * 将文档添加到检索结果中
 * @param[in] results 指向检索结果的指针
 * @param[in] document_id 要添加的文档的编号
 * @param[in] score 得分
 */
static void
add_search_result(wiser_env *env, search_results **results, const int document_id,
                  const double score)
{
  search_results *r;
  if (*results)
  {
    HASH_FIND_INT(*results, &document_id, r);
  }
  else
  {
    r = NULL;
  }
  if (!r)
  {
    if ((r = malloc(sizeof(search_results))))
    {
      r->document_id = document_id;
      r->score = 0;
      if (SORT == "tf-idf")
      {
      }
      else if (SORT == "size")
      {
        db_get_document_size1(env, document_id, &r->body_size);
      }
      else if (SORT == "time-sort")
      {
        db_get_document_visit_time(env, document_id, &r->stamp);
      }

      HASH_ADD_INT(*results, document_id, r);
    }
  }
  if (r)
  {
    r->score += score;
  }
}

/**
 * 进行短语检索
 * @param[in] query_tokens 从查询中提取出的词元信息
 * @param[in] doc_cursors 用于检索文档的游标的集合
 * @return 检索出的短语数
 */
static int
search_phrase(const query_token_hash *query_tokens,
              doc_search_cursor *doc_cursors)
{
  int n_positions = 0;
  const query_token_value *qt;
  phrase_search_cursor *cursors;

  /* 获取查询中词元的总数 */
  for (qt = query_tokens; qt; qt = qt->hh.next)
  {
    n_positions += qt->positions_count;
  }

  if ((cursors = (phrase_search_cursor *)malloc(sizeof(
                                                    phrase_search_cursor) *
                                                n_positions)))
  {
    int i, phrase_count = 0;
    phrase_search_cursor *cur;
    /* 初始化游标 */
    for (i = 0, cur = cursors, qt = query_tokens; qt;
         i++, qt = qt->hh.next)
    {
      int *pos = NULL;
      while ((pos = (int *)utarray_next(qt->postings_list->positions,
                                        pos)))
      {
        cur->base = *pos;
        cur->positions = doc_cursors[i].current->positions;
        cur->current = (int *)utarray_front(cur->positions);
        cur++;
      }
    }
    /* 检索短语 */
    while (cursors[0].current)
    {
      int rel_position, next_rel_position;
      rel_position = next_rel_position = *cursors[0].current -
                                         cursors[0].base;
      /* 对于除词元A以外的词元，不断地向后读取其出现位置，直到其偏移量不小于词元A的偏移量为止 */
      for (cur = cursors + 1, i = 1; i < n_positions; cur++, i++)
      {
        for (; cur->current && (*cur->current - cur->base) < rel_position;
             cur->current = (int *)utarray_next(cur->positions, cur->current))
        {
        }
        if (!cur->current)
        {
          goto exit;
        }

        /* 对于除词元A以外的词元，若其偏移量不等于A的偏移量，就退出循环 */
        if ((*cur->current - cur->base) != rel_position)
        {
          next_rel_position = *cur->current - cur->base;
          break;
        }
      }
      if (next_rel_position > rel_position)
      {
        /* 不断向后读取，直到词元A的偏移量不小于next_rel_position为止 */
        while (cursors[0].current &&
               (*cursors[0].current - cursors[0].base) < next_rel_position)
        {
          cursors[0].current = (int *)utarray_next(
              cursors[0].positions, cursors[0].current);
        }
      }
      else
      {
        /* 找到了短语 */
        phrase_count++;
        cursors->current = (int *)utarray_next(
            cursors->positions, cursors->current);
      }
    }
  exit:
    free(cursors);
    return phrase_count;
  }
  return 0;
}

/**
 * 用TF-IDF计算得分
 * @param[in] query_tokens 查询
 * @param[in] doc_cursors 用于文档检索的游标的集合
 * @param[in] n_query_tokens 查询中的词元数
 * @param[in] indexed_count 建立过索引的文档总数
 * @return 得分
 */
static double
calc_tf_idf(const query_token_hash *query_tokens, doc_search_cursor *doc_cursors, const int n_query_tokens, const int indexed_count)
{
  int i;
  const query_token_value *qt;
  doc_search_cursor *dcur;
  double score = 0;
  for (qt = query_tokens, dcur = doc_cursors, i = 0; i < n_query_tokens; qt = qt->hh.next, dcur++, i++)
  {
    double idf = log2((double)indexed_count / qt->docs_count);
    // printf("the value %d\n",dcur->current->document_id);
    score += (double)dcur->current->positions_count * idf;
  }
  return score;
}

/**
 * 释放词元的出现位置列表
 * @param[in] pl 待释放的出现位置列表的首元素
 */
void free_token_positions_list(token_positions_list *list)
{
  free_postings_list((postings_list *)list);
}

/**
 * 检索文档
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in,out] results 检索结果
 * @param[in] tokens 从查询中提取出的词元信息
 */
void search_docs_and(wiser_env *env, search_results **results, query_token_hash *tokens)
{
  int n_tokens;
  doc_search_cursor *cursors;

  if (!tokens)
  {
    return;
  }

  /* 按照文档频率的升序对tokens排序 */
  HASH_SORT(tokens, query_token_value_docs_count_desc_sort);

  /* 初始化 */
  n_tokens = HASH_COUNT(tokens);
  if (n_tokens && (cursors = (doc_search_cursor *)calloc(sizeof(doc_search_cursor), n_tokens)))
  {
    int i;
    doc_search_cursor *cur;
    query_token_value *token;
    for (i = 0, token = tokens; token; i++, token = token->hh.next)
    {
      if (!token->token_id)
      {
        /* 当前的token在构建索引的过程中从未出现过 */
        goto exit;
      }
      if (fetch_postings(env, token->token_id,
                         &cursors[i].documents, NULL))
      {
        print_error("decode postings error!: %d\n", token->token_id);
        goto exit;
      }
      if (!cursors[i].documents)
      {
        /* 虽然当前的token存在，但是由于更新或删除导致其倒排列表为空 */
        goto exit;
      }

      cursors[i].current = cursors[i].documents;
    }
    // printf("the value of postions %d\n", cursors[0].current->positions_count);

    while (cursors[0].current)
    {
      int doc_id, next_doc_id = 0;
      /* 将拥有文档最少的词元称作A */
      doc_id = cursors[0].current->document_id;
      /* 对于除词元A以外的词元，不断获取其下一个document_id，直到当前的document_id不小于词元A的document_id为止 */
      for (cur = cursors + 1, i = 1; i < n_tokens; cur++, i++)
      {
        while (cur->current && cur->current->document_id < doc_id)
        {
          cur->current = cur->current->next;
        }
        //go to the ending of the list
        if (!cur->current)
        {
          goto exit;
        }
        /* 对于除词元A以外的词元，如果其document_id不等于词元A的document_id，*/
        /* 那么就将这个document_id设定为next_doc_id */
        if (cur->current->document_id != doc_id)
        {
          next_doc_id = cur->current->document_id;
          break;
        }
      }
      if (next_doc_id > 0)
      {
        /* 不断获取A的下一个document_id，直到其当前的document_id不小于next_doc_id为止 */
        while (cursors[0].current && cursors[0].current->document_id < next_doc_id)
        {
          cursors[0].current = cursors[0].current->next;
        }
      }
      else
      {
        int phrase_count = -1;
        if (env->enable_phrase_search)
        {
          phrase_count = search_phrase(tokens, cursors);
        }
        if (phrase_count)
        {

          double score = calc_tf_idf(tokens, cursors, n_tokens,
                                     env->indexed_count);
          add_search_result(env, results, doc_id, score);
        }

        cursors[0].current = cursors[0].current->next;
      }
    }
  exit:
    for (i = 0; i < n_tokens; i++)
    {
      if (cursors[i].documents)
      {
        free_token_positions_list(cursors[i].documents);
      }
    }
    free(cursors);
  }

  free_inverted_index(tokens);

  if (SORT == "tf-idf")
  {
    HASH_SORT(*results, search_results_score_desc_sort);
  }
  else if (SORT == "size")
  {
    HASH_SORT(*results, search_results_body_size_desc_size);
  }
  else if (SORT == "time-sort")
  {
    HASH_SORT(*results, search_results_score_time);
  }
}

/**
 * 检索文档
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in,out] results 检索结果
 * @param[in] tokens 从查询中提取出的词元信息
 */

static int intsort(const void *_a, const void *_b)
{
  const int *a = (const int *)_a;
  const int *b = (const int *)_b;

  if (*a > *b)
    return 1;
  else if (*a == *b)
    return 0;
  else
    return -1;
}

void search_docs_or(wiser_env *env, search_results **results, query_token_hash *tokens)
{
  int n_tokens;
  doc_search_cursor *cursors;

  if (!tokens)
  {
    return;
  }

  /* 按照文档频率的升序对tokens排序 */
  HASH_SORT(tokens, query_token_value_docs_count_desc_sort);

  /* 初始化 */

  n_tokens = HASH_COUNT(tokens);
  if (n_tokens && (cursors = (doc_search_cursor *)calloc(sizeof(doc_search_cursor), n_tokens)))
  {
    int i;
    doc_search_cursor *cur;
    doc_search_cursor *head;
    query_token_value *token;
    for (i = 0, token = tokens; token; i++, token = token->hh.next)
    {
      if (!token->token_id)
      {
        /* 当前的token在构建索引的过程中从未出现过 */
        goto exit;
      }
      if (fetch_postings(env, token->token_id, &cursors[i].documents, NULL))
      {
        print_error("decode postings error!: %d\n", token->token_id);
        goto exit;
      }

      if (!cursors[i].documents)
      {
        /* 虽然当前的token存在，但是由于更新或删除导致其倒排列表为空 */
        goto exit;
      }

      cursors[i].current = cursors[i].documents;
    }

    UT_array *arr;
    utarray_new(arr, &ut_int_icd);
    token_positions_list *cur_next = 0;
    for (cur = cursors, cur_next = cur->current, i = 0; i < n_tokens; cur++, i++)
    {
      while (cur_next)
      {

        utarray_sort(arr, intsort);
        int *p1 = NULL;
        p1 = utarray_find(arr, &cur_next->document_id, intsort);
        if (p1 == NULL)
        {
          utarray_push_back(arr, &cur_next->document_id);
        }
        cur_next = cur_next->next;
      }
    }

    int *p;
    for (p = (int *)utarray_front(arr); p != NULL; p = (int *)utarray_next(arr, p))
    {
      // printf("line %d  the value of postions %d\n", __LINE__, *p);

      int phrase_count = -1;
      if (phrase_count)
      {
        double score = calc_tf_idf(tokens, cursors, n_tokens, env->indexed_count);
        add_search_result(env, results, *p, score);
      }
    }
    utarray_free(arr);

  exit:
    for (i = 0; i < n_tokens; i++)
    {
      if (cursors[i].documents)
      {
        free_token_positions_list(cursors[i].documents);
      }
    }
    free(cursors);
  }
  free_inverted_index(tokens);
  if (SORT == "tf-idf")
  {
    HASH_SORT(*results, search_results_score_desc_sort);
  }
  else if (SORT == "size")
  {
    HASH_SORT(*results, search_results_body_size_desc_size);
  }
  else if (SORT == "time-sort")
  {
    HASH_SORT(*results, search_results_score_time);
  }
}

/*
 * 从查询字符串中提取出词元的信息
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] text 查询字符串
 * @param[in] text_len 查询字符串的长度
 * @param[in] n N-gram中N的取值
 * @param[in,out] query_tokens 按词元编号存储位置信息序列的关联数组
 *                             若传入的是指向NULL的指针，则新建一个关联数组
 * @retval 0 成功
 * @retval -1 失败
 */
int split_query_to_tokens(wiser_env *env,
                          const UTF32Char *text,
                          const unsigned int text_len,
                          const int n, query_token_hash **query_tokens)
{

  return text_to_postings_lists(env,
                                0, /* 将document_id设为0 */
                                text, text_len, n,
                                (inverted_index_hash **)query_tokens);
}

/**
 * 打印检索结果
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] results 检索结果
 */
void print_search_results(wiser_env *env, search_results *results)
{
  int num_search_results;

  if (!results)
  {
    return;
  }

  num_search_results = HASH_COUNT(results);
  int count = 0;

  while (results)
  {
    

    int title_len;
    const char *title;
    search_results *r;

    r = results;
    HASH_DEL(results, r);
    count++;
    if (count < K)
    {

      db_get_document_title1(env, r->document_id, &title, &title_len);
      char myStr[255] = "\0";
      if (SORT == "time-sort")
      {
        char *myFormat = "%Y-%m-%d:%H:%M:%S"; //自定义格式
        //strftime 第一个参数是 char *
        strftime(myStr, 255, myFormat, &r->stamp);
      }

      if (SORT == "tf-idf")
      {

        printf("document_id: %d title: %.*s score: %lf\n", r->document_id, title_len, title, r->score);
      }
      else if (SORT == "size")
      {
        printf("document_id: %d title: %.*s score: %lf  body-size: %d d\n",
               r->document_id, title_len, title, r->score, r->body_size);
      }
      else if (SORT == "time-sort")
      {
        printf("document_id: %d title: %.*s score: %lf  visit-time: %s \n",
               r->document_id, title_len, title, r->score, myStr);
      }
    }
    free(r);
  }

  printf("Total %u documents are found!\n", num_search_results);
}


void print_search_results_for_browser(wiser_env *env, search_results *results,char **result)
{
  int num_search_results;

  if (!results)
  {
    return;
  }

  num_search_results = HASH_COUNT(results);
  int count = 0;

  while (results)
  {
    

    int title_len;
    const char *title;
    search_results *r;

    r = results;
    HASH_DEL(results, r);
    count++;
    if (count < K)
    {
      char tmp[4096]={'\0'};

      db_get_document_title1(env, r->document_id, &title, &title_len);
      char myStr[255] = "\0";
      if (SORT == "time-sort")
      {
        char *myFormat = "%Y-%m-%d:%H:%M:%S"; //自定义格式
        //strftime 第一个参数是 char *
        strftime(myStr, 255, myFormat, &r->stamp);
      }

      if (SORT == "tf-idf")
      {

        // printf("document_id: %d title: %.*s score: %lf\n", r->document_id, title_len, title, r->score);
      }
      else if (SORT == "size")
      {
        // printf("document_id: %d title: %.*s score: %lf  body-size: %d d\n", r->document_id, title_len, title, r->score, r->body_size);
      }
      else if (SORT == "time-sort")
      {
        // printf("document_id: %d title: %.*s score: %lf  visit-time: %s \n",r->document_id, title_len, title, r->score, myStr);
      }

      sprintf(tmp,"document_id: %d ,title %s:  score %lf \n",r->document_id,title,r->score);
      strcat(*result,tmp);
      memset(tmp,0,sizeof(tmp));

    }
    free(r);
  }
  char tot[1024]={'\0'};
  sprintf(tot,"Total %u documents are found!\n", num_search_results);
  strcat(*result,tot);
 
     
  //printf("Total %u documents are found!\n", num_search_results);
}

/**
 * 进行全文检索
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] query 查询
 */
void search(wiser_env *env, const char *query)
{
  int query32_len;
  UTF32Char *query32;

  if (!utf8toutf32(query, strlen(query), &query32, &query32_len))
  {
    search_results *results = NULL;

    if (query32_len < env->token_len)
    {
      // print_error("too short query.");

      char **p;
      UT_array *partial_tokens;

      utarray_new(partial_tokens, &ut_str_icd);
      token_partial_match1(env, query, strlen(query), partial_tokens);

      for (p = (char **)utarray_front(partial_tokens); p; p = (char **)utarray_next(partial_tokens, p))
      {
        inverted_index_hash *query_postings = NULL;
        token_to_postings_list(env, 0, *p, strlen(*p), 0, &query_postings);

        search_docs_and(env, &results, query_postings);
      }

      utarray_free(partial_tokens);
    }
    else
    {
      query_token_hash *query_tokens = NULL;
      split_query_to_tokens(env, query32, query32_len, env->token_len, &query_tokens);
      int begintime, endtime;

      if (env->enable_or_query)
      {
        begintime = clock(); //计时开始

        search_docs_or(env, &results, query_tokens);
        endtime = clock();
        printf("\n\nsearch Time：%lf ms\n", (double)1000 * (endtime - begintime) / CLOCKS_PER_SEC);
      }
      else
      {

        begintime = clock(); //计时开始

        search_docs_and(env, &results, query_tokens);
        endtime = clock();
        printf("\n\nsearch Time：%lf ms\n", (double)1000 * (endtime - begintime) / CLOCKS_PER_SEC);
      }
    }

    print_search_results(env, results);

    free(query32);
  }
}

void search_for_browser(wiser_env *env, const char *query,char **result){

  int query32_len;
  UTF32Char *query32;

  if (!utf8toutf32(query, strlen(query), &query32, &query32_len))
  {
    search_results *results = NULL;

    if (query32_len < env->token_len)
    {
      // print_error("too short query.");

      char **p;
      UT_array *partial_tokens;

      utarray_new(partial_tokens, &ut_str_icd);
      token_partial_match1(env, query, strlen(query), partial_tokens);

      for (p = (char **)utarray_front(partial_tokens); p; p = (char **)utarray_next(partial_tokens, p))
      {
        inverted_index_hash *query_postings = NULL;
        token_to_postings_list(env, 0, *p, strlen(*p), 0, &query_postings);

        search_docs_and(env, &results, query_postings);
      }

      utarray_free(partial_tokens);
    }
    else
    {
      query_token_hash *query_tokens = NULL;
      split_query_to_tokens(env, query32, query32_len, env->token_len, &query_tokens);
      int begintime, endtime;

      if (env->enable_or_query)
      {
        //begintime = clock(); //计时开始

        search_docs_or(env, &results, query_tokens);
        //endtime = clock();
       // printf("\n\nsearch Time：%lf ms\n", (double)1000 * (endtime - begintime) / CLOCKS_PER_SEC);
      }
      else
      {

       // begintime = clock(); //计时开始

        search_docs_and(env, &results, query_tokens);
       // endtime = clock();
        //printf("\n\nsearch Time：%lf ms\n", (double)1000 * (endtime - begintime) / CLOCKS_PER_SEC);
      }
    }

    print_search_results_for_browser(env, results,result);

    free(query32);
  }

}