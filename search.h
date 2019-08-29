#ifndef __SEARCH_H__
#define __SEARCH_H__

#include "wiser.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
typedef struct
{
  int document_id; /* 检索出的文档编号 */
  double score;
  unsigned int body_size; /* 检索得分 */
  struct tm stamp;
  UT_hash_handle hh; /* 用于将该结构体转化为哈希表 */
} search_results;


void search(wiser_env *env, const char *query);


void search_for_browser(wiser_env *env, const char *query,char **result);
#endif /* __SEARCH_H__ */
