/*!****************************************************************************
* @file    emphasis_filters.h
* @author  INFERION
* @version V1.3
* @date    05.05.2020, INFERION
* @brief   --
*/
#include <stdint.h>
#ifndef emphasis_filters_H
#define emphasis_filters_H

/*!****************************************************************************
* User Defines
*/
//x4
#define Pre_Emphasis_T1 0.000042f	//t1 = 50us
#define Pre_Emphasis_T2 0.0000155f	//t1 = 15us
//x2
#define De_Emphasis_T1 0.000050f	//t1 = 50us (49 4x)
#define De_Emphasis_T2 0.000015f	//t1 = 15us (18 4x)

/*!****************************************************************************
* Structures
*/
typedef struct{
	float oversmpl_buffer[128];
	uint32_t ovsmpl_lp;
}SINC_Interpolation_TypeDef;
typedef struct{
	float X1;
}Emphasis_TypeDef;

/*!****************************************************************************
* Prototypes for the functions
*/
void sinc_x2(SINC_Interpolation_TypeDef *Context, float *In, float *Out, uint32_t len);
void sinc_x4(SINC_Interpolation_TypeDef *Context, float *In, float *Out, uint32_t len);
void pre_emphasis(Emphasis_TypeDef *Context, float *In, float *Out, uint32_t len, float sampleRate);
void de_emphasis(Emphasis_TypeDef *Context, float *In, float *Out, uint32_t len, float sampleRate);

#endif //emphas_filters_H
/***************** (C) COPYRIGHT *********** END OF FILE ******** INFERION ****/