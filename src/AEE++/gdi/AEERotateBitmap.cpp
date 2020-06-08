

#include "zmaee.h"
#include "zmaee_stdlib.h"
#include "zmaee_helper.h"
#include "zmaee_shell.h"
#include "zmaee_util.h"
#include "AEERotateBitmap.h"

#define ZMAEE_BMP24TO16(r, g, b) ((((r) & 0xF8)<<8) | (((g) & 0xFC)<<3) | (((b) & 0xF8)>>3))
#define ZMAEE_BMP32TO16(rgb) ((((rgb) & 0xF80000) >> 8) | (((rgb) & 0xFC00) >> 5) | (((rgb) & 0xF8) >> 3))
//#define ZMAEE_BMP16TO32(c) (0xFF000000 | (((c) << 8) & 0xF80000) | (((c) << 5) & 0xFC00) | (((c) << 3) & 0xF8))
#define ZMAEE_BMP16TO32(c) ((0xFF000000) | (((c) & 0xF800) << 8) | (((c) & 0x07e0) << 5) | (((c) & 0x001F) << 3))


#define BORDER_WIDTH	1



#define ALPHA32T024(r, d32, s32) do {                                         \
	unsigned int t_s = (unsigned int)(s32);                                   \
	unsigned int t_sa = t_s >> 24;                                            \
	if (t_sa) {                                                               \
		if (t_sa != 255) {                                                    \
			unsigned int t_d = (unsigned int)(d32);                           \
			unsigned int t_srb = t_s & 0x00FF00FF;                        \
			unsigned int t_sg  = t_s & 0x0000FF00;                        \
			unsigned int t_drb = t_d & 0x00FF00FF;                        \
			unsigned int t_dg  = t_d & 0x0000FF00;                        \
			t_srb *= t_sa;                                                \
			t_sg  *= t_sa;                                                \
			t_sa = 256 - t_sa;                                            \
			t_drb *= t_sa;                                                \
			t_dg  *= t_sa;                                                \
			t_srb += t_drb;                                               \
			t_sg  += t_dg;                                                \
			t_srb &= 0xFF00FFFF;                                          \
			t_sg  &= 0x00FF0000;                                          \
			t_s = (t_srb | t_sg) >> 8;                                   \
		}                                                                     \
		r = t_s;                                                              \
	}                                                                         \
} while (0)

#define ALPHAP32T0P32(r, d32, s32) do {                                       \
	unsigned int t_s = (unsigned int)(s32);                                   \
	unsigned int t_sa = t_s >> 24;                                            \
	if (t_sa == 255) {                                                        \
	r = t_s;                                                              \
	} else if (t_sa != 0) {                                                   \
	unsigned int t_mask = s_mask_00ff00ff;							      \
	unsigned int t_d = (unsigned int)(d32);                               \
	unsigned int t_drb = t_d & t_mask;                                    \
	unsigned int t_dag = (t_d >> 8) & t_mask;                             \
	t_sa = 256 - t_sa;                                                    \
	t_drb = (t_drb * t_sa) & ~t_mask;                                     \
	t_dag = (t_dag * t_sa) & ~t_mask;									  \
	r = t_s + (t_dag | (t_drb >> 8));                                     \
	}                                                                         \
} while (0)

#define ALPHA32T0P32(r, d32, s32) do {                                        \
	unsigned t_s = (unsigned)(s32);                                           \
	unsigned t_a = t_s >> 24;                                                 \
	if (t_a == 255) {                                                         \
	r = t_s;                                                              \
	} else if (t_a != 0) {                                                    \
	unsigned t_mask = s_mask_00ff00ff;							          \
	unsigned t_d = (unsigned)(d32);                                       \
	unsigned t_rb = t_s & t_mask;                                         \
	unsigned t_ag = (t_s >> 8) & t_mask;                                  \
	t_rb = (t_rb * t_a) & ~t_mask;                                        \
	t_ag = (t_ag * t_a) & 0xff00;                                         \
	t_s = t_ag | (t_a << 24) | (t_rb >> 8);                               \
	t_a = 256 - t_a;                                                      \
	t_rb = t_d & t_mask;                                                  \
	t_ag = (t_d >> 8) & t_mask;                                           \
	t_rb = (t_rb * t_a) & ~t_mask;                                        \
	t_ag = (t_ag * t_a) & ~t_mask;									      \
	r = t_s + (t_ag | (t_rb >> 8));                                       \
	}                                                                         \
} while (0)


static const unsigned long s_mask_00ff00ff = 0x00ff00ff;

static const unsigned long s_mask_07e0f81f = 0x07e0f81f;


static const unsigned short g_table_65536_div_1_255[256] = {
0x0000,0xFFFF,0x8000,0x5555,0x4000,0x3333,0x2AAA,0x2492,0x2000,0x1C71,0x1999,0x1745,0x1555,0x13B1,0x1249,0x1111,
0x1000,0x0F0F,0x0E38,0x0D79,0x0CCC,0x0C30,0x0BA2,0x0B21,0x0AAA,0x0A3D,0x09D8,0x097B,0x0924,0x08D3,0x0888,0x0842,
0x0800,0x07C1,0x0787,0x0750,0x071C,0x06EB,0x06BC,0x0690,0x0666,0x063E,0x0618,0x05F4,0x05D1,0x05B0,0x0590,0x0572,
0x0555,0x0539,0x051E,0x0505,0x04EC,0x04D4,0x04BD,0x04A7,0x0492,0x047D,0x0469,0x0456,0x0444,0x0432,0x0421,0x0410,
0x0400,0x03F0,0x03E0,0x03D2,0x03C3,0x03B5,0x03A8,0x039B,0x038E,0x0381,0x0375,0x0369,0x035E,0x0353,0x0348,0x033D,
0x0333,0x0329,0x031F,0x0315,0x030C,0x0303,0x02FA,0x02F1,0x02E8,0x02E0,0x02D8,0x02D0,0x02C8,0x02C0,0x02B9,0x02B1,
0x02AA,0x02A3,0x029C,0x0295,0x028F,0x0288,0x0282,0x027C,0x0276,0x0270,0x026A,0x0264,0x025E,0x0259,0x0253,0x024E,
0x0249,0x0243,0x023E,0x0239,0x0234,0x0230,0x022B,0x0226,0x0222,0x021D,0x0219,0x0214,0x0210,0x020C,0x0208,0x0204,
0x0200,0x01FC,0x01F8,0x01F4,0x01F0,0x01EC,0x01E9,0x01E5,0x01E1,0x01DE,0x01DA,0x01D7,0x01D4,0x01D0,0x01CD,0x01CA,
0x01C7,0x01C3,0x01C0,0x01BD,0x01BA,0x01B7,0x01B4,0x01B2,0x01AF,0x01AC,0x01A9,0x01A6,0x01A4,0x01A1,0x019E,0x019C,
0x0199,0x0197,0x0194,0x0192,0x018F,0x018D,0x018A,0x0188,0x0186,0x0183,0x0181,0x017F,0x017D,0x017A,0x0178,0x0176,
0x0174,0x0172,0x0170,0x016E,0x016C,0x016A,0x0168,0x0166,0x0164,0x0162,0x0160,0x015E,0x015C,0x015A,0x0158,0x0157,
0x0155,0x0153,0x0151,0x0150,0x014E,0x014C,0x014A,0x0149,0x0147,0x0146,0x0144,0x0142,0x0141,0x013F,0x013E,0x013C,
0x013B,0x0139,0x0138,0x0136,0x0135,0x0133,0x0132,0x0130,0x012F,0x012E,0x012C,0x012B,0x0129,0x0128,0x0127,0x0125,
0x0124,0x0123,0x0121,0x0120,0x011F,0x011E,0x011C,0x011B,0x011A,0x0119,0x0118,0x0116,0x0115,0x0114,0x0113,0x0112,
0x0111,0x010F,0x010E,0x010D,0x010C,0x010B,0x010A,0x0109,0x0108,0x0107,0x0106,0x0105,0x0104,0x0103,0x0102,0x0101
};


