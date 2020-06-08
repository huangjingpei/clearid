#include "mui_util.h"

bool MUIUtil::PtInRect(MUIRect& rc, int x, int y)
{
	return (x > rc.x && x < rc.x+rc.width && y > rc.y && y < rc.y + rc.height);
}

bool MUIUtil::IntersectRect(const MUIRect& rc1, const MUIRect& rc2, MUIRect* rcIntersect)
{
	int left	= MUI_MAX(rc1.x, rc2.x);
	int top		= MUI_MAX(rc1.y, rc2.y);
	int right	= MUI_MIN(rc1.x + rc1.width, rc2.x + rc2.width);
	int bottom	= MUI_MIN(rc1.y + rc1.height, rc2.y + rc2.height);

	if(left < right && top < bottom)
	{
		if(rcIntersect){
			rcIntersect->x = left;
			rcIntersect->y = top;
			rcIntersect->width = right - left;
			rcIntersect->height = bottom - top;
		}
		return true;
	}

	return false;
}

void MUIUtil::int_to_wstr(unsigned short* lpwcsText, int val)
{
	int i=0;
	char tmp[16];
	
	zmaee_sprintf(tmp, "%d", val);
	while(tmp[i]) {lpwcsText[i] = tmp[i];++i;}
	lpwcsText[i] = 0;
}

MUIRect MUIUtil::parseRect(const AEEStringRef& str)
{
	MUIRect rc;
	char* p = NULL;
	
	rc.x = zmaee_strtol((char*)str.ptr(), &p, 10);
	rc.y = zmaee_strtol(p + 1, &p, 10);
	rc.width = zmaee_strtol(p + 1, &p, 10);
	rc.height = zmaee_strtol(p + 1, &p, 10);
	
	return rc;
}

int     MUIUtil::parseStyle(const AEEStringRef& str)
{
	const char* p = str.ptr();
	int i = 0, n_start = 0, n_len = str.length();
	int nStyle = 0;
	
	while(i < n_len)
	{
		if(p[i] == '|' || i == (n_len-1))
		{
			if(i == (n_len-1)) i = n_len;
			AEEStringRef strStyle(p + n_start, i-n_start);
			if(strStyle == "select")
				nStyle |= MUI_STYLE_SELECT;
			else if(strStyle == "canfocus")
				nStyle |= MUI_STYLE_CANFOCUS;
			else if(strStyle == "canclick")
				nStyle |= MUI_STYLE_CANCLICK;
			else if(strStyle == "visible")
				nStyle |= MUI_STYLE_VISIBLE;
			else if(strStyle == "model")
				nStyle |= MUI_STYLE_MODEL;
			else if(strStyle == "disable")
				nStyle |= MUI_STYLE_DISABLE;
			else if(strStyle == "notpenredraw")
				nStyle |= MUI_STYLE_NOT_PENREDRAW;
			n_start = i + 1;
		}
		++i;
	}

	return nStyle;
}

int MUIUtil::parseInt(const AEEStringRef& str)
{
	return zmaee_strtol((char*)str.ptr(), 0, 10);
}

ZMAEE_Color MUIUtil::ParseColor(const AEEStringRef& str)
{
	ZMAEE_Color c;
	char tmp[4] = {0};
	int r = 0, g = 0, b = 0, a = 0;
	const char* p = str.ptr();
	
	if(*p++ == '#')
	{
		//r
		zmaee_memcpy(tmp, p, 2);
		r = zmaee_strtol(tmp, 0, 16);
		//g
		p+=2;
		zmaee_memset(tmp, 0, sizeof(tmp));
		zmaee_memcpy(tmp, p, 2);
		g = zmaee_strtol(tmp, 0, 16);
		//b
		p+=2;
		zmaee_memset(tmp, 0, sizeof(tmp));
		zmaee_memcpy(tmp, p, 2);
		b = zmaee_strtol(tmp, 0, 16);
		//a
		p+=2;
		if(*p)
			a = zmaee_strtol((char*)p, 0, 16);
		if(a)
			c = ZMAEE_GET_RGBA(r,g,b,a);
		else
			c = ZMAEE_GET_RGB(r,g,b);
	}
	
	return c;
}

