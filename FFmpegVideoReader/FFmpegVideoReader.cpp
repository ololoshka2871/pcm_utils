// FFmpegVideoReader.cpp : Определяет экспортированные функции для приложения DLL.
//

#include "Context.h"
#include "FFmpegException.h"

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT void CreateFFmpegContext(char* filename, char* protocol, void **pCtx, int *error_code) {
	try {
		*pCtx = new FfmpegContext{ std::string(filename), std::string(protocol) };
		*error_code = 0;
	}
	catch (FFmpegException ex) {
		*error_code = ex.errorCode();
	}
}

extern "C" DLLEXPORT void DestroyFFmpegContext(void** pCtx) {
	auto _ctx = static_cast<FfmpegContext*>(*pCtx);
	delete _ctx;
}

extern "C" DLLEXPORT void GetFrameSize(
	/* In */ FfmpegContext **ctx, 
	/* out */ int32_t *weigth,
	/* out */ int32_t *heigth
) {
	auto _ctx = static_cast<FfmpegContext*>(*ctx);
	std::tie(*weigth, *heigth) = _ctx->get_frame_size();
}

extern "C" DLLEXPORT void GetFrameCount(
	/* In */ FfmpegContext **ctx,
	/* out */ int32_t *frame_count
) {
	auto _ctx = static_cast<FfmpegContext*>(*ctx);
	*frame_count = _ctx->get_frame_count();
}

extern "C" DLLEXPORT int SkipFrames(
	/* In */ FfmpegContext **ctx,
	/* out */ int64_t frames_to_skip
) {
	if (frames_to_skip > 0) {
		auto _ctx = static_cast<FfmpegContext*>(*ctx);
		if (_ctx->skip_frames(frames_to_skip) < 0) {
			// reopen
			auto fname = _ctx->getFilename();
			auto proto = _ctx->getProtocol();

			DestroyFFmpegContext((void**)ctx);

			try {
				*ctx = new FfmpegContext{ fname, proto };
			}
			catch (FFmpegException ex) {
				return ex.errorCode();
			}
		}
	}
	return 0;
}

extern "C" DLLEXPORT int64_t GetNextFrame(
	/* In */ FfmpegContext **ctx,

	/* out */ uint8_t *pixel_data,
	/* In */  char* timestamp,
	/* out */ int *error_code
) {
	auto _ctx = static_cast<FfmpegContext*>(*ctx);

	try {
		auto result = _ctx->read_frame_to(pixel_data);

		if (timestamp != nullptr) {
			_ctx->print_timestamp_to(timestamp);
		}

		return result;
	} catch (FFmpegException ex) {
		*error_code = ex.errorCode();
		return 0;
	}
}
