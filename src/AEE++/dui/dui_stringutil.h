#ifndef _DUI_STRING_UTIL_H_
#define _DUI_STRING_UTIL_H_

#include "zmaee_typedef.h"
#include "util/AEEStringRef.h"
#include "dui_util.h"

class DUIStringUtil
{
public:
	/**
	 * return: string hash value
	 */
	static unsigned int StrHash(const AEEStringRef& str);

	/**
	 * str format:
		x,y,width,height
	 */
	static TRect ParseRect(const AEEStringRef& str);

	/**
	 * str format:
		int
	 */
	static int ParseInt(const AEEStringRef& str);

	/**
	 * str format:
		x, y
	 */
	static TPoint ParsePoint(const AEEStringRef& str);

	/**
	 * str format
	  [TOPLEFT|TOPRIGHT|LEFTBOTTOM|RIGHTBOTTOM|CENTER],offset_x, offset_y
	 */
	static TRelativePos ParseRelativePos(const AEEStringRef& str);

	/**
	 * str format:
		#rrggbbaa
	 */
	static TColor ParseColor(const AEEStringRef& str);

	/**
	 * str format:
		left|right|center|top|vcenter|bottom
	 */
	static ZMAEE_TextAlign ParseAlign(const AEEStringRef& str);

	/**
	 *  str == "0" || str == "false"
			return false
		else
			return true
	 */
	static bool ParseBool(const AEEStringRef& str);

	/**
	 * return utf8 charactor length[×Ö·ûÊý]
	 */
	static int Utf8CharactorLen(const AEEStringRef& str);

	/**
	 * utf8->ucs2 decode
	 * return:
			AEEAlloc::malloc new buffer
	 */
	static TString Utf8StrDup(const AEEStringRef& str);

	/**
	 * dup str
	 */
	static char* strdup(const AEEStringRef& str);
};

#endif//_DUI_STRING_UTIL_H_
