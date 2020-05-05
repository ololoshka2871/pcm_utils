// FFmpegVideoReader.cpp : Определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"

#include "Context.h"

extern "C" __declspec(dllexport) void CreateFFmpegContext(char* filename, void **pCtx, int *error_code) {
	try {
		*pCtx = new FfmpegContext{ std::string{filename} };
		*error_code = 0;
	}
	catch (FFmpegException ex) {
		*error_code = ex.errorCode();
	}
}

extern "C" __declspec(dllexport) void DestroyFFmpegContext(void** pCtx) {
	auto _ctx = static_cast<FfmpegContext*>(*pCtx);
	delete _ctx;
}

extern "C" __declspec(dllexport) void GetNextFrame(
	/* In */ FfmpegContext **ctx
) {

}