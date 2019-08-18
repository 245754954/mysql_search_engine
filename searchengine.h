#ifndef __SEARCHENGINE_H__
#define __SEARCHENGINE_H__
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

#include "encrypt.h"
void find_http_header(struct evhttp_request *req, struct evkeyvalq *params, const char *query_char,char *term);

void httpd_handler(struct evhttp_request *req, void *arg);
void show_help();

void signal_handler(int sig);



#endif