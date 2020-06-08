#include "AEEBitmapScale.h"
#include "zmaee_stdlib.h"


// 00000111111000001111100000011111
static const unsigned s_mask_07e0f81f = 0x07e0f81f;

static const unsigned s_mask_00ff00ff = 0x00ff00ff;

#define ZMAEE_BMP24TO16(r, g, b) ((((r) & 0xF8)<<8) | (((g) & 0xFC)<<3) | (((b) & 0xF8)>>3))
#define ZMAEE_BMP16TO32(c) ((0xFF000000) | (((c) & 0xF800) << 8) | (((c) & 0x07e0) << 5) | (((c) & 0x001F) << 3))


// #define USE_OLD_VERSION

#if defined (USE_OLD_VERSION)

#define _PI 3.1415926f                                                  // Value of PI
#define _BITS_PER_PIXEL_32  32                                          // 32-bit color depth
#define _BITS_PER_PIXEL_24  24                                          // 24-bit color depth
#define _PIXEL  unsigned int                                            // Pixel
#define _RGB(r,g,b) (((r) << 16) | ((g) << 8) | (b))                    // Convert to RGB

#define _GetRValue(c)   ((unsigned char)(((c) & 0x00FF0000) >> 16))     // Red color component
#define _GetGValue(c)   ((unsigned char)(((c) & 0x0000FF00) >> 8))      // Green color component
#define _GetBValue(c)   ((unsigned char)((c) & 0x000000FF))             // Blue color component
#define _GetAValue(c)   ((unsigned char)(((c) & 0xFF000000) >> 24))

#define _GetRValue16(c) ((unsigned char)(((c) & 0xf800) >> 8))      // Red color component
#define _GetGValue16(c) ((unsigned char)(((c) & 0x7e0) >> 3))       // Green color component
#define _GetBValue16(c) ((unsigned char)((c) & 0x1f) << 3)              // Blue color component

#define _BMP16TO32(c) ((0xFF000000) | (((c) & 0xF800) << 8) | (((c) & 0x07e0) << 5) | (((c) & 0x001F) << 3))


#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

typedef long fixed;                                                     // Our new fixed point type
#define itofx(x) ((x) << 8)                                             // Integer to fixed point
#define ftofx(x) (long)((x) * 256)                                      // Float to fixed point
#define dtofx(x) (long)((x) * 256)                                      // Double to fixed point
#define fxtoi(x) ((x) >> 8)                                             // Fixed point to integer
#define fxtof(x) ((float) (x) / 256)                                    // Fixed point to float
#define fxtod(x) ((double)(x) / 256)                                    // Fixed point to double
#define Mulfx(x,y) (((x) * (y)) >> 8)                                   // Multiply a fixed by a fixed
#define Divfx(x,y) (((x) << 8) / (y))                                   // Divide a fixed by a fixed


void _ScaleBicubic32(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	long m_iBpp = 4;
	long _pitch = m_iBpp * _width;
	long m_iPitch = m_iBpp * src.width;

	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	fixed f_2 = itofx(2);
	fixed f_4 = itofx(4);
	fixed f_6 = itofx(6);
	fixed f_gama = ftofx(1.04f);

	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	unsigned int dwDstTotalOffset;
	unsigned int* lpSrcData = (unsigned int*)src.pBits;
	unsigned int dwSrcTotalOffset;
	unsigned int* lpDstData = (unsigned int*)lpData;

	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
			unsigned long dwSrcOffsets[16];
			dwSrcOffsets[0] = dwSrcTotalOffset - m_iPitch - m_iBpp;
			if ((m < 1) || (n < 1))
				dwSrcOffsets[0] = dwSrcTotalOffset;
			dwSrcOffsets[1] = dwSrcTotalOffset - m_iPitch;
			if (m < 1)
				dwSrcOffsets[1] = dwSrcTotalOffset;
			dwSrcOffsets[2] = dwSrcTotalOffset - m_iPitch + m_iBpp;
			if ((m < 1) || (n >= src.width-1))
				dwSrcOffsets[2] = dwSrcTotalOffset;
			dwSrcOffsets[3] = dwSrcTotalOffset - m_iPitch + m_iBpp + m_iBpp;
			if ((m < 1) || (n >= src.width-2))
				dwSrcOffsets[3] = dwSrcTotalOffset;
			dwSrcOffsets[4] = dwSrcTotalOffset - m_iBpp;
			if (n < 1)
				dwSrcOffsets[4] = dwSrcTotalOffset;
			dwSrcOffsets[5] = dwSrcTotalOffset;
			dwSrcOffsets[6] = dwSrcTotalOffset + m_iBpp;
			if (n >= src.width-1)
				dwSrcOffsets[6] = dwSrcTotalOffset;
			dwSrcOffsets[7] = dwSrcTotalOffset + m_iBpp + m_iBpp;
			if (n >= src.width-2)
				dwSrcOffsets[7] = dwSrcTotalOffset;
			dwSrcOffsets[8] = dwSrcTotalOffset + m_iPitch - m_iBpp;
			if ((m >= src.height-1) || (n < 1))
				dwSrcOffsets[8] = dwSrcTotalOffset;
			dwSrcOffsets[9] = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcOffsets[9] = dwSrcTotalOffset;
			dwSrcOffsets[10] = dwSrcTotalOffset + m_iPitch + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-1))
				dwSrcOffsets[10] = dwSrcTotalOffset;
			dwSrcOffsets[11] = dwSrcTotalOffset + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-2))
				dwSrcOffsets[11] = dwSrcTotalOffset;
			dwSrcOffsets[12] = dwSrcTotalOffset + m_iPitch + m_iPitch - m_iBpp;
			if ((m >= src.height-2) || (n < 1))
				dwSrcOffsets[12] = dwSrcTotalOffset;
			dwSrcOffsets[13] = dwSrcTotalOffset + m_iPitch + m_iPitch;
			if (m >= src.height-2)
				dwSrcOffsets[13] = dwSrcTotalOffset;
			dwSrcOffsets[14] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-1))
				dwSrcOffsets[14] = dwSrcTotalOffset;
			dwSrcOffsets[15] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-2))
				dwSrcOffsets[15] = dwSrcTotalOffset;
			fixed f_red=0, f_green=0, f_blue=0, f_alpha=0;
			for (long k=-1; k<3; k++)
			{
				fixed f = itofx(k)-f_f;
				fixed f_fm1 = f - f_1;
				fixed f_fp1 = f + f_1;
				fixed f_fp2 = f + f_2;
				fixed f_a = 0;
				if (f_fp2 > 0)
					f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
				fixed f_b = 0;
				if (f_fp1 > 0)
					f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
				fixed f_c = 0;
				if (f > 0)
					f_c = Mulfx(f,Mulfx(f,f));
				fixed f_d = 0;
				if (f_fm1 > 0)
					f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
				fixed f_RY = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
				for (long l=-1; l<3; l++)
				{
					fixed f = itofx(l)-f_g;
					fixed f_fm1 = f - f_1;
					fixed f_fp1 = f + f_1;
					fixed f_fp2 = f + f_2;
					fixed f_a = 0;
					if (f_fp2 > 0)
						f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
					fixed f_b = 0;
					if (f_fp1 > 0)
						f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
					fixed f_c = 0;
					if (f > 0)
						f_c = Mulfx(f,Mulfx(f,f));
					fixed f_d = 0;
					if (f_fm1 > 0)
						f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
					fixed f_RX = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
					fixed f_R = Mulfx(f_RY,f_RX);
					long _k = ((k+1)*4) + (l+1);
					fixed f_rs = itofx(_GetRValue(lpSrcData[dwSrcOffsets[_k]>>2]));
					fixed f_gs = itofx(_GetGValue(lpSrcData[dwSrcOffsets[_k]>>2]));
					fixed f_bs = itofx(_GetBValue(lpSrcData[dwSrcOffsets[_k]>>2]));
					fixed f_as = itofx(_GetAValue(lpSrcData[dwSrcOffsets[_k]>>2]));
					f_red += Mulfx(f_rs,f_R);
					f_green += Mulfx(f_gs,f_R);
					f_blue += Mulfx(f_bs,f_R);
					f_alpha += Mulfx(f_as,f_R);
				}
			}
			unsigned char red = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_red,f_gama))));
			unsigned char green = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_green,f_gama))));
			unsigned char blue = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_blue,f_gama))));
			unsigned char alpha = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_alpha,f_gama))));
			lpDstData[dwDstTotalOffset>>2] = ZMAEE_GET_RGBA(red, green, blue,alpha);

			// Update destination horizontal offset
			dwDstHorizontalOffset += m_iBpp;
		}

		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}


void _ScaleBilinear32(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	long m_iBpp = 4;
	long _pitch = m_iBpp * _width;
	long m_iPitch = m_iBpp * src.width;
	

	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	
	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	unsigned int dwDstTotalOffset;
	unsigned int* lpSrcData = (unsigned int*)src.pBits;
	unsigned int dwSrcTotalOffset;
	unsigned int* lpDstData = (unsigned int*)lpData;
	
	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;
			
			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
			unsigned int dwSrcTopLeft = dwSrcTotalOffset;
			unsigned int dwSrcTopRight = dwSrcTotalOffset + m_iBpp;
			if (n >= src.width-1)
				dwSrcTopRight = dwSrcTotalOffset;
			unsigned int dwSrcBottomLeft = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcBottomLeft = dwSrcTotalOffset;
			unsigned int dwSrcBottomRight = dwSrcTotalOffset + m_iPitch + m_iBpp;
			if ((n >= src.width-1) || (m >= src.height-1))
				dwSrcBottomRight = dwSrcTotalOffset;
			fixed f_w1 = Mulfx(f_1-f_f, f_1-f_g);
			fixed f_w2 = Mulfx(f_1-f_f, f_g);
			fixed f_w3 = Mulfx(f_f, f_1-f_g);
			fixed f_w4 = Mulfx(f_f, f_g);
			_PIXEL pixel1 = lpSrcData[dwSrcTopLeft>>2];
			_PIXEL pixel2 = lpSrcData[dwSrcTopRight>>2];
			_PIXEL pixel3 = lpSrcData[dwSrcBottomLeft>>2];
			_PIXEL pixel4 = lpSrcData[dwSrcBottomRight>>2];
			
			fixed f_r1 = itofx(_GetRValue(pixel1));
			fixed f_r2 = itofx(_GetRValue(pixel2));
			fixed f_r3 = itofx(_GetRValue(pixel3));
			fixed f_r4 = itofx(_GetRValue(pixel4));
			
			fixed f_g1 = itofx(_GetGValue(pixel1));
			fixed f_g2 = itofx(_GetGValue(pixel2));
			fixed f_g3 = itofx(_GetGValue(pixel3));
			fixed f_g4 = itofx(_GetGValue(pixel4));
			
			fixed f_b1 = itofx(_GetBValue(pixel1));
			fixed f_b2 = itofx(_GetBValue(pixel2));
			fixed f_b3 = itofx(_GetBValue(pixel3));
			fixed f_b4 = itofx(_GetBValue(pixel4));
			
			fixed f_a1 = itofx(_GetAValue(pixel1));
			fixed f_a2 = itofx(_GetAValue(pixel2));
			fixed f_a3 = itofx(_GetAValue(pixel3));
			fixed f_a4 = itofx(_GetAValue(pixel4));

			int red = fxtoi(Mulfx(f_w1, f_r1) + Mulfx(f_w2, f_r2) + Mulfx(f_w3, f_r3) + Mulfx(f_w4, f_r4));
			int green = fxtoi(Mulfx(f_w1, f_g1) + Mulfx(f_w2, f_g2) + Mulfx(f_w3, f_g3) + Mulfx(f_w4, f_g4));
			int blue = fxtoi(Mulfx(f_w1, f_b1) + Mulfx(f_w2, f_b2) + Mulfx(f_w3, f_b3) + Mulfx(f_w4, f_b4));
			int alpha = fxtoi(Mulfx(f_w1, f_a1) + Mulfx(f_w2, f_a2) + Mulfx(f_w3, f_a3) + Mulfx(f_w4, f_a4));
			lpDstData[dwDstTotalOffset>>2] = ZMAEE_GET_RGBA(red, green, blue, alpha);
			
			// Update destination horizontal offset
			dwDstHorizontalOffset += m_iBpp;
		}
		
		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}

void _ScaleBicubic16_16(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	long m_iBpp = 2;
	long _pitch = m_iBpp * _width;
	long m_iPitch = m_iBpp * src.width;

	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	fixed f_2 = itofx(2);
	fixed f_4 = itofx(4);
	fixed f_6 = itofx(6);
	fixed f_gama = ftofx(1.04f);

	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	unsigned int dwDstTotalOffset;
	unsigned short* lpSrcData = (unsigned short*)src.pBits;
	unsigned int dwSrcTotalOffset;
	unsigned short* lpDstData = (unsigned short*)lpData;

	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
			unsigned long dwSrcOffsets[16];
			dwSrcOffsets[0] = dwSrcTotalOffset - m_iPitch - m_iBpp;
			if ((m < 1) || (n < 1))
				dwSrcOffsets[0] = dwSrcTotalOffset;
			dwSrcOffsets[1] = dwSrcTotalOffset - m_iPitch;
			if (m < 1)
				dwSrcOffsets[1] = dwSrcTotalOffset;
			dwSrcOffsets[2] = dwSrcTotalOffset - m_iPitch + m_iBpp;
			if ((m < 1) || (n >= src.width-1))
				dwSrcOffsets[2] = dwSrcTotalOffset;
			dwSrcOffsets[3] = dwSrcTotalOffset - m_iPitch + m_iBpp + m_iBpp;
			if ((m < 1) || (n >= src.width-2))
				dwSrcOffsets[3] = dwSrcTotalOffset;
			dwSrcOffsets[4] = dwSrcTotalOffset - m_iBpp;
			if (n < 1)
				dwSrcOffsets[4] = dwSrcTotalOffset;
			dwSrcOffsets[5] = dwSrcTotalOffset;
			dwSrcOffsets[6] = dwSrcTotalOffset + m_iBpp;
			if (n >= src.width-1)
				dwSrcOffsets[6] = dwSrcTotalOffset;
			dwSrcOffsets[7] = dwSrcTotalOffset + m_iBpp + m_iBpp;
			if (n >= src.width-2)
				dwSrcOffsets[7] = dwSrcTotalOffset;
			dwSrcOffsets[8] = dwSrcTotalOffset + m_iPitch - m_iBpp;
			if ((m >= src.height-1) || (n < 1))
				dwSrcOffsets[8] = dwSrcTotalOffset;
			dwSrcOffsets[9] = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcOffsets[9] = dwSrcTotalOffset;
			dwSrcOffsets[10] = dwSrcTotalOffset + m_iPitch + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-1))
				dwSrcOffsets[10] = dwSrcTotalOffset;
			dwSrcOffsets[11] = dwSrcTotalOffset + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-2))
				dwSrcOffsets[11] = dwSrcTotalOffset;
			dwSrcOffsets[12] = dwSrcTotalOffset + m_iPitch + m_iPitch - m_iBpp;
			if ((m >= src.height-2) || (n < 1))
				dwSrcOffsets[12] = dwSrcTotalOffset;
			dwSrcOffsets[13] = dwSrcTotalOffset + m_iPitch + m_iPitch;
			if (m >= src.height-2)
				dwSrcOffsets[13] = dwSrcTotalOffset;
			dwSrcOffsets[14] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-1))
				dwSrcOffsets[14] = dwSrcTotalOffset;
			dwSrcOffsets[15] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-2))
				dwSrcOffsets[15] = dwSrcTotalOffset;
			fixed f_red=0, f_green=0, f_blue=0;
			for (long k=-1; k<3; k++)
			{
				fixed f = itofx(k)-f_f;
				fixed f_fm1 = f - f_1;
				fixed f_fp1 = f + f_1;
				fixed f_fp2 = f + f_2;
				fixed f_a = 0;
				if (f_fp2 > 0)
					f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
				fixed f_b = 0;
				if (f_fp1 > 0)
					f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
				fixed f_c = 0;
				if (f > 0)
					f_c = Mulfx(f,Mulfx(f,f));
				fixed f_d = 0;
				if (f_fm1 > 0)
					f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
				fixed f_RY = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
				for (long l=-1; l<3; l++)
				{
					fixed f = itofx(l)-f_g;
					fixed f_fm1 = f - f_1;
					fixed f_fp1 = f + f_1;
					fixed f_fp2 = f + f_2;
					fixed f_a = 0;
					if (f_fp2 > 0)
						f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
					fixed f_b = 0;
					if (f_fp1 > 0)
						f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
					fixed f_c = 0;
					if (f > 0)
						f_c = Mulfx(f,Mulfx(f,f));
					fixed f_d = 0;
					if (f_fm1 > 0)
						f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
					fixed f_RX = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
					fixed f_R = Mulfx(f_RY,f_RX);
					long _k = ((k+1)*4) + (l+1);

					_PIXEL pixel1 = _BMP16TO32(lpSrcData[dwSrcOffsets[_k]>>1]);

					fixed f_rs = itofx(_GetRValue(pixel1));
					fixed f_gs = itofx(_GetGValue(pixel1));
					fixed f_bs = itofx(_GetBValue(pixel1));
					f_red += Mulfx(f_rs,f_R);
					f_green += Mulfx(f_gs,f_R);
					f_blue += Mulfx(f_bs,f_R);
				}
			}
			unsigned char red = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_red,f_gama))));
			unsigned char green = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_green,f_gama))));
			unsigned char blue = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_blue,f_gama))));
			lpDstData[dwDstTotalOffset>>1] = ZMAEE_BMP24TO16(red, green, blue);

			// Update destination horizontal offset
			dwDstHorizontalOffset += m_iBpp;
		}

		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}


