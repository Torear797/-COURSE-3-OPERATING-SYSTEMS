#define _CRT_SECURE_NO_WARNINGS
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
#include <strsafe.h>
#include <Psapi.h>
#include <sddl.h>
#include <LMCons.h>
#include <NTSecAPI.h>
#pragma comment(lib, "Psapi.lib")
MSG msg;
HWND hwnd = NULL;
BOOL bRet; HWND listV;
HWND hDlg = NULL;
HWND hFindDlg = NULL;
HACCEL hAccel = NULL;
#define IDR_MENU1  101
#define IDR_ACCELERATOR1  2020
#define ID_Update  2001
#define IDC_LIST  2002
#define IDC_LIST2  2003
#define IDC_LIST3  2004
#define IDC_LIST4  2005
#define IDC_LIST5  2006
#define ID_Enable 2007
#define ID_Disable  2008

#define IDC_CB1 2009
#define IDC_CB2   2010
#define IDC_CB3   2011
#define IDC_CB4   2012
#define IDC_CB5   2013
#define ID_LV  2016
#define IDC_BUTTON_PRIVILEGE_ENABLE     2014
#define IDC_BUTTON_PRIVILEGE_DISABLE    2015


HWND list; HWND list2; HWND list3; HWND list4; HWND list5;
HWND box1; HWND box2; HWND box3; HWND box4; HWND box5;
HWND test1; HWND test2;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL PreTranslateMessage(LPMSG lpMsg);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void LoadProcessesToListBox(HWND hwndCtl);
BOOL GetAccountName(PSID pSid, LPTSTR *lpName);
BOOL GetTokenGroups(HANDLE hToken, PTOKEN_GROUPS * pTokenGroups);
BOOL GetTokenPrivileges(HANDLE hToken, PTOKEN_PRIVILEGES * pTokenPrivileges);
BOOL IsPrivilegeExists(HANDLE hToken, LPCTSTR lpszPrivilegeName);
BOOL IsPrivilegeEnabled(HANDLE hToken, LPCTSTR lpszPrivilegeName);
void WriteExistPriv(HANDLE hToken, LPCTSTR PrivilegeName);
BOOL EnablePrivilege(HANDLE hToken, LPCTSTR lpszPrivilegeName, BOOL bEnable);
HANDLE OpenProcessTokenByProcessId(DWORD dwProcessId, DWORD dwDesiredAccess);

BOOL GetTokenUser(HANDLE hToken, PSID * ppSid);

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

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Windowslass"), TEXT("Приложение № 2"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 1200, 800, NULL, NULL, hInstance, NULL);

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
	/*CreateWindowEx(0, TEXT("Button"), TEXT("Обновить"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 30, 450, 200, 40, hwnd, (HMENU)ID_Update, lpCreateStruct->hInstance, NULL);
 	*/
	CreateWindowEx(0, TEXT("static"), TEXT("Процессы"),
		WS_CHILD | WS_VISIBLE , 80, 450, 200, 40, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("static"), TEXT("Имя и SID"),
		WS_CHILD | WS_VISIBLE, 380, 450, 200, 40, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("static"), TEXT("Группы"),
		WS_CHILD | WS_VISIBLE, 680, 450, 200, 40, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("static"), TEXT("Привелегии"),
		WS_CHILD | WS_VISIBLE, 980, 450, 200, 40, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	

	list = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 10, 10, 250, 420, hwnd, (HMENU)IDC_LIST, lpCreateStruct->hInstance, NULL);
	list2 = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 300, 10, 250, 420, hwnd, (HMENU)IDC_LIST2, lpCreateStruct->hInstance, NULL);
	list3 = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 600, 10, 250, 420, hwnd, (HMENU)IDC_LIST3, lpCreateStruct->hInstance, NULL);
	list4 = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD  | WS_BORDER | LBS_STANDARD, 900, 10, 250, 420, hwnd, (HMENU)IDC_LIST4, lpCreateStruct->hInstance, NULL);
	//list5 = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 10, 500, 550, 200, hwnd, (HMENU)IDC_LIST5, lpCreateStruct->hInstance, NULL);

