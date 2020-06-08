#ifndef _DUI_STRINGTABLE_H_
#define _DUI_STRINGTABLE_H_

#include "dui_util.h"
#include "zmaee_stdlib.h"
#include "AEEFile.h"

/**
 * 格式说明：
 [flag]			"zmstrtab"
 [int]			string_items_count,
 [stringitem]	string_items
 [int]			string_table_size,
 [wchar]		string\0string\0
 */
class DUIStringTable
{
public:
	DUIStringTable();
	~DUIStringTable();
	bool Load(AEEFile* pFile);
	KString GetString(int nID) const;
protected:
	struct StringItem {
		int mID;
		int mOffset;
	};
protected:
	int mCount;
	StringItem* mItems;
	int mStrTabSize;
	TString mStrTab;
};

#endif//_DUI_STRINGTABLE_H_