inline void ALPHA32T032(unsigned int& r, unsigned int d32, unsigned int s32) {
	unsigned int t_s = (unsigned int)(s32);
	unsigned int t_sa = t_s >> 24;
	if (t_sa) {
		if (t_sa != 255) {
			unsigned int _dv = (unsigned int)(d32);
			unsigned int t_da = _dv >> 24;
			if (t_da) {
				unsigned int t_srb = t_s & 0x00FF00FF;
				unsigned int t_sg  = t_s & 0x0000FF00;
				unsigned int t_drb = _dv & 0x00FF00FF;
				unsigned int t_dg  = _dv & 0x0000FF00;
				if (t_da == 255) {
					t_s = 0xFF000000;
				} else {
					unsigned int t_dasa = t_da * t_sa;
					unsigned int t_a = t_sa + t_da;
					t_a = (t_a << 8) - t_dasa;
					t_a >>= 8;
					t_sa = (t_sa * g_table_65536_div_1_255[t_a]) >> 8;
					t_s = t_a << 24;
				}
				t_srb *= t_sa;
				t_sg  *= t_sa;
				t_da = 256 - t_sa;
				t_drb *= t_da;
				t_dg  *= t_da;
				t_srb += t_drb;
				t_sg  += t_dg;
				t_srb &= 0xFF00FFFF;
				t_sg  &= 0x00FF0000;
				t_s |= (t_srb | t_sg) >> 8;
			}
		}
		r = t_s;
	}
}

__inline static ZMAEE_Fixed
__fixed_mul(ZMAEE_Fixed a, ZMAEE_Fixed b)
{
#if defined(WIN32)
	return (ZMAEE_Fixed)((((__int64)a) * ((__int64)b)) >> 16);
#elif defined(__COOLSAND__) || defined(__ANDROID__)
	return (ZMAEE_Fixed)((((long long)a) * ((long long)b)) >> 16);
#else
	ZMAEE_Fixed ret;
	int __x, __y;
	// r0:低位, r1:高位
	__asm {
		SMULL __x, __y, a, b
		MOV __x, __x, LSR #16
		ADD ret, __x, __y, LSL #(32 - 16)
	}
	return ret;
#endif
}

/* ------------------------------------------------------------------------- */

#define inline __inline
#define bool int
#define true 1
#define false 0

typedef struct TRotaryClipData TRotaryClipData;
struct TRotaryClipData {
	zmaee_context_copy_ex context;
	zmaee_context_copy_ex border_context;
	unsigned int sshift;
	unsigned int dshift;
    long src_width;
    long src_height;
    long dst_width;
    long dst_height;

	long clip_left;
	long clip_right;
	
    long Ax_16;
    long Ay_16;
    long Bx_16;
    long By_16;
    long Cx_16;
    long Cy_16;

//    long border_width; // 插值边界宽度
	
    long cur_dst_up_x0;
    long cur_dst_up_x1;
    long cur_dst_down_x0;
    long cur_dst_down_x1;

    long out_dst_up_y;
    long out_dst_down_y;

    long out_dst_x0_boder;
    long out_dst_x0_in;
    long out_dst_x1_in;
    long out_dst_x1_boder;

	long src_border_data[4];


	ZMAEE_COPY_EX_FUNC copy_fuc;
};


static void
zmaee_bd_copy_8_to_16(zmaee_context_copy_ex* context)
{
	unsigned int	count		= context->count;
	unsigned short*	dst			= (unsigned short*)context->dst;
	unsigned char*	src			= (unsigned char*)context->src;
	int				sx			= context->sx;
	int				sy			= context->sy;
	int				sdx			= context->sdx;
	int				sdy			= context->sdy;
	unsigned int	rowpixels	= context->rowpixels;
	unsigned int	transcolor	= context->transcolor;
	unsigned short	*palette	= (unsigned short*)context->palette;

	if (count > 0) {
		do {
			unsigned int index;
			index = rowpixels * (sy >> 16);
			index += (sx >> 16);
			index = src[index];
			index = palette[index];
			if (index != transcolor)
				*dst = index;
			++dst;
			sx += sdx;
			sy += sdy;
		} while (--count);
	}
}

static void
zmaee_bd_copy_8_to_32(zmaee_context_copy_ex* context)
{
	unsigned int	count		= context->count;
	unsigned int*	dst			= (unsigned int*)context->dst;
	unsigned char*	src			= (unsigned char*)context->src;
	int				sx			= context->sx;
	int				sy			= context->sy;
	int				sdx			= context->sdx;
	int				sdy			= context->sdy;
	unsigned int	rowpixels	= context->rowpixels;
	unsigned int	transcolor	= context->transcolor;
	unsigned short*	palette		= (unsigned short*)context->palette;

	if (count > 0) {
		do {
			unsigned int index;
			index = rowpixels * (sy >> 16);
			index += (sx >> 16);
			index = src[index];
			index = palette[index];
			if (index != transcolor)
				*dst = ZMAEE_BMP16TO32(index);
			++dst;
			sx += sdx;
			sy += sdy;
		} while (--count);
	}
}


static void
zmaee_bd_copy_16_to_16(zmaee_context_copy_ex* context)
{
	unsigned short*	dst		= (unsigned short*)context->dst;
	unsigned short*	src		= (unsigned short*)context->src;
	unsigned int	count	= context->count;
	int				sx		= context->sx;
	int				sy		= context->sy;
	int				sdx		= context->sdx;
	int				sdy		= context->sdy;
	unsigned int	rowpixels = context->rowpixels;
	unsigned int	transcolor = context->transcolor;

	if (count > 0) {
		do {
			unsigned int index;
			index = rowpixels * (sy >> 16);
			index += (sx >> 16);
			index = src[index];
			if (index != transcolor)
				*dst = index;
			++dst;
			sx += sdx;
			sy += sdy;
		} while (--count);
	}
}

