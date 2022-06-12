#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <Richedit.h>
#include <Process.h>
#include <psapi.h>
#include <stdio.h>
#include <strsafe.h>
#include <tlhelp32.h>
#include <ctime>
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
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "strsafe.lib")
#define IDC_LIST  2001
HDC hdc;
PAINTSTRUCT ps;
#define MATRIX_SIZE 4
HWND hWnd = NULL;
HWND hFont = NULL;
HACCEL hAccel = NULL;
HWND hDlg = NULL;
HWND hFindDlg = NULL;
FINDREPLACE findDlg;
long lock = 0;
UINT uFindMsgString = 0;
static int wheelDelta = 0;
LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL PreTranslateMessage(LPMSG lpMsg);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hwnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Dialog_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Dialog_OnClose(HWND hwnd);
void LoadProcessesToListBox(HWND hwnd);
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
	LoadLibrary(TEXT("ComCtl32.dll"));// LoadLibrary(TEXT("Psapi.dll"));
									  //  hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	hWnd = CreateWindowEx(0, TEXT("MyWindowClass"), TEXT("Lab3"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 600, 300, NULL, NULL, hInstance, NULL);
	if (NULL == hWnd)
	{
		return -1;
	}
	ShowWindow(hWnd, nCmdShow);
	//	AddProcess();
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
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
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

	CreateWindowEx(0, TEXT("Button"), TEXT("Построить матрицу"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 10, 200, 40, hwnd, (HMENU)ID_NEW_RECORD, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Static"), TEXT("0"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 70, 80, 60, 60, hwnd, (HMENU)3500, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Static"), TEXT("0"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 70, 120, 60, 60, hwnd, (HMENU)3501, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Static"), TEXT("0"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 70, 160, 60, 60, hwnd, (HMENU)3502, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Static"), TEXT("MAX: "),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 80, 60, 60, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Static"), TEXT("MIN: "),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 120, 60, 60, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Static"), TEXT("Double: "),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 160, 60, 60, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	int ID = 4001, X = 300, Y = 10, X0 = 300;
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		for (int j = 0; j < MATRIX_SIZE; j++)
		{

			CreateWindowEx(0, TEXT("Static"), TEXT("0"),
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, X, Y, 60, 60, hwnd, (HMENU)ID, lpCreateStruct->hInstance, NULL);

			ID++; X = X + 60;

		}

		Y = Y + 60; X = X0;
	}
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
struct row
{
	int value[MATRIX_SIZE];
	int MAX;
	int MIN;
	int DblC;
	int rnd;
};
int MAX = 0, MIN = 1000, DblC = 0;
unsigned _stdcall generateAndCalc(void *data)
{
	//преобразую полученные данные к типу структуры
	row *r = (row *)data;
	//инициализирую генератор случайных чисел полученным числом
	srand(r->rnd);
	//генерирую элементы строки
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		r->value[i] = rand() % 999;
	}
	//нахожу мин макс и двойные
	r->MIN = 1000; r->MAX = 0; r->DblC = 0;
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		if (r->value[i] > 9 && r->value[i] < 100)r->DblC++;
		if (r->value[i] > r->MAX)r->MAX = r->value[i];
		if (r->value[i] < r->MIN) r->MIN = r->value[i];
	}
	while (InterlockedExchange(&lock, 1) != 0)SwitchToThread();
	DblC = DblC + r->DblC;
	if (MAX < r->MAX)MAX = r->MAX;
	if (MIN > r->MIN)MIN = r->MIN;
	InterlockedExchange(&lock, 0);
	return 0;
}
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HINSTANCE hInstance = GetWindowInstance(hwnd);
	switch (id)
	{
	case ID_NEW_RECORD:
	{
		MIN = 1000; MAX = 0; DblC = 0;
		//инициализируем генератор случайных чисел
		srand(time(NULL));
		//определяем дескрипторы потоков,
		//идентификаторы потоков и структуры для строк матрицы
		HANDLE thread[MATRIX_SIZE];
		row rows[MATRIX_SIZE];

		for (int i = 0; i < MATRIX_SIZE; i++)
		{
			//генерируем случайные числа для каждой строки
			rows[i].rnd = rand();
			//создаем потоки
			thread[i] = (HANDLE)_beginthreadex(NULL, 0, generateAndCalc, &rows[i], 0, NULL);
			/*WaitForMultipleObjects(3, thread, TRUE, INFINITE);
			CloseHandle(thread[i]);*/
		}
		WaitForMultipleObjects(MATRIX_SIZE, thread, TRUE, INFINITE);
		for (int i = 0; i < MATRIX_SIZE; i++)
			CloseHandle(thread[i]);

		int ID = 4001, ID0 = 4001;
		for (int i = 0; i < MATRIX_SIZE; i++)
		{
			for (int j = 0; j < MATRIX_SIZE; j++)
			{
				SetDlgItemInt(hWnd, ID, rows[i].value[j], true);
				ID++;
			}
		}
		ID = ID0;


		SetDlgItemInt(hWnd, 3500, MAX, TRUE);
		SetDlgItemInt(hWnd, 3501, MIN, TRUE);
		SetDlgItemInt(hWnd, 3502, DblC, TRUE);
	}
	break;
	}
}