box1 =	CreateWindowEx(0, L"Button", L"SeEnableDelegationPrivilege ", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, 500, 250, 25, hwnd, (HMENU)IDC_CB1, lpCreateStruct->hInstance, NULL);
box2 = CreateWindowEx(0, L"Button", L"SeImpersonatePrivilege", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX , 10, 530, 250, 25, hwnd, (HMENU)IDC_CB2, lpCreateStruct->hInstance, NULL);
box3 = CreateWindowEx(0, L"Button", L"SeIncreaseBasePriorityPrivilege", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX , 10, 560, 250, 25, hwnd, (HMENU)IDC_CB3, lpCreateStruct->hInstance, NULL);
box4 = CreateWindowEx(0, L"Button", L"SeIncreaseQuotaPrivilege", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX , 10, 590, 250, 25, hwnd, (HMENU)IDC_CB4, lpCreateStruct->hInstance, NULL);
box5 = CreateWindowEx(0, L"Button", L"SeIncreaseWorkingSetPrivilege", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 10, 620, 250, 25, hwnd, (HMENU)IDC_CB5, lpCreateStruct->hInstance, NULL);

	//CreateWindowEx(0, L"Button", L"Сохранить", WS_CHILD | WS_VISIBLE, 10, 650, 220, 25, hwnd, (HMENU)ID_Enable, lpCreateStruct->hInstance, NULL);

	test1 = CreateWindowEx(0, TEXT("Button"), TEXT("Включить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		900, 500, 250, 30, hwnd, (HMENU)ID_Enable, lpCreateStruct->hInstance, NULL);

	test2 = CreateWindowEx(0, TEXT("Button"), TEXT("Выключить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON ,// | WS_DISABLED
		900, 550, 250, 30, hwnd, (HMENU)ID_Disable, lpCreateStruct->hInstance, NULL);

	listV = CreateWindowEx(0, WC_LISTVIEW, L"", WS_VISIBLE | WS_CHILD |
		WS_BORDER | LVS_REPORT | LVS_EDITLABELS, 900, 10, 250, 420, hwnd, (HMENU)ID_LV, lpCreateStruct->hInstance, NULL);

	LVCOLUMN lvc;
	wchar_t *str = new wchar_t[100];
	wcscpy(str, L"Название");
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = str;
	lvc.cx = 125;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(listV, 0, &lvc);
	wcscpy(str, L"Состояние");
	lvc.iSubItem = 1;
	ListView_InsertColumn(listV, 1, &lvc);
	
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
	static HWND hE = GetDlgItem(hwnd, IDC_LIST2);
	static HWND ProcessList = GetDlgItem(hwnd, IDC_LIST);
	static HWND GroupList = GetDlgItem(hwnd, IDC_LIST3);
	static HWND PrivelegList = GetDlgItem(hwnd, IDC_LIST4);

	switch (id)
	{

	case ID_Update:
	{
		LoadProcessesToListBox(list);
	}
	break;
	case ID_Disable: 
	{
		if (IsWindowEnabled(test2))
		{
			int iItem = ListBox_GetCurSel(ProcessList);
			if (iItem != -1)
			{
				DWORD dwProcessId = (DWORD)ListBox_GetItemData(ProcessList, iItem);
				//int iItem2 = ListBox_GetCurSel(PrivelegList);
				int iItem2 = ListView_GetNextItem(listV, -1, LVNI_SELECTED);
				if (iItem2 != -1)
				{
					HANDLE hToken = OpenProcessTokenByProcessId(dwProcessId, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES);
					if (NULL != hToken)
					{
						TCHAR NameofPrivilege[256];
						//ListBox_GetText(PrivelegList, iItem2, NameofPrivilege);
						ListView_GetItemText(listV, iItem2, 0, NameofPrivilege, _countof(NameofPrivilege));
						/*LPTSTR p = _tcschr(NameofPrivilege, TEXT(' '));
						if (NULL != p) *p = TEXT('\0');*/

						BOOL bRet = EnablePrivilege(hToken, NameofPrivilege, FALSE);
						if (FALSE != bRet)
						{
							ListBox_ResetContent(hE);
							ListBox_ResetContent(GroupList);
							ListBox_ResetContent(PrivelegList);
							ListView_DeleteAllItems(listV);
							LoadProcessesToListBox(list);
						}
					}
				}
			}
		}
	}
	break;
	case ID_Enable:
	{
		if (IsWindowEnabled(test1))
		{
			int iItem = ListBox_GetCurSel(ProcessList);
			if (iItem != -1)
			{
				DWORD dwProcessId = (DWORD)ListBox_GetItemData(ProcessList, iItem);
				//int iItem2 = ListBox_GetCurSel(PrivelegList);
				int iItem2 = ListView_GetNextItem(listV, -1, LVNI_SELECTED);
				if (iItem2 != -1)
				{
					HANDLE hToken = OpenProcessTokenByProcessId(dwProcessId, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES);
					if (NULL != hToken)
					{
						TCHAR NameofPrivilege[256];
					//	ListBox_GetText(PrivelegList, iItem2, NameofPrivilege);
						ListView_GetItemText(listV, iItem2, 0, NameofPrivilege, _countof(NameofPrivilege));
						/*LPTSTR p = _tcschr(NameofPrivilege, TEXT(' '));
						if (NULL != p) *p = TEXT('\0');*/
						BOOL bRet = EnablePrivilege(hToken, NameofPrivilege, TRUE);
						if (FALSE != bRet)
						{
							ListBox_ResetContent(hE);
							ListBox_ResetContent(GroupList);
							ListBox_ResetContent(PrivelegList);
							ListView_DeleteAllItems(listV);
							LoadProcessesToListBox(list);
						}
					}
				}
			}
			
		}
	}
	break;
	case IDC_LIST:
	{
		ListBox_ResetContent(hE);
		ListBox_ResetContent(GroupList);
		ListBox_ResetContent(PrivelegList);
		ListView_DeleteAllItems(listV);
		int iItem = ListBox_GetCurSel(ProcessList);
		if (iItem != -1)
		{
			DWORD dwProcessId = (DWORD)ListBox_GetItemData(ProcessList, iItem);
			HANDLE hToken = OpenProcessTokenByProcessId(dwProcessId, TOKEN_QUERY);
			PSID SIDUser = NULL;
			LPTSTR UserName = NULL, SIDWrite;
			if (NULL != hToken)
			{
				BOOL bRet = GetTokenUser(hToken, &SIDUser);
				if (FALSE != bRet)
				{
					GetAccountName(SIDUser, &UserName);
					ConvertSidToStringSid(SIDUser, &SIDWrite);
					ListBox_AddItemData(hE, SIDWrite);
					ListBox_AddItemData(hE, UserName);
					
				}
				//
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
							ListBox_AddItemData(GroupList, lpString);
							LocalFree(lpString),
								lpString = NULL;
						}
					}
				}
				LocalFree(pTokenGroups);
				//
				PTOKEN_PRIVILEGES pTokenPrivileges = NULL;
				bRet = GetTokenPrivileges(hToken, &pTokenPrivileges);
				if (FALSE != bRet)
				{
					TCHAR szNamePrivileges[256];

					for (DWORD i = 0; i < pTokenPrivileges->PrivilegeCount; ++i)
					{
						LUID Luid = pTokenPrivileges->Privileges[i].Luid;
						DWORD cch = _countof(szNamePrivileges);
						LookupPrivilegeName(NULL, &Luid, szNamePrivileges, &cch);
						DWORD dwAttributes = pTokenPrivileges->Privileges[i].Attributes;


						LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM };
						lvItem.pszText = (LPTSTR)szNamePrivileges;

						int iIteml = ListView_InsertItem(listV, &lvItem);
						


						if (dwAttributes & SE_PRIVILEGE_ENABLED) 
						{
							StringCchCat(szNamePrivileges, _countof(szNamePrivileges), TEXT(" [Активна]"));
							ListView_SetItemText(listV, iIteml, 1, (LPTSTR)TEXT(" [Активна]"));
						} else ListView_SetItemText(listV, iIteml, 1, (LPTSTR)TEXT(" [Не Активна]"));
						int iItem = ListBox_AddItemData(PrivelegList, szNamePrivileges);
						//ListBox_SetItemData(PrivelegList, iItem, dwAttributes);
					}
				}
				LocalFree(pTokenPrivileges);
			//	LPWSTR PrivilegeName = L"SeSystemtimePrivilege";
				bRet = IsPrivilegeExists(hToken, TEXT("SeEnableDelegationPrivilege"));
				if (FALSE != bRet)
				{
					bRet = IsPrivilegeEnabled(hToken, TEXT("SeEnableDelegationPrivilege"));
					if (FALSE != bRet)
					{
						CheckDlgButton(hwnd, IDC_CB1, BST_CHECKED);
						EnableWindow(box1, true);

					}
					else
					{
						CheckDlgButton(hwnd, IDC_CB1, BST_UNCHECKED);
						EnableWindow(box1, true);

					}
				}
				else EnableWindow(box1, false);

				bRet = IsPrivilegeExists(hToken, TEXT("SeImpersonatePrivilege"));
				if (FALSE != bRet)
				{
					bRet = IsPrivilegeEnabled(hToken, TEXT("SeImpersonatePrivilege"));
					if (FALSE != bRet) {
						EnableWindow(box2, true);
						CheckDlgButton(hwnd, IDC_CB2, BST_CHECKED);
					}
					else
					{
						EnableWindow(box2, true);
						CheckDlgButton(hwnd, IDC_CB2, BST_UNCHECKED);

					}
				}
				else EnableWindow(box2, false);

				bRet = IsPrivilegeExists(hToken, TEXT("SeIncreaseBasePriorityPrivilege"));
				if (FALSE != bRet)
				{
					bRet = IsPrivilegeEnabled(hToken, TEXT("SeIncreaseBasePriorityPrivilege"));
					if (FALSE != bRet)
					{
						EnableWindow(box3, true);
						CheckDlgButton(hwnd, IDC_CB3, BST_CHECKED);
					}
					else
					{
						EnableWindow(box3, true);
						CheckDlgButton(hwnd, IDC_CB3, BST_UNCHECKED);
					}
				}
				else EnableWindow(box3, false);

				bRet = IsPrivilegeExists(hToken, TEXT("SeIncreaseQuotaPrivilege"));
				if (FALSE != bRet)
				{
					bRet = IsPrivilegeEnabled(hToken, TEXT("SeIncreaseQuotaPrivilege"));
					if (FALSE != bRet)
					{
						EnableWindow(box4, true);
						CheckDlgButton(hwnd, IDC_CB4, BST_CHECKED);
					}
					else
					{
						EnableWindow(box4, true);
						CheckDlgButton(hwnd, IDC_CB4, BST_UNCHECKED);
					}
				}
				else EnableWindow(box4, false);

				bRet = IsPrivilegeExists(hToken, TEXT("SeIncreaseWorkingSetPrivilege"));
				if (FALSE != bRet)
				{
					bRet = IsPrivilegeEnabled(hToken, TEXT("SeIncreaseWorkingSetPrivilege"));
					if (FALSE != bRet)
					{
						EnableWindow(box5, true);
						CheckDlgButton(hwnd, IDC_CB5, BST_CHECKED);
					}
					else
					{
					    EnableWindow(box5, true);
						CheckDlgButton(hwnd, IDC_CB5, BST_UNCHECKED);
					}
				}
				else EnableWindow(box5, false);
			}
			

			CloseHandle(hToken);


		}		
	}
	break;
	}//switch
}
void LoadProcessesToListBox(HWND hwndCtl)
{
	ListBox_ResetContent(hwndCtl);
	DWORD aProcessIds[1024], cbNeeded = 0;
	BOOL bRet = EnumProcesses(aProcessIds, sizeof(aProcessIds), &cbNeeded);
	if (FALSE != bRet)
	{
		TCHAR szName[MAX_PATH], szBuffer[300];
		for (DWORD i = 0, n = cbNeeded / sizeof(DWORD); i < n; ++i)
		{
			DWORD dwProcessId = aProcessIds[i], cch = 0;
			if (0 == dwProcessId) continue;
			HANDLE hProcess = OpenProcess(PROCESS_VM_READ |
				PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
			if (NULL != hProcess)
			{
				cch = GetModuleBaseName(hProcess, NULL, szName, MAX_PATH);
				CloseHandle(hProcess);
			} 
	if (0 == cch) StringCchCopy(szName, MAX_PATH, TEXT("Неизвестный процесс"));
			StringCchPrintf(szBuffer, _countof(szBuffer),
				TEXT("%s (PID: %u)"), szName, dwProcessId);
			int iItem = ListBox_AddString(hwndCtl, szBuffer);
			ListBox_SetItemData(hwndCtl, iItem, dwProcessId);
		}
	} 
}
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
BOOL GetTokenUser(HANDLE hToken, PSID *ppSid)
{
	DWORD cb;

	// определеям размер блока памяти (в байтах)
	GetTokenInformation(hToken, TokenUser, NULL, 0, &cb);

	// выделяем блок памяти
	PTOKEN_USER pTokenUser = (PTOKEN_USER)LocalAlloc(LPTR, cb);
	if (NULL == pTokenUser) return FALSE;

	// получаем информацию о пользователе в маркере доступа
	BOOL bRet = GetTokenInformation(hToken, TokenUser, pTokenUser, cb, &cb);

	PSID pSid = NULL;

	// копируем SID пользователя
	if (FALSE != bRet)
	{
		DWORD cbSid = GetLengthSid(pTokenUser->User.Sid);
		pSid = (PSID)LocalAlloc(LPTR, cbSid);

		bRet = CopySid(cbSid, pSid, pTokenUser->User.Sid);
	} // if

	  // освобождаем выделенную память
	LocalFree(pTokenUser);

	if (FALSE != bRet)
	{
		*ppSid = pSid; // возвращаем результат
	} // if
	else
	{
		LocalFree(pSid); // освобождаем выделенную память
	} // else

	return bRet;
} // GetTokenUser
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
BOOL GetTokenPrivileges(HANDLE hToken, PTOKEN_PRIVILEGES *pTokenPrivileges)
{
	DWORD cb;

	// определеям размер блока памяти (в байтах)
	GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &cb);

	// выделяем блок памяти
	PTOKEN_PRIVILEGES Privileges = (PTOKEN_PRIVILEGES)LocalAlloc(LPTR, cb);
	if (NULL == Privileges) return FALSE;

	// получаем список привилегий в маркере доступа
	BOOL bRet = GetTokenInformation(hToken, TokenPrivileges, Privileges, cb, &cb);

	if (FALSE != bRet)
	{
		*pTokenPrivileges = Privileges; // возвращаем результат
	} // if
	else
	{
		// освобождаем выделенную память
		LocalFree(Privileges);
	} // else

	return bRet;
} // GetTokenPrivileges
BOOL IsPrivilegeExists(HANDLE hToken, LPCTSTR lpszPrivilegeName)
{
	LUID Luid;
	PTOKEN_PRIVILEGES pTokenPrivileges = NULL;

	// определяем LUID указанной привилегии
	BOOL bRet = LookupPrivilegeValue(NULL, lpszPrivilegeName, &Luid);

	// получаем список привилегий
	if (FALSE != bRet)
		bRet = GetTokenPrivileges(hToken, &pTokenPrivileges);

	if (FALSE == bRet) return -1; // возникла ошибка

	bRet = FALSE;
	for (DWORD i = 0; i < pTokenPrivileges->PrivilegeCount; ++i)
	{
		// сравниваем два LUID
		int res = memcmp(&pTokenPrivileges->Privileges[i].Luid, &Luid, sizeof(LUID));

		if (0 == res)
		{
			bRet = TRUE;
			break; // выходим из цикла
		} // if
	} // for

	  // освобождаем выделенную память
	LocalFree(pTokenPrivileges);

	return bRet;
} // IsPrivilegeExists
BOOL IsPrivilegeEnabled(HANDLE hToken, LPCTSTR lpszPrivilegeName)
{
	BOOL fResult; // результат проверки
	PRIVILEGE_SET PrivSet;

	PrivSet.PrivilegeCount = 1; // количество привилегий
	PrivSet.Control = PRIVILEGE_SET_ALL_NECESSARY;

	// определяем LUID указанной привилегии
	BOOL bRet = LookupPrivilegeValue(NULL, lpszPrivilegeName, &PrivSet.Privilege[0].Luid);

	if (FALSE != bRet)
	{
		// определяем состояние указанной привилегии
		bRet = PrivilegeCheck(hToken, &PrivSet, &fResult);
	} // if

	return (FALSE != bRet) ? fResult : -1;
} // IsPrivilegeEnabled
void WriteExistPriv(HANDLE hToken, LPCTSTR PrivilegeName)
{
	static HWND PrivelegListVar = GetDlgItem(hwnd, IDC_LIST5);
	BOOL priv = IsPrivilegeExists(hToken, PrivilegeName);
	if (FALSE != bRet)
	{
		bRet = IsPrivilegeEnabled(hToken, PrivilegeName);
		if (FALSE != bRet)
		{	
			int iItem = ListBox_AddString(PrivelegListVar, PrivilegeName);
			ListBox_SetItemData(PrivelegListVar, iItem, TEXT("[В наличии][Включена]"));
		}
		else
		{
			int iItem = ListBox_AddString(PrivelegListVar, PrivilegeName);
			ListBox_SetItemData(PrivelegListVar, iItem, TEXT("[В наличии][Выключена]"));		
		}
	}
	else
	{ 
		int iItem = ListBox_AddString(PrivelegListVar, PrivilegeName);
		ListBox_SetItemData(PrivelegListVar, iItem, TEXT("[Отсутствует]"));
	}

}
BOOL EnablePrivilege(HANDLE hToken, LPCTSTR lpszPrivilegeName, BOOL bEnable)
{
	TOKEN_PRIVILEGES Privileges;
	Privileges.PrivilegeCount = 1; // количество привилегий

								   // установим новое состояние указанной привилегии
	Privileges.Privileges[0].Attributes = (FALSE != bEnable) ? SE_PRIVILEGE_ENABLED : 0;

	// определяем LUID указанной привилегии
	BOOL bRet = LookupPrivilegeValue(NULL, lpszPrivilegeName, &Privileges.Privileges[0].Luid);

	if (FALSE != bRet)
	{
		// изменяем состояние указанной привилегии
		bRet = AdjustTokenPrivileges(hToken, FALSE, &Privileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	} // if

	return bRet;
} // EnablePrivilege