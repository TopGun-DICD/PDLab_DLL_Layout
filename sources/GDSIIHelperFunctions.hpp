#pragma once

#include <cstdint>

// To access bits of a bit array
#define  GET_BIT0 (data) (data & 0x001)
#define  GET_BIT1 (data) (data & 0x002)
#define  GET_BIT2 (data) (data & 0x004)
#define  GET_BIT3 (data) (data & 0x008)
#define  GET_BIT4 (data) (data & 0x010)
#define  GET_BIT5 (data) (data & 0x020)
#define  GET_BIT6 (data) (data & 0x040)
#define  GET_BIT7 (data) (data & 0x080)
#define  GET_BIT8 (data) (data & 0x100)
#define  GET_BIT9 (data) (data & 0x200)
#define  GET_BIT10(data) (data & 0x400)
#define  GET_BIT11(data) (data & 0x800)
#define  GET_BIT12(data) (data & 0x1000)
#define  GET_BIT13(data) (data & 0x2000)
#define  GET_BIT14(data) (data & 0x4000)
#define  GET_BIT15(data) (data & 0x8000)

void Normalize_WORD(int16_t &_value);
void DeNormalize_WORD(int16_t &_value);

void Normalize_DWORD(int32_t &_value);
void DeNormalize_DWORD(int32_t &_value);

void Normalize_DOUBLE(double &_value);
void DeNormalize_DOUBLE(double &_value);