static void
zmaee_bd_copy_16_to_32(zmaee_context_copy_ex* context)
{
	unsigned int*	dst		= (unsigned int*)context->dst;
	unsigned short*	src		= (unsigned short*)context->src;
	unsigned int	count	= context->count;
	int				sx		= context->sx;
	int				sy		= context->sy;
	int				sdx		= context->sdx;
	int				sdy		= context->sdy;
	unsigned int	rowpixels = context->rowpixels;
	unsigned int	transcolor	= context->transcolor;

	if (count > 0) {
		do {
			unsigned int index;
			unsigned int c;
			unsigned int t;
			index = rowpixels * (sy >> 16);
			index += (sx >> 16);
			index = src[index];
			c = index & 0xFF00;
			if (index != transcolor)  {
				c = 0xFF000000 | (c << 8);
				t = index & 0x07F0;
				c |= t << 5;
				index &= 0x001F;
				c |= index << 3;
				*dst = c;
			}
			++dst;
			sx += sdx;
			sy += sdy;
		} while (--count);
	}
}


static void
zmaee_bd_copy_32_to_16(zmaee_context_copy_ex* context)
{
	unsigned short*	dst		= (unsigned short*)context->dst;
	unsigned int*	src		= (unsigned int*)context->src;
	unsigned int	count	= context->count;
	int				sx		= context->sx;
	int				sy		= context->sy;
	int				sdx		= context->sdx;
	int				sdy		= context->sdy;
	unsigned int	rowpixels = context->rowpixels;

	if (count > 0) {
		do {
			unsigned int index;
			unsigned int a;

			index = rowpixels * (sy >> 16);
			index += (sx >> 16);
			index = src[index];

			sx += sdx;
			sy += sdy;

			a = index >> 27;

			if (a != 0) {

				unsigned int s;
				unsigned int t;
				unsigned int d;

				t = index & 0xF8;
				s = (index >> 8) & 0xF800;
				s |= t >> 3; /* red & blue */
				t = index & 0xFC00;

				if (a == 31) {
					s |= t >> 5;
				} else  {
					d = *dst;
					s |= t << 11;
					d |= d << 16;
					t = s_mask_07e0f81f;
					d &= t;
					s -= d;
					s *= a;
					s = d + (s >> 5);
					s &= t;
					s |= s >> 16;
				}
				*dst = s;
			}

			++dst;

		} while (--count);
	}
}


static void
zmaee_bd_copy_32_to_24(zmaee_context_copy_ex* context)
{
	unsigned int*	dst		= (unsigned int*)context->dst;
	unsigned int*	src		= (unsigned int*)context->src;
	unsigned int	count	= context->count;
	int				sx		= context->sx;
	int				sy		= context->sy;
	int				sdx		= context->sdx;
	int				sdy		= context->sdy;
	unsigned int	rowpixels = context->rowpixels;

	if (count > 0) {
		do {
			unsigned int index;
			index = rowpixels * (sy >> 16);
			index += (sx >> 16);
			index = src[index];
			ALPHA32T024(*dst, *dst, index);
			sx += sdx;
			sy += sdy;
			++dst;
		} while (--count);
	}
}

static void
zmaee_bd_copy_32_to_P32(zmaee_context_copy_ex* context)
{
	unsigned int*	dst		= (unsigned int*)context->dst;
	unsigned int*	src		= (unsigned int*)context->src;
	unsigned int	count	= context->count;
	int				sx		= context->sx;
	int				sy		= context->sy;
	int				sdx		= context->sdx;
	int				sdy		= context->sdy;
	unsigned int	rowpixels = context->rowpixels;
	
	if (count > 0) {
		do {
			unsigned int index;
			index = rowpixels * (sy >> 16);
			index += (sx >> 16);
			index = src[index];
			ALPHA32T0P32(*dst, *dst, index);
			sx += sdx;
			sy += sdy;
			++dst;
		} while (--count);
	}
}

static void
zmaee_bd_copy_32_to_32(zmaee_context_copy_ex *context)
{
	unsigned int*	dst		= (unsigned int*)context->dst;
	unsigned int*	src		= (unsigned int*)context->src;
	unsigned int	count	= context->count;
	int				sx		= context->sx;
	int				sy		= context->sy;
	int				sdx		= context->sdx;
	int				sdy		= context->sdy;
	unsigned int	rowpixels = context->rowpixels;

	if (count > 0) {
		do {
			unsigned int index;
			index = rowpixels * (sy >> 16);
			index += (sx >> 16);
			index = src[index];
			ALPHA32T032(*dst, *dst, index);
			++dst;
			sx += sdx;
			sy += sdy;

		} while (--count);
	}
}
static void 
zmaee_bd_copy_32_to_16_linear(zmaee_context_copy_ex *context)
{
	unsigned short*	dst		= (unsigned short*)context->dst;
	unsigned int*	src		= (unsigned int*)context->src;
	unsigned int	count	= context->count;
	int				sx		= context->sx;
	int				sy		= context->sy;
	int				sdx		= context->sdx;
	int				sdy		= context->sdy;
	unsigned int	rowpixels = context->rowpixels;
	
	if (count > 0) {
		do {
			unsigned int t;
			unsigned int fx, fy, fxy;
			unsigned int c0, c1;
			
			t = sy >> 16;
			t *= rowpixels;
			t += (sx >> 16);
			t = (unsigned int)(src + t);
			
			fx = sx & 0xE000;
			fy = sy & 0xE000;
			fxy = fx * fy;
			fxy >>= 29;
			fx = (fx >> 13) - fxy;

			c0 = ((unsigned int*)t)[0];
			c1 = ((unsigned int*)t)[1];
		//	c0 = (c0 >> 3) & 0x1f1f1f1f;
		//	c1 = (c1 >> 3) & 0x1f1f1f1f;
			
			c1 *= fx;
			fx = 8 - fx;
			fx -= fy >> 13;

			fx *= c0;
			fx += c1;

			t += rowpixels << 2;
			c0 = ((unsigned int*)t)[0];
			c1 = ((unsigned int*)t)[1];
		//	c0 = (c0 >> 3) & 0x1f1f1f1f;
		//	c1 = (c1 >> 3) & 0x1f1f1f1f;

			c1 *= fxy;
			fx += c1;
			fy = (fy >> 13) - fxy;
			c0 *= fy;
			fx += c0;

			fy = fx >> 27;
			if (fy != 0) {
				unsigned int s;
				unsigned int t;
				unsigned int d;
				
				t = fx & 0xF8; 
				s = (fx >> 8) & 0xF800;
				s |= t >> 3; /* red & blue */
				t = fx & 0xFC00;

				if (fy == 31) {
					s |= t >> 5;
				} else  {
					d = *dst;
					s |= t << 11;
					d |= d << 16;
					t = s_mask_07e0f81f;
					d &= t;
					s -= d;
					s *= fy;
					s = d + (s >> 5);
					s &= t;
					s |= s >> 16;
				}
				*dst = s;
			}
			
			++dst;

			sx += sdx;
			sy += sdy;

		} while (--count);
	}	
}