void _ScaleBilinear16_16(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	long m_iBpp = 2;
	long _pitch = m_iBpp * _width;
	long m_iPitch = m_iBpp * src.width;
	
	
	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	
	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;

	unsigned int dwDstTotalOffset;
	unsigned short* lpSrcData = (unsigned short*)src.pBits;

	unsigned int dwSrcTotalOffset;
	unsigned short* lpDstData = (unsigned short*)lpData;
	
	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;
			
			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
			unsigned int dwSrcTopLeft = dwSrcTotalOffset;
			unsigned int dwSrcTopRight = dwSrcTotalOffset + m_iBpp;
			if (n >= src.width-1)
				dwSrcTopRight = dwSrcTotalOffset;
			unsigned int dwSrcBottomLeft = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcBottomLeft = dwSrcTotalOffset;
			unsigned int dwSrcBottomRight = dwSrcTotalOffset + m_iPitch + m_iBpp;
			if ((n >= src.width-1) || (m >= src.height-1))
				dwSrcBottomRight = dwSrcTotalOffset;
			fixed f_w1 = Mulfx(f_1-f_f, f_1-f_g);
			fixed f_w2 = Mulfx(f_1-f_f, f_g);
			fixed f_w3 = Mulfx(f_f, f_1-f_g);
			fixed f_w4 = Mulfx(f_f, f_g);

			_PIXEL pixel1 = _BMP16TO32(lpSrcData[dwSrcTopLeft>>1]);
			_PIXEL pixel2 = _BMP16TO32(lpSrcData[dwSrcTopRight>>1]);
			_PIXEL pixel3 = _BMP16TO32(lpSrcData[dwSrcBottomLeft>>1]);
			_PIXEL pixel4 = _BMP16TO32(lpSrcData[dwSrcBottomRight>>1]);
			
			fixed f_r1 = itofx(_GetRValue(pixel1));
			fixed f_r2 = itofx(_GetRValue(pixel2));
			fixed f_r3 = itofx(_GetRValue(pixel3));
			fixed f_r4 = itofx(_GetRValue(pixel4));
			
			fixed f_g1 = itofx(_GetGValue(pixel1));
			fixed f_g2 = itofx(_GetGValue(pixel2));
			fixed f_g3 = itofx(_GetGValue(pixel3));
			fixed f_g4 = itofx(_GetGValue(pixel4));
			
			fixed f_b1 = itofx(_GetBValue(pixel1));
			fixed f_b2 = itofx(_GetBValue(pixel2));
			fixed f_b3 = itofx(_GetBValue(pixel3));
			fixed f_b4 = itofx(_GetBValue(pixel4));
			
			int red = fxtoi(Mulfx(f_w1, f_r1) + Mulfx(f_w2, f_r2) + Mulfx(f_w3, f_r3) + Mulfx(f_w4, f_r4));
			int green = fxtoi(Mulfx(f_w1, f_g1) + Mulfx(f_w2, f_g2) + Mulfx(f_w3, f_g3) + Mulfx(f_w4, f_g4));
			int blue = fxtoi(Mulfx(f_w1, f_b1) + Mulfx(f_w2, f_b2) + Mulfx(f_w3, f_b3) + Mulfx(f_w4, f_b4));
			lpDstData[dwDstTotalOffset>>1] = ZMAEE_BMP24TO16(red, green, blue);
			
			// Update destination horizontal offset
			dwDstHorizontalOffset += m_iBpp;
		}
		
		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}

void _ScaleBicubic16_32(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	long m_iBpp = 2;
	long _pitch = 4 * _width;
	long m_iPitch = m_iBpp * src.width;

	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	fixed f_2 = itofx(2);
	fixed f_4 = itofx(4);
	fixed f_6 = itofx(6);
	fixed f_gama = ftofx(1.04f);

	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	unsigned int dwDstTotalOffset;
	unsigned short* lpSrcData = (unsigned short*)src.pBits;
	unsigned int dwSrcTotalOffset;
	unsigned int* lpDstData = (unsigned int*)lpData;

	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
			unsigned long dwSrcOffsets[16];
			dwSrcOffsets[0] = dwSrcTotalOffset - m_iPitch - m_iBpp;
			if ((m < 1) || (n < 1))
				dwSrcOffsets[0] = dwSrcTotalOffset;
			dwSrcOffsets[1] = dwSrcTotalOffset - m_iPitch;
			if (m < 1)
				dwSrcOffsets[1] = dwSrcTotalOffset;
			dwSrcOffsets[2] = dwSrcTotalOffset - m_iPitch + m_iBpp;
			if ((m < 1) || (n >= src.width-1))
				dwSrcOffsets[2] = dwSrcTotalOffset;
			dwSrcOffsets[3] = dwSrcTotalOffset - m_iPitch + m_iBpp + m_iBpp;
			if ((m < 1) || (n >= src.width-2))
				dwSrcOffsets[3] = dwSrcTotalOffset;
			dwSrcOffsets[4] = dwSrcTotalOffset - m_iBpp;
			if (n < 1)
				dwSrcOffsets[4] = dwSrcTotalOffset;
			dwSrcOffsets[5] = dwSrcTotalOffset;
			dwSrcOffsets[6] = dwSrcTotalOffset + m_iBpp;
			if (n >= src.width-1)
				dwSrcOffsets[6] = dwSrcTotalOffset;
			dwSrcOffsets[7] = dwSrcTotalOffset + m_iBpp + m_iBpp;
			if (n >= src.width-2)
				dwSrcOffsets[7] = dwSrcTotalOffset;
			dwSrcOffsets[8] = dwSrcTotalOffset + m_iPitch - m_iBpp;
			if ((m >= src.height-1) || (n < 1))
				dwSrcOffsets[8] = dwSrcTotalOffset;
			dwSrcOffsets[9] = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcOffsets[9] = dwSrcTotalOffset;
			dwSrcOffsets[10] = dwSrcTotalOffset + m_iPitch + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-1))
				dwSrcOffsets[10] = dwSrcTotalOffset;
			dwSrcOffsets[11] = dwSrcTotalOffset + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-2))
				dwSrcOffsets[11] = dwSrcTotalOffset;
			dwSrcOffsets[12] = dwSrcTotalOffset + m_iPitch + m_iPitch - m_iBpp;
			if ((m >= src.height-2) || (n < 1))
				dwSrcOffsets[12] = dwSrcTotalOffset;
			dwSrcOffsets[13] = dwSrcTotalOffset + m_iPitch + m_iPitch;
			if (m >= src.height-2)
				dwSrcOffsets[13] = dwSrcTotalOffset;
			dwSrcOffsets[14] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-1))
				dwSrcOffsets[14] = dwSrcTotalOffset;
			dwSrcOffsets[15] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-2))
				dwSrcOffsets[15] = dwSrcTotalOffset;
			fixed f_red=0, f_green=0, f_blue=0;
			for (long k=-1; k<3; k++)
			{
				fixed f = itofx(k)-f_f;
				fixed f_fm1 = f - f_1;
				fixed f_fp1 = f + f_1;
				fixed f_fp2 = f + f_2;
				fixed f_a = 0;
				if (f_fp2 > 0)
					f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
				fixed f_b = 0;
				if (f_fp1 > 0)
					f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
				fixed f_c = 0;
				if (f > 0)
					f_c = Mulfx(f,Mulfx(f,f));
				fixed f_d = 0;
				if (f_fm1 > 0)
					f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
				fixed f_RY = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
				for (long l=-1; l<3; l++)
				{
					fixed f = itofx(l)-f_g;
					fixed f_fm1 = f - f_1;
					fixed f_fp1 = f + f_1;
					fixed f_fp2 = f + f_2;
					fixed f_a = 0;
					if (f_fp2 > 0)
						f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
					fixed f_b = 0;
					if (f_fp1 > 0)
						f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
					fixed f_c = 0;
					if (f > 0)
						f_c = Mulfx(f,Mulfx(f,f));
					fixed f_d = 0;
					if (f_fm1 > 0)
						f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
					fixed f_RX = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
					fixed f_R = Mulfx(f_RY,f_RX);
					long _k = ((k+1)*4) + (l+1);

					_PIXEL pixel1 = _BMP16TO32(lpSrcData[dwSrcOffsets[_k]>>1]);

					fixed f_rs = itofx(_GetRValue(pixel1));
					fixed f_gs = itofx(_GetGValue(pixel1));
					fixed f_bs = itofx(_GetBValue(pixel1));
					f_red += Mulfx(f_rs,f_R);
					f_green += Mulfx(f_gs,f_R);
					f_blue += Mulfx(f_bs,f_R);
				}
			}
			unsigned char red = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_red,f_gama))));
			unsigned char green = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_green,f_gama))));
			unsigned char blue = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_blue,f_gama))));
			lpDstData[dwDstTotalOffset>>2] = ZMAEE_GET_RGB(red, green, blue);

			// Update destination horizontal offset
			dwDstHorizontalOffset += 4;
		}

		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}

void _ScaleBilinear16_32(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	long m_iBpp = 2;
	long _pitch = 4 * _width;
	long m_iPitch = m_iBpp * src.width;
	
	
	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	
	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	
	unsigned int dwDstTotalOffset;
	unsigned short* lpSrcData = (unsigned short*)src.pBits;
	
	unsigned int dwSrcTotalOffset;
	unsigned int* lpDstData = (unsigned int*)lpData;
	
	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;
			
			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
			unsigned int dwSrcTopLeft = dwSrcTotalOffset;
			unsigned int dwSrcTopRight = dwSrcTotalOffset + m_iBpp;
			if (n >= src.width-1)
				dwSrcTopRight = dwSrcTotalOffset;
			unsigned int dwSrcBottomLeft = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcBottomLeft = dwSrcTotalOffset;
			unsigned int dwSrcBottomRight = dwSrcTotalOffset + m_iPitch + m_iBpp;
			if ((n >= src.width-1) || (m >= src.height-1))
				dwSrcBottomRight = dwSrcTotalOffset;
			fixed f_w1 = Mulfx(f_1-f_f, f_1-f_g);
			fixed f_w2 = Mulfx(f_1-f_f, f_g);
			fixed f_w3 = Mulfx(f_f, f_1-f_g);
			fixed f_w4 = Mulfx(f_f, f_g);
			
			_PIXEL pixel1 = _BMP16TO32(lpSrcData[dwSrcTopLeft>>1]);
			_PIXEL pixel2 = _BMP16TO32(lpSrcData[dwSrcTopRight>>1]);
			_PIXEL pixel3 = _BMP16TO32(lpSrcData[dwSrcBottomLeft>>1]);
			_PIXEL pixel4 = _BMP16TO32(lpSrcData[dwSrcBottomRight>>1]);
			
			fixed f_r1 = itofx(_GetRValue(pixel1));
			fixed f_r2 = itofx(_GetRValue(pixel2));
			fixed f_r3 = itofx(_GetRValue(pixel3));
			fixed f_r4 = itofx(_GetRValue(pixel4));
			
			fixed f_g1 = itofx(_GetGValue(pixel1));
			fixed f_g2 = itofx(_GetGValue(pixel2));
			fixed f_g3 = itofx(_GetGValue(pixel3));
			fixed f_g4 = itofx(_GetGValue(pixel4));
			
			fixed f_b1 = itofx(_GetBValue(pixel1));
			fixed f_b2 = itofx(_GetBValue(pixel2));
			fixed f_b3 = itofx(_GetBValue(pixel3));
			fixed f_b4 = itofx(_GetBValue(pixel4));
			
			int red = fxtoi(Mulfx(f_w1, f_r1) + Mulfx(f_w2, f_r2) + Mulfx(f_w3, f_r3) + Mulfx(f_w4, f_r4));
			int green = fxtoi(Mulfx(f_w1, f_g1) + Mulfx(f_w2, f_g2) + Mulfx(f_w3, f_g3) + Mulfx(f_w4, f_g4));
			int blue = fxtoi(Mulfx(f_w1, f_b1) + Mulfx(f_w2, f_b2) + Mulfx(f_w3, f_b3) + Mulfx(f_w4, f_b4));
			lpDstData[dwDstTotalOffset>>2] = ZMAEE_GET_RGB(red, green, blue);
			
			// Update destination horizontal offset
			dwDstHorizontalOffset += 4;
		}
		
		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}


void _ScaleBicubic32_16(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	long m_iBpp = 4;
	long _pitch = 2 * _width;
	long m_iPitch = m_iBpp * src.width;

	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	fixed f_2 = itofx(2);
	fixed f_4 = itofx(4);
	fixed f_6 = itofx(6);
	fixed f_gama = ftofx(1.04f);

	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	unsigned int dwDstTotalOffset;
	unsigned int* lpSrcData = (unsigned int*)src.pBits;
	unsigned int dwSrcTotalOffset;
	unsigned short* lpDstData = (unsigned short*)lpData;

	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
			unsigned long dwSrcOffsets[16];
			dwSrcOffsets[0] = dwSrcTotalOffset - m_iPitch - m_iBpp;
			if ((m < 1) || (n < 1))
				dwSrcOffsets[0] = dwSrcTotalOffset;
			dwSrcOffsets[1] = dwSrcTotalOffset - m_iPitch;
			if (m < 1)
				dwSrcOffsets[1] = dwSrcTotalOffset;
			dwSrcOffsets[2] = dwSrcTotalOffset - m_iPitch + m_iBpp;
			if ((m < 1) || (n >= src.width-1))
				dwSrcOffsets[2] = dwSrcTotalOffset;
			dwSrcOffsets[3] = dwSrcTotalOffset - m_iPitch + m_iBpp + m_iBpp;
			if ((m < 1) || (n >= src.width-2))
				dwSrcOffsets[3] = dwSrcTotalOffset;
			dwSrcOffsets[4] = dwSrcTotalOffset - m_iBpp;
			if (n < 1)
				dwSrcOffsets[4] = dwSrcTotalOffset;
			dwSrcOffsets[5] = dwSrcTotalOffset;
			dwSrcOffsets[6] = dwSrcTotalOffset + m_iBpp;
			if (n >= src.width-1)
				dwSrcOffsets[6] = dwSrcTotalOffset;
			dwSrcOffsets[7] = dwSrcTotalOffset + m_iBpp + m_iBpp;
			if (n >= src.width-2)
				dwSrcOffsets[7] = dwSrcTotalOffset;
			dwSrcOffsets[8] = dwSrcTotalOffset + m_iPitch - m_iBpp;
			if ((m >= src.height-1) || (n < 1))
				dwSrcOffsets[8] = dwSrcTotalOffset;
			dwSrcOffsets[9] = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcOffsets[9] = dwSrcTotalOffset;
			dwSrcOffsets[10] = dwSrcTotalOffset + m_iPitch + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-1))
				dwSrcOffsets[10] = dwSrcTotalOffset;
			dwSrcOffsets[11] = dwSrcTotalOffset + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-2))
				dwSrcOffsets[11] = dwSrcTotalOffset;
			dwSrcOffsets[12] = dwSrcTotalOffset + m_iPitch + m_iPitch - m_iBpp;
			if ((m >= src.height-2) || (n < 1))
				dwSrcOffsets[12] = dwSrcTotalOffset;
			dwSrcOffsets[13] = dwSrcTotalOffset + m_iPitch + m_iPitch;
			if (m >= src.height-2)
				dwSrcOffsets[13] = dwSrcTotalOffset;
			dwSrcOffsets[14] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-1))
				dwSrcOffsets[14] = dwSrcTotalOffset;
			dwSrcOffsets[15] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-2))
				dwSrcOffsets[15] = dwSrcTotalOffset;
			fixed f_red=0, f_green=0, f_blue=0;
			for (long k=-1; k<3; k++)
			{
				fixed f = itofx(k)-f_f;
				fixed f_fm1 = f - f_1;
				fixed f_fp1 = f + f_1;
				fixed f_fp2 = f + f_2;
				fixed f_a = 0;
				if (f_fp2 > 0)
					f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
				fixed f_b = 0;
				if (f_fp1 > 0)
					f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
				fixed f_c = 0;
				if (f > 0)
					f_c = Mulfx(f,Mulfx(f,f));
				fixed f_d = 0;
				if (f_fm1 > 0)
					f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
				fixed f_RY = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
				for (long l=-1; l<3; l++)
				{
					fixed f = itofx(l)-f_g;
					fixed f_fm1 = f - f_1;
					fixed f_fp1 = f + f_1;
					fixed f_fp2 = f + f_2;
					fixed f_a = 0;
					if (f_fp2 > 0)
						f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
					fixed f_b = 0;
					if (f_fp1 > 0)
						f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
					fixed f_c = 0;
					if (f > 0)
						f_c = Mulfx(f,Mulfx(f,f));
					fixed f_d = 0;
					if (f_fm1 > 0)
						f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
					fixed f_RX = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
					fixed f_R = Mulfx(f_RY,f_RX);
					long _k = ((k+1)*4) + (l+1);

					_PIXEL pixel1 = lpSrcData[dwSrcOffsets[_k]>>2];

					fixed f_rs = itofx(_GetRValue(pixel1));
					fixed f_gs = itofx(_GetGValue(pixel1));
					fixed f_bs = itofx(_GetBValue(pixel1));
					f_red += Mulfx(f_rs,f_R);
					f_green += Mulfx(f_gs,f_R);
					f_blue += Mulfx(f_bs,f_R);
				}
			}
			unsigned char red = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_red,f_gama))));
			unsigned char green = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_green,f_gama))));
			unsigned char blue = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_blue,f_gama))));
			lpDstData[dwDstTotalOffset>>1] = ZMAEE_BMP24TO16(red, green, blue);

			// Update destination horizontal offset
			dwDstHorizontalOffset += 2;
		}

		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}

