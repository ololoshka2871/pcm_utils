// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "stdafx.h"

#include <cstdint>
#include <cassert>
#include <cstdlib>

#include "MyTwoDimArray.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//_putenv_s("OMP_WAIT_POLICY", "PASSIVE");
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
