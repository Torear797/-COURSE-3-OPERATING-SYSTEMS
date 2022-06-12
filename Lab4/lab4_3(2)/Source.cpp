#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h> 
#include <random>  
#include <process.h>
#include "resource.h"
#include "psapi.h"
#include "strsafe.h"
#include <strsafe.h>
#define IDR_MENU1  101
#define IDR_ACCELERATOR1  2020
#define IDC_EDIT_TEXT  2001
#define ID_CreateFile  2002
#define ID_SafeFile  2003
#define ID_OpenFile  2004
#define ID_SafeFileAS  2005
#define ID_Exit  2006
MSG msg;
HWND hwnd = NULL;
HWND hbtn;
HWND hbtn2;
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
bool SaveAs = false;
LOGFONT logFont;
HFONT hFont = NULL;
void WriteInstelPrograms(REGSAM samDesired);
void WriteAutorun(HKEY hRootKey, REGSAM samDesired);
LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPTSTR lpszData, DWORD cch, LPDWORD lpcchNeeded);
LPSTR lpszBufferText = NULL;
OVERLAPPED _oRead = { 0 }, _oWrite = { 0 };
BOOL OpenFileAsync(HWND hwndCtl);
BOOL SaveFileAsync(HWND hwndCtl, BOOL fSaveAs = FALSE);
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
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Windowsclass"), TEXT("Программа № 1"), WS_OVERLAPPEDWINDOW,
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
BOOL ReadAsync(HANDLE hFile, LPVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED lpOverlapped)
{
	ZeroMemory(lpOverlapped, sizeof(OVERLAPPED));
	lpOverlapped->Offset = dwOffset;
	lpOverlapped->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	BOOL bRet = ReadFile(hFile, lpBuffer, dwSize, NULL, lpOverlapped);
	if (FALSE == bRet && ERROR_IO_PENDING != GetLastError())
	{
		CloseHandle(lpOverlapped->hEvent), lpOverlapped->hEvent = NULL;
		return false;
	}
	return true;
}
BOOL WriteAsync(HANDLE hFile, LPCVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED lpOverlapped)
{
	ZeroMemory(lpOverlapped, sizeof(OVERLAPPED));
	lpOverlapped->Offset = dwOffset;
	lpOverlapped->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	BOOL bRet = WriteFile(hFile, lpBuffer, dwSize, NULL, lpOverlapped);
	if (FALSE == bRet && ERROR_IO_PENDING != GetLastError())
	{
		CloseHandle(lpOverlapped->hEvent), lpOverlapped->hEvent = NULL;
		return false;
	}
	return true;
}
BOOL FinishIo(LPOVERLAPPED lpOverlapped)
{
	if (NULL != lpOverlapped->hEvent)
	{
		DWORD dwResult = WaitForSingleObject(lpOverlapped->hEvent, INFINITE);
		if (WAIT_OBJECT_0 == dwResult)
		{
			CloseHandle(lpOverlapped->hEvent), lpOverlapped->hEvent = NULL;
			return true;
		}
	}
	return FALSE;
}
BOOL TryFinishIo(LPOVERLAPPED lpOverlapped)
{
	if (NULL != lpOverlapped->hEvent)
	{
		DWORD dwResult = WaitForSingleObject(lpOverlapped->hEvent, 0);
		if (WAIT_OBJECT_0 == dwResult)
		{
			CloseHandle(lpOverlapped->hEvent), lpOverlapped->hEvent = NULL;
			return true;
		}
	}
	return false;
}
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_WANTRETURN | WS_VSCROLL;
	//HWND hwndCtl = CreateWindowEx(0, TEXT("Edit"), TEXT(""), dwStyle, 10, 10, 300, 110, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);

	//HWND hwndCtl = CreateWindowEx(0, TEXT("combobox"), TEXT(""), WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST|PGS_AUTOSCROLL , 10, 10, 300, 300, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, 0);

	HWND hwndCtl = CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 10, 10, 600, 600, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, 0);

	//hFont = CreateFont(20, 8, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));
	if (NULL != hFont)SendMessage(hwndCtl, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	if (OpenFileAsync(hwndCtl) != FALSE) SetWindowText(hwnd, szFileName);
	else
	{
		szFileName[0] = _T('\0');
		SaveAs = true;
		SetWindowText(hwnd, TEXT("Новый файл"));
	}
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
void WriteInstelPrograms(REGSAM samDesired)
{
	HKEY hKey;
	LSTATUS lStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"), REG_OPTION_NON_VOLATILE, samDesired, &hKey);
	static HWND hE = GetDlgItem(hwnd, IDC_EDIT_TEXT);
	if (ERROR_SUCCESS == lStatus)
	{
		DWORD cSubKeys;
		DWORD cMaxName;
		lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, &cMaxName, NULL, NULL, NULL, NULL, NULL, NULL);
		if ((ERROR_SUCCESS == lStatus) && (cSubKeys > 0))
		{
			LPTSTR szSubKeyName = new TCHAR[cMaxName + 1];

			for (DWORD dwIndex = 0; dwIndex < cSubKeys; ++dwIndex)
			{
				HKEY hSubKey = NULL;
				DWORD cchName = cMaxName + 1;
				lStatus = RegEnumKeyEx(hKey, dwIndex, szSubKeyName, &cchName, NULL, NULL, NULL, NULL);

				if (ERROR_SUCCESS == lStatus) lStatus = RegOpenKeyEx(hKey, szSubKeyName, REG_OPTION_NON_VOLATILE, samDesired, &hSubKey);

				if (ERROR_SUCCESS == lStatus)
				{
					DWORD cMaxValueLen = 0;
					lStatus = RegQueryInfoKey(hSubKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &cMaxValueLen, NULL, NULL);

					if ((ERROR_SUCCESS == lStatus) && (cMaxValueLen > 0))
					{
						LPTSTR lpValueData = new TCHAR[cMaxValueLen];

						lStatus = RegGetValueSZ(hSubKey, TEXT("DisplayName"), lpValueData, cMaxValueLen, NULL);

						if ((ERROR_SUCCESS == lStatus) && (_T('\0') != lpValueData[0]))
						{
							//SetDlgItemText(hwnd, IDC_EDIT_TEXT, TEXT("Name:"));
						//	SetDlgItemText(hwnd, IDC_EDIT_TEXT, lpValueData);
						//	ComboBox_AddItemData(hE, lpValueData);
							ListBox_AddItemData(hE, lpValueData);
							
						
							lStatus = RegGetValueSZ(hSubKey, TEXT("Publisher"), lpValueData, cMaxValueLen, NULL);

							if ((ERROR_SUCCESS == lStatus) && (_T('\0') != lpValueData[0]))  ListBox_AddItemData(hE, lpValueData);

							lStatus = RegGetValueSZ(hSubKey, TEXT("DisplayVersion"), lpValueData, cMaxValueLen, NULL);

							if ((ERROR_SUCCESS == lStatus) && (_T('\0') != lpValueData[0]))  ListBox_AddItemData(hE, lpValueData);
							ListBox_AddItemData(hE, TEXT(" "));
						}
						delete[] lpValueData;
					}
					RegCloseKey(hSubKey), hSubKey = NULL;
				}
			}
			delete[] szSubKeyName;
		}
		RegCloseKey(hKey), hKey = NULL;
	}
}
void WriteAutorun(HKEY hRootKey, REGSAM samDesired)
{
	HKEY hKey = NULL;
	LSTATUS lStatus = RegOpenKeyEx(hRootKey, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), REG_OPTION_NON_VOLATILE, samDesired, &hKey);
	if (ERROR_SUCCESS == lStatus)
	{
		DWORD cValues; DWORD cMaxValueNameLen;
		DWORD cbMaxValueLen;
		static HWND hE = GetDlgItem(hwnd, IDC_EDIT_TEXT);
		lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &cValues, &cMaxValueNameLen, &cbMaxValueLen, NULL, NULL);

		if ((ERROR_SUCCESS == lStatus) && (cValues > 0))
		{
			LPTSTR szValueName = new TCHAR[cMaxValueNameLen + 1];
			LPBYTE lpValueData = new BYTE[cbMaxValueLen];

			for (DWORD dwIndex = 0; dwIndex < cValues; ++dwIndex)
			{
				DWORD cchValueName = cMaxValueNameLen + 1, dwType, cbData = cbMaxValueLen;
				lStatus = RegEnumValue(hKey, dwIndex, szValueName, &cchValueName, NULL, &dwType, lpValueData, &cbData);

				if ((ERROR_SUCCESS == lStatus) && (cchValueName > 0))
				{
					if ((REG_SZ == dwType) || (REG_EXPAND_SZ == dwType))
					{
						//SetDlgItemText(hwnd, IDC_EDIT_TEXT, TEXT("Name:"));
						//SetDlgItemText(hwnd, IDC_EDIT_TEXT, szValueName);
						
						//SetDlgItemText(hwnd, IDC_EDIT_TEXT, TEXT("Path:"));
						//SetDlgItemText(hwnd, IDC_EDIT_TEXT, (LPTSTR)lpValueData);
					//	ListBox_AddItemData(hE, TEXT("Name: "));
						ListBox_AddItemData(hE, szValueName);

						//ListBox_AddItemData(hE, TEXT("Path:"));
						ListBox_AddItemData(hE, (LPTSTR)lpValueData);
						ListBox_AddItemData(hE, TEXT(" "));
					//	ComboBox_AddItemData(hE, szValueName);
					//	StringCchPrintf(Bufer4, _countof(Bufer4), TEXT("%s"), szValueName);
						//wcout << "Path: " << (LPTSTR)lpValueData << endl << endl;
					}
				}
			}
			delete[] lpValueData;
			delete[] szValueName;
		}
		RegCloseKey(hKey), hKey = NULL;
	}
}
LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPTSTR lpszData, DWORD cch, LPDWORD lpcchNeeded)
{
	DWORD dwType;
	LSTATUS lStatus = RegQueryValueEx(hKey, lpValueName, NULL, &dwType, NULL, NULL);
	if (ERROR_SUCCESS == lStatus && REG_SZ == dwType)
	{
		DWORD cb = cch * sizeof(TCHAR);
		lStatus = RegQueryValueEx(hKey, lpValueName, NULL, NULL, (LPBYTE)lpszData, &cb);
		if (NULL != lpcchNeeded)*lpcchNeeded = cb / sizeof(TCHAR);
	}
	else if (ERROR_SUCCESS == lStatus) lStatus = ERROR_UNSUPPORTED_TYPE;
	return lStatus;
}
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	static HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_TEXT);
	static HWND hE = GetDlgItem(hwnd, IDC_EDIT_TEXT);
	switch (id)
	{
	case ID_CreateFile:
	{
		if (INVALID_HANDLE_VALUE != hFile)
		{
			FinishIo(&_oWrite);
			CloseHandle(hFile), hFile = INVALID_HANDLE_VALUE;
		}
		Edit_SetText(hEdit, NULL);
		szFileName[0] = _T('\0');
		SetWindowText(hwnd, TEXT("Новый файл"));
		SaveAs = true;
	}
	break;
	case ID_OpenFile:
	{
		//StringCchPrintf(Bufer4, _countof(Bufer4), TEXT("%s"), TEXT("На данном компьютере установлены следующие программы:"));
	//	SetDlgItemText(hwnd, IDC_EDIT_TEXT, TEXT("На данном компьютере установлены следующие программы:"));
		ListBox_AddItemData(hE, TEXT("На данном компьютере установлены следующие программы:"));
		//ComboBox_AddItemData(hwndCtl, TEXT("На данном компьютере установлены следующие программы:"));
		
		WriteInstelPrograms(KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_64KEY);
		WriteInstelPrograms(KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_32KEY);
		//ComboBox_AddItemData(hwndCtl, TEXT("В автозагрузке данного компьютера:"));
		ListBox_AddItemData(hE, TEXT("В автозагрузке данного компьютера:"));
	//	SetDlgItemText(hwnd, IDC_EDIT_TEXT, TEXT("В автозагрузке данного компьютера:"));
		WriteAutorun(HKEY_CURRENT_USER, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_64KEY);
		WriteAutorun(HKEY_LOCAL_MACHINE, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_64KEY);
		WriteAutorun(HKEY_CURRENT_USER, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_32KEY);
		WriteAutorun(HKEY_LOCAL_MACHINE, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_32KEY);

	//	SetDlgItemText(hwnd, IDC_EDIT_TEXT, Bufer4);
	}
	break;
	case ID_SafeFile:
	{
		if (SaveAs == false)
		{
			if (SaveFileAsync(hEdit) != FALSE)
			{
				MessageBox(hwnd, TEXT("Файл успешно сохранен!"), TEXT("Работа с файлом"), MB_OK);
				break;
			}
		}
		else
		{
			OPENFILENAME ofn = { sizeof(OPENFILENAME) };
			ofn.hInstance = GetWindowInstance(hwnd);
			ofn.lpstrFilter = TEXT("Текстовые документы (*.txt)\0*.txt\0");
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = _countof(szFileName);
			ofn.lpstrTitle = TEXT("Сохранить как");
			ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;
			ofn.lpstrDefExt = TEXT("txt");
			if (GetSaveFileName(&ofn) != FALSE)
			{
				if (SaveFileAsync(hEdit, TRUE) != FALSE) {
					SetWindowText(hwnd, szFileName);
					MessageBox(hwnd, TEXT("Новый файл успешно сохранен!"), TEXT("Работа с файлом"), MB_OK);
					SaveAs = false;
				}
				else MessageBox(NULL, TEXT("Не удалось сохранить текстовый файл."), NULL, MB_OK | MB_ICONERROR);
			}
		}

	}
	case ID_SafeFileAS:
	{
		OPENFILENAME ofn = { sizeof(OPENFILENAME) };
		ofn.hInstance = GetWindowInstance(hwnd);
		ofn.lpstrFilter = TEXT("Текстовые документы (*.txt)\0*.txt\0");
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = _countof(szFileName);
		ofn.lpstrTitle = TEXT("Сохранить как");
		ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;
		ofn.lpstrDefExt = TEXT("txt");
		if (GetSaveFileName(&ofn) != FALSE)
		{
			if (SaveFileAsync(hEdit, TRUE) != FALSE) {
				SetWindowText(hwnd, szFileName);
				MessageBox(hwnd, TEXT("Новый файл успешно сохранен!"), TEXT("Работа с файлом"), MB_OK);
			}
			else MessageBox(NULL, TEXT("Не удалось сохранить текстовый файл."), NULL, MB_OK | MB_ICONERROR);
		}
	}
	break;
	case ID_Exit:
		OnClose(hwnd);
		break;
	}
}
BOOL OpenFileAsync(HWND hwndCtl)
{
	HANDLE hExistingFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (INVALID_HANDLE_VALUE == hExistingFile) return FALSE;
	Edit_SetText(hwndCtl, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		FinishIo(&_oWrite);
		CloseHandle(hFile);
	}
	hFile = hExistingFile;
	LARGE_INTEGER size;
	BOOL bRet = GetFileSizeEx(hFile, &size);
	if ((FALSE != bRet) && (size.LowPart > 0))
	{
		lpszBufferText = new CHAR[size.LowPart + 2];
		bRet = ReadAsync(hFile, lpszBufferText, 0, size.LowPart, &_oRead);
		if (FALSE == bRet) delete[] lpszBufferText, lpszBufferText = NULL;
	}
	return bRet;
}
BOOL SaveFileAsync(HWND hwndCtl, BOOL fSaveAs)
{
	if (FALSE != fSaveAs)
	{
		HANDLE hNewFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
		if (INVALID_HANDLE_VALUE == hNewFile) return FALSE;
		if (INVALID_HANDLE_VALUE != hFile)
		{
			FinishIo(&_oWrite);
			CloseHandle(hFile);
		}
		hFile = hNewFile;
	}
	else if (INVALID_HANDLE_VALUE != hFile) FinishIo(&_oWrite);
	else
	{
		hFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
		if (INVALID_HANDLE_VALUE == hFile) return FALSE;
	}
	LARGE_INTEGER size;
	size.QuadPart = GetWindowTextLengthA(hwndCtl);
	BOOL bRet = SetFilePointerEx(hFile, size, NULL, FILE_BEGIN);
	if (FALSE != bRet) bRet = SetEndOfFile(hFile);
	if ((FALSE != bRet) && (size.LowPart > 0))
	{
		lpszBufferText = new CHAR[size.LowPart + 1];
		GetWindowTextA(hwndCtl, lpszBufferText, size.LowPart + 1);
		bRet = WriteAsync(hFile, lpszBufferText, 0, size.LowPart, &_oWrite);
		if (FALSE == bRet) delete[] lpszBufferText, lpszBufferText = NULL;
	}
	return bRet;
}
void OnIdle(HWND hwnd)
{
	if (TryFinishIo(&_oRead) != FALSE)
	{
		if (ERROR_SUCCESS == _oRead.Internal)
		{
			WORD bom = *(LPWORD)lpszBufferText;

			if (0xFEFF == bom)
			{
				LPWSTR lpszText = (LPWSTR)(lpszBufferText + sizeof(WORD));
				DWORD cch = (_oRead.InternalHigh - sizeof(WORD)) / sizeof(WCHAR);
				lpszText[cch] = L'\0';
				SetDlgItemTextW(hwnd, IDC_EDIT_TEXT, lpszText);
			}
			else
			{
				lpszBufferText[_oRead.InternalHigh] = '\0';
				SetDlgItemTextA(hwnd, IDC_EDIT_TEXT, lpszBufferText);
			}
		}
		delete[] lpszBufferText, lpszBufferText = NULL;
	}
	else if (TryFinishIo(&_oWrite) != FALSE)
	{
		if (ERROR_SUCCESS == _oWrite.Internal) FlushFileBuffers(hFile);
		delete[] lpszBufferText, lpszBufferText = NULL;
	}
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
	/*if (SIZE_MINIMIZED != state)
	{
		HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_TEXT);
		MoveWindow(hwndCtl, 0, 0, cx, cy, TRUE);
	}*/
}
void OnDestroy(HWND hwnd)
{
	if (INVALID_HANDLE_VALUE != hFile)
	{
		FinishIo(&_oWrite);
		CloseHandle(hFile), hFile = INVALID_HANDLE_VALUE;
	}
	if (NULL != hFont)
		DeleteObject(hFont), hFont = NULL;
	PostQuitMessage(0);
}