void _ScaleBilinear32_16(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	long m_iBpp = 4;
	long _pitch = 2 * _width;
	long m_iPitch = m_iBpp * src.width;
	
	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	
	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	
	unsigned int dwDstTotalOffset;
	unsigned int* lpSrcData = (unsigned int*)src.pBits;
	
	unsigned int dwSrcTotalOffset;
	unsigned short* lpDstData = (unsigned short*)lpData;
	
	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;
			
			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
			unsigned int dwSrcTopLeft = dwSrcTotalOffset;
			unsigned int dwSrcTopRight = dwSrcTotalOffset + m_iBpp;
			if (n >= src.width-1)
				dwSrcTopRight = dwSrcTotalOffset;
			unsigned int dwSrcBottomLeft = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcBottomLeft = dwSrcTotalOffset;
			unsigned int dwSrcBottomRight = dwSrcTotalOffset + m_iPitch + m_iBpp;
			if ((n >= src.width-1) || (m >= src.height-1))
				dwSrcBottomRight = dwSrcTotalOffset;
			fixed f_w1 = Mulfx(f_1-f_f, f_1-f_g);
			fixed f_w2 = Mulfx(f_1-f_f, f_g);
			fixed f_w3 = Mulfx(f_f, f_1-f_g);
			fixed f_w4 = Mulfx(f_f, f_g);
			
			_PIXEL pixel1 = lpSrcData[dwSrcTopLeft>>2];
			_PIXEL pixel2 = lpSrcData[dwSrcTopRight>>2];
			_PIXEL pixel3 = lpSrcData[dwSrcBottomLeft>>2];
			_PIXEL pixel4 = lpSrcData[dwSrcBottomRight>>2];
			
			fixed f_r1 = itofx(_GetRValue(pixel1));
			fixed f_r2 = itofx(_GetRValue(pixel2));
			fixed f_r3 = itofx(_GetRValue(pixel3));
			fixed f_r4 = itofx(_GetRValue(pixel4));
			
			fixed f_g1 = itofx(_GetGValue(pixel1));
			fixed f_g2 = itofx(_GetGValue(pixel2));
			fixed f_g3 = itofx(_GetGValue(pixel3));
			fixed f_g4 = itofx(_GetGValue(pixel4));
			
			fixed f_b1 = itofx(_GetBValue(pixel1));
			fixed f_b2 = itofx(_GetBValue(pixel2));
			fixed f_b3 = itofx(_GetBValue(pixel3));
			fixed f_b4 = itofx(_GetBValue(pixel4));
			
			int red = fxtoi(Mulfx(f_w1, f_r1) + Mulfx(f_w2, f_r2) + Mulfx(f_w3, f_r3) + Mulfx(f_w4, f_r4));
			int green = fxtoi(Mulfx(f_w1, f_g1) + Mulfx(f_w2, f_g2) + Mulfx(f_w3, f_g3) + Mulfx(f_w4, f_g4));
			int blue = fxtoi(Mulfx(f_w1, f_b1) + Mulfx(f_w2, f_b2) + Mulfx(f_w3, f_b3) + Mulfx(f_w4, f_b4));
			lpDstData[dwDstTotalOffset>>1] = ZMAEE_BMP24TO16(red, green, blue);
			
			// Update destination horizontal offset
			dwDstHorizontalOffset += 2;
		}
		
		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}

void _ScaleBicubic8_32(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	long m_iBpp = 1;
	long _pitch = 4*_width;
	long m_iPitch = m_iBpp * src.width;

	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	fixed f_2 = itofx(2);
	fixed f_4 = itofx(4);
	fixed f_6 = itofx(6);
	fixed f_gama = ftofx(1.04f);

	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	unsigned int dwDstTotalOffset;
	unsigned char* lpSrcData = (unsigned char*)src.pBits;
	unsigned int dwSrcTotalOffset;
	unsigned int* lpDstData = (unsigned int*)lpData;
	
	unsigned short* palette = (unsigned short*)src.pPalette;

	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
			unsigned long dwSrcOffsets[16];
			dwSrcOffsets[0] = dwSrcTotalOffset - m_iPitch - m_iBpp;
			if ((m < 1) || (n < 1))
				dwSrcOffsets[0] = dwSrcTotalOffset;
			dwSrcOffsets[1] = dwSrcTotalOffset - m_iPitch;
			if (m < 1)
				dwSrcOffsets[1] = dwSrcTotalOffset;
			dwSrcOffsets[2] = dwSrcTotalOffset - m_iPitch + m_iBpp;
			if ((m < 1) || (n >= src.width-1))
				dwSrcOffsets[2] = dwSrcTotalOffset;
			dwSrcOffsets[3] = dwSrcTotalOffset - m_iPitch + m_iBpp + m_iBpp;
			if ((m < 1) || (n >= src.width-2))
				dwSrcOffsets[3] = dwSrcTotalOffset;
			dwSrcOffsets[4] = dwSrcTotalOffset - m_iBpp;
			if (n < 1)
				dwSrcOffsets[4] = dwSrcTotalOffset;
			dwSrcOffsets[5] = dwSrcTotalOffset;
			dwSrcOffsets[6] = dwSrcTotalOffset + m_iBpp;
			if (n >= src.width-1)
				dwSrcOffsets[6] = dwSrcTotalOffset;
			dwSrcOffsets[7] = dwSrcTotalOffset + m_iBpp + m_iBpp;
			if (n >= src.width-2)
				dwSrcOffsets[7] = dwSrcTotalOffset;
			dwSrcOffsets[8] = dwSrcTotalOffset + m_iPitch - m_iBpp;
			if ((m >= src.height-1) || (n < 1))
				dwSrcOffsets[8] = dwSrcTotalOffset;
			dwSrcOffsets[9] = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcOffsets[9] = dwSrcTotalOffset;
			dwSrcOffsets[10] = dwSrcTotalOffset + m_iPitch + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-1))
				dwSrcOffsets[10] = dwSrcTotalOffset;
			dwSrcOffsets[11] = dwSrcTotalOffset + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-2))
				dwSrcOffsets[11] = dwSrcTotalOffset;
			dwSrcOffsets[12] = dwSrcTotalOffset + m_iPitch + m_iPitch - m_iBpp;
			if ((m >= src.height-2) || (n < 1))
				dwSrcOffsets[12] = dwSrcTotalOffset;
			dwSrcOffsets[13] = dwSrcTotalOffset + m_iPitch + m_iPitch;
			if (m >= src.height-2)
				dwSrcOffsets[13] = dwSrcTotalOffset;
			dwSrcOffsets[14] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-1))
				dwSrcOffsets[14] = dwSrcTotalOffset;
			dwSrcOffsets[15] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-2))
				dwSrcOffsets[15] = dwSrcTotalOffset;
			fixed f_red=0, f_green=0, f_blue=0;
			for (long k=-1; k<3; k++)
			{
				fixed f = itofx(k)-f_f;
				fixed f_fm1 = f - f_1;
				fixed f_fp1 = f + f_1;
				fixed f_fp2 = f + f_2;
				fixed f_a = 0;
				if (f_fp2 > 0)
					f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
				fixed f_b = 0;
				if (f_fp1 > 0)
					f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
				fixed f_c = 0;
				if (f > 0)
					f_c = Mulfx(f,Mulfx(f,f));
				fixed f_d = 0;
				if (f_fm1 > 0)
					f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
				fixed f_RY = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
				for (long l=-1; l<3; l++)
				{
					fixed f = itofx(l)-f_g;
					fixed f_fm1 = f - f_1;
					fixed f_fp1 = f + f_1;
					fixed f_fp2 = f + f_2;
					fixed f_a = 0;
					if (f_fp2 > 0)
						f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
					fixed f_b = 0;
					if (f_fp1 > 0)
						f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
					fixed f_c = 0;
					if (f > 0)
						f_c = Mulfx(f,Mulfx(f,f));
					fixed f_d = 0;
					if (f_fm1 > 0)
						f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
					fixed f_RX = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
					fixed f_R = Mulfx(f_RY,f_RX);
					long _k = ((k+1)*4) + (l+1);

					_PIXEL pixel1 = palette[lpSrcData[dwSrcOffsets[_k]]];

					fixed f_rs = itofx(_GetRValue16(pixel1));
					fixed f_gs = itofx(_GetGValue16(pixel1));
					fixed f_bs = itofx(_GetBValue16(pixel1));
					f_red += Mulfx(f_rs,f_R);
					f_green += Mulfx(f_gs,f_R);
					f_blue += Mulfx(f_bs,f_R);
				}
			}
			unsigned char red = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_red,f_gama))));
			unsigned char green = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_green,f_gama))));
			unsigned char blue = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_blue,f_gama))));
			lpDstData[dwDstTotalOffset>>2] = ZMAEE_GET_RGB(red, green, blue);

			// Update destination horizontal offset
			dwDstHorizontalOffset += 4;
		}

		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}

void _ScaleBilinear8_32(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;

	long _pitch = 1 * _width;
	long m_iPitch = 1 * src.width;
	
	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	
	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	
	unsigned int dwDstTotalOffset;
	unsigned char* lpSrcData = (unsigned char*)src.pBits;
	
	unsigned int dwSrcTotalOffset;
	unsigned int* lpDstData = (unsigned int*)lpData;
	unsigned short* palette = (unsigned short*)src.pPalette;
	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;
			
			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*1;
			unsigned int dwSrcTopLeft = dwSrcTotalOffset;
			unsigned int dwSrcTopRight = dwSrcTotalOffset + 1;
			if (n >= src.width-1)
				dwSrcTopRight = dwSrcTotalOffset;
			unsigned int dwSrcBottomLeft = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcBottomLeft = dwSrcTotalOffset;
			unsigned int dwSrcBottomRight = dwSrcTotalOffset + m_iPitch + 1;
			if ((n >= src.width-1) || (m >= src.height-1))
				dwSrcBottomRight = dwSrcTotalOffset;
			fixed f_w1 = Mulfx(f_1-f_f, f_1-f_g);
			fixed f_w2 = Mulfx(f_1-f_f, f_g);
			fixed f_w3 = Mulfx(f_f, f_1-f_g);
			fixed f_w4 = Mulfx(f_f, f_g);
			
			_PIXEL pixel1 = palette[lpSrcData[dwSrcTopLeft]];
			_PIXEL pixel2 = palette[lpSrcData[dwSrcTopRight]];
			_PIXEL pixel3 = palette[lpSrcData[dwSrcBottomLeft]];
			_PIXEL pixel4 = palette[lpSrcData[dwSrcBottomRight]];
			
			fixed f_r1 = itofx(_GetRValue16(pixel1));
			fixed f_r2 = itofx(_GetRValue16(pixel2));
			fixed f_r3 = itofx(_GetRValue16(pixel3));
			fixed f_r4 = itofx(_GetRValue16(pixel4));
			
			fixed f_g1 = itofx(_GetGValue16(pixel1));
			fixed f_g2 = itofx(_GetGValue16(pixel2));
			fixed f_g3 = itofx(_GetGValue16(pixel3));
			fixed f_g4 = itofx(_GetGValue16(pixel4));
			
			fixed f_b1 = itofx(_GetBValue16(pixel1));
			fixed f_b2 = itofx(_GetBValue16(pixel2));
			fixed f_b3 = itofx(_GetBValue16(pixel3));
			fixed f_b4 = itofx(_GetBValue16(pixel4));
			
			int red = fxtoi(Mulfx(f_w1, f_r1) + Mulfx(f_w2, f_r2) + Mulfx(f_w3, f_r3) + Mulfx(f_w4, f_r4));
			int green = fxtoi(Mulfx(f_w1, f_g1) + Mulfx(f_w2, f_g2) + Mulfx(f_w3, f_g3) + Mulfx(f_w4, f_g4));
			int blue = fxtoi(Mulfx(f_w1, f_b1) + Mulfx(f_w2, f_b2) + Mulfx(f_w3, f_b3) + Mulfx(f_w4, f_b4));
			lpDstData[dwDstTotalOffset] = ZMAEE_GET_RGBA(red, green, blue, 255);
			
			// Update destination horizontal offset
			dwDstHorizontalOffset += 1;
		}
		
		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}


