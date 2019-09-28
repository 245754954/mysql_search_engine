#ifndef __WISER_H__
#define __WISER_H__

#include "include/utlist.h"
#include "include/uthash.h"
#include "include/utarray.h"
#include <mysql/mysql.h>
#include <openssl/md5.h>

/* bi-gram */
#define N_GRAM 2

/* 倒排列表（以文档编号和位置信息为元素的链表结构）*/
typedef struct _postings_list
{
  int document_id;             /* 文档编号 */
  UT_array *positions;         /* 位置信息的数组 */
  int positions_count;         /* 位置信息的条数 */
  struct _postings_list *next; /* 指向下一个倒排列表的指针 */
} postings_list;

/* 倒排索引（以词元编号为键，以倒排列表为值的关联数组） */
typedef struct
{
  int token_id;                 /* 词元编号（Token ID）*/
  postings_list *postings_list; /* 指向包含该词元的倒排列表的指针 */
  int docs_count;               /* 出现过该词元的文档数 */
  int positions_count;          /* 该词元在所有文档中的出现次数之和 */
  UT_hash_handle hh;            /* 用于将该结构体转化为哈希表 */
} inverted_index_hash, inverted_index_value;

/* 压缩倒排列表等数据的方法 */
typedef enum
{
  compress_none,  /* 不压缩 */
  compress_golomb /* 使用Golomb编码压缩 */
} compress_method;

/* 应用程序的全局配置 */
typedef struct _wiser_env
{


  int token_len;            /* 词元的长度。N-gram中N的取值 */
  compress_method compress; /* 压缩倒排列表等数据的方法 */
  int enable_phrase_search; /* 是否进行短语检索 */

  inverted_index_hash *ii_buffer; /* 用于更新倒排索引的缓冲区（Buffer） */
  int ii_buffer_count;            /* 用于更新倒排索引的缓冲区中的文档数 */
  int ii_buffer_update_threshold; /* 缓冲区中文档数的阈值 */
  int indexed_count;              /* 建立了索引的文档数 */
  int enable_or_query;

  int K;


  MYSQL *conn;

  MD5_CTX ctx;
  /* sqlite3的准备语句 */
  MYSQL_STMT *get_document_id_st1;
  MYSQL_STMT *get_document_title_st1;
  MYSQL_STMT *insert_document_st1;
  MYSQL_STMT *update_document_st1;
  MYSQL_STMT *get_token_id_st1;
  MYSQL_STMT *get_token_st1;
  MYSQL_STMT *store_token_st1;
  MYSQL_STMT *get_postings_st1;
  MYSQL_STMT *update_postings_st1;
  MYSQL_STMT *get_settings_st1;
  MYSQL_STMT *replace_settings_st1;
  MYSQL_STMT *get_document_count_st1;

  MYSQL_STMT *get_document_body_size_st1;
  MYSQL_STMT *get_document_visit_time_st1;

  MYSQL_STMT *get_count1;

  MYSQL_STMT *add_doc_count1;


  MYSQL_STMT *token_partial_match_st1;
  // MYSQL_STMT *begin_st1;
  MYSQL_STMT *commit_st1;
  MYSQL_STMT *rollback_st1;
} wiser_env;

/* TRUE/FALSE */
#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#define DEFAULT_II_BUFFER_UPDATE_THRESHOLD 150

#define BUFFER_SIZE_DOC_CONTENT 8192

#define DBHOST "127.0.0.1"
#define DBUSER "root"
#define DBPASS "123456"
#define DBPORT 3306
#define DBNAME "invertindex1"
#define DBSOCK NULL //"/var/lib/mysql/mysql.sock"
#define DBPCNT 0

#define DATABASE "invertindex1"

//sort order way
#define SORT "tf-idf"
//#define SORT "size"
//#define SORT "time-sort"



#define K 4




 void parse_compress_method(wiser_env *env, const char *method,int method_size);
 void fin_env(wiser_env *env);
 int init_env(wiser_env *env,int ii_buffer_update_threshold, int enable_phrase_search,int enable_or_query,const char *db_path);

 void add_document(wiser_env *env, const char *title, const char *body, const char *timestamp);





#endif /* __WISER_H__ */
