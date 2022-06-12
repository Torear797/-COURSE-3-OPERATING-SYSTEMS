#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <locale.h>
#include <iostream>
#include <strsafe.h>
extern LPCTSTR gSvcName;
extern LPCTSTR gSvcDisplayName;
SERVICE_STATUS gSvcStatus;
SERVICE_STATUS_HANDLE gSvcStatusHandle;										
BOOL OnSvcInit(DWORD dwArgc, LPTSTR *lpszArgv);
void OnSvcStop(void);
DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv);
DWORD WINAPI SvcHandler(DWORD fdwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	if (SERVICE_CONTROL_STOP == fdwControl || SERVICE_CONTROL_SHUTDOWN == fdwControl)
	{
		OnSvcStop();
		gSvcStatus.dwCurrentState = SERVICE_STOP_PENDING;
	}
	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
	return NO_ERROR;
}
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	gSvcStatusHandle = RegisterServiceCtrlHandlerEx(gSvcName, SvcHandler, NULL);
	if (NULL != gSvcStatusHandle)
	{
		gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		gSvcStatus.dwCurrentState = SERVICE_START_PENDING;
		gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
		gSvcStatus.dwWin32ExitCode = NO_ERROR;
		gSvcStatus.dwServiceSpecificExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 0;
		SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
		if (OnSvcInit(dwArgc, lpszArgv) != FALSE)
		{
			gSvcStatus.dwCurrentState = SERVICE_RUNNING;													
			SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
			DWORD dwExitCode = SvcMain(dwArgc, lpszArgv);
			if (dwExitCode != 0)
			{
				gSvcStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
				gSvcStatus.dwServiceSpecificExitCode = dwExitCode;
			}
			else
			{
				gSvcStatus.dwWin32ExitCode = NO_ERROR;
			}
		}
		gSvcStatus.dwCurrentState = SERVICE_STOPPED;								
		SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
	}
}
int _tmain(int argc, LPTSTR argv[])
{
	_tsetlocale(LC_ALL, TEXT(""));
	if (argc < 2)
	{
		_tprintf(TEXT("> Не указан параметр.\n"));
		return 0;
	}
	if (_tcscmp(argv[1], TEXT("/runservice")) == 0)
	{
		SERVICE_TABLE_ENTRY svcDispatchTable[] =
		{
			{ (LPTSTR)gSvcName, ServiceMain },
		{ NULL, NULL }
		};
		StartServiceCtrlDispatcher(svcDispatchTable);
		int Error = GetLastError();
		std::cout << "Number Error = " << Error << "\n";
		return 0;
	}
	if (_tcscmp(argv[1], TEXT("/create")) == 0)
	{
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		if (NULL == hSCM)
		{
			_tprintf(TEXT("> Не удалось открыть диспетчер управления службами Windows.\n"));
			return -1;
		}
		TCHAR szCmdLine[MAX_PATH + 13];
		GetModuleFileName(NULL, szCmdLine, _countof(szCmdLine));
		StringCchCat(szCmdLine, _countof(szCmdLine), TEXT(" /runservice"));
		SC_HANDLE hSvc = CreateService(hSCM, gSvcName, gSvcDisplayName, 0,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL, szCmdLine, NULL, NULL, NULL, NULL, NULL);
		if (NULL != hSvc)
		{
			_tprintf(TEXT("> Служба успешно создана!\n"));
			CloseServiceHandle(hSvc);
		}
		else
		{
			_tprintf(TEXT("> Не удалось создать службу.\n"));
		}

		CloseServiceHandle(hSCM);
		return 0;
	}
	if (_tcscmp(argv[1], TEXT("/delete")) == 0)
	{
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		if (NULL == hSCM)
		{
			_tprintf(TEXT("> Не удалось открыть диспетчер управления службами Windows.\n"));
			return -1;
		}
		SC_HANDLE hSvc = OpenService(hSCM, gSvcName, DELETE);
		if (NULL != hSvc)
		{
			BOOL bRet = DeleteService(hSvc);
			if (FALSE != bRet) _tprintf(TEXT("> Служба успешно удалена!\n"));
			else _tprintf(TEXT("> Не удалось удалить службу.\n"));
			CloseServiceHandle(hSvc);
		}
		else
		{
			_tprintf(TEXT("> Не удалось удалить службу.\n"));
		}
		CloseServiceHandle(hSCM);
		return 0;
	} 
	_tprintf(TEXT("> Неизвестный параметр.\n"));
}