static void 
zmaee_bd_copy_32_to_24_linear(zmaee_context_copy_ex *context)
{
	unsigned int*	dst		= (unsigned int*)context->dst;
	unsigned int*	src		= (unsigned int*)context->src;
	unsigned int	count	= context->count;
	int				sx		= context->sx;
	int				sy		= context->sy;
	int				sdx		= context->sdx;
	int				sdy		= context->sdy;
	unsigned int	rowpixels = context->rowpixels;
	
	if (count > 0) {
		do {
			unsigned int t;
			unsigned int fx, fy, fxy;
			unsigned int c0, c1;
			
			t = sy >> 16;
			t *= rowpixels;
			t += (sx >> 16);
			t = (unsigned int)(src + t);
			
			fx = sx & 0xE000;
			fy = sy & 0xE000;
			fxy = fx * fy;
			fxy >>= 29;
			fx = (fx >> 13) - fxy;

			c0 = ((unsigned int*)t)[0];
			c1 = ((unsigned int*)t)[1];
			
			c1 *= fx;
			fx = 8 - fx;
			fx -= fy >> 13;

			fx *= c0;
			fx += c1;

			t += rowpixels << 2;
			c0 = ((unsigned int*)t)[0];
			c1 = ((unsigned int*)t)[1];

			c1 *= fxy;
			fx += c1;
			fy = (fy >> 13) - fxy;
			c0 *= fy;
			fx += c0;

			ALPHA32T024(*dst, *dst, fx);
			
			++dst;

			sx += sdx;
			sy += sdy;

		} while (--count);
	}	
}

static void 
zmaee_bd_copy_32_to_32_linear(zmaee_context_copy_ex *context)
{
	unsigned int*	dst		= (unsigned int*)context->dst;
	unsigned int*	src		= (unsigned int*)context->src;
	unsigned int	count	= context->count;
	int				sx		= context->sx;
	int				sy		= context->sy;
	int				sdx		= context->sdx;
	int				sdy		= context->sdy;
	unsigned int	rowpixels = context->rowpixels;
	
	if (count > 0) {
		do {
			unsigned int t;
			unsigned int fx, fy, fxy;
			unsigned int c0, c1;
			
			t = sy >> 16;
			t *= rowpixels;
			t += (sx >> 16);
			t = (unsigned int)(src + t);
			
			fx = sx & 0xE000;
			fy = sy & 0xE000;
			
			fxy = fx * fy;
			fxy >>= 29;
			fx = (fx >> 13) - fxy;

			c0 = ((unsigned int*)t)[0];
			c1 = ((unsigned int*)t)[1];
		//	c0 = (c0 >> 3) & 0x1f1f1f1f;
		//	c1 = (c1 >> 3) & 0x1f1f1f1f;
			
			c1 *= fx;
			fx = 8 - fx;
			fx -= fy >> 13;

			fx *= c0;
			fx += c1;

			t += rowpixels << 2;
			c0 = ((unsigned int*)t)[0];
			c1 = ((unsigned int*)t)[1];
		//	c0 = (c0 >> 3) & 0x1f1f1f1f;
		//	c1 = (c1 >> 3) & 0x1f1f1f1f;
			
			c1 *= fxy;
			fx += c1;
			fy = (fy >> 13) - fxy;
			c0 *= fy;
			fx += c0;
			
			*dst++ = fx;

			sx += sdx;
			sy += sdy;

		} while (--count);
	}
}

static void 
zmaee_bd_copy_32_to_P32_linear(zmaee_context_copy_ex *context)
{
	unsigned int*	dst		= (unsigned int*)context->dst;
	unsigned int*	src		= (unsigned int*)context->src;
	unsigned int	count	= context->count;
	int				sx		= context->sx;
	int				sy		= context->sy;
	int				sdx		= context->sdx;
	int				sdy		= context->sdy;
	unsigned int	rowpixels = context->rowpixels;
	
	if (count > 0) {
		do {
			unsigned int t;
			unsigned int fx, fy, fxy;
			unsigned int c0, c1;
			
			t = sy >> 16;
			t *= rowpixels;
			t += (sx >> 16);
			t = (unsigned int)(src + t);
			
			fx = sx & 0xE000;
			fy = sy & 0xE000;
			fxy = fx * fy;
			fxy >>= 29;
			fx = (fx >> 13) - fxy;
			
			c0 = ((unsigned int*)t)[0];
			c1 = ((unsigned int*)t)[1];
			
			c1 *= fx;
			fx = 8 - fx;
			fx -= fy >> 13;
			
			fx *= c0;
			fx += c1;
			
			t += rowpixels << 2;
			c0 = ((unsigned int*)t)[0];
			c1 = ((unsigned int*)t)[1];
			
			c1 *= fxy;
			fx += c1;
			fy = (fy >> 13) - fxy;
			c0 *= fy;
			fx += c0;
			
			ALPHA32T0P32(*dst, *dst, fx);
			
			++dst;
			
			sx += sdx;
			sy += sdy;
			
		} while (--count);
	}	
}


inline static bool is_border_src(TRotaryClipData* cd,
						  long src_x_16,
						  long src_y_16)
{
#if 1
	long src_width = cd->src_width;
	long src_height = cd->src_height;
	src_x_16 += BORDER_WIDTH << 16;
	if (((unsigned)src_x_16) >= (unsigned)((src_width + BORDER_WIDTH * 2) << 16))
		return false;
	src_y_16 += BORDER_WIDTH << 16;
	if (((unsigned)src_y_16) >= (unsigned)((src_height + BORDER_WIDTH * 2) << 16))
		return false;
	return true;
#else
     return ( ( (src_x_16>=(-(BORDER_WIDTH<<16)))&&((src_x_16>>16)<(cd->src_width  + BORDER_WIDTH)) )
           && ( (src_y_16>=(-(BORDER_WIDTH<<16)))&&((src_y_16>>16)<(cd->src_height + BORDER_WIDTH)) ) );
#endif
}

