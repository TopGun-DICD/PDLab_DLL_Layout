#pragma once

// [1] http://boolean.klaasholwerda.nl/interface/bnf/gdsformat.html#recordover
// [2] https://github.com/HomerReid/libGDSII/blob/master/lib/ReadGDSIIFile.cc
// [3] https://www.artwork.com/gdsii/gdsii/index.htm
// [4] https://gitlab.com/stone.code/liblayout/-/blob/master/gdsii_write.cpp
// [5] https://github.com/heitzmann/gdstk/blob/master/src/gdsii.h

struct GDSIIRecord {
  int16_t length;
  int8_t  recordType,
          dataType;
};

struct DateTime {
  int16_t year,
          month,
          day,
          hour,
          minute,
          second;
};

enum DataType {
  noData = 0,
  bitArray,
  WORD,
  DWORD,
  REAL,
  DOUBLE,
  ASCIISTRING,
};

enum RecordType {
  rt_HEADER       = 0x00,
  rt_BGNLIB       = 0x01,
  rt_LIBNAME      = 0x02,
  rt_UNITS        = 0x03,
  rt_ENDLIB       = 0x04,
  rt_BGNSTR       = 0x05,
  rt_STRNAME      = 0x06,
  rt_ENDSTR       = 0x07,
  rt_BOUNDARY     = 0x08,
  rt_PATH         = 0x09,
  rt_SREF         = 0x0A,
  rt_AREF         = 0x0B,
  rt_TEXT         = 0x0C,
  rt_LAYER        = 0x0D,
  rt_DATATYPE     = 0x0E,
  rt_WIDTH        = 0x0F,
  rt_XY           = 0x10,
  rt_ENDEL        = 0x11,
  rt_SNAME        = 0x12,
  rt_COLROW       = 0x13,
  rt_TEXTNODE     = 0x14,
  rt_NODE         = 0x15,
  rt_TEXTTYPE     = 0x16,
  rt_PRESENTATION = 0x17,
  // UNUSED       = 0x18,
  rt_STRING       = 0x19,
  rt_STRANS       = 0x1A,
  rt_MAG          = 0x1B,
  rt_ANGLE        = 0x1C,
  //rt_LINKTYPE     = 0x1D, // or UNUSED ??? [2]
  //rt_LINKKEYS     = 0x1E, // or UNUSED ??? [2]
  rt_REFLIBS      = 0x1F,
  rt_FONTS        = 0x20,
  rt_PATHTYPE     = 0x21,
  rt_GENERATIONS  = 0x22,
  rt_ATTRTABLE    = 0x23,
  rt_STYPTABLE    = 0x24,
  rt_STRTYPE      = 0x25,
  rt_ELFLAGS      = 0x26,
  rt_ELKEY        = 0x27,
  // UNUSED       = 0x28, or rt_LINKTYPE ??? [2]
  // UNUSED       = 0x29, or rt_LINKKEYS ??? [2]
  rt_NODETYPE     = 0x2A,
  rt_PROPATTR     = 0x2B,
  rt_PROPVALUE    = 0x2C,
  rt_BOX          = 0x2D,
  rt_BOXTYPE      = 0x2E,
  rt_PLEX         = 0x2F,
  rt_BGNEXTN      = 0x30,
  rt_ENDTEXTN     = 0x31,
  rt_TAPENUM      = 0x32,
  rt_TAPECODE     = 0x33,
  rt_STRCLASS     = 0x34,
  rt_RESERVED     = 0x35,
  rt_FORMAT       = 0x36,
  rt_MASK         = 0x37,
  rt_ENDMASK      = 0x38,
  rt_LIBDIRSIZE   = 0x39,
  rt_SRFNAME      = 0x3A,
  rt_LIBSECUR     = 0x3b,
};

union UNION_WORD {
  int16_t       value;
  unsigned char byteArray[sizeof(int16_t)];
};

union UNION_DWORD {
  int32_t       value;
  unsigned char byteArray[sizeof(int32_t)];
};

union UNION_DOUBLE {
  double        value;
  unsigned char byteArray[sizeof(double)];
};
