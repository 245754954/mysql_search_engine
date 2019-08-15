#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include "util.h"
#include "token.h"
#include "search.h"
#include "postings.h"
#include "wikiload.h"
#include "mysqldatabase.h"
#include "searchengine.h"


/**
 * 将文档添加到数据库中，建立倒排索引
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] title 文档标题，为NULL时将会清空缓冲区
 * @param[in] body 文档正文
 */
void add_document(wiser_env *env, const char *title, const char *body, const char *timestamp)
{

  if (title && body)
  {

    UTF32Char *body32;
    int body32_len, document_id;
    unsigned int title_size, body_size, timestamp_size;

    title_size = strlen(title);
    body_size = strlen(body);
    timestamp_size = strlen(timestamp);
    /* 将文档存储到数据库中S并获取该文档对应的文档编号 */
    char path[255]={'\0'};
    sprintf(path,"/home/hadoop/wiser/%s",title);
  /*
    FILE *fp=NULL;
    printf("the file path is %s\n",path);
    if ((fp = fopen(path, "w+"))== NULL)
    {
      printf("file cannot open \n");
      fprintf(stderr, "打开文件错误: %s\n", strerror( errno ));
    }
    fwrite(body , 1 , body_size , fp );

    fclose(fp);
    fp=NULL;
*/

    db_add_document1(env, title, title_size, body, body_size, timestamp, timestamp_size);
    document_id = db_get_document_id1(env, title, title_size);

    /* 转换文档正文的字符编码 */
    if (!utf8toutf32(body, body_size, &body32, &body32_len))
    {
      /* 为文档创建倒排列表 */
      text_to_postings_lists(env, document_id, body32, body32_len, env->token_len, &env->ii_buffer);
      env->ii_buffer_count++;
      free(body32);
    }
    env->indexed_count++;
    print_error("count:%d title: %s", env->indexed_count, title);
  }

  /* 存储在缓冲区中的文档数量达到了指定的阈值时，更新存储器上的倒排索引 */
  if (env->ii_buffer && (env->ii_buffer_count >= env->ii_buffer_update_threshold || !title))
  {
    inverted_index_hash *p;

    print_time_diff();

    /* 更新所有词元对应的倒排项 */
    for (p = env->ii_buffer; p != NULL; p = p->hh.next)
    {
      update_postings(env, p);
    }
    free_inverted_index(env->ii_buffer);
    print_error("index flushed.");
    env->ii_buffer = NULL;
    env->ii_buffer_count = 0;

    print_time_diff();
  }
}

/**
 * 设定应用程序的运行环境
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] ii_buffer_update_threshold 清空（Flush）倒排索引缓冲区的阈值
 * @param[in] enable_phrase_search 是否启用短语检索
 * @param[in] db_path 数据库的路径
 * @return 错误代码
 * @retval 0 成功
 */
int init_env(wiser_env *env, int ii_buffer_update_threshold, int enable_phrase_search, int enable_or_query,const char *db_path)
{
  int rc;
  memset(env, 0, sizeof(wiser_env));
  rc = init_database1(env, db_path);
  if (!rc)
  {
    env->token_len = N_GRAM;
    env->ii_buffer_update_threshold = ii_buffer_update_threshold;
    env->enable_phrase_search = enable_phrase_search;
    env->enable_or_query = enable_or_query;
  }
  return rc;
}

/**
 * 释放应用程序的运行环境
 * @param[in] env 存储着应用程序运行环境的结构体
 */
void fin_env(wiser_env *env)
{
  fin_database1(env);
}

/* 判断从地址t开始的、长度为l的二进制序列是否与字符串c一致 */
#define MEMSTRCMP(t, l, c) (l == (sizeof(c) - 1) && !memcmp(t, c, l))

/**
 * 进行全文检索
 * @param[in] env 存储着应用程序运行环境的结构体
 * @param[in] method 压缩倒排列表的方法
 * @param[in] method_size 压缩方法名称的字节数
 */
void parse_compress_method(wiser_env *env, const char *method, int method_size)
{
  if (method && method_size < 0)
  {
    method_size = strlen(method);
  }
  if (!method || !method_size || MEMSTRCMP(method, method_size, "golomb"))
  {
    env->compress = compress_golomb;
  }
  else if (MEMSTRCMP(method, method_size, "none"))
  {
    env->compress = compress_none;
  }
  else
  {
    print_error("invalid compress method(%.*s). use golomb instead.",
                method_size, method);
    env->compress = compress_golomb;
  }
  switch (env->compress)
  {
  case compress_none:
    db_replace_settings1(env,
                         "compress_method", sizeof("compress_method") - 1,
                         "none", sizeof("none") - 1);
    break;
  case compress_golomb:
    db_replace_settings1(env,
                         "compress_method", sizeof("compress_method") - 1,
                         "golomb", sizeof("golomb") - 1);
    break;
  }
}
