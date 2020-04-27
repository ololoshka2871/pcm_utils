#include "stdafx.h"
#include "matrix_constants.h"

#if 1
const uint8_t Tm6[14][14]{
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}
};

const uint8_t Tm5[14][14]{
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

const uint8_t Tm4[14][14]{
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

const uint8_t Tm3[14][14]{
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

const uint8_t Tm2[14][14]{
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

const uint8_t Tm1[14][14]{
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

const uint8_t TpIm1[14][14]{
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

const uint8_t T2pm1[14][14]{
	{0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	{0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
	{0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	{0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
	{0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
	{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0},
	{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
	{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}
};

const uint8_t T3pm1[14][14]{
	{0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
	{0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	{0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0},
	{0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1},
	{0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1},
	{1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
	{0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	{0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}
};

const uint8_t T4pm1[14][14]{
	{1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
	{0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
	{1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
	{0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
	{1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0},
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0},
	{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0},
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
	{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
	{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
	{0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}
};

const uint8_t T5pm1[14][14]{
	{1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0},
	{1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1},
	{0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
	{1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
	{1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1},
	{1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0},
	{1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1},
	{0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
	{1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0},
	{0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0},
	{1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1},
	{0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
	{1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
	{1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1}
};
#else
const uint8_t Tm6[14][14]{
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}
};

const uint8_t Tm5[14][14]{
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}
};

const uint8_t Tm4[14][14]{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}
};

const uint8_t Tm3[14][14]{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}
};

const uint8_t Tm2[14][14]{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}
};

const uint8_t Tm1[14][14]{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};

const uint8_t TpIm1[14][14]{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

const uint8_t T2pm1[14][14]{
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
	{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0},
	{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
	{1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
	{0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	{0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
	{0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
	{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}
};

const uint8_t T3pm1[14][14]{
	{0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0},
	{1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	{1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
	{1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
	{0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
	{0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}
};

const uint8_t T4pm1[14][14]{
	{0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
	{0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0},
	{0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
	{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0},
	{0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1},
	{0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	{0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	{0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
	{0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
	{0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
	{0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
	{0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
	{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}
};

const uint8_t T5pm1[14][14]{
	{0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1},
	{1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1},
	{1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0},
	{0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
	{1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1},
	{0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
	{1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1},
	{1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0},
	{0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1},
	{0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0},
	{1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1},
	{1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0},
	{0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
	{1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1}
};
#endif