void _ScaleBicubic8_16(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	long m_iBpp = 1;
	long _pitch = 2*_width;
	long m_iPitch = m_iBpp * src.width;

	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	fixed f_2 = itofx(2);
	fixed f_4 = itofx(4);
	fixed f_6 = itofx(6);
	fixed f_gama = ftofx(1.04f);

	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	unsigned int dwDstTotalOffset;
	unsigned char* lpSrcData = (unsigned char*)src.pBits;
	unsigned int dwSrcTotalOffset;
	unsigned short* lpDstData = (unsigned short*)lpData;
	
	unsigned short* palette = (unsigned short*)src.pPalette;

	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
			unsigned long dwSrcOffsets[16];
			dwSrcOffsets[0] = dwSrcTotalOffset - m_iPitch - m_iBpp;
			if ((m < 1) || (n < 1))
				dwSrcOffsets[0] = dwSrcTotalOffset;
			dwSrcOffsets[1] = dwSrcTotalOffset - m_iPitch;
			if (m < 1)
				dwSrcOffsets[1] = dwSrcTotalOffset;
			dwSrcOffsets[2] = dwSrcTotalOffset - m_iPitch + m_iBpp;
			if ((m < 1) || (n >= src.width-1))
				dwSrcOffsets[2] = dwSrcTotalOffset;
			dwSrcOffsets[3] = dwSrcTotalOffset - m_iPitch + m_iBpp + m_iBpp;
			if ((m < 1) || (n >= src.width-2))
				dwSrcOffsets[3] = dwSrcTotalOffset;
			dwSrcOffsets[4] = dwSrcTotalOffset - m_iBpp;
			if (n < 1)
				dwSrcOffsets[4] = dwSrcTotalOffset;
			dwSrcOffsets[5] = dwSrcTotalOffset;
			dwSrcOffsets[6] = dwSrcTotalOffset + m_iBpp;
			if (n >= src.width-1)
				dwSrcOffsets[6] = dwSrcTotalOffset;
			dwSrcOffsets[7] = dwSrcTotalOffset + m_iBpp + m_iBpp;
			if (n >= src.width-2)
				dwSrcOffsets[7] = dwSrcTotalOffset;
			dwSrcOffsets[8] = dwSrcTotalOffset + m_iPitch - m_iBpp;
			if ((m >= src.height-1) || (n < 1))
				dwSrcOffsets[8] = dwSrcTotalOffset;
			dwSrcOffsets[9] = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcOffsets[9] = dwSrcTotalOffset;
			dwSrcOffsets[10] = dwSrcTotalOffset + m_iPitch + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-1))
				dwSrcOffsets[10] = dwSrcTotalOffset;
			dwSrcOffsets[11] = dwSrcTotalOffset + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-1) || (n >= src.width-2))
				dwSrcOffsets[11] = dwSrcTotalOffset;
			dwSrcOffsets[12] = dwSrcTotalOffset + m_iPitch + m_iPitch - m_iBpp;
			if ((m >= src.height-2) || (n < 1))
				dwSrcOffsets[12] = dwSrcTotalOffset;
			dwSrcOffsets[13] = dwSrcTotalOffset + m_iPitch + m_iPitch;
			if (m >= src.height-2)
				dwSrcOffsets[13] = dwSrcTotalOffset;
			dwSrcOffsets[14] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-1))
				dwSrcOffsets[14] = dwSrcTotalOffset;
			dwSrcOffsets[15] = dwSrcTotalOffset + m_iPitch + m_iPitch + m_iBpp + m_iBpp;
			if ((m >= src.height-2) || (n >= src.width-2))
				dwSrcOffsets[15] = dwSrcTotalOffset;
			fixed f_red=0, f_green=0, f_blue=0;
			for (long k=-1; k<3; k++)
			{
				fixed f = itofx(k)-f_f;
				fixed f_fm1 = f - f_1;
				fixed f_fp1 = f + f_1;
				fixed f_fp2 = f + f_2;
				fixed f_a = 0;
				if (f_fp2 > 0)
					f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
				fixed f_b = 0;
				if (f_fp1 > 0)
					f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
				fixed f_c = 0;
				if (f > 0)
					f_c = Mulfx(f,Mulfx(f,f));
				fixed f_d = 0;
				if (f_fm1 > 0)
					f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
				fixed f_RY = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
				for (long l=-1; l<3; l++)
				{
					fixed f = itofx(l)-f_g;
					fixed f_fm1 = f - f_1;
					fixed f_fp1 = f + f_1;
					fixed f_fp2 = f + f_2;
					fixed f_a = 0;
					if (f_fp2 > 0)
						f_a = Mulfx(f_fp2,Mulfx(f_fp2,f_fp2));
					fixed f_b = 0;
					if (f_fp1 > 0)
						f_b = Mulfx(f_fp1,Mulfx(f_fp1,f_fp1));
					fixed f_c = 0;
					if (f > 0)
						f_c = Mulfx(f,Mulfx(f,f));
					fixed f_d = 0;
					if (f_fm1 > 0)
						f_d = Mulfx(f_fm1,Mulfx(f_fm1,f_fm1));
					fixed f_RX = Divfx((f_a-Mulfx(f_4,f_b)+Mulfx(f_6,f_c)-Mulfx(f_4,f_d)),f_6);
					fixed f_R = Mulfx(f_RY,f_RX);
					long _k = ((k+1)*4) + (l+1);

					_PIXEL pixel1 = palette[lpSrcData[dwSrcOffsets[_k]]];

					fixed f_rs = itofx(_GetRValue16(pixel1));
					fixed f_gs = itofx(_GetGValue16(pixel1));
					fixed f_bs = itofx(_GetBValue16(pixel1));
					f_red += Mulfx(f_rs,f_R);
					f_green += Mulfx(f_gs,f_R);
					f_blue += Mulfx(f_bs,f_R);
				}
			}
			unsigned char red = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_red,f_gama))));
			unsigned char green = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_green,f_gama))));
			unsigned char blue = (unsigned char)max(0, min(255, fxtoi(Mulfx(f_blue,f_gama))));
			lpDstData[dwDstTotalOffset>>1] = ZMAEE_BMP24TO16(red, green, blue);

			// Update destination horizontal offset
			dwDstHorizontalOffset += 2;
		}

		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}


void _ScaleBilinear8_16(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	// Calculate scaling params
	float dx = (float)src.width/dst.width;
	float dy = (float)src.height/dst.height;
	long _width = dst.width;
	long _height = dst.height;
	//long m_iBpp = 1;
	long _pitch = 1 * _width;
	long m_iPitch = 1 * src.width;
	
	fixed f_dx = ftofx(dx);
	fixed f_dy = ftofx(dy);
	fixed f_1 = itofx(1);
	
	//Create temporary bitmap
	unsigned int dwSize = _pitch * _height;
	unsigned char* lpData = (unsigned char*)dst.pBits;
	
	// Scale bitmap
	unsigned int dwDstHorizontalOffset;
	unsigned int dwDstVerticalOffset = 0;
	
	unsigned int dwDstTotalOffset;
	unsigned char* lpSrcData = (unsigned char*)src.pBits;
	
	unsigned int dwSrcTotalOffset;
	unsigned short* lpDstData = (unsigned short*)lpData;
	
	unsigned short* palette = (unsigned short*)src.pPalette;
	for (long i=0; i<_height; i++)
	{
		dwDstHorizontalOffset = 0;
		for (long j=0; j<_width; j++)
		{
			// Update destination total offset
			dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;
			
			// Update bitmap
			fixed f_i = itofx(i);
			fixed f_j = itofx(j);
			fixed f_a = Mulfx(f_i, f_dy);
			fixed f_b = Mulfx(f_j, f_dx);
			long m = fxtoi(f_a);
			long n = fxtoi(f_b);
			fixed f_f = f_a - itofx(m);
			fixed f_g = f_b - itofx(n);
			dwSrcTotalOffset = m*m_iPitch + n;
			unsigned int dwSrcTopLeft = dwSrcTotalOffset;
			unsigned int dwSrcTopRight = dwSrcTotalOffset + 1;
			if (n >= src.width-1)
				dwSrcTopRight = dwSrcTotalOffset;
			unsigned int dwSrcBottomLeft = dwSrcTotalOffset + m_iPitch;
			if (m >= src.height-1)
				dwSrcBottomLeft = dwSrcTotalOffset;
			unsigned int dwSrcBottomRight = dwSrcTotalOffset + m_iPitch + 1;
			if ((n >= src.width-1) || (m >= src.height-1))
				dwSrcBottomRight = dwSrcTotalOffset;
			fixed f_w1 = Mulfx(f_1-f_f, f_1-f_g);
			fixed f_w2 = Mulfx(f_1-f_f, f_g);
			fixed f_w3 = Mulfx(f_f, f_1-f_g);
			fixed f_w4 = Mulfx(f_f, f_g);
			
			_PIXEL pixel1 = palette[lpSrcData[dwSrcTopLeft]];
			_PIXEL pixel2 = palette[lpSrcData[dwSrcTopRight]];
			_PIXEL pixel3 = palette[lpSrcData[dwSrcBottomLeft]];
			_PIXEL pixel4 = palette[lpSrcData[dwSrcBottomRight]];
			
			fixed f_r1 = itofx(_GetRValue16(pixel1));
			fixed f_r2 = itofx(_GetRValue16(pixel2));
			fixed f_r3 = itofx(_GetRValue16(pixel3));
			fixed f_r4 = itofx(_GetRValue16(pixel4));
			
			fixed f_g1 = itofx(_GetGValue16(pixel1));
			fixed f_g2 = itofx(_GetGValue16(pixel2));
			fixed f_g3 = itofx(_GetGValue16(pixel3));
			fixed f_g4 = itofx(_GetGValue16(pixel4));
			
			fixed f_b1 = itofx(_GetBValue16(pixel1));
			fixed f_b2 = itofx(_GetBValue16(pixel2));
			fixed f_b3 = itofx(_GetBValue16(pixel3));
			fixed f_b4 = itofx(_GetBValue16(pixel4));
			
			int red = fxtoi(Mulfx(f_w1, f_r1) + Mulfx(f_w2, f_r2) + Mulfx(f_w3, f_r3) + Mulfx(f_w4, f_r4));
			int green = fxtoi(Mulfx(f_w1, f_g1) + Mulfx(f_w2, f_g2) + Mulfx(f_w3, f_g3) + Mulfx(f_w4, f_g4));
			int blue = fxtoi(Mulfx(f_w1, f_b1) + Mulfx(f_w2, f_b2) + Mulfx(f_w3, f_b3) + Mulfx(f_w4, f_b4));
			lpDstData[dwDstTotalOffset] = ZMAEE_BMP24TO16(red, green, blue);
			
			// Update destination horizontal offset
			dwDstHorizontalOffset += 1;
		}
		
		// Update destination vertical offset
		dwDstVerticalOffset += _pitch;
	}
}

void _ScaleBilinear(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	bool bUseCubic = false;
	if(src.width < 200) bUseCubic = true;
	switch(src.nDepth)
	{
	case ZMAEE_COLORFORMAT_32:
		if (dst.nDepth == ZMAEE_COLORFORMAT_32 || 
			dst.nDepth == ZMAEE_COLORFORMAT_PM32 ||
			dst.nDepth == ZMAEE_COLORFORMAT_24)
		{
			if(bUseCubic) 
				_ScaleBicubic32(src,dst);
			else
				_ScaleBilinear32(src, dst);
		}
		else if (dst.nDepth == ZMAEE_COLORFORMAT_16)
		{
			if(bUseCubic)
				_ScaleBicubic32_16(src,dst);
			else
				_ScaleBilinear32_16(src, dst);
		}
		break;
	case ZMAEE_COLORFORMAT_24:
		if (dst.nDepth == ZMAEE_COLORFORMAT_32 || 
			dst.nDepth == ZMAEE_COLORFORMAT_PM32 ||
			dst.nDepth == ZMAEE_COLORFORMAT_24)
		{
			if(bUseCubic)
				_ScaleBicubic32(src, dst);
			else
				_ScaleBilinear32(src, dst);
		}
		else if (dst.nDepth == ZMAEE_COLORFORMAT_16)
		{
			if(bUseCubic)
				_ScaleBicubic32_16(src,dst);
			else
				_ScaleBilinear32_16(src, dst);
		}
		break;
	case ZMAEE_COLORFORMAT_16:
		if (dst.nDepth == ZMAEE_COLORFORMAT_16)
		{
			if(bUseCubic)
				_ScaleBicubic16_16(src, dst);
			else
				_ScaleBilinear16_16(src, dst);
		}
		else
		{
			if(bUseCubic)
				_ScaleBicubic16_32(src,dst);
			else
				_ScaleBilinear16_32(src,dst);
		}
		break;
	case ZMAEE_COLORFORMAT_8:
		if (dst.nDepth == ZMAEE_COLORFORMAT_32 || 
			dst.nDepth == ZMAEE_COLORFORMAT_PM32 ||
			dst.nDepth == ZMAEE_COLORFORMAT_24) 
		{
			if(bUseCubic)
				_ScaleBicubic8_32(src,dst);
			else
				_ScaleBilinear8_32(src, dst);
		} 
		else if (dst.nDepth == ZMAEE_COLORFORMAT_16) 
		{
			if(bUseCubic)
				_ScaleBicubic8_16(src, dst);
			else
				_ScaleBilinear8_16(src, dst);
		}
		break;
	}
}

#else // USE_OLD_VERSION


typedef struct {
    unsigned            lineBytes;
    unsigned            copyCount;
	unsigned			sx;
    unsigned            dsx;
    const void*         palette;
} BilinearCopyInfo;

typedef void (*FuncBilinearCopyLine)(void*, const void*, unsigned, BilinearCopyInfo*);

static void BilinearCopyLine8To16(void* d, const void* s, unsigned v, BilinearCopyInfo* info)
{
    unsigned lineBytes = info->lineBytes;
    unsigned copyCount = info->copyCount;
    unsigned dsx = info->dsx;
    const unsigned* prePalette = (const unsigned*)info->palette;
    unsigned sx = info->sx;
    unsigned short* dst = (unsigned short*)d;
    unsigned mask = s_mask_07e0f81f;

    v >>= 27; // 0 - 31的值

    do {
        const unsigned char* src0 = (const unsigned char*)s + (sx >> 16);
        const unsigned char* src1 = src0 + lineBytes;

        unsigned uv5;
        unsigned color, tmp;
        unsigned u5 = (sx << 16) >> 27;
        unsigned v5 = v;

        tmp = prePalette[src1[1]];
        uv5 = (u5 * v5) >> 5;
        color = tmp * uv5;

        tmp = prePalette[src1[0]];
        v5 -= uv5;
        color += tmp * v5;

        tmp = prePalette[src0[1]];
        u5 -= uv5;
        color += tmp * u5;

        tmp = prePalette[src0[0]];
        uv5 = 32 - u5 - v5 - uv5;
        color += tmp * uv5;

        color = (color >> 5) & mask;
        *dst++ = (color >> 16) | color;

        sx += dsx;

    } while (--copyCount);
}

static void BilinearCopyLine8To32(void* d, const void* s, unsigned v, BilinearCopyInfo* info)
{
    unsigned lineBytes = info->lineBytes;
    unsigned copyCount = info->copyCount;
    unsigned dsx = info->dsx;
    const unsigned* prePalette = (const unsigned*)info->palette;
    unsigned sx = info->sx;
    unsigned* dst = (unsigned*)d;
    unsigned mask = s_mask_07e0f81f;

    v >>= 27; // 0 - 31的值

    do {
        const unsigned char* src0 = (const unsigned char*)s + (sx >> 16);
        const unsigned char* src1 = src0 + lineBytes;
        unsigned color, tmp;
        unsigned uv5;
        unsigned u5 = (sx << 16) >> 27;
        unsigned v5 = v;

        tmp = prePalette[src1[1]];
        uv5 = (u5 * v5) >> 5;
        color = tmp * uv5;

        tmp = prePalette[src1[0]];
        v5 -= uv5;
        color += tmp * v5;

        tmp = prePalette[src0[1]];
        u5 -= uv5;
        color += tmp * u5;

        tmp = prePalette[src0[0]];
        uv5 = 32 - u5 - v5 - uv5;
        color += tmp * uv5;

        color = 0xff000000 | (((color << 11) >> 24) << 16) | ((color & 0xff000000) >> 16) | ((color << 22) >> 24);
        *dst++ = color;
        sx += dsx;
    } while (--copyCount);
}

static void BilinearCopyLine16To16(void* d, const void* s, unsigned v, BilinearCopyInfo* info)
{
    unsigned lineBytes = info->lineBytes;
    unsigned copyCount = info->copyCount;
    unsigned dsx = info->dsx;

    unsigned sx = info->sx;

    unsigned short* dst = (unsigned short*)d;
    unsigned mask = s_mask_07e0f81f;

    v >>= 27; // 0 - 31的值

    do {
        const unsigned short* src0 = (const unsigned short*)s + (sx >> 16);
        const unsigned short* src1 = (const unsigned short*)((char*)src0 + lineBytes);

        unsigned color, tmp;
        unsigned uv5;
        unsigned u5 = (sx << 16) >> 27;
        unsigned v5 = v;

        tmp = src1[1];
        tmp |= tmp << 16;
        uv5 = (u5 * v5) >> 5;
        color = (tmp & mask) * uv5;

        tmp = src1[0];
        tmp |= tmp << 16;
        v5 -= uv5;
        color += (tmp & mask) * v5;

        tmp = src0[1];
        tmp |= tmp << 16;
        u5 -= uv5;
        color += (tmp & mask) * u5;

        tmp = src0[0];
        tmp |= tmp << 16;
        uv5 = 32 - u5 - v5 - uv5;
        color += (tmp & mask) * uv5;

        color = (color >> 5) & mask;
        *dst++ = (color >> 16) | color;

        sx += dsx;

    } while (--copyCount);
}

static void BilinearCopyLine16To32(void* d, const void* s, unsigned v, BilinearCopyInfo* info)
{
    unsigned lineBytes = info->lineBytes;
    unsigned copyCount = info->copyCount;
    unsigned dsx = info->dsx;
    unsigned sx = info->sx;
    unsigned* dst = (unsigned*)d;
    unsigned mask = s_mask_07e0f81f;

    v >>= 27; // 0 - 31的值

    do {
        const unsigned short* src0 = (const unsigned short*)s + (sx >> 16);
        const unsigned short* src1 = (const unsigned short*)((char*)src0 + lineBytes);

        unsigned color, tmp;
        unsigned uv5;
        unsigned u5 = (sx << 16) >> 27;
        unsigned v5 = v;

        tmp = src1[1];
        tmp |= tmp << 16;
        uv5 = (u5 * v5) >> 5;
        color = (tmp & mask) * uv5;

        tmp = src1[0];
        tmp |= tmp << 16;
        v5 -= uv5;
        color += (tmp & mask) * v5;

        tmp = src0[1];
        tmp |= tmp << 16;
        u5 -= uv5;
        color += (tmp & mask) * u5;

        tmp = src0[0];
        tmp |= tmp << 16;
        uv5 = 32 - u5 - v5 - uv5;
        color += (tmp & mask) * uv5;

        color = 0xff000000 | ((color >> 13) << 16) | ((color & 0xff000000) >> 16) | ((color << 22) >> 24);
        *dst++ = color;

        sx += dsx;

    } while (--copyCount);
}

