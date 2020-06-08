#include "dui_spacer.h"
#include "dui_stringutil.h"

DUISpacer::DUISpacer(bool isHoriz):mIsHoriz(isHoriz)
{
	mClassID = DUIStringUtil::StrHash("spacer");
}

DUISpacer::~DUISpacer(){

}