inline static bool is_in_src(TRotaryClipData* cd, long src_x_16,long src_y_16)
{
#if 1
	long src_width = cd->src_width;
	long src_height = cd->src_height;
	src_x_16 -= BORDER_WIDTH << 16;
	if (((unsigned)src_x_16) >= (unsigned)((src_width - BORDER_WIDTH * 2) << 16))
		return false;
	src_y_16 -= BORDER_WIDTH << 16;
	if (((unsigned)src_y_16) >= (unsigned)((src_height - BORDER_WIDTH * 2) << 16))
		return false;
	return true;
#else
     return ( ( (src_x_16>=(BORDER_WIDTH<<16))&&((src_x_16>>16)<(cd->src_width - BORDER_WIDTH) ) )
           && ( (src_y_16>=(BORDER_WIDTH<<16))&&((src_y_16>>16)<(cd->src_height - BORDER_WIDTH)) ) );
#endif
}

static void find_begin_in(TRotaryClipData* cd, long dst_y,long* out_dst_x,long* src_x_16,long* src_y_16)
{
	long t_out_dst_x = *out_dst_x;
	long t_src_x_16 = *src_x_16;
	long t_src_y_16 = *src_y_16;
	long Ax_16 = cd->Ax_16;
	long Ay_16 = cd->Ay_16;
	
    t_src_x_16 -= Ax_16;
    t_src_y_16 -= Ay_16;
    while (is_border_src(cd, t_src_x_16,t_src_y_16)) {
        --t_out_dst_x;
        t_src_x_16 -= Ax_16;
        t_src_y_16 -= Ay_16;
    }
    t_src_x_16 += Ax_16;
    t_src_y_16 += Ay_16;
	
	*out_dst_x = t_out_dst_x;
	*src_x_16 = t_src_x_16;
	*src_y_16 = t_src_y_16;

}

static bool find_begin(TRotaryClipData* cd, long dst_y,long* out_dst_x0,long dst_x1)
{
	long i;
    long test_dst_x0 = *out_dst_x0 - 1;
    long src_x_16 = cd->Ax_16*test_dst_x0 + cd->Bx_16*dst_y + cd->Cx_16;
    long src_y_16 = cd->Ay_16*test_dst_x0 + cd->By_16*dst_y + cd->Cy_16;


    for (i=test_dst_x0;i<=dst_x1;++i)
    {
        if (is_border_src(cd, src_x_16,src_y_16))
        {
            *out_dst_x0=i;
            if (i==test_dst_x0)
                find_begin_in(cd, dst_y,out_dst_x0,&src_x_16,&src_y_16);
			
			// 裁剪
			int delta = cd->clip_left - (*out_dst_x0);
            if (delta > 0) {
                src_x_16 += (cd->Ax_16 * delta);
                src_y_16 += (cd->Ay_16 * delta);
            }
			
            cd->context.sx=src_x_16;
            cd->context.sy=src_y_16;
            return true;
        }
        else
        {
            src_x_16+=cd->Ax_16;
            src_y_16+=cd->Ay_16;
        }
    }
    return false;
}

static void find_end(TRotaryClipData* cd, long dst_y,long dst_x0,long* out_dst_x1)
{
	long src_x_16;
	long src_y_16;
    long test_dst_x1=*out_dst_x1;
    if (test_dst_x1<dst_x0) test_dst_x1=dst_x0;

    src_x_16=cd->Ax_16*test_dst_x1 + cd->Bx_16*dst_y + cd->Cx_16;
    src_y_16=cd->Ay_16*test_dst_x1 + cd->By_16*dst_y + cd->Cy_16;

	if (is_border_src(cd, src_x_16,src_y_16))
    {
        ++test_dst_x1;
        src_x_16+=cd->Ax_16;
        src_y_16+=cd->Ay_16;
        while (is_border_src(cd, src_x_16,src_y_16))
        {
            ++test_dst_x1;
            src_x_16+=cd->Ax_16;
            src_y_16+=cd->Ay_16;
        }
        *out_dst_x1=test_dst_x1;
    }
    else
    {
        src_x_16-=cd->Ax_16;
        src_y_16-=cd->Ay_16;
        while (!is_border_src(cd, src_x_16,src_y_16))
        {
            --test_dst_x1;
            src_x_16-=cd->Ax_16;
            src_y_16-=cd->Ay_16;
        }
        *out_dst_x1=test_dst_x1;
    }
}

static void update_out_dst_x_in(TRotaryClipData* cd)
{
    if ((0==BORDER_WIDTH)||(cd->out_dst_x0_boder>=cd->out_dst_x1_boder) )
    {
        cd->out_dst_x0_in=cd->out_dst_x0_boder;
        cd->out_dst_x1_in=cd->out_dst_x1_boder;
    }
    else
    {
        long src_x_16=cd->context.sx;
        long src_y_16=cd->context.sy;
        long i=cd->out_dst_x0_boder;
        while (i<cd->out_dst_x1_boder)
        {
            if (is_in_src(cd, src_x_16,src_y_16)) break;
            src_x_16+=cd->Ax_16;
            src_y_16+=cd->Ay_16;
            ++i;
        }
        cd->out_dst_x0_in=i;

        src_x_16=cd->context.sx+(cd->out_dst_x1_boder-cd->out_dst_x0_boder)*cd->Ax_16;
        src_y_16=cd->context.sy+(cd->out_dst_x1_boder-cd->out_dst_x0_boder)*cd->Ay_16;
        i=cd->out_dst_x1_boder;
        while (i>cd->out_dst_x0_in)
        {
            src_x_16-=cd->Ax_16;
            src_y_16-=cd->Ay_16;
            if (is_in_src(cd, src_x_16,src_y_16)) break;
            --i;
        }
        cd->out_dst_x1_in=i;
    }
}

static void update_out_dst_up_x(TRotaryClipData* cd)
{
    if (cd->cur_dst_up_x0 < cd->clip_left) {
        cd->out_dst_x0_boder = cd->clip_left;
    } else {
        cd->out_dst_x0_boder=cd->cur_dst_up_x0;
	}



    if (cd->cur_dst_up_x1 >= cd->clip_right) {
        cd->out_dst_x1_boder = cd->clip_right;
    } else {
        cd->out_dst_x1_boder = cd->cur_dst_up_x1;
	}


    update_out_dst_x_in(cd);
}

static void update_out_dst_down_x(TRotaryClipData* cd)
{
	
    if (cd->cur_dst_down_x0 < cd->clip_left) {
        cd->out_dst_x0_boder = cd->clip_left;
    } else {
        cd->out_dst_x0_boder=cd->cur_dst_down_x0;
	}


    if (cd->cur_dst_down_x1 >= cd->clip_right) {
		cd->cur_dst_down_x1 = cd->clip_right;
    } else {
        cd->out_dst_x1_boder=cd->cur_dst_down_x1;
	}

	
    update_out_dst_x_in(cd);
}