static void BilinearCopyLine24To16(void* d, const void* s, unsigned v, BilinearCopyInfo* info)
{
    unsigned lineBytes = info->lineBytes;
    unsigned copyCount = info->copyCount;
    unsigned dsx = info->dsx;
    unsigned sx = info->sx;

    unsigned short* dst = (unsigned short*)d;
    unsigned mask = s_mask_07e0f81f;

    v >>= 27; // 0 - 31的值

    do {
        const unsigned* src0 = (const unsigned*)s + (sx >> 16);
        const unsigned* src1 = (const unsigned*)((char*)src0 + lineBytes);

        unsigned color, tmp;
        unsigned uv5;
        unsigned u5 = (sx << 16) >> 27;
        unsigned v5 = v;


        tmp = src1[1];
        tmp = ((tmp & 0xF8) >> 3) | ((tmp & 0xF80000) >> 8) | ((tmp & 0xFC00) << 11);
        uv5 = (u5 * v5) >> 5;
        color = tmp * uv5;

        tmp = src1[0];
        tmp = ((tmp & 0xF8) >> 3) | ((tmp & 0xF80000) >> 8) | ((tmp & 0xFC00) << 11);
        v5 -= uv5;
        color += tmp * v5;

        tmp = src0[1];
        tmp = ((tmp & 0xF8) >> 3) | ((tmp & 0xF80000) >> 8) | ((tmp & 0xFC00) << 11);
        u5 -= uv5;
        color += tmp * u5;

        tmp = src0[0];
        tmp = ((tmp & 0xF8) >> 3) | ((tmp & 0xF80000) >> 8) | ((tmp & 0xFC00) << 11);
        uv5 = 32 - u5 - v5 - uv5;
        color += tmp * uv5;

        color = (color >> 5) & mask;
        *dst++ = (color >> 16) | color;

        sx += dsx;

    } while (--copyCount);
}

static void BilinearCopyLine32To32(void* d, const void* s, unsigned v, BilinearCopyInfo* info)
{
    unsigned lineBytes = info->lineBytes;
    unsigned copyCount = info->copyCount;
    unsigned dsx = info->dsx;
    unsigned sx = info->sx;
    unsigned* dst = (unsigned*)d;
    unsigned mask = s_mask_00ff00ff;

    v >>= 24; // 0 - 255的值

    do {
        const unsigned* src0 = (const unsigned*)s + (sx >> 16);
        const unsigned* src1 = (const unsigned*)((char*)src0 + lineBytes);

        unsigned long color, ag, br;
        unsigned uv8;
        unsigned u8 = (sx << 16) >> 24;
        unsigned v8 = v;

        color = ((unsigned long*)(src1))[1];
        uv8 = (u8 * v8) >> 8;
        ag = ((color >> 8) & mask) * uv8;
        br = (color & mask) * uv8;

        color = ((unsigned long*)(src1))[0];
        v8 -= uv8;
        ag += ((color >> 8) & mask) * v8;
        br += (color & mask) * v8;

        color = ((unsigned long*)(src0))[1];
        u8 -= uv8;
        ag += ((color >> 8) & mask) * u8;
        br += (color & mask) * u8;

        color = ((unsigned long*)(src0))[0];
        uv8 = 256 - u8 - v8 - uv8;
        ag += ((color >> 8) & mask) * uv8;
        br += (color & mask) * uv8;

        *dst++ = (ag & ~mask) | ((br & ~mask) >> 8);

        sx += dsx;
    } while (--copyCount);
}

static void BilinearCopyLine32ToPM32(void* d, const void* s, unsigned v, BilinearCopyInfo* info)
{
    unsigned lineBytes = info->lineBytes;
    unsigned copyCount = info->copyCount;
    unsigned dsx = info->dsx;
    unsigned sx = info->sx;
    unsigned* dst = (unsigned*)d;
    unsigned mask = s_mask_00ff00ff;

    v >>= 24; // 0 - 255的值

    do {
        const unsigned* src0 = (const unsigned*)s + (sx >> 16);
        const unsigned* src1 = (const unsigned*)((char*)src0 + lineBytes);

        unsigned long color, ag, br;
        unsigned uv8;
        unsigned u8 = (sx << 16) >> 24;
        unsigned v8 = v;
        unsigned alpha;
        color = ((unsigned long*)(src1))[1];
        uv8 = (u8 * v8) >> 8;
        ag = ((color >> 8) & mask) * uv8;
        br = (color & mask) * uv8;

        color = ((unsigned long*)(src1))[0];
        v8 -= uv8;
        ag += ((color >> 8) & mask) * v8;
        br += (color & mask) * v8;

        color = ((unsigned long*)(src0))[1];
        u8 -= uv8;
        ag += ((color >> 8) & mask) * u8;
        br += (color & mask) * u8;

        color = ((unsigned long*)(src0))[0];
        uv8 = 256 - u8 - v8 - uv8;
        ag += ((color >> 8) & mask) * uv8;
        br += (color & mask) * uv8;

        alpha = ag >> 24;
        br = ((br >> 8) & mask) * alpha;
        ag = (ag & 0xff00) * alpha;
        *dst++ = (alpha << 24) | ((ag & 0xff0000) >> 8) | ((br & ~mask) >> 8);

        sx += dsx;
    } while (--copyCount);
}

static void _ScaleBilinearFast(const ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
    unsigned sy;
    unsigned dsx;
    unsigned dsy;
    unsigned dstW;
    unsigned dstH;
    unsigned srcW;
    unsigned srcH;
    unsigned sshift;
    unsigned dshift;
	unsigned sxStart;
	unsigned syStart;
    const void* srcBuf;
    void* dstBuf;
    FuncBilinearCopyLine copyLine;
    BilinearCopyInfo copyInfo;

    char srcLastData[16];
    unsigned prePalette[256];

    if ((int)(dstW = dst.width) <= 0)
        return;
    if ((int)(dstH = dst.height) <= 0)
        return;
    if ((int)(srcW = src.width) <= 0)
        return;
    if ((int)(srcH = src.height) <= 0)
        return;
	
    if (src.nDepth == ZMAEE_COLORFORMAT_8) {
        unsigned* dPal;
        const unsigned short* sPal;
        unsigned cnt;
        unsigned mask = s_mask_07e0f81f;
        dPal = (unsigned*)prePalette;
        copyInfo.palette = (const void*)dPal;
        cnt = src.nPaletteLen / 2;
        sPal = (const unsigned short*)src.pPalette;
        while (cnt-- > 0) {
            unsigned c = *sPal++;
            *dPal++ = (c | (c << 16)) & mask;
        }

        sshift = 0;

        if (dst.nDepth == ZMAEE_COLORFORMAT_16) {
            dshift = 1;
            copyLine = BilinearCopyLine8To16;
        } else if (
            dst.nDepth == ZMAEE_COLORFORMAT_24 ||
            dst.nDepth == ZMAEE_COLORFORMAT_32 ||
            dst.nDepth == ZMAEE_COLORFORMAT_PM32) {
            dshift = 2;
            copyLine = BilinearCopyLine8To32;
        } else {
            return;
        }
    } else if (src.nDepth == ZMAEE_COLORFORMAT_16) {
        sshift = 1;
        if (dst.nDepth == ZMAEE_COLORFORMAT_16) {
            dshift = 1;
            copyLine = BilinearCopyLine16To16;
        } else if (
            dst.nDepth == ZMAEE_COLORFORMAT_24 ||
            dst.nDepth == ZMAEE_COLORFORMAT_32 ||
            dst.nDepth == ZMAEE_COLORFORMAT_PM32) {
            dshift = 2;
            copyLine = BilinearCopyLine16To32;
        } else {
            return;
        }
    } else if (src.nDepth == ZMAEE_COLORFORMAT_24) {
        sshift = 2;
        if (dst.nDepth == ZMAEE_COLORFORMAT_16) {
            dshift = 1;
            copyLine = BilinearCopyLine24To16;
        } else if (
            dst.nDepth == ZMAEE_COLORFORMAT_24 ||
            dst.nDepth == ZMAEE_COLORFORMAT_32 ||
            dst.nDepth == ZMAEE_COLORFORMAT_PM32
        ) {
            dshift = 2;
            copyLine = BilinearCopyLine32To32;
        } else {
            return;
        }
    } else if (src.nDepth == ZMAEE_COLORFORMAT_32) {
        sshift = 2;
        if (dst.nDepth == ZMAEE_COLORFORMAT_32) {
            dshift = 2;
            copyLine = BilinearCopyLine32To32;
        } else if (dst.nDepth == ZMAEE_COLORFORMAT_PM32) {
            dshift = 2;
            copyLine = BilinearCopyLine32ToPM32;
        } else {
            return;
        }
    } else if (src.nDepth == ZMAEE_COLORFORMAT_PM32) {
        sshift = 2;
        if (dst.nDepth == ZMAEE_COLORFORMAT_PM32) {
            dshift = 2;
            copyLine = BilinearCopyLine32To32;
        } else {
            return;
        }
    } else {
        return;
    }
	
	if (dstW > srcW) {
		// 放大
		dsx = ((srcW - 1) << 16) / (dstW - 1);
		sxStart = 0;
	} else {
		// 缩小
		dsx = (srcW << 16) / dstW;
		sxStart = (dsx >> 1) - (1 << 15);
	}

	if (dstH > srcH) {
		// 放大
		dsy = ((srcH - 1) << 16) / (dstH - 1);
		syStart = 0;
	} else {
		// 缩小
		dsy = (srcH << 16) / dstH;
		syStart = (dsy >> 1) - (1 << 15);
	}


    srcW <<= sshift;


    copyInfo.lineBytes = srcW;
    copyInfo.copyCount = dstW;
    copyInfo.dsx = dsx;
	copyInfo.sx = sxStart;

    srcBuf = src.pBits;
    dstBuf = dst.pBits;

    sy = syStart;

    do {
		if ((sy >> 16) + 2 >= srcH) // 已经遍历到源图片的最后两行,每行的最后一个像素可能需要特殊处理
			break;
		copyLine(dstBuf, (const void*)((char*)srcBuf + ((sy >> 16) * srcW)), sy << 16, &copyInfo);
        dstBuf = (void*)((char*)dstBuf + (dstW << dshift));
        sy += dsy;
    } while (--dstH);
	
	if (dstH > 0) {
		unsigned sxLast = sxStart + dsx * (dstW - 1);
		if ((sxLast >> 16) + 1 >= (srcW >> sshift)) { // 每行的最后一个像素需要特殊处理
			BilinearCopyInfo copyInfoBorder;
			copyInfoBorder.palette = copyInfo.palette;
			copyInfoBorder.lineBytes = 1 << sshift;
			copyInfoBorder.copyCount = 1;
			copyInfoBorder.dsx = 0;
			copyInfoBorder.sx = 0; //(sxLast & 0xffff);
			
			copyInfo.copyCount = dstW - 1;

			// 当前行最后一个像素
			zmaee_memcpy(srcLastData, (const void*)((char*)srcBuf + ((sy >> 16) * srcW) + (srcW - (1 << sshift))), 1 << sshift);
			// 最后行最后一个像素
			zmaee_memcpy(srcLastData + (1 << sshift), (const void*)((char*)srcBuf + (srcH * srcW) - (1 << sshift)), 1 << sshift);
			do {
				if ((sy >> 16) + 1 >= srcH) { // 遍历到源图片的最后一行
					copyInfo.lineBytes = 0;
					copyInfoBorder.lineBytes = 0;
					zmaee_memcpy(srcLastData, (const void*)((char*)srcBuf + (srcH * srcW) - (1 << sshift)), 1 << sshift);
				}
				if (dstW > 1)
					copyLine(dstBuf, (const void*)((char*)srcBuf + ((sy >> 16) * srcW)), sy << 16, &copyInfo);
				
				dstBuf = (void*)((char*)dstBuf + (dstW << dshift));
				
				// 处理当前行的最后一个像素
				copyLine((void*)((char*)dstBuf - (1 << dshift)), (const void*)srcLastData, sy << 16, &copyInfoBorder);
				
				sy += dsy;
			} while (--dstH);

		} else { // 每行的最后一个像素不会访问越界
			do {
				if ((sy >> 16) + 1 >= srcH) // 遍历到源图片的最后一行
					copyInfo.lineBytes = 0;
				copyLine(dstBuf, (const void*)((char*)srcBuf + ((sy >> 16) * srcW)), sy << 16, &copyInfo);
				dstBuf = (void*)((char*)dstBuf + (dstW << dshift));
				sy += dsy;
			} while (--dstH);
		}
	}

}


