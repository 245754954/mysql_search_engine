PROJECT(trunk)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
add_compile_options(-std=c++11)
SET(CMAKE_CXX_COMPILER "/usr/bin/gcc-8") 

SET(INDEX_ENGINE  indexengine.c  wiser.c mysqldatabase.c  encrypt.c util.c token.c search.c postings.c  wikiload.c)

SET(SEARCH_ENGINE  searchengine.c  wiser.c mysqldatabase.c  encrypt.c util.c token.c search.c postings.c  wikiload.c)


SET(ENCRYPT_TOOl aesencrypt.c)

SET(evp_tool evp_encrypt.c)

SET(evp_tool_only evp_encrypt_only.c)

#判断编译器类型,如果是gcc编译器,则在编译选项中加入c++11支持

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall -std=c99 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -O3 -g  -I ./includec -I . ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_BUILD_TYPE "Debug")  
    SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -DDEBUG -ggdb3 -O0")  
    SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")  
    message(STATUS "optional:-std=c99")   
endif(CMAKE_COMPILER_IS_GNUCXX)


ADD_EXECUTABLE(indexengine ${INDEX_ENGINE})
TARGET_LINK_LIBRARIES(indexengine PRIVATE  m  expat crypto mysqlclient )


add_executable(searchengine  ${SEARCH_ENGINE})
target_link_libraries (searchengine PRIVATE  m  mysqlclient expat event crypto  )





add_executable(encrypttool  ${ENCRYPT_TOOl})
target_link_libraries (encrypttool PRIVATE crypto  )

add_executable(evp_tool  ${evp_tool})
target_link_libraries (evp_tool PRIVATE crypto  )

add_executable(evp_tool_only  ${evp_tool_only})
target_link_libraries (evp_tool_only PRIVATE crypto  )
