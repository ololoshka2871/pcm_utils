/*!****************************************************************************
* @file    emphasis_filters.c
* @author  INFERION
* @version V1.1
* @date    30.04.2020, INFERION
* @brief   --
*/

/*!****************************************************************************
* Include
*/

#include "emphasis_filters.h"

/*****************************************************************************/

void pre_emphasis(PRE_Emphasis_TypeDef *Context, float *In, float *Out, uint32_t len, float sampleRate) {
	float x = 0.125f / (Emphasis_T2*sampleRate);
	float point = 0.50f - 0.15f*x;
	for (uint32_t i = 0; i < len; i++) {
		float step = (In[i] - Context->In_Old)*0.125f;
		for (uint32_t loop = 0; loop < 8; loop++) {
			Context->In_Old += step;
			float k = (Context->In_Old - Context->X1)*x;
			Context->X1 += (Context->In_Old - (Context->X1 + k * point))*x;
		}
		Out[i] = Context->In_Old + (Context->In_Old - Context->X1)*(Emphasis_T1 / Emphasis_T2);
	}
}

void de_emphasis(DE_Emphasis_TypeDef *Context, float *In, float *Out, uint32_t len, float sampleRate) {
	float x = 0.125f / (Emphasis_T1*sampleRate);
	float point = 0.50f - 0.15f*x;
	for (uint32_t i = 0; i < len; i++) {
		float step = (In[i] - Context->In_Old)*0.125f;
		for (uint32_t loop = 0; loop < 8; loop++) {
			Context->In_Old += step;
			float k = (Context->In_Old - Context->X1)*x;
			Context->X1 += (Context->In_Old - (Context->X1 + k * point))*x;
		}
		Out[i] = Context->X1 + Context->In_Old*(Emphasis_T2 / Emphasis_T1);
	}
}

void de_emphasis_sample(DE_Emphasis_TypeDef &Context, double In, double &Out, float sampleRate, const Magick_constants& m) {
	const float x = m.Magick1 / (m.EmphasisT1 * sampleRate);
	const float point = m.Magick2 - m.Magick3 * x;

	const float step = (In - Context.In_Old) * m.Magick4;
	for (uint32_t loop = 0; loop < m.Magick5; loop++) {
		Context.In_Old += step;
		float k = (Context.In_Old - Context.X1)*x;
		Context.X1 += (Context.In_Old - (Context.X1 + k * point))*x;
	}
	Out = Context.X1 + Context.In_Old * (m.EmphasisT2 / m.EmphasisT1);
}

/***************** (C) COPYRIGHT *********** END OF FILE ******** INFERION ****/