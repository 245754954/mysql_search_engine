#ifndef __MYSQLDATABASE_H__
#define __MYSQLDATABASE_H__
//apt-get install libmysqlclient-dev
#include <mysql/mysql.h>
#include "wiser.h"
#include <time.h>
int init_database1(wiser_env *env, const char *db_path);
void fin_database1(wiser_env *env);

int db_get_document_id1(const wiser_env *env,
                       const char *title, unsigned int title_size);
int db_get_document_title1(const wiser_env *env, int document_id,
                           const char ** title, int *title_size);
int db_add_document1(const wiser_env *env,
                    const char *title, unsigned int title_size,
                    const char *body, unsigned int body_size,const char *timestamp,int timestamp_size);
int db_get_token_id1(const wiser_env *env,
                    const char *str, unsigned int str_size, int insert,
                    int *docs_count);
int db_get_token1(const wiser_env *env,
                 const int token_id,
                  char * token, int *token_size);
int db_get_postings1(const wiser_env *env, int token_id,
                    int *docs_count, void **postings, int *postings_size);
int db_update_postings1(const wiser_env *env, int token_id,
                       int docs_count,
                       void *postings, int postings_size);
int db_get_settings1(const wiser_env *env, const char *key,
                    int key_size,
                    char **value, int *value_size);
int db_replace_settings1(const wiser_env *env, const char *key,
                        int key_size,
                        const char *value, int value_size);
int db_get_document_count1(const wiser_env *env);
int begin1(const wiser_env *env);
int commit1(const wiser_env *env);
int rollback1(const wiser_env *env);

int db_get_document_size1(const wiser_env *env,int document_id,unsigned int *document_size);


int db_get_document_visit_time(const wiser_env *env,int document_id,struct tm *tm);

int db_get_count1(const wiser_env *env);


int add_doc_count1(const wiser_env *env,int val);

#endif