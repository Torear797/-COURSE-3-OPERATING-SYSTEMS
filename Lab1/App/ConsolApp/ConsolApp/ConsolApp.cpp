#include "st.h"
#include <iostream>
#include <ShlObj.h>
#include <DelayImp.h>
extern "C" __declspec(dllimport) void CSIDL(const long csid[], unsigned long nCount);
extern "C" __declspec(dllimport) void OS_Print();
extern "C" __declspec(dllimport) void DataTime_Print();
extern "C" __declspec(dllimport) void SystemParametersInfoPrint1();
extern "C" __declspec(dllimport) void SystemParametersInfoPrint2();
using namespace std;
typedef void(*CSIDL_Print)(const long csidl[],unsigned long nCount);
typedef void(*PRINT)();
int main()
{
	setlocale(LC_CTYPE, "");
	cout << "___________________________________" << endl << endl;
	cout << "Статическая библиотека" << endl << endl;
	print::LovalName();
	print::domenName();
	print::userName();
	print::fulluserName();
	const long csidl[] = {
		CSIDL_APPDATA,
		CSIDL_COMMON_APPDATA,
		CSIDL_COMMON_DOCUMENTS,
		CSIDL_HISTORY,
		CSIDL_INTERNET_CACHE,
		CSIDL_LOCAL_APPDATA,
		CSIDL_PERSONAL,
		CSIDL_PROGRAM_FILES,
		CSIDL_PROGRAM_FILES_COMMON,
		CSIDL_SYSTEM,
		CSIDL_WINDOWS
	};
	cout << "___________________________________" << endl << endl;
	cout << "DLL 1" << endl << endl;
	CSIDL(csidl, _countof(csidl)); cout << endl;
	OS_Print(); cout << endl;
	DataTime_Print();
	cout << "___________________________________" << endl << endl;
	cout << "DLL 2" << endl << endl;
	HMODULE	hMyDll2 = LoadLibrary(TEXT("dll2.dll"));
	if (NULL != hMyDll2)
	{
		PRINT func = (PRINT)GetProcAddress(hMyDll2, "GetSystemMetricsPrint");
		if (func != NULL)  func();
		FreeLibrary(hMyDll2);
	}
	cout << "___________________________________" << endl << endl;
	cout << "DLL 3" << endl << endl;
		__try
			{
			SystemParametersInfoPrint1();
			SystemParametersInfoPrint2();
			} 
		__except (EXCEPTION_EXECUTE_HANDLER)
			{
			switch (GetLastError())
				{
			case ERROR_MOD_NOT_FOUND:
				
					break;
			case ERROR_PROC_NOT_FOUND:
				
					break;   } 
		 } 
			__FUnloadDelayLoadedDLL2("DLL3.dll");
	system("Pause");
	return 0;
}