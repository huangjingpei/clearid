#ifndef _AEE_JSON_PARSER_H_
#define _AEE_JSON_PARSER_H_

#include "AEEBase.h"
#include "util/AEEStack.h"
#include "util/AEEStringRef.h"

class IAEEJsonHandler
{
public:
	virtual ~IAEEJsonHandler();
	virtual void OnBeginObject() = 0;
	virtual void OnEndObject() = 0;
	virtual void OnBeginArray() = 0;
	virtual void OnEndArray() = 0;
	virtual void OnKey(const AEEStringRef& aKey) = 0;
	virtual void OnValue(const AEEStringRef& aValue) = 0;
};

class AEEJsonParser : public AEEBase
{
	enum TJsonParserState{
		ENone,
		EParseObject,
		EParseArray,
		EParseKey,
		EParseValue,
		EParseComa,
		EParseColon
	};
public:
	enum{
		KErrNone = 1,
			KErrEndOfFile = 0,
			KErrOverflow = -1,
			KErrNotFound = -2,
			KErrNotSupported = -3
	};
	AEEJsonParser(IAEEJsonHandler& aHandler);
	virtual ~AEEJsonParser();
	int Parse(const char* szJson, int nLen = -1);

	const AEEStringRef& LastString() const{
		return mLastString;
	}

private:
	IAEEJsonHandler& mHandler;
	const char* mContent;
	int mLen;
	int mPosition;
	AEEStack<TJsonParserState>* mStateStack;
	AEEStringRef mLastString;
};

#endif//_AEE_JSON_PARSER_H_