// #define MAKE_TABLE
#if defined (MAKE_TABLE)
static int s_tab0[256];
static int s_tab1[256];
static int s_tab2[256];
static int s_tab3[256];
static int makeTable()
{
	{
		fixed f_1 = itofx(1);
		fixed f_2 = itofx(2);
		fixed f_4 = itofx(4);
		fixed f_6 = itofx(6);

		for (int i = 0; i < 256; ++i) {

			{
				fixed f = itofx(-1) - i;
				fixed f_fm1 = f - f_1;
				fixed f_fp1 = f + f_1;
				fixed f_fp2 = f + f_2;
				fixed f_a = 0;
				if (f_fp2 > 0)
					f_a = Mulfx(f_fp2, Mulfx(f_fp2, f_fp2));
				fixed f_b = 0;
				if (f_fp1 > 0)
					f_b = Mulfx(f_fp1, Mulfx(f_fp1, f_fp1));
				fixed f_c = 0;
				if (f > 0)
					f_c = Mulfx(f, Mulfx(f, f));
				fixed f_d = 0;
				if (f_fm1 > 0)
					f_d = Mulfx(f_fm1, Mulfx(f_fm1, f_fm1));
				s_tab0[i] = Divfx((f_a - Mulfx(f_4, f_b) + Mulfx(f_6, f_c) - Mulfx(f_4, f_d)), f_6);

			}
			

			{
				fixed f = itofx(0) - i;
				fixed f_fm1 = f - f_1;
				fixed f_fp1 = f + f_1;
				fixed f_fp2 = f + f_2;
				fixed f_a = 0;
				if (f_fp2 > 0)
					f_a = Mulfx(f_fp2, Mulfx(f_fp2, f_fp2));
				fixed f_b = 0;
				if (f_fp1 > 0)
					f_b = Mulfx(f_fp1, Mulfx(f_fp1, f_fp1));
				fixed f_c = 0;
				if (f > 0)
					f_c = Mulfx(f, Mulfx(f, f));
				fixed f_d = 0;
				if (f_fm1 > 0)
					f_d = Mulfx(f_fm1, Mulfx(f_fm1, f_fm1));
				s_tab1[i] = Divfx((f_a - Mulfx(f_4, f_b) + Mulfx(f_6, f_c) - Mulfx(f_4, f_d)), f_6);
				

				
			}
			{
				
				fixed f = itofx(1) - i;
				fixed f_fm1 = f - f_1;
				fixed f_fp1 = f + f_1;
				fixed f_fp2 = f + f_2;
				fixed f_a = 0;
				if (f_fp2 > 0)
					f_a = Mulfx(f_fp2, Mulfx(f_fp2, f_fp2));
				fixed f_b = 0;
				if (f_fp1 > 0)
					f_b = Mulfx(f_fp1, Mulfx(f_fp1, f_fp1));
				fixed f_c = 0;
				if (f > 0)
					f_c = Mulfx(f, Mulfx(f, f));
				fixed f_d = 0;
				if (f_fm1 > 0)
					f_d = Mulfx(f_fm1, Mulfx(f_fm1, f_fm1));
				s_tab2[i] = Divfx((f_a - Mulfx(f_4, f_b) + Mulfx(f_6, f_c) - Mulfx(f_4, f_d)), f_6);
				

			}
			{
				
				fixed f = itofx(2) - i;
				fixed f_fm1 = f - f_1;
				fixed f_fp1 = f + f_1;
				fixed f_fp2 = f + f_2;
				fixed f_a = 0;
				if (f_fp2 > 0)
					f_a = Mulfx(f_fp2, Mulfx(f_fp2, f_fp2));
				fixed f_b = 0;
				if (f_fp1 > 0)
					f_b = Mulfx(f_fp1, Mulfx(f_fp1, f_fp1));
				fixed f_c = 0;
				if (f > 0)
					f_c = Mulfx(f, Mulfx(f, f));
				fixed f_d = 0;
				if (f_fm1 > 0)
					f_d = Mulfx(f_fm1, Mulfx(f_fm1, f_fm1));
				s_tab3[i] = Divfx((f_a - Mulfx(f_4, f_b) + Mulfx(f_6, f_c) - Mulfx(f_4, f_d)), f_6);
	
			}


		}

	}

	
    {
		fixed f_1 = itofx(1);
		fixed f_2 = itofx(2);
		fixed f_4 = itofx(4);
		fixed f_6 = itofx(6);
		fixed f_gama = ftofx(1.04f);
        {

            // Update bitmap
         //   fixed f_i = itofx(i);
         //   fixed f_j = itofx(j);
         //   fixed f_a = Mulfx(f_i, f_dy);
         //   fixed f_b = Mulfx(f_j, f_dx);
         //   long m = fxtoi(f_a);
         //   long n = fxtoi(f_b);
            fixed f_v = 2;
            fixed f_g = 253;

            fixed f_red = 0, f_green = 0, f_blue = 0;
            for (long k = -1; k < 3; k++) {
                fixed f = itofx(k) - f_v;
                fixed f_fm1 = f - f_1;
                fixed f_fp1 = f + f_1;
                fixed f_fp2 = f + f_2;
                fixed f_a = 0;
                if (f_fp2 > 0)
                    f_a = Mulfx(f_fp2, Mulfx(f_fp2, f_fp2));
                fixed f_b = 0;
                if (f_fp1 > 0)
                    f_b = Mulfx(f_fp1, Mulfx(f_fp1, f_fp1));
                fixed f_c = 0;
                if (f > 0)
                    f_c = Mulfx(f, Mulfx(f, f));
                fixed f_d = 0;
                if (f_fm1 > 0)
                    f_d = Mulfx(f_fm1, Mulfx(f_fm1, f_fm1));
                fixed f_RY = Divfx((f_a - Mulfx(f_4, f_b) + Mulfx(f_6, f_c) - Mulfx(f_4, f_d)), f_6);
                for (long l = -1; l < 3; l++) {
                    fixed f = itofx(l) - f_g;
                    fixed f_fm1 = f - f_1;
                    fixed f_fp1 = f + f_1;
                    fixed f_fp2 = f + f_2;
                    fixed f_a = 0;
                    if (f_fp2 > 0)
                        f_a = Mulfx(f_fp2, Mulfx(f_fp2, f_fp2));
                    fixed f_b = 0;
                    if (f_fp1 > 0)
                        f_b = Mulfx(f_fp1, Mulfx(f_fp1, f_fp1));
                    fixed f_c = 0;
                    if (f > 0)
                        f_c = Mulfx(f, Mulfx(f, f));
                    fixed f_d = 0;
                    if (f_fm1 > 0)
                        f_d = Mulfx(f_fm1, Mulfx(f_fm1, f_fm1));
                    fixed f_RX = Divfx((f_a - Mulfx(f_4, f_b) + Mulfx(f_6, f_c) - Mulfx(f_4, f_d)), f_6);
                    fixed f_R = Mulfx(f_RY, f_RX);
                  
					//long _k = ((k + 1) * 4) + (l + 1);

                    //_PIXEL pixel1 = palette[lpSrcData[dwSrcOffsets[_k]]];

                    fixed f_rs = itofx(_GetRValue16(0));
                    fixed f_gs = itofx(_GetGValue16(0));
                    fixed f_bs = itofx(_GetBValue16(0));
                    f_red += Mulfx(f_rs, f_R);
                    f_green += Mulfx(f_gs, f_R);
                    f_blue += Mulfx(f_bs, f_R);
                }
            }
        }
    }
	return 0;
}
static int sMakeTable = makeTable();
inline double SinXDivX(double x)
{
	if (x == 0)
		return 1.0;
	
	//return sin(x * 3.14159265) / (x * 3.14159265);
	
    //该函数计算插值曲线sin(x*PI)/(x*PI)的值 //PI=3.1415926535897932385;
    //下面是它的近似拟合表达式
    const double a = -0.5; //a还可以取 a=-2,-1,-0.75,-0.5等等，起到调节锐化或模糊程度的作用
	
    if (x < 0) x = -x; //x=abs(x);
    double x2 = x * x;
    double x3 = x2 * x;
    if (x <= 1)
        return (a + 2) * x3 - (a + 3) * x2 + 1;
    else if (x <= 2)
        return a * x3 - (5 * a) * x2 + (8 * a) * x - (4 * a);
    else
        return 0;
}
static long SinXDivX_Table_8[(2 << 8) + 1];
class _CAutoInti_SinXDivX_Table
{
private:
    void _Inti_SinXDivX_Table() {
		int i;
        for (i = 0; i <= (2 << 8); ++i) {
            double t = SinXDivX(i * (1.0 / (256))) * 256.0;
            SinXDivX_Table_8[i] = long(t >= 0.0 ? (t + 0.5) : (t - 0.5)) * 1;
        }


    };
public:
    _CAutoInti_SinXDivX_Table() { _Inti_SinXDivX_Table(); }
};
static _CAutoInti_SinXDivX_Table __tmp_CAutoInti_SinXDivX_Table;

inline unsigned char border_color(long Color)
{
    if (Color <= 0)
        return 0;
    else if (Color >= 255)
        return 255;
    else
        return (unsigned char)Color;
}


//颜色查表
static unsigned char _color_table[256 * 3];
static const unsigned char* color_table = &_color_table[256];
class _CAuto_inti_color_table
{
public:
    _CAuto_inti_color_table() {
        for (int i = 0; i < 256 * 3; ++i)
            _color_table[i] = border_color(i - 256);
		
    }
};

static _CAuto_inti_color_table _Auto_inti_color_table;

#else // MAKE_TABLE

static const unsigned char SinXDivX_Table_8[512 + 1] = {
0xac,0xab,0xab,0xab,0xab,0xab,0xab,0xab,0xab,0xab,0xab,0xab,0xab,0xab,0xab,0xab,
0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xa9,0xa9,0xa9,0xa9,0xa9,0xa8,0xa8,0xa8,0xa8,
0xa8,0xa7,0xa7,0xa7,0xa6,0xa6,0xa6,0xa6,0xa6,0xa5,0xa5,0xa5,0xa5,0xa5,0xa4,0xa4,
0xa4,0xa3,0xa2,0xa2,0xa2,0xa2,0xa2,0xa2,0xa1,0xa1,0xa0,0x9f,0x9f,0x9f,0x9f,0x9e,
0x9e,0x9e,0x9d,0x9d,0x9d,0x9c,0x9c,0x9b,0x9b,0x9a,0x99,0x99,0x99,0x98,0x98,0x97,
0x97,0x96,0x96,0x95,0x95,0x95,0x94,0x93,0x93,0x93,0x92,0x92,0x91,0x91,0x90,0x8f,
0x8e,0x8d,0x8d,0x8d,0x8d,0x8c,0x8b,0x8b,0x8b,0x8a,0x89,0x88,0x87,0x87,0x87,0x86,
0x86,0x85,0x85,0x83,0x83,0x83,0x82,0x81,0x81,0x80,0x7f,0x7f,0x7f,0x7e,0x7d,0x7c,
0x7b,0x7a,0x79,0x79,0x78,0x77,0x77,0x76,0x75,0x75,0x74,0x73,0x73,0x73,0x71,0x71,
0x70,0x70,0x6f,0x6e,0x6e,0x6d,0x6d,0x6c,0x6b,0x6a,0x6a,0x69,0x68,0x68,0x67,0x67,
0x66,0x65,0x64,0x64,0x63,0x62,0x62,0x61,0x60,0x60,0x5f,0x5e,0x5e,0x5d,0x5c,0x5c,
0x5b,0x5a,0x59,0x59,0x58,0x58,0x57,0x57,0x56,0x55,0x54,0x54,0x53,0x52,0x52,0x51,
0x50,0x50,0x4f,0x4e,0x4e,0x4e,0x4d,0x4c,0x4b,0x4a,0x4a,0x49,0x48,0x48,0x48,0x47,
0x46,0x45,0x45,0x44,0x43,0x43,0x43,0x42,0x41,0x41,0x40,0x3f,0x3f,0x3e,0x3d,0x3d,
0x3c,0x3b,0x3b,0x3a,0x3a,0x39,0x38,0x38,0x37,0x37,0x36,0x35,0x35,0x34,0x34,0x33,
0x33,0x32,0x31,0x31,0x30,0x30,0x2f,0x2f,0x2e,0x2e,0x2d,0x2d,0x2c,0x2c,0x2b,0x2b,
0x2a,0x2a,0x2a,0x29,0x29,0x28,0x28,0x27,0x27,0x26,0x26,0x25,0x25,0x24,0x24,0x23,
0x23,0x23,0x22,0x22,0x21,0x21,0x20,0x20,0x20,0x1f,0x1f,0x1e,0x1e,0x1e,0x1d,0x1d,
0x1c,0x1c,0x1c,0x1b,0x1b,0x1b,0x1a,0x19,0x19,0x19,0x18,0x18,0x18,0x17,0x17,0x17,
0x16,0x16,0x16,0x16,0x16,0x15,0x14,0x14,0x14,0x13,0x13,0x13,0x13,0x13,0x12,0x12,
0x12,0x11,0x11,0x11,0x10,0x10,0x10,0x10,0x0f,0x0f,0x0f,0x0e,0x0e,0x0e,0x0e,0x0e,
0x0d,0x0d,0x0d,0x0d,0x0c,0x0c,0x0c,0x0c,0x0c,0x0b,0x0b,0x0b,0x0b,0x0a,0x0a,0x0a,
0x0a,0x0a,0x0a,0x09,0x09,0x09,0x09,0x09,0x08,0x08,0x08,0x08,0x08,0x08,0x07,0x07,
0x07,0x07,0x07,0x07,0x07,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x05,0x05,0x05,0x05,
0x05,0x05,0x05,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x03,0x03,0x03,0x03,
0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x02,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0
};
#endif // MAKE_TABLE


typedef struct ThreeOrderCopyInfo ThreeOrderCopyInfo;
typedef int (*FuncGetPixelBorder)(const ZMAEE_BitmapInfo* pic, void* data, int x, int y);
typedef void (*FuncThreeOrderCopyLine)(void*, const void*, ThreeOrderCopyInfo*);
struct ThreeOrderCopyInfo {
    //unsigned lineBytes;
    int copyCount;
    int x_16;
	int y_16;
	unsigned			srcWidth;
//	unsigned			sshift;
	unsigned			dshift;
	unsigned            dsx;
    const void*         palette;
	const ZMAEE_BitmapInfo* src;
	FuncGetPixelBorder getPixelBorder;
	FuncThreeOrderCopyLine copyLine;
	void*				borderPixels;
};

struct Color32 {
    unsigned char  b;
    unsigned char  g;
    unsigned char  r;
    unsigned char  a;
} ;

// 边界饱和
static int getPixelBorder8(const ZMAEE_BitmapInfo* pic, void* data, int x, int y)
{
	unsigned char* src_end;
	unsigned char* dst = (unsigned char*)data - 5;
	unsigned char* src = (unsigned char*)pic->pBits;

	int width = pic->width;
	int height = pic->height;
	int x_start = --x;
	int y_start = --y;
	int x_end = x + 3;
	int y_end = y + 3;
	if (x_start < 0)
		x_start = 0;
	if (y_start < 0)
		y_start = 0;
	if (x_end >= width)
		x_end = width - 1;
	if (y_end >= height)
		y_end = height - 1;
	
	if (x_end < x_start || y_end < y_start)
		return 0;
	
	src = src + y_start * width + x_start;
	--y;
	int t = 4;
	do {
		int tx = x - 1;
		unsigned val = src[0];
		src_end = src;
		dst[0] = val;
		if (++tx >= x_start && tx < x_end) val = *++src_end;
		dst[1] = val;
		if (++tx >= x_start && tx < x_end) val = *++src_end;
		dst[2] = val;
		if (++tx >= x_start && tx < x_end) val = *++src_end;
		dst[3] = val;
		dst += 4;
		if (++y >= y_start && y < y_end) src += width;
	} while (--t);
	return 1;
}


static int getPixelBorder16(const ZMAEE_BitmapInfo* pic, void* data, int x, int y)
{
	unsigned short* src_end;
	unsigned short* dst = (unsigned short*)data - 5;
	unsigned short* src = (unsigned short*)pic->pBits;
	
	int width = pic->width;
	int height = pic->height;
	int x_start = --x;
	int y_start = --y;
	int x_end = x + 3;
	int y_end = y + 3;
	if (x_start < 0)
		x_start = 0;
	if (y_start < 0)
		y_start = 0;
	if (x_end >= width)
		x_end = width - 1;
	if (y_end >= height)
		y_end = height - 1;
	
	if (x_end < x_start || y_end < y_start)
		return 0;
	
	src = src + y_start * width + x_start;
	--y;
	int t = 4;
	do {
		int tx = x - 1;
		unsigned val = src[0];
		src_end = src;
		dst[0] = val;
		if (++tx >= x_start && tx < x_end) val = *++src_end;
		dst[1] = val;
		if (++tx >= x_start && tx < x_end) val = *++src_end;
		dst[2] = val;
		if (++tx >= x_start && tx < x_end) val = *++src_end;
		dst[3] = val;
		dst += 4;
		if (++y >= y_start && y < y_end) src += width;
	} while (--t);
	return 1;
}

static int getPixelBorder32(const ZMAEE_BitmapInfo* pic, void* data, int x, int y)
{
	unsigned* src_end;
	unsigned* dst = (unsigned*)data - 5;
	unsigned* src = (unsigned*)pic->pBits;
	
	int width = pic->width;
	int height = pic->height;
	int x_start = --x;
	int y_start = --y;
	int x_end = x + 3;
	int y_end = y + 3;
	if (x_start < 0)
		x_start = 0;
	if (y_start < 0)
		y_start = 0;
	if (x_end >= width)
		x_end = width - 1;
	if (y_end >= height)
		y_end = height - 1;
	
	if (x_end < x_start || y_end < y_start)
		return 0;
	
	src = src + y_start * width + x_start;
	--y;
	int t = 4;
	do {
		int tx = x - 1;
		unsigned val = src[0];
		src_end = src;
		dst[0] = val;
		if (++tx >= x_start && tx < x_end) val = *++src_end;
		dst[1] = val;
		if (++tx >= x_start && tx < x_end) val = *++src_end;
		dst[2] = val;
		if (++tx >= x_start && tx < x_end) val = *++src_end;
		dst[3] = val;
		dst += 4;
		if (++y >= y_start && y < y_end) src += width;
	} while (--t);
	return 1;
}

static
void ThreeOrderCopyLine8To16(void* d, const void* s, ThreeOrderCopyInfo* info) 
{
    int srcWidth = (int)info->srcWidth;
    int copyCount = info->copyCount;
    int x_16 = info->x_16;
	int y_16 = info->y_16;
	int dsx = info->dsx;
    unsigned av_8[4];
	unsigned* palette = (unsigned*)info->palette;
    unsigned v_8 = ((unsigned)(y_16 << 16)) >> 24;
    unsigned short* dst = (unsigned short*)d;
	unsigned mask = s_mask_00ff00ff;
	s = (void*)((char*)s + ((y_16 >> 16) - 1) * srcWidth - 1);
	
	av_8[0] = SinXDivX_Table_8[(1 << 8) + v_8];
	av_8[1] = SinXDivX_Table_8[v_8];
	av_8[2] = SinXDivX_Table_8[(1 << 8) - v_8];
    av_8[3] = SinXDivX_Table_8[(2 << 8) - v_8];
    do {
		unsigned au_8[4];
		unsigned u_8 = ((unsigned)(x_16 << 16)) >> 24;
		unsigned char* src = ((unsigned char*)s) + (x_16 >> 16);
		x_16 += dsx;
        au_8[0] = SinXDivX_Table_8[(1 << 8) + u_8];
        au_8[1] = SinXDivX_Table_8[u_8];
        au_8[2] = SinXDivX_Table_8[(1 << 8) - u_8];
        au_8[3] = SinXDivX_Table_8[(2 << 8) - u_8];
		unsigned rb = 0, ag = 0;
        for (unsigned i = 0; i < 4; ++i) {
			unsigned av = av_8[i];
            unsigned* pixel0 = (unsigned*)(palette + src[0]);
            unsigned* pixel1 = (unsigned*)(palette + src[1]);
            unsigned* pixel2 = (unsigned*)(palette + src[2]);
            unsigned* pixel3 = (unsigned*)(palette + src[3]);
			unsigned t0 = (au_8[0] * av) >> 8;
			unsigned t1 = (au_8[1] * av) >> 8;
			unsigned t2 = (au_8[2] * av) >> 8;
			unsigned t3 = (au_8[3] * av) >> 8;
			rb += t0 * (pixel0[0] & mask) + 
				  t1 * (pixel1[0] & mask) + 
				  t2 * (pixel2[0] & mask) + 
				  t3 * (pixel3[0] & mask);
			ag += t0 * ((pixel0[0] >> 8) & mask) + 
				  t1 * ((pixel1[0] >> 8) & mask) +
				  t2 * ((pixel2[0] >> 8) & mask) +
				  t3 * ((pixel3[0] >> 8) & mask);
            src += srcWidth;
        }
        *dst++ = ZMAEE_BMP24TO16(rb >> 24, ag >> 8, rb >> 8);
    } while (--copyCount);
}


