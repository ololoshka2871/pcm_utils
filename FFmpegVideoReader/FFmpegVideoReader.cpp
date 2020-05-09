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

extern "C" __declspec(dllexport) void GetFrameSize(
	/* In */ FfmpegContext **ctx, 
	/* out */ int32_t *weigth,
	/* out */ int32_t *heigth
) {
	auto _ctx = static_cast<FfmpegContext*>(*ctx);
	std::tie(*weigth, *heigth) = _ctx->get_frame_size();
}

extern "C" __declspec(dllexport) void GetFrameCount(
	/* In */ FfmpegContext **ctx,
	/* out */ int32_t *frame_count
) {
	auto _ctx = static_cast<FfmpegContext*>(*ctx);
	*frame_count = _ctx->get_frame_count();
}

extern "C" __declspec(dllexport) int SkipFrames(
	/* In */ FfmpegContext **ctx,
	/* out */ int64_t timestamp
) {
	auto _ctx = static_cast<FfmpegContext*>(*ctx);
	return _ctx->skip_frames(timestamp);
}

extern "C" __declspec(dllexport) int64_t GetNextFrame(
	/* In */ FfmpegContext **ctx,

	/* out */ uint8_t *pixel_data,
	/* out */ int *error_code
) {
	auto _ctx = static_cast<FfmpegContext*>(*ctx);

	try {
		return _ctx->read_frame_to(pixel_data);
	} catch (FFmpegException ex) {
		*error_code = ex.errorCode();
	}
}