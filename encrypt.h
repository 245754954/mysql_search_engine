#ifndef __ENCRYPT_H__
#define __ENCRYPT_H__
#include <stdio.h>


#include <openssl/md5.h>


/*extern int verify(MD5_CTX *ctx, char *input, char *md5, int salt_len);*/
extern void get_salt(char salt[], int len);
extern void get_salt_from_hash(char *md5, char *salt, int salt_len);
extern void md5_with_salt(MD5_CTX *ctx, char salt[],char *hash_result);
extern void md5_without_salt(MD5_CTX *ctx, char input[],  char output[]);
extern void HexStrToByte(const char *source, unsigned char *dest, int sourceLen);
extern void Hex2Str(const char *sSrc, char *sDest, int nSrcLen);
extern void ByteToHexStr(const unsigned char *source, char *dest, int sourceLen);


#endif