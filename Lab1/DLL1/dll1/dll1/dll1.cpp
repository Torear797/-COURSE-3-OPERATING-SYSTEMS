#include "header.h"
extern "C" __declspec(dllexport) void CSIDL(const long csid[],unsigned long nCount)
{
	CHAR szPath[MAX_PATH + 1];
	for(unsigned long i=0;i < nCount;++i) 
	{
	
	SHGetFolderPathA(
	NULL,
	csid[i],
	NULL,
	SHGFP_TYPE_DEFAULT,
	szPath);
	if(S_OK != S_FALSE)
	cout <<i+1 <<". " << szPath << endl;
	}

}
extern "C" __declspec(dllexport) void OS_Print()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx((OSVERSIONINFO*)&osvi);
	cout << "ID OS: " << osvi.dwMajorVersion << endl;
if(osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0) cout << "Версия ОС: Microsoft Windows 10" << endl;
else
if (osvi.dwMajorVersion == 6.3 && osvi.dwMinorVersion ==3) cout << "Версия ОС: Microsoft Windows 8.1" << endl;
else
if (osvi.dwMajorVersion == 6.2 && osvi.dwMinorVersion == 2) cout << "Версия ОС: Microsoft Windows 8" << endl;
	cout << "Ид Версии: " << osvi.dwMinorVersion << endl;
	cout << "ИД Сборки: " << osvi.dwBuildNumber << endl;
	cout << "Платформа: " << osvi.dwPlatformId << endl;

}
extern "C" __declspec(dllexport) void DataTime_Print()
{
	TCHAR svDate[260];
	TCHAR svTime[260];
	GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, NULL, TEXT("dd-MM-yyyy"), svDate, 260);
	GetTimeFormatEx(LOCALE_NAME_INVARIANT, TIME_FORCE24HOURFORMAT, NULL,L"hh':'mm':'ss tt", svTime, 260);
	wcout << svDate << endl;
	wcout << svTime << endl;
}