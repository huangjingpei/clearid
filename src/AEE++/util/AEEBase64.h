#ifndef _AEEBASE64_H_
#define _AEEBASE64_H_

#include "zmaee_typedef.h"

class Base64
{
public:
	static int Encode(unsigned char const* pSrc, unsigned int nSreLen,char* pDes, unsigned int nDesMax);
	static int Decode(unsigned char const* pSrc, unsigned int nSreLen,char* pDes, unsigned int nDesMax);
private:
	static int GetIdx(char c);
	static bool IsBase64(unsigned char c);
};

#endif // _AEEBASE64_H_
