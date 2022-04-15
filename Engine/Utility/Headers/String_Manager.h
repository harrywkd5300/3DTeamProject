//Jangwon
#pragma once

#include "Engine_Defines.h"
#include "base.h"

BEGIN(Engine)


class DLL_EXPORT CString_Manager :public CBase
{
	DECLARE_SINGLETON(CString_Manager)
private:
	explicit CString_Manager();
	virtual ~CString_Manager() = default;

protected:
	list<_tchar*> m_tcharList;
	list<char*> m_charList;

public:
	inline _tchar* GetStr(const _tchar* str)
	{
		_tchar* tmpStr = new _tchar[lstrlen(str)+1];
		lstrcpy(tmpStr, str);
		m_tcharList.push_back(tmpStr);
		return tmpStr;
	}
	inline _tchar* GetTchar(int iLen)
	{
		_tchar* tmpStr = new _tchar[iLen];
		lstrcpy(tmpStr, L"");
		m_tcharList.push_back(tmpStr);
		return tmpStr;
	}
	inline char* GetStr(const char* str)
	{
		int len = int(strlen(str) + 1);
		char* tmpStr = new char[len];
		strcpy_s(tmpStr,len,str);
		m_charList.push_back(tmpStr);
		return tmpStr;
	}
	inline char* GetChar(int iLen)
	{
		char* tmpStr = new char[iLen];
		strcpy_s(tmpStr, iLen, "");
		m_charList.push_back(tmpStr);
		return tmpStr;
	}

private:
	virtual _ulong Free();
};

END