static
void ThreeOrderCopyLine16To16(void* d, const void* s, ThreeOrderCopyInfo* info) 
{
    int srcWidth = (int)info->srcWidth;
    int copyCount = info->copyCount;
    int x_16 = info->x_16;
	int y_16 = info->y_16;
	int dsx = info->dsx;
    unsigned av_8[4];
    unsigned v_8 = ((unsigned)(y_16 << 16)) >> 24;
    unsigned short* dst = (unsigned short*)d;
	s = (void*)((short*)s + ((y_16 >> 16) - 1) * info->srcWidth - 1);
	av_8[0] = SinXDivX_Table_8[(1 << 8) + v_8];
	av_8[1] = SinXDivX_Table_8[v_8];
	av_8[2] = SinXDivX_Table_8[(1 << 8) - v_8];
    av_8[3] = SinXDivX_Table_8[(2 << 8) - v_8];
    do {
		unsigned au_8[4];
		unsigned u_8 = ((unsigned)(x_16 << 16)) >> 24;
		unsigned short* src = ((unsigned short*)s) + (x_16 >> 16);
		x_16 += dsx;
		
        au_8[0] = SinXDivX_Table_8[(1 << 8) + u_8];
        au_8[1] = SinXDivX_Table_8[u_8];
        au_8[2] = SinXDivX_Table_8[(1 << 8) - u_8];
        au_8[3] = SinXDivX_Table_8[(2 << 8) - u_8];
		
        unsigned sR = 0, sG = 0, sB = 0;
        for (unsigned i = 0; i < 4; ++i) {
            int aR, aG, aB;
            unsigned pixel0 = src[0];
            unsigned pixel1 = src[1];
            unsigned pixel2 = src[2];
            unsigned pixel3 = src[3];
            aR = au_8[0] * (pixel0 & 0xf800) + au_8[1] * (pixel1 & 0xf800) + au_8[2] * (pixel2 & 0xf800) + au_8[3] * (pixel3 & 0xf800);
            aG = au_8[0] * (pixel0 & 0x07e0) + au_8[1] * (pixel1 & 0x07e0) + au_8[2] * (pixel2 & 0x07e0) + au_8[3] * (pixel3 & 0x07e0);
            aB = au_8[0] * (pixel0 & 0x001f) + au_8[1] * (pixel1 & 0x001f) + au_8[2] * (pixel2 & 0x001f) + au_8[3] * (pixel3 & 0x001f);
            sR += aR * av_8[i];
            sG += aG * av_8[i];
            sB += aB * av_8[i];
            src += srcWidth;
        }
		sR = sR >> 27;
		sG = (sG << 5) >> 26;
		sB = (sB << 11) >> 27;
        *dst++ = sB | (sG << 5) | (sR << 11);
    } while (--copyCount);
	
}

static
void ThreeOrderCopyLine24To16(void* d, const void* s, ThreeOrderCopyInfo* info) 
{
    int srcWidth = (int)info->srcWidth;
    int copyCount = info->copyCount;
    int x_16 = info->x_16;
	int y_16 = info->y_16;
	int dsx = info->dsx;
    unsigned av_8[4];
    unsigned v_8 = ((unsigned)(y_16 << 16)) >> 24;
    unsigned short* dst = (unsigned short*)d;
	s = (void*)((unsigned*)s + ((y_16 >> 16) - 1) * info->srcWidth - 1);
	av_8[0] = SinXDivX_Table_8[(1 << 8) + v_8];
	av_8[1] = SinXDivX_Table_8[v_8];
	av_8[2] = SinXDivX_Table_8[(1 << 8) - v_8];
    av_8[3] = SinXDivX_Table_8[(2 << 8) - v_8];
    do {
		unsigned au_8[4];
		unsigned u_8 = ((unsigned)(x_16 << 16)) >> 24;
		unsigned* src = ((unsigned*)s) + (x_16 >> 16);
		x_16 += dsx;
		
        au_8[0] = SinXDivX_Table_8[(1 << 8) + u_8];
        au_8[1] = SinXDivX_Table_8[u_8];
        au_8[2] = SinXDivX_Table_8[(1 << 8) - u_8];
        au_8[3] = SinXDivX_Table_8[(2 << 8) - u_8];
		
        unsigned sR = 0, sG = 0, sB = 0;
        for (unsigned i = 0; i < 4; ++i) {
            unsigned aR, aG, aB;
            Color32* pixel = (Color32*)src;
            aR = au_8[0] * pixel[0].r + au_8[1] * pixel[1].r + au_8[2] * pixel[2].r + au_8[3] * pixel[3].r;
            aG = au_8[0] * pixel[0].g + au_8[1] * pixel[1].g + au_8[2] * pixel[2].g + au_8[3] * pixel[3].g;
            aB = au_8[0] * pixel[0].b + au_8[1] * pixel[1].b + au_8[2] * pixel[2].b + au_8[3] * pixel[3].b;			
			sR += aR * av_8[i];
            sG += aG * av_8[i];
            sB += aB * av_8[i];
            src += srcWidth;
        }
		sR = sR >> 19;
		sG = sG >> 18;
		sB = sB >> 19;
        *dst++ = sB | (sG << 5) | (sR << 11);
    } while (--copyCount);
	
}

static
void ThreeOrderCopyLine8To32(void* d, const void* s, ThreeOrderCopyInfo* info) 
{
#if 0
    int srcWidth = (int)info->srcWidth;
    int copyCount = info->copyCount;
    int x_16 = info->x_16;
	int y_16 = info->y_16;
	int dsx = info->dsx;
    unsigned av_8[4];
	unsigned* palette = (unsigned*)info->palette;
    unsigned v_8 = ((unsigned)(y_16 << 16)) >> 24;
    unsigned* dst = (unsigned*)d;
	unsigned mask = s_mask_00ff00ff;
	s = (void*)((char*)s + ((y_16 >> 16) - 1) * srcWidth - 1);
	
	av_8[0] = SinXDivX_Table_8[(1 << 8) + v_8];
	av_8[1] = SinXDivX_Table_8[v_8];
	av_8[2] = SinXDivX_Table_8[(1 << 8) - v_8];
    av_8[3] = SinXDivX_Table_8[(2 << 8) - v_8];
	
    do {
		unsigned au_8[4];
		unsigned u_8 = ((unsigned)(x_16 << 16)) >> 24;
		unsigned char* src = ((unsigned char*)s) + (x_16 >> 16);
		x_16 += dsx;
        au_8[0] = SinXDivX_Table_8[(1 << 8) + u_8];
        au_8[1] = SinXDivX_Table_8[u_8];
        au_8[2] = SinXDivX_Table_8[(1 << 8) - u_8];
        au_8[3] = SinXDivX_Table_8[(2 << 8) - u_8];
		unsigned rb = 0, ag = 0;

        for (unsigned i = 0; i < 4; ++i) {
			unsigned av = av_8[i];
            unsigned pixel0 = *(unsigned*)(palette + src[0]);
            unsigned pixel1 = *(unsigned*)(palette + src[1]);
            unsigned pixel2 = *(unsigned*)(palette + src[2]);
            unsigned pixel3 = *(unsigned*)(palette + src[3]);
			unsigned t0 = (au_8[0] * av + 64) >> 8;
			unsigned t1 = (au_8[1] * av + 64) >> 8;
			unsigned t2 = (au_8[2] * av + 64) >> 8;
			unsigned t3 = (au_8[3] * av + 64) >> 8;

			rb += t0 * (pixel0 & mask) + 
				  t1 * (pixel1 & mask) + 
				  t2 * (pixel2 & mask) + 
				  t3 * (pixel3 & mask);
			ag += t0 * ((pixel0 >> 8) & mask) + 
				  t1 * ((pixel1 >> 8) & mask) +
				  t2 * ((pixel2 >> 8) & mask) +
				  t3 * ((pixel3 >> 8) & mask);
            src += srcWidth;
        }

        *dst++ = (ag & 0xff00) | ((rb >> 8) & mask) | 0xff000000;
    } while (--copyCount);
#else
    int srcWidth = (int)info->srcWidth;
    int copyCount = info->copyCount;
    int x_16 = info->x_16;
	int y_16 = info->y_16;
	int dsx = info->dsx;
    unsigned av_8[4];
	unsigned* palette = (unsigned*)info->palette;
    unsigned v_8 = ((unsigned)(y_16 << 16)) >> 24;
    unsigned* dst = (unsigned*)d;
	s = (void*)((char*)s + ((y_16 >> 16) - 1) * info->srcWidth - 1);
	av_8[0] = SinXDivX_Table_8[(1 << 8) + v_8];
	av_8[1] = SinXDivX_Table_8[v_8];
	av_8[2] = SinXDivX_Table_8[(1 << 8) - v_8];
    av_8[3] = SinXDivX_Table_8[(2 << 8) - v_8];
    do {
		unsigned au_8[4];
		unsigned u_8 = ((unsigned)(x_16 << 16)) >> 24;
		unsigned char* src = ((unsigned char*)s) + (x_16 >> 16);
		x_16 += dsx;
		
        au_8[0] = SinXDivX_Table_8[(1 << 8) + u_8];
        au_8[1] = SinXDivX_Table_8[u_8];
        au_8[2] = SinXDivX_Table_8[(1 << 8) - u_8];
        au_8[3] = SinXDivX_Table_8[(2 << 8) - u_8];
		
        unsigned sR = 0, sG = 0, sB = 0;
        for (unsigned i = 0; i < 4; ++i) {
            int aR, aG, aB;
            Color32* pixel0 = (Color32*)(palette + src[0]);
            Color32* pixel1 = (Color32*)(palette + src[1]);
            Color32* pixel2 = (Color32*)(palette + src[2]);
            Color32* pixel3 = (Color32*)(palette + src[3]);
            aR = au_8[0] * pixel0->r + au_8[1] * pixel1->r + au_8[2] * pixel2->r + au_8[3] * pixel3->r;
            aG = au_8[0] * pixel0->g + au_8[1] * pixel1->g + au_8[2] * pixel2->g + au_8[3] * pixel3->g;
            aB = au_8[0] * pixel0->b + au_8[1] * pixel1->b + au_8[2] * pixel2->b + au_8[3] * pixel3->b;
            sR += aR * av_8[i];
            sG += aG * av_8[i];
            sB += aB * av_8[i];
            src += srcWidth;
        }
		sR = sR >> 16;
		sG = sG >> 16;
		sB = sB >> 16;
        *dst++ = 0xff000000 | (sR << 16) | (sG << 8) | sB;
    } while (--copyCount);

#endif
}

static
void ThreeOrderCopyLine16To32(void* d, const void* s, ThreeOrderCopyInfo* info) 
{
    int srcWidth = (int)info->srcWidth;
    int copyCount = info->copyCount;
    int x_16 = info->x_16;
	int y_16 = info->y_16;
	int dsx = info->dsx;
    unsigned av_8[4];
    unsigned v_8 = ((unsigned)(y_16 << 16)) >> 24;
    unsigned* dst = (unsigned*)d;
	s = (void*)((short*)s + ((y_16 >> 16) - 1) * info->srcWidth - 1);
	av_8[0] = SinXDivX_Table_8[(1 << 8) + v_8];
	av_8[1] = SinXDivX_Table_8[v_8];
	av_8[2] = SinXDivX_Table_8[(1 << 8) - v_8];
    av_8[3] = SinXDivX_Table_8[(2 << 8) - v_8];
    do {
		unsigned au_8[4];
		unsigned u_8 = ((unsigned)(x_16 << 16)) >> 24;
		unsigned short* src = ((unsigned short*)s) + (x_16 >> 16);
		x_16 += dsx;
		
        au_8[0] = SinXDivX_Table_8[(1 << 8) + u_8];
        au_8[1] = SinXDivX_Table_8[u_8];
        au_8[2] = SinXDivX_Table_8[(1 << 8) - u_8];
        au_8[3] = SinXDivX_Table_8[(2 << 8) - u_8];
		
        unsigned sR = 0, sG = 0, sB = 0;
        for (unsigned i = 0; i < 4; ++i) {
            int aR, aG, aB;
            unsigned pixel0 = src[0];
            unsigned pixel1 = src[1];
            unsigned pixel2 = src[2];
            unsigned pixel3 = src[3];
            aR = au_8[0] * (pixel0 & 0xf800) + au_8[1] * (pixel1 & 0xf800) + au_8[2] * (pixel2 & 0xf800) + au_8[3] * (pixel3 & 0xf800);
            aG = au_8[0] * (pixel0 & 0x07e0) + au_8[1] * (pixel1 & 0x07e0) + au_8[2] * (pixel2 & 0x07e0) + au_8[3] * (pixel3 & 0x07e0);
            aB = au_8[0] * (pixel0 & 0x001f) + au_8[1] * (pixel1 & 0x001f) + au_8[2] * (pixel2 & 0x001f) + au_8[3] * (pixel3 & 0x001f);
            sR += aR * av_8[i];
            sG += aG * av_8[i];
            sB += aB * av_8[i];
            src += srcWidth;
        }
		sR = sR >> 24;
		sG = (sG << 5) >> 24;
		sB = (sB << 11) >> 24;
        *dst++ = 0xff000000 | (sR << 16) | (sG << 8) | sB;
    } while (--copyCount);
}

static
void ThreeOrderCopyLine32To32(void* d, const void* s, ThreeOrderCopyInfo* info) 
{
    int srcWidth = (int)info->srcWidth;
    int copyCount = info->copyCount;
    int x_16 = info->x_16;
	int y_16 = info->y_16;
	int dsx = info->dsx;
    unsigned av_8[4];
    unsigned v_8 = ((unsigned)(y_16 << 16)) >> 24;
    unsigned* dst = (unsigned*)d;
	unsigned mask = s_mask_00ff00ff;
	s = (void*)((unsigned*)s + ((y_16 >> 16) - 1) * info->srcWidth - 1);
	av_8[0] = SinXDivX_Table_8[(1 << 8) + v_8];
	av_8[1] = SinXDivX_Table_8[v_8];
	av_8[2] = SinXDivX_Table_8[(1 << 8) - v_8];
    av_8[3] = SinXDivX_Table_8[(2 << 8) - v_8];
    do {
		unsigned au_8[4];
		unsigned u_8 = ((unsigned)(x_16 << 16)) >> 24;
		unsigned* src = ((unsigned*)s) + (x_16 >> 16);
		x_16 += dsx;
        au_8[0] = SinXDivX_Table_8[(1 << 8) + u_8];
        au_8[1] = SinXDivX_Table_8[u_8];
        au_8[2] = SinXDivX_Table_8[(1 << 8) - u_8];
        au_8[3] = SinXDivX_Table_8[(2 << 8) - u_8];
		unsigned rb = 0, ag = 0;
        for (unsigned i = 0; i < 4; ++i) {
			unsigned av = av_8[i];
			unsigned t0 = (au_8[0] * av) >> 8;
			unsigned t1 = (au_8[1] * av) >> 8;
			unsigned t2 = (au_8[2] * av) >> 8;
			unsigned t3 = (au_8[3] * av) >> 8;
			rb += t0 * (src[0] & mask) + 
				  t1 * (src[1] & mask) + 
				  t2 * (src[2] & mask) + 
				  t3 * (src[3] & mask);
			ag += t0 * ((src[0] >> 8) & mask) + 
				  t1 * ((src[1] >> 8) & mask) +
				  t2 * ((src[2] >> 8) & mask) +
				  t3 * ((src[3] >> 8) & mask);
            src += srcWidth;
        }
        *dst++ = (ag & ~mask) | ((rb >> 8) & mask);
    } while (--copyCount);
}