static bool init_clip(TRotaryClipData* cd,
			   ZMAEE_Fixed dx,
			   ZMAEE_Fixed dy
		//	   unsigned long aborder_width
			   )
{
//    BORDER_WIDTH=aborder_width;

    //计算src中心点映射到dst后的坐标
    cd->out_dst_down_y=(long)(cd->src_height* (1 << 15) + dy) >> 16;
    cd->cur_dst_down_x0=(long)(cd->src_width* (1 << 15) + dx) >> 16;
    cd->cur_dst_down_x1=cd->cur_dst_down_x0;
    //得到初始扫描线
    if (find_begin(cd, cd->out_dst_down_y,&cd->cur_dst_down_x0,cd->cur_dst_down_x1))
        find_end(cd, cd->out_dst_down_y,cd->cur_dst_down_x0,&cd->cur_dst_down_x1);
    cd->out_dst_up_y=cd->out_dst_down_y;
    cd->cur_dst_up_x0=cd->cur_dst_down_x0;
    cd->cur_dst_up_x1=cd->cur_dst_down_x1;
    update_out_dst_up_x(cd);
    return (cd->cur_dst_down_x0<cd->cur_dst_down_x1);
}

static bool next_clip_line_down(TRotaryClipData* cd)
{
    ++cd->out_dst_down_y;
    if (!find_begin(cd, cd->out_dst_down_y,&cd->cur_dst_down_x0,cd->cur_dst_down_x1))
		return false;
    find_end(cd, cd->out_dst_down_y,cd->cur_dst_down_x0,&cd->cur_dst_down_x1);
    update_out_dst_down_x(cd);
    return (cd->cur_dst_down_x0<cd->cur_dst_down_x1);
}
static bool next_clip_line_up(TRotaryClipData* cd)
{
    --cd->out_dst_up_y;
    if (!find_begin(cd, cd->out_dst_up_y,&cd->cur_dst_up_x0,cd->cur_dst_up_x1))
		return false;
    find_end(cd, cd->out_dst_up_y,cd->cur_dst_up_x0,&cd->cur_dst_up_x1);
    update_out_dst_up_x(cd);
    return (cd->cur_dst_up_x0<cd->cur_dst_up_x1);
}

static int 
SetBorderData32(
	int sx,
	int sy,
	TRotaryClipData* data)
{
	unsigned char* src;
	int src_width;
	int src_height;
	int c0;
	int c1;
	int c2;
	int c3;

	int flag_x0;
	int flag_x1;
	int flag_y0;
	int flag_y1;

	src_width = data->src_width;
	src_height = data->src_height;

	flag_x0 = (sx >= 0 && sx < src_width);
	flag_x1 = (sx + 1 >= 0 && sx + 1 < src_width);
	if (flag_x0 + flag_x1 == 0)
		return 0;

	flag_y0 = (sy >= 0 && sy < src_height);
	flag_y1 = (sy + 1 >= 0 && sy + 1 < src_height);
	if (flag_y0 + flag_y1 == 0)
		return 0;

	src = (unsigned char*)((zmaee_context_copy_ex*)data)->src + ((sy * src_width + sx) << 2);


	if (flag_y0) {
		if (flag_x0) {
			c0 = ((int*)src)[0];
		}
		if (flag_x1) {
			c1 = ((int*)src)[1];
		}
		if (!flag_x0) {
			c0 = c1 & 0xffffff;
		}
		if (!flag_x1) {
			c1 = c0 & 0xffffff;
		}
	}

	if (flag_y1) {
		src += src_width << 2;
		if (flag_x0) {
			c2 = ((int*)src)[0];
		}
		if (flag_x1) {
			c3 = ((int*)src)[1];
		}
		if (!flag_x0) {
			c2 = c3 & 0xffffff;
		}
		if (!flag_x1) {
			c3 = c2 & 0xffffff;
		}
	}
	
	if (!flag_y1) {
		c2 = c0 & 0xffffff;
		c3 = c1 & 0xffffff;
	}
	
	if (!flag_y0) {
		c0 = c2 & 0xffffff;
		c1 = c3 & 0xffffff;
	}

	data->src_border_data[0] = c0;
	data->src_border_data[1] = c1;
	data->src_border_data[2] = c2;
	data->src_border_data[3] = c3;

	return 1;
}

static int 
SetBorderData16(
	int sx,
	int sy,
	TRotaryClipData* data
	)
{
	unsigned char* src;
	int src_width;
	int src_height;
	int c0;
	int c1;
	int c2;
	int c3;

	int flag_x0;
	int flag_x1;
	int flag_y0;
	int flag_y1;

	src_width = data->src_width;
	src_height = data->src_height;

	flag_x0 = (sx >= 0 && sx < src_width);
	flag_x1 = (sx + 1 >= 0 && sx + 1 < src_width);
	if (flag_x0 + flag_x1 == 0)
		return 0;

	flag_y0 = (sy >= 0 && sy < src_height);
	flag_y1 = (sy + 1 >= 0 && sy + 1 < src_height);
	if (flag_y0 + flag_y1 == 0)
		return 0;

	src = (unsigned char*)((zmaee_context_copy_ex*)data)->src + ((sy * src_width + sx) << 1);

	if (flag_y0) {
		if (flag_x0) {
			c0 = ((unsigned short*)src)[0];
		}
		if (flag_x1) {
			c1 = ((unsigned short*)src)[1];
		}
		if (!flag_x0) {
			c0 = c1;
		}
		if (!flag_x1) {
			c1 = c0;
		}
	}

	if (flag_y1) {
		src += src_width << 1;
		if (flag_x0) {
			c2 = ((unsigned short*)src)[0];
		}
		if (flag_x1) {
			c3 = ((unsigned short*)src)[1];
		}
		if (!flag_x0) {
			c2 = c3;
		}
		if (!flag_x1) {
			c3 = c2;
		}
	}
	
	if (!flag_y1) {
		c2 = c0;
		c3 = c1;
	}
	
	if (!flag_y0) {
		c0 = c2;
		c1 = c3;
	}

	((unsigned short*)data->src_border_data)[0] = c0;
	((unsigned short*)data->src_border_data)[1] = c1;
	((unsigned short*)data->src_border_data)[2] = c2;
	((unsigned short*)data->src_border_data)[3] = c3;

	return 1;
}

