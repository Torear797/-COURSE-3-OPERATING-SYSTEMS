#include "header.h"
extern "C" __declspec(dllexport) void GetSystemMetricsPrint()
{
	cout << "Высота трехмернй границы в пикселях = " << GetSystemMetrics(SM_CYEDGE) << endl;
	cout << "Ширина трехмернй границы в пикселях = " << GetSystemMetrics(SM_CXEDGE) << endl;
	cout << "Высота ячейки сетки для свер. окна = " << GetSystemMetrics(SM_CXMINSPACING) << endl;
	cout << "Ширина ячейки сетки для свер. окна) = " << GetSystemMetrics(SM_CYMINSPACING) << endl;
	cout << "Нужно представить  в звуковой форме? 0 - нет , 1 - да = " << GetSystemMetrics(SM_SHOWSOUNDS) << endl;
}


