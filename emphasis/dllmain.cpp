// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "stdafx.h"

#include <cstring>

#include "LabViewArray.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

struct s{
	INT32 A;
	INT64 B;
	double C;
	LabViewArray<UINT8> **data;

	s& operator=(const s& lr) {
		A = lr.A;
		B = lr.B;
		C = lr.C;
		**data = **lr.data;
		return *this;
	}
};


extern "C"  __declspec(dllexport) void test_struct_in_out(s* in, s* out) {
	*out = *in;
}