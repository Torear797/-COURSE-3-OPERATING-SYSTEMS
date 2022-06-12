#include "st.h"
#pragma comment(lib,"Secur32.lib")
namespace print {
	void LovalName()
	{
		char buffer[UNLEN + 1]; 
		DWORD size; 
		size = sizeof(buffer); 
		GetComputerNameA(buffer, &size);
		cout << "Name local PC: " << buffer << endl;
	}
	void domenName()
	{
		LPTSTR buffer = NULL;
		DWORD size = 0;

		GetComputerNameEx(ComputerNamePhysicalDnsHostname, NULL, &size);
		buffer = new TCHAR[size];

		BOOL bRet = GetComputerNameEx(ComputerNamePhysicalDnsHostname, buffer, &size);
		if (FALSE != bRet)
		{
			wcout << "Domen Name: " << buffer << endl;
		}
		delete[]buffer;
	}
	void userName()
	{
		char buffer[UNLEN + 1]; 
		DWORD size; 
		size = sizeof(buffer); 
		GetUserNameA(buffer, &size);
		cout << "User Name: " << buffer << endl;
	}
	void fulluserName()
	{
		CHAR buffer[UNLEN + 1];
		DWORD size; 
		size = sizeof(buffer); 
		GetUserNameExA(NameSamCompatible,buffer, &size);
		cout << "Full User Name: " << buffer << endl;
	}
}