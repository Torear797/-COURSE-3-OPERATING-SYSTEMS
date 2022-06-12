#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h> 
#include <random>  
#include <process.h>
#include "resourcer.h"
#include "psapi.h"
#include "strsafe.h"
#include <strsafe.h>
#include <shlwapi.h>
#include <shlobj.h>
#pragma comment(lib, "shlwapi.lib")
#define IDR_MENU1  101
#define IDC_EDIT_TEXT  2001
#define ID_OpenFile  2003
#define ID_OpenFolder 2021
#define ID_Rename  2004
#define ID_change  2005
#define ID_Exit  2006
#define ID_Size  2007

#define IDC_EDIT1 2012
#define IDD_DIALOG1 2011
#define ID_TimeCreate  2008
#define ID_TimeEdit 2009
#define ID_TimeOpen  2010
#define IDC_EDITDIALOG 2014
#define IDC_OpenFolder 2015
#define IDC_EDITDIALOG2 2016

#define IDC_ATTRIBUTE_READONLY 3000
#define IDC_ATTRIBUTE_HIDDEN 3001
#define IDC_ATTRIBUTE_ARCHIVE 3002
#define IDC_ATTRIBUTE_SYSTEM 3003
#define IDC_ATTRIBUTE_TEMPORARY 3004
#define IDC_ATTRIBUTE_COMPRESSED 3005
#define IDC_ATTRIBUTE_ENCRYPTED 3006
#define	IDC_ATTRIBUTE_CONTENT_INDEXED 3007

MSG msg;
HWND hwnd = NULL;
HWND hbtn;
HWND hbtn2;
BOOL bRet;
HWND hlabel1;
HWND hlabel2;
HWND hlabel3;
HWND hlabel4;
HWND hlabel5;
HWND hlabel6;
HWND hlabel7;
HWND hlabel8;
HWND hlabel9;
HWND hlabel10;
HWND hlabel11;
HWND hlabel12; HWND hlabel13; HWND hlabelPath;
HWND hDlg = NULL;
HWND hFindDlg = NULL;
HACCEL hAccel = NULL;
HFONT hFont;
typedef BOOL(__stdcall *LPSEARCHFUNC)(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);
TCHAR szPath[MAX_PATH] = TEXT("");
DWORD cchPath = 0;
LPCTSTR lpszFileName = NULL; 
RECT rect = { 0 }; 
static int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);


HANDLE hFile = INVALID_HANDLE_VALUE; OVERLAPPED _oRead = { 0 }, _oWrite = { 0 };
LPSTR lpszBufferText = NULL; 

void Dialog_OnClose(HWND hwnd);
void Dialog_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void Dialog_OnCommand2(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
BOOL GetFileTimeFormat(const LPFILETIME lpFileTime, LPTSTR lpszFileTime, DWORD cchFileTime);
void StringCchPrintFileSize(LPTSTR lpszBuffer, DWORD cch, ULARGE_INTEGER size);
BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR lpszDirName, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam);
BOOL FileMove(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);
BOOL __stdcall CalculateSize(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);
LSTATUS RegGetValueBinary(HKEY hKey, LPCTSTR lpValueName, LPBYTE lpData, DWORD cb, LPDWORD lpcbNeeded);
LSTATUS RegGetValueDWORD(HKEY hKey, LPCTSTR lpValueName, LPDWORD lpdwData);
LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPTSTR lpszData, DWORD cch, LPDWORD lpcchNeeded);
LSTATUS RegSetValueDWORD(HKEY hKey, LPCTSTR lpValueName, DWORD dwData);
LSTATUS RegSetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPCTSTR lpszData);
LSTATUS RegSetValueBinary(HKEY hKey, LPCTSTR lpValueName, LPCBYTE lpData, DWORD cb);
void OnClose(HWND hwnd); void OnDestroy(HWND hwnd);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL PreTranslateMessage(LPMSG lpMsg);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#define WM_RENAME  WM_USER + 1

