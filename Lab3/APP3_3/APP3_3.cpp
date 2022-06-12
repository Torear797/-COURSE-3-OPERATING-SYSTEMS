#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <windowsX.h> 
#include <process.h>
#include <Psapi.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
using namespace std;
int _tmain(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	if (argc == 1)
	{
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi;
		TCHAR szCmdLine[MAX_PATH] = "APP3_3.exe Instance";
		BOOL bRet;
		HANDLE hEvent = CreateEvent(NULL, FALSE, TRUE, "Event1");
		cout << "MAIN PROGRAM";
		HANDLE* hT = new HANDLE[3];
		for (int i = 0; i < 3; i++)
		{
			bRet = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
			hT[i] = pi.hProcess;
			CloseHandle(pi.hThread);
		}
		WaitForMultipleObjects(3, hT, TRUE, INFINITE);
		for (int i = 0; i < 3; i++)
			CloseHandle(hT[i]);
		CloseHandle(hEvent);
	}
	else if (argc == 2)
	{
		cout << argv[1] << endl;
		HANDLE THandle = OpenEvent(EVENT_ALL_ACCESS, TRUE, "Event1");
		DWORD dwResult;
		for (int i = 0; i<3; i++)
		{
			dwResult = WaitForSingleObject(THandle, INFINITE);
			if (WAIT_FAILED != dwResult)
			{
				for (int j = 1; j < 11; j++) {
					cout << j << endl;
					Sleep(200);
				}
				SetEvent(THandle);
			}
		}
		CloseHandle(THandle);
	}
}