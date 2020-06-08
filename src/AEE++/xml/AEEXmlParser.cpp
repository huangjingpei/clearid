#include "zmaee_stdlib.h"
#include "zmaee_helper.h"
#include "AEEXmlParser.h"

AEEXmlHandler::~AEEXmlHandler()
{
}
AEEXMLParser::AEEXMLParser(AEEXmlHandler* pHandler)
{
	m_pHandler = pHandler;
}

AEEXMLParser::~AEEXMLParser()
{
}

char* AEEXMLParser::skipWhite(char* p)
{
	while(*p==' ' || *p=='\r' || *p=='\n' || *p=='\t')
		++p;
	return p;
}

char* AEEXMLParser::skipTo(char* p, char ch)
{
	while(*p != ch) ++p;
	return p;
}

AEEStringRef AEEXMLParser::readString(char** p, const char* brkstr)
{
	char* pStart;
	char* pEnd = zmaee_strpbrk(*p, brkstr);
	if(pEnd==NULL) 
		return AEEStringRef();
	pStart = *p;
	*p = pEnd;
	return AEEStringRef(pStart, pEnd-pStart);
}

AEEStringRef AEEXMLParser::readCharacters(char** p)
{
	char* pStart = *p;
	char* pEnd = 0;
	pEnd = zmaee_strpbrk(*p, "<");
	if(pEnd && zmaee_memcmp(pEnd, (void*)"<![CDATA[", 9) == 0)
	{
		*p = pEnd + 9;
		pEnd = zmaee_strpbrk(*p, "<");
	}

	if(pEnd==NULL) 
		return AEEStringRef();
	*p = pEnd;
	return AEEStringRef(pStart, pEnd-pStart);
}

void AEEXMLParser::parse(const char* strxml, int len)
{
	if(strxml == NULL)
		return;

	if(len == 0) len = zmaee_strlen(strxml);

	//skip utf8 file head
	char* p = (char*)strxml;
	if(zmaee_memcmp((void*)p, (void*)"\xEF\xBB\xBF", 3) == 0)
		p += 3;
	
	/* start document */
	m_pHandler->startDocument();

	/* parse node */
	while(*p)
	{
		p = skipWhite(p);
		if(*p == '<')
		{
			//<
			++p;
			if(p[0] == '?' || p[0] == '!')
			{
				p = skipTo(p, '>');
				//>
				++p;
				continue;
			}
			else if(p[0] == '/')
			{
				++p;
				AEEStringRef name = readString(&p, ">");
				m_pHandler->endElement(name);
				//>
				++p;
			}
			else
			{
				AEEStringRef name = readString(&p, "\t />");
				m_pHandler->startElement(name);
				p = skipWhite(p);
				if(*p != '>' && *p != '/')
				{
					//´æÔÚÊôÐÔ
					while(*p != '>' && *p != '/')
					{
						p = skipWhite(p);
						AEEStringRef attrName = readString(&p, " =");
						p = skipTo(p, '=');
						char* pTemp = zmaee_strpbrk(p, "\"'");
						if (pTemp)
							p = pTemp;
						else
						{
							m_pHandler->endDocument();
							return;
						}
						++p;
						AEEStringRef attrValue = readString(&p, "\"'");
						++p;
						if(attrName.length()>0 && attrValue.length()>0)
							m_pHandler->attribute(attrName, attrValue/*attrValue.entityDecode()*/);
						p = skipWhite(p);
					}
				}
				if(*p == '/') 
				{
					++p;
					m_pHandler->endElement(name);
				}
				//>
				++p;
			}
		}
		else
		{
			p = skipWhite(p);
			if (*p)
			{
				AEEStringRef strText = readCharacters(&p);
				if (strText.length() > 0)
					m_pHandler->characters(/*strText.textDecode()*/strText);
			}
		}
	}
	/* end document */
	m_pHandler->endDocument();
}
