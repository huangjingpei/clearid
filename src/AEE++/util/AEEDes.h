#ifndef _AEEDES_H_
#define _AEEDES_H_
#include "zmaee_typedef.h"
class AEEDes
{

public:
	static int DesEncrypt(const char *str,int lenOfStr, const char password[8], char* out,int maxLen);
	static int DesDecrypt(const char *str, int lenOfStr,const char password[8], char* out,int maxLen);
	
private:
	static void DES(const char str[8],char out[8],  const char password[8]);
	static void DES_1(const char str[8], char out[8], const char password[8]);
	static int GetBit(const char *number, int pos, int bits);
	static void SetBit(char *number, int *bit, int bits);
	static void ip(char LR[8]);
	static void ip_1(char LR[8]);
	static void pc_1(const char password[8], char key[]);
	static void pc_2(const char CD[7], char K[6]);
	static void LeftMoveOf28Bit(int num[28], int n);
	static void lm(char CD[7], int n);
	static void expansion(const char Ri[4], char Re[6]);
	static void XOR(const char *A, const char *B, char *X, int bits);
	static void sboxSetBit(int a[], int x);
	static void sbox(char A[6], char B[4]);
	static void pbox(char A[4]);
};

#endif//_AEEDES_H_