#pragma once
#if !defined st_h_
#define st_h_

#include <Windows.h>
#include <iostream>
namespace print {
	void LovalName();

	void domenName();

	void userName();

	void fulluserName();
}
#define SECURITY_WIN32
#include <security.h>
using namespace std;
#define UNLEN 256
#endif