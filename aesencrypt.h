#ifndef __AESENCRYPT_H__
#define __AESENCRYPT_H__


void decrpyt_buf(char *raw_buf, char **encrpy_buf, int len );
void encrpyt_buf(char *raw_buf, char **encrpy_buf, int len );
void printf_buff(char *buff,int size);
char *padding_buf(char *buf,int size, int *final_size);
unsigned char* str2hex(char *str);

#endif