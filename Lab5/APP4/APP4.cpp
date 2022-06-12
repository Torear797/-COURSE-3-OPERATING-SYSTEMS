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

#include <iostream>
#include <locale.h>
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <locale.h>
#include <strsafe.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#define IDR_MENU1  101
#define IDR_ACCELERATOR1  2020
#define IDC_EDIT_TEXT  2001
#define IDC_EDIT_TEXT2  2008
#define ID_CreateFile  2002
#define ID_SafeFile  2003
#define ID_OpenFile  2004
#define ID_SafeFileAS  2005
#define ID_Exit  2006

#define ID_Login  2025
#define ID_Password  2026
#define ID_Auth  2027
#define IDC_DialogAuth 2028

#define ID_DlgLogin  2030
#define ID_DlgPassword  2031
MSG msg;
HWND hwnd = NULL;
HWND hbtn;
HWND hbtn2; HWND hLogin; HWND hPassword; HWND label1; HWND label2; HWND label3; HWND label4;
HWND but1; HWND but2; HWND hwndCur; HWND hwndNew;
BOOL bRet;
HWND hDlg = NULL;
HWND hFindDlg = NULL;
HACCEL hAccel = NULL;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL PreTranslateMessage(LPMSG lpMsg);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void LoadSettings(LPCTSTR lpFileName);
void OnClose(HWND hwnd);
void OnDestroy(HWND hwnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void SaveSettings(LPCTSTR lpFileName);
POINT wndPos;
SIZE wndSize;

TCHAR Bufer4[MAX_PATH] = TEXT("");
TCHAR szFileName[MAX_PATH] = TEXT("");
HANDLE hFile = INVALID_HANDLE_VALUE;
HANDLE hGlobalUser = NULL;
bool SaveAs = false;
LOGFONT logFont;
HFONT hFont = NULL;
HANDLE OpenUserToken(LPCTSTR lpszUsername, LPCTSTR lpszDomain, LPCTSTR lpszPassword, DWORD dwLogonType,
	DWORD dwDesiredAccess, PSECURITY_ATTRIBUTES lpTokenAttributes, TOKEN_TYPE TokenType, SECURITY_IMPERSONATION_LEVEL ImpersonationLevel);
typedef BOOL(__stdcall *LPSEARCHFUNC)(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);
LPSTR lpszBufferText = NULL;
OVERLAPPED _oRead = { 0 }, _oWrite = { 0 };
void Dialog_OnClose(HWND hwnd);
void Dialog_OnCommand2(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnIdle(HWND hwnd);
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
	wcex.lpszClassName = TEXT("Windowsclass");;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (0 == RegisterClassEx(&wcex))
	{
		MessageBox(NULL, TEXT("Не удалось зарегестрировать класс!"), TEXT("Ошибка"), MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}
	LoadLibrary(TEXT("ComCtl32.dll"));
	TCHAR szIniFileName[MAX_PATH];
	{
		GetModuleFileName(NULL, szIniFileName, MAX_PATH);
		StringCchCat(szIniFileName, MAX_PATH, TEXT(" MySettings.ini"));
	}
	LoadSettings(szIniFileName);
	//hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Windowsclass"), TEXT("Перемещение"), WS_OVERLAPPEDWINDOW,
		wndPos.x, wndPos.y, wndSize.cx, wndSize.cy, NULL, NULL, hInstance, NULL);
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
			OnIdle(hwnd);
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
	SaveSettings(szIniFileName);
	return (int)msg.wParam;
}
BOOL PreTranslateMessage(LPMSG lpMsg)
{
	BOOL bRet = true;
	if (!TranslateAccelerator(hwnd, hAccel, lpMsg))
	{
		bRet = IsDialogMessage(hDlg, lpMsg);
		if (false == bRet)bRet = IsDialogMessage(hFindDlg, lpMsg);
	}
	return bRet;
}
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_WANTRETURN;
	label1 = CreateWindowEx(0, L"Static", L"Текущий файл / каталог:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 5, 200, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	 hwndCur = CreateWindowEx(0, TEXT("Edit"), TEXT(""), dwStyle, 10, 30, 300, 20, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);
	label2 = CreateWindowEx(0, L"Static", L"Куда переместить:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 75, 200, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	hwndNew = CreateWindowEx(0, TEXT("Edit"), TEXT(""), dwStyle, 10, 100, 300, 20, hwnd, (HMENU)IDC_EDIT_TEXT2, lpCreateStruct->hInstance, NULL);

	but1 = CreateWindowEx(0, L"Button", L"Переместить", WS_CHILD | WS_VISIBLE, 60, 150, 200, 25, hwnd, (HMENU)ID_OpenFile, lpCreateStruct->hInstance, NULL);

	SetWindowText(hwndCur, L"C:\\Users\\Тореар\\Desktop\\1");
	SetWindowText(hwndNew, L"E:\\");

	hLogin = CreateWindowEx(0, L"edit", NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE, 10, 30, 300, 20, hwnd, (HMENU)ID_Login, NULL, NULL);
	hPassword = CreateWindowEx(0, L"edit", NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE, 10, 100, 300, 20, hwnd, (HMENU)ID_Password, NULL, NULL);
	label3 = CreateWindowEx(0, L"Static", L"Login:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 5, 200, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	label4 = CreateWindowEx(0, L"Static", L"Password:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 75, 200, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	but2 = CreateWindowEx(0, L"Button", L"Войти", WS_CHILD | WS_VISIBLE, 60, 150, 200, 25, hwnd, (HMENU)ID_Auth, lpCreateStruct->hInstance, NULL);
	
	//ShowWindow(hwndCur, FALSE);
	//ShowWindow(hwndNew, FALSE);
	//ShowWindow(label1, FALSE);
	//ShowWindow(label2, FALSE);
	//ShowWindow(but1, FALSE);

	ShowWindow(hLogin, FALSE);
	ShowWindow(hPassword, FALSE);
	ShowWindow(label3, FALSE);
	ShowWindow(label4, FALSE);
	ShowWindow(but2, FALSE);
	return true;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hwnd, WM_SIZE, OnSize);
		HANDLE_MSG(hwnd, WM_CLOSE, OnClose);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
BOOL DelDirectory(LPCTSTR indir)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	RemoveDirectory(indir);
	TCHAR maska[MAX_PATH];
	StringCchPrintf(maska, _countof(maska), TEXT("%s\\%s"), (LPCTSTR)indir, TEXT("*"));

	hFind = FindFirstFile(maska, &FindFileData);//Поиск файлов
	if (hFind == NULL) return FALSE;

	do
	{

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)// нашли папку
		{
			if (_tcscmp(FindFileData.cFileName, TEXT(".")) && _tcscmp(FindFileData.cFileName, TEXT("..")))
			{
				TCHAR indir2[MAX_PATH], outdir2[MAX_PATH];
				StringCchPrintf(indir2, _countof(indir2), TEXT("%s/%s/"), indir, FindFileData.cFileName);
				//StringCchPrintf(outdir2, _countof(outdir2), TEXT("%s/%s"), outdir, FindFileData.cFileName);
				//Copy(indir2, outdir2);//рекурсивный вызов функции
				RemoveDirectory(indir2);
			}
			RemoveDirectory(indir);
		}
	} while (FindNextFile(hFind, &FindFileData));//пока есть что копировать
	FindClose(hFind);

	return TRUE;
}
INT_PTR CALLBACK DialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL MoveDirectory(LPCTSTR LPCurrentPath, LPCTSTR szNewPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//CreateDirectory(szNewPath, NULL);
	BOOL bRet = CreateDirectoryEx(LPCurrentPath, szNewPath, NULL);
	if (FALSE != bRet) {
		TCHAR maska[MAX_PATH];
		BOOL bret;
		StringCchPrintf(maska, _countof(maska), TEXT("%s\\%s"), (LPCTSTR)LPCurrentPath, TEXT("*"));
		hFind = FindFirstFile(maska, &FindFileData);
		if (hFind == NULL) return FALSE;
		do
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(FindFileData.cFileName, TEXT(".")) && _tcscmp(FindFileData.cFileName, TEXT("..")))
				{
					TCHAR indir2[MAX_PATH], outdir2[MAX_PATH];
					StringCchPrintf(indir2, _countof(indir2), TEXT("%s/%s/"), LPCurrentPath, FindFileData.cFileName);
					StringCchPrintf(outdir2, _countof(outdir2), TEXT("%s/%s"), szNewPath, FindFileData.cFileName);
					bret = MoveDirectory(indir2, outdir2);
					if (FALSE == bRet) return bRet;
				}
			}
			else
			{
				TCHAR indir2[MAX_PATH], outdir2[MAX_PATH];
				StringCchPrintf(indir2, _countof(indir2), TEXT("%s/%s"), LPCurrentPath, FindFileData.cFileName);
				StringCchPrintf(outdir2, _countof(outdir2), TEXT("%s/%s"), szNewPath, FindFileData.cFileName);
				bret = MoveFileEx(indir2, outdir2, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
				if (FALSE == bRet) return bRet;
			}
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	} else return bRet;
	return TRUE;
}
HANDLE Auth()
{
	TCHAR Login[255];
	TCHAR Password[255];
	Edit_GetText(hLogin, Login, _countof(Login));
	Edit_GetText(hPassword, Password, _countof(Password));

	HANDLE hToken = OpenUserToken(Login, TEXT("."), Password,
		LOGON32_LOGON_INTERACTIVE, TOKEN_QUERY | TOKEN_IMPERSONATE, NULL, TokenImpersonation, SecurityImpersonation);

	if (NULL != hToken)
	{
		ShowWindow(hLogin, FALSE);
		ShowWindow(hPassword, FALSE);
		ShowWindow(label3, FALSE);
		ShowWindow(label4, FALSE);
		ShowWindow(but2, FALSE);

		ShowWindow(hwndCur, TRUE);
		ShowWindow(hwndNew, TRUE);
		ShowWindow(label1, TRUE);
		ShowWindow(label2, TRUE);
		ShowWindow(but1, TRUE);
		return hToken;
		CloseHandle(hToken);
	}
	else
	{
		MessageBox(hwnd, TEXT("Неверные данные. Попробуйте еще раз."), TEXT("Сообщение"), MB_OK);
	}
	return NULL;
}
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HINSTANCE hInstance = GetWindowInstance(hwnd);
	TCHAR CurrentPath[MAX_PATH];
	GetDlgItemText(hwnd, IDC_EDIT_TEXT, CurrentPath, _countof(CurrentPath));
	TCHAR NewPath[MAX_PATH];
	GetDlgItemText(hwnd, IDC_EDIT_TEXT2, NewPath, _countof(NewPath));
	LPCTSTR LPCurrentPath = (LPCTSTR)CurrentPath;
	LPCTSTR LPNewPath = (LPCTSTR)NewPath;

	switch (id)
	{
	case ID_Auth: 
	{
		TCHAR Login[255]; 
		TCHAR Password[255];
		Edit_GetText(hLogin, Login, _countof(Login));
		Edit_GetText(hPassword, Password, _countof(Password));

		HANDLE hToken = OpenUserToken(Login, TEXT("."), Password,
			LOGON32_LOGON_INTERACTIVE, TOKEN_QUERY | TOKEN_IMPERSONATE, NULL, TokenImpersonation, SecurityImpersonation);

		if (NULL != hToken)
		{
			ShowWindow(hLogin, FALSE);
			ShowWindow(hPassword, FALSE);
			ShowWindow(label3, FALSE);
			ShowWindow(label4, FALSE);
			ShowWindow(but2, FALSE);

			ShowWindow(hwndCur, TRUE);
			ShowWindow(hwndNew, TRUE);
			ShowWindow(label1, TRUE);
			ShowWindow(label2, TRUE);
			ShowWindow(but1, TRUE);

			CloseHandle(hToken);
		}
		else 
		{ 
			MessageBox(hwnd, TEXT("Неверные данные. Попробуйте еще раз."), TEXT("Сообщение"), MB_OK); 
		}
	}
    break;
	case ID_OpenFile:
	{
		if (LPNewPath != NULL && LPCurrentPath != NULL)
		{
			DWORD FileAttrib;
			FileAttrib = GetFileAttributes(LPCurrentPath);
			if (FileAttrib != DWORD(-1))
			{
				TCHAR szNewPath[MAX_PATH];
				StringCchPrintf(szNewPath, _countof(szNewPath), TEXT("%s\\%s"), (LPCTSTR)LPNewPath, PathFindFileName(LPCurrentPath));
				FileAttrib = GetFileAttributes(szNewPath);
				if (FileAttrib == DWORD(-1))
				{
					WIN32_FILE_ATTRIBUTE_DATA fad;
					BOOL bRet = GetFileAttributesEx(LPCurrentPath, GetFileExInfoStandard, &fad);
					if (FALSE != bRet)
					{
						WIN32_FIND_DATA wfd;
						HANDLE hFindFile;
					//	for (int i = 0; i < 3; ++i) // максимальное число попыток = 3
						//{
						bRet = MoveDirectory(LPCurrentPath, szNewPath);
						DWORD dwError;
					//	DWORD dwError = (FALSE == bRet) ? GetLastError() : ERROR_SUCCESS;
						if (FALSE == bRet)dwError = GetLastError(); 
						else 
						{
							DelDirectory(LPCurrentPath);
							RemoveDirectory(LPCurrentPath);
							MessageBox(hwnd, TEXT("Файл/Каталог успешно перемещен!"), TEXT("Сообщение"), MB_OK);
							dwError = ERROR_SUCCESS;
						}
						//RevertToSelf();
						if (ERROR_ACCESS_DENIED == dwError)
						{
							MessageBox(hwnd, TEXT("У вас нет доступа!"), TEXT("Сообщение"), MB_OK);
							DialogBox(hInstance, MAKEINTRESOURCE(IDC_DialogAuth), hwnd, DialogProc2);
							/*HANDLE hToken = OpenUserToken(TEXT("Тореар"), TEXT("."), TEXT("Идея"),
								LOGON32_LOGON_INTERACTIVE, TOKEN_QUERY | TOKEN_IMPERSONATE, NULL, TokenImpersonation, SecurityImpersonation);
							*/ 
							/*HANDLE hToken = hGlobalUser;
							BOOL bret = ImpersonateLoggedOnUser(hToken);
							 if (FALSE != bret) {
								 MessageBox(hwnd, TEXT("Что за хрень."), TEXT("Сообщение"), MB_YESNO | MB_ICONQUESTION);
								 MoveDirectory(LPCurrentPath, szNewPath);

							 } else MessageBox(hwnd, TEXT("Не работает замещение"), TEXT("Сообщение"), MB_YESNO | MB_ICONQUESTION);
							CloseHandle(hToken);*/
							
							//else { MessageBox(hwnd, TEXT("Весело"), TEXT("Сообщение"), MB_OK); break; }
						}// else  break;
					//	}
					}
				}
				else {
					int mbResult = MessageBox(hwnd, TEXT("Данный файл уже существует! Заменить файлы в папке назначения?"), TEXT("Сообщение"), MB_YESNO | MB_ICONQUESTION);
					if (mbResult == IDYES)
					{
						WIN32_FILE_ATTRIBUTE_DATA fa;
						bRet = GetFileAttributesEx(LPCurrentPath, GetFileExInfoStandard, &fa);
						if (FALSE != bRet)
						{
							if (MoveDirectory(LPCurrentPath, szNewPath))
							{
								DelDirectory(LPCurrentPath);
								RemoveDirectory(LPCurrentPath);
								MessageBox(hwnd, TEXT("Файл/Каталог успешно перемещен!"), TEXT("Сообщение"), MB_OK);
							}
							else
								MessageBox(hwnd, TEXT("Ошибка!"), TEXT("Сообщение"), MB_OK);
						}
					}
				}
			}
			else MessageBox(hwnd, TEXT("Исходный файл не существует!"), TEXT("Сообщение"), MB_OK);
		}
		else MessageBox(hwnd, TEXT("Не все данные введены!"), TEXT("Сообщение"), MB_OK);
	}
	break;
	}
}
void OnIdle(HWND hwnd)
{

}
void SaveSettings(LPCTSTR lpFileName)
{
	TCHAR szString[10];
	StringCchPrintf(szString, 10, TEXT("%d"), wndPos.x);
	WritePrivateProfileString(TEXT("Window"), TEXT("X"), szString, lpFileName);
	StringCchPrintf(szString, 10, TEXT("%d"), wndPos.y);
	WritePrivateProfileString(TEXT("Window"), TEXT("Y"), szString, lpFileName);
	StringCchPrintf(szString, 10, TEXT("%d"), wndSize.cx);
	WritePrivateProfileString(TEXT("Window"), TEXT("Width"), szString, lpFileName);
	StringCchPrintf(szString, 10, TEXT("%d"), wndSize.cy);
	WritePrivateProfileString(TEXT("Window"), TEXT("Height"), szString, lpFileName);
	WritePrivateProfileString(TEXT("File"), TEXT("Filename"), szFileName, lpFileName);
}
void OnClose(HWND hwnd)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	wndPos.x = rect.left;
	wndPos.y = rect.top;
	wndSize.cx = rect.right - rect.left;
	wndSize.cy = rect.bottom - rect.top;
	DestroyWindow(hwnd);
}
void LoadSettings(LPCTSTR lpFileName)
{
	wndPos.x = GetPrivateProfileInt(TEXT("Window"), TEXT("X"), CW_USEDEFAULT, lpFileName);
	wndPos.y = GetPrivateProfileInt(TEXT("Window"), TEXT("Y"), 0, lpFileName);
	wndSize.cx = GetPrivateProfileInt(TEXT("Window"), TEXT("Width"), CW_USEDEFAULT, lpFileName);
	wndSize.cy = GetPrivateProfileInt(TEXT("Window"), TEXT("Height"), 600, lpFileName);
	GetPrivateProfileString(TEXT("File"), TEXT("Filename"), NULL, szFileName, MAX_PATH, lpFileName);
}
void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
}
void OnDestroy(HWND hwnd)
{
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile), hFile = INVALID_HANDLE_VALUE;
	}
	if (NULL != hFont)
		DeleteObject(hFont), hFont = NULL;
	PostQuitMessage(0);
}
HANDLE OpenUserToken(LPCTSTR lpszUsername, LPCTSTR lpszDomain, LPCTSTR lpszPassword, DWORD dwLogonType,
	DWORD dwDesiredAccess, PSECURITY_ATTRIBUTES lpTokenAttributes, TOKEN_TYPE TokenType, SECURITY_IMPERSONATION_LEVEL ImpersonationLevel)
{
	HANDLE hToken = NULL;

	// получаем маркер доступа указанного пользователя
	BOOL bRet = LogonUser(lpszUsername, lpszDomain, lpszPassword, dwLogonType, LOGON32_PROVIDER_DEFAULT, &hToken);

	if (FALSE != bRet)
	{
		HANDLE hNewToken = NULL;

		// дублируем маркер доступа
		bRet = DuplicateTokenEx(hToken, dwDesiredAccess, lpTokenAttributes, ImpersonationLevel, TokenType, &hNewToken);

		// закрываем ранее полученный маркер доступа
		CloseHandle(hToken);

		// возвращаем результат
		hToken = (FALSE != bRet) ? hNewToken : NULL;
	} // if

	return hToken;
} // OpenUserToken
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
		TCHAR Login[255];
		TCHAR Password[255];
		hGlobalUser = NULL;
		GetDlgItemText(hwnd, ID_DlgLogin, Login, _countof(Login));
		GetDlgItemText(hwnd, ID_DlgPassword, Password, _countof(Password));

		HANDLE hToken = OpenUserToken(Login, TEXT("."), Password,
			LOGON32_LOGON_INTERACTIVE, TOKEN_QUERY | TOKEN_IMPERSONATE, NULL, TokenImpersonation, SecurityImpersonation);

		if (NULL != hToken)
		{
			hGlobalUser = hToken;

			BOOL bret = ImpersonateLoggedOnUser(hToken);
			if (FALSE != bret) {
				MessageBox(hwnd, TEXT("Вы успешно авторизировалиьс под Администратором!"), TEXT("Сообщение"), MB_OK | MB_ICONQUESTION);
			}
			else MessageBox(hwnd, TEXT("Не удалось зайти под Администратором!"), TEXT("Сообщение"), MB_OK | MB_ICONQUESTION);

			CloseHandle(hToken);
			EndDialog(hwnd, IDCANCEL);
		}
		else
		{
			MessageBox(hwnd, TEXT("Неверные данные. Попробуйте еще раз."), TEXT("Сообщение"), MB_OK);
		}
		
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
