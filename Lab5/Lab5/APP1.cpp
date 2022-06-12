#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h> 
#include <random>  
#include <process.h>
#include "psapi.h"
#include "strsafe.h"
#include <tchar.h>
#include <sddl.h>
#include <strsafe.h>
#include <LMCons.h>
#include <NTSecAPI.h>
#include <ntstatus.h>
MSG msg;
HWND hwnd = NULL;
BOOL bRet;
HWND hDlg = NULL;
HWND hFindDlg = NULL;
HACCEL hAccel = NULL;
#define IDR_MENU1  101
#define IDR_ACCELERATOR1  2020
#define IDC_EDIT_TEXT  2001
#define ID_Write  2002

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL PreTranslateMessage(LPMSG lpMsg);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
BOOL GetTokenGroups(HANDLE hToken, PTOKEN_GROUPS *pTokenGroups);
BOOL GetAccountSID(LPCTSTR lpAccountName, PSID * ppSid);
HANDLE OpenProcessTokenByProcessId(DWORD dwProcessId, DWORD dwDesiredAccess);
BOOL GetWellKnownSID(WELL_KNOWN_SID_TYPE WellKnownSidType, PSID pDomainSid, PSID * ppSid);

BOOL GetAccountName(PSID pSid, LPTSTR * lpName);

static void InitUnicodeString(LPCWSTR lpString, PLSA_UNICODE_STRING pUnicodeString);

LSA_HANDLE OpenLocalPolicy(ACCESS_MASK DesiredAccess);

