#pragma once
#include "stdafx.h"

class CIniReader
{
public:
	CIniReader(LPCTSTR szFileName);
	int ReadInteger(LPCTSTR szSection, LPCTSTR szKey, int iDefaultValue);
	float ReadFloat(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR format, float fltDefaultValue);
	bool ReadBoolean(LPCTSTR szSection, LPCTSTR szKey, bool bolDefaultValue);
	LPTSTR ReadString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szDefaultValue);
private:
	TCHAR m_szFileName[255];
};