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
HWND Id = NULL;
HWND list = NULL;
HWND hFindDlg = NULL;
HACCEL hAccel = NULL;
#define IDR_MENU1  101
#define IDR_ACCELERATOR1  2020
#define IDC_EDIT_TEXT  2001
#define ID_Write  2002
#define ID_Clear  2003
#define ID_Exit 2004
#define ID_WriteOne  2005
#define IDR_EditName  2006
#pragma pack(push, 1)
struct Students_DATA
{
	DWORD dwProcessId;
	DWORD dwIndex;
};
#pragma pack(pop)
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL PreTranslateMessage(LPMSG lpMsg);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
BOOL Write(HANDLE hFileMapping, LPCVOID lpData, DWORD cb);
BOOL Read(HANDLE hFileMapping, LPVOID lpData, DWORD cb);
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
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Windowslass"), TEXT("Клиент"), WS_OVERLAPPED,
		700, 86, 640, 720, NULL, NULL, hInstance, NULL);
	if (NULL == hwnd)
	{
		MessageBox(NULL, TEXT("Не удалось создать окно!"), TEXT("Ошибка"), MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}
	ShowWindow(hwnd, nCmdShow);
	for (;;)
	{
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){}
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
	list  = CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 10, 10, 600, 600, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, 0);
	CreateWindowEx(0, L"Button", L"Вывести всех", WS_CHILD | WS_VISIBLE, 10, 610, 220, 30, hwnd, (HMENU)ID_Write, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Button", L"Вывести одного", WS_CHILD | WS_VISIBLE, 250, 610, 220, 30, hwnd, (HMENU)ID_WriteOne, lpCreateStruct->hInstance, NULL);
	Id = CreateWindowEx(0, L"edit", NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE, 500, 615, 110, 20, hwnd, (HMENU)IDR_EditName, NULL, NULL);
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
	switch (id)
	{
	case ID_WriteOne: 
	{
		int cch = GetWindowTextLength(GetDlgItem(hwnd, IDR_EditName));
		if (cch > 0)
		{
			HANDLE hProjectionFile = NULL;
			HANDLE hServerNewData = NULL;
			HANDLE hClientNewData = NULL;
			HANDLE hSynchronization = NULL;
			hProjectionFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("Global\\ProjectionFile"));
			if (NULL == hProjectionFile)
			{
				ListBox_AddItemData(list, TEXT("Error"));
				break;
			}
			hServerNewData = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, TEXT("Global\\ServerNewData"));
			if (NULL == hServerNewData)
			{
				break;
			}
			hClientNewData = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, TEXT("Global\\ClientNewData"));
			if (NULL == hClientNewData)
			{
				break;
			}
			hSynchronization = OpenMutex(SYNCHRONIZE, FALSE, TEXT("Global\\Synchronization"));
			if (NULL == hSynchronization)
			{
				break;
			}
			BOOL Err;
			Students_DATA Request;
			Request.dwProcessId = GetCurrentProcessId();
			Request.dwIndex = GetDlgItemInt(hwnd, IDR_EditName, &Err, FALSE);
			TCHAR szResponse[100];
			if (Err != FALSE) {
				WaitForSingleObject(hSynchronization, INFINITE);
				BOOL bRet = Write(hProjectionFile, &Request, sizeof(Request));
				if (FALSE != bRet)
				{
					SetEvent(hServerNewData);
					WaitForSingleObject(hClientNewData, INFINITE);
					bRet = Read(hProjectionFile, szResponse, sizeof(szResponse));
				}
				ReleaseMutex(hSynchronization);
				if (FALSE != bRet)
				{
					if (_T('\0') == szResponse[0])
					{
						MessageBox(hwnd, TEXT("Нет студента с данным ID."), NULL, MB_OK | MB_ICONERROR);
						break;
					}
					ListBox_AddItemData(list, szResponse);
					Sleep(200);
				}
				else
				{
					ListBox_AddItemData(list, GetLastError());
					break;
				}
			} else MessageBox(hwnd, TEXT("Вы ввели текст!"), NULL, MB_OK | MB_ICONERROR);
			CloseHandle(hProjectionFile);
			CloseHandle(hServerNewData);
			CloseHandle(hClientNewData);
			CloseHandle(hSynchronization);
		} else MessageBox(hwnd, TEXT("Введите ID!"), NULL, MB_OK | MB_ICONERROR);
	}
	break;
	case ID_Exit:
	{
		PostQuitMessage(0);
	}
	break;
	case ID_Clear: 
	{
		ListBox_ResetContent(list);
	}
	break;
	case ID_Write: 
	{
		HANDLE hProjectionFile = NULL;
		HANDLE hServerNewData = NULL;
		HANDLE hClientNewData = NULL;
		HANDLE hSynchronization = NULL;
		hProjectionFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("Global\\ProjectionFile"));
		if (NULL == hProjectionFile)
		{
			ListBox_AddItemData(list, TEXT("Error"));
			break;	
		} 
		hServerNewData = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, TEXT("Global\\ServerNewData"));
		if (NULL == hServerNewData)
		{
			break;
		} 
		hClientNewData = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, TEXT("Global\\ClientNewData"));
		if (NULL == hClientNewData)
		{
			break;
		}
		hSynchronization = OpenMutex(SYNCHRONIZE, FALSE, TEXT("Global\\Synchronization"));
		if (NULL == hSynchronization)
		{
			break;
		}
		Students_DATA Request;
		Request.dwProcessId = GetCurrentProcessId();
		Request.dwIndex = 0;
		TCHAR szResponse[100];
		while (Request.dwIndex <= 16)
		{
			WaitForSingleObject(hSynchronization, INFINITE);
			BOOL bRet = Write(hProjectionFile, &Request, sizeof(Request));
			if (FALSE != bRet)
			{
				SetEvent(hServerNewData);
				WaitForSingleObject(hClientNewData, INFINITE);
				bRet = Read(hProjectionFile, szResponse, sizeof(szResponse));
			}
			ReleaseMutex(hSynchronization);
			if (FALSE != bRet)
			{
				if (_T('\0') == szResponse[0])
				{
					
					break;
				}
				ListBox_AddItemData(list, szResponse);
				Sleep(200);
			}
			else
			{
				ListBox_AddItemData(list, GetLastError());
				break;
			}
			Request.dwIndex++;
		}
		CloseHandle(hProjectionFile);
		CloseHandle(hServerNewData);
		CloseHandle(hClientNewData);
		CloseHandle(hSynchronization);
	}
	break;
	}
}
BOOL Write(HANDLE hFileMapping, LPCVOID lpData, DWORD cb)
{
	PBYTE pbFile = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, cb);
	if (NULL == pbFile) return FALSE;
	CopyMemory(pbFile, lpData, cb);
	BOOL bRet = FlushViewOfFile(pbFile, cb);
	UnmapViewOfFile(pbFile);
	return bRet;
}
BOOL Read(HANDLE hFileMapping, LPVOID lpData, DWORD cb)
{
	PBYTE pbFile = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, cb);
	if (NULL == pbFile) return FALSE;
	CopyMemory(lpData, pbFile, cb);
	UnmapViewOfFile(pbFile);
	return TRUE;
}