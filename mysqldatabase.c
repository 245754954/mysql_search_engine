#include "mysqldatabase.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "wiser.h"

int init_database1(wiser_env *env, const char *db)
{
    mysql_library_init(0, NULL, NULL);
    env->conn = mysql_init(NULL);
    // unsigned int timeout = 3000;
    // mysql_options(env->conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);

    if (!env->conn)
    {
        fprintf(stderr, "mysql_init failed\n");
        return EXIT_FAILURE;
    }

    if (strcmp("none", db) == 0)
    {
        if (!mysql_real_connect(env->conn, DBHOST, DBUSER, DBPASS, NULL, DBPORT, DBSOCK, DBPCNT))
        {
            printf("connect failed: %s\n", mysql_error(env->conn));
            mysql_close(env->conn);
            mysql_library_end();
            return 0;
        }

        if (env->conn)
            printf("Connection success\n");
        else
            printf("Connection failed\n");

        const char *csname = "utf8mb4";
        mysql_set_character_set(env->conn, csname);

        char drop_database[100] = {'\0'};
        sprintf(drop_database, "drop database if exists %s;", DATABASE);
        if (mysql_query(env->conn, drop_database))
        {
            fprintf(stderr, "41 *_prepare: %s\n", mysql_error(env->conn));
        }

        char create_database[100] = {'\0'};
        sprintf(create_database, "create database %s  CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;", DATABASE);
        //printf("%s\n", create_database);
        if (mysql_query(env->conn, create_database))
        {
            fprintf(stderr, "49 *_prepare: %s\n", mysql_error(env->conn));
        }

        char use_database[100] = {'\0'};
        sprintf(use_database, "use %s;", DATABASE);
        if (mysql_query(env->conn, use_database))
        {
            fprintf(stderr, "56 *_prepare: %s\n", mysql_error(env->conn));
        }

        if (mysql_select_db(env->conn, DATABASE))
        {
            fprintf(stderr, "41 *_prepare: %s\n", mysql_error(env->conn));
            return -1;
        }

        mysql_query(env->conn, "DROP TABLE IF EXISTS `settings`;");
        mysql_query(env->conn, "DROP TABLE IF EXISTS `tokens`;");
        mysql_query(env->conn, "DROP TABLE IF EXISTS `documents`;");

        if (mysql_query(env->conn, "CREATE TABLE documents ("
                                   "id int(11) NOT NULL AUTO_INCREMENT,"
                                   "title varchar(512),"
                                   "body longtext,"
                                   "tstamp datetime,"
                                   "PRIMARY KEY (id)) ENGINE=InnoDb  DEFAULT CHARSET=utf8mb4;"))
        {

            fprintf(stderr, "60 char *_prepare: %s\n", mysql_error(env->conn));
            return -1;
        };

        if (mysql_query(env->conn,
                        "CREATE TABLE tokens ("
                        "id int(11) NOT NULL AUTO_INCREMENT,"
                        "token varchar(512) ,"
                        "docs_count int(255) NOT NULL,"
                        "postings longblob,"
                        "PRIMARY KEY (id)"
                        ") ENGINE=InnoDb  DEFAULT CHARSET=utf8mb4;"))
        {

            fprintf(stderr, "74 char *_prepare: %s\n", mysql_error(env->conn));
            return -1;
        };

        if (mysql_query(env->conn,
                        "CREATE TABLE settings ("
                        "key1 varchar(255),"
                        "value1 varchar(255) ,"
                        "PRIMARY KEY (key1)"
                        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;"))
        {

            fprintf(stderr, "86 char *_prepare: %s\n", mysql_error(env->conn));
            return -1;
        };

        if (mysql_query(env->conn,
                        "CREATE TABLE doccount("
                        "id int(11) NOT NULL AUTO_INCREMENT,"
                        "doc int(11) ,"
                        "PRIMARY KEY (id)"
                        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;"))
        {

            fprintf(stderr, "86 char *_prepare: %s\n", mysql_error(env->conn));
            return -1;
        };

        if (mysql_query(env->conn,
                        "CREATE UNIQUE INDEX token_index ON tokens(token);"))
        {

            fprintf(stderr, "94 char *_prepare: %s\n", mysql_error(env->conn));
            return -1;
        };

        if (mysql_query(env->conn,
                        "CREATE UNIQUE INDEX title_index ON documents(title);"))
        {

            fprintf(stderr, "102 char *_prepare: %s\n", mysql_error(env->conn));
            return -1;
        };
    }
    else
    {
        if (!mysql_real_connect(env->conn, DBHOST, DBUSER, DBPASS, db, DBPORT, DBSOCK, DBPCNT))
        {
            printf("connect failed: %s\n", mysql_error(env->conn));
            mysql_close(env->conn);
            mysql_library_end();
            return 0;
        }

        if (env->conn)
            printf("Connection success\n");
        else
            printf("Connection failed\n");
        const char *csname = "utf8mb4";
        mysql_set_character_set(env->conn, csname);
    }

    env->get_document_id_st1 = mysql_stmt_init(env->conn);
    env->get_document_title_st1 = mysql_stmt_init(env->conn);
    env->insert_document_st1 = mysql_stmt_init(env->conn);
    env->update_document_st1 = mysql_stmt_init(env->conn);
    env->get_token_id_st1 = mysql_stmt_init(env->conn);
    env->get_token_st1 = mysql_stmt_init(env->conn);
    env->store_token_st1 = mysql_stmt_init(env->conn);
    env->get_postings_st1 = mysql_stmt_init(env->conn);
    env->update_postings_st1 = mysql_stmt_init(env->conn);
    env->get_settings_st1 = mysql_stmt_init(env->conn);
    env->replace_settings_st1 = mysql_stmt_init(env->conn);
    env->get_document_count_st1 = mysql_stmt_init(env->conn);
    // env->begin_st1 = mysql_stmt_init(env->conn);
    env->commit_st1 = mysql_stmt_init(env->conn);
    env->rollback_st1 = mysql_stmt_init(env->conn);

    env->get_document_body_size_st1 = mysql_stmt_init(env->conn);

    env->get_document_visit_time_st1 = mysql_stmt_init(env->conn);

    env->get_count1 = mysql_stmt_init(env->conn);
    env->add_doc_count1 = mysql_stmt_init(env->conn);

    env->token_partial_match_st1 = mysql_stmt_init(env->conn);

    char *get_document_id_st_str = "SELECT id FROM documents WHERE title = ?;";
    char *get_document_title_st_str = "SELECT title FROM documents WHERE id = ?;";
    char *insert_document_st_str = "INSERT INTO documents (title, body,tstamp) VALUES (?, ?,?)";
    char *update_document_st_str = "UPDATE documents set body = ? WHERE id = ?;";
    char *get_token_id_st_str = "SELECT id, docs_count FROM tokens WHERE token = ?;";
    char *get_token_st_str = "SELECT token FROM tokens WHERE id = ?;";
    char *store_token_st_str = "INSERT  IGNORE INTO tokens (token, docs_count, postings) VALUES (?, ?, ?);";
    char *get_postings_st_str = "SELECT docs_count, postings FROM tokens WHERE id = ?;";
    char *update_postings_st_str = "UPDATE tokens SET docs_count = ?, postings = ? WHERE id = ?;";
    char *get_settings_st_str = "SELECT value1 FROM settings WHERE key1 = ?;";
    char *replace_settings_st_str = " REPLACE INTO settings (key1, value1) VALUES (?, ?);";
    char *get_document_count_st_str = "SELECT COUNT(*) FROM documents;";
    //char *begin_st_str = "BEGIN;";not supported now
    char *commit_st_str = "COMMIT;";
    char *rollback_st_str = "ROLLBACK;";
    char *get_document_body_size_st_str = "SELECT LENGTH(body) from documents where id =?;";

    char *get_document_visit_time_str = "SELECT tstamp from documents where id =?;";

    char *get_count_st_str = "select count(*) from doccount;";

    char *add_doc_count_str = "INSERT INTO doccount(doc) VALUES(?);";

    char *token_partial_match_str = "SELECT token FROM tokens WHERE token like ?|| '%';";

    if (mysql_stmt_prepare(env->get_document_id_st1,
                           get_document_id_st_str,
                           strlen(get_document_id_st_str)))
    {

        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(env->get_document_title_st1,
                           get_document_title_st_str,
                           strlen(get_document_title_st_str)))
    {

        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(env->insert_document_st1,
                           insert_document_st_str,
                           strlen(insert_document_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(env->update_document_st1,
                           update_document_st_str,
                           strlen(update_document_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(env->get_token_id_st1,
                           get_token_id_st_str,
                           strlen(get_token_id_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(env->get_token_st1,
                           get_token_st_str,
                           strlen(get_token_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(
            env->store_token_st1,
            store_token_st_str,
            strlen(store_token_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(

            env->get_postings_st1,
            get_postings_st_str,
            strlen(get_postings_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(
            env->update_postings_st1,
            update_postings_st_str,
            strlen(update_postings_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(
            env->get_settings_st1,
            get_settings_st_str,
            strlen(get_settings_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(
            env->replace_settings_st1,
            replace_settings_st_str,
            strlen(replace_settings_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(
            env->get_document_count_st1,
            get_document_count_st_str,
            strlen(get_document_count_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    // if (mysql_stmt_prepare(
    //         env->begin_st1,
    //         begin_st_str,
    //         strlen(begin_st_str)))
    // {
    //     fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
    //     return -1;
    // };

    if (mysql_stmt_prepare(env->commit_st1, commit_st_str, strlen(commit_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    }

    if (mysql_stmt_prepare(
            env->rollback_st1,
            rollback_st_str,
            strlen(rollback_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(
            env->get_document_body_size_st1,
            get_document_body_size_st_str,
            strlen(get_document_body_size_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    };

    if (mysql_stmt_prepare(env->get_document_visit_time_st1,
                           get_document_visit_time_str,
                           strlen(get_document_visit_time_str)))
    {

        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    }

    if (mysql_stmt_prepare(env->get_count1,
                           get_count_st_str,
                           strlen(get_count_st_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    }

    if (mysql_stmt_prepare(env->add_doc_count1,
                           add_doc_count_str,
                           strlen(add_doc_count_str)))
    {
        fprintf(stderr, "char *_prepare: %s\n", mysql_error(env->conn));
        return -1;
    }

    if (mysql_stmt_prepare(env->token_partial_match_st1,
                           token_partial_match_str,
                           strlen(token_partial_match_str)))
    {
        fprintf(stderr, "line %d preapre error %s \n", __LINE__, mysql_error(env->conn));
        exit(1);
    }

    mysql_library_end();
    return EXIT_SUCCESS;
}

void fin_database1(wiser_env *env)
{
    mysql_stmt_close(env->get_document_id_st1);
    mysql_stmt_close(env->get_document_title_st1);
    mysql_stmt_close(env->insert_document_st1);
    mysql_stmt_close(env->update_document_st1);
    mysql_stmt_close(env->get_token_id_st1);
    mysql_stmt_close(env->get_token_st1);
    mysql_stmt_close(env->store_token_st1);
    mysql_stmt_close(env->get_postings_st1);
    mysql_stmt_close(env->update_postings_st1);
    mysql_stmt_close(env->get_document_count_st1);
    mysql_stmt_close(env->get_settings_st1);
    mysql_stmt_close(env->replace_settings_st1);
    mysql_stmt_close(env->commit_st1);
    mysql_stmt_close(env->rollback_st1);
    mysql_stmt_close(env->get_document_body_size_st1);
    mysql_stmt_close(env->get_document_visit_time_st1);
    mysql_stmt_close(env->get_count1);
    mysql_stmt_close(env->token_partial_match_st1);
    mysql_stmt_close(env->add_doc_count1);
    //mysql_stmt_close(env->begin_st1);
    mysql_close(env->conn);
}

int db_get_document_id1(const wiser_env *env, const char *title, unsigned int title_size)
{

    my_bool is_null[1];
    my_bool error[1];
    int document_id;
    unsigned long length[1];
    mysql_stmt_reset(env->get_document_id_st1);

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));
    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (void *)title;
    params[0].buffer_length = title_size;
    mysql_stmt_bind_param(env->get_document_id_st1, params);
    ////执行与语句句柄相关的预处理
    /* Execute the SELECT query */
    if (mysql_stmt_execute(env->get_document_id_st1))
    {
        fprintf(stderr, " mysql_stmt_execute(), failed\n");
        fprintf(stderr, " %s   %d   %s\n", mysql_stmt_error(env->get_document_id_st1), mysql_stmt_errno(env->get_document_id_st1), mysql_stmt_sqlstate(env->get_document_id_st1));
        exit(0);
    }

    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));
    /* INTEGER COLUMN */
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = (char *)&document_id;
    bind[0].is_null = &is_null[0];
    bind[0].length = &length[0];
    bind[0].error = &error[0];

    if (mysql_stmt_bind_result(env->get_document_id_st1, bind))
    {
        fprintf(stderr, " mysql_stmt_bind_result() failed\n");
        fprintf(stderr, " %s   %d   %s\n", mysql_stmt_error(env->get_document_id_st1), mysql_stmt_errno(env->get_document_id_st1), mysql_stmt_sqlstate(env->get_document_id_st1));
        exit(0);
    }

    if (mysql_stmt_fetch(env->get_document_id_st1))
    {
        mysql_stmt_free_result(env->get_document_id_st1);
        return 0;
    }
    else
    {
        mysql_stmt_free_result(env->get_document_id_st1);
        return document_id;
    }
}

int db_get_document_title1(const wiser_env *env, int document_id,
                           const char **title, int *title_size)
{

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));
    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = (void *)&document_id;

    mysql_stmt_reset(env->get_document_title_st1);
    mysql_stmt_bind_param(env->get_document_title_st1, params);

    if (mysql_stmt_execute(env->get_document_title_st1))
    {
        fprintf(stderr, " mysql_stmt_execute(), failed\n");
        fprintf(stderr, " %s   %d   %s\n", mysql_stmt_error(env->get_document_title_st1), mysql_stmt_errno(env->get_document_title_st1), mysql_stmt_sqlstate(env->get_document_title_st1));
        exit(0);
    }

    char title1[BUFFER_SIZE_DOC_CONTENT];
    unsigned long length[1];
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));
    /* INTEGER COLUMN */
    bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[0].buffer = (char *)title1;
    bind[0].buffer_length = BUFFER_SIZE_DOC_CONTENT;
    bind[0].length = &length[0];
    bind[0].is_null = 0;

    if (mysql_stmt_bind_result(env->get_document_title_st1, bind))
    {
        fprintf(stderr, " mysql_stmt_bind_result() failed\n");
        fprintf(stderr, " %s   %d   %s\n", mysql_stmt_error(env->get_document_id_st1), mysql_stmt_errno(env->get_document_id_st1), mysql_stmt_sqlstate(env->get_document_id_st1));
        exit(0);
    }

    if (mysql_stmt_fetch(env->get_document_title_st1))
    {
        fprintf(stderr, " %s   %d   %s\n", mysql_stmt_error(env->get_document_title_st1), mysql_stmt_errno(env->get_document_title_st1), mysql_stmt_sqlstate(env->get_document_title_st1));
        mysql_stmt_free_result(env->get_document_title_st1);
        return 0;
    }
    else
    {

        if (title)
        {
            *title = title1;
        }
        if (title_size)
        {
            *title_size = length[0];
        }
        mysql_stmt_free_result(env->get_document_title_st1);
        return 0;
    }
}
//  char *insert_document_st_str = "INSERT INTO documents (title, body,timestamp) VALUES (?, ?,?);";
//     char *update_document_st_str = "UPDATE documents set body = ? WHERE id = ?;";
int db_add_document1(const wiser_env *env,
                     const char *title, unsigned int title_size,
                     const char *body, unsigned int body_size, const char *timestamp, int timestamp_size)
{
    printf("the time stamp is %s\n", timestamp);

    int rc, document_id;

    if ((document_id = db_get_document_id1(env, title, title_size)))
    {
        mysql_stmt_reset(env->update_document_st1);
        MYSQL_BIND param[2];
        memset(param, 0, sizeof(param));

        param[0].buffer_type = MYSQL_TYPE_BLOB;
        param[0].buffer = (void *)body;
        param[0].buffer_length = body_size;

        param[1].buffer_type = MYSQL_TYPE_LONG;
        param[1].buffer = &document_id;

        mysql_stmt_bind_param(env->update_document_st1, param);
        if (rc = mysql_stmt_execute(env->update_document_st1))
        {
            fprintf(stderr, " mysql_stmt_execute(), failed\n");
            fprintf(stderr, " %s   %d   %s\n", mysql_stmt_error(env->update_document_st1), mysql_stmt_errno(env->update_document_st1), mysql_stmt_sqlstate(env->update_document_st1));
            mysql_stmt_free_result(env->update_document_st1);
            return rc;
        }
    }
    else
    {

        MYSQL_BIND param[3];
        memset(param, 0, sizeof(param));

        param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
        param[0].buffer = (void *)title;
        param[0].buffer_length = title_size;

        param[1].buffer_type = MYSQL_TYPE_BLOB;
        param[1].buffer = (void *)body;
        param[1].buffer_length = body_size;

        // struct tm p;
        // memset(&p, 0, sizeof(struct tm));
        int year = 0;
        int mon = 0;
        int mday = 0;
        int hour = 0;
        int min = 0;
        int sec = 0;

        sscanf(timestamp, "%d-%d-%dT%d:%d:%dZ", &year, &mon, &mday, &hour, &min, &sec);
        MYSQL_TIME tm;
        memset(&tm, 0, sizeof(MYSQL_TIME));

        tm.year = (year);
        tm.month = (mon);
        tm.day = mday;
        tm.hour = hour;
        tm.minute = min;
        tm.second = sec;
        tm.second_part = 0;
        tm.neg = 0;

        // char buffer[80];
        // strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",&p);
        // printf("the value of bufer %s\n",buffer);

        param[2].buffer_type = MYSQL_TYPE_DATETIME;
        param[2].buffer = (char *)&tm;
        param[2].is_null = 0;
        param[2].length = 0;

        mysql_stmt_reset(env->insert_document_st1);
        mysql_stmt_bind_param(env->insert_document_st1, param);

        if (rc = mysql_stmt_execute(env->insert_document_st1))
        {
            fprintf(stderr, " mysql_stmt_execute(), failed\n");
            fprintf(stderr, " %s   %d   %s\n", mysql_stmt_error(env->insert_document_st1), mysql_stmt_errno(env->insert_document_st1), mysql_stmt_sqlstate(env->insert_document_st1));
            mysql_stmt_free_result(env->insert_document_st1);
            return rc;
        }
    }
    mysql_stmt_free_result(env->insert_document_st1);
    return rc;
}

//
int db_get_token_id1(const wiser_env *env,
                     const char *str, unsigned int str_size, int insert,
                     int *docs_count)
{
    int rc;
    if (insert)
    {
        mysql_stmt_reset(env->store_token_st1);
        //INSERT OR IGNORE INTO tokens (token, docs_count, postings) VALUES (?, ?, ?);
        MYSQL_BIND param[3];

        memset(param, 0, sizeof(param));
        param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
        param[0].buffer = (void *)str;
        param[0].buffer_length = str_size;

        int doc_count = 0;
        param[1].buffer_type = MYSQL_TYPE_LONG;
        param[1].buffer = (char *)&doc_count;
        param[1].buffer_length = sizeof(doc_count);

        param[2].buffer_type = MYSQL_TYPE_BLOB;

        if (mysql_stmt_bind_param(env->store_token_st1, param))
        {
            fprintf(stderr, "\n param bind failed");
            fprintf(stderr, "\n %s", mysql_stmt_error(env->store_token_st1));
            exit(0);
        }

        if (mysql_stmt_send_long_data(env->store_token_st1, 2, "", 0))
        {
            fprintf(stderr, "\n send_long_data failed");
            fprintf(stderr, "\n %s\n", mysql_stmt_error(env->store_token_st1));
            exit(0);
        }

        if (rc = mysql_stmt_execute(env->store_token_st1))
        {
            fprintf(stderr, " mysql_stmt_execute(), failed\n");
            fprintf(stderr, " %s   %d   %s\n", mysql_stmt_error(env->store_token_st1), mysql_stmt_errno(env->store_token_st1), mysql_stmt_sqlstate(env->store_token_st1));
            mysql_stmt_free_result(env->store_token_st1);
            return rc;
        }
    }
    //SELECT id, docs_count FROM tokens WHERE token = ?
    mysql_stmt_reset(env->get_token_id_st1);
    MYSQL_BIND param1[1];
    memset(param1, 0, sizeof(param1));

    param1[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param1[0].buffer = (void *)str;
    param1[0].buffer_length = str_size;

    if (mysql_stmt_bind_param(env->get_token_id_st1, param1))
    {
        fprintf(stderr, "\n param bind failed");
        fprintf(stderr, "\n %s", mysql_stmt_error(env->get_token_id_st1));
        exit(0);
    }

    if (mysql_stmt_execute(env->get_token_id_st1))
    {

        fprintf(stderr, "\n mysql_stmt_execute() failed");
        fprintf(stderr, "\n %s", mysql_stmt_error(env->get_token_id_st1));
        mysql_stmt_free_result(env->get_token_id_st1);
        exit(0);
    }

    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));
    int id;
    int doc_count;
    /* INTEGER COLUMN */
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = (char *)&id;

    bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[1].buffer = (char *)&doc_count;
    if (mysql_stmt_bind_result(env->get_token_id_st1, bind))
    {
        fprintf(stderr, "\n mysql_stmt_execute() failed");
        fprintf(stderr, "\n %s", mysql_stmt_error(env->get_token_id_st1));
        exit(0);
    }

    if (mysql_stmt_fetch(env->get_token_id_st1))
    {
        // fprintf(stderr, " %s   %d   %s\n", mysql_stmt_error(env->get_token_id_st1), mysql_stmt_errno(env->get_token_id_st1), mysql_stmt_sqlstate(env->get_token_id_st1));

        if (docs_count)
        {
            *docs_count = 0;
        }
        mysql_stmt_free_result(env->get_token_id_st1);
        return 0;
    }
    else
    {

        if (docs_count)
        {
            *docs_count = doc_count;
        }
        mysql_stmt_free_result(env->get_token_id_st1);
        return id;
    }
}

int db_get_token1(const wiser_env *env,
                  const int token_id,
                  char *token, int *token_size)
{

    //SELECT token FROM tokens WHERE id = ?;
    mysql_stmt_reset(env->get_token_st1);
    MYSQL_BIND param[1];
    memset(param, 0, sizeof(param));
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = (void *)&token_id;

    if (mysql_stmt_bind_param(env->get_token_st1, param))
    {

        fprintf(stderr, "\n %s", mysql_stmt_error(env->get_token_st1));
        exit(1);
    }
    if (mysql_stmt_execute(env->get_token_st1))
    {
        fprintf(stderr, "\n mysql_stmt_execute() failed");
        fprintf(stderr, "\n %s", mysql_stmt_error(env->get_token_st1));
        exit(0);
    }

    char token_str[BUFFER_SIZE_DOC_CONTENT];
    unsigned long length[1];
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));
    /* INTEGER COLUMN */
    bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[0].buffer = (char *)token_str;
    bind[0].buffer_length = BUFFER_SIZE_DOC_CONTENT;
    bind[0].length = &length[0];
    bind[0].is_null = 0;

    if (mysql_stmt_bind_result(env->get_token_st1, bind))
    {
        fprintf(stderr, "\n mysql_stmt_bind_result() failed");
        fprintf(stderr, "\n %s", mysql_stmt_error(env->get_token_st1));
        exit(0);
    }
    if (mysql_stmt_fetch(env->get_token_st1))
    {
        fprintf(stderr, "\n mysql_stmt_fetch() failed");
        fprintf(stderr, "\n %s", mysql_stmt_error(env->get_token_st1));
        mysql_stmt_free_result(env->get_token_st1);
        return 0;
    }
    else
    {
        if (token)
        {
            strcpy(token, token_str);
        }
        if (token_size)
        {
            *token_size = length[0];
        }
        mysql_stmt_free_result(env->get_token_st1);
        return 0;
    }
}

int db_get_postings1(const wiser_env *env, int token_id,
                     int *docs_count, void **postings, int *postings_size)
{
    int rc;
    //SELECT docs_count, postings FROM tokens WHERE id = ?;
    mysql_stmt_reset(env->get_postings_st1);

    MYSQL_BIND param[1];
    memset(param, 0, sizeof(param));
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = (void *)&token_id;
    if (mysql_stmt_bind_param(env->get_postings_st1, param))
    {

        printf("\nmysql_stmt_bind_param() failed %s\n", mysql_stmt_error(env->get_postings_st1));
        exit(1);
    }

    if (mysql_stmt_execute(env->get_postings_st1))
    {

        printf("\nmysql_stmt_bind_param() failed %s\n", mysql_stmt_error(env->get_postings_st1));
        exit(1);
    }

    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    unsigned long length[1];
    int doc_count;
    char *buf = (char *)malloc(sizeof(char) * BUFFER_SIZE_DOC_CONTENT * 512);
    memset(buf, 0, sizeof(buf));

    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = (char *)&doc_count;

    bind[1].buffer_type = MYSQL_TYPE_LONG_BLOB;
    bind[1].buffer = buf;
    bind[1].buffer_length = BUFFER_SIZE_DOC_CONTENT * 512;
    bind[1].length = &length[0];

    if (mysql_stmt_bind_result(env->get_postings_st1, bind))
    {

        printf("\nmysql_stmt_bind_param() failed %s\n", mysql_stmt_error(env->get_postings_st1));
        exit(1);
    }

    if (mysql_stmt_fetch(env->get_postings_st1))
    {
        //  printf("\nmysql_stmt_fetch() failed %s\n",mysql_stmt_error(env->get_postings_st1));
        if (docs_count)
        {
            *docs_count = 0;
        }
        if (postings)
        {
            *postings = NULL;
        }
        if (postings_size)
        {
            *postings_size = 0;
        }

        rc = 0;
    }
    else
    {
        if (postings_size)
        {
            *postings_size = length[0];
        }

        if (postings)
        {
            *postings = buf;
        }

        if (docs_count)
        {
            *docs_count = doc_count;
        }

        rc = 0;
    }

    mysql_stmt_free_result(env->get_postings_st1);
    return rc;
}

//UPDATE tokens SET docs_count = ?, postings = ? WHERE id = ?;
int db_update_postings1(const wiser_env *env, int token_id, int docs_count,
                        void *postings, int postings_size)
{
    int rc;
    mysql_stmt_reset(env->update_postings_st1);

    MYSQL_BIND param[3];
    memset(param, 0, sizeof(param));
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = (void *)&docs_count;

    param[1].buffer_type = MYSQL_TYPE_LONG_BLOB;

    param[2].buffer_type = MYSQL_TYPE_LONG;
    param[2].buffer = (void *)&token_id;

    if (mysql_stmt_bind_param(env->update_postings_st1, param))
    {

        printf("\nmysql_stmt_bind_param() failed %s\n", mysql_stmt_error(env->update_postings_st1));
        exit(1);
    }

    if (mysql_stmt_send_long_data(env->update_postings_st1, 1, postings, postings_size))
    {

        printf("\nmysql_stmt_send_long_data() failed %s\n", mysql_stmt_error(env->update_postings_st1));
        exit(1);
    }
    if (mysql_stmt_execute(env->update_postings_st1))
    {
        printf("\nmysql_stmt_execute() failed %s\n", mysql_stmt_error(env->update_postings_st1));
    }

    rc = 0;
    mysql_stmt_free_result(env->update_postings_st1);
    return rc;
}

//SELECT value1 FROM settings WHERE key1 = ?;
int db_get_settings1(const wiser_env *env, const char *key, int key_size,
                     char **value, int *value_size)
{
    int rc;

    mysql_stmt_reset(env->get_settings_st1);
    MYSQL_BIND param[1];
    memset(param, 0, sizeof(param));
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = (void *)key;
    param[0].buffer_length = key_size;

    if (mysql_stmt_bind_param(env->get_settings_st1, param))
    {
        printf("\nmysql_stmt_bind_param() failed %s\n", mysql_stmt_error(env->get_settings_st1));
        exit(1);
    }

    if (mysql_stmt_execute(env->get_settings_st1))
    {
        printf("\nmysql_stmt_execute() failed %s\n", mysql_stmt_error(env->get_settings_st1));
        exit(1);
    }

    MYSQL_BIND bind[1];
    char buf[BUFFER_SIZE_DOC_CONTENT];
    unsigned long length;
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[0].buffer = buf;
    bind[0].buffer_length = BUFFER_SIZE_DOC_CONTENT;
    bind[0].length = &length;

    if (mysql_stmt_bind_result(env->get_settings_st1, bind))
    {
        printf("\nmysql_stmt_bind_result() failed %s\n", mysql_stmt_error(env->get_settings_st1));
        exit(1);
    }
    if (mysql_stmt_fetch(env->get_settings_st1))
    {
        mysql_stmt_free_result(env->get_settings_st1);
        return 0;
    }
    else
    {
        *value = buf;

        *value_size = length;
        mysql_stmt_free_result(env->get_settings_st1);
        return 0;
    }
}

//INSERT OR REPLACE INTO settings (key1, value1) VALUES (?, ?);
int db_replace_settings1(const wiser_env *env, const char *key,
                         int key_size,
                         const char *value, int value_size)
{
    int rc;

    mysql_stmt_reset(env->replace_settings_st1);

    MYSQL_BIND param[2];
    memset(param, 0, sizeof(param));
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = (void *)key;
    param[0].buffer_length = key_size;

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = (void *)value;
    param[1].buffer_length = value_size;

    if (mysql_stmt_bind_param(env->replace_settings_st1, param))
    {
        printf("\nmysql_stmt_bind_param() failed %s\n", mysql_stmt_error(env->replace_settings_st1));
        exit(1);
    }

    if (rc = mysql_stmt_execute(env->replace_settings_st1))
    {
        printf("\nmysql_stmt_execute() failed %s\n", mysql_stmt_error(env->replace_settings_st1));
        mysql_stmt_free_result(env->replace_settings_st1);
        return rc;
    }
    else
    {
        mysql_stmt_free_result(env->replace_settings_st1);
        return rc;
    }
}

int db_get_document_count1(const wiser_env *env)
{

    //SELECT COUNT(*) FROM documents;
    mysql_stmt_reset(env->get_document_count_st1);
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));

    int count;
    unsigned long length[1];
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = (char *)&count;
    bind[0].length = &length[0];

    if (mysql_stmt_execute(env->get_document_count_st1))
    {
        printf("\nmysql_stmt_execute() failed %s\n", mysql_stmt_error(env->get_document_count_st1));
        exit(1);
    }

    if (mysql_stmt_bind_result(env->get_document_count_st1, bind))
    {
        printf("\nmysql_stmt_bind_result() failed %s\n", mysql_stmt_error(env->get_document_count_st1));
        exit(1);
    }

    if (mysql_stmt_fetch(env->get_document_count_st1))
    {
        mysql_stmt_free_result(env->get_document_count_st1);
        return -1;
    }
    else
    {
        mysql_stmt_free_result(env->get_document_count_st1);
        return count;
    }
}

/**
 * 开启事务
 * @param[in] env 存储着应用程序运行环境的结构体
 */
int begin1(const wiser_env *env)
{

    return mysql_query(env->conn, "begin;");
}

/**
 * 提交事务
 * @param[in] env 存储着应用程序运行环境的结构体
 */
int commit1(const wiser_env *env)
{

    return mysql_stmt_execute(env->commit_st1);
}

/**
 * 回滚事务
 * @param[in] env 存储着应用程序运行环境的结构体
 */
int rollback1(const wiser_env *env)
{
    return mysql_stmt_execute(env->rollback_st1);
}

int db_get_document_size1(const wiser_env *env, int document_id, unsigned int *document_size)
{

    mysql_stmt_reset(env->get_document_body_size_st1);

    MYSQL_BIND param[1];
    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = (char *)&document_id;
    param[0].buffer_length = sizeof(document_id);

    if (mysql_stmt_bind_param(env->get_document_body_size_st1, param))
    {
        printf("\nmysql_stmt_bind_param() failed %s\n", mysql_stmt_error(env->get_document_body_size_st1));
        exit(1);
    }

    if (mysql_stmt_execute(env->get_document_body_size_st1))
    {
        printf("\nmysql_stmt_execute() failed %s\n", mysql_stmt_error(env->get_document_body_size_st1));
        exit(1);
    }

    int doc_size;
    unsigned long len;
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = (char *)&doc_size;
    bind[0].length = &len;

    if (mysql_stmt_bind_result(env->get_document_body_size_st1, bind))
    {
        printf("\nmysql_stmt_bind_result() failed %s\n", mysql_stmt_error(env->get_document_body_size_st1));
        exit(1);
    }

    if (mysql_stmt_fetch(env->get_document_body_size_st1))
    {
        printf("\nmysql_stmt_fetch() failed %s\n", mysql_stmt_error(env->get_document_body_size_st1));
        exit(1);
    }
    else
    {
        *document_size = (unsigned int)doc_size;
    }

    mysql_stmt_free_result(env->get_document_body_size_st1);

    return 0;
}

int db_get_document_visit_time(const wiser_env *env, int document_id, struct tm *tm)
{

    mysql_stmt_reset(env->get_document_visit_time_st1);

    MYSQL_BIND param[1];

    memset(param, 0, sizeof(param));
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = (char *)&document_id;

    if (mysql_stmt_bind_param(env->get_document_visit_time_st1, param))
    {
        printf("bind param error %s\n", mysql_stmt_error(env->get_document_visit_time_st1));
        exit(1);
    }

    if (mysql_stmt_execute(env->get_document_visit_time_st1))
    {
        mysql_stmt_free_result(env->get_document_visit_time_st1);
        printf("execute error %s\n", mysql_stmt_error(env->get_document_visit_time_st1));
        exit(1);
    }
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));

    MYSQL_TIME mytime;
    memset(&mytime, 0, sizeof(MYSQL_TIME));
    bind[0].buffer_type = MYSQL_TYPE_DATETIME;
    bind[0].buffer = (char *)&mytime;

    if (mysql_stmt_bind_result(env->get_document_visit_time_st1, bind))
    {
        mysql_stmt_free_result(env->get_document_visit_time_st1);
        printf("bind param error %s\n", mysql_stmt_error(env->get_document_visit_time_st1));
        exit(1);
    }

    if (mysql_stmt_fetch(env->get_document_visit_time_st1))
    {
        mysql_stmt_free_result(env->get_document_visit_time_st1);
        printf("mysql_stmt_fetch %s\n", mysql_stmt_error(env->get_document_visit_time_st1));
        exit(1);
    }
    else
    {

        struct tm p;
        p.tm_year = mytime.year - 1900;
        p.tm_mon = mytime.month - 1;
        p.tm_mday = mytime.day;
        p.tm_hour = mytime.hour;
        p.tm_min = mytime.minute;
        p.tm_sec = mytime.second;
        *tm = p;
        // time_t t1 = mktime(&p);
    }
    mysql_stmt_free_result(env->get_document_visit_time_st1);
    return 0;
}

int db_get_count1(const wiser_env *env)
{
    int count;
    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));
    result[0].buffer_type = MYSQL_TYPE_LONG;
    result[0].buffer = (char *)&count;
    if (mysql_stmt_reset(env->get_count1))
    {
        printf("reset error %s\n", mysql_stmt_error(env->get_count1));
        return -1;
    }

    if (mysql_stmt_execute(env->get_count1))
    {
        printf("execute error %s\n", mysql_stmt_error(env->get_count1));
        exit(1);
    }

    if (mysql_stmt_bind_result(env->get_count1, result))
    {

        printf("bind result error %s \n", mysql_stmt_error(env->get_count1));
    }

    if (mysql_stmt_fetch(env->get_count1))
    {
        mysql_stmt_free_result(env->get_count1);
        printf("fetch error %s\n", mysql_stmt_error(env->get_count1));
        return 0;
    }
    else
    {
        mysql_stmt_free_result(env->get_count1);
        return count;
    }
}

int add_doc_count1(const wiser_env *env, int val)
{
    MYSQL_BIND param[1];
    memset(param, 0, sizeof(param));
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = (char *)&val;

    if (mysql_stmt_reset(env->add_doc_count1))
    {
        printf("reset error %s\n", mysql_stmt_error(env->add_doc_count1));
        exit(1);
    }

    if (mysql_stmt_bind_param(env->add_doc_count1, param))
    {
        printf("bind param error %s\n", mysql_stmt_error(env->add_doc_count1));
        exit(1);
    }

    if (mysql_stmt_execute(env->add_doc_count1))
    {
        printf("execute error %s\n", mysql_stmt_error(env->add_doc_count1));
        exit(1);
    }
    mysql_stmt_free_result(env->add_doc_count1);

    return 0;
}

int token_partial_match1(const wiser_env *env, const char *query, int query_len, UT_array *token_ids)
{

    if (mysql_stmt_reset(env->token_partial_match_st1))
    {
        printf("reset error %d error %s\n", __LINE__, mysql_stmt_error(env->token_partial_match_st1));
        exit(1);
    }

    MYSQL_BIND param[1];
    memset(param, 0, sizeof(param));
    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = (char*)query;
    param[0].buffer_length = query_len;

    if (mysql_stmt_bind_param(env->token_partial_match_st1, param))
    {
        printf("line %d error %s\n", __LINE__, mysql_stmt_error(env->token_partial_match_st1));
        exit(1);
    }

    if (mysql_stmt_execute(env->token_partial_match_st1))
    {
        printf("line %d error %s\n", __LINE__, mysql_stmt_error(env->token_partial_match_st1));
        exit(1);
    }

    char token[BUFFER_SIZE_DOC_CONTENT];
   unsigned long length;
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[0].buffer = token;
    bind[0].buffer_length = BUFFER_SIZE_DOC_CONTENT;
    bind[0].length = &length;
    bind[0].is_null=0;

    if (mysql_stmt_bind_result(env->token_partial_match_st1, bind))
    {
        printf("line  %d error %s\n", __LINE__, mysql_stmt_error(env->token_partial_match_st1));
        exit(1);
    }

    if (mysql_stmt_fetch(env->token_partial_match_st1))
    {
      //  printf("line  %d error %s \n", __LINE__, mysql_stmt_error(env->token_partial_match_st1));
        mysql_stmt_free_result(env->token_partial_match_st1);
    }
    else
    {
        utarray_push_back(token_ids, &token);
        mysql_stmt_free_result(env->token_partial_match_st1);
    }

    return 0;
}
/*
int main(int argc, char const *argv[])
{
    wiser_env env;
    memset(&env, 0, sizeof(env));
    init_database1(&env, "none");
    struct tm p;

    db_get_document_visit_time(&env, 1, &p);
     printf("the value of time %s \n",asctime(&p));
    fin_database1(&env);
    return 0;
    //int doc = db_get_document_id1(&env, "hh", strlen("hh"));
    // char title[BUFFER_SIZE_DOC_CONTENT];
    // int size;
    // int id = 3;
    // int fl = db_get_document_title1(&env, id, title, &size);

    // printf("%s\n", title);
    // printf("%d\n", size);

    //db_add_document1(&env, "qq", 2, "yy", 2);
    // int doc_count = 0;
    // int id=5;
    // id = db_get_token_id1(&env, "hh", 2, 1, &doc_count);
    // printf("the value of doc_count %d\n", doc_count);
    // printf("the valuer of id %d\n",id);

    // char token[BUFFER_SIZE_DOC_CONTENT];
    // int token_size;
    // const int token_id = 1;
    // db_get_token1(&env, token_id, token, &token_size);
    // printf("the token is %s and token_size is %d\n", token, token_size);

    // int doc_count = 0;
    // char postings[BUFFER_SIZE_DOC_CONTENT];
    // int size;
    // db_get_postings1(&env, 1, &doc_count, (void **)&postings, &size);

    // printf("the postings is %s and size is %d\n", postings, size);
    // char *escape_object = NULL;
    // unsigned int = scape_size = mysql_real_escape_string(env.conn, escape_object, (char *)object, objsize);

    // char *bu="hhha";
    // db_update_postings1(&env,1,9,(void*)bu,4);
    //     char buf[BUFFER_SIZE_DOC_CONTENT];
    //     int size;
    //     db_get_settings1(&env, "compress_method", strlen("compress_method"), buf, &size);
    //     printf("the value is %s and ythe size is %d\n", buf, size);

    // int flag = db_replace_settings1(&env, "compress_method", strlen("compress_method"), "none", strlen("none"));
    // printf("%d\n", flag);

    // int count  = db_get_document_count1(&env);
    // printf("%d\n",count);

    
}
*/