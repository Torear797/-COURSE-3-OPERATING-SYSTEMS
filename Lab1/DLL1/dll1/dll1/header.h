#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             
#include <windows.h>
#include <iostream>
#include <ShlObj.h>
using namespace std;
extern "C" __declspec(dllexport) void CSIDL(const long csid[], unsigned long nCount);
extern "C" __declspec(dllexport) void OS_Print();
extern "C" __declspec(dllexport) void DataTime_Print();