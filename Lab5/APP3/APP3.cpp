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
#include <LMCons.h>
#include <sddl.h>
#include <NTSecAPI.h>
#include <AclAPI.h>
#include <AccCtrl.h>
#include <WinNT.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
PSECURITY_DESCRIPTOR pSD = NULL;
MSG msg;
HWND hwnd = NULL;
BOOL bRet;
HWND hDlg = NULL; HWND EditFolder; HWND Name; HWND list; HWND NewName;
HWND hFindDlg = NULL;
HACCEL hAccel = NULL;
#define IDR_MENU1  101
#define IDR_ACCELERATOR1  2020
#define IDR_Open  2021
#define IDR_EditName  2022
#define IDR_Save  2023
#define IDR_OpenFolder  2024
#define IDC_OpenFolder 2025
#define IDC_EDIT1 2026
#define ID_LV 2027
#define ID_Update 2028
#define ID_AddACE 2029
#define ID_DeleteACE 2030
#define IDR_Save2  2031
#define IDC_LIST4  2033
HWND list4;
#define IDR_EditName2  2032
HWND hlabel1; HWND hlabelPath; HWND hNo; HWND hYes; HWND hbox;
TCHAR szPath[MAX_PATH] = TEXT("");
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL PreTranslateMessage(LPMSG lpMsg);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
BOOL GetAccountName(PSID pSid, LPTSTR *lpName);
HANDLE OpenUserToken(LPCTSTR lpszUsername, LPCTSTR lpszDomain, LPCTSTR lpszPassword, DWORD dwLogonType,
	DWORD dwDesiredAccess, PSECURITY_ATTRIBUTES lpTokenAttributes, TOKEN_TYPE TokenType, SECURITY_IMPERSONATION_LEVEL ImpersonationLevel);