static
void ThreeOrderCopyLine32ToPM32(void* d, const void* s, ThreeOrderCopyInfo* info) 
{
#if 0
	int srcWidth = (int)info->srcWidth;
    int copyCount = info->copyCount;
    int x_16 = info->x_16;
	int y_16 = info->y_16;
	int dsx = info->dsx;
    unsigned av_8[4];
    unsigned v_8 = ((unsigned)(y_16 << 16)) >> 24;
    unsigned* dst = (unsigned*)d;
	unsigned mask = s_mask_00ff00ff;
	s = (void*)((unsigned*)s + ((y_16 >> 16) - 1) * info->srcWidth - 1);
	av_8[0] = SinXDivX_Table_8[(1 << 8) + v_8];
	av_8[1] = SinXDivX_Table_8[v_8];
	av_8[2] = SinXDivX_Table_8[(1 << 8) - v_8];
    av_8[3] = SinXDivX_Table_8[(2 << 8) - v_8];
    do {
		unsigned au_8[4];
		unsigned u_8 = ((unsigned)(x_16 << 16)) >> 24;
		unsigned* src = ((unsigned*)s) + (x_16 >> 16);
		x_16 += dsx;
        au_8[0] = SinXDivX_Table_8[(1 << 8) + u_8];
        au_8[1] = SinXDivX_Table_8[u_8];
        au_8[2] = SinXDivX_Table_8[(1 << 8) - u_8];
        au_8[3] = SinXDivX_Table_8[(2 << 8) - u_8];
		unsigned rb = 0, ag = 0;
        for (unsigned i = 0; i < 4; ++i) {
			unsigned av = av_8[i];
			unsigned t0 = (au_8[0] * av + 64) >> 8;
			unsigned t1 = (au_8[1] * av + 64) >> 8;
			unsigned t2 = (au_8[2] * av + 64) >> 8;
			unsigned t3 = (au_8[3] * av + 64) >> 8;
			rb += t0 * (src[0] & mask) + 
				t1 * (src[1] & mask) + 
				t2 * (src[2] & mask) + 
				t3 * (src[3] & mask);
			ag += t0 * ((src[0] >> 8) & mask) + 
				t1 * ((src[1] >> 8) & mask) +
				t2 * ((src[2] >> 8) & mask) +
				t3 * ((src[3] >> 8) & mask);
            src += srcWidth;
        }
		
        unsigned alpha = ag >> 24;
        rb = ((rb >> 8) & mask) * alpha;
        ag = (ag & 0xff00) * alpha;
        *dst++ = (alpha << 24) | ((ag & 0xff0000) >> 8) | ((rb >> 8) & mask);

    } while (--copyCount);

#else
    int srcWidth = (int)info->srcWidth;
    int copyCount = info->copyCount;
    int x_16 = info->x_16;
	int y_16 = info->y_16;
	int dsx = info->dsx;
    int av_8[4];
	
    unsigned v_8 = ((unsigned)(y_16 << 16)) >> 24;
    unsigned* dst = (unsigned*)d;
	s = (void*)((unsigned*)s + ((y_16 >> 16) - 1) * info->srcWidth - 1);
	
	av_8[0] = SinXDivX_Table_8[(1 << 8) + v_8];
	av_8[1] = SinXDivX_Table_8[v_8];
	av_8[2] = SinXDivX_Table_8[(1 << 8) - v_8];
    av_8[3] = SinXDivX_Table_8[(2 << 8) - v_8];
	
    do {
		unsigned au_8[4];
        unsigned color;
		unsigned u_8 = ((unsigned)(x_16 << 16)) >> 24;
		unsigned* src = ((unsigned*)s) + (x_16 >> 16);
		x_16 += dsx;
		
        //
        au_8[0] = SinXDivX_Table_8[(1 << 8) + u_8];
        au_8[1] = SinXDivX_Table_8[u_8];
        au_8[2] = SinXDivX_Table_8[(1 << 8) - u_8];
        au_8[3] = SinXDivX_Table_8[(2 << 8) - u_8];
		
        unsigned sR = 0, sG = 0, sB = 0, sA = 0;
        for (unsigned i = 0; i < 4; ++i) {
            unsigned aA, aR, aG, aB;
            Color32* pixel = (Color32*)src;
            aA = au_8[0] * pixel[0].a + au_8[1] * pixel[1].a + au_8[2] * pixel[2].a + au_8[3] * pixel[3].a;
            aR = au_8[0] * pixel[0].r + au_8[1] * pixel[1].r + au_8[2] * pixel[2].r + au_8[3] * pixel[3].r;
            aG = au_8[0] * pixel[0].g + au_8[1] * pixel[1].g + au_8[2] * pixel[2].g + au_8[3] * pixel[3].g;
            aB = au_8[0] * pixel[0].b + au_8[1] * pixel[1].b + au_8[2] * pixel[2].b + au_8[3] * pixel[3].b;
            sA += aA * av_8[i];
            sR += aR * av_8[i];
            sG += aG * av_8[i];
            sB += aB * av_8[i];
            src += srcWidth;
        }
		sA = sA >> 16;
		sR = sR >> 16;
		sG = sG >> 16;
		sB = sB >> 16;
// 		sA = color_table[(sA >> 16) * 266 / 256];
// 		sR = color_table[(sR >> 16) * 266 / 256];
// 		sG = color_table[(sG >> 16) * 266 / 256];
// 		sB = color_table[(sB >> 16) * 266 / 256];
		sB |= (sR << 16);
		sG = ((sG * sA) & 0xff00);
		color = sG | (sA << 24) | (((sB * sA) & 0xff00ff00) >> 8);

        *dst++ = color;
    } while (--copyCount);
#endif
}

static void ThreeOrder_Border_Common(void* dst, int cnt, long x_16, long y_16, ThreeOrderCopyInfo* info)
{
	void* pixel = info->borderPixels;
	int dsx = info->dsx;
	int dstep = 1 << info->dshift;
	info->y_16 = ((unsigned short)y_16);
	y_16 >>= 16;
	do {
		if (info->getPixelBorder(info->src, pixel, (x_16 >> 16), y_16)) {
			info->x_16 = (unsigned short)x_16;
			info->copyLine(dst, pixel, info);
		}
		x_16 += dsx;
		dst = (void*)((char*)dst + dstep);
	} while (--cnt);
}

static void ThreeOrder_Fast_Common(void* dst, const long x_16, const long y_16, ThreeOrderCopyInfo* info)
{
	const ZMAEE_BitmapInfo* srcBmp = info->src;
	char* src = (char*)srcBmp->pBits;
	info->x_16 = x_16;
	info->y_16 = y_16;
	info->copyLine(dst, (void*)src, info);
}

// 
// void ThreeOrder_Border_Common(const ZMAEE_BitmapInfo& pic, const long x_16, const long y_16, void* result)
// {
//     long x0_sub1 = (x_16 >> 16) - 1;
//     long y0_sub1 = (y_16 >> 16) - 1;
//     unsigned long u_16_add1 = ((unsigned short)(x_16)) + (1 << 16);
//     unsigned long v_16_add1 = ((unsigned short)(y_16)) + (1 << 16);
// 
//     Color32 pixel[16];
//     long i;
// 
// 
//     for (i = 0; i < 4; ++i) {
//         long y = y0_sub1 + i;
//         pixel[i * 4 + 0] = pic.getPixelsBorder(x0_sub1 + 0, y);
//         pixel[i * 4 + 1] = pic.getPixelsBorder(x0_sub1 + 1, y);
//         pixel[i * 4 + 2] = pic.getPixelsBorder(x0_sub1 + 2, y);
//         pixel[i * 4 + 3] = pic.getPixelsBorder(x0_sub1 + 3, y);
//     }
// 
//     TPixels32Ref npic;
//     npic.pdata     = &pixel[0];
//     npic.byte_width = 4 * sizeof(Color32);
//     //npic.width     =4;
//     //npic.height    =4;
//     ThreeOrder_Fast_Common(npic, u_16_add1, v_16_add1, result);
// }


void PicZoom_ThreeOrder_Common(const ZMAEE_BitmapInfo& src, const ZMAEE_BitmapInfo& dst)
{
    int dsx;
    int dsy;
    int dstW;
    int dstH;
    int srcW;
    int srcH;
	int sx_start;
	int sy_start;
    if ((dstW = dst.width) <= 0)
        return;
    if ((dstH = dst.height) <= 0)
        return;
    if ((srcW = src.width) <= 0)
        return;
    if ((srcH = src.height) <= 0)
        return;


	if (dstW > srcW) {
		// 放大
		dsx = ((srcW - 1) << 16) / (dstW - 1);
		sx_start = 0;
	} else {
		// 缩小
		dsx = (srcW << 16) / dstW;
		sx_start = (dsx >> 1) - (1 << 15);
		if (dstW == 1) {
			dstW = 1;
		}
	}
	
	if (dstH > srcH) {
		// 放大
		dsy = ((srcH - 1) << 16) / (dstH - 1);
		sy_start = 0;
	} else {
		// 缩小
		dsy = (srcH << 16) / dstH;
		sy_start = (dsy >> 1) - (1 << 15);
	}

    unsigned sshift;
    unsigned dshift;

    FuncThreeOrderCopyLine copyLine;
	FuncGetPixelBorder getPixelBorder;
    ThreeOrderCopyInfo copyInfo;
    ThreeOrderCopyInfo copyInfoBorder;
    unsigned prePalette[256];
	unsigned borderPixels[16];

    if (src.nDepth == ZMAEE_COLORFORMAT_8) {
        unsigned* dPal;
        const unsigned short* sPal;
        unsigned cnt;
        dPal = (unsigned*)prePalette;
        copyInfo.palette = (const void*)dPal;
        copyInfoBorder.palette = (const void*)dPal;
        cnt = src.nPaletteLen / 2;
        sPal = (const unsigned short*)src.pPalette;
        while (cnt-- > 0) {
            unsigned c = *sPal++;
#if 1
			unsigned r = c & 0xf800;
			unsigned b = c & 0x001f;
			unsigned g = (c << 5) & 0xfc00; 
			r = (r << 8) | ((r & 0xe000) << 3);
			b = (b << 3) | (b >> 2);
			g = g | ((g & 0xc000) >> 6);
            *dPal++ = 0xff000000 | g | r | b;
#else
			*dPal++ = ZMAEE_BMP16TO32(c);
#endif
        }
        sshift = 0;
        if (dst.nDepth == ZMAEE_COLORFORMAT_16) {
            dshift = 1;
            copyLine = ThreeOrderCopyLine8To16;
        } else if (
            dst.nDepth == ZMAEE_COLORFORMAT_24 ||
            dst.nDepth == ZMAEE_COLORFORMAT_32 ||
            dst.nDepth == ZMAEE_COLORFORMAT_PM32) {
            dshift = 2;
            copyLine = ThreeOrderCopyLine8To32;
        } else {
            return;
        }
		getPixelBorder = getPixelBorder8;
    } else if (src.nDepth == ZMAEE_COLORFORMAT_16) {
        sshift = 1;
        if (dst.nDepth == ZMAEE_COLORFORMAT_16) {
            dshift = 1;
            copyLine = ThreeOrderCopyLine16To16;
        } else if (
            dst.nDepth == ZMAEE_COLORFORMAT_24 ||
            dst.nDepth == ZMAEE_COLORFORMAT_32 ||
            dst.nDepth == ZMAEE_COLORFORMAT_PM32) {
            dshift = 2;
            copyLine = ThreeOrderCopyLine16To32;
        } else {
            return;
        }
		getPixelBorder = getPixelBorder16;
    } else if (src.nDepth == ZMAEE_COLORFORMAT_24) {
        sshift = 2;
        if (dst.nDepth == ZMAEE_COLORFORMAT_16) {
            dshift = 1;
            copyLine = ThreeOrderCopyLine24To16;
        } else if (
            dst.nDepth == ZMAEE_COLORFORMAT_24 ||
            dst.nDepth == ZMAEE_COLORFORMAT_32 ||
            dst.nDepth == ZMAEE_COLORFORMAT_PM32
        ) {
            dshift = 2;
            copyLine = ThreeOrderCopyLine32To32;
        } else {
            return;
        }
		getPixelBorder = getPixelBorder32;
    } else if (src.nDepth == ZMAEE_COLORFORMAT_32) {
        sshift = 2;
        if (dst.nDepth == ZMAEE_COLORFORMAT_32) {
            dshift = 2;
            copyLine = ThreeOrderCopyLine32To32;
        } else if (dst.nDepth == ZMAEE_COLORFORMAT_PM32) {
            dshift = 2;
            copyLine = ThreeOrderCopyLine32ToPM32;
        } else {
            return;
        }
		getPixelBorder = getPixelBorder32;
    } else if (src.nDepth == ZMAEE_COLORFORMAT_PM32) {
        sshift = 2;
        if (dst.nDepth == ZMAEE_COLORFORMAT_PM32) {
            dshift = 2;
            copyLine = ThreeOrderCopyLine32To32;
        } else {
            return;
        }
		getPixelBorder = getPixelBorder32;
    } else {
        return;
    }

	copyInfo.src = &src;
	copyInfo.copyLine = copyLine;
	copyInfo.dshift = dshift;
	copyInfo.srcWidth = src.width;
	copyInfo.dsx = dsx;
	zmaee_memcpy(&copyInfoBorder, &copyInfo, sizeof(copyInfo));
	copyInfoBorder.copyCount = 1;
	copyInfoBorder.srcWidth = 4;
	copyInfoBorder.borderPixels = (void*)&borderPixels[5];
	copyInfoBorder.getPixelBorder = getPixelBorder;

    void* pdstLine = dst.pBits;
    int srcy_16 = sy_start;
	int pass_y = 0;
    // top
	do {
		int t = dstW;
		int x_16 = sx_start;
		copyInfoBorder.y_16 = ((unsigned short)srcy_16);
		do {
			if (getPixelBorder(&src, copyInfoBorder.borderPixels, (x_16 >> 16), srcy_16 >> 16)) {
				copyInfoBorder.x_16 = (unsigned short)x_16;
				copyLine(pdstLine, copyInfoBorder.borderPixels, &copyInfoBorder);
			}
			x_16 += dsx;
			pdstLine = (void*)((char*)pdstLine + (1 << dshift));
		} while (--t);

		if (++pass_y >= dstH)
			return;
		srcy_16 += dsy;
	} while (srcy_16 < (1 << 16));
	
	
	// 
	while ((srcy_16 >> 16) + 2 < srcH) {

		int pass_x = 0;
		int x_16 = sx_start;
		copyInfoBorder.y_16 = ((unsigned short)srcy_16);
		// left
		do {
			if (getPixelBorder(&src, copyInfoBorder.borderPixels, x_16 >> 16, srcy_16 >> 16)) {
				copyInfoBorder.x_16 = (unsigned short)x_16;
				copyLine(pdstLine, copyInfoBorder.borderPixels, &copyInfoBorder);
			}
			pdstLine = (void*)((char*)pdstLine + (1 << dshift));
			if (++pass_x >= dstW)
				goto next_line;
			x_16 += dsx;
		} while (x_16 < (1 << 16));		
		
		// center
		copyInfo.x_16 = x_16;
		copyInfo.y_16 = srcy_16;
		{
			int pass_x_old = pass_x;
			while ((x_16 >> 16) + 2 < srcW) {
				if (++pass_x >= dstW)
					goto next_line;
				x_16 += dsx;
			}
			if (pass_x > pass_x_old) {
				copyInfo.copyCount = pass_x - pass_x_old;
				copyLine(pdstLine, src.pBits, &copyInfo);
				pdstLine = (void*)((char*)pdstLine + ((pass_x - pass_x_old) << dshift));
			}
		}

		// right
		for (; pass_x < dstW; ++pass_x) {
			if (getPixelBorder(&src, copyInfoBorder.borderPixels, x_16 >> 16, srcy_16 >> 16)) {
				copyInfoBorder.x_16 = (unsigned short)x_16;
				copyLine(pdstLine, copyInfoBorder.borderPixels, &copyInfoBorder);
			}
			x_16 += dsx;
			pdstLine = (void*)((char*)pdstLine + (1 << dshift));
		} 
next_line:
		if (++pass_y >= dstH)
			return;
		srcy_16 += dsy;
	}

	// bottom
	for (; pass_y < dstH; ++pass_y) {
		ThreeOrder_Border_Common(
			(void*)pdstLine, 
			dstW,
			sx_start, 
			srcy_16,
			&copyInfoBorder
			);
        srcy_16 += dsy;
        pdstLine = (void*)((char*)pdstLine + (dstW << dshift));
	}
}

void _ScaleBilinear(ZMAEE_BitmapInfo& src, ZMAEE_BitmapInfo& dst)
{
	
	if (dst.width >= src.width && dst.height >= src.height) { 
		// 放大用二次线性差值
		_ScaleBilinearFast(src, dst);
	} else {
		// 缩小用三次卷积差值
		PicZoom_ThreeOrder_Common(src, dst);
	}

}


#endif // USE_OLD_VERSION




