#include "zmaee_stdlib.h"
#include "AEEBase.h"
#include "dui_stringutil.h"
unsigned int DUIStringUtil::StrHash(const AEEStringRef& str)
{
	unsigned int h, c;

	h = 131;
	for(int i = 0; i < str.length(); ++i)
	{
		c = str[i];
		if(c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		h = h * 241 + c;
	}
	
	
	
	return h;
}


TRect DUIStringUtil::ParseRect(const AEEStringRef& str)
{
	TRect r;
	char* p = NULL;

	r.x = zmaee_strtol((char*)str.ptr(), &p, 10);
	r.y = zmaee_strtol(p + 1, &p, 10);
	r.width = zmaee_strtol(p + 1, &p, 10);
	r.height = zmaee_strtol(p + 1, &p, 10);

	return r;
}

int DUIStringUtil::ParseInt(const AEEStringRef& str)
{
	return zmaee_strtol((char*)str.ptr(), 0, 10);
}

TColor DUIStringUtil::ParseColor(const AEEStringRef& str)
{
	TColor c;
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
			c.mColor = ZMAEE_GET_RGBA(r,g,b,a);
		else
			c.mColor = ZMAEE_GET_RGB(r,g,b);
	}

	return c;
}

static ZMAEE_TextAlign GetAlign(const char* p)
{
	if(zmaee_strcmp(p, "left") == 0)
		return ZMAEE_ALIGN_LEFT;
	else if(zmaee_strcmp(p, "right") == 0)
		return ZMAEE_ALIGN_RIGHT;
	else if(zmaee_strcmp(p, "center") == 0)
		return ZMAEE_ALIGN_CENTER;
	else if(zmaee_strcmp(p, "top") == 0)
		return ZMAEE_ALIGN_TOP;
	else if(zmaee_strcmp(p, "vcenter") == 0)
		return ZMAEE_ALIGN_VCENTER;
	else if(zmaee_strcmp(p, "bottom") == 0)
		return ZMAEE_ALIGN_BOTTOM;

	return 0;
}

ZMAEE_TextAlign DUIStringUtil::ParseAlign(const AEEStringRef& str)
{
	ZMAEE_TextAlign a = 0;
	char* pS = NULL;
	const char* p = str.ptr();

	while((pS = zmaee_strpbrk(p, "|")) != NULL) 
	{
		char t[16] = {0};
		zmaee_memcpy(t, p, pS - p);
		a |= GetAlign(t);
			
		p = pS + 1;
	}
	
	if(p < str.ptr() + str.length())
		a |= GetAlign(p);

	return a;
}

bool DUIStringUtil::ParseBool(const AEEStringRef& str)
{	
	if(zmaee_memcmp((void*)str.ptr(), (void*)"1", 1) == 0 || zmaee_memcmp((void*)str.ptr(), (void*)"true", 4) == 0)
		return true;
	return false;
}


/**
 * str format:
	x, y
 */
TPoint DUIStringUtil::ParsePoint(const AEEStringRef& str)
{
	TPoint pt;
	char* p = NULL;
	pt.x = zmaee_strtol((char*)str.ptr(), &p, 10);
	pt.y = zmaee_strtol(p + 1, &p, 10);
	return pt;
}

/**
 * str format
  [TOPLEFT|TOPRIGHT|LEFTBOTTOM|RIGHTBOTTOM|CENTER],offset_x, offset_y
 */
TRelativePos DUIStringUtil::ParseRelativePos(const AEEStringRef& str)
{
	TRelativePos rpos;
	char* p = NULL;
	rpos.type = zmaee_strtol((char*)str.ptr(), &p, 10);
	rpos.x = zmaee_strtol(p + 1, &p, 10);
	rpos.y = zmaee_strtol(p + 1, &p, 10);
	return rpos;
}

static bool StrStepCheck(const char* p, int* len)
{
	unsigned char c = p[0];
	if((c & 0x80) == 0)
		*len = 1;
	else if((c & 0xE0) == 0xC0)
		*len = 2;
	else if((c & 0xF0) == 0xE0)
		*len = 3;
	else
	{
		if((c & 0xF8) == 0xF0)
			*len = 4;
		else
			*len = 5;
		return false;
	}
	return true;
}

int DUIStringUtil::Utf8CharactorLen(const AEEStringRef& str)
{
	int step = 0, count = 0;
	const char* p = str.ptr();
	
	while(*p && (p < str.ptr() + str.length()))
	{
		if(StrStepCheck(p, &step) == false)
			break;
		p += step;
		++count;
	}
	return count;
}

TString DUIStringUtil::Utf8StrDup(const AEEStringRef& str)
{
	int ul, l; 

	ul = Utf8CharactorLen(str);
	l = (ul + 1) * 2;

	TString p = (TString)AEEAlloc::Alloc(l);
	ZMAEE_Utf8_2_Ucs2(str.ptr(), str.length(), p, ul+1);

	return p;
}

char* DUIStringUtil::strdup(const AEEStringRef& str)
{
	char* p = (char*)AEEAlloc::Alloc(str.length() + 1);
	if(p)
	{
		zmaee_memcpy(p, str.ptr(), str.length());
		p[str.length()] = 0;
	}
	return p;
}
