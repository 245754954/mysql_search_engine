#include <openssl/aes.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "aesencrypt.h"
#include <unistd.h>

unsigned char* str2hex(char *str) {
    unsigned char *ret = NULL;
    int str_len = strlen(str);
    int i = 0;
    assert((str_len%2) == 0);
    ret = (char *)malloc(str_len/2);
    for (i =0;i < str_len; i = i+2 ) {
        sscanf(str+i,"%2hhx",&ret[i/2]);
    }
    return ret;
}
char *padding_buf(char *buf,int size, int *final_size) {
    char *ret = NULL;
    int pidding_size = AES_BLOCK_SIZE - (size % AES_BLOCK_SIZE);
    int i;
    *final_size = size + pidding_size;
    ret = (char *)malloc(size+pidding_size);
    memcpy( ret, buf, size);
    if (pidding_size!=0) {
        for (i =size;i < (size+pidding_size); i++ ) {
            ret[i] = 0;
        }
    }
    return ret;
}
void printf_buff(char *buff,int size) {
    int i = 0;
    for (i=0;i<size;i ++ ) {
        printf( "%02X ", (unsigned char)buff[i] );
        if ((i+1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n\n\n\n");
}
void encrpyt_buf(char *raw_buf, char **encrpy_buf, int len ) {
    AES_KEY aes;
    unsigned char *key = str2hex("8cc72b05705d5c46f412af8cbed55aad");
    unsigned char *iv = str2hex("667b02a85c61c786def4521b060265e8");
    AES_set_encrypt_key(key,128,&aes);
    AES_cbc_encrypt(raw_buf,*encrpy_buf,len,&aes,iv,AES_ENCRYPT);
    free(key);
    free(iv);
}
void decrpyt_buf(char *raw_buf, char **encrpy_buf, int len ) {
    AES_KEY aes;
    unsigned char *key = str2hex("8cc72b05705d5c46f412af8cbed55aad");
    unsigned char *iv = str2hex("667b02a85c61c786def4521b060265e8");
    AES_set_decrypt_key(key,128,&aes);
    AES_cbc_encrypt(raw_buf,*encrpy_buf,len,&aes,iv,AES_DECRYPT);
    free(key);
}
  

int main(int argn, char *argv[] ) {
    // char *raw_buf = NULL;
    // char *after_padding_buf = NULL;
    // int padding_size = 0;
    // char *encrypt_buf = NULL;
    // char *decrypt_buf = NULL;
    // // 1
    // raw_buf = (char *)malloc(17);
    // memcpy(raw_buf,"life's a struggle",17);
    // printf("------------------raw_buf\n");
    // printf_buff(raw_buf,17);
    // // 2
    // after_padding_buf = padding_buf(raw_buf,17,&padding_size);
    // printf("------------------after_padding_buf\n");
    // printf_buff(after_padding_buf,padding_size);
    // // 3
    // encrypt_buf = (char *)malloc(padding_size);
    // encrpyt_buf(after_padding_buf,&encrypt_buf, padding_size);
    // printf("------------------encrypt_buf\n");
    // printf_buff(encrypt_buf,padding_size);
    // // 4
    // decrypt_buf = (char *)malloc(padding_size);
    // decrpyt_buf(encrypt_buf,&decrypt_buf,padding_size);
    // printf("------------------decrypt_buf\n");
    // printf_buff(decrypt_buf,padding_size);
    // free(raw_buf);
    // free(after_padding_buf);
    // free(encrypt_buf);
    // free(decrypt_buf);




    execv("/usr/bin/openssl",argv);

    return 0;
}