void WriteInformation(LPARAM lParam)
{
	DWORD dwAttr = GetFileAttributes(szPath);
	if (INVALID_FILE_ATTRIBUTES != dwAttr && (dwAttr & FILE_ATTRIBUTE_HIDDEN))
	{
		CheckDlgButton(hwnd, IDC_ATTRIBUTE_HIDDEN, BST_CHECKED);
	}
	else CheckDlgButton(hwnd, IDC_ATTRIBUTE_HIDDEN, BST_UNCHECKED);

	if (INVALID_FILE_ATTRIBUTES != dwAttr && (dwAttr & FILE_ATTRIBUTE_ARCHIVE))
	{
		CheckDlgButton(hwnd, IDC_ATTRIBUTE_ARCHIVE, BST_CHECKED);
	}
	else CheckDlgButton(hwnd, IDC_ATTRIBUTE_ARCHIVE, BST_UNCHECKED);

	if (INVALID_FILE_ATTRIBUTES != dwAttr && (dwAttr & FILE_ATTRIBUTE_COMPRESSED))
	{
		CheckDlgButton(hwnd, IDC_ATTRIBUTE_COMPRESSED, BST_CHECKED);
	}
	else CheckDlgButton(hwnd, IDC_ATTRIBUTE_COMPRESSED, BST_UNCHECKED);

	if (INVALID_FILE_ATTRIBUTES != dwAttr && (dwAttr & FILE_ATTRIBUTE_ENCRYPTED))
	{
		CheckDlgButton(hwnd, IDC_ATTRIBUTE_ENCRYPTED, BST_CHECKED);
	}
	else CheckDlgButton(hwnd, IDC_ATTRIBUTE_ENCRYPTED, BST_UNCHECKED);

	if (INVALID_FILE_ATTRIBUTES != dwAttr && (dwAttr & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED))
	{
		CheckDlgButton(hwnd, IDC_ATTRIBUTE_CONTENT_INDEXED, BST_UNCHECKED);
	}
	else CheckDlgButton(hwnd, IDC_ATTRIBUTE_CONTENT_INDEXED, BST_CHECKED);

	if (INVALID_FILE_ATTRIBUTES != dwAttr && (dwAttr & FILE_ATTRIBUTE_READONLY))
	{
		CheckDlgButton(hwnd, IDC_ATTRIBUTE_READONLY, BST_CHECKED);
	}
	else CheckDlgButton(hwnd, IDC_ATTRIBUTE_READONLY, BST_UNCHECKED);

	if (INVALID_FILE_ATTRIBUTES != dwAttr && (dwAttr & FILE_ATTRIBUTE_SYSTEM))
	{
		CheckDlgButton(hwnd, IDC_ATTRIBUTE_SYSTEM, BST_CHECKED);
	}
	else CheckDlgButton(hwnd, IDC_ATTRIBUTE_SYSTEM, BST_UNCHECKED);

	if (INVALID_FILE_ATTRIBUTES != dwAttr && (dwAttr & FILE_ATTRIBUTE_TEMPORARY))
	{
		CheckDlgButton(hwnd, IDC_ATTRIBUTE_TEMPORARY, BST_CHECKED);
	}
	else CheckDlgButton(hwnd, IDC_ATTRIBUTE_TEMPORARY, BST_UNCHECKED);
	SetWindowText(hlabelPath, szPath);
	LPWIN32_FILE_ATTRIBUTE_DATA lpfa = (LPWIN32_FILE_ATTRIBUTE_DATA)lParam;
	TCHAR szBuffer[100];
	ULARGE_INTEGER size = { 0 };
	CalculateSize(szPath, lpfa, &size);
	lpszFileName = PathFindFileName(szPath);
	SetWindowText(hlabel1, lpszFileName);
	StringCchPrintFileSize(szBuffer, _countof(szBuffer), size);
	SetDlgItemText(hwnd, ID_Size, szBuffer); // Размер
	GetFileTimeFormat(&lpfa->ftCreationTime, szBuffer, _countof(szBuffer));
	SetDlgItemText(hwnd, ID_TimeCreate, szBuffer); // Время создания
	GetFileTimeFormat(&lpfa->ftLastWriteTime, szBuffer, _countof(szBuffer));
	SetDlgItemText(hwnd, ID_TimeEdit, szBuffer); // Время последнего изменения
	GetFileTimeFormat(&lpfa->ftLastAccessTime, szBuffer, _countof(szBuffer));
	SetDlgItemText(hwnd, ID_TimeOpen, szBuffer); // Время последнего обращения
}
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	HKEY hKey = NULL;
	DWORD dwDisposition;
	RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\IT-3(10)"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKey, &dwDisposition);
	if ((NULL != lpszCmdLine) && (_T('\0') != lpszCmdLine[0]))
	{
		StringCchCopy(szPath, _countof(szPath), lpszCmdLine);
		PathUnquoteSpaces(szPath);
	} 
	else RegGetValueSZ(hKey, TEXT("szPath"), szPath, _countof(szPath), NULL);
	WIN32_FILE_ATTRIBUTE_DATA fa;
	BOOL bRet = GetFileAttributesEx(szPath, GetFileExInfoStandard, &fa);
	if (FALSE != bRet)
	{
		RegGetValueBinary(hKey, TEXT("rect"), (LPBYTE)&rect, sizeof(rect), NULL);
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
		wcex.lpszClassName = TEXT("Windowsclass");;
		wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		if (0 == RegisterClassEx(&wcex))
		{
			MessageBox(NULL, TEXT("Не удалось зарегестрировать класс!"), TEXT("Ошибка"), MB_ICONEXCLAMATION | MB_OK);
			return -1;
		}
		LoadLibrary(TEXT("ComCtl32.dll"));
		hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Windowsclass"), TEXT("Программа № 2"), WS_OVERLAPPEDWINDOW,
			rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
		if (NULL == hwnd)
		{
			MessageBox(NULL, TEXT("Не удалось создать окно!"), TEXT("Ошибка"), MB_ICONEXCLAMATION | MB_OK);
			return -1;
		}
		if (fa.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			 ShowWindow(hlabel6, SW_HIDE);
		}
		else
		{
			ShowWindow(hlabel6, SW_NORMAL);
		} 
		ShowWindow(hwnd, nCmdShow);
		WriteInformation((LPARAM)&fa);
	}
	for (;;)
	{
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			//OnIdle(hwnd);
		}
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
	RegSetValueSZ(hKey, TEXT("szPath"), szPath);
	RegSetValueBinary(hKey, TEXT("rect"), (LPCBYTE)&rect, sizeof(rect));
	RegCloseKey(hKey);
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
LOGFONT lf;
CreateWindowEx(0, L"Static", L"Имя:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 10, 100, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Static", L"Расположение:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 40, 100, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Static", L"Размер:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 70, 100, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Static", L"Создан:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 100, 100, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Static", L"Изменен:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 130,100, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Static", L"Открыт:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 160,100, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
hlabel1 = CreateWindowEx(0, L"Static", L"0", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 150, 10, 200, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
hlabelPath = CreateWindowEx(0, L"Static", L"0", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 150, 40, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
hlabel2 = CreateWindowEx(0, L"Static", L"0", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 150, 70, 200, 20, hwnd, (HMENU)ID_Size, lpCreateStruct->hInstance, NULL);
hlabel3 = CreateWindowEx(0, L"Static", L"0", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 150, 100, 200, 20, hwnd, (HMENU)ID_TimeCreate, lpCreateStruct->hInstance, NULL);
hlabel4 = CreateWindowEx(0, L"Static", L"0", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 150, 130, 200, 20, hwnd, (HMENU)ID_TimeEdit, lpCreateStruct->hInstance, NULL);
hlabel5 = CreateWindowEx(0, L"Static", L"0", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 150, 160, 200, 20, hwnd, (HMENU)ID_TimeOpen, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Static", L"______________________________________________", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 180, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Static", L"Атрибуты:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 210, 80, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
hlabel6 = CreateWindowEx(0, L"Button", L"Только чтение", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 220, 210, 120, 25, hwnd, (HMENU)IDC_ATTRIBUTE_READONLY, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Button", L"Скрытый", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 100, 210, 100, 25, hwnd, (HMENU)IDC_ATTRIBUTE_HIDDEN, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Button", L"Готово для архивирования", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 100, 240, 200, 25, hwnd, (HMENU)IDC_ATTRIBUTE_ARCHIVE, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Button", L"Разрешить индесировать", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 100, 270, 200, 25, hwnd, (HMENU)IDC_ATTRIBUTE_CONTENT_INDEXED, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Button", L"Системный", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_DISABLED, 100, 300, 120, 25, hwnd, (HMENU)IDC_ATTRIBUTE_SYSTEM, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Button", L"Временный", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_DISABLED, 220, 300, 120, 25, hwnd, (HMENU)IDC_ATTRIBUTE_TEMPORARY, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Button", L"Сжимать содержимое", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_DISABLED, 100, 330, 200, 25, hwnd, (HMENU)IDC_ATTRIBUTE_COMPRESSED, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Button", L"Шифровать содержимое", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_DISABLED, 100, 360, 200, 25, hwnd, (HMENU)IDC_ATTRIBUTE_ENCRYPTED, lpCreateStruct->hInstance, NULL);
CreateWindowEx(0, L"Button", L"Сохранить", WS_CHILD | WS_VISIBLE, 100, 360, 220, 25, hwnd, (HMENU)ID_change, lpCreateStruct->hInstance, NULL);
return true;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hwnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	static HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_TEXT);
	HINSTANCE hInstance = GetWindowInstance(hwnd);
	switch (id)
	{
	case ID_OpenFile:
	{
		OPENFILENAME ofn = { sizeof(OPENFILENAME) };
		ofn.hInstance = GetWindowInstance(hwnd);
		ofn.lpstrFilter = TEXT("Файлы (*.*)\0*.*\0");
		ofn.lpstrFile = szPath;
		ofn.nMaxFile = _countof(szPath);
		ofn.lpstrTitle = TEXT("Открыть");
		ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST;
		ofn.lpstrDefExt = TEXT("Все файлы");

		if (GetOpenFileName(&ofn) != FALSE)
		{
				WIN32_FILE_ATTRIBUTE_DATA fa;
				BOOL bRet = GetFileAttributesEx(szPath, GetFileExInfoStandard, &fa);

				if (FALSE != bRet)
				{	
					ShowWindow(hlabel6, SW_NORMAL);	
					WriteInformation((LPARAM)&fa);
				}
		}
	}
	break;
	case ID_change:
	{
		DWORD dwFileAttributes = 0;
		if (IsDlgButtonChecked(hwnd, IDC_ATTRIBUTE_READONLY) == BST_CHECKED) dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
		if (IsDlgButtonChecked(hwnd, IDC_ATTRIBUTE_HIDDEN) == BST_CHECKED) dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
		if (IsDlgButtonChecked(hwnd, IDC_ATTRIBUTE_ARCHIVE) == BST_CHECKED) dwFileAttributes |= FILE_ATTRIBUTE_ARCHIVE;
		if (IsDlgButtonChecked(hwnd, IDC_ATTRIBUTE_CONTENT_INDEXED) == BST_UNCHECKED) dwFileAttributes |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
		SetFileAttributes(szPath, dwFileAttributes);
		MessageBox(hwnd, TEXT("Атрибуты успешно изменены!"), TEXT("Работа с файлом"), MB_OK);
	}
	break;
	case ID_OpenFolder:
	{
		DialogBox(hInstance, MAKEINTRESOURCE(IDC_OpenFolder), hwnd, DialogProc2);
	}
	break;
	case ID_Rename:
	{
	  
	   DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc);
	}
		break;
	case ID_Exit:
		OnClose(hwnd);
		break;
	}
}
void OnClose(HWND hwnd)
{
	GetWindowRect(hwnd, &rect);
	DestroyWindow(hwnd); 
} 
void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0); 
}
LSTATUS RegGetValueDWORD(HKEY hKey, LPCTSTR lpValueName, LPDWORD lpdwData)
{
	DWORD dwType;
	LSTATUS lStatus = RegQueryValueEx(hKey, lpValueName, NULL, &dwType, NULL, NULL);
	if (ERROR_SUCCESS == lStatus && REG_DWORD == dwType)
	{
		DWORD cb = sizeof(DWORD);
		lStatus = RegQueryValueEx(hKey, lpValueName, NULL, NULL, (LPBYTE)lpdwData, &cb);
	}
	else if (ERROR_SUCCESS == lStatus)
	{
		lStatus = ERROR_UNSUPPORTED_TYPE;
	} 
	return lStatus;
}
LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPTSTR lpszData, DWORD cch, LPDWORD lpcchNeeded)
{
	DWORD dwType;
	LSTATUS lStatus = RegQueryValueEx(hKey, lpValueName, NULL, &dwType, NULL, NULL);
	if (ERROR_SUCCESS == lStatus && REG_SZ == dwType)
	{
		DWORD cb = cch * sizeof(TCHAR);
		lStatus = RegQueryValueEx(hKey, lpValueName, NULL, NULL, (LPBYTE)lpszData, &cb);
		if (NULL != lpcchNeeded)
			*lpcchNeeded = cb / sizeof(TCHAR);
	}
	else if (ERROR_SUCCESS == lStatus)
    lStatus = ERROR_UNSUPPORTED_TYPE; 	
	return lStatus;
}
LSTATUS RegSetValueDWORD(HKEY hKey, LPCTSTR lpValueName, DWORD dwData)
{
return RegSetValueEx(hKey, lpValueName, 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(DWORD));
}
LSTATUS RegSetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPCTSTR lpszData)
{
	DWORD cb = (_tcslen(lpszData) + 1) * sizeof(TCHAR);
	return RegSetValueEx(hKey, lpValueName, 0, REG_SZ, (LPCBYTE)lpszData, cb);
}
LSTATUS RegSetValueBinary(HKEY hKey, LPCTSTR lpValueName, LPCBYTE lpData, DWORD cb)
{
	return RegSetValueEx(hKey, lpValueName, 0, REG_BINARY, (LPCBYTE)lpData, cb);
}
LSTATUS RegGetValueBinary(HKEY hKey, LPCTSTR lpValueName, LPBYTE lpData, DWORD cb, LPDWORD lpcbNeeded)
{
	DWORD dwType;
	LSTATUS lStatus = RegQueryValueEx(hKey, lpValueName, NULL, &dwType, NULL, NULL);
	if (ERROR_SUCCESS == lStatus && REG_BINARY == dwType)
	{
		lStatus = RegQueryValueEx(hKey, lpValueName, NULL, NULL, lpData, &cb);
		if (NULL != lpcbNeeded) *lpcbNeeded = cb;
	}
	else if (ERROR_SUCCESS == lStatus)
	{
		lStatus = ERROR_UNSUPPORTED_TYPE;
	}
	return lStatus;
}
BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR lpszDirName, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam)
{
	WIN32_FIND_DATA fd;
	TCHAR szFileName[MAX_PATH];
	StringCchPrintf(szFileName, MAX_PATH, TEXT("%s\\%s"), lpszDirName, lpszFileName);
	HANDLE hFindFile = FindFirstFile(szFileName, &fd);
	if (INVALID_HANDLE_VALUE == hFindFile) return FALSE;
	BOOL bRet = TRUE;
	for (BOOL bFindNext = TRUE; FALSE != bFindNext; bFindNext = FindNextFile(hFindFile, &fd))
	{
if (_tcscmp(fd.cFileName, TEXT(".")) == 0 || _tcscmp(fd.cFileName, TEXT("..")) == 0)continue;
		StringCchPrintf(szFileName, MAX_PATH, TEXT("%s\\%s"), lpszDirName, fd.cFileName);
		bRet = lpSearchFunc(szFileName, (LPWIN32_FILE_ATTRIBUTE_DATA)&fd, lpvParam);
		if (FALSE == bRet) break;
	}
	FindClose(hFindFile);
	return bRet;
}
BOOL __stdcall CalculateSize(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam)
{
	if (lpFileAttributeData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return FileSearch(TEXT("*"), lpszFileName, CalculateSize, lpvParam);
	}
	ULARGE_INTEGER size = { lpFileAttributeData->nFileSizeLow, lpFileAttributeData->nFileSizeHigh };
	((ULARGE_INTEGER *)lpvParam)->QuadPart += size.QuadPart;
	return TRUE;
}
void StringCchPrintFileSize(LPTSTR lpszBuffer, DWORD cch, ULARGE_INTEGER size)
{
	if (size.QuadPart >= 0x40000000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ГБ"), (size.QuadPart / (float)0x40000000ULL));
	}
	else if (size.QuadPart >= 0x100000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f МБ"), (size.QuadPart / (float)0x100000ULL));
	}
	else if (size.QuadPart >= 0x0400ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f КБ"), (size.QuadPart / (float)0x0400ULL));
	} 
	else
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%u байт"), size.LowPart);
	} 
	size_t len = _tcslen(lpszBuffer);
	if (len < cch)
	{
		StringCchPrintf((lpszBuffer + len), (cch - len), TEXT(" (%llu байт)"), size.QuadPart);
	}
}
BOOL GetFileTimeFormat(const LPFILETIME lpFileTime, LPTSTR lpszFileTime, DWORD cchFileTime)
{
	SYSTEMTIME st;
	BOOL bRet = FileTimeToSystemTime(lpFileTime, &st);
	if (FALSE != bRet)
		bRet = SystemTimeToTzSpecificLocalTime(NULL, &st, &st);

	if (FALSE != bRet)
	{
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, lpszFileTime, cchFileTime);
		StringCchCat(lpszFileTime, cchFileTime, TEXT(", "));
		DWORD len = _tcslen(lpszFileTime);
		if (len < cchFileTime)
			GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &st, NULL, lpszFileTime + len, cchFileTime - len);
	}
	return bRet;
}
static int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED) {
		LPCTSTR path = reinterpret_cast<LPCTSTR>(lpData);
		::SendMessage(hwnd, BFFM_SETSELECTION, true, (LPARAM)path);
	}
	return 0;
}
void Dialog_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
	{
		TCHAR szNewFileName[MAX_PATH]; 
		TCHAR szCurrentPath[MAX_PATH];							  
		GetDlgItemText(hwnd, IDC_EDITDIALOG, szNewFileName, _countof(szNewFileName));						
				PathAppend(szCurrentPath, szPath);
				PathAppend(szCurrentPath, szNewFileName);
				MoveFile(szPath, szNewFileName);
				StringCchCopy(szPath, _countof(szPath), TEXT(""));
				StringCchCopy(szPath, _countof(szPath), szCurrentPath);	
				lpszFileName = PathFindFileName(szPath);
				SetWindowText(hlabel1, lpszFileName);
				SetWindowText(hlabelPath, szPath);	
				MessageBox(NULL, TEXT("Файл/Каталог успешно переименован!"), TEXT("Сообщение"), MB_OK | MB_OK);
				EndDialog(hwnd, IDCANCEL);	
	}
	break;
	case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
		break;
	}
}
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hwndDlg, IDC_EDITDIALOG, szPath);
		return TRUE;
	case WM_CLOSE:
	HANDLE_WM_CLOSE(hwndDlg, wParam, lParam, Dialog_OnClose); 
	return TRUE; 

	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwndDlg, wParam, lParam, Dialog_OnCommand);
		return TRUE;
	}
	return FALSE;
}
INT_PTR CALLBACK DialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		HANDLE_WM_CLOSE(hwndDlg, wParam, lParam, Dialog_OnClose);
		return TRUE;

	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwndDlg, wParam, lParam, Dialog_OnCommand2);
		return TRUE;
	}
	return FALSE;
}
void Dialog_OnCommand2(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
	{
		TCHAR szNewFileName[MAX_PATH];
		GetDlgItemText(hwnd, IDC_EDITDIALOG2, szNewFileName, _countof(szNewFileName));
			StringCchCopy(szPath, _countof(szPath), TEXT(""));
			StringCchCopy(szPath, _countof(szPath), szNewFileName);
			WIN32_FILE_ATTRIBUTE_DATA fa;
			BOOL bRet = GetFileAttributesEx(szPath, GetFileExInfoStandard, &fa);
			if (FALSE != bRet) { ShowWindow(hlabel6, SW_HIDE); WriteInformation((LPARAM)&fa); }
			EndDialog(hwnd, IDCANCEL);	
	}
	break;
	case IDCANCEL:
		EndDialog(hwnd, IDCANCEL);
		break;
	}
}
void Dialog_OnClose(HWND hwnd)
{
	if (hwnd == hDlg) DestroyWindow(hwnd);
	else EndDialog(hwnd, IDCLOSE);
}