static int 
SetBorderData8(
	int sx,
	int sy,
	TRotaryClipData* data
	)
{
	unsigned char* src;
	int src_width;
	int src_height;
	int c0;
	int c1;
	int c2;
	int c3;

	int flag_x0;
	int flag_x1;
	int flag_y0;
	int flag_y1;

	src_width = data->src_width;
	src_height = data->src_height;

	flag_x0 = (sx >= 0 && sx < src_width);
	flag_x1 = (sx + 1 >= 0 && sx + 1 < src_width);
	if (flag_x0 + flag_x1 == 0)
		return 0;

	flag_y0 = (sy >= 0 && sy < src_height);
	flag_y1 = (sy + 1 >= 0 && sy + 1 < src_height);
	if (flag_y0 + flag_y1 == 0)
		return 0;

	src = (unsigned char*)((zmaee_context_copy_ex*)data)->src + ((sy * src_width + sx) << 0);

	if (flag_y0) {
		if (flag_x0) {
			c0 = ((unsigned char*)src)[0];
		}
		if (flag_x1) {
			c1 = ((unsigned char*)src)[1];
		}
		if (!flag_x0) {
			c0 = c1;
		}
		if (!flag_x1) {
			c1 = c0;
		}
	}

	if (flag_y1) {
		src += src_width << 0;
		if (flag_x0) {
			c2 = ((unsigned char*)src)[0];
		}
		if (flag_x1) {
			c3 = ((unsigned char*)src)[1];
		}
		if (!flag_x0) {
			c2 = c3;
		}
		if (!flag_x1) {
			c3 = c2;
		}
	}
	
	if (!flag_y1) {
		c2 = c0;
		c3 = c1;
	}
	
	if (!flag_y0) {
		c0 = c2;
		c1 = c3;
	}

	((unsigned char*)data->src_border_data)[0] = c0;
	((unsigned char*)data->src_border_data)[1] = c1;
	((unsigned char*)data->src_border_data)[2] = c2;
	((unsigned char*)data->src_border_data)[3] = c3;

	return 1;
}


typedef void (*BilInear_CopyLine)(
	unsigned char* pDstLine,
	TRotaryClipData* pContext
	);
typedef int (*SetBorderData)(
	int sx, 
	int sy, 
	TRotaryClipData* data
	);

static void 
PicRotary_BilInear_CopyLine(
	unsigned char* pDstLine,
	TRotaryClipData* data,
	SetBorderData setBorderData
	)
{
	long count;
	long dst_border_x0;
	long dst_in_x0;
	long dst_in_x1;
	long dst_border_x1;
	unsigned int dshift;

	zmaee_context_copy_ex* pContext;
	

	pContext = (zmaee_context_copy_ex*)data;
	dshift = data->dshift;
	dst_border_x0 = data->out_dst_x0_boder,
	dst_in_x0 = data->out_dst_x0_in;
	dst_in_x1 = data->out_dst_x1_in;
	dst_border_x1 = data->out_dst_x1_boder;
	

	
	count = dst_in_x0 - dst_border_x0;
	if (count > 0) {
		int sx_fixed;
		int sy_fixed;

		data->border_context.dst = &pDstLine[dst_border_x0 << dshift];
		sx_fixed = pContext->sx;
		sy_fixed = pContext->sy;

		do {
			int sx;
			int sy;

			sx = sx_fixed >> 16;
			sy = sy_fixed >> 16;

			data->border_context.sx = sx_fixed & 0xffff;
			data->border_context.sy = sy_fixed & 0xffff;

			sx_fixed += pContext->sdx;
			sy_fixed += pContext->sdy;

			if (setBorderData(sx, sy, data))
				data->copy_fuc(&data->border_context);

			data->border_context.dst = ((char*)data->border_context.dst) + (1 << dshift);

		} while (--count);

		pContext->sx = sx_fixed;
		pContext->sy = sy_fixed;
	}

	count = dst_in_x1 - dst_in_x0;
	if (count > 0) {
		pContext->dst = &pDstLine[dst_in_x0 << dshift];
		pContext->count = count;
		data->copy_fuc(pContext);
		pContext->sx += count * pContext->sdx;
		pContext->sy += count * pContext->sdy;
	}

	count = dst_border_x1 - dst_in_x1;
	if (count > 0) {
		int sx_fixed;
		int sy_fixed;
		data->border_context.dst = &pDstLine[dst_in_x1 << dshift];
		sx_fixed = pContext->sx;
		sy_fixed = pContext->sy;
		do {
			int sx;
			int sy;
			sx = sx_fixed >> 16;
			sy = sy_fixed >> 16;
			data->border_context.sx = sx_fixed & 0xffff;
			data->border_context.sy = sy_fixed & 0xffff;
			sx_fixed += pContext->sdx;
			sy_fixed += pContext->sdy;

			if (setBorderData(sx, sy, data))
				data->copy_fuc(&data->border_context);

			data->border_context.dst = ((char*)data->border_context.dst) + (1 << dshift);

		} while (--count);
	}

}


/*
 *           | cos(a)  sin(a)|
 *  (x, y)   |               |
 *           |-sin(a)  cos(a)|
 */
