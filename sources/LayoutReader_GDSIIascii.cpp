#include "LayoutReader_GDSIIascii.hpp"

#include <ctime>


// [1] http://boolean.klaasholwerda.nl/interface/bnf/gdsformat.html#recordover
// [2] https://github.com/HomerReid/libGDSII/blob/master/lib/ReadGDSIIFile.cc
// [3] https://github.com/icdop/gds2gdt/blob/main/libsrc/libgds/gdsdata.h
// [4] https://www.artwork.com/gdsii/gdsii/index.htm

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
  rt_LINKTYPE     = 0x1D, // or UNUSED ??? [2]
  rt_LINKKEYS     = 0x1E, // or UNUSED ??? [2]
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

  struct Record {
  __int16 length;
  __int8  recordType,
          dataType;
};

struct DateTime {
  __int16 year,
          month,
          day,
          hour,
          minute,
          second;
};

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

union UNION_WORD {
  __int16       value;
  unsigned char byteArray[sizeof(__int16)];
};

union UNION_DWORD {
  __int32       value;
  unsigned char byteArray[sizeof(__int32)];
};

union UNION_DOUBLE {
  double        value;
  unsigned char byteArray[sizeof(double)];
};


GDSIIASCIIReader::GDSIIASCIIReader() : p_activeLibrary(nullptr), p_activeElement(nullptr), p_activeItem(nullptr) {

}

bool GDSIIASCIIReader::IsMyFormat(const std::wstring &fName) {
  fileName = fName;

  return false;

  if (fName.substr(fName.find_last_of(L".") + 1) == L"gds")
    return true;
  if (fName.substr(fName.find_last_of(L".") + 1) == L"gdt")
    return true;  
  if (fName.substr(fName.find_last_of(L".") + 1) == L"txt")
    return true;

  return false;
}

bool GDSIIASCIIReader::Read(Layout *layout) {
  if (!layout)
    return false;
  p_data = layout;

  file.open(fileName, std::ios::in | std::ios::binary);
  if (!file.is_open())
    return false;


  return false;
}
