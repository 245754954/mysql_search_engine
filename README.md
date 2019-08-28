This project is based on the open source wiser project,Wiser project information can be accessed through books “how to Develops a Search Engine” the author is shantainhaozhi(jepanese). 

The original code can be downloaded through the following links(http://www.ituring.com.cn/book/1582),Inverted index in source project is stored in SQLite database,However, its scale of expansion is limited, and it just support and query ,and phrase query,so the project decided to change the inverted index storage to mysql.and extend it's function to support or query,Because the my level is limited, if the code is inadequate, please contact the author.

the function of this project are listed as below:
    support and query
    support or query 
    support phrase query
    the dictionary of invertindex file is encrypted by symmetric encryption algorithm
    Supporting TF-IDF sorting algorithm
    Supporting sorting algorithm based on document creation time
    support sorting algorithm based on the size of document
    Support dynamic index construction
    Support HTTP module

My email address is:zhangfucheng18@nudt.edu.cn


Installation Prerequisites
    (1) Install project dependency Libraries (ubuntu16)

        apt-get install build-essential mysql-server mysql-client libmysqlclient-dev libexpat1-dev bzip2  openssl libssl-dev libevent-dev 

    (2)compile

        mkdir build

        cd ./build

        cmake ..

        make

    (3) run

        ./indexengine [options] [database or none]
        "options:\n"
        "  -c compress_method            : compress method for postings list\n"
        "  -x wikipedia_dump_xml         : wikipedia dump xml path for indexing\n"
        "  -q search_query               : query for search\n"
        "  -m max_index_count            : max count for indexing document\n"
        "  -t ii_buffer_update_threshold : inverted index buffer merge threshold\n"
        "  -s                            : don't use tokens' positions for search\n"
        "  -a enable_or_query             :enable or query"
       "database or none:\n"

       "database name:When executing a query, you need to specify the name of the database that stores the inverted index"

       "none                             :when the database name is none the programe will create a default database automatically ,the default database name is  invertindex1 ,you can chanage the default database name in wiser.h  "

    Running Example
        (1) when you have enough files ,you need to build invert index through the command as follows
            template : ./indexengine -x files -m  ax_index_count -t inverted index buffer merge threshold database none
            instance: ./indexengine -x zhwiki-20190701-pages-articles-multistream1.xml -m 5000 -t 500 none
        
        (2) query
            template : ./indexengine -q [query condition] [database name]
            instance : ./indexengine -q "huawei" invertindex
            instance : ./indexengine -q "huawei" -a invertindex1  //-a means this query is or query


audit:
In order to test the concurrent quantity of HTTP modulethe test script is written in the audit directory.

First you need to configure the nginx reverse proxy module,The reverse proxy configuration file for nginx is in "nginx_conf",after installing the nginx server,you can use config file as "nginx_conf",The configuration file is in the audit directory.if you operating system is ubuntu16.04,You can follow the following commands:

apt-get install nginx

vi /etc/nginx/nginx.conf

Replace the original nginx configuration file with "nginx_conf" file in audit directory

restart nginx by using the command :nginx -s reload

start the searchengine server by using the script "start_server.sh",after execute the "start_server.sh" script,fourty daemons will be started in the background

using the http-load tool to test the searchengine server,for example ,you can use the command as follows:
    http-load -p 40 -s 10 as you can see -p means parallel ,-s means second








