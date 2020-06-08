#ifndef _AEE_XML_PARSER_H_
#define _AEE_XML_PARSER_H_

#include "AEEBase.h"
#include "util/AEEStringRef.h"

class AEEXmlHandler
{
public:
	virtual ~AEEXmlHandler();
	virtual void startDocument() = 0;
	virtual void endDocument() = 0;
	virtual void startElement(const AEEStringRef& strName) = 0;
	virtual void endElement(const AEEStringRef& strName) = 0;
	virtual void attribute(const AEEStringRef& strName, const AEEStringRef& strValue) = 0;
	virtual void characters(const AEEStringRef& strValue) = 0;
};

class AEEXMLParser : public AEEBase
{
public:
	AEEXMLParser(AEEXmlHandler* pHandler);
	virtual ~AEEXMLParser();
	void parse(const char* strxml, int len=0);
	char* skipWhite(char* p);
	char* skipTo(char* p, char ch);
	AEEStringRef readString(char** p, const char* brkstr);
	AEEStringRef readCharacters(char** p);
protected:
	AEEXmlHandler* m_pHandler;
};

#endif//_AEE_XML_PARSER_H_
