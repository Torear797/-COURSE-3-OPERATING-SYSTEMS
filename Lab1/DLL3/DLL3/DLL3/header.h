#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN           
#include <windows.h>
#include <iostream>
using namespace std;
extern "C" __declspec(dllexport) void SystemParametersInfoPrint1();
extern "C" __declspec(dllexport) void SystemParametersInfoPrint2();