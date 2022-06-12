#include "header.h"
extern "C" __declspec(dllexport) void SystemParametersInfoPrint1()
{
	RECT rectWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&rectWorkArea,0);
	cout << "Размеры рабочей области " << endl;
	cout << "Left = "  << rectWorkArea.left<< endl;
	cout << "Right = " << rectWorkArea.right << endl;
	cout << "Top = " << rectWorkArea.top << endl;
	cout << "bottom = " << rectWorkArea.bottom << endl;
}
extern "C" __declspec(dllexport) void SystemParametersInfoPrint2()
{
	DWORD pvParam;
	cout << "Передать кнопку прокрутки с фокусом ?(0 - нет , 1 - да) = " << SystemParametersInfo(SPI_GETMOUSEWHEELROUTING, 0, (LPVOID)&pvParam, 0) << endl;
}