unsigned int MUIUtil::StrHash(const char* str, int len)
{
	unsigned int h, c;
	if(len == -1) len = zmaee_strlen(str);

	h = 131;
	for(int i = 0; i < len; ++i)
	{
		c = str[i];
		if(c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		h = h * 241 + c;
	}

	return h;
}

char* MUIUtil::strdup(const char* str)
{
	char* ret = (char*)AEEAlloc::Alloc(zmaee_strlen(str)+1);
	zmaee_strcpy(ret, str);
	return ret;
}

/**
 * 绘制图片数字
 */
int MUIUtil::DrawNumber(MUIDC* pDC, ZMAEE_BitmapInfo& bif, const MUIRect& rcDst, int nNumber)
{
	char tmp[16];
	zmaee_sprintf(tmp, "%d", nNumber);

	int nNumWidth = bif.width/10;
	int x = rcDst.x + (rcDst.width -nNumWidth * zmaee_strlen(tmp))/2;
	int y = rcDst.y + (rcDst.height -bif.height)/2;
	
	int i = 0;
	while(tmp[i]){
		pDC->BitBlt(i*nNumWidth + x, y, &bif, (tmp[i]-'0')*nNumWidth, 0, nNumWidth, bif.height, 0, 1);
		++i;
	}

	return nNumWidth*zmaee_strlen(tmp);
}

/**
 * 绘制断数
 */
int MUIUtil::DrawBrokenNumber(MUIDC* pDC, ZMAEE_BitmapInfo& bifNum, ZMAEE_BitmapInfo& bifLink, const MUIRect& rcDst, int nNumberA, int nNumberB)
{
	char tmpA[16], tmpB[16];
	zmaee_sprintf(tmpA, "%d", nNumberA);
	zmaee_sprintf(tmpB, "%d", nNumberB);

	int nNumWidth = bifNum.width/10;
	int len = bifLink.width + nNumWidth * (zmaee_strlen(tmpA) + zmaee_strlen(tmpB));
	int x = rcDst.x + (rcDst.width - len)/2;
	int y = rcDst.y + (rcDst.height - bifNum.height)/2;
	int i = 0;
	while(tmpA[i]){
		pDC->BitBlt(i*nNumWidth + x, y, &bifNum, (tmpA[i]-'0')*nNumWidth, 0, nNumWidth, bifNum.height, 0, 1);
		++i;
	}
	x += nNumWidth * zmaee_strlen(tmpA);
	pDC->BitBlt(x, rcDst.y + (rcDst.height - bifLink.height)/2, &bifLink, 0, 0, bifLink.width, bifLink.height, 0, 1);
	x += bifLink.width;
	i = 0;
	while(tmpB[i]){
		pDC->BitBlt(i*nNumWidth + x, y, &bifNum, (tmpB[i]-'0')*nNumWidth, 0, nNumWidth, bifNum.height, 0, 1);
		++i;
	}
	return len;
}

/**
 * 绘制带符号图片数字
 */
int MUIUtil::DrawSignedNumber(MUIDC* pDC, ZMAEE_BitmapInfo& bif, const MUIRect& rcDst, int nNumber, ZMAEE_TextAlign nAlign)
{
	char tmp[16] = {0};
	tmp[0] = '0'-1;
	zmaee_sprintf(tmp+1, "%d", nNumber);
	
	int nNumWidth = bif.width/11;
	int x = rcDst.x ;
	if(nAlign == ZMAEE_ALIGN_CENTER)
		x += (rcDst.width - nNumWidth * (zmaee_strlen(tmp)))/2;
	else if(nAlign == ZMAEE_ALIGN_RIGHT)
		x += rcDst.width - nNumWidth * (zmaee_strlen(tmp));
	int y = rcDst.y + (rcDst.height -bif.height)/2;
	
	int i = 0;
	while(tmp[i]){
		pDC->BitBlt(i*nNumWidth + x, y, &bif, (tmp[i]-'0'+1)*nNumWidth, 0, nNumWidth, bif.height, 0, 1);
		++i;
	}
	
	return nNumWidth*zmaee_strlen(tmp);
}

/**
 * {A ： A是需要水平拉伸的像素， { 是左边需要绘制的部分，右边镜像绘制 }
 */
void MUIUtil::DrawHorStretch(MUIDC* pDC, ZMAEE_BitmapInfo& bif, const MUIRect& rcDst)
{
	pDC->BitBlt(rcDst.x,  rcDst.y, 
		&bif, 0, 0, bif.width, bif.height, ZMAEE_TRANSFORMAT_NORMAL, 1);
	pDC->BitBlt(rcDst.x + rcDst.width - bif.width, rcDst.y, 
		&bif, 0, 0, bif.width, bif.height, ZMAEE_TRANSFORMAT_MIRROR, 1);
	pDC->StretchBlt(rcDst.x + bif.width, rcDst.y, rcDst.width-bif.width*2, bif.height,
		&bif, bif.width-1, 0, 1, bif.height, 1);
}

/**
 * {A ： A是需要水平平铺的部分， { 是左边需要绘制的部分，右边镜像绘制 }
 */
void MUIUtil::DrawHorTile(MUIDC* pDC, ZMAEE_BitmapInfo& bif, int nOffsetX, const MUIRect& rcDst)
{
	pDC->BitBlt(rcDst.x, rcDst.y,
		&bif, 0, 0, nOffsetX, bif.height, ZMAEE_TRANSFORMAT_NORMAL, 1);
	pDC->BitBlt(rcDst.x + rcDst.width - nOffsetX, rcDst.y,
		&bif, 0, 0, nOffsetX, bif.height, ZMAEE_TRANSFORMAT_MIRROR, 1);
	int nMidWidth = rcDst.width - nOffsetX*2;
	int x = 0;
	while(x < nMidWidth)
	{
		pDC->BitBlt(rcDst.x + nOffsetX + x, rcDst.y,
			&bif, nOffsetX, 0, bif.width-nOffsetX, bif.height, ZMAEE_TRANSFORMAT_NORMAL, 1);
		x += bif.width-nOffsetX;
	}
}

/**
 * A|B : 左边画A，| 部分拉伸, 右边画B
 * @nStrechOffset 是上述 | 部分的x坐标
 */
void MUIUtil::DrawHorMidStretch(MUIDC* pDC, ZMAEE_BitmapInfo& bif, int nStrechOffset, const MUIRect& rcDst)
{
	if(nStrechOffset > rcDst.width)
	{
		//区域不到左半边的，只画左半边区域大小
		pDC->BitBlt(rcDst.x, rcDst.y, 
			&bif, 0, 0, rcDst.width, bif.height, 0, 1);
		return;
	}
	else
	{
		if (bif.width < rcDst.width)
		{
			pDC->BitBlt(rcDst.x, rcDst.y, 
				&bif, 0, 0, nStrechOffset, bif.height, 0, 1);
		}
		else
		{
			pDC->BitBlt(rcDst.x, rcDst.y, 
				&bif, 0, 0, rcDst.width / 2, bif.height, 0, 1);
		}
	}
	
	if(bif.width > rcDst.width)
	{
		//区域不到源图片的，只画区域大小
		pDC->BitBlt(rcDst.x + rcDst.width / 2/*-(bif.width-nStrechOffset)*/, rcDst.y, 
			&bif, bif.width - rcDst.width / 2, 0, rcDst.width / 2, bif.height, 0, 1);
		return;
	}
	else
	{
		pDC->BitBlt(rcDst.x+rcDst.width-(bif.width-nStrechOffset), rcDst.y, 
			&bif, nStrechOffset, 0, bif.width-nStrechOffset, bif.height, 0, 1);
	}

	if(rcDst.width > bif.width)
	{
		pDC->StretchBlt(rcDst.x+nStrechOffset, rcDst.y, rcDst.width-bif.width, bif.height, 
			&bif, nStrechOffset, 0, 1, bif.height, 1);
	}
}


/**
 * A/B : 上边边画A，/ 部分拉伸, 下边画B
 * @nStrechOffset 是上述 / 部分的y坐标
 */
void MUIUtil::DrawVerMidStretch(MUIDC* pDC, ZMAEE_BitmapInfo& bif, int nStrechOffset, const MUIRect& rcDst)
{
	pDC->BitBlt(rcDst.x, rcDst.y, 
		&bif, 0, 0, bif.width, nStrechOffset, 0, 1);
	pDC->BitBlt(rcDst.x, rcDst.y+rcDst.height-(bif.height-nStrechOffset), 
		&bif, 0, nStrechOffset, bif.width, bif.height-nStrechOffset, 0, 1);
	pDC->StretchBlt(rcDst.x, rcDst.y+nStrechOffset, bif.width, rcDst.height-bif.height, 
		&bif, 0, nStrechOffset, bif.width, 1, 1);
}

/**
 * 绘制四角镜像框
 */
void MUIUtil::Draw4CornerFrame(MUIDC* pDC, ZMAEE_BitmapInfo& bif, const MUIRect& rcDst)
{
	int width = bif.width;
	int height = bif.height;
	int half_width = (width>>1);
	int half_height = (height>>1);

	pDC->BitBlt(rcDst.x, rcDst.y, 
		&bif, 0, 0, half_width, half_height, ZMAEE_TRANSFORMAT_NORMAL,1);
	
	pDC->BitBlt(rcDst.x+rcDst.width-half_width, rcDst.y, 
		&bif, width - half_width, 0, half_width, half_height, ZMAEE_TRANSFORMAT_NORMAL, 1);
	
	pDC->BitBlt(rcDst.x+rcDst.width-half_width, rcDst.y+rcDst.height-half_height,
		&bif, width - half_width, height - half_height, half_width, half_height, ZMAEE_TRANSFORMAT_NORMAL, 1);
	
	pDC->BitBlt(rcDst.x, rcDst.y+rcDst.height-half_height,
		&bif, 0, height - half_height, half_width, half_height, ZMAEE_TRANSFORMAT_NORMAL, 1);

	pDC->StretchBlt(rcDst.x + half_width, rcDst.y, rcDst.width - half_width * 2, half_height,
		&bif, half_width, 0, 1, half_height, 1);

	pDC->StretchBlt(rcDst.x + half_width, rcDst.y+rcDst.height-half_height, rcDst.width - half_width * 2, half_height,
		&bif, half_width, height - half_height, 1, half_height, 1);
	
	pDC->StretchBlt(rcDst.x, rcDst.y+half_height, half_width, rcDst.height - half_height * 2,
		&bif, 0, half_height, half_width, 1, 1);

	pDC->StretchBlt(rcDst.x+rcDst.width-half_width, rcDst.y+half_height, half_width, rcDst.height- half_height * 2,
		&bif, width - half_width, half_height, half_width, 1, 1);
}



#define _DAY_SEC (24L * 60L * 60L) 
#define _HOUR_SEC (60L*60L)
#define _MIN_SEC	(60L)
#define _YEAR_SEC (365L * _DAY_SEC) 
#define _BASE_DOW 4 
#define _MULTI_SEC	1000
#define _FOUR_YEAR_SEC 126230400L //一个闰年，三个非闰年的综合秒数
static const int _days[] = {-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364}; //非润年(每月比实际天数少一天)
static const int _lpdays[] = {-1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365}; //润年(每月比实际天数少一天)
void MUIUtil::Tick2DateTime(ZMAEE_DATETIME* dateTime, unsigned int t)
{
        long caltim = 0;				/* calendar time to convert */
		int islpyr = 0;                 /* is-current-year-a-leap-year flag */
		int tmptim = 0;
		int *mdays = 0;					/* pointer to days or lpdays */
		int tm_yday = 0;

		t += 28800;

		caltim = t; 

        tmptim = (int)(caltim / _FOUR_YEAR_SEC);
        caltim -= ((long)tmptim * _FOUR_YEAR_SEC);

        /*
         * Determine which year of the interval
         */
        tmptim = (tmptim * 4) + 70;         /* 1970, 1974, 1978,...,etc. */

        if ( caltim >= _YEAR_SEC ) {

            tmptim++;                       /* 1971, 1975, 1979,...,etc. */
            caltim -= _YEAR_SEC;

            if ( caltim >= _YEAR_SEC ) {

                tmptim++;                   /* 1972, 1976, 1980,...,etc. */
                caltim -= _YEAR_SEC;

                /*
                 * Note, it takes 366 days-worth of seconds to get past a leap
                 * year.
                 */
                if ( caltim >= (_YEAR_SEC + _DAY_SEC) ) {

                        tmptim++;           /* 1973, 1977, 1981,...,etc. */
                        caltim -= (_YEAR_SEC + _DAY_SEC);
                }
                else {
                        /*
                         * In a leap year after all, set the flag.
                         */
						islpyr++;
                }
            }
        }

        /*
         * tmptim now holds the value for tm_year. caltim now holds the
         * number of elapsed seconds since the beginning of that year.
         */
        dateTime->nYear = tmptim;

        /*
         * Determine days since January 1 (0 - 365). This is the tm_yday value.
         * Leave caltim with number of elapsed seconds in that day.
         */
        tm_yday = (int)(caltim / _DAY_SEC);		
        caltim -= (long)(tm_yday) * _DAY_SEC;

        /*
         * Determine months since January (0 - 11) and day of month (1 - 31)
		*/
		if(islpyr)
			mdays = (int*)_lpdays;
        else
            mdays = (int*)_days;

        for ( tmptim = 1 ; mdays[tmptim] < tm_yday ; tmptim++ );

        dateTime->nMonth = --tmptim;

        dateTime->nDay = tm_yday - mdays[tmptim];

        /*
         * Determine days since Sunday (0 - 6)
         */
        dateTime->nWeekDay = ((int)(t / _DAY_SEC) + _BASE_DOW) % 7;

        /*
         *  Determine hours since midnight (0 - 23), minutes after the hour
         *  (0 - 59), and seconds after the minute (0 - 59).
         */
		if (caltim < 3600)
		{
			dateTime->nHour = 0;
		}
		else
		{
			dateTime->nHour = (int)(caltim / 3600);
		}
        caltim -= (long)dateTime->nHour * 3600L;
        dateTime->nMin = (int)(caltim / 60);
        dateTime->nSec = (int)(caltim - (dateTime->nMin) * 60);


		dateTime->nYear += 1900;
		dateTime->nMonth += 1;
}

void MUIUtil::RectCat(MUIRect* pRcDst, MUIRect rcSrc)
{
	if(pRcDst == NULL)
		return;
	MUIRect& rcDst = *pRcDst;
	if(rcDst.width == 0||rcDst.height == 0)
	{
		zmaee_memcpy(pRcDst, &rcSrc, sizeof(MUIRect));
		return;
	}
	if(rcDst.x>rcSrc.x)
	{
		rcDst.width += rcDst.x - rcSrc.x;
		rcDst.x = rcSrc.x;
	}
	else if(rcDst.x<rcSrc.x)
	{
		rcSrc.width += rcSrc.x - rcDst.x;
		rcSrc.x = rcDst.x;
	}
	if(rcDst.y>rcSrc.y)
	{
		rcDst.height += rcDst.y - rcSrc.y;
		rcDst.y = rcSrc.y;
	}
	else if(rcDst.y<rcSrc.y)
	{
		rcSrc.height += rcSrc.y - rcDst.y;
		rcSrc.y = rcDst.y;
	}
	if(rcDst.width<rcSrc.width)
		rcDst.width = rcSrc.width;
	if(rcDst.height<rcSrc.height)
		rcDst.height = rcSrc.height;
}

void MUIUtil::DrawBitmapGL( 
		AEE_IDisplay* display, float tx, float ty, 
		AEE_IBitmap* bitmap, ZMAEE_Rect* rc, float scale_x, float scale_y, int angle, int alpha, bool aa)
{
	ZMAEE_FPoint dpts[4];
	ZMAEE_Rect rcBmp;
	if (rc == NULL) {
		ZMAEE_BitmapInfo bi;
		AEE_IBitmap_GetInfo(bitmap, &bi);
		rc = &rcBmp;
		rc->x = 0;
		rc->y = 0;
		rc->width = bi.width;
		rc->height = bi.height;
	}
	int width = rc->width;
	int height = rc->height;
	float fw, fh;
	float cos_a, sin_a;
	double fa = (3.1415926535 / 180.0) * angle;
	cos_a = (float)zmaee_cos(fa);
	sin_a = (float)zmaee_sin(fa);

	fw = scale_x * (float)width * -0.5f;
	fh = scale_y * (float)height * -0.5f;
	
	dpts[0].x = tx + fw * cos_a - fh * sin_a; dpts[0].y = ty + fw * sin_a + fh * cos_a;
	fw = -fw;
	dpts[1].x = tx + fw * cos_a - fh * sin_a; dpts[1].y = ty + fw * sin_a + fh * cos_a;
	fh = -fh;
	dpts[2].x = tx + fw * cos_a - fh * sin_a; dpts[2].y = ty + fw * sin_a + fh * cos_a;
	fw = -fw;
	dpts[3].x = tx + fw * cos_a - fh * sin_a; dpts[3].y = ty + fw * sin_a + fh * cos_a;
	
	AEE_IGLDisplay_DrawGLBitmap(display, dpts, bitmap, rc, (1.0f / 255.0f) * alpha, aa ? 1 : 0);
}

void MUIUtil::DrawBitmapGL( 
					   AEE_IDisplay* display, float tx, float ty, int centerx, int centery,
					   AEE_IBitmap* bitmap, ZMAEE_Rect* rc, float scale_x, float scale_y, double radin, int alpha, bool aa)
{
	ZMAEE_FPoint dpts[4];
	ZMAEE_Rect rcBmp;
	if (rc == NULL) {
		ZMAEE_BitmapInfo bi;
		AEE_IBitmap_GetInfo(bitmap, &bi);
		rc = &rcBmp;
		rc->x = 0;
		rc->y = 0;
		rc->width = bi.width;
		rc->height = bi.height;
	}
	int width = rc->width;
	int height = rc->height;
	float fw, fh, fx, fy;
	float cos_a, sin_a;
	cos_a = (float)zmaee_cos(radin);
	sin_a = (float)zmaee_sin(radin);
	
	fw = scale_x * (float)width;
	fh = scale_y * (float)height;
	fx = -scale_x*centerx;
	fy = -scale_y*(height-centery);
	
	dpts[0].x = tx + fx * cos_a - fy * sin_a; dpts[0].y = ty + fx * sin_a + fy * cos_a;
	fx += fw;
	dpts[1].x = tx + fx * cos_a - fy * sin_a; dpts[1].y = ty + fx * sin_a + fy * cos_a;
	fy += fh;
	dpts[2].x = tx + fx * cos_a - fy * sin_a; dpts[2].y = ty + fx * sin_a + fy * cos_a;
	fx -= fw;
	dpts[3].x = tx + fx * cos_a - fy * sin_a; dpts[3].y = ty + fx * sin_a + fy * cos_a;
	
	AEE_IGLDisplay_DrawGLBitmap(display, dpts, bitmap, rc, (1.0f / 255.0f) * alpha, aa ? 1 : 0);
}

