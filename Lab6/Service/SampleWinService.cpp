#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <process.h>
LPCTSTR gSvcName = TEXT("Artyom_FirstService");
LPCTSTR gSvcDisplayName = TEXT("Artyom_FirstService");
BOOL WriteFileMapping(HANDLE hFileMapping, LPCVOID lpData, DWORD cb);
BOOL ReadFileMapping(HANDLE hFileMapping, LPVOID lpData, DWORD cb);
HANDLE hProjectionFile = NULL;
HANDLE hServerNewData = NULL;
HANDLE hClientNewData = NULL;
HANDLE hSynchronization = NULL;
HANDLE hShutdown = NULL;
HANDLE hThreads[1];
unsigned __stdcall ThreadFunc(void *lpParameter);
BOOL OnSvcInit(DWORD dwArgc, LPTSTR *lpszArgv) 
{
	SECURITY_DESCRIPTOR sd;
	BOOL bRet = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	if (FALSE != bRet)
	bRet = SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	if (FALSE != bRet)
	{
		SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
		sa.lpSecurityDescriptor = &sd;
		hProjectionFile = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, 100 * sizeof(TCHAR), TEXT("Global\\ProjectionFile"));
		if (NULL == hProjectionFile)
		{
			return FALSE;
		}
		hServerNewData = CreateEventEx(&sa, TEXT("Global\\ServerNewData"), 0, EVENT_ALL_ACCESS);
		if (NULL == hServerNewData)
		{
			return FALSE;
		}
		hClientNewData = CreateEventEx(&sa, TEXT("Global\\ClientNewData"), 0, EVENT_ALL_ACCESS);
		if (NULL == hClientNewData)
		{
			return FALSE;
		}
		hSynchronization = CreateMutexEx(&sa, TEXT("Global\\Synchronization"), 0, MUTEX_ALL_ACCESS);
		if (NULL == hSynchronization)
		{
			return FALSE;
		}
	}
	return bRet;
}
void OnSvcStop(void) 
{
	SetEvent(hShutdown);
}
DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv) 
{
	hShutdown = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, NULL, 0, NULL);
	WaitForMultipleObjects(_countof(hThreads), hThreads, TRUE, INFINITE);
	CloseHandle(hShutdown);
	return 0;
}
BOOL WriteFileMapping(HANDLE hFileMapping, LPCVOID lpData, DWORD cb)
{
	PBYTE pbFile = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, cb);
	if (NULL == pbFile) return FALSE;
	CopyMemory(pbFile, lpData, cb);
	BOOL bRet = FlushViewOfFile(pbFile, cb);
	UnmapViewOfFile(pbFile);
	return bRet;
}
BOOL ReadFileMapping(HANDLE hFileMapping, LPVOID lpData, DWORD cb)
{
	PBYTE pbFile = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, cb);
	if (NULL == pbFile) return FALSE;
	CopyMemory(lpData, pbFile, cb);
	UnmapViewOfFile(pbFile);
	return TRUE;
}
constexpr LPCTSTR StudentsList[] = {
	TEXT("Аладьина Анастасия Сергеевна ИТ-31"),
	TEXT("Аникеев Денис Владимирович ИТ-31"),
	TEXT("Бабакина Юлия Александровна ИТ-31"),
	TEXT("Баженов Владислав Владимирович ИТ-31"),
	TEXT("Ильченко Владислав Андреевич ИТ-31"),
	TEXT("Кириенко Наталья Владимировна ИТ-31"),
	TEXT("Клыч Артем Арманович ИТ-31"),
	TEXT("Коршунов Артем Геннадьевич ИТ-31"),
	TEXT("Мавлянов Руслан Джалилович ИТ-31"),
	TEXT("Поляков Валентин Евгеньевич ИТ-31"),
	TEXT("Ромащенко Николай Андреевич ИТ-31"),
	TEXT("Симонова Мария Петровна ИТ-31"),
	TEXT("Станкевич Ангелина Дмитриевна ИТ-31"),
	TEXT("Стрельников Антон Павлович ИТ-31"),
	TEXT("Сыровацкий Евгений Юрьевич ИТ-31"),
	TEXT("Шаповалов Дмитрий Иванович ИТ-31"),
	TEXT("Эймонт Сергей Вячеславович ИТ-31")
};
#pragma pack(push, 1)
struct Students_DATA
{
	DWORD dwProcessId;
	DWORD dwIndex;
};
#pragma pack(pop)
unsigned __stdcall ThreadFunc(void *lpParameter)
{
	HANDLE events[2] = { hShutdown, hServerNewData };
	for (;;)
	{
		DWORD dwResult = WaitForMultipleObjects(2, events, FALSE, INFINITE);
		if (WAIT_OBJECT_0 == dwResult) 
		{
			break; 
		} 
		else if ((WAIT_OBJECT_0 + 1) == dwResult) 
		{
			Students_DATA Request;
			BOOL bRet = ReadFileMapping(hProjectionFile, &Request, sizeof(Request));
			if (FALSE == bRet) break; 
			TCHAR szResponse[100] = TEXT(""); 
			if (Request.dwIndex < _countof(StudentsList))
			{
				StringCchCopy(szResponse, _countof(szResponse), StudentsList[Request.dwIndex]);
			} 
			WriteFileMapping(hProjectionFile, szResponse, sizeof(szResponse));
			SetEvent(hClientNewData);
		} 
	}
	CloseHandle(hProjectionFile);
	CloseHandle(hServerNewData);
	CloseHandle(hClientNewData);
	CloseHandle(hSynchronization);
	return 0;
}