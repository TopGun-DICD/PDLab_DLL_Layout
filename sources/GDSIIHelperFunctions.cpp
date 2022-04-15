#include "GDSIIHelperFunctions.hpp"
#include "GDSIITypes.hpp"

#include <cmath>
#include <cfloat>

void Normalize_WORD(int16_t &_value) {
  UNION_WORD u;
  unsigned char tempByte = 0;

  u.value = _value;
  tempByte = u.byteArray[1];
  u.byteArray[1] = u.byteArray[0];
  u.byteArray[0] = tempByte;
  _value = u.value;

  if (_value & 0x8000) {
    _value &= 0x7fff;
    _value ^= 0x7fff;
    _value += 1;
    _value *= -1;
  }
}

void DeNormalize_WORD(int16_t &_value) {
  char* const p = reinterpret_cast<char *>(&_value);
  char byte;
  for (size_t i = 0; i < sizeof(int16_t) / 2; ++i) {
    byte = p[i];
    p[i] = p[sizeof(int16_t) - i - 1];
    p[sizeof(int16_t) - i - 1] = byte;
  }
}

void Normalize_DWORD(int32_t &_value) {
  UNION_DWORD u;
  unsigned char tempByte = 0;

  u.value = _value;
  tempByte = u.byteArray[3];
  u.byteArray[3] = u.byteArray[0];
  u.byteArray[0] = tempByte;
  tempByte = u.byteArray[2];
  u.byteArray[2] = u.byteArray[1];
  u.byteArray[1] = tempByte;
  _value = u.value;

  if (_value & 0x80000000) {
    _value &= 0x7fffffff;
    _value ^= 0x7fffffff;
    _value += 1;
    _value *= -1;
  }
}

void DeNormalize_DWORD(int32_t &_value) {
  UNION_DWORD u;
  unsigned char tempByte = 0;

  u.value = _value;
  tempByte = u.byteArray[3];
  u.byteArray[3] = u.byteArray[0];
  u.byteArray[0] = tempByte;
  tempByte = u.byteArray[2];
  u.byteArray[2] = u.byteArray[1];
  u.byteArray[1] = tempByte;
  _value = u.value;
}

void Normalize_DOUBLE(double &_value) {
  UNION_DOUBLE u;
  unsigned char tempByte = 0;

  u.value = _value;

  int realSign = u.byteArray[0] & 0x80;
  int realExponent = (u.byteArray[0] & 0x7f) - 64;

  double realMantissa = 0.0;
  for (int i = 1; i < 8; ++i) {
    realMantissa *= 0x100;
    realMantissa += u.byteArray[i];
  }

  realMantissa /= pow((double)2, 7 * 8);
  realMantissa *= pow((double)16, realExponent);
  if (realSign)
    realMantissa *= -1;

  _value = realMantissa;
}

void DeNormalize_DOUBLE(double &_value) {
  UNION_DOUBLE u;

  int exponent = 0;
  double mantissa = 0.0;

  double in_num = fabs(_value);
  if (fabs(in_num) >= 2.2204460492503131e-016)
    for (exponent = -64; exponent < 64; ++exponent) {
      mantissa = in_num / pow((double)16, exponent);
      if (mantissa <= 1) 
        break;
    }

  exponent += 64;
  if (_value < 0) 
    exponent |= 0x80;

  mantissa *= pow((double)2, 56);
  double int_mantissa = mantissa;
  u.byteArray[0] = ((char *)&exponent)[0];

  int i = 1;
  for (int mantissa_ktr = 6; mantissa_ktr >= 0; mantissa_ktr--) {
    double part_mantissa = int_mantissa;
    for (int sm_ktr = 0; sm_ktr < mantissa_ktr; sm_ktr++)
      part_mantissa /= pow((double)2, 8);
    part_mantissa = fmod(part_mantissa, 0x100);
    u.byteArray[i++] = (unsigned char)part_mantissa;
  }

  _value = u.value;
}


