#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "resource.h"
#include <Richedit.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")
#define IDC_BUTTON1 2002
#define IDC_LIST  2001
#define TestButton 2444
#define ID_C1 2445
#define ID_C2 2446
#define ID_LV 2447
#define ID_SPIN 2448
#define ID_edit 2449
const wchar_t* header[3] = { L"Item", L"Subitem0", L"Subitem2" };
const wchar_t* item[3][3] = { L"00", L"01", L"02", L"10", L"11", L"12", L"20", L"21", L"22" };
int const colNum = 3;
int const itemNum = 3;
int const textMaxLen = 10;
int Col2 = 0;
static int i = 60;
bool startTimer;
wchar_t Buffer[50];
wchar_t BufferXY[50];
HDC hdc;
PAINTSTRUCT ps;
#define WM_ADDITEM  WM_USER + 1
HWND hWnd = NULL;
HWND hEdit= NULL;
HWND hSpin = NULL;
HWND hlabel = NULL;
HWND hX= NULL;
HWND hY = NULL;
HWND hX2 = NULL;
HWND hY2 = NULL; HWND hCur = NULL;
HWND list;
HACCEL hAccel = NULL;
HWND hDlg = NULL;
HWND hFindDlg = NULL;
TCHAR szBuffer[100] = TEXT("");
TCHAR szBuffer2[100] = TEXT("");
FINDREPLACE findDlg;
UINT uFindMsgString = 0;
HFONT hFont = NULL;	
static int wheelDelta = 0;
LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnIdle(HWND hwnd);
BOOL PreTranslateMessage(LPMSG lpMsg);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hwnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnAddItem(HWND hwnd);
void OnFindMsgString(HWND hwnd, LPFINDREPLACE lpFindReplace);
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Dialog_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Dialog_OnClose(HWND hwnd);
void Dialog_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = MyWindowProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = TEXT("MyWindowClass");
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (0 == RegisterClassEx(&wcex))
	{
		return -1;
	}
	LoadLibrary(TEXT("ComCtl32.dll")); 
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
   hWnd = CreateWindowEx(0,TEXT("MyWindowClass"), TEXT("Lab2"),WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,0,800,500,NULL, NULL, hInstance, NULL);
	if (NULL == hWnd)
	{
		return -1;
	}
	ShowWindow(hWnd, nCmdShow);
	MSG  msg;
	BOOL bRet;
	for (;;)
	{
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			OnIdle(hWnd);
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
	return (int)msg.wParam;
} 
LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
	case WM_TIMER:
		if (wParam == 777 && startTimer == true && i > 0)
		{
			i += -1;
			InvalidateRect(hWnd, NULL, true);
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		wsprintfW(Buffer, L"%d", i);
		SetWindowText(hlabel, Buffer);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_ADDITEM:
		OnAddItem(hWnd);
		return 0;
	case WM_CHAR:
	{
		TCHAR Buf[10] = TEXT("");
		TCHAR BufS[10]; TCHAR BufS2[10]; TCHAR BufS3[10]; 
		Buf[0] = (wchar_t)wParam;
		
		LVFINDINFO lvfiInfo;
		lvfiInfo.flags = LVFI_STRING;
		lvfiInfo.psz = Buf;
		int iStart = -1;

		int iIndex = SendMessage(list, LVM_FINDITEM, (WPARAM)(int)iStart, (LPARAM)&lvfiInfo);
		if (iIndex != -1)
		{
			ListView_SetItemState(list, iIndex, LVIS_SELECTED, LVIS_SELECTED);
			SetFocus(list);
			ListView_SetItemText(list, iIndex, 0, Buf);
		}	
		//LPNMLISTVIEW pnmLV = (LPNMLISTVIEW)lParam;
		//char buf[10] = "??";

		//        ListView_GetItemText(BufS->hwndFrom, pnmLV->iItem, 0, buf, 10);
		//		int iIndex = SendMessage(list, LVM_FINDITEM, (WPARAM)(int)iStart, (LPARAM)&lvfiInfo);
		//		if (iIndex != -1)
		//		{
		//		ListView_SetItemState(list, iIndex, LVIS_SELECTED, LVIS_SELECTED);
		//		SetFocus(list);
		//		ListView_SetItemText(list, iIndex, 0, Buf);
		//		}
		//		for(int i =0;i< Col2;i++)
		//		{
		//			if(Buf == ListView_)
		//		}
	}
		return 0;
	case	WM_KEYUP:
		if (wParam == VK_RETURN) {
			startTimer = true;
			i = 61;
			SetTimer(hWnd, 777, 1000, NULL);
		}
		if (wParam == VK_PAUSE) {
			startTimer = false;
		
		}
		if (wParam == VK_SPACE) {
			startTimer = false;
				startTimer = true;
		}
		return 0;

	case WM_LBUTTONDBLCLK:
	{
			int mbResult = MessageBox(hWnd, TEXT("Удалить все элементы?"), TEXT("SampleWin32"),MB_YESNO | MB_ICONQUESTION);
			if (mbResult == IDYES) { ListView_DeleteAllItems(list); Col2 = 0; }
	}
		return 0;
	case	WM_LBUTTONDOWN:
	{
			ReleaseCapture();
			SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		
	}
		return 0;
	case	WM_LBUTTONUP:
	{
		POINT pt;
		TCHAR Cur[32];

		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		wsprintf(Cur, L"x: %d y: %d", pt.x, pt.y);
		SetWindowText(hCur, Cur);
	}
	return 0;
	case	WM_MOUSEWHEEL:
	{
		RECT Rect;
		GetWindowRect(hWnd, &Rect);
		
		wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);

		for (; wheelDelta > WHEEL_DELTA; wheelDelta -= WHEEL_DELTA)
			MoveWindow(hWnd, Rect.left, Rect.top + wheelDelta, 800, 500, NULL);
		for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA)
			MoveWindow(hWnd, Rect.left, Rect.top + wheelDelta, 800, 500, NULL);
		ShowWindow(hWnd, 1);
		UpdateWindow(hWnd);
	}
	return 0;
	case	WM_MOVING:
	{
		RECT Rect;
		GetWindowRect(hWnd, &Rect);

		/*wsprintfW(BufferXY, L"%d", Rect.top);
		SetWindowText(hX, BufferXY);
		wsprintfW(BufferXY, L"%d", Rect.bottom);
		SetWindowText(hY2, BufferXY);
		wsprintfW(BufferXY, L"%d", Rect.right);
		SetWindowText(hY, BufferXY);*/
		/*wsprintfW(BufferXY, L"%d", Rect.left);
		SetWindowText(hX2, BufferXY);*/
		SetDlgItemInt(hWnd, 1056, Rect.top, true);
		SetDlgItemInt(hWnd, 1057, Rect.bottom, true);
		SetDlgItemInt(hWnd, 1058, Rect.right, true);
		SetDlgItemInt(hWnd,1055 , Rect.left,true);
		//setdlgItemint

	}
	return 0;
	}
	if (uFindMsgString == uMsg)
	{
		OnFindMsgString(hWnd, (LPFINDREPLACE)lParam);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
void OnIdle(HWND hwnd){} 
  BOOL PreTranslateMessage(LPMSG lpMsg)
  {
  BOOL bRet = TRUE;
  	if (!TranslateAccelerator(hWnd, hAccel, lpMsg))
  	{
  	bRet = IsDialogMessage(hDlg, lpMsg);
  
  		if (FALSE == bRet)
  		bRet = IsDialogMessage(hFindDlg, lpMsg);
  	}
  	return bRet;
  } 
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	CreateWindowEx(0, TEXT("Button"), TEXT("Добавить запись"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 10, 200, 40, hwnd, (HMENU)ID_NEW_RECORD, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Добавить неск. записей"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 55, 200, 40, hwnd, (HMENU)ID_NEW_RECORD2, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Удалить запись"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 100, 200, 40, hwnd, (HMENU)ID_DEL_RECORD, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Замена"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 145, 200, 40, hwnd, (HMENU)ID_REPLACE, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Печать"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 210, 200, 40, hwnd, (HMENU)ID_PRINT, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0,L"button", L"Сообщение 1", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 270, 320, 150, 30, hwnd, (HMENU)ID_C1, NULL, NULL);
	CreateWindowEx(0, L"button", L"Сообщение 2", WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE, 270, 350, 150, 30, hwnd, (HMENU)ID_C2, NULL, NULL);
	
	list = CreateWindowEx(0, WC_LISTVIEW, L"", WS_VISIBLE | WS_CHILD |
		WS_BORDER | LVS_REPORT | LVS_EDITLABELS , 10, 10, 250, 360, hwnd, (HMENU)ID_LV, lpCreateStruct->hInstance, NULL);
	LVCOLUMN lvc;
	wchar_t *str = new wchar_t[100];
	wcscpy(str, L"Поле1");
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = str;
	lvc.cx = 125;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(list, 0, &lvc);
	wcscpy(str, L"Поле2");
	lvc.iSubItem = 1;
	ListView_InsertColumn(list, 1, &lvc);
	hEdit = CreateWindowEx(0,L"edit", L"0", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, 270, 280, 210, 23, hwnd, (HMENU)ID_edit, lpCreateStruct->hInstance, NULL);
	
	/*hSpin = CreateWindowEx(0, L"msctls_updown32", L"0", WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | BS_TEXT |UDS_AUTOBUDDY| UDS_SETBUDDYINT, 400, 280, 200, 30, hwnd, (HMENU)ID_SPIN, lpCreateStruct->hInstance, NULL);
	SendMessage(hSpin, UDM_SETBUDDY, (WPARAM)hEdit, (LPARAM)0L);
	
	SendMessage(hSpin, UDM_SETRANGE32, (WPARAM)0, (LPARAM)50);*/
	
	//hSpin = CreateUpDownControl(WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | BS_TEXT | UDS_AUTOBUDDY | UDS_SETBUDDYINT, 400, 280, 200, 30, hwnd, 2448, lpCreateStruct->hInstance, NULL, 10, 0, 0);
	
	CreateUpDownControl(
		WS_CHILD | WS_BORDER | WS_VISIBLE |
		UDS_WRAP | UDS_ARROWKEYS | UDS_ALIGNRIGHT |
		UDS_SETBUDDYINT,
		400, 280, 200, 30, hwnd, 1080, lpCreateStruct->hInstance,
		hEdit, 10, 1, 1);
	//hSpin = CreateUpDownControl(0, L"msctls_updown32", L"0", WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | BS_TEXT | UDS_AUTOBUDDY | UDS_SETBUDDYINT, 400, 280, 200, 30, hwnd, (HMENU)ID_SPIN, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Запись"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 390, 250, 40, hwnd, (HMENU)TestButton, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"msctls_hotkey32", L"", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 270, 390, 200, 40, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Static", L"Управление таймером", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 590, 10, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Static", L"Таймер:", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 590, 40, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	hlabel = CreateWindowEx(0, L"Static", L"0", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 660, 40, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, L"Static", L"Старт - Enter", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 590, 70, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Static", L"Пауза - Pause", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 590, 100, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Static", L"Продолжить - Пробел", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 590, 130, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	
	CreateWindowEx(0, L"Static", L"Курсор =", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 620, 270, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Static", L"Top =", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 640, 310, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Static", L"Bottom =", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 640, 340, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Static", L"Right =", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 640, 370, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, L"Static", L"Left =", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 640, 400, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	hCur = CreateWindowEx(0, L"Static", L"0", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 690, 270, 200, 30, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	hX = CreateWindowEx(0, L"Static", L"x", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 710, 310, 200, 30, hwnd, (HMENU)1056, lpCreateStruct->hInstance, NULL);
	hY = CreateWindowEx(0, L"Static", L"y", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 710, 340, 200, 30, hwnd, (HMENU)1057, lpCreateStruct->hInstance, NULL);
	hX2 = CreateWindowEx(0, L"Static", L"x", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 710, 370, 200, 30, hwnd, (HMENU)1058, lpCreateStruct->hInstance, NULL);
	hY2 = CreateWindowEx(0, L"Static", L"y", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_TEXT | BS_VCENTER, 710, 400, 200, 30, hwnd, (HMENU)1055, lpCreateStruct->hInstance, NULL);
	return TRUE;
} 
void OnDestroy(HWND hwnd)
{ 
	if (NULL != hFont)
		DeleteObject(hFont), hFont = NULL;
	PostQuitMessage(0);
}
void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if (state != SIZE_MINIMIZED)
	{
		HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST);
		MoveWindow(hwndCtl, 10, 10, 250, cy - 20, TRUE);
	} 
} 
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HINSTANCE hInstance = GetWindowInstance(hwnd);
	switch (id)
	{
	case ID_NEW_RECORD: 
	{
		int nDlgResult = DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc);
		if (IDOK == nDlgResult)
		{
			SendMessage(hwnd, WM_ADDITEM, 0, 0);
		}  
	}
	break;
	case ID_NEW_RECORD2: 
		if (IsWindow(hDlg) == FALSE)
		{
			hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(
				IDD_DIALOG1), hwnd, DialogProc);
			ShowWindow(hDlg, SW_SHOW);
		} 
		break;
	case ID_DEL_RECORD: 
	{
		int item = ListView_GetNextItem(list, -1, LVNI_FOCUSED | LVNI_SELECTED);
		if (item != -1)
		{
			int mbResult = MessageBox(hWnd, TEXT("Удалить выбранный элемент?"), TEXT("SampleWin32"),MB_YESNO | MB_ICONQUESTION);
			if (mbResult == IDYES)
			{

				ListView_DeleteItem(list, item); Col2--;
			}
		}
	}
	break;
	case ID_FIND_RECORD:
		if (0 == uFindMsgString)
		{
			uFindMsgString = RegisterWindowMessage(FINDMSGSTRING);
		} 
		if (IsWindow(hFindDlg) == FALSE)
		{
			findDlg.lStructSize = sizeof(FINDREPLACE);
			findDlg.hInstance = hInstance;
			findDlg.hwndOwner = hwnd;
			findDlg.lpstrFindWhat = szBuffer;
			findDlg.wFindWhatLen = _countof(szBuffer);
			hFindDlg = FindText(&findDlg);
		} 
		break;
	case TestButton:
		{
		BOOL ischk = (BOOL)SendDlgItemMessage(hwnd, ID_C1, BM_GETCHECK, 0, 0);
		BOOL ischk2 = (BOOL)SendDlgItemMessage(hwnd, ID_C2, BM_GETCHECK, 0, 0);
		wchar_t *str = new wchar_t[100];
		wcscpy(str, L"Пусто");
		if (ischk) wcscpy(str, L"Привет!");
		if (ischk2) wcscpy(str, L"Как дела?");
		if (ischk2 && ischk) wcscpy(str, L"Привет! Как дела?");
		TCHAR tmp[MAX_PATH];
	    GetWindowText(hEdit, tmp, GetWindowTextLength(hWnd) + 1);
		LVITEM LvItem;
		int iItem;
		iItem = SendMessage(GetDlgItem(hWnd, ID_LV), LVM_GETITEMCOUNT, 0, 0);
		LvItem.mask = LVIF_TEXT;
		LvItem.cchTextMax = 10;
		LvItem.iItem = iItem;
		LvItem.iSubItem = 0;
		LvItem.pszText = str;
		ListView_InsertItem(GetDlgItem(hWnd, ID_LV), &LvItem);
		ListView_SetItemText(GetDlgItem(hWnd, ID_LV), Col2, 1, tmp);
		Col2++;
		}
		break;
	case ID_REPLACE:
		if (0 == uFindMsgString)
		{
			uFindMsgString = RegisterWindowMessage(FINDMSGSTRING);
		}
		if (IsWindow(hFindDlg) == FALSE)
		{
			
			findDlg.lStructSize = sizeof(FINDREPLACE);
			findDlg.hInstance = hInstance;
			findDlg.hwndOwner = hwnd;
			findDlg.lpstrReplaceWith = szBuffer2;
			findDlg.wReplaceWithLen= _countof(szBuffer2);
			findDlg.lpstrFindWhat = szBuffer;
			findDlg.wFindWhatLen = _countof(szBuffer);
			hFindDlg = ReplaceText(&findDlg);
	    }
	break;
	case ID_PRINT:
	{
		PRINTDLG pd = { sizeof(PRINTDLG) };
		pd.Flags = PD_ALLPAGES | PD_USEDEVMODECOPIESANDCOLLATE;
		pd.hwndOwner = hWnd; // дескриптор окна-владельца 
		pd.nCopies = 1; // число копий 
		pd.nFromPage = 15; // начальная страница 
		pd.nToPage = 48; // конечная страница 
		pd.nMinPage = 1;  // минимальное значение диапазона страниц 
		pd.nMaxPage = 65535; // максимальное значение диапазона страниц 
		
		if (PrintDlg(&pd) != FALSE)	
		{
			//HWND hLV = GetDlgItem(hWnd, ID_LV);
			//DWORD dwByte; TCHAR szN[2];
			//szN[0] = '\r'; szN[1] = '\n';
			//HANDLE hFile = CreateFile(_T("Print.txt"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			//if (INVALID_HANDLE_VALUE == hFile)
			//	MessageBox(hWnd, _T("Ошибка печати"), _T("Возникла ошибка"), MB_OK);
			//else
			//{
			//	MessageBox(hWnd, _T(" успешно!"), _T("Печать"), MB_OK | MB_ICONINFORMATION);
			//	int iItemCount = ListView_GetItemCount(hLV);
			//	for (int i = 0; i < iItemCount; i++)
			//	{
			//		TCHAR szTemp[100]; 
			//		memset(szTemp, 0, sizeof(szTemp) / sizeof(TCHAR));
			//		ListView_GetItemText(hLV, i, 0, szTemp, sizeof(szTemp)); // Первый столбец
			//		WriteFile(hFile, szTemp, lstrlen(szTemp) * sizeof(TCHAR), &dwByte, NULL);

			//		WriteFile(hFile, L"  ", 2, &dwByte, NULL);

			//		memset(szTemp, 0, sizeof(szTemp) / sizeof(TCHAR));
			//		ListView_GetItemText(hLV, i, 1, szTemp, sizeof(szTemp)); // Второй столбец
			//		WriteFile(hFile, szTemp, lstrlen(szTemp) * sizeof(TCHAR), &dwByte, NULL);
			//		
			//		WriteFile(hFile, szN, 3*sizeof(TCHAR), &dwByte, NULL);
			//	}
			//	CloseHandle(hFile);
			//}
		
			WCHAR  text[500];
			_stprintf(text, TEXT("Кол-во копий: %d\nНачальная страница = %d\nКонечная страница = %d\nМинимальное значение диапазона = %d\nМаксимальное значение диапазона = %d"), pd.nCopies, pd.nFromPage, pd.nToPage, pd.nMinPage, pd.nMaxPage);
			MessageBox(hWnd, text, TEXT("Печать"), MB_OK | MB_ICONQUESTION);
		} 
	} break;
	} 
}
void OnAddItem(HWND hwnd)
{
	LVITEM LvItem;
	int iItem = SendMessage(GetDlgItem(hWnd, ID_LV), LVM_GETITEMCOUNT, 0, 0);
	
	LvItem.mask = LVIF_TEXT;
	LvItem.cchTextMax = 10;
	LvItem.iItem = iItem;
	LvItem.iSubItem = 0;
	LvItem.pszText = szBuffer;
	
	ListView_InsertItem(GetDlgItem(hWnd, ID_LV), &LvItem);
	
	TCHAR tmp[MAX_PATH];
	GetWindowText(hEdit, tmp, GetWindowTextLength(hWnd) + 1);
	ListView_SetItemText(GetDlgItem(hWnd, ID_LV), Col2, 1, tmp);
	Col2++;
}
void OnFindMsgString(HWND hwnd, LPFINDREPLACE lpFindReplace) {
	if (lpFindReplace->Flags & FR_FINDNEXT)
	{
		LVFINDINFO lvfiInfo;
		lvfiInfo.flags = LVFI_STRING;
		lvfiInfo.psz = szBuffer; // Строка, которую будем искать
		int iStart = -1; // Ищем с начала списка
		int iIndex = SendMessage(list, LVM_FINDITEM, (WPARAM)(int)iStart, (LPARAM)&lvfiInfo);
		if (iIndex != -1) // Что-то нашли?
		{
			ListView_SetItemState(list, iIndex, LVIS_SELECTED, LVIS_SELECTED); // Выделяем
			SetFocus(list); // У меня LVS_SHOWSELALWAYS не выставлено, поэтому перевожу фокус
		}
	} 
	if (lpFindReplace->Flags & FR_REPLACE)
	{
		LVFINDINFO lvfiInfo;
		lvfiInfo.flags = LVFI_STRING;
		lvfiInfo.psz = szBuffer; // Строка, которую будем искать
		int iStart = -1; // Ищем с начала списка
		int iIndex = SendMessage(list, LVM_FINDITEM, (WPARAM)(int)iStart, (LPARAM)&lvfiInfo);
		if (iIndex != -1) // Что-то нашли?
		{
			ListView_SetItemState(list, iIndex, LVIS_SELECTED, LVIS_SELECTED); // Выделяем
			SetFocus(list); 
		}
		ListView_SetItemText(list, iIndex,0, szBuffer2); 
	}
	if (lpFindReplace->Flags & FR_REPLACEALL)
	{
		LVFINDINFO lvfiInfo;
		lvfiInfo.flags = LVFI_STRING;
		lvfiInfo.psz = szBuffer; // Строка, которую будем искать
		int iStart = -1; // Ищем с начала списка
		for (int i = 0; i < Col2; i++) {
			int iIndex = SendMessage(list, LVM_FINDITEM, (WPARAM)(int)iStart, (LPARAM)&lvfiInfo);
			if (iIndex != -1) // Что-то нашли?
			{
				ListView_SetItemState(list, iIndex, LVIS_SELECTED, LVIS_SELECTED); // Выделяем
				SetFocus(list);
			}
			ListView_SetItemText(list, iIndex, 0, szBuffer2);
		}
	}
} 
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		BOOL bRet = HANDLE_WM_INITDIALOG(hwndDlg, wParam, lParam, Dialog_OnInitDialog);
		return SetDlgMsgResult(hwndDlg, uMsg, bRet);
	}
	case WM_CLOSE:
	HANDLE_WM_CLOSE(hwndDlg, wParam, lParam, Dialog_OnClose);   	return TRUE;  case WM_COMMAND:
		HANDLE_WM_COMMAND(hwndDlg, wParam, lParam,
			Dialog_OnCommand);
		return TRUE;
	} 
	return FALSE;
} 
BOOL Dialog_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HWND hwndEdit = GetDlgItem(hwnd, IDC_EDIT1);
	Edit_LimitText(hwndEdit, _countof(szBuffer) - 1);
	Edit_SetCueBannerText(hwndEdit, L"Название новой записи");
	return TRUE;
} 
void Dialog_OnClose(HWND hwnd)
{
	if (hwnd == hDlg)
	{
		DestroyWindow(hwnd);
	} 
	else
	{
		EndDialog(hwnd, IDCLOSE);
	} 
} 
void Dialog_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
	{
		int cch = GetDlgItemText(hwnd, IDC_EDIT1, szBuffer, _countof(szBuffer));

		if (0 == cch)
		{
			HWND hwndEdit = GetDlgItem(hwnd, IDC_EDIT1);
			EDITBALLOONTIP ebt = { sizeof(EDITBALLOONTIP) };
			ebt.pszTitle = L"SampleWin32";
			ebt.pszText = L"Укажите название новой записи";
			ebt.ttiIcon = TTI_WARNING;
			Edit_ShowBalloonTip(hwndEdit, &ebt);
		} 
		else if (hwnd == hDlg)
		{
			SetDlgItemText(hwnd, IDC_EDIT1, NULL);
			SendMessage(GetParent(hwnd), WM_ADDITEM, 0, 0);
		}
		else
		{
			EndDialog(hwnd, IDOK);
		} 
	} 
	break;
	case IDCANCEL: 
		if (hwnd == hDlg)
		{
			DestroyWindow(hwnd);
		} 
		else
		{
			EndDialog(hwnd, IDCANCEL);
		}
		break;
	}
}