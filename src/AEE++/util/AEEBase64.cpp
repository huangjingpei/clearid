
#include "AEEBase64.h"
#include "zmaee_stdlib.h"

const char base64_chars[65]={
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/','='
};

bool Base64::IsBase64(unsigned char c)
{
	return (zmaee_isalnum(c) || (c == '+') || (c == '/'));
}

int Base64:: GetIdx(char c)
{
	unsigned int i = 0;
	for(i = 0;i<sizeof(base64_chars);i++)
	{
		if(base64_chars[i] == c)
		{
			return i;
		}
	}
	return 0;
}

int Base64::Decode(unsigned char const* encoded_string, unsigned int in_len,char* pDes, unsigned int nDesMax)
{
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	int idx = 0;

	for(j = in_len; j>0 && '='==encoded_string[j-1]; --j) ;
	unsigned int desLen = (unsigned int)(j/4)*3+(j%4+1)/2;
	if(desLen > nDesMax)
		return 0;

	while (in_len-- && ( encoded_string[in_] != '=') && IsBase64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) {
			for (i = 0; i <4; i++)
			{
				char_array_4[i] = GetIdx(char_array_4[i]) ;

			}


			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
			{
				pDes[idx++] = char_array_3[i];
			}

			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = GetIdx(char_array_4[i]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
			pDes[idx++] = char_array_3[j];
	}

	return idx ;
}

int Base64::Encode(unsigned char const* bytes_to_encode, unsigned int in_len,char* pDes, unsigned int nDesMax)
{
	int i = 0;
	int j = 0;
	int idx = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];
	unsigned int desLen = (in_len+2) / 3 * 4 ;
	if(desLen > nDesMax)
		return 0;

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
			{
				pDes[idx++] =  base64_chars[char_array_4[i]];
			}

			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
		{
			pDes[idx++] = base64_chars[char_array_4[j]];
		}
		while((i++ < 3))
		{
			pDes[idx++] ='=';
		}
	}
	return idx ;
}


