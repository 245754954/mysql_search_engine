#include "encrypt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <ctype.h>
#include <openssl/md5.h>

//字节流转换为十六进制字符串

void ByteToHexStr(const unsigned char *source, char *dest, int sourceLen)

{
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f;

        highByte += 0x30;

        if (highByte > 0x39)
            dest[i * 2] = highByte + 0x07;
        else
            dest[i * 2] = highByte;

        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
    return;
}

//字节流转换为十六进制字符串的另一种实现方式

void Hex2Str(const char *sSrc, char *sDest, int nSrcLen)
{
    int i;
    char szTmp[3];

    for (i = 0; i < nSrcLen; i++)
    {
        sprintf(szTmp, "%02X", (unsigned char)sSrc[i]);
        memcpy(&sDest[i * 2], szTmp, 2);
    }
    return;
}

//十六进制字符串转换为字节流
void HexStrToByte(const char *source, unsigned char *dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i += 2)
    {

        highByte = toupper(source[i]);
        lowByte = toupper(source[i + 1]);

        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;

        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;

        dest[i / 2] = (highByte << 4) | lowByte;
    }
    return;
}

void md5_without_salt(MD5_CTX *ctx, char input[],  char *hash_result)
{
    unsigned char output[16];
    char buf[33]={'\0'};
    MD5_Init(ctx);
    MD5_Update(ctx, input, strlen(input));
   
    // gf_log("the length of input is %ld",GF_LOG_ERROR,"%ld\n",strlen(input));
    MD5_Final(output, ctx);
    int i=0;
    char tmp[3]={'\0'};
    for( i=0; i<16; i++ ){
        sprintf(tmp,"%02X",output[i]);
        strcat(buf,tmp);
    }
    
    memset(hash_result,0,strlen(hash_result));
    strcpy(hash_result,buf);
    //printf("the hash result is %s\n",buf);
     //gf_log("the the length of input text is",GF_LOG_ERROR,"%ld\n",strlen(input));
    return;
}



void md5_with_salt(MD5_CTX *ctx, char salt[],char *hash_result)
{
    unsigned char output[64];
    memset(output,0,sizeof(output));
   // char *plaintext = (char *)malloc(strlen(input) + strlen(salt));
    //strcpy(plaintext, input);

    //strcat(plaintext, salt);
    //gf_log("the plaintext is",GF_LOG_ERROR,"%s\n",plaintext);
   // printf("the plaintext is %s\n", plaintext);
    //gf_log("the strlen of plaintext is %ld ",GF_LOG_ERROR,"%ld\n",strlen(plaintext));
    //printf("the strlen of plaintext is %ld\n",strlen(plaintext));
    MD5_Update(ctx,salt,strlen(salt));
   // MD5_Update(ctx, plaintext, strlen(plaintext));

    MD5_Final(output, ctx);
   
    bzero(hash_result,strlen(hash_result));
    ByteToHexStr(output, hash_result, 64);

    //生成md5是带盐的md5，将来需要提取md5
    char cs[49];
    int i=0;
    for (i = 0; i < 48; i += 3)
    {
        cs[i] = hash_result[i / 3 * 2];
        cs[i + 1] = salt[i / 3];
        cs[i + 2] = hash_result[i / 3 * 2 + 1];
    }
    cs[48]='\0';

   // gf_log("cs content ",GF_LOG_ERROR,"%s\n",cs);
    //gf_log("before hash result by bcopy from cs ",GF_LOG_ERROR,"%s\n",hash_result);
    memset(hash_result,0,strlen(hash_result));
    //gf_log("strlen of hash_result",GF_LOG_ERROR,"%ld\n",strlen(hash_result));
    bcopy(cs,hash_result,strlen(cs));
    
   // gf_log("hash result by bcopy from cs ",GF_LOG_ERROR,"%s\n",hash_result);
    //free(plaintext);
    //plaintext=NULL;
    return ;  
}

void get_salt_from_hash(char *md5,  char *salt, int salt_len)
{
    //17位，包含结束位
    char *salt1 = calloc(salt_len, sizeof(char));
    int len = strlen(md5);
    
    char str[1];
    int i=0;
    for (i = 0; i < len; i += 3)
    {
        memset(str, 0, sizeof(str));
        str[0] = md5[i + 1];
        if (i == 0)
            strcpy(salt1, str);
        else
        {
            strcat(salt1, str);
        }
    }
    salt1[salt_len]='\0';
    //printf("the salt1 is %s\n",salt1);
    //字符串的长度计算一般采用strlen()+1
   // printf("the length of salt1 get from hash is %ld\n",strlen(salt1));
    bcopy(salt1,salt,strlen(salt1));

    free(salt1);
    salt1=NULL;
  
}

void get_salt(char salt[], int len)
{
    srand(rand());
    char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    int i=0;
    for (i = 0; i < len; i++)
    {
      salt[i]=hex[rand()%16];  
    }
    salt[len]='\0';  
}
/*
int verify(MD5_CTX *ctx, char *input, char *md5, int salt_len)
{
    //申请盐的长度是17
     char salt[salt_len+1];
  
   
    char *hash_again = calloc(48,sizeof(char));
    bzero(hash_again,48);

    //通过计算从密文里边得到hash，有效位16位，包含以为结束位17位
    get_salt_from_hash(md5,salt,salt_len+1);
    //printf("the strlen of md5 is %ld\n",strlen(md5));

    md5_with_salt(ctx, input, salt,hash_again);

    
    //printf("the hash again is %s\n",hash_again);
   
    if (strcmp(md5, hash_again) == 0)
    {
        free(hash_again);
        hash_again=NULL;
        return 1;
    }
    else
    {
        free(hash_again);
        hash_again=NULL;
        return 0;
    }

   
}
*/

// int main(){

//    MD5_CTX ctx;
//    int i = 0;
//    char *str = "zhang";
//    unsigned char out[16];
//    md5_without_salt(&ctx, str, out);

// }
/*
int main(int argc, char const *argv[])
{

    MD5_CTX ctx;

    int i = 0;
    char *str = "zfc";
    
    
    md5_without_salt(&ctx, str, out);


    //盐的有效长度是16位，但是strlen是义‘\0’辨别结束的
    //所以这李分配17个字节
    char salt[17];
    get_salt(salt, 16); //产生随机
    printf("the salt is %s\n",salt);
    printf("the length os salt is %ld\n",strlen(salt));
    char *md5 = calloc(48,sizeof(char));

    md5_with_salt(&ctx, str, salt,md5);
    printf("the md5_result is %s \n",md5);
    printf("the length of md5 is %ld\n",strlen(md5));
    
  

    if (verify(&ctx, str, md5, 16))
    {
        printf("true\n");
    }
    else
    {

        printf("false\n");
    }

    free(md5);
    md5 = NULL;
   
    return 0;
}*/