BOOL GetAccountSID(LPCTSTR lpAccountName, PSID *ppSid);
BOOL SetFileSecurityInfo(LPCTSTR lpFileName, LPCTSTR lpAccountOwner, ULONG cCountOfEntries, PEXPLICIT_ACCESS pListOfEntries, BOOL bMergeEntries);
BOOL GetFileSecurityDescriptor(LPCTSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR *ppSD);
BOOL GetOwnerName(PSECURITY_DESCRIPTOR pSD, LPTSTR *lpName);
BOOL GetEntriesFromDacl(PSECURITY_DESCRIPTOR pSD, PULONG pcCountOfEntries, PEXPLICIT_ACCESS *pListOfEntries);
void Dialog_OnCommand2(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
BOOL DeleteEntryFromDalc(PSECURITY_DESCRIPTOR pSD, DWORD dwIndex);
INT_PTR CALLBACK DialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
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

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Windowslass"), TEXT("Приложение № 3"), WS_OVERLAPPEDWINDOW,
		200, 100, 1200, 650, NULL, NULL, hInstance, NULL);

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
	CreateWindowEx(0, L"Static", L"Расположение:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 10, 10, 100, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	hlabelPath = CreateWindowEx(0, L"Static", L"0", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 150, 10, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
Name = CreateWindowEx(0, L"edit", NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE, 10, 50, 200, 20, hwnd, (HMENU)IDR_EditName, NULL, NULL);
	CreateWindowEx(0, L"Button", L"Сохранить", WS_CHILD | WS_VISIBLE, 220, 47, 165, 25, hwnd, (HMENU)IDR_Save, lpCreateStruct->hInstance, NULL);
	
	list = CreateWindowEx(0, WC_LISTVIEW, L"", WS_VISIBLE | WS_CHILD |
		WS_BORDER | LVS_REPORT | LVS_EDITLABELS, 10, 80, 375, 360, hwnd, (HMENU)ID_LV, lpCreateStruct->hInstance, NULL);

	LVCOLUMN lvc;
	wchar_t *str = new wchar_t[100];
	wcscpy(str, L"Тип");
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = str;
	lvc.cx = 125;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(list, 0, &lvc);
	wcscpy(str, L"Имя");
	lvc.iSubItem = 1;
	ListView_InsertColumn(list, 1, &lvc);
	wcscpy(str, L"Применить к");
	lvc.iSubItem = 2;
	ListView_InsertColumn(list, 2, &lvc);

	CreateWindowEx(0, L"Button", L"Добавить", WS_CHILD | WS_VISIBLE, 550, 480, 200, 25, hwnd, (HMENU)ID_AddACE, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Button", L"Удалить", WS_CHILD | WS_VISIBLE, 10, 450, 100, 25, hwnd, (HMENU)ID_DeleteACE, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Button", L"Обновить", WS_CHILD | WS_VISIBLE, 280, 450, 100, 25, hwnd, (HMENU)ID_Update, lpCreateStruct->hInstance, NULL);
	//
	CreateWindowEx(0, L"Static", L"Тип:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 400, 80, 50, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	hNo = CreateWindowEx(0, L"button", L"Запретить", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 450, 75, 100, 30, hwnd, (HMENU)2066, NULL, NULL);
	hYes =CreateWindowEx(0, L"button", L"Разрешить", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 600, 75, 100, 30, hwnd, (HMENU)2067, NULL, NULL);
	SendMessage(hYes, BM_SETCHECK, BST_CHECKED, 0);

	CreateWindowEx(0, L"Static", L"Имя:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 400, 130, 50, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	NewName = CreateWindowEx(0, L"edit", NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE, 440, 130, 200, 20, hwnd, (HMENU)IDR_EditName2, NULL, NULL);
	CreateWindowEx(0, L"Button", L"Проверить", WS_CHILD | WS_VISIBLE, 660, 127, 165, 25, hwnd, (HMENU)IDR_Save2, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, L"Static", L"Применять:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 400, 180, 100, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	//hbox = CreateWindowEx(0, L"combobox", L"combobox", WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST, 500, 177, 325, 25, hwnd, (HMENU)IDR_Save2, lpCreateStruct->hInstance, NULL);

    CreateWindowEx(0, L"button", L"Только этот каталог", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 480, 177, 250, 30, hwnd, (HMENU)2068, NULL, NULL);
	CreateWindowEx(0, L"button", L"Этот каталог, его подкаталоги и файлы", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 480, 200, 300, 30, hwnd, (HMENU)2069, NULL, NULL);
	CreateWindowEx(0, L"button", L"Этот каталог и его подкаталоги", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 480, 223, 250, 30, hwnd, (HMENU)2070, NULL, NULL);
	
	CreateWindowEx(0, L"button", L"Этот каталог и его файлы", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 800, 177, 200, 30, hwnd, (HMENU)2071, NULL, NULL);
	CreateWindowEx(0, L"button", L"Подкаталоги и файлы", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 800, 200, 200, 30, hwnd, (HMENU)2072, NULL, NULL);
	CreateWindowEx(0, L"button", L"Только для подкаталогов", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 800, 223, 200, 30, hwnd, (HMENU)2073, NULL, NULL);
	
	CreateWindowEx(0, L"button", L"Только для файлов", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 1000, 177, 200, 30, hwnd, (HMENU)2074, NULL, NULL);

	CreateWindowEx(0, L"Static", L"Разрешения:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 400, 250, 100, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	
	         CreateWindowEx(0, L"button", L"Создание каталогов", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 400, 270, 200, 30, hwnd, (HMENU)2050, NULL, NULL);
                  CreateWindowEx(0, L"button", L"Полный доступ", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 400, 300, 200, 30, hwnd, (HMENU)2051, NULL, NULL);
	            CreateWindowEx(0, L"button", L"Дозапись данных", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 400, 330, 200, 30, hwnd, (HMENU)2052, NULL, NULL);
    CreateWindowEx(0, L"button", L"Удаление файлов и каталогов", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 400, 360, 300, 30, hwnd, (HMENU)2053, NULL, NULL);
	          CreateWindowEx(0, L"button", L"Выполнение файлов", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 400, 390, 200, 30, hwnd, (HMENU)2054, NULL, NULL);
	          CreateWindowEx(0, L"button", L"Просмотр каталога", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 400, 420, 200, 30, hwnd, (HMENU)2055, NULL, NULL);
	           CreateWindowEx(0, L"button", L"Чтение атрибутов", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 400, 450, 200, 30, hwnd, (HMENU)2056, NULL, NULL);

	              CreateWindowEx(0, L"button", L"Чтение данных", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 700, 250, 200, 30, hwnd, (HMENU)2057, NULL, NULL);
	      CreateWindowEx(0, L"button", L"Чтение доп. атрибутов", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 700, 280, 200, 30, hwnd, (HMENU)2058, NULL, NULL);
              CreateWindowEx(0, L"button", L"Траверс каталогов", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 700, 310, 200, 30, hwnd, (HMENU)2059, NULL, NULL);
	           CreateWindowEx(0, L"button", L"Запись атрибутов", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 700, 340, 200, 30, hwnd, (HMENU)2060, NULL, NULL);
	              CreateWindowEx(0, L"button", L"Запись данных", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 700, 370, 200, 30, hwnd, (HMENU)2061, NULL, NULL);
	      CreateWindowEx(0, L"button", L"Запись доп. атрибутов", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 700, 400, 200, 30, hwnd, (HMENU)2062, NULL, NULL);
	          CreateWindowEx(0, L"button", L"Чтение разрешений", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 700, 430, 200, 30, hwnd, (HMENU)2063, NULL, NULL);

	           CreateWindowEx(0, L"button", L"Смена разрешений", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 1000, 250, 200, 30, hwnd, (HMENU)2064, NULL, NULL);
	            CreateWindowEx(0, L"button", L"Смена владельца", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 1000, 280, 200, 30, hwnd, (HMENU)2065, NULL, NULL);

				//list4 = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 300, 10, 250, 420, hwnd, (HMENU)IDC_LIST4, lpCreateStruct->hInstance, NULL);
				
				
				//list4 = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_BORDER | LBS_STANDARD | WS_VISIBLE, 600, 1200, 250, 420, hwnd, (HMENU)IDC_LIST4, lpCreateStruct->hInstance, NULL);
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
	HINSTANCE hInstance = GetWindowInstance(hwnd);
	switch (id)
	{
	case IDR_Save2:
	{
		int cch = GetWindowTextLength(GetDlgItem(hwnd, IDR_EditName2));
		if (cch > 0)
		{
			PSID pSid = NULL;

			LPTSTR lpszName = new TCHAR[cch + 1]; 											 
			GetDlgItemText(hwnd, IDR_EditName2, lpszName, cch + 1);

			GetAccountSID(lpszName, &pSid);
			delete[]lpszName, lpszName = NULL;

			if (NULL != pSid)
			{
				GetAccountName(pSid, &lpszName);
				SetDlgItemText(hwnd, IDR_EditName2, lpszName);
				if (NULL != lpszName) LocalFree(lpszName);
				LocalFree(pSid);
			}
			else
			{
				MessageBox(hwnd, TEXT("Такой учетной записи нет!"), NULL, MB_OK | MB_ICONERROR);
				SetDlgItemText(hwnd, IDR_EditName2, NULL);
			}
		}
	}
	break;
	case ID_AddACE:
	{
		int cch = GetWindowTextLength(GetDlgItem(hwnd, IDR_EditName));
		if (cch > 0)
		{
			cch = GetWindowTextLength(GetDlgItem(hwnd, IDR_EditName2));
			if (cch > 0) 
			{
				PSID pSid = NULL;

				LPTSTR lpszName = new TCHAR[cch + 1];							
				GetDlgItemText(hwnd, IDR_EditName2, lpszName, cch + 1);
				GetAccountSID(lpszName, &pSid);
				delete[] lpszName, lpszName = NULL;
				if (NULL != pSid)
				{
					EXPLICIT_ACCESS ea;

					ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
					ea.Trustee.ptstrName = (LPTSTR)pSid;
					ea.grfAccessPermissions = 0;

					if (IsDlgButtonChecked(hwnd, 2050) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_ADD_SUBDIRECTORY;
					if (IsDlgButtonChecked(hwnd, 2051) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_ALL_ACCESS;
					if (IsDlgButtonChecked(hwnd, 2052) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_APPEND_DATA;
					if (IsDlgButtonChecked(hwnd, 2053) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_DELETE_CHILD;
					if (IsDlgButtonChecked(hwnd, 2054) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_EXECUTE;
					if (IsDlgButtonChecked(hwnd, 2055) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_LIST_DIRECTORY;
					if (IsDlgButtonChecked(hwnd, 2056) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_READ_ATTRIBUTES;
					if (IsDlgButtonChecked(hwnd, 2057) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_READ_DATA;
					if (IsDlgButtonChecked(hwnd, 2058) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_READ_EA;
					if (IsDlgButtonChecked(hwnd, 2059) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_TRAVERSE;
					if (IsDlgButtonChecked(hwnd, 2060) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_WRITE_ATTRIBUTES;
					if (IsDlgButtonChecked(hwnd, 2061) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_WRITE_DATA;
					if (IsDlgButtonChecked(hwnd, 2062) == BST_CHECKED)
						ea.grfAccessPermissions |= FILE_WRITE_EA;
					if (IsDlgButtonChecked(hwnd, 2063) == BST_CHECKED)
						ea.grfAccessPermissions |= READ_CONTROL;
					if (IsDlgButtonChecked(hwnd, 2064) == BST_CHECKED)
						ea.grfAccessPermissions |= WRITE_DAC;
					if (IsDlgButtonChecked(hwnd, 2065) == BST_CHECKED)
						ea.grfAccessPermissions |= WRITE_OWNER;

					if(IsDlgButtonChecked(hwnd,2066))
		        	ea.grfAccessMode = DENY_ACCESS;
					else ea.grfAccessMode = GRANT_ACCESS;

					if (IsDlgButtonChecked(hwnd, 2068))
						ea.grfInheritance = NO_INHERITANCE;
					else if(IsDlgButtonChecked(hwnd, 2069))
						ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
					else if (IsDlgButtonChecked(hwnd, 2070))
						ea.grfInheritance = SUB_CONTAINERS_ONLY_INHERIT;
					else if (IsDlgButtonChecked(hwnd, 2071))
						ea.grfInheritance = SUB_OBJECTS_ONLY_INHERIT;
					else if (IsDlgButtonChecked(hwnd, 2072))
						ea.grfInheritance = INHERIT_ONLY | SUB_CONTAINERS_AND_OBJECTS_INHERIT;
					else if (IsDlgButtonChecked(hwnd, 2073))
						ea.grfInheritance = INHERIT_ONLY | SUB_CONTAINERS_ONLY_INHERIT;
					else if (IsDlgButtonChecked(hwnd, 2074))
						ea.grfInheritance = INHERIT_ONLY | SUB_OBJECTS_ONLY_INHERIT;

					if (0 != ea.grfAccessPermissions)
					{
						BOOL bRet = SetFileSecurityInfo(szPath, NULL, 1, &ea, TRUE);
						if (FALSE != bRet)
						{
							MessageBox(hwnd, TEXT("ACE успешно добавлено!"), NULL, MB_OK);
						} 
					} 
					LocalFree(pSid);
				}
			}
			else MessageBox(hwnd, TEXT("Введите существующего пользователя!"), NULL, MB_OK | MB_ICONERROR);
		} else MessageBox(hwnd, TEXT("Выберите файл / каталог!"), NULL, MB_OK | MB_ICONERROR);
	}
	break;
	case ID_DeleteACE: 
	{
			for (;;)
			{
				int iItem = ListView_GetNextItem(list, -1, LVNI_SELECTED);
				if (iItem == -1) break; 
				DeleteEntryFromDalc(pSD, iItem);
				ListView_DeleteItem(list, iItem);
			}
			SetFileSecurity(szPath, DACL_SECURITY_INFORMATION, pSD);	
	}
	break;
	case ID_Update: 
	{
		BOOL bRet = GetFileSecurityDescriptor(szPath, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &pSD);
		if (FALSE != bRet)
		{
			ListView_DeleteAllItems(list);
			ULONG uCount = 0; // количество элементов в массиве ACE
			PEXPLICIT_ACCESS pEA = NULL; // массив ACE
			BOOL bRet = GetEntriesFromDacl(pSD, &uCount, &pEA);
			if (FALSE != bRet)
			{
				for (ULONG i = 0; i < uCount; ++i)
				{
					LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM };

					lvItem.iItem = (int)i;

					// определим права доступа
					lvItem.lParam = (LPARAM)pEA[i].grfAccessPermissions;

					// определим тип ACE
					switch (pEA[i].grfAccessMode)
					{
					case GRANT_ACCESS:
						lvItem.pszText = (LPTSTR)TEXT("Разрешить");
						break;
					case DENY_ACCESS:
						lvItem.pszText = (LPTSTR)TEXT("Запретить");
						break;
					} // switch

					  // добавляем новый элемент в список просмотра DACL
					int iItem = ListView_InsertItem(list, &lvItem);
					if (iItem == -1) continue; // не удалось добавить новый элемент

											   // определим имя учетной записи
					if (TRUSTEE_IS_SID == pEA[i].Trustee.TrusteeForm)
					{
						LPTSTR lpszName = NULL; // имя учетной записи

												// получим имя учетной записи
						GetAccountName(pEA[i].Trustee.ptstrName, &lpszName);

						if (NULL != lpszName)
						{
							// копируем имя учетной записи в ячейку списка просмотра DACL
							ListView_SetItemText(list, iItem, 1, lpszName);
							// освобождаем выделенную память
							LocalFree(lpszName);
						} // if
					} // if


					DWORD grfInheritance = pEA[i].grfInheritance & (~INHERIT_NO_PROPAGATE);


					if (grfInheritance == NO_INHERITANCE)
					{
						ListView_SetItemText(list, iItem, 2, (LPTSTR)TEXT("Только для этого каталога"));
					}
					else
						if (grfInheritance == SUB_CONTAINERS_AND_OBJECTS_INHERIT)
						{
							ListView_SetItemText(list, iItem, 2, (LPTSTR)TEXT("Для этого каталога, его подкаталогов и файлов"));
						}
						else
							if (grfInheritance == SUB_CONTAINERS_ONLY_INHERIT)
							{
								ListView_SetItemText(list, iItem, 2, (LPTSTR)TEXT("Для этого каталога и его подкаталогов"));
							}
							else
								if (grfInheritance == SUB_OBJECTS_ONLY_INHERIT)
								{
									ListView_SetItemText(list, iItem, 2, (LPTSTR)TEXT("Для этого каталога и его файлов"));
								}
								else
									if (grfInheritance == INHERIT_ONLY | SUB_CONTAINERS_AND_OBJECTS_INHERIT)
									{
										ListView_SetItemText(list, iItem, 2, (LPTSTR)TEXT("Только для подкаталогов и файлов"));
									}
									else
										if (grfInheritance == INHERIT_ONLY | SUB_CONTAINERS_ONLY_INHERIT)
										{
											ListView_SetItemText(list, iItem, 2, (LPTSTR)TEXT("Только для подкаталогов"));
										}
										else
											if (grfInheritance == INHERIT_ONLY | SUB_OBJECTS_ONLY_INHERIT)
											{
												ListView_SetItemText(list, iItem, 2, (LPTSTR)TEXT("Только для файлов"));
											}

				}
			}

				 
			
		
			LocalFree(pEA);
		}
	}
	break;
	case IDR_Open:
	{
		TCHAR FolderPath[250] = TEXT("");
		 GetWindowText(EditFolder, FolderPath, _countof(FolderPath));
		
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
					SetWindowText(hlabelPath, szPath);
					LPTSTR lpszName = NULL;
					GetFileSecurityDescriptor(szPath, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &pSD);
					GetOwnerName(pSD, &lpszName);
					Edit_SetText(Name, lpszName);
				}
			}
	}
	break;
	case IDR_Save: 
	{
		TCHAR NewName[UNLEN + 1];
		Edit_GetText(Name, NewName, _countof(NewName));
		BOOL bRet = SetFileSecurityInfo(szPath, NewName, 0, NULL, FALSE);

		if (FALSE != bRet) Edit_SetText(Name, NewName);
		else {
			LPTSTR lpszName = NULL;
			GetFileSecurityDescriptor(szPath, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &pSD);
			GetOwnerName(pSD, &lpszName);
			Edit_SetText(Name, lpszName);
			MessageBox(hwnd, TEXT("Ошибка изменения имени владельца!"), NULL, MB_OK | MB_ICONERROR);
		}
	}
	break;
	case IDR_OpenFolder:
	{
		DialogBox(hInstance, MAKEINTRESOURCE(IDC_OpenFolder), hwnd, DialogProc2);
	}
	break;


	}//switch
}
BOOL GetOwnerName(PSECURITY_DESCRIPTOR pSD, LPTSTR *lpName)
{
	PSID pSid;
	BOOL bDefaulted;

	// получаем SID владельца
	BOOL bRet = GetSecurityDescriptorOwner(pSD, &pSid, &bDefaulted);

	if (FALSE != bRet)
	{
		// определяем имя учетной записи владельца
		bRet = GetAccountName(pSid, lpName);
	} // if

	return bRet;
} // GetOwnerName
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
void Dialog_OnClose(HWND hwnd)
{
	if (hwnd == hDlg) DestroyWindow(hwnd);
	else EndDialog(hwnd, IDCLOSE);
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
		HANDLE hToken = OpenUserToken(TEXT("Тореар"), TEXT("."), TEXT("123"),
			LOGON32_LOGON_INTERACTIVE, TOKEN_QUERY | TOKEN_IMPERSONATE, NULL, TokenImpersonation, SecurityImpersonation);
		
			if (NULL != hToken)
			{

				TCHAR szNewFileName[MAX_PATH];
				GetDlgItemText(hwnd, IDC_EDIT1, szNewFileName, _countof(szNewFileName));

				StringCchCopy(szPath, _countof(szPath), TEXT(""));
				StringCchCopy(szPath, _countof(szPath), szNewFileName);

				
					SetWindowText(hlabelPath, szPath);
					LPTSTR lpszName = NULL;
					GetFileSecurityDescriptor(szPath, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &pSD);

					//	GetOwnerName(pSD, &lpszName);
						Edit_SetText(Name, lpszName);



				 
		}
		EndDialog(hwnd, IDCANCEL);
	
	}
	break;
	case IDCANCEL:
		EndDialog(hwnd, IDCANCEL);
		break;
	}
}
BOOL GetFileSecurityDescriptor(LPCTSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR *ppSD)
{
	DWORD cb = 0;

	// определим размер дескриптора безопасности
	GetFileSecurity(lpFileName, RequestedInformation, NULL, 0, &cb);

	// выделим память для дескриптора безопасности
	PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LMEM_FIXED, cb);
	if (NULL == pSD) return FALSE;

	// получим дескриптор безопасности
	BOOL bRet = GetFileSecurity(lpFileName, RequestedInformation, pSD, cb, &cb);

	if (FALSE != bRet)
	{
		*ppSD = pSD; // возвращаем полученный дескриптор безопасности
	} // if
	else
	{
		LocalFree(pSD); // освобождаем выделенную память
	} // else

	return bRet;
} // GetFileSecurityDescriptor
BOOL SetFileSecurityInfo(LPCTSTR lpFileName, LPCTSTR lpAccountOwner, ULONG cCountOfEntries, PEXPLICIT_ACCESS pListOfEntries, BOOL bMergeEntries)
{
	SECURITY_DESCRIPTOR sd; // дескриптор безопасности

	PSID pOwner = NULL; // буфер для SID владельца
	PACL pNewDacl = NULL; // буфер для нового DACL

						  // инициализируем дескриптор безопасности
	BOOL bRet = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

	// ///// //

	if (FALSE != bRet && NULL != lpAccountOwner)
	{
		// получаем SID указанной учетной записи
		bRet = GetAccountSID(lpAccountOwner, &pOwner);

		// связываем полученный SID с дескриптором безопасности
		if (FALSE != bRet)
			bRet = SetSecurityDescriptorOwner(&sd, pOwner, FALSE);
	} // if

	  // ///// //

	if (FALSE != bRet && cCountOfEntries > 0 && NULL != pListOfEntries)
	{
		PSECURITY_DESCRIPTOR pOldSD = NULL;
		PACL pOldDacl = NULL; // указатель на буфер для DACL

		BOOL bDaclDefaulted = FALSE;

		if (FALSE != bMergeEntries)
		{
			bRet = GetFileSecurityDescriptor(lpFileName, DACL_SECURITY_INFORMATION, &pOldSD);

			if (FALSE != bRet)
			{
				BOOL bDaclPresent;

				// получаем DACL
				GetSecurityDescriptorDacl(pOldSD, &bDaclPresent, &pOldDacl, &bDaclDefaulted);
			} // if
		} // if

		  // создаем новый DACL
		DWORD dwResult = SetEntriesInAcl(cCountOfEntries, pListOfEntries, pOldDacl, &pNewDacl);
		bRet = (ERROR_SUCCESS == dwResult) ? TRUE : FALSE;

		// связываем новый DACL с дескриптором безопасности
		if (FALSE != bRet)
			bRet = SetSecurityDescriptorDacl(&sd, TRUE, pNewDacl, bDaclDefaulted);

		// освобождаем память
		if (NULL != pOldSD) LocalFree(pOldSD);
	} // if

	  // ///// //

	if (FALSE != bRet)
	{
		SECURITY_INFORMATION si = 0;
		if (NULL != pOwner) si |= OWNER_SECURITY_INFORMATION;
		if (NULL != pNewDacl) si |= DACL_SECURITY_INFORMATION;

		// изменяем дескриптор безопасности
		bRet = SetFileSecurity(lpFileName, si, &sd);
	} // if

	  // освобождаем память, выделенную под буфер для SID
	if (NULL != pOwner) LocalFree(pOwner);
	// освобождаем память, выделенную под буфер для DACL
	if (NULL != pNewDacl) LocalFree(pNewDacl);

	return bRet;
} // SetFileSecurityInfo
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
BOOL GetEntriesFromDacl(PSECURITY_DESCRIPTOR pSD, PULONG pcCountOfEntries, PEXPLICIT_ACCESS *pListOfEntries)
{
	PACL pDacl;
	BOOL bDaclPresent, bDaclDefaulted;

	// получаем DACL
	BOOL bRet = GetSecurityDescriptorDacl(pSD, &bDaclPresent, &pDacl, &bDaclDefaulted);

	if (FALSE != bRet && FALSE != bDaclPresent)
	{
		// извлекаем элементы из DACL
		DWORD dwResult = GetExplicitEntriesFromAcl(pDacl, pcCountOfEntries, pListOfEntries);
		bRet = (ERROR_SUCCESS == dwResult) ? TRUE : FALSE;
	} // if
	else
	{
		*pcCountOfEntries = 0; // возвращаем 0 элементов
	} // else

	return bRet;
} // GetExplicitEntriesFromDacl
BOOL DeleteEntryFromDalc(PSECURITY_DESCRIPTOR pSD, DWORD dwIndex)
{
	PACL pDacl;
	BOOL bDaclPresent, bDaclDefaulted;

	// получаем DACL
	BOOL bRet = GetSecurityDescriptorDacl(pSD, &bDaclPresent, &pDacl, &bDaclDefaulted);

	if (FALSE != bRet)
	{
		// удаляем элемент из DACL
		bRet = DeleteAce(pDacl, dwIndex);
	} // if

	return bRet;
} // DeleteEntryFromDalc
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