#include "AEEJsonParser.h"

IAEEJsonHandler::~IAEEJsonHandler()
{
}

class AEEJsonTokener
{
public:
	AEEJsonTokener(const char* szJson, int nLen)
		:mJsonContent(szJson), mLen(nLen) {
		mPosition = 0;
		if( mLen < 0 ) 
			mLen = zmaee_strlen(szJson);
	}

	void Back(){
		if(mPosition>0) --mPosition;
	}

	bool More() const{
		return (mPosition < mLen);
	}

	int Next()
	{
		if( More() )
			return mJsonContent[ mPosition++ ]; 
		return 0;
	}

	int Next(char aChar)
	{
		char n = Next();
		if(n != aChar)
			return AEEJsonParser::KErrNotFound;
		return n;
	}

	int Next(int aLength, AEEStringRef& str)
	{
		if( mPosition + aLength >= mLen )
			return (AEEJsonParser::KErrOverflow);
		str.set(mJsonContent+mPosition, aLength);
		return AEEJsonParser::KErrNone;
	}

	int NextClean()
	{
		for (;;) 
		{
			int c = Next();
			if (c == '/') 
			{
				switch( Next() ) 
				{
				case '/':
					do 	{
						c = Next();
					} while (c != '\n' && c != '\r' && c != 0);
					break;
					
				case '*':
					for (;;) 
					{
						c = Next();
						if (c == 0)
							return  AEEJsonParser::KErrNotSupported;

						if (c == '*') 
						{
							if (Next() == '/')
								break;
							Back();
						}
					}
					break;

				default:
					Back();
					break;
				}
			}
			else if (c == '#') 
			{
				do {
					c = Next();
				} while (c != '\n' && c != '\r' && c != 0);
			} 
			else if (c == 0 || c > ' ')
			{
				return c;
			}
		}
		return 0;
	}

	int NextString(char aQuote, AEEStringRef& str)
	{
		char c;
		int pos = mPosition, length = 0;
		
		for (;;) 
		{
			c = Next();		
			switch (c) 
			{
			case 0:
			case '\n':
			case '\r':
				return AEEJsonParser::KErrNotSupported;
				
			case '\\':
				Next();
				++length;
				break;
				
			default:
				if (c == aQuote)
				{
					str.set(mJsonContent+pos, length);
					return AEEJsonParser::KErrNone;
				}
			}
			++length;
		}
		str = AEEStringRef();
		return AEEJsonParser::KErrNone;
	}
	
	int NextTo(char aChar, AEEStringRef& str)
	{
		int pos = mPosition, length = 0;
		for (;;) 
		{
			char c = Next();		
			
			if (c == aChar || c == 0 || c == '\n' || c == '\r') 
			{
				if (c != 0)
					Back();
				str.set(mJsonContent+pos, length);
				return AEEJsonParser::KErrNone;
			}
			++length;
		}

		return AEEJsonParser::KErrNotFound;
	}
	
	int NextTo(const char* aDelimiters,AEEStringRef& str)
	{
		char c;
		int pos = mPosition, length = 0;
		int tmp_len = zmaee_strlen(aDelimiters);
		for (;;) 
		{
			c = Next();
			for(int i = 0 ; i < tmp_len; ++i)
			{
				if (c == aDelimiters[i] || c == 0 || c == '\n' || c == '\r') 
				{
					if (c != 0)
						Back();

					str.set(mJsonContent+pos, length);
					return AEEJsonParser::KErrNone;
				}
			}
			++length;
		}
		return AEEJsonParser::KErrNotFound;
	}

private:
	const char* mJsonContent;
	int mLen;
	int mPosition;
};


//////////////////////////////////////////////////////////////////////////
//class AEEJsonParser
//////////////////////////////////////////////////////////////////////////
AEEJsonParser::AEEJsonParser(IAEEJsonHandler& aHandler) 
	:mHandler(aHandler)
{
	mStateStack = new AEEStack<TJsonParserState>(16);
}

AEEJsonParser::~AEEJsonParser()
{
	if(mStateStack)
	{
		delete mStateStack;
		mStateStack = NULL;
	}
}

int AEEJsonParser::Parse(const char* szJson, int nLen /*= -1*/)
{
	AEEJsonTokener aTokener(szJson, nLen);
	AEEJsonTokener* iTokener = &aTokener;

	char c;
	for(;;)
	{
		c = iTokener->NextClean();
		switch( c )
		{
		case 0:
			return KErrNone;

		case '{':
			mStateStack->push( EParseObject );
			mHandler.OnBeginObject();
			break;

		case '}':
			if( mStateStack->top() == EParseValue )
				mStateStack->pop();
			if( mStateStack->top() != EParseObject )
			{
				return KErrNotSupported;
			}
			mStateStack->pop();
			mHandler.OnEndObject();
			break;

		case '[':				
			mStateStack->push( EParseArray );
			mHandler.OnBeginArray();
			break;

		case ']':
			if( mStateStack->top() == EParseValue )
				mStateStack->pop();
			if( mStateStack->top() != EParseArray )
			{
				return KErrNotSupported;
			}
			mStateStack->pop();
			mHandler.OnEndArray();
			break;

		case ':':
			mStateStack->push( EParseValue );
			break;

		case ',':
			if( mStateStack->top() == EParseValue )
				mStateStack->pop();
			break;

		case '\"':
			{
				 iTokener->NextString( '\"', mLastString );
				if( mStateStack->top() != EParseArray && 
					mStateStack->top() != EParseValue)
				{
					mHandler.OnKey( mLastString );
				}
				else
				{
					if( mStateStack->top() == EParseValue )
						mStateStack->pop();
					mHandler.OnValue( mLastString );
				}
			}
			break;

		default:
			{
				iTokener->Back();
				iTokener->NextTo(",}]", mLastString);
				mHandler.OnValue( mLastString );
				if( mStateStack->top() == EParseValue )
					mStateStack->pop();
			}
			break;
		}
	}

	return KErrNone;
}