void WritePrivelegii(PSID SampleSid);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = TEXT("Windowslass");;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (0 == RegisterClassEx(&wcex)) // регистрирует класс
	{
		MessageBox(NULL, TEXT("Не удалось зарегестрировать класс!"), TEXT("Ошибка"), MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	LoadLibrary(TEXT("ComCtl32.dll"));

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Windowslass"), TEXT("Приложение № 1"), WS_OVERLAPPEDWINDOW,
		700, 86, 640, 700, NULL, NULL, hInstance, NULL);

	if (NULL == hwnd)
	{
		MessageBox(NULL, TEXT("Не удалось создать окно!"), TEXT("Ошибка"), MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}
	ShowWindow(hwnd, nCmdShow);

	for (;;)
	{
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			//OnIdle(hwnd);
		} // while
		bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet == -1)
		{
		}
		else if (FALSE == bRet)
		{
			break;
		}
		else if (!PreTranslateMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

BOOL PreTranslateMessage(LPMSG lpMsg)
{
	BOOL bRet = TRUE;

	if (!TranslateAccelerator(hwnd, hAccel, lpMsg))
	{
		bRet = IsDialogMessage(hDlg, lpMsg);

		if (FALSE == bRet)
			bRet = IsDialogMessage(hFindDlg, lpMsg);
	}

	return bRet;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_WANTRETURN | WS_VSCROLL;
	HWND hwndCtl = CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 10, 10, 600, 600, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, 0);
	CreateWindowEx(0, L"Button", L"Вывод", WS_CHILD | WS_VISIBLE, 200, 610, 220, 40, hwnd, (HMENU)ID_Write, lpCreateStruct->hInstance, NULL);
	return TRUE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
	case WM_CLOSE:
	{
		DestroyWindow(hwnd);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	static HWND hE = GetDlgItem(hwnd, IDC_EDIT_TEXT);
	constexpr WELL_KNOWN_SID_TYPE aWellKnown[] = {
		WinAccountGuestSid,
		WinBuiltinNetworkConfigurationOperatorsSid
	};
	switch (id)
	{

	case ID_Write: 
	{
		PSID SampleSid, SIDLocalPC, SIDUser;
		TCHAR buffer[UNLEN + 1] = TEXT("");
		DWORD size;
		LPTSTR SIDWrite;
		size = _countof(buffer);
		LPCWSTR SIDGOOD;
		GetComputerName(buffer, &size);
		GetAccountSID(buffer, &SampleSid);
		ListBox_AddItemData(hE, TEXT("SID локального компьютера:"));
		ConvertSidToStringSid(SampleSid, &SIDWrite); SIDLocalPC = SampleSid;
		ListBox_AddItemData(hE, buffer);
		ListBox_AddItemData(hE, SIDWrite);	ListBox_AddItemData(hE, "");

		SampleSid = NULL;
		GetUserName(buffer, &size);
		GetAccountSID(buffer, &SampleSid);
		ListBox_AddItemData(hE, TEXT("SID учетной записи текущего пользователя:"));
		ConvertSidToStringSid(SampleSid, &SIDWrite); SIDUser = SampleSid;
		ListBox_AddItemData(hE, buffer);
		ListBox_AddItemData(hE, SIDWrite); ListBox_AddItemData(hE, "");
		//WritePrivelegii(SampleSid);

		ListBox_AddItemData(hE, TEXT("Имена хорошо известных SID:")); ListBox_AddItemData(hE, "");
		SIDGOOD = TEXT("S-1-1-0");
		ConvertStringSidToSid(SIDGOOD, &SampleSid);
		GetAccountName(SampleSid, &SIDWrite);
		ListBox_AddItemData(hE, TEXT("S - 1 - 1 - 0: "));
		ListBox_AddItemData(hE, SIDWrite); ListBox_AddItemData(hE, "");

		SIDGOOD = TEXT("S-1-5-11");
		ConvertStringSidToSid(SIDGOOD, &SampleSid);
		GetAccountName(SampleSid, &SIDWrite);
		ListBox_AddItemData(hE, TEXT("S - 1 - 5 - 11: "));
		ListBox_AddItemData(hE, SIDWrite); ListBox_AddItemData(hE, "");

		SIDGOOD = TEXT("S-1-5-20");
		ConvertStringSidToSid(SIDGOOD, &SampleSid);
		GetAccountName(SampleSid, &SIDWrite);
		ListBox_AddItemData(hE, TEXT("S - 1 - 5 - 20: "));
		ListBox_AddItemData(hE, SIDWrite); ListBox_AddItemData(hE, "");

		//SIDGOOD = TEXT("S-1-5-21-4244118458-1117717580-575774867-1001-501");
	//	ConvertStringSidToSid(SIDGOOD, &SampleSid);
		GetWellKnownSID(aWellKnown[0], SIDLocalPC, &SampleSid);
		GetAccountName(SampleSid, &SIDWrite);
		ListBox_AddItemData(hE, TEXT("S - 1 - 5 - 21 - X - X - X - 501: "));
		ListBox_AddItemData(hE, SIDWrite); ListBox_AddItemData(hE, "");

		SIDGOOD = TEXT("S-1-5-32-546");
		ConvertStringSidToSid(SIDGOOD, &SampleSid);
		GetAccountName(SampleSid, &SIDWrite);
		ListBox_AddItemData(hE, TEXT("S - 1 - 5 - 32 - 546: "));
		ListBox_AddItemData(hE, SIDWrite); ListBox_AddItemData(hE, "");

	    SIDGOOD = TEXT("S-1-5-32-556");
		ConvertStringSidToSid(SIDGOOD, &SampleSid);
		//GetWellKnownSID(aWellKnown[1], SIDLocalPC, &SampleSid);
		GetAccountName(SampleSid, &SIDWrite);
		ListBox_AddItemData(hE, TEXT("S - 1 - 5 - 32 - 556: "));
		ListBox_AddItemData(hE, SIDWrite); ListBox_AddItemData(hE, "");

		

		//SIDGOOD = TEXT("S-1-5-32-544");
	//	ConvertStringSidToSid(SIDGOOD, &SampleSid);

		ListBox_AddItemData(hE, TEXT("Список привелегий и прав текущего пользователя:")); ListBox_AddItemData(hE, "");
		DWORD dwProcessId = GetCurrentProcessId();
		HANDLE hToken = OpenProcessTokenByProcessId(dwProcessId, TOKEN_QUERY);
		if (NULL != hToken)
		{
			PTOKEN_GROUPS pTokenGroups = NULL;
			bRet = GetTokenGroups(hToken, &pTokenGroups);
			if (FALSE != bRet)
			{
				for (int i = 0; i < pTokenGroups->GroupCount; i++)
				{
					LPTSTR lpString = NULL;
					GetAccountName(pTokenGroups->Groups[i].Sid, &lpString);
					if (NULL != lpString)
					{
						WritePrivelegii(pTokenGroups->Groups[i].Sid);
						//ListBox_AddItemData(GroupList, lpString);
						LocalFree(lpString),
						lpString = NULL;
					}
				}
			}
			LocalFree(pTokenGroups);

			
		}
		CloseHandle(hToken);

		ListBox_AddItemData(hE, TEXT("Список привелегий и прав для хорошо известных SID:")); ListBox_AddItemData(hE, "");
	
		ListBox_AddItemData(hE, TEXT("S-1-1-0:"));
		SIDGOOD = TEXT("S-1-1-0");
		ConvertStringSidToSid(SIDGOOD, &SampleSid);
		WritePrivelegii(SampleSid);

		ListBox_AddItemData(hE, TEXT("S-1-5-11:"));
		SIDGOOD = TEXT("S-1-5-11");
		ConvertStringSidToSid(SIDGOOD, &SampleSid);
		WritePrivelegii(SampleSid);

		ListBox_AddItemData(hE, TEXT("S-1-5-20:"));
		SIDGOOD = TEXT("S-1-5-20");
		ConvertStringSidToSid(SIDGOOD, &SampleSid);
		WritePrivelegii(SampleSid);

		ListBox_AddItemData(hE, TEXT("S-1-5-21-X-X-X-501:"));
		GetWellKnownSID(aWellKnown[0], SIDLocalPC, &SampleSid);
		//ListBox_AddItemData(hE, SIDWrite);
	//	SIDGOOD = TEXT("S-1-5-21-4244118458-1117717580-575774867-501");
		//ConvertStringSidToSid(SIDGOOD, &SampleSid);
		WritePrivelegii(SampleSid);

		ListBox_AddItemData(hE, TEXT("S-1-5-32-546:"));
		SIDGOOD = TEXT("S-1-5-32-546");
		ConvertStringSidToSid(SIDGOOD, &SampleSid);
		WritePrivelegii(SampleSid);



		ListBox_AddItemData(hE, TEXT("S-1-5-32-556:"));
		SIDGOOD = TEXT("S-1-5-32-556");
		//GetWellKnownSID(aWellKnown[1], SIDLocalPC, &SampleSid);
		ConvertStringSidToSid(SIDGOOD, &SampleSid);
		WritePrivelegii(SampleSid);	
	}
	}
}
BOOL GetAccountSID(LPCTSTR lpAccountName, PSID *ppSid)
{
	PSID pSid = NULL;
	LPTSTR lpszDomainName = NULL;

	DWORD cbSid = 0, cchDomainName = 0;

	// определяем размеры буферов
	LookupAccountName(NULL, lpAccountName, NULL, &cbSid, NULL, &cchDomainName, NULL);

	if (cbSid > 0)
	{
		// выделяем блок памяти под буфер для SID
		pSid = (PSID)LocalAlloc(LMEM_FIXED, cbSid);
	} // if

	if (cchDomainName > 0)
	{
		// выделяем блок памяти под буфер для имени домена
		lpszDomainName = (LPTSTR)LocalAlloc(LMEM_FIXED, cchDomainName * sizeof(TCHAR));
	} // if

	BOOL bRet = FALSE;

	if (NULL != pSid && NULL != lpszDomainName)
	{
		SID_NAME_USE SidType;

		// определяем SID указанной учетной записи
		bRet = LookupAccountName(NULL, lpAccountName, pSid, &cbSid, lpszDomainName, &cchDomainName, &SidType);
	} // if

	if (FALSE != bRet)
	{
		*ppSid = pSid; // возвращаем полученный SID
	} // if
	else if (NULL != pSid)
	{
		LocalFree(pSid); // освобождаем выделенную память
	} // if

	  // освобождаем выделенную память
	if (NULL != lpszDomainName) LocalFree(lpszDomainName);

	return bRet;
} // GetAccountSID
BOOL GetWellKnownSID(WELL_KNOWN_SID_TYPE WellKnownSidType, PSID pDomainSid, PSID *ppSid)
{
	DWORD cbSid = SECURITY_MAX_SID_SIZE;

	// выделяем блок памяти под буфер для SID
	PSID pSid = (PSID)LocalAlloc(LMEM_FIXED, cbSid);
	if (NULL == pSid) return FALSE;

	// определяем SID 
	BOOL bRet = CreateWellKnownSid(WellKnownSidType, pDomainSid, pSid, &cbSid);

	if (FALSE != bRet)
	{
		*ppSid = pSid; // возвращаем полученный SID
	} // if
	else
	{
		LocalFree(pSid); // освобождаем выделенную память
	} // else

	return bRet;
} // GetWellKnownSID
BOOL GetAccountName(PSID pSid, LPTSTR *lpName)
{
	// проверяем корректность SID
	if (FALSE == IsValidSid(pSid)) return FALSE;

	LPTSTR lpszName = NULL;
	DWORD cch = 0, cchDomainName = 0;

	// определяем размер буфера
	LookupAccountSid(NULL, pSid, NULL, &cch, NULL, &cchDomainName, NULL);
	DWORD cb = (cch + cchDomainName) * sizeof(TCHAR);

	// выделяем блок памяти под буфер
	if (cb > 0) lpszName = (LPTSTR)LocalAlloc(LMEM_FIXED, cb);

	BOOL bRet = FALSE;
	SID_NAME_USE SidType;

	if (NULL != lpszName)
	{
		// определяем имя учетной записи
		bRet = LookupAccountSid(NULL, pSid, lpszName + cchDomainName, &cch, lpszName, &cchDomainName, &SidType);
	} // if

	if (FALSE != bRet)
	{
		if (SidTypeDomain != SidType)
		{
			if (cchDomainName > 0) lpszName[cchDomainName] = TEXT('\\');
			else StringCbCopy(lpszName, cb, lpszName + 1);
		} // if

		*lpName = lpszName; // возвращаем полученную строку
	} // if
	else
	{
		// если не удалось получить имя, преобразуем SID в строку
		ConvertSidToStringSid(pSid, lpName);

		// освобождаем выделенную память
		if (NULL != lpszName) LocalFree(lpszName);
	} // else

	return bRet;
} // GetAccountName
static inline void InitUnicodeString(LPCWSTR lpString, PLSA_UNICODE_STRING pUnicodeString)
{
	size_t cch = wcslen(lpString); // определяем длину строки

	pUnicodeString->Length = (USHORT)(cch * sizeof(WCHAR));
	pUnicodeString->MaximumLength = (USHORT)((cch + 1) * sizeof(WCHAR));
	pUnicodeString->Buffer = (PWSTR)lpString;
} // InitUnicodeString
LSA_HANDLE OpenLocalPolicy(ACCESS_MASK DesiredAccess)
{
	LSA_HANDLE lsahPolicy;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;

	// заполняем структуру LSA_OBJECT_ATTRIBUTES нулями
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	// получаем дескриптор объекта политики безопасности
	NTSTATUS ntsResult = LsaOpenPolicy(NULL, &ObjectAttributes, DesiredAccess, &lsahPolicy);

	// сохраняем код последней ошибки
	SetLastError(LsaNtStatusToWinError(ntsResult));

	// возвращаем дескриптор объекта политики безопасности
	return LSA_SUCCESS(ntsResult) ? lsahPolicy : NULL;
} // OpenLocalPolicy
void WritePrivelegii(PSID SampleSid)
{
	static HWND hE = GetDlgItem(hwnd, IDC_EDIT_TEXT);
	PLSA_UNICODE_STRING UserRights;
	ULONG nCountOfRights;
	LSA_HANDLE lsahPolicy = OpenLocalPolicy(POLICY_LOOKUP_NAMES);
	if (NULL != lsahPolicy) {

		NTSTATUS ntsResult = LsaEnumerateAccountRights(lsahPolicy, SampleSid, &UserRights, &nCountOfRights);

		if (LSA_SUCCESS(ntsResult))
		{
			for (ULONG i = 0; i < nCountOfRights; ++i)
			{
				LPCWSTR sUserRight = UserRights[i].Buffer;
				ListBox_AddItemData(hE, sUserRight);

				TCHAR WriteInformation[256];
				DWORD szWriteInformation = _countof(WriteInformation), dwLangId;

			BOOL bret =	LookupPrivilegeDisplayName(NULL, sUserRight, WriteInformation, &szWriteInformation, &dwLangId);
			if (FALSE != bRet)
			{
				ListBox_AddItemData(hE, WriteInformation); ListBox_AddItemData(hE, "");
			}
		////	else 
		//	{
		//		if (_tcscmp(SE_DENY_REMOTE_INTERACTIVE_LOGON_NAME, sUserRight) == 0)
		//		{
		//			ListBox_AddItemData(hE, TEXT("Запретить вход в систему через службы удаленных рабочих столов"));
		//		}
		//		if (_tcscmp(SE_REMOTE_INTERACTIVE_LOGON_NAME, sUserRight) == 0)
		//		{
		//			ListBox_AddItemData(hE, TEXT("Разрешать вход в систему через службы удаленных рабочих столов"));
		//		}
		//		if (_tcscmp(SE_DENY_SERVICE_LOGON_NAME, sUserRight) == 0)
		//		{
		//			ListBox_AddItemData(hE, TEXT("Отказать во входе в качестве службы"));
		//		}
		//		if (_tcscmp(SE_SERVICE_LOGON_NAME, sUserRight) == 0)
		//		{
		//			ListBox_AddItemData(hE, TEXT("Вход в качестве службы"));
		//		}
		//		if (_tcscmp(SE_DENY_BATCH_LOGON_NAME, sUserRight) == 0)
		//		{
		//			ListBox_AddItemData(hE, TEXT("Отказать во входе в качестве пакетного задания"));
		//		}
		//		if (_tcscmp(SE_BATCH_LOGON_NAME, sUserRight) == 0)
		//		{
		//			ListBox_AddItemData(hE, TEXT("Вход в качестве пакетного задания"));
		//		}
		//		if (_tcscmp(SE_DENY_NETWORK_LOGON_NAME, sUserRight) == 0)
		//		{
		//			ListBox_AddItemData(hE, TEXT("Отказать в доступе к этому компьютеру"));
		//		}
		//		if (_tcscmp(SE_NETWORK_LOGON_NAME, sUserRight) == 0)
		//		{
		//			ListBox_AddItemData(hE, TEXT("Доступ к компьютеру из сети"));
		//		}
		//		if (_tcscmp(SE_DENY_INTERACTIVE_LOGON_NAME, sUserRight) == 0)
		//		{
		//			ListBox_AddItemData(hE, TEXT("Запретить локальный вход"));
		//		}
		//		if (_tcscmp(SE_INTERACTIVE_LOGON_NAME, sUserRight) == 0)
		//		{
		//			ListBox_AddItemData(hE, TEXT("Локальный вход в систему"));
		//		}
		//	}
			
			}		
		}
		//else { ListBox_AddItemData(hE, TEXT("Привелегии не назначены!")); ListBox_AddItemData(hE, "");
		//}
		LsaFreeMemory(UserRights);
		
	}
	LsaClose(lsahPolicy);
}
BOOL GetTokenGroups(HANDLE hToken, PTOKEN_GROUPS *pTokenGroups)
{
	DWORD cb;

	// определеям размер блока памяти (в байтах)
	GetTokenInformation(hToken, TokenGroups, NULL, 0, &cb);

	// выделяем блок памяти
	PTOKEN_GROUPS Groups = (PTOKEN_GROUPS)LocalAlloc(LPTR, cb);
	if (NULL == Groups) return FALSE;

	// получаем список привилегий в маркере доступа
	BOOL bRet = GetTokenInformation(hToken, TokenGroups, Groups, cb, &cb);

	if (FALSE != bRet)
	{
		*pTokenGroups = Groups; // возвращаем результат
	} // if
	else
	{
		LocalFree(Groups); // освобождаем выделенную память
	} // else

	return bRet;
} // GetTokenGroups
HANDLE OpenProcessTokenByProcessId(DWORD dwProcessId, DWORD dwDesiredAccess)
{
	HANDLE hToken = NULL;

	// получаем дескриптор процесса
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);

	if (NULL != hProcess)
	{
		// открываем маркер доступа процесса
		OpenProcessToken(hProcess, dwDesiredAccess, &hToken);
		// закрываем дескриптор процесса
		CloseHandle(hProcess);
	} // if

	return hToken;
} // OpenProcessTokenByProcessId