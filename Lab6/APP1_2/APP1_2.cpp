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

MSG msg;
HWND hwnd = NULL;
BOOL bRet;
HWND hDlg = NULL;
HWND hFindDlg = NULL;
HACCEL hAccel = NULL;
#define IDR_MENU1  101
#define IDR_ACCELERATOR1  2020

HANDLE thread[1];
HWND label1 = NULL;
HWND hwndEdit = NULL;
#define IDC_EDIT_TEXT  200
#define IDC_LIST  201
HWND list = NULL;
HWND SendText = NULL;
#define IDC_SendText  202
BOOL WriteClipboard(LPCTSTR lpszText);
BOOL ReadClipboard(LPTSTR lpszText, DWORD cch);
HWND GetText = NULL;
HANDLE hSendmes = NULL;
HANDLE hBufer = NULL;
HANDLE hExitApp;
HKEY hKey = NULL; 
HANDLE hBuffer_synchronization = NULL;
#define IDC_GetText  203
#define IDC_Exit 205
#define IDC_Clear 206
unsigned __stdcall FuncGetMessage(void *lpParameter);

BOOL WriteReestrAPPExemplar();

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL PreTranslateMessage(LPMSG lpMsg);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

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

	if (0 == RegisterClassEx(&wcex))
	{
		MessageBox(NULL, TEXT("Не удалось зарегестрировать класс!"), TEXT("Ошибка"), MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	LoadLibrary(TEXT("ComCtl32.dll"));

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Windowslass"), TEXT("Приложение № 1"), WS_OVERLAPPED,
		CW_USEDEFAULT, 0, 300, 520, NULL, NULL, hInstance, NULL);

	if (NULL == hwnd)
	{
		MessageBox(NULL, TEXT("Не удалось создать окно!"), TEXT("Ошибка"), MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	BOOL bRet = WriteReestrAPPExemplar();

	if (FALSE == bRet) return -1;
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

	if (NULL != hKey)
	{
		DWORD cSubKeys = 0;
		LSTATUS lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		if ((ERROR_SUCCESS == lStatus) && (cSubKeys < 2))
		{
			RegDeleteTree(hKey, NULL);
			RegDeleteKey(HKEY_CURRENT_USER, TEXT("Software\\Lab6"));
		}
		else
		{
			TCHAR szSubKey[20];
			StringCchPrintf(szSubKey, _countof(szSubKey), TEXT("%d"), GetCurrentProcessId());
			RegDeleteKey(hKey, szSubKey);
		}
		RegCloseKey(hKey), hKey = NULL;
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
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_WANTRETURN;
	label1 = CreateWindowEx(0, L"Static", L"Введите текст:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 280, 200, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	hwndEdit = CreateWindowEx(0, TEXT("Edit"), TEXT(""), dwStyle, 10, 300, 250, 20, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);
	list = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 10, 10, 250, 250, hwnd, (HMENU)IDC_LIST, lpCreateStruct->hInstance, NULL);
	SendText = CreateWindowEx(0, TEXT("Button"), TEXT("Отправить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		10, 340, 250, 30, hwnd, (HMENU)IDC_SendText, lpCreateStruct->hInstance, NULL);
	GetText = CreateWindowEx(0, TEXT("Button"), TEXT("Принять"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		10, 380, 250, 30, hwnd, (HMENU)IDC_GetText, lpCreateStruct->hInstance, NULL);
	 CreateWindowEx(0, TEXT("Button"), TEXT("Выход"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		10, 420, 250, 30, hwnd, (HMENU)IDC_Exit, lpCreateStruct->hInstance, NULL);
	
	hExitApp = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
	hSendmes = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
	hBuffer_synchronization = CreateEventEx(NULL, TEXT("EBuffer"), 0, EVENT_ALL_ACCESS);
	thread[0] = (HANDLE)_beginthreadex(NULL, 0, FuncGetMessage, NULL, 0, NULL);
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
		SetEvent(hExitApp);
		WaitForMultipleObjects(_countof(thread), thread, TRUE, INFINITE);
		CloseHandle(hExitApp);
		CloseHandle(hSendmes);
		CloseHandle(hBuffer_synchronization);
		PostQuitMessage(0);
	}
	break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{

	switch (id)
	{
	case IDC_Clear: 
	{
		ListBox_ResetContent(list);
	}
	break;
	case IDC_Exit:
	{
		SendMessage(hwnd, WM_DESTROY, 0, 0);
		//OnClose(hwnd);
	}
	break;
	case IDC_SendText:
	{
		TCHAR ESendMessage[255];
		DWORD Lenth = Edit_GetText(hwndEdit, ESendMessage, sizeof(ESendMessage));
		SetEvent(hBuffer_synchronization);
		if (Lenth > 0)
		{
			WaitForSingleObject(hBuffer_synchronization, INFINITE);
			BOOL bRet = WriteClipboard(ESendMessage);
			if (FALSE != bRet)
			{
				DWORD cSubKeys = 0;
				LSTATUS lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				if ((ERROR_SUCCESS == lStatus) && (cSubKeys > 0))
				{
					HKEY  hSubKey = NULL;
					TCHAR szSubKey[20];
					for (DWORD dwIndex = 0; dwIndex < cSubKeys; ++dwIndex)
					{
						DWORD cchSubKey = _countof(szSubKey);
						lStatus = RegEnumKeyEx(hKey, dwIndex, szSubKey, &cchSubKey, NULL, NULL, NULL, NULL);
						if (ERROR_SUCCESS == lStatus)
						{
							lStatus = RegOpenKeyEx(hKey, szSubKey, 0, KEY_QUERY_VALUE, &hSubKey);
						}

						if (ERROR_SUCCESS == lStatus)
						{
							DWORD dwProcessId = 0;
							HANDLE hEvent = NULL;

							DWORD cb = sizeof(DWORD);
							lStatus = RegQueryValueEx(hSubKey, NULL, NULL, NULL, (LPBYTE)&dwProcessId, &cb);

							if (ERROR_SUCCESS == lStatus)
							{
								cb = sizeof(HANDLE);
								lStatus = RegQueryValueEx(hSubKey, TEXT("hSendmes"), NULL, NULL, (LPBYTE)&hEvent, &cb);
							}
							if (ERROR_SUCCESS == lStatus)
							{
								BOOL bRet = FALSE;
								HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, dwProcessId);

								if (NULL != hProcess)
								{
									bRet = DuplicateHandle(hProcess, hEvent, GetCurrentProcess(), &hEvent, GENERIC_WRITE, FALSE, 0);
									CloseHandle(hProcess);
								}
								if (FALSE != bRet)
								{
									SetEvent(hEvent);
									CloseHandle(hEvent);
								}
							}
							RegCloseKey(hSubKey), hSubKey = NULL;
						}
					}
				}
				MessageBox(hwnd, TEXT("Данные успешно записаны в буфер!"), TEXT("Сообщение"), MB_OK);
			}
			else MessageBox(hwnd, TEXT("Ошибка записи в буфер!"), TEXT("Сообщение"), MB_OK);
		}
		else MessageBox(hwnd, TEXT("Введите сообщение!"), TEXT("Сообщение"), MB_OK);
		SetEvent(hBuffer_synchronization);
	}
	break;
	case IDC_GetText:
	{
		TCHAR EGetMessage[255];
		ReadClipboard(EGetMessage, _countof(EGetMessage));
		ListBox_AddItemData(list, EGetMessage);
	}
	break;
	}
}
BOOL WriteClipboard(LPCTSTR lpszText)
{
	BOOL bRet = OpenClipboard(NULL); // открывем буфер обмена

	if (FALSE != bRet)
	{
		EmptyClipboard(); // удаляем содержимое буфера обмена

						  // определяем размер (в байтах) блока памяти
		DWORD cb = (_tcslen(lpszText) + 1) * sizeof(TCHAR);

		// выделяем блок памяти под текст
		HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, cb);
		bRet = (NULL != hMem) ? TRUE : FALSE;

		if (FALSE != bRet)
		{
			// копируем текст в блок памяти
			StringCbCopy((LPTSTR)GlobalLock(hMem), cb, lpszText);
			GlobalUnlock(hMem);

#ifdef UNICODE
			UINT uFormat = CF_UNICODETEXT;
#else
			UINT uFormat = CF_TEXT;
#endif /* UNICODE */

			// передаем данные в буфер обмена
			HANDLE hRet = SetClipboardData(uFormat, hMem);
			bRet = (NULL != hRet) ? TRUE : FALSE;
		} // if

		CloseClipboard(); // закрываем буфер обмена

						  // освобождаем блок памяти
		if (NULL != hMem) GlobalFree(hMem);
	} // if

	return bRet;
} // WriteClipboard
BOOL ReadClipboard(LPTSTR lpszText, DWORD cch)
{
#ifdef UNICODE
	UINT uFormat = CF_UNICODETEXT;
#else
	UINT uFormat = CF_TEXT;
#endif /* UNICODE */

	// проверяем есть ли в буфере обмена данные нужного формата
	BOOL bRet = IsClipboardFormatAvailable(uFormat);

	if (FALSE != bRet) // если да
		bRet = OpenClipboard(NULL); // открывем буфер обмена

	if (FALSE != bRet)
	{
		// получаем дескриптор блока памяти
		HANDLE hMem = GetClipboardData(uFormat);
		bRet = (NULL != hMem) ? TRUE : FALSE;

		if (FALSE != bRet)
		{
			// копируем текст из полученного блока памяти
			StringCchCopy(lpszText, cch, (LPCTSTR)GlobalLock(hMem));
			GlobalUnlock(hMem);
		} // if

		CloseClipboard(); // закрываем буфер обмена
	} // if

	return bRet;
} // ReadClipboard
unsigned __stdcall FuncGetMessage(void *lpParameter)
{
	HANDLE hEvents[2] = { hExitApp, hSendmes };
	TCHAR EGetMessage[255];
	for (;;)
	{
		DWORD dwResult = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
		if (WAIT_OBJECT_0 == dwResult)
		{
			break;
		}
		else if ((WAIT_OBJECT_0 + 1) == dwResult)
		{
			WaitForSingleObject(hBuffer_synchronization, INFINITE);
			BOOL bRet = ReadClipboard(EGetMessage, _countof(EGetMessage));
			if (FALSE != bRet)
			{
				ListBox_AddItemData(list, EGetMessage);
			} 
			SetEvent(hBuffer_synchronization);
		}
	}
	return 0;
}
BOOL WriteReestrAPPExemplar() 
{
	DWORD dwDisposition;
	LONG lStatus = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Lab6"),
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (ERROR_SUCCESS == lStatus)
	{
		DWORD dwProcessId = GetCurrentProcessId();
		HKEY  hSubKey = NULL;
		TCHAR szSubKey[20];
		StringCchPrintf(szSubKey, _countof(szSubKey), TEXT("%d"), dwProcessId);
		lStatus = RegCreateKey(hKey, szSubKey, &hSubKey);
		if (ERROR_SUCCESS == lStatus)
		{
			RegSetValueEx(hSubKey, NULL, 0, REG_DWORD, (LPBYTE)&dwProcessId, sizeof(DWORD));
			//RegSetValueEx(hSubKey, TEXT("hwnd"), 0, REG_BINARY, (LPBYTE)&hwnd, sizeof(HWND));
			RegSetValueEx(hSubKey, TEXT("hSendmes"), 0, REG_BINARY, (LPBYTE)&hSendmes, sizeof(HANDLE));
			RegCloseKey(hSubKey), hSubKey = NULL;
			return TRUE;
		}
		RegCloseKey(hKey), hKey = NULL;
	}
	return FALSE;
}