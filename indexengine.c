#include "wiser.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h> //for getopt, fork
#include "util.h"
#include "mysqldatabase.h"
#include "search.h"
#include "wikiload.h"
/**
 * 入口
 * @param[in] argc 参数的个数
 * @param[in] argv 参数指针的数组
 */


int main(int argc, char *argv[])
{

  wiser_env env;
  extern int optind;
 
  int enable_or_query = 0;
  int max_index_count = -1;
  int ii_buffer_update_threshold = DEFAULT_II_BUFFER_UPDATE_THRESHOLD;
  int enable_phrase_search = TRUE;
  const char *compress_method_str = NULL, *wikipedia_dump_file = NULL,*query = NULL;

  {
    int ch;
    extern int opterr;
    extern char *optarg;

    while ((ch = getopt(argc, argv, "c:x:q:m:t:s:a")) != -1)
    {
      switch (ch)
      {
      case 'c':
        compress_method_str = optarg;
        break;
      case 'x':
        wikipedia_dump_file = optarg;
        break;
      case 'q':
        query = optarg;
        break;
      case 'm':
        max_index_count = atoi(optarg);
        break;
      case 't':
        ii_buffer_update_threshold = atoi(optarg);
        break;
      case 's':
        enable_phrase_search = FALSE;
        break;
      case 'a':
        enable_or_query=TRUE;
        break;
      
      default:
        exit(EXIT_SUCCESS);

      }
    }
  }
  //printf("the value of optind is %d\n",optind);
  if (argc != optind + 1)
  {
    printf(
        "usage: %s [options] [database name (none or invertindex)] \n"
        "\n"
        "options:\n"
        "  -c compress_method            : compress method for postings list\n"
        "  -x wikipedia_dump_xml         : wikipedia dump xml path for indexing\n"
        "  -q search_query               : query for search\n"
        "  -m max_index_count            : max count for indexing document\n"
        "  -t ii_buffer_update_threshold : inverted index buffer merge threshold\n"
        "  -s                            : don't use tokens' positions for search\n"
        "\n"
        "compress_methods:\n"
        "  none   : don't compress.\n"
        "  golomb : Golomb-Rice coding(default).\n",
        argv[0]);
    return -1;
  }

  {
    
    int rc = init_env(&env, ii_buffer_update_threshold, enable_phrase_search,enable_or_query, argv[optind]);
   
    
    if (!rc)
    {
      print_time_diff();

      if (wikipedia_dump_file)
      {
        parse_compress_method(&env, compress_method_str, -1);
        begin1(&env);
        if (!load_wikipedia_dump(&env, wikipedia_dump_file, add_document, max_index_count))
        {
          add_document(&env, NULL, NULL, NULL);
          commit1(&env);
        }
        else
        {
          rollback1(&env);
        }
      }

      if (query)
      {
        int cm_size;
        char *cm;
        db_get_settings1(&env, "compress_method", sizeof("compress_method") - 1, &cm, &cm_size);
        parse_compress_method(&env, cm, cm_size);
        env.indexed_count = db_get_document_count1(&env);
        search(&env, query);
      }
      fin_env(&env);

      print_time_diff();
    }
    return rc;
  }
}
