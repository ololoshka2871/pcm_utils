// emphasis.cpp : Определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"

#include <cmath>
#include <vector>
#include <limits>

#include "LabViewArray.h"

#include "emphasis_filters2.h"

#define Shift16s (int64_t) 0x0000800000000000	// Переменная для преобразования int64_t в int16_t (делением, эквивалент >>48)

struct deemphasis_stereo2_context {
	SINC_Interpolation_TypeDef SINC_1, SINC_2;
	Emphasis_TypeDef EMP_1, EMP_2;

	deemphasis_stereo2_context() : SINC_1{}, SINC_2{}, EMP_1{}, EMP_2{} {}

	deemphasis_stereo2_context& operator=(deemphasis_stereo2_context& lr) {
		std::memcpy(&SINC_1, &lr.SINC_1, sizeof(SINC_Interpolation_TypeDef));
		std::memcpy(&SINC_2, &lr.SINC_2, sizeof(SINC_Interpolation_TypeDef));

		EMP_1 = lr.EMP_1;
		EMP_2 = lr.EMP_2;

		return *this;
	}
};

extern "C" __declspec(dllexport) void allocate_deemphasis_stereo2_context(void** pCtx) {
	*pCtx = new deemphasis_stereo2_context{};
}

extern "C" __declspec(dllexport) void deallocate_deemphasis_stereo2_context(void** pCtx) {
	deemphasis_stereo2_context* _ctx = static_cast<deemphasis_stereo2_context*>(*pCtx);

	delete _ctx;
}

static float normalise_value(int16_t v) {
	return (static_cast<int32_t>(v) << 16) / static_cast<float>(INT_MAX);
}

static void demux_stereo(int16_t *stereo_data_In, int32_t size, 
	std::vector<float>::iterator l_it,
	std::vector<float>::iterator r_it) {
	for (auto i = 0; i < size; i += 2, ++l_it, ++r_it) {
		*l_it = normalise_value(stereo_data_In[i]);
		*r_it = normalise_value(stereo_data_In[i + 1]);
	}
}

void filter(std::vector<float> &L_channel_data, deemphasis_stereo2_context * ctx_out,
	std::vector<float> &R_channel_data, const int32_t &sample_rate)
{
	for (uint32_t loop = 0; loop < L_channel_data.size(); ++loop) {
		float sinc_out1[2];
		float sinc_out2[2];

		sinc_x2(&ctx_out->SINC_1, &L_channel_data.at(loop), sinc_out1, 1);
		sinc_x2(&ctx_out->SINC_2, &R_channel_data.at(loop), sinc_out2, 1);

		de_emphasis(&ctx_out->EMP_1, sinc_out1, sinc_out1, 2, static_cast<float>(sample_rate * 2));
		de_emphasis(&ctx_out->EMP_2, sinc_out2, sinc_out2, 2, static_cast<float>(sample_rate * 2));
		L_channel_data.at(loop) = sinc_out1[1];
		R_channel_data.at(loop) = sinc_out2[1];
	}
}

uint64_t fix_overflow(float f) {
	if (f > 1.0f) {
		return INT64_MAX / 2;
	}
	else if(f < -1.0f) {
		return INT64_MIN / 2;
	}

	return f * (float)0x4000000000000000;
}

void generate_result(int16_t *stereo_data_Out, int32_t size,
	std::vector<float>::iterator l_it,
	std::vector<float>::iterator r_it) {

	int64_t error_integrator_L = 0, error_integrator_R = 0;
	for (auto i = 0; i < size; i += 2, ++l_it, ++r_it)	{
		int64_t L = fix_overflow(*l_it);
		int64_t R = fix_overflow(*r_it);

		auto A = (int32_t)(L / Shift16s);
		/*
		error_integrator_L += L % Shift16s;
		if (error_integrator_L >= Shift16s && A < INT16_MAX) { 
			A += 1;
			error_integrator_L -= Shift16s;
		}
		*/

		auto B = (int32_t)(R / Shift16s);
		//auto B = static_cast<int32_t>(R / Shift16s); <-- not this!!!!!
		/*
		error_integrator_R += R % Shift16s;
		if (error_integrator_R >= Shift16s && B < INT16_MAX) {
			B += 1;
			error_integrator_R -= Shift16s;
		}
		*/

		stereo_data_Out[i] = A;
		stereo_data_Out[i + 1] = B;
	}
}

extern "C" __declspec(dllexport) void deemphasis_stereo2(
	/* In */ int16_t *stereo_data_In,
	/* Out */ int16_t *stereo_data_Out,
	/* In */ int32_t size,
	/* In */ int32_t sample_rate,

	/* In */ deemphasis_stereo2_context **ctx
) {
	std::vector<float> L_channel_data(size / 2), R_channel_data(size / 2);

	demux_stereo(stereo_data_In, size, L_channel_data.begin(), R_channel_data.begin());
	filter(L_channel_data, *ctx, R_channel_data, sample_rate);
	generate_result(stereo_data_Out, size, L_channel_data.begin(), R_channel_data.begin());
}