#pragma once
#include "stdafx.h"

class CIniWriter
{
public:
	CIniWriter(LPCTSTR szFileName);
	void WriteInteger(LPCTSTR szSection, LPCTSTR szKey, int iValue);
	void WriteFloat(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR format, float fltValue);
	void WriteBoolean(LPCTSTR szSection, LPCTSTR szKey, bool bolValue);
	void WriteString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue);
private:
	TCHAR m_szFileName[255];
};