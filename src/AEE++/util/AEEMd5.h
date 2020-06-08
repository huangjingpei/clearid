#ifndef _AEEMD5_H_
#define _AEEMD5_H_


class AEEMD5
{
public:
	AEEMD5();
	virtual ~AEEMD5();
	void DigestMd5(unsigned char const *buf, unsigned int len, unsigned char digest[16]);

private:
	struct MD5Context
	{
		unsigned int buf[4];
		unsigned int bits[2];
		unsigned char in[64];
	};

	void MD5Init (MD5Context *ctx);
	void MD5Update (MD5Context *ctx,unsigned char const *buf, unsigned int len);
	void MD5Final (unsigned char digest[16], MD5Context *context);
	void MD5Transform (unsigned int buf[4], unsigned int const in[16]);
	MD5Context m_ctx;

};
#endif//_AEEMD5_H_

