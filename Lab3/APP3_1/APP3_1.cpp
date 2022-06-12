#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <Richedit.h>
#include <psapi.h>
#include <stdio.h>
#include <strsafe.h>
#include <tlhelp32.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "strsafe.lib")
#define IDC_BUTTON1 2002
#define IDC_LIST  2001
#define IDC_LIST2  2055
#define IDC_LIST3  2056
#define TestButton 2444
#define ID_Update 2600
#define ID_C1 2445
#define ID_C2 2446
#define ID_LV 2447
#define ID_SPIN 2448
#define ID_edit 2449

#define IDD_DIALOG1                     101
#define IDR_MENU1                       102
#define IDR_ACCELERATOR1                103
#define IDC_EDIT1                       1002
#define IDC_HOTKEY1                     1004
#define ID_DEL_RECORD                   40001
#define ID_FIND_RECORD                  40002
#define ID_NEW_RECORD                   40003
#define ID_40004                        40004
#define ID_40005                        40005
#define ID_40006                        40006
#define ID_40007                        40007
#define ID_40008                        40008
#define ID_NEW_RECORD2                  40009
#define ID_FORMAT_FONT                  40010
#define ID_40011                        40011
#define ID_40012                        40012
#define ID_PRINT                        40013
#define ID_REPLACE                      40014
#define ID_NEW_Wait                  40015
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
HANDLE hJobGlobal = NULL;
HWND hEdit = NULL; HWND hEdit2 = NULL;
HWND hSpin = NULL;
HWND hlabel = NULL;
HWND hX = NULL;
HWND RBtn2;
HWND RBtn5;
HWND hFont = NULL;
HWND hY = NULL;
HWND hX2 = NULL;
HWND hY2 = NULL; HWND hCur = NULL;
HWND list; HWND list2; HWND list3;
HACCEL hAccel = NULL;
HWND hDlg = NULL;
HWND hFindDlg = NULL;
TCHAR szBuffer[100] = TEXT("");
TCHAR szBuffer2[100] = TEXT(""); TCHAR szBufferJob[100] = TEXT("");
FINDREPLACE findDlg;
UINT uFindMsgString = 0;
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
void LoadProcessesToListBox(HWND hwnd);
//void AddProcess();
void EnumProcessesInJob(HANDLE hJob, ULONG_PTR *lpidProcess, DWORD cb, LPDWORD lpcbNeeded);
void LoadModulesToListBox(HWND hwndCtl, DWORD dwProcessId);
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
	LoadLibrary(TEXT("ComCtl32.dll")); LoadLibrary(TEXT("Psapi.dll"));
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	hWnd = CreateWindowEx(0, TEXT("MyWindowClass"), TEXT("Lab3"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 1200, 800, NULL, NULL, hInstance, NULL);
	if (NULL == hWnd)
	{
		return -1;
	}
	ShowWindow(hWnd, nCmdShow);
	//	AddProcess();
	LoadProcessesToListBox(list);
	MSG  msg;
	BOOL bRet;
	for (;;)
	{
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			//OnIdle(hWnd);
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

		/*case WM_TIMER:
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
		return 0;*/
	case WM_ADDITEM:
		OnAddItem(hWnd);
		return 0;
		/*case WM_CHAR:
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
		}	*/
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
		//}
		/*return 0;*/
		/*case	WM_KEYUP:
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
		return 0;*/

		//case WM_LBUTTONDBLCLK:
		//{
		//		int mbResult = MessageBox(hWnd, TEXT("Удалить все элементы?"), TEXT("SampleWin32"),MB_YESNO | MB_ICONQUESTION);
		//		if (mbResult == IDYES) { ListView_DeleteAllItems(list); Col2 = 0; }
		//}
		//	return 0;
		//case	WM_LBUTTONDOWN:
		//{
		//		ReleaseCapture();
		//		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		//	
		//}
		//	return 0;
		//case	WM_LBUTTONUP:
		//{
		//	POINT pt;
		//	TCHAR Cur[32];

		//	GetCursorPos(&pt);
		//	ScreenToClient(hWnd, &pt);
		//	wsprintf(Cur, L"x: %d y: %d", pt.x, pt.y);
		//	SetWindowText(hCur, Cur);
		//}
		//return 0;
		//case	WM_MOUSEWHEEL:
		//{
		//	RECT Rect;
		//	GetWindowRect(hWnd, &Rect);
		//	
		//	wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);

		//	for (; wheelDelta > WHEEL_DELTA; wheelDelta -= WHEEL_DELTA)
		//		MoveWindow(hWnd, Rect.left, Rect.top + wheelDelta, 800, 500, NULL);
		//	for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA)
		//		MoveWindow(hWnd, Rect.left, Rect.top + wheelDelta, 800, 500, NULL);
		//	ShowWindow(hWnd, 1);
		//	UpdateWindow(hWnd);
		//}
		//return 0;
		//case	WM_MOVING:
		//{
		//	RECT Rect;
		//	GetWindowRect(hWnd, &Rect);

		//	/*wsprintfW(BufferXY, L"%d", Rect.top);
		//	SetWindowText(hX, BufferXY);
		//	wsprintfW(BufferXY, L"%d", Rect.bottom);
		//	SetWindowText(hY2, BufferXY);
		//	wsprintfW(BufferXY, L"%d", Rect.right);
		//	SetWindowText(hY, BufferXY);*/
		//	/*wsprintfW(BufferXY, L"%d", Rect.left);
		//	SetWindowText(hX2, BufferXY);*/
		//	SetDlgItemInt(hWnd, 1056, Rect.top, true);
		//	SetDlgItemInt(hWnd, 1057, Rect.bottom, true);
		//	SetDlgItemInt(hWnd, 1058, Rect.right, true);
		//	SetDlgItemInt(hWnd,1055 , Rect.left,true);
		//	//setdlgItemint

		//}
		//return 0;
	}
	if (uFindMsgString == uMsg)
	{
		OnFindMsgString(hWnd, (LPFINDREPLACE)lParam);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//void OnIdle(HWND hwnd) {}
void LoadProcessesToListBox(HWND hwndCtl)
{
	// удалим все строки из списка 
	ListBox_ResetContent(hwndCtl);

	// получим список идентификаторов процессов 
	DWORD aProcessIds[1024], cbNeeded = 0;
	BOOL bRet = EnumProcesses(aProcessIds, sizeof(aProcessIds), &cbNeeded);

	if (FALSE != bRet)
	{
		TCHAR szName[MAX_PATH], szBuffer[300];

		for (DWORD i = 0, n = cbNeeded / sizeof(DWORD); i < n; ++i)
		{
			DWORD dwProcessId = aProcessIds[i], cch = 0;
			if (0 == dwProcessId) continue;

			// открываем объект ядра "процесс" 
			HANDLE hProcess = OpenProcess(PROCESS_VM_READ |
				PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);

			if (NULL != hProcess)
			{
				// определяем имя главного модуля процесса 
				cch = GetModuleBaseName(hProcess, NULL, szName, MAX_PATH);
				CloseHandle(hProcess); // закрываем объект ядра 
			} // if 


			if (0 == cch)
				StringCchCopy(szName, MAX_PATH, TEXT("Неизвестный процесс"));

			// формируем строку для списка 
			StringCchPrintf(szBuffer, _countof(szBuffer),
				TEXT("%s (PID: %u)"), szName, dwProcessId);
			// добавляем в список новую строку 
			int iItem = ListBox_AddString(hwndCtl, szBuffer);

			// сохраняем в новой строке идентификатор процесса 

			ListBox_SetItemData(hwndCtl, iItem, dwProcessId);
		} // for 
	} // if 
} // LoadProcessesToListBox 
void LoadModulesToListBox(HWND hwndCtl, DWORD dwProcessId)
{
	// удалим все строки из списка 
	ListBox_ResetContent(hwndCtl);

	// открываем объект ядра "процесс" 
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ, FALSE, dwProcessId);

	if (NULL != hProcess)
	{
		// определяем размер (в байтах) списка модулей 
		DWORD cb = 0;
		EnumProcessModulesEx(hProcess, NULL, 0, &cb,
			LIST_MODULES_ALL);

		// вычисляем количество модулей 
		DWORD nCount = cb / sizeof(HMODULE);

		// выделяем память для списка модулей 
		HMODULE *hModule = new HMODULE[nCount];

		// получаем список модулей 
		cb = nCount * sizeof(HMODULE);
		BOOL bRet = EnumProcessModulesEx(hProcess, hModule, cb, &cb, LIST_MODULES_ALL);

		if (FALSE != bRet)
		{
			TCHAR szFileName[MAX_PATH];

			for (DWORD i = 0; i < nCount; ++i)
			{
				// получаем имя загруженного модуля 
				bRet = GetModuleFileNameEx(hProcess, hModule[i], szFileName, MAX_PATH);
				if (FALSE != bRet) ListBox_AddString(hwndCtl, szFileName); // добавляем в список новую строку 
			} // for 
		} // if 

		delete[]hModule; // освобождаем память 

		CloseHandle(hProcess); // закрываем объект ядра 
	} // if 
} // LoadModulesToListBox 
BOOL WaitProcessById(DWORD dwProcessId, DWORD dwMilliseconds, LPDWORD lpExitCode)
{
	// открываем процесс 
	HANDLE hProcess = OpenProcess(SYNCHRONIZE |
		PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);

	if (NULL == hProcess)
	{
		return FALSE; // не удалось открыть процесс 
	} // if 

	  // ожидаем завершения процесса 
	WaitForSingleObject(hProcess, dwMilliseconds);

	if (NULL != lpExitCode)
	{
		// получим код завершения процесса 
		GetExitCodeProcess(hProcess, lpExitCode);
	} // if 

	  // закрываем дескриптор процесса 
	CloseHandle(hProcess);

	return TRUE;
} // WaitProcessById 

BOOL StartGroupProcessesAsJob(HANDLE hJob, LPCTSTR lpszCmdLine[],
	DWORD nCount, BOOL bInheritHandles, DWORD dwCreationFlags)
{
	// определим, включен ли вызывающий процесс в задание 
	BOOL bInJob = FALSE;
	IsProcessInJob(GetCurrentProcess(), NULL, &bInJob);

	if (FALSE != bInJob) // если да (!) 
	{
		// определим разрешено ли порождать процессы, 
		// которые не будут принадлежать этому заданию ... 

		JOBOBJECT_BASIC_LIMIT_INFORMATION jobli = { 0 };

		QueryInformationJobObject(NULL,
			JobObjectBasicLimitInformation, &jobli, sizeof(jobli), NULL);
		DWORD dwLimitMask = JOB_OBJECT_LIMIT_BREAKAWAY_OK |
			JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;

		if ((jobli.LimitFlags & dwLimitMask) == 0)
		{
			/* все порожденные процессы
			20	автоматически включаются в задание */
			return FALSE;
		} // if 
	} // if 

	  // порождаем процессы... 

	TCHAR szCmdLine[MAX_PATH];

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;


	StringCchCopy(szCmdLine, MAX_PATH, szBuffer);

	// порождаем новый процесс, 
	// приостанавливая работу его главного потока 
	BOOL bRet = CreateProcess(NULL, szCmdLine, NULL, NULL, bInheritHandles, dwCreationFlags | CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB, NULL, NULL, &si, &pi);

	if (FALSE != bRet)
	{
		// добавляем новый процесс в задание 
		AssignProcessToJobObject(hJob, pi.hProcess);
		// возобновляем работу потока нового процесса 
		ResumeThread(pi.hThread);

		// закрывает дескриптор потока нового процесса 
		CloseHandle(pi.hThread);
		// закрывает дескриптор нового процесса 
		CloseHandle(pi.hProcess);
		// if 
	} // for 
	return TRUE;
} // StartGroupProcessesAsJob 
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
	CreateWindowEx(0, TEXT("Button"), TEXT("Показать модули"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 350, 450, 200, 40, hwnd, (HMENU)ID_NEW_RECORD, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Ждать завершения"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620, 55, 200, 40, hwnd, (HMENU)ID_NEW_Wait, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Временно приостановить"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620, 100, 200, 40, hwnd, (HMENU)ID_NEW_RECORD2, lpCreateStruct->hInstance, NULL);

	hEdit = CreateWindowEx(0, TEXT("edit"), TEXT("5"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620, 145, 200, 20, hwnd, (HMENU)ID_NEW_RECORD2, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Завершить работу процесса"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620, 195, 200, 40, hwnd, (HMENU)ID_DEL_RECORD, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Создать задание"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620, 245, 200, 40, hwnd, (HMENU)ID_FIND_RECORD, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Включено в задание?"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620, 290, 200, 40, hwnd, (HMENU)TestButton, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Реального времени"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON | WS_GROUP, 370, 550, 200, 20, hwnd, (HMENU)150, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Высокий"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 370, 570, 200, 20, hwnd, (HMENU)144, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Выше сдерднего"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 370, 590, 200, 20, hwnd, (HMENU)151, lpCreateStruct->hInstance, NULL);
	RBtn2 = CreateWindowEx(0, TEXT("Button"), TEXT("Средний"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 370, 610, 200, 20, hwnd, (HMENU)145, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Ниже среднего"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 370, 630, 200, 20, hwnd, (HMENU)152, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Низкий"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 370, 650, 200, 20, hwnd, (HMENU)146, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Изменить приоритет процесса"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 320, 680, 250, 40, hwnd, (HMENU)ID_REPLACE, lpCreateStruct->hInstance, NULL);


	CreateWindowEx(0, TEXT("Button"), TEXT("Критический по времени"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON | WS_GROUP, 40, 530, 200, 20, hwnd, (HMENU)147, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Максимальный"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 40, 550, 200, 20, hwnd, (HMENU)156, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Выше среднего"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 40, 570, 200, 20, hwnd, (HMENU)153, lpCreateStruct->hInstance, NULL);

	RBtn5 = CreateWindowEx(0, TEXT("Button"), TEXT("Средний"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 40, 590, 200, 20, hwnd, (HMENU)148, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Ниже среднего"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 40, 610, 200, 20, hwnd, (HMENU)154, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Минимальный"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 40, 630, 200, 20, hwnd, (HMENU)155, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Простаивающий"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_AUTORADIOBUTTON, 40, 650, 200, 20, hwnd, (HMENU)149, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Изменить приоритет потока"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 680, 200, 40, hwnd, (HMENU)ID_PRINT, lpCreateStruct->hInstance, NULL);


	CreateWindowEx(0, TEXT("Button"), TEXT("Обновить"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 30, 450, 200, 40, hwnd, (HMENU)ID_Update, lpCreateStruct->hInstance, NULL);

	list = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 10, 10, 250, 420, hwnd, (HMENU)IDC_LIST, lpCreateStruct->hInstance, NULL);
	list3 = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 850, 10, 300, 420, hwnd, (HMENU)IDC_LIST3, lpCreateStruct->hInstance, NULL);
	list2 = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 300, 10, 300, 420, hwnd, (HMENU)IDC_LIST2, lpCreateStruct->hInstance, NULL);
	SendMessage(RBtn2, BM_CLICK, 0, 0); SendMessage(RBtn5, BM_CLICK, 0, 0);
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
	//if (state != SIZE_MINIMIZED)
	//{
	//	HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST);
	//	MoveWindow(hwndCtl, 10, 10, 250, cy - 20, TRUE);
	//}
}
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HINSTANCE hInstance = GetWindowInstance(hwnd);
	switch (id)
	{
	case ID_Update:
	{
		LoadProcessesToListBox(list);
	}
	break;
	case ID_NEW_RECORD2:
	{
		TCHAR al[10];
		Edit_GetText(hEdit, al, 10);
		//GetWindowText(hEdit,al,3);
		DWORD a = _wtoi(al);
		a = 1000 * a;
		HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST);
		int dwProcessId = ListBox_GetCurSel(hwndCtl);
		dwProcessId = ListBox_GetItemData(hwndCtl, dwProcessId);
		WaitProcessById(dwProcessId, a, NULL);
		MessageBox(hWnd, TEXT("Работа приложения восстановлена!"), TEXT("Ожидание"), MB_OK);
	}
	break;
	case ID_NEW_RECORD:
	{
		HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST);
		int dwProcessId = ListBox_GetCurSel(hwndCtl);
		dwProcessId = ListBox_GetItemData(hwndCtl, dwProcessId);
		LoadModulesToListBox(list2, dwProcessId);
	}
	break;
	case  ID_NEW_Wait:
	{
		HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST);
		int dwProcessId = ListBox_GetCurSel(hwndCtl);
		dwProcessId = ListBox_GetItemData(hwndCtl, dwProcessId);
		WaitProcessById(dwProcessId, INFINITE, NULL);
		MessageBox(hWnd, TEXT("Работа приложения восстановлена!"), TEXT("Ожидание"), MB_OK);
	}
	break;
	case ID_DEL_RECORD:
	{
		int mbResult = MessageBox(hWnd, TEXT("Завершить процесс?"), TEXT("Завершение процесса"), MB_YESNO | MB_ICONQUESTION);
		if (mbResult == IDYES) {
			HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST);
			int dwProcessId = ListBox_GetCurSel(hwndCtl);
			dwProcessId = ListBox_GetItemData(hwndCtl, dwProcessId);

			HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, dwProcessId);
			TerminateProcess(hProcess, 0);
			LoadProcessesToListBox(list);
		}
	}
	break;
	case ID_FIND_RECORD:
	{
		if (IsWindow(hDlg) == FALSE)
		{
			hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc);
			ShowWindow(hDlg, SW_SHOW);
		}
	}
	break;
	case TestButton:
	{
		//HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST);
		//int dwProcessId = ListBox_GetCurSel(hwndCtl);
		//dwProcessId = ListBox_GetItemData(hwndCtl, dwProcessId);
		//
		DWORD bts;
		EnumProcessesInJob(NULL, new ULONG_PTR[24], 1024, &bts);

	}
	break;
	case ID_REPLACE:
	{
		HANDLE hProcess = NULL;
		HANDLE hCurrentProcess = GetCurrentProcess();
		BOOL bRet = DuplicateHandle(hCurrentProcess, hCurrentProcess, hCurrentProcess, &hProcess, 0, FALSE, DUPLICATE_SAME_ACCESS);

		TCHAR Buflvl[10] = TEXT("");
		if (IsDlgButtonChecked(hwnd, 144))
		{
			SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
		}
		else
			if (IsDlgButtonChecked(hwnd, 145))
			{
				SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);

			}
			else
				if (IsDlgButtonChecked(hwnd, 146))
				{
					SetPriorityClass(hProcess, IDLE_PRIORITY_CLASS);

				}
				else if (IsDlgButtonChecked(hwnd, 150))
				{
					SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS);
				}
				else
					if (IsDlgButtonChecked(hwnd, 151))
					{
						SetPriorityClass(hProcess, ABOVE_NORMAL_PRIORITY_CLASS);

					}
					else
						if (IsDlgButtonChecked(hwnd, 152))
						{
							SetPriorityClass(hProcess, BELOW_NORMAL_PRIORITY_CLASS);

						}
		if (FALSE != bRet)
		{
			CloseHandle(hProcess), hProcess = NULL;
		}

	}
	break;
	case ID_PRINT:
	{

		HANDLE hPriorthread = NULL;

		DWORD threadid = GetCurrentThreadId();
		hPriorthread = OpenThread(THREAD_SET_INFORMATION, FALSE, threadid);

		TCHAR Buflvl[100] = TEXT("");
		if (IsDlgButtonChecked(hwnd, 147))
		{
			if (IsDlgButtonChecked(hwnd, 150))SetThreadPriority(hPriorthread, 31); else
				SetThreadPriority(hPriorthread, 15);

		}
		else
			if (IsDlgButtonChecked(hwnd, 148))
			{
				if (IsDlgButtonChecked(hwnd, 144))SetThreadPriority(hPriorthread, 13);
				else
					if (IsDlgButtonChecked(hwnd, 145))SetThreadPriority(hPriorthread, 8);
					else
						if (IsDlgButtonChecked(hwnd, 146))SetThreadPriority(hPriorthread, 4); else

							if (IsDlgButtonChecked(hwnd, 152))SetThreadPriority(hPriorthread, 6); else
								if (IsDlgButtonChecked(hwnd, 151))SetThreadPriority(hPriorthread, 10); else
									if (IsDlgButtonChecked(hwnd, 150))SetThreadPriority(hPriorthread, 24);
			}
			else
				if (IsDlgButtonChecked(hwnd, 149))
				{
					if (IsDlgButtonChecked(hwnd, 150))SetThreadPriority(hPriorthread, 16); else
						SetThreadPriority(hPriorthread, 1);

				}
				else
				{
					if (IsDlgButtonChecked(hwnd, 155))
					{
						if (IsDlgButtonChecked(hwnd, 144))SetThreadPriority(hPriorthread, 11);
						else
							if (IsDlgButtonChecked(hwnd, 145))SetThreadPriority(hPriorthread, 6);
							else
								if (IsDlgButtonChecked(hwnd, 146))SetThreadPriority(hPriorthread, 2); else

									if (IsDlgButtonChecked(hwnd, 152))SetThreadPriority(hPriorthread, 4); else
										if (IsDlgButtonChecked(hwnd, 151))SetThreadPriority(hPriorthread, 8); else
											if (IsDlgButtonChecked(hwnd, 150))SetThreadPriority(hPriorthread, 22);

					}
					else
					{
						if (IsDlgButtonChecked(hwnd, 154))
						{
							if (IsDlgButtonChecked(hwnd, 144))SetThreadPriority(hPriorthread, 12);
							else
								if (IsDlgButtonChecked(hwnd, 145))SetThreadPriority(hPriorthread, 7);
								else
									if (IsDlgButtonChecked(hwnd, 146))SetThreadPriority(hPriorthread, 3); else

										if (IsDlgButtonChecked(hwnd, 152))SetThreadPriority(hPriorthread, 5); else
											if (IsDlgButtonChecked(hwnd, 151))SetThreadPriority(hPriorthread, 9); else
												if (IsDlgButtonChecked(hwnd, 150))SetThreadPriority(hPriorthread, 23);
						}
						else
						{
							if (IsDlgButtonChecked(hwnd, 153))
							{
								if (IsDlgButtonChecked(hwnd, 144))SetThreadPriority(hPriorthread, 14);
								else
									if (IsDlgButtonChecked(hwnd, 145))SetThreadPriority(hPriorthread, 9);
									else
										if (IsDlgButtonChecked(hwnd, 146))SetThreadPriority(hPriorthread, 5); else

											if (IsDlgButtonChecked(hwnd, 152))SetThreadPriority(hPriorthread, 7); else
												if (IsDlgButtonChecked(hwnd, 151))SetThreadPriority(hPriorthread, 11); else
													if (IsDlgButtonChecked(hwnd, 150))SetThreadPriority(hPriorthread, 25);
							}
							else
							{
								if (IsDlgButtonChecked(hwnd, 156))
								{
									if (IsDlgButtonChecked(hwnd, 144))SetThreadPriority(hPriorthread, 15);
									else
										if (IsDlgButtonChecked(hwnd, 145))SetThreadPriority(hPriorthread, 10);
										else
											if (IsDlgButtonChecked(hwnd, 146))SetThreadPriority(hPriorthread, 6); else

												if (IsDlgButtonChecked(hwnd, 152))SetThreadPriority(hPriorthread, 8); else
													if (IsDlgButtonChecked(hwnd, 151))SetThreadPriority(hPriorthread, 12); else
														if (IsDlgButtonChecked(hwnd, 150))SetThreadPriority(hPriorthread, 26);
								}
							}
						}
					}
				}
		CloseHandle(hPriorthread), hPriorthread = NULL;

	} break;
	}
}
void EnumProcessesInJob(HANDLE hJob, ULONG_PTR *lpidProcess, DWORD cb, LPDWORD lpcbNeeded)
{
	DWORD nCount = cb / sizeof(ULONG_PTR);
	BOOL OutputList = FALSE;
	if (NULL != lpidProcess && nCount > 0)
	{
		DWORD cbJobPIL = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + (nCount - 1) * sizeof(ULONG_PTR);
		JOBOBJECT_BASIC_PROCESS_ID_LIST *pJobPIL = (JOBOBJECT_BASIC_PROCESS_ID_LIST *)malloc(cbJobPIL);
		if (NULL != pJobPIL)
		{
			pJobPIL->NumberOfAssignedProcesses = nCount;
			BOOL bRet = QueryInformationJobObject(hJob,
				JobObjectBasicProcessIdList, pJobPIL, cbJobPIL, NULL);

			if (FALSE != bRet)
			{
				nCount = pJobPIL->NumberOfProcessIdsInList;
				CopyMemory(lpidProcess, pJobPIL->ProcessIdList, nCount * sizeof(ULONG_PTR));
				if (NULL != lpcbNeeded)
					* lpcbNeeded = nCount * sizeof(ULONG_PTR);
			}
			OutputList = bRet;
			free(pJobPIL);
			//return bRet;
		}
	}
	if (OutputList != FALSE) {

		TCHAR nme[MAX_PATH];
		TCHAR buff[300];

		HWND lb = GetDlgItem(hWnd, IDC_LIST3);
		ListBox_ResetContent(lb);
		for (DWORD i = 0, n = *lpcbNeeded / sizeof(DWORD); i < n; ++i) {
			DWORD chrs;
			if (lpidProcess[i] != 0) {
				HANDLE Proc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, lpidProcess[i]);
				if (Proc != NULL) {
					chrs = GetModuleBaseName(Proc, NULL, nme, MAX_PATH);
					CloseHandle(Proc);

					if (chrs == 0) {
						StringCchCopy(nme, MAX_PATH, TEXT("Неизвестный процесс"));
					}
					StringCchPrintf(buff, _countof(buff), TEXT("%s (ID: %u)"), nme, lpidProcess[i]);
					int nitem = ListBox_AddString(lb, buff);
					ListBox_SetItemData(lb, nitem, lpidProcess[i]);
				}
			}
		}
	}
	//return FALSE;
} // EnumProcessesInJob 
void OnAddItem(HWND hwnd)
{
	//wcscpy(szBufferJob, szBuffer);
	//wcscat(szBufferJob, TEXT("cmd"));
	//wcscat(szBufferJob, TEXT(" "));
	//wcscat(szBufferJob, TEXT("cmd"));

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
		ListView_SetItemText(list, iIndex, 0, szBuffer2);
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
	Edit_SetCueBannerText(hwndEdit, L"Добавить новый процесс");
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
			ebt.pszTitle = L"Лабораторная работа № 3";
			ebt.pszText = L"Добавить процесс в задание";
			ebt.ttiIcon = TTI_WARNING;
			Edit_ShowBalloonTip(hwndEdit, &ebt);



		}
		else if (hwnd == hDlg)
		{

			SetDlgItemText(hwnd, IDC_EDIT1, NULL);

			if (hJobGlobal == NULL)
				hJobGlobal = CreateJobObject(NULL, NULL); // создаем задание 

			if (NULL != hJobGlobal)
			{
				// установим ограничения для процессов в задании ... 
				JOBOBJECT_BASIC_LIMIT_INFORMATION jobli = { 0 };

				// процессы в задании должны выполняется с низким приоритетом
				jobli.PriorityClass = IDLE_PRIORITY_CLASS;
				// задаем установленные ограничения 
				jobli.LimitFlags = JOB_OBJECT_LIMIT_PRIORITY_CLASS;

				BOOL bRet = SetInformationJobObject(hJobGlobal, JobObjectBasicLimitInformation, &jobli, sizeof(jobli));

				if (FALSE != bRet)
				{
					// порождаем процессы... 

					LPCTSTR szCmdLine[] = { szBufferJob };

					bRet = StartGroupProcessesAsJob(hJobGlobal, szCmdLine, _countof(szCmdLine), FALSE, 0);
				} // if 

				  // закрываем дескриптор задания 
				  //	MessageBox(hWnd, szBufferJob, TEXT("SampleWin32"), MB_YESNO | MB_ICONQUESTION);

			} // if 
			LoadProcessesToListBox(list);

			//	SendMessage(GetParent(hwnd), WM_ADDITEM, 0, 0);
		}
		else
		{
			EndDialog(hwnd, IDOK);
		}
	}
	break;
	case IDCANCEL:
		CloseHandle(hJobGlobal);
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