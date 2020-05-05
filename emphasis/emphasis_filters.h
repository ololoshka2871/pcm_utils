/*!****************************************************************************
* @file    emphasis_filters.h
* @author  INFERION
* @version V1.1
* @date    30.04.2020, INFERION
* @brief   --
*/
#include <stdint.h>
#ifndef emphasis_filters_H
#define emphasis_filters_H

/*!****************************************************************************
* User Defines
*/
#define Emphasis_T1 0.000050f	//t1 = 75us
#define Emphasis_T2 0.000015f	//t1 = 15us

/*!****************************************************************************
* Structures
*/
typedef struct{
	float In_Old;
	float X1;
}PRE_Emphasis_TypeDef;
typedef struct{
	float In_Old;
	float X1;
}DE_Emphasis_TypeDef;

struct Magick_constants {
	double Magick1, Magick2, Magick3, Magick4;
	uint32_t Magick5;
	double EmphasisT1, EmphasisT2;
};

/*!****************************************************************************
* Prototypes for the functions
*/
void pre_emphasis(PRE_Emphasis_TypeDef *Context, float *In, float *Out, uint32_t len, float sampleRate);
void de_emphasis(DE_Emphasis_TypeDef *Context, float *In, float *Out, uint32_t len, float sampleRate);

void de_emphasis_sample(DE_Emphasis_TypeDef &Context, double In, double &Out, float sampleRate, const Magick_constants& m);

#endif //emphas_filters_H
/***************** (C) COPYRIGHT *********** END OF FILE ******** INFERION ****/