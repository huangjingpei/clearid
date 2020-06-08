#include "AEEDes.h"
#include "zmaee_typedef.h"
#include "aeebase.h"


//初始换位IP
static const char IP[64] =
{
	58,50,42,34,26,18,10,2,
	60,52,44,36,28,20,12,4,
	62,54,46,38,30,22,14,6,
	64,56,48,40,32,24,16,8,
	57,49,41,33,25,17,9,1,
	59,51,43,35,27,19,11,3,
	61,53,45,37,29,21,13,5,
	63,55,47,39,31,23,15,7
};

//逆初始换位IP-1
static  const char IP_1[64] =
{
	40,8,48,16,56,24,64,32,
	39,7,47,15,55,23,63,31,
	38,6,46,14,54,22,62,30,
	37,5,45,13,53,21,61,29,
	36,4,44,12,52,20,60,28,
	35,3,43,11,51,19,59,27,
	34,2,42,10,50,18,58,26,
	33,1,41,9,49,17,57,25
};

//置换选择
static const char PC_1[56] =
{
	57,49,41,33,25,17,9,
	1,58,50,42,34,26,18,
	10,2,59,51,43,35,27,
	19,11,3,60,52,44,36,
	63,55,47,39,31,23,15,
	7,62,54,46,38,30,22,
	14,6,61,53,45,37,29,
	21,13,5,28,20,12,4
};

//置换选择
static const char PC_2[48] =
{
	14,17,11,24,1,5,
	3,28,15,6,21,10,
	23,19,12,4,26,8,
	16,7,27,20,13,2,
	41,52,31,37,47,55,
	30,40,51,45,33,48,
	44,49,39,56,34,53,
	46,42,50,36,29,32
};

//扩展置换Expansion
static const char Expansion[48]=
{
	32,1,2,3,4,5,
	4,5,6,7,8,9,
	8,9,10,11,12,13,
	12,13,14,15,16,17,
	16,17,18,19,20,21,
	20,21,22,23,24,25,
	24,25,26,27,28,29,
	28,29,30,31,32,1
};

//P盒置换
static const char PBox[32] = {16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25};

//S盒子
static const char SBox[8][4][16] =
{
	{
		{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
		{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
		{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
		{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
	},
	{
		{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
		{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
		{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
		{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
	},
	{
		{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
		{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
		{13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
		{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
	},
	{
		{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
		{13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
		{10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
		{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
	},
	{
		{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
		{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
		{4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
		{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
	},
	{
		{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
		{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
		{9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
		{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
	},
	{
		{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
		{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
		{1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
		{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
	},
	{
		{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
		{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
		{7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
		{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
	}
};

//密钥移位次序(循环左移)，左右位分别循环左移的
static const char LM[16]={1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};




int AEEDes::DesEncrypt(const char *str,int lenOfStr, const char password[8], char* out,int maxLen)
{
	char tmpout[9] = {0};
	int i,n;
	if(lenOfStr%8==0)
		n = lenOfStr/8;
	else
		n = lenOfStr/8 + 1;

	if(maxLen < n*8)
		return 0;
	for(i=0; i<n; i++)
	{
		DES(str + i*8, tmpout,password);
		zmaee_memcpy(out + i*8, tmpout, 8);
	}

	return n*8;
}

int AEEDes::DesDecrypt(const char *str, int lenOfStr,const char password[8], char* out,int maxLen)
{
	int i,n;
	char tmpout[9] = {0};

	if(maxLen < lenOfStr)
		return 0;

	n = (lenOfStr%8==0) ? (lenOfStr/8) : (lenOfStr/8 + 1);

	for(i=0; i<n; i++)
	{
		zmaee_memset(tmpout,0,sizeof(tmpout));
		DES_1(str + i*8,tmpout, password);
		zmaee_memcpy(out +i*8, tmpout, 8);
	}

	return n*8;
}

//输入的明文是个字节长的带加密字符，和密钥
void AEEDes::DES(const char str[8],char out[8], const char password[8])
{
	int i;
	char  LR[8], key[8], CD[7], K[16][6], LRi[16][8], Rie[6], xorResult[6], sResult[4];
	zmaee_memcpy(LR, str, 8);
	zmaee_memcpy(key, password, 8);

	//1、初始置换
	ip(LR);

	//2、置换选择
	//密钥产生，置换选择，位->56位
	pc_1(key, CD);

	//3、依次产生组密钥
	for(i=0; i<16; i++){
		//3.1、把CD分成两个位，并分别循环移位
		lm(CD, LM[i]);

		//3.2、经过置换选择，从位选出为的K
		pc_2(CD, K[i]);

		//输入子密钥
		//printf("K%d=",i); printHex(K[i], 6);
	}


	//4、进行轮的扩展置换和S盒子代换
	for(i=0; i<16; i++){
		//4.1 设置Li = Ri-1
		if(i == 0) {
			zmaee_memcpy(LRi[0], LR+4, 4);
		}else{
			zmaee_memcpy(LRi[i], LRi[i-1]+4, 4);//Li=Ri-1
		}

		//4.2、扩展Ri
		if(i == 0){
			expansion(LR+4, Rie);
		}else{
			expansion(LRi[i-1]+4, Rie);//扩展Ri-1
		}

		//4.3、密钥Ki与扩展后的分组异或
		XOR(K[i], Rie, xorResult, 48);

		//4.4、进行S盒子运算，将上步得到的位的xorResult转换成位
		sbox(xorResult, sResult);

		//4.5、将上步得到的sResult做p盒子置换
		pbox(sResult);

		//4.6、继续和Li-1做异或运算,结果保存到Ri
		if(i == 0){
			XOR(sResult, LR, LRi[0]+4, 32);
		}else{
			XOR(sResult, LRi[i-1], LRi[i]+4, 32);
		}

		//输出每轮的加密结果
		//printf("i=%-2d:",i); printHex(LRi[i],8);
	}

	//5、需要把最后一轮的左右两部分互换一下
	char  t_char[4];
	zmaee_memcpy(t_char,   LRi[15],   4);
	zmaee_memcpy(LRi[15],   LRi[15]+4, 4);
	zmaee_memcpy(LRi[15]+4, t_char,   4);


	//6、逆初始置换
	ip_1(LRi[15]);
	zmaee_memcpy(out, LRi[15], 8);
}




//取bits位数的某位，为返回，为就返回
int AEEDes::GetBit(const char  *number, int pos, int /*bits*/)
{
	int i,j;
	pos--;
	i = pos / 8;
	j = pos % 8;

	return (number[i] >> (7-j)) & 0x01;
}


//使用bit数组来设置number的各位的值(共更改bits位)
void AEEDes::SetBit(char  *number, int *bit, int bits)
{
	char  t;
	int i,j,bytes;

	if(bits%8 ==0) bytes = bits/8;
	else bytes = bits/8  + 1;

	for(i=0; i<bytes; i++){
		t = 0;
		for(j=0; (j<8) && (i*8+j<bits); j++)
			t |= (char)(bit[i*8+j] << (7-j));
		number[i] = t;
	}
}


//初始换位
void AEEDes::ip(char  LR[8])
{
	int i,bit[64];
	for(i=0; i<64; i++)
		bit[i] = GetBit(LR, IP[i], 64);

	SetBit(LR, bit, 64);
}


//逆初始换位
void AEEDes::ip_1(char  LR[8])
{
	int i, bit[64];
	for(i=0; i<64; i++)
		bit[i] = GetBit(LR, IP_1[i], 64);

	SetBit(LR, bit, 64);
}


//置换选择,从初始的位密钥中选取位
void AEEDes::pc_1(const char  password[8], char  key[])
{
	int i, bit[56];
	for(i=0; i<56; i++)
		bit[i] = GetBit(password, PC_1[i], 64);
	SetBit(key, bit, 56);
}


//置换选择，从位的CD(28b+28b)导出为密钥
void AEEDes::pc_2(const char  CD[7], char  K[6])
{
	int i, bit[48];
	for(i=0; i<48; i++)
		bit[i] = GetBit(CD, PC_2[i], 56);
	SetBit(K, bit, 48);
}


//对位数循环左移n位，num[28]是字符数组
void AEEDes::LeftMoveOf28Bit(int num[28], int n)
{
	int i,t;
	while(n--){
		t = num[0];
		for(i=0; i<27; i++)
			num[i] = num[i+1];
		num[27] = t;
	}
}


//把为的cd分成两个位，分别循环左移n位
void AEEDes::lm(char  CD[7], int n)
{
	int i,bit[56],*C,*D;

	for(i=0; i<56; i++)
		bit[i] = GetBit(CD, i+1, 56);

	C = bit;
	D = bit + 28;

	LeftMoveOf28Bit(C, n);
	LeftMoveOf28Bit(D, n);

	SetBit(CD, bit, 56);
}


//扩展置换，把Ri从为扩展到位
void AEEDes::expansion(const char  Ri[4], char  Re[6])
{
	int i, bit[48];
	for(i=0; i<48; i++)
		bit[i] = GetBit(Ri, Expansion[i], 32);
	SetBit(Re, bit, 48);
}


//对前bits位进行异或操作，X=A^B
void AEEDes::XOR(const char  *A, const char  *B, char  *X, int bits)
{
	int i,bytes;
	if(bits%8 == 0) bytes = bits/8;
	else bytes = bits/8 + 1;

	//zmaee_memset(X, 0, bytes);
	for(i=0; i<bytes; i++)
		X[i] = A[i] ^ B[i];
}

//设置a[1]~a[4]，表示为二进制的x, x>=0 && x<=15
void AEEDes::sboxSetBit(int a[], int x)
{
	a[0] = (x & 8) >> 3;
	a[1] = (x & 4) >> 2;
	a[2] = (x & 2) >> 1;
	a[3] = (x & 1);
}

//S盒子操作
void AEEDes::sbox(char  A[6], char  B[4])
{
	int i,t,row,column,bitA[48],bitB[32];
	for(i=0; i<48; i++)
		bitA[i] = GetBit(A, i+1, 48);

	//s盒子代换,由bitA[48]得到bitB[32]
	for(i=0; i<8; i++){
		row = bitA[i*6]*2 + bitA[i*6+5];
		column = bitA[i*6+1]*8 + bitA[i*6+2]*4 + bitA[i*6+3]*2 + bitA[i*6+4];
		t = SBox[i][row][column];

		sboxSetBit(bitB+i*4, t);//t>=0 && t=<15,占bit
	}

	SetBit(B, bitB, 32);
}


//P盒子操作
void AEEDes::pbox(char  A[4])
{
	int i,bit[32];
	for(i=0; i<32; i++)
		bit[i] = GetBit(A, PBox[i], 32);
	SetBit(A, bit, 32);
}

//DES解密
void AEEDes::DES_1(const char  str[8],char out[8], const char password[8])
{
	int i;
	char  LR[8], key[8], CD[7], K[16][6], LRi[16][8], Rie[6], xorResult[6], sResult[4];
	zmaee_memcpy(LR, str, 8);
	zmaee_memcpy(key, password, 8);

	//1、初始置换
	ip(LR);

	//2、置换选择
	//密钥产生，置换选择，位->56位
	pc_1(key, CD);

	//3、依次产生组密钥
	for(i=0; i<16; i++){
		//3.1、把CD分成两个位，并分别循环移位
		lm(CD, LM[i]);

		//3.2、经过置换选择，从位选出为的K
		pc_2(CD, K[i]);

		//输入子密钥
		//printf("K%d=",i); printHex(K[i], 6);
	}


	//4、进行轮的扩展置换和S盒子代换
	for(i=0; i<16; i++){
		//4.1 设置Li = Ri-1
		if(i == 0) {
			zmaee_memcpy(LRi[0], LR+4, 4);
		}else{
			zmaee_memcpy(LRi[i], LRi[i-1]+4, 4);//Li=Ri-1
		}

		//4.2、扩展Ri
		if(i == 0){
			expansion(LR+4, Rie);
		}else{
			expansion(LRi[i-1]+4, Rie);//扩展Ri-1
		}

		//4.3、密钥Ki与扩展后的分组异或
		XOR(K[15-i], Rie, xorResult, 48);

		//4.4、进行S盒子运算，将上步得到的位的xorResult转换成位
		sbox(xorResult, sResult);

		//4.5、将上步得到的sResult做p盒子置换
		pbox(sResult);

		//4.6、继续和Li-1做异或运算,结果保存到Ri
		if(i == 0){
			XOR(sResult, LR, LRi[0]+4, 32);
		}else{
			XOR(sResult, LRi[i-1], LRi[i]+4, 32);
		}

		//输出每轮的加密结果
		//printf("i=%-2d:",i); printHex(LRi[i],8);
	}

	//5、需要把最后一轮的左右两部分互换一下
	char  t_char[4];
	zmaee_memcpy(t_char,   LRi[15],   4);
	zmaee_memcpy(LRi[15],   LRi[15]+4, 4);
	zmaee_memcpy(LRi[15]+4, t_char,   4);


	//6、逆初始置换
	ip_1(LRi[15]);
	zmaee_memcpy(out, LRi[15], 8);

}
