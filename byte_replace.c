#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "byte_replace.h"
int main(int argc, char *argv[])
{
  int ch;
  extern int opterr;

  const char *replace_str = NULL;
  const char *file = NULL;
  int position;

  while ((ch = getopt(argc, argv, "c:p:f:")) != -1)
  {

    switch (ch)
    {
    case 'c':
      replace_str = optarg;
      //printf("option a:%s\n",optarg);
      break;
    case 'p':
      position = atoi(optarg);
      break;
    case 'f':
      file = optarg;
      //printf("option c\n");
      break;
    default:
      printf("Knowed option:%c\n", ch);
    }
  }

  //以r方式打开文件，可读写
  FILE *fp = fopen(file, "r+");
  if (fp == NULL)
  {
    perror("Open file recfile");
    exit(1);
  }
  //fwrite(str,sizeof(str),1,fp);

  //输出现在文件指针的位置，发现在文件的第10个字节的位置
  //说明fwrite写完文件后，文件指针移动到文件结尾
  //printf("%d  ",ftell(fp));
  //将文件指针返回0处
  //fseek(fp, 0, SEEK_SET);
  //将首字节位置内容置为#
  //printf("file position %ld\n", ftell(fp));
  int len;
  len = strlen(replace_str);

  char buf[1024] = {'\0'};
  fseek(fp, position, SEEK_SET);
  int read_len = fread(buf, sizeof(char), len, fp);
  printf("read buf is %s \n and the len is %d\n", buf,read_len);
  //fputc('#',fp);
  //将2字节位置内容置为%
  fseek(fp, position, SEEK_SET);
  int write_len = fwrite(replace_str,sizeof(char),len,fp);
  printf("write buf is %s \n and thr len is %d\n",replace_str,write_len);
 
  //printf("the write item is %d\n",n_w);
 
 

  //printf("len is %d\n", len);
  fclose(fp);
  return 0;
}