static void
zmaee_bitblt_ex1(
	ZMAEE_LayerInfo* li,
	ZMAEE_Fixed transform[4],
	ZMAEE_BitmapInfo* bi,
	ZMAEE_Fixed dx,
	ZMAEE_Fixed dy,
	ZMAEE_COPY_EX_FUNC copy_fuc
	)
{

	int src_width = bi->width;
	int src_height = bi->height;
	int dst_width = li->nWidth;
	int dst_height = li->nHeight;

    long Ax_16 = (long)transform[0];
    long Ay_16 = (long)transform[1];
    long Bx_16 = (long)transform[2];
    long By_16 = (long)transform[3];
    long Cx_16;
    long Cy_16;
	long clip_x;
	long clip_y;
	long clip_w;
	long clip_h;
	int dshift;
    ZMAEE_Fixed rx0 = src_width * (1 << 15);  //(rx0,ry0)为旋转中心
    ZMAEE_Fixed ry0 = src_height * (1 << 15);

	long dst_byte_width;
	unsigned char* pDstLine;
	SetBorderData setBorderData;
    TRotaryClipData rcData;
	
	dx -= rx0;
	dy -= ry0;

	dst_byte_width = dst_width;

	if (bi->nDepth == ZMAEE_COLORFORMAT_32) {
		setBorderData = SetBorderData32;
		rcData.sshift = 2;
	} else if (bi->nDepth == ZMAEE_COLORFORMAT_16) {
		setBorderData = SetBorderData16;
		rcData.sshift = 1;
	} else {
		setBorderData = SetBorderData8;
		rcData.sshift = 0;
	}

	if (li->cf == ZMAEE_COLORFORMAT_PM32) {
		dshift = 2;
	} else if (li->cf == ZMAEE_COLORFORMAT_32) {
		dshift = 2;
	} else if (li->cf == ZMAEE_COLORFORMAT_24) {
		dshift = 2;
	} else if (li->cf == ZMAEE_COLORFORMAT_16) {
		dshift = 1;
	} else {
		return;
	}

	rcData.dshift = dshift;

	dst_byte_width <<= dshift;

	Cx_16 = __fixed_mul(-(rx0 + dx), Ax_16) +
			__fixed_mul(-(ry0 + dy), Bx_16) + rx0;

	Cy_16 = __fixed_mul(-(rx0 + dx), Ay_16) +
			__fixed_mul(-(ry0 + dy), By_16) + ry0;

	rcData.context.src = bi->pBits;
	rcData.context.sdx = Ax_16;
	rcData.context.sdy = Ay_16;
	rcData.context.rowpixels = src_width;
	rcData.context.transcolor = -1;
	rcData.context.palette   = bi->pPalette;

	rcData.border_context.src = rcData.src_border_data;
	rcData.border_context.count = 1;
	rcData.border_context.rowpixels = 2;
	rcData.border_context.transcolor = -1;
	rcData.border_context.palette   = bi->pPalette;

    rcData.Ax_16 = Ax_16;
    rcData.Bx_16 = Bx_16;
    rcData.Cx_16 = Cx_16;
    rcData.Ay_16 = Ay_16;
    rcData.By_16 = By_16;
    rcData.Cy_16 = Cy_16;

    rcData.dst_width= dst_width;
    rcData.dst_height= dst_height;
    rcData.src_width = src_width;
    rcData.src_height = src_height;

	rcData.copy_fuc = copy_fuc;


	clip_x = li->rcClip.x;
	clip_y = li->rcClip.y;
	clip_w = li->rcClip.width;
	clip_h = li->rcClip.height;
	clip_w += clip_x;
	clip_h += clip_y;
	if (clip_x < 0)
		clip_x = 0;
	if (clip_y < 0)
		clip_y = 0;
	if (clip_w > dst_width)
		clip_w = dst_width;
	if (clip_h > dst_height)
		clip_h = dst_height;

	rcData.clip_left = clip_x;
	rcData.clip_right = clip_w;


    if (!init_clip(&rcData, dx, dy/*, BORDER_WIDTH*/))
		return;

    pDstLine= (unsigned char*)li->pFrameBuf;

    pDstLine += (dst_byte_width * rcData.out_dst_down_y);

	while (1) //to down
    {
        long y = rcData.out_dst_down_y;
        if (y >= clip_h) break;
		
        if (y >= clip_y) {
            PicRotary_BilInear_CopyLine(pDstLine, &rcData, setBorderData);
        }

        if (!next_clip_line_down(&rcData))
			break;

        pDstLine += dst_byte_width;
    }


    pDstLine = (unsigned char*)li->pFrameBuf;
	pDstLine += (dst_byte_width * rcData.out_dst_up_y);

    while (next_clip_line_up(&rcData)) //to up
    {
        long y = rcData.out_dst_up_y;
        if (y < clip_y)
			break;
		pDstLine -= dst_byte_width;
        if (y < clip_h) {
            PicRotary_BilInear_CopyLine(pDstLine, &rcData, setBorderData);
        }

    }
}


#undef bool

void AEERotateBitmap::Render(
	ZMAEE_LayerInfo* li,
	int transform[4], 	
	ZMAEE_BitmapInfo* bi, 
	int originX, 
	int originY,
	bool aa
	)
{
	Render(li, transform, bi, (float)originX, (float)originY, aa);
}

void AEERotateBitmap::Render(
	ZMAEE_LayerInfo* li,
	int transform[4], 	
	ZMAEE_BitmapInfo* bi, 
	float originX, 
	float originY,
	bool aa
	)
{
	ZMAEE_COPY_EX_FUNC copy_func;
	ZMAEE_CopyExFuncList copy_ex_func_list;
	AEE_IShell* shell;
	AEE_IUtil* iUtil;

	shell = ZMAEE_GetShell();
	if (AEE_IShell_CreateInstance(shell, ZM_AEE_CLSID_UTIL, (void**)&iUtil) == E_ZM_AEE_SUCCESS) {
		AEE_IUtil_ExportCopyExFunc(iUtil, &copy_ex_func_list);
		AEE_IUtil_Release(iUtil);
		copy_ex_func_list.copy_8_to_16 = zmaee_bd_copy_8_to_16;
	} else {
		copy_ex_func_list.copy_8_to_16 = zmaee_bd_copy_8_to_16;
		copy_ex_func_list.copy_8_to_32 = zmaee_bd_copy_8_to_32;
		copy_ex_func_list.copy_16_to_16 = zmaee_bd_copy_16_to_16;
		copy_ex_func_list.copy_16_to_32 = zmaee_bd_copy_16_to_32;
		copy_ex_func_list.copy_32_to_16 = zmaee_bd_copy_32_to_16;
		copy_ex_func_list.copy_32_to_32 = zmaee_bd_copy_32_to_32;
		copy_ex_func_list.copy_32_to_16_linear = zmaee_bd_copy_32_to_16_linear;
		copy_ex_func_list.copy_32_to_32_linear = zmaee_bd_copy_32_to_32_linear;
	}

	switch (bi->nDepth) {
	case ZMAEE_COLORFORMAT_8:
		if (li->cf == ZMAEE_COLORFORMAT_16) {
			copy_func = copy_ex_func_list.copy_8_to_16;
		} else {
			copy_func = copy_ex_func_list.copy_8_to_32;
		}
		break;
	case ZMAEE_COLORFORMAT_16:
		if (li->cf == ZMAEE_COLORFORMAT_16) {
			copy_func = copy_ex_func_list.copy_16_to_16;
		} else {
			copy_func = copy_ex_func_list.copy_16_to_32;
		}
		break;
	case ZMAEE_COLORFORMAT_32:
		if (li->cf == ZMAEE_COLORFORMAT_16) {
			copy_func = copy_ex_func_list.copy_32_to_16;
			if (aa) 
				copy_func = copy_ex_func_list.copy_32_to_16_linear;
			
		} else if (li->cf == ZMAEE_COLORFORMAT_24) {
			copy_func = zmaee_bd_copy_32_to_24;
			if (aa) 
				copy_func = zmaee_bd_copy_32_to_24_linear;
		} else if (li->cf == ZMAEE_COLORFORMAT_PM32) {
			copy_func = zmaee_bd_copy_32_to_P32;
			if (aa) 
				copy_func = zmaee_bd_copy_32_to_P32_linear;
		} else {
			copy_func = copy_ex_func_list.copy_32_to_32;
			if (aa) 
				copy_func = copy_ex_func_list.copy_32_to_32_linear;

		}
		break;
	default:
		return;
	}


	zmaee_bitblt_ex1(li, transform, bi, (int)(originX * 65536.0f), (int)(originY * 65536.0f), copy_func);


}

void AEERotateBitmap::RotateRender(
	ZMAEE_LayerInfo* li, 
	float angle, 
	ZMAEE_BitmapInfo* bi, 
	int originX, 
	int originY,
	bool aa
	)
{

	ZMAEE_Fixed transform[4];

	transform[0] = (int)(zmaee_cos(angle)*65536);
	transform[1] = (int)(zmaee_sin(angle)*65536);
	transform[2] = -transform[1];
	transform[3] = transform[0];

	Render(li, transform, bi, originX, originY, aa);
}
