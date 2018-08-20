#include "stdafx.h"
#include "IniWriter.h"

CIniWriter::CIniWriter(LPCTSTR szFileName)
{
	memset(m_szFileName, 0x00, sizeof(m_szFileName));
	memcpy(m_szFileName, szFileName, _tcslen(szFileName) * sizeof(TCHAR));
}
void CIniWriter::WriteInteger(LPCTSTR szSection, LPCTSTR szKey, int iValue)
{
	TCHAR szValue[255];
	_stprintf_s(szValue, 255, TEXT("%d"), iValue);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CIniWriter::WriteFloat(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR format, float fltValue)
{
	TCHAR szValue[255];
	_stprintf_s(szValue, 255, (wchar_t *)format, fltValue);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CIniWriter::WriteBoolean(LPCTSTR szSection, LPCTSTR szKey, bool bolValue)
{
	TCHAR szValue[255];
	_stprintf_s(szValue, 255, TEXT("%s"), bolValue ? TEXT("True") : TEXT("False"));
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CIniWriter::WriteString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue)
{
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
