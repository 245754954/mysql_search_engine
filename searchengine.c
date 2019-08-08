#include "searchengine.h"
#include "wiser.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h> //for getopt, fork
#include <string.h> //for strcat
//for struct evkeyvalq
#include <sys/queue.h>
#include <event.h>
//for http
#include <evhttp.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <signal.h>
#include <evhttp.h>
#include <event.h>
#include <event2/http.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_compat.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <event2/util.h>
#include <event2/listener.h>

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include "search.h"
#include "wiser.h"
#include "mysqldatabase.h"
#define DEFAULT_TIME 10        /*10s检测一次*/
#define MIN_WAIT_TASK_NUM 10   /*如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池*/
#define DEFAULT_THREAD_VARY 10 /*每次创建和销毁线程的个数*/
#define true 1
#define false 0

#define MYHTTPD_SIGNATURE "myhttpd v 0.0.1"
#define BUF_MAX 8192

//解析http头，主要用于get请求时解析uri和请求参数
void find_http_header(struct evhttp_request *req, struct evkeyvalq *params, const char *query_char, char *term)
{

    if (req == NULL || params == NULL || query_char == NULL)
    {
        printf("====line:%d,%s\n", __LINE__, "input params is null.\n");
        return;
    }

    struct evhttp_uri *decoded = NULL;
    char *query = NULL;
    char *query_result = NULL;
    const char *path;
    const char *uri = evhttp_request_get_uri(req); //获取请求uri

    if (uri == NULL)
    {
        printf("====line:%d,evhttp_request_get_uri return null\n", __LINE__);
        return;
    }
    else
    {
        // printf("====line:%d,Got a GET request for <%s>\n", __LINE__, uri);
    }

    //解码uri
    decoded = evhttp_uri_parse(uri);
    if (!decoded)
    {
        printf("====line:%d,It's not a good URI. Sending BADREQUEST\n", __LINE__);
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return;
    }
    //获取uri中的path部分
    path = evhttp_uri_get_path(decoded);
    if (path == NULL)
    {
        path = "/";
    }
    else
    {
        // printf("====line:%d,path is:%s\n", __LINE__, path);
    }

    //获取uri中的参数部分
    query = (char *)evhttp_uri_get_query(decoded);
    if (query == NULL)
    {
        // printf("====line:%d,evhttp_uri_get_query return null\n", __LINE__);
        return;
    }

    //查询指定参数的值
    evhttp_parse_query_str(query, params);
    query_result = (char *)evhttp_find_header(params, query_char);
    strcpy(term, query_result);

    return;
}

//处理模块
void httpd_handler(struct evhttp_request *req, void *arg)
{
    wiser_env *env = (wiser_env *)arg;

    char term[BUFFER_SIZE_DOC_CONTENT] = {'\0'};
    
    struct evkeyvalq sign_params = {0};
    find_http_header(req, &sign_params, "term", term); //获取get请求uri中的sign参数
                                                       // printf("the term is %s\n",term);
    if (term == NULL || !strcmp("", term)||!strcmp(" ",term))
    {
        printf("====line:%d,%s\n", __LINE__, "request uri no param term.\n");
    }
    else
    {
        // printf("====line:%d,get request param: term=[%s]\n", __LINE__, term);
        search(env, term);
    }

    /*
    具体的：可以根据GET/POST的参数执行相应操作，然后将结果输出
    ...
    */
    /* 输出到客户端 */
    //HTTP header
    evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
    //处理输出header头
    evhttp_add_header(req->output_headers, "Connection", "keep-alive");
    evhttp_add_header(req->output_headers, "Cache-Control", "no-cache");
    //输出的内容
    struct evbuffer *buf;
    buf = evbuffer_new();
    if (buf == NULL)
    {
        printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
        return;
    }

    evbuffer_add_printf(buf, "Receive get request,Thamks for the request! the received trem is \n%s\n", term);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
}

void show_help()
{
    char *help = "http://localhost:8080\n"
                 "-l <ip_addr> interface to listen on, default is 0.0.0.0\n "
                 "-p <num> port number to listen on, default is 8080\n"
                 "-d run as a deamon\n"
                 "-t <second> timeout for a http request, default is 120 seconds\n "
                 "-h print this help and exit\n"
                 "\n";
    fprintf(stderr, "%s", help);
}

//当向进程发出SIGTERM/SIGHUP/SIGINT/SIGQUIT的时候，终止event的事件侦听循环
void signal_handler(int sig)
{
    switch (sig)
    {
    case SIGTERM:
    case SIGHUP:
    case SIGQUIT:
    case SIGINT:
        event_loopbreak(); //终止侦听event_dispatch()的事件侦听循环，执行之后的代码
        break;
    }
}

int main(int argc, char *argv[])
{

    wiser_env env;
    int max_index_count = -1;
    int ii_buffer_update_threshold = DEFAULT_II_BUFFER_UPDATE_THRESHOLD;
    int enable_phrase_search = TRUE;
    const char *compress_method_str = NULL,*query = NULL;

    //默认参数
    char *httpd_option_listen = "0.0.0.0";
    int httpd_option_port = 8080;
    int httpd_option_daemon = 0;
    int httpd_option_timeout = 120; //in seconds


    //自定义信号处理函数
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    
    //获取参数
    int c;
    extern char *optarg;
    while ((c = getopt(argc, argv, "l:p:dt:h")) != -1)
    {
        switch (c)
        {
        case 'l':
            httpd_option_listen = optarg;
            break;
        case 'p':
            httpd_option_port = atoi(optarg);
            break;
        case 'd':
            httpd_option_daemon = 1;
            break;
        case 't':
            httpd_option_timeout = atoi(optarg);
            break;
        case 'h':
        default:
            show_help();
            exit(EXIT_SUCCESS);
        }
    }

    //判断是否设置了-d，以daemon运行
    if (httpd_option_daemon)
    {
        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (pid > 0)
        {
            //生成子进程成功，退出父进程
            exit(EXIT_SUCCESS);
        }
    }



    int rc = init_env(&env, ii_buffer_update_threshold, enable_phrase_search, DATABASE);
    int cm_size;
    char *cm;
    db_get_settings1(&env, "compress_method", sizeof("compress_method") - 1, &cm, &cm_size);
    parse_compress_method(&env, cm, cm_size);
    env.indexed_count = db_get_document_count1(&env);


    //初始化event API
    event_init();


    //创建一个http server
    struct evhttp *httpd;
    httpd = evhttp_start(httpd_option_listen, httpd_option_port);
    if (httpd == NULL)
    {

        fprintf(stderr, "Error: Unable to listen on %s:%d\n\n", httpd_option_listen, httpd_option_port);

        exit(1);
    }
    evhttp_set_timeout(httpd, httpd_option_timeout);
    //指定generic callback
    evhttp_set_gencb(httpd, httpd_handler, &env);

    //也可以为特定的URI指定callback
    //evhttp_set_cb(httpd, "/", specific_handler, NULL);
    //循环处理events
    event_dispatch();

    fin_env(&env);
    evhttp_free(httpd);

    return 0;
}
