// emphasis.cpp : Определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"

#include <cmath>

#include "emphasis_filters.h"

extern "C"  __declspec(dllexport) void deemphasis(float In_Old, float X1, float *In,
	float *Out, uint32_t len, float *In_Old_OUT, float *X1_OUT, float sampleRate) {
	DE_Emphasis_TypeDef ctx{ In_Old, X1 };
	de_emphasis(&ctx, In, Out, len, sampleRate);
	*In_Old_OUT = ctx.In_Old;
	*X1_OUT = ctx.X1;
}

extern "C"  __declspec(dllexport) void deemphasis_stereo(
	/* In */ int16_t *stereo_data_In,
	/* Out */ int16_t *stereo_data_Out,
	/* In */ int32_t size,
	/* In */ int32_t sample_rate,

	/* In */  double In_Old_L,
	/* In */ double X1_L,
	/* Out */ double *In_old_out_L,
	/* Out */ double *X1_out_L,

	/* In */  double In_Old_R,
	/* In */ double X1_R,
	/* Out */ double *In_old_out_R,
	/* Out */ double *X1_out_R,

	/* config */
	double Magick1,
	double Magick2,
	double Magick3,
	double Magick4,
	uint32_t Magick5,

	double EmphasisT1,
	double EmphasisT2
)
{
	/* load context */
	DE_Emphasis_TypeDef ctx[]{
		{ In_Old_L, X1_L },
		{ In_Old_R, X1_R }
	};

	const Magick_constants m{
		Magick1, Magick2, Magick3, Magick4, Magick5,
		EmphasisT1, EmphasisT2
	};

	for (auto i = 0; i < size; ++i) {
		double sample = stereo_data_In[i];
		double res;

		de_emphasis_sample(ctx[i & 1], sample, res, sample_rate, m);

		stereo_data_Out[i] = std::round(res);
	}

	/* save context */
	*In_old_out_L = ctx[0].In_Old;
	*X1_out_L = ctx[0].X1;

	*In_old_out_R = ctx[1].In_Old;
	*X1_out_R = ctx[1].X1;
}