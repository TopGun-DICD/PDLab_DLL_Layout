#include "LayoutReader_GDSIIbin.hpp"

#include <ctime>

#pragma warning(disable: 26451) // Warning C26451 Arithmetic overflow : Using operator '+' on a 4 byte value and then casting the result to a 8 byte value. 
                                // Cast the value to the wider type before calling operator '+' to avoid overflow(io.2).


// [1] http://boolean.klaasholwerda.nl/interface/bnf/gdsformat.html#recordover
// [2] https://github.com/HomerReid/libGDSII/blob/master/lib/ReadGDSIIFile.cc
// [3] https://github.com/icdop/gds2gdt/blob/main/libsrc/libgds/gdsdata.h
// [4] https://www.artwork.com/gdsii/gdsii/index.htm

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

void Normalize_WORD(__int16 &_value) {
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

void Normalize_DWORD(__int32 &_value) {
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


GDSIIBinaryReader::GDSIIBinaryReader() : p_activeLibrary(nullptr), p_activeElement(nullptr), p_activeItem(nullptr) {

}

bool GDSIIBinaryReader::IsMyFormat(const std::wstring &fName) {
  fileName = fName;

  if (fName.substr(fName.find_last_of(L".") + 1) == L"gds")
    return true;
  if (fName.substr(fName.find_last_of(L".") + 1) == L"gdsii")
    return true;

  return false;
}

bool GDSIIBinaryReader::Read(Layout *layout) {
  if (!layout)
    return false;
  p_data = layout;

  file.open(fileName, std::ios::in | std::ios::binary);
  if (!file.is_open())
    return false;

  Record gdsiiRecord;
  while (true) {

    file.read(reinterpret_cast<char *>(&gdsiiRecord), sizeof(Record));
    if (file.eof())
      break;
    Normalize_WORD(gdsiiRecord.length);
    gdsiiRecord.length -= sizeof(Record);

    switch (gdsiiRecord.recordType) {
      case rt_HEADER      : ReadSection_HEADER(gdsiiRecord);          break;
      case rt_BGNLIB      : ReadSection_BEGINLIBRARY(gdsiiRecord);    break;
      case rt_LIBNAME     : ReadSection_LIBNAME(gdsiiRecord);         break;
      case rt_UNITS       : ReadSection_UNITS(gdsiiRecord);           break;
      case rt_ENDLIB      : ReadSection_ENDLIBRARY(gdsiiRecord);      break;
      case rt_BGNSTR      : ReadSection_BEGINSTRUCTURE(gdsiiRecord);  break;
      case rt_STRNAME     : ReadSection_STRNAME(gdsiiRecord);         break;
      case rt_ENDSTR      : ReadSection_ENDSTRUCTURE(gdsiiRecord);    break;
      case rt_BOUNDARY    : ReadSection_BOUNDARY(gdsiiRecord);        break;
      case rt_PATH        : ReadSection_PATH(gdsiiRecord);            break;
      case rt_SREF        : ReadSection_SREF(gdsiiRecord);            break;
      //case rt_AREF        : ReadSection_AREF(gdsiirecord);            break;
      case rt_TEXT        : ReadSection_TEXT(gdsiiRecord);            break;
      case rt_LAYER       : ReadSection_LAYER(gdsiiRecord);           break;
      case rt_DATATYPE    : ReadSection_DATATYPE(gdsiiRecord);        break;
      case rt_WIDTH       : ReadSection_WIDTH(gdsiiRecord);           break;
      case rt_XY          : ReadSection_XY(gdsiiRecord);              break;
      case rt_ENDEL       : ReadSection_ENDELEMENT(gdsiiRecord);      break;
      case rt_SNAME       : ReadSection_SNAME(gdsiiRecord);           break;
      //case rt_COLROW      : ReadSection_COLROW(gdsiirecord);          break;
      //case rt_TEXTNODE    : ReadSection_TEXTNODE(gdsiiRecord);        break;
      //case rt_NODE        : ReadSection_NODE(gdsiiRecord);            break;
      case rt_TEXTTYPE    : ReadSection_TEXTTYPE(gdsiiRecord);        break;
      case rt_PRESENTATION: ReadSection_PRESENTATION(gdsiiRecord);    break;
      case rt_STRING      : ReadSection_STRING(gdsiiRecord);          break;
      case rt_STRANS      : ReadSection_STRANS(gdsiiRecord);          break;
      case rt_MAG         : ReadSection_MAG(gdsiiRecord);             break;
      //case rt_ANGLE       : ReadSection_ANGLE(gdsiirecord);           break;
      //case rt_LINKTYPE    : ReadSection_LINKTYPE(gdsiirecord);        break;
      //case rt_LINKKEYS    : ReadSection_LINKKEYS(gdsiirecord);        break;
      //case rt_REFLIBS     : ReadSection_REFLIBS(gdsiirecord);         break;
      //case rt_FONTS       : ReadSection_FONTS(gdsiirecord);           break;
      case rt_PATHTYPE    : ReadSection_PATHTYPE(gdsiiRecord);        break;
      //case rt_GENERATIONS : ReadSection_GENERATIONS(gdsiirecord);     break;
      //case rt_ATTRTABLE   : ReadSection_ATTRTABLE(gdsiirecord);       break;
      //case rt_STYPTABLE   : ReadSection_STYPTABLE(gdsiirecord);       break;
      //case rt_STRTYPE     : ReadSection_STRTYPE(gdsiirecord);         break;
      //case rt_ELFLAGS     : ReadSection_ELFLAGS(gdsiirecord);         break;
      //case rt_ELKEY       : ReadSection_ELKEY(gdsiirecord);           break;
      // UNUSED
      // UNUSED
      //case rt_NODETYPE    : ReadSection_NODETYPE(gdsiirecord);        break;
      case rt_PROPATTR    : ReadSection_PROPATTR(gdsiiRecord);        break;
        //void ReadSection_PROPVALUE(Record &_record);
      case rt_BOX         : ReadSection_BOX(gdsiiRecord);             break;
      case rt_BOXTYPE     : ReadSection_BOXTYPE(gdsiiRecord);         break;
        //void ReadSection_PLEX(Record &_record);
        //void ReadSection_BGNEXTN(Record &_record);
        //void ReadSection_ENDTEXTN(Record &_record);
        //void ReadSection_TAPENUM(Record &_record);
        //void ReadSection_TAPECODE(Record &_record);
        //void ReadSection_STRCLASS(Record &_record);
        //void ReadSection_RESERVED(Record &_record);
        //void ReadSection_FORMAT(Record &_record);
        //void ReadSection_MASK(Record &_record);
        //void ReadSection_ENDMASK(Record &_record);
        //void ReadSection_LIBDIRSIZE(Record &_record);
        //void ReadSection_SRFNAME(Record &_record);
        //void ReadSection_LIBSECUR(Record &_record);
      default:
        // wrong data
        file.seekg(gdsiiRecord.length, std::ios_base::cur);
      }
  }

  file .close();
  //std::clock_t stopReading = std::clock();
  layout->fileFormat = FileFormat::GDSII_bin;
  return ResolveReferences();
}

void GDSIIBinaryReader::ReadSection_HEADER(Record &_record) {
  //TODO: read version number
  file.seekg(_record.length, std::ios_base::cur);
}


void GDSIIBinaryReader::ReadSection_BEGINLIBRARY(Record &_record) {
  if (p_activeLibrary) {
    //TODO: push error, library already started
    return;
  }
  p_activeLibrary = new Library;
  p_data->libraries.push_back(p_activeLibrary);

  DateTime lastTimeModified, lastTimeAccessed;
  file.read(reinterpret_cast<char *>(&lastTimeModified), sizeof(DateTime));
  file.read(reinterpret_cast<char *>(&lastTimeAccessed), sizeof(DateTime));

  /*
  Normalize___int16(p_activeLibrary->lastTimeModified.year);
  Normalize___int16(p_activeLibrary->lastTimeModified.month);
  Normalize___int16(p_activeLibrary->lastTimeModified.day);
  Normalize___int16(p_activeLibrary->lastTimeModified.hour);
  Normalize___int16(p_activeLibrary->lastTimeModified.minute);
  Normalize___int16(p_activeLibrary->lastTimeModified.second);

  Normalize___int16(p_activeLibrary->lastTimeAccessed.year);
  Normalize___int16(p_activeLibrary->lastTimeAccessed.month);
  Normalize___int16(p_activeLibrary->lastTimeAccessed.day);
  Normalize___int16(p_activeLibrary->lastTimeAccessed.hour);
  Normalize___int16(p_activeLibrary->lastTimeAccessed.minute);
  Normalize___int16(p_activeLibrary->lastTimeAccessed.second);
  */
}

void GDSIIBinaryReader::ReadSection_LIBNAME(Record &_record) {
  if (!p_activeLibrary) {
    ////MessageManager::Get()->PushError("Format error. Found LIBNAME section outside of library.");
    return;
  }
  if (p_activeItem) {
    ////MessageManager::Get()->PushError("Format error. Found LIBNAME section inside of structure.");
    return;
  }
  if (p_activeItem) {
    ////MessageManager::Get()->PushError("Format error. Found LIBNAME section inside of element.");
    return;
  }

  char *str = new char[_record.length + 1];
  memset(str, 0, _record.length + 1);
  file.read(str, _record.length);
  p_activeLibrary->name = str;
  delete[] str;
  str = nullptr;
}

void GDSIIBinaryReader::ReadSection_UNITS(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found UNITS section outside of library.");
    return;
  }
  if (p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found UNITS section inside of structure.");
    return;
  }
  if (p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found UNITS section inside of element.");
    return;
  }

  file.read(reinterpret_cast<char *>(&p_activeLibrary->units), sizeof(Units));

  Normalize_DOUBLE(p_activeLibrary->units.user);
  Normalize_DOUBLE(p_activeLibrary->units.physical);
}

void GDSIIBinaryReader::ReadSection_ENDLIBRARY(Record &_record) {
  //file.seekg(_record.length, std::ios_base::cur);
  // Nothing to do

  p_activeLibrary = nullptr;
}

void GDSIIBinaryReader::ReadSection_BEGINSTRUCTURE(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found BGNSTR section outside of library.");
    return;
  }
  if (p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found BGNSTR section inside of structure.");
    return;
  }
  if (p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found BGNSTR section inside of element.");
    return;
  }

  p_activeElement = new Element;
  p_activeElement->nested = false;
  p_activeLibrary->elements.push_back(p_activeElement);

  DateTime lastTimeModified, lastTimeAccessed;
  file.read(reinterpret_cast<char *>(&lastTimeModified), sizeof(DateTime));
  file.read(reinterpret_cast<char *>(&lastTimeAccessed), sizeof(DateTime));

  /*
  Normalize___int16(p_activeElement->lastTimeModified.year);
  Normalize___int16(p_activeElement->lastTimeModified.month);
  Normalize___int16(p_activeElement->lastTimeModified.day);
  Normalize___int16(p_activeElement->lastTimeModified.hour);
  Normalize___int16(p_activeElement->lastTimeModified.minute);
  Normalize___int16(p_activeElement->lastTimeModified.second);

  Normalize___int16(p_activeElement->lastTimeAccessed.year);
  Normalize___int16(p_activeElement->lastTimeAccessed.month);
  Normalize___int16(p_activeElement->lastTimeAccessed.day);
  Normalize___int16(p_activeElement->lastTimeAccessed.hour);
  Normalize___int16(p_activeElement->lastTimeAccessed.minute);
  Normalize___int16(p_activeElement->lastTimeAccessed.second);
  */
}

void GDSIIBinaryReader::ReadSection_STRNAME(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found STRNAME section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found STRNAME section outside of structure.");
    return;
  }
  if (p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found STRNME section inside of element.");
    return;
  }

  char *str = new char[_record.length + 1];
  memset(str, 0, _record.length + 1);
  file.read(str, _record.length);
  p_activeElement->name = str;
  delete[] str;
  str = nullptr;
}

void GDSIIBinaryReader::ReadSection_ENDSTRUCTURE(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found ENDSTR section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found ENDSTR section outside of structure.");
    return;
  }
  if (p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found ENDSTR section inside of element.");
    return;
  }

  p_activeElement->min.x = p_activeElement->items[0]->coords[0].x;
  p_activeElement->min.y = p_activeElement->items[0]->coords[0].y;
  p_activeElement->max = p_activeElement->min;

  for (size_t i = 0; i < p_activeElement->items.size(); ++i) {
    if (p_activeElement->items[i]->type == GeometryType::reference)
      continue;
    for (size_t j = 0; j < p_activeElement->items[i]->coords.size(); ++j) {
      if (p_activeElement->min.x > p_activeElement->items[i]->coords[j].x)
        p_activeElement->min.x = p_activeElement->items[i]->coords[j].x;
      if (p_activeElement->min.y > p_activeElement->items[i]->coords[j].y)
        p_activeElement->min.y = p_activeElement->items[i]->coords[j].y;
      if (p_activeElement->max.x < p_activeElement->items[i]->coords[j].x)
        p_activeElement->max.x = p_activeElement->items[i]->coords[j].x;
      if (p_activeElement->max.y < p_activeElement->items[i]->coords[j].y)
        p_activeElement->max.y = p_activeElement->items[i]->coords[j].y;
    }
  }

  p_activeElement = nullptr;
}

void GDSIIBinaryReader::ReadSection_BOUNDARY(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found BOUNDARY section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found BOUNDARY section outside of structure.");
    return;
  }
  if (p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found BOUNDARY section inside of element.");
    return;
  }

  p_activeItem = new Polygon;
  p_activeItem->type = GeometryType::polygon;
  p_activeItem->layer = -1;
  static_cast<Polygon *>(p_activeItem)->dataType = 0;
  p_activeElement->items.push_back(p_activeItem);
}

void GDSIIBinaryReader::ReadSection_PATH(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found PATH section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found PATH section outside of structure.");
    return;
  }
  if (p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found PATH section inside of element.");
    return;
  }

  p_activeItem = new Path;
  p_activeItem->type = GeometryType::path;
  p_activeItem->layer = -1;
  static_cast<Path *>(p_activeItem)->dataType = 0;
  static_cast<Path *>(p_activeItem)->pathType = 0;
  static_cast<Path *>(p_activeItem)->width = 0;

  p_activeElement->items.push_back(p_activeItem);
}

void GDSIIBinaryReader::ReadSection_SREF(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found SREF section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found SREF section outside of structure.");
    return;
  }
  if (p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found SREF section inside of element.");
    return;
  }

  p_activeItem = new Reference;
  p_activeItem->type = GeometryType::reference;
  p_activeItem->layer = -1;
  static_cast<Reference *>(p_activeItem)->name.erase();
  static_cast<Reference *>(p_activeItem)->referenceTo = nullptr;
  static_cast<Reference *>(p_activeItem)->transformationFlags = 0;
  static_cast<Reference *>(p_activeItem)->magnification = 1.0;

  p_activeElement->items.push_back(p_activeItem);
}

//void ReadSection_AREF(Record &_record);

void GDSIIBinaryReader::ReadSection_TEXT(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found TEXT section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found TEXT section outside of structure.");
    return;
  }
  if (p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found TEXT section inside of element.");
    return;
  }

  p_activeItem = new Text;
  p_activeItem->type = GeometryType::text;
  p_activeItem->layer = -1;
  static_cast<Text *>(p_activeItem)->textType = 0;
  static_cast<Text *>(p_activeItem)->flagsPresentation = 0;
  static_cast<Text *>(p_activeItem)->flagsTransformation = 0;
  static_cast<Text *>(p_activeItem)->pathType = 0;
  static_cast<Text *>(p_activeItem)->width = 0;
  static_cast<Text *>(p_activeItem)->magnification = 1.0;
  p_activeElement->items.push_back(p_activeItem);
}

void GDSIIBinaryReader::ReadSection_LAYER(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found LAYER section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found LAYER section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found LAYER section outside of element.");
    return;
  }

  file.read(reinterpret_cast<char *>(&p_activeItem->layer), sizeof(__int16));
  Normalize_WORD(p_activeItem->layer);
}

void GDSIIBinaryReader::ReadSection_DATATYPE(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found DATATYPE section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found DATATYPE section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found DATATYPE section outside of element.");
    return;
  }

  __int16 dataType = 0;
  file.read(reinterpret_cast<char *>(&dataType), sizeof(__int16));
  Normalize_WORD(dataType);

  switch (p_activeItem->type) {
    case GeometryType::polygon:
      static_cast<Polygon *>(p_activeItem)->dataType = dataType;
      break;
    case GeometryType::path:
      static_cast<Path *>(p_activeItem)->dataType = dataType;
      break;
    default:
      ;
      //TODO: push error that this element does not support property datatype
  }
}

void GDSIIBinaryReader::ReadSection_WIDTH(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found WIDTH section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found WIDTH section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found WIDTH section outside of element.");
    return;
  }

  __int32 width = 0;
  file.read(reinterpret_cast<char *>(&width), sizeof(__int32));
  Normalize_DWORD(width);

  switch (p_activeItem->type) {
    case GeometryType::path:
      static_cast<Path *>(p_activeItem)->width = width;
      break;
    case GeometryType::text:
      static_cast<Text *>(p_activeItem)->width = width;
      break;
    default:
      ;
      //MessageManager::Get()->PushError("Format error. Found WIDTH section given for inproper type of element.");
  }
}

void GDSIIBinaryReader::ReadSection_XY(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found XY section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found XY section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found XY section outside of element.");
    return;
  }

  int numberOfCoors = _record.length / sizeof(Coord);

  Polygon   *p_boundary  = nullptr;
  Path      *p_path      = nullptr;
  Rectangle *p_box       = nullptr;
  Reference *p_reference = nullptr;
  Text      *p_text      = nullptr;
  Coord               coord = {0, 0};
  int                 i = 0;

  switch (p_activeItem->type) {
    case GeometryType::polygon:
      p_boundary = static_cast<Polygon *>(p_activeItem);
      p_boundary->coords.resize(numberOfCoors);
      for (i = 0; i < numberOfCoors; ++i) {
        file.read(reinterpret_cast<char *>(&coord), sizeof(Coord));

        Normalize_DWORD(coord.x);
        Normalize_DWORD(coord.y);

        p_boundary->coords[i] = coord;
      }
      break;
    case GeometryType::path:
      p_path = static_cast<Path *>(p_activeItem);
      p_path->coords.resize(numberOfCoors);
      for (i = 0; i < numberOfCoors; ++i) {
        file.read(reinterpret_cast<char *>(&coord), sizeof(Coord));

        Normalize_DWORD(coord.x);
        Normalize_DWORD(coord.y);

        p_path->coords[i] = coord;
      }
      break;
    case GeometryType::rectangle:
      p_box = static_cast<Rectangle *>(p_activeItem);
      p_box->coords.resize(numberOfCoors);
      for (i = 0; i < numberOfCoors; ++i) {
        file.read(reinterpret_cast<char *>(&coord), sizeof(Coord));

        Normalize_DWORD(coord.x);
        Normalize_DWORD(coord.y);

        p_box->coords[i] = coord;
      }
      break;
    case GeometryType::reference:
      p_reference = static_cast<Reference *>(p_activeItem);

      file.read(reinterpret_cast<char *>(&coord), sizeof(Coord));
      Normalize_DWORD(coord.x);
      Normalize_DWORD(coord.y);

      p_reference->coords.push_back(coord);
      break;
    case GeometryType::text:
      p_text = static_cast<Text *>(p_activeItem);

      file.read(reinterpret_cast<char *>(&coord), sizeof(Coord));
      Normalize_DWORD(coord.x);
      Normalize_DWORD(coord.y);

      p_text->coords.push_back(coord);
      break;
    default:
      ;
      //MessageManager::Get()->PushError("Format error. Found XY section given for inproper type of element.");
  }
}

void GDSIIBinaryReader::ReadSection_ENDELEMENT(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found ENDELEMENT section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found ENDELEMENT section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found ENDELEMENT section outside of element.");
    return;
  }

  p_activeItem = nullptr;
}

void GDSIIBinaryReader::ReadSection_SNAME(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found SNAME section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found SNAME section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found SNAME section outside of element.");
    return;
  }
  if (p_activeItem->type != GeometryType::reference) {
    //MessageManager::Get()->PushError("Format error. Found SNAME section given for inproper type of element.");
    return;
  }

  char *str = new char[_record.length + 1];
  memset(str, 0, _record.length + 1);
  file.read(str, _record.length);
  static_cast<Reference *>(p_activeItem)->name = str;
  delete[] str;
  str = nullptr;
}

//void GDSIIBinaryReader::ReadSection_COLROW(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_TEXTNODE(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_NODE(Record &_record) {}

void GDSIIBinaryReader::ReadSection_TEXTTYPE(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found TEXTTYPE section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found TEXTTYPE section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found TEXTTYPE section outside of element.");
    return;
  }
  if (p_activeItem->type != GeometryType::text) {
    //MessageManager::Get()->PushError("Format error. Found TEXTTYPE section given for inproper type of element.");
    return;
  }

  __int16 type;
  file.read(reinterpret_cast<char *>(&type), sizeof(__int16));
  Normalize_WORD(type);

  static_cast<Text *>(p_activeItem)->textType = type;
}

void GDSIIBinaryReader::ReadSection_PRESENTATION(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found PRESENTATION section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found PRESENTATION section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found PRESENTATION section outside of element.");
    return;
  }
  if (p_activeItem->type != GeometryType::text) {
    //MessageManager::Get()->PushError("Format error. Found PRESENTATION section given for inproper type of element.");
    return;
  }

  __int16 flags;
  file.read(reinterpret_cast<char *>(&flags), sizeof(__int16));
  Normalize_WORD(flags);

  static_cast<Text *>(p_activeItem)->flagsPresentation = flags;
}

// UNUSED

void GDSIIBinaryReader::ReadSection_STRING(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found STRING section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found STRING section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found STRING section outside of element.");
    return;
  }
  if (p_activeItem->type != GeometryType::text) {
    //MessageManager::Get()->PushError("Format error. Found STRING section given for inproper type of element.");
    return;
  }

  char *str = new char[_record.length + 1];
  memset(str, 0, _record.length + 1);
  file.read(str, _record.length);
  static_cast<Text *>(p_activeItem)->stringValue = str;
  delete[] str;
  str = nullptr;
}

void GDSIIBinaryReader::ReadSection_STRANS(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found STRANS section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found STRANS section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found STRANS section outside of element.");
    return;
  }

  __int16 flags;
  file.read(reinterpret_cast<char *>(&flags), sizeof(__int16));
  Normalize_WORD(flags);

  switch (p_activeItem->type) {
    case GeometryType::reference:
      //static_cast<GDSII_StructureRef *>(p_activeItem)-> = flags;
      break;
    //case it_arrayRef:
      //static_cast<GDSII_ArrayRef *>(p_activeItem)-> = flags;
      //break;
    case GeometryType::text:
      static_cast<Text *>(p_activeItem)->flagsTransformation = flags;
      break;
    default:
      ;
      //MessageManager::Get()->PushError("Format error. Found STRING section given for inproper type of element.");
  }
}

void GDSIIBinaryReader::ReadSection_MAG(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found MAG section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found MAG section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found MAG section outside of element.");
    return;
  }

  double mag = 0.0;
  file.read(reinterpret_cast<char *>(&mag), sizeof(double));
  Normalize_DOUBLE(mag);

  switch (p_activeItem->type) {
    case GeometryType::reference:
      static_cast<Reference *>(p_activeItem)->magnification = mag;
      break;
    //case et_arrayRef:
      //static_cast<GDSII_ArrayRef *>(p_activeItem)->magnification = mag;
      //break;
    case GeometryType::text:
      static_cast<Text *>(p_activeItem)->magnification = mag;
      break;
    default:
      ;
      //MessageManager::Get()->PushError("Format error. Found MAG section given for inproper type of element.");
  }
}

//void GDSIIBinaryReader::ReadSection_ANGLE(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_LINKTYPE(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_LINKKEYS(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_REFLIBS(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_FONTS(Record &_record) {}

void GDSIIBinaryReader::ReadSection_PATHTYPE(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found PATHTYPE section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found PATHTYPE section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found PATHTYPE section outside of element.");
    return;
  }

  __int16 type;
  file.read(reinterpret_cast<char *>(&type), sizeof(__int16));
  Normalize_WORD(type);

  switch (p_activeItem->type) {
    case GeometryType::path:
      static_cast<Path *>(p_activeItem)->pathType = type;
      break;
    case GeometryType::text:
      static_cast<Text *>(p_activeItem)->pathType = type;
      break;
    default:
      ;
      //MessageManager::Get()->PushError("Format error. Found PATHTYPE section given for inproper type of element.");
  }
}

//void GDSIIBinaryReader::ReadSection_GENERATIONS(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_ATTRTABLE(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_STYPTABLE(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_STRTYPE(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_ELFLAGS(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_ELKEY(Record &_record) {}
// UNUSED
// UNUSED
//void GDSIIBinaryReader::ReadSection_NODETYPE(Record &_record) {}

void GDSIIBinaryReader::ReadSection_PROPATTR(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found PROPATTR section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found PROPATTR section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found PROPATTR section outside of element.");
    return;
  }
  if (!p_activeItem->properties.empty() && p_activeItem->properties.back().value.empty()) {
    //MessageManager::Get()->PushError("Format error. Found next PROPATTR section before previous PROPVALUE section was met.");
    return;
  }

  Property prop = { 0, "" };
  file.read(reinterpret_cast<char *>(&prop.index), sizeof(__int16));
  Normalize_WORD(prop.index);

  p_activeItem->properties.push_back(prop);
  //p_activeItem->readingProperty = true;
}

void GDSIIBinaryReader::ReadSection_PROPVALUE(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found PROPVALUE section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found PROPVALUE section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found PROPVALUE section outside of element.");
    return;
  }
  if (!p_activeItem->properties.empty() && p_activeItem->properties.back().value.empty()) {
    //MessageManager::Get()->PushError("Format error. Found PROPVALUE section before PROPATTR section was met.");
    return;
  }

  char *str = new char[_record.length + 1];
  memset(str, 0, _record.length + 1);
  file.read(str, _record.length);
  p_activeItem->properties[p_activeItem->properties.size() - 1].value = str;
  delete[] str;
  str = nullptr;

  //p_activeItem->readingProperty = false;
}

void GDSIIBinaryReader::ReadSection_BOX(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found BOX section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found BOX section outside of structure.");
    return;
  }
  if (p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found BOX section inside of element.");
    return;
  }

  p_activeItem = new Rectangle;
  p_activeItem->type = GeometryType::rectangle;
  p_activeItem->layer = -1;
  static_cast<Rectangle *>(p_activeItem)->boxType = 0;
  p_activeElement->items.push_back(p_activeItem);
}

void GDSIIBinaryReader::ReadSection_BOXTYPE(Record &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found BOXTYPE section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found BOXTYPE section outside of structure.");
    return;
  }
  if (!p_activeItem) {
    //MessageManager::Get()->PushError("Format error. Found BOXTYPE section outside of element.");
    return;
  }
  if (p_activeItem->type != GeometryType::rectangle) {
    //MessageManager::Get()->PushError("Format error. Found BOXTYPE section given for wrong type of element.");
    return;
  }

  __int16 type = 0;
  file.read(reinterpret_cast<char *>(&type), sizeof(__int16));
  Normalize_WORD(type);

  static_cast<Rectangle *>(p_activeItem)->boxType = type;
}

//void GDSIIBinaryReader::ReadSection_PLEX(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_BGNEXTN(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_ENDTEXTN(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_TAPENUM(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_TAPECODE(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_STRCLASS(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_RESERVED(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_FORMAT(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_MASK(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_ENDMASK(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_LIBDIRSIZE(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_SRFNAME(Record &_record) {}
//void GDSIIBinaryReader::ReadSection_LIBSECUR(Record &_record) {}

bool GDSIIBinaryReader::ResolveReferences() {
  Reference  *p_reference = nullptr;
  bool                 refFound = false;

  // Structure references
  for (size_t i = 0; i < p_data->libraries.size(); ++i)
    for (size_t j = 0; j < p_data->libraries[i]->elements.size(); ++j)
      for (size_t k = 0; k < p_data->libraries[i]->elements[j]->items.size(); ++k) {
        if (p_data->libraries[i]->elements[j]->items[k]->type != GeometryType::reference)
          continue;
        p_reference = static_cast<Reference *>(p_data->libraries[i]->elements[j]->items[k]);
        refFound = false;
        for (size_t l = 0; l < p_data->libraries.size() && !refFound; ++l)
          for (size_t m = 0; m < p_data->libraries[l]->elements.size() && !refFound; ++m) {
            if (p_data->libraries[l]->elements[m]->name == p_reference->name) {
              p_reference->referenceTo = p_data->libraries[l]->elements[m];
              p_data->libraries[l]->elements[m]->nested = true;
              refFound = true;
              break;
            }
          }
        if (!refFound)
          return false;
      }

  // Filling library->layers information
  Library *p_lib = nullptr;
  for (size_t i = 0; i < p_data->libraries.size(); ++i) {
    p_lib = p_data->libraries[i];
    for (size_t j = 0; j < p_lib->elements.size(); ++j) {
      for (size_t k = 0; k < p_lib->elements[j]->items.size(); ++k) {
        if (p_lib->elements[j]->items[k]->type == GeometryType::reference)
          continue;
        int layer = p_lib->elements[j]->items[k]->layer;
        size_t l = 0;
        for(; l < p_lib->layers.size(); ++l)
          if (p_lib->layers[l].layer == layer) {
            p_lib->layers[l].items.push_back(p_lib->elements[j]->items[k]);
            break;
          }
        if (l < p_lib->layers.size())
          continue;
        Layer li;
        li.layer = layer;
        li.items.push_back(p_lib->elements[j]->items[k]);
        p_lib->layers.push_back(li);
      }
    }
  }

  // Calculate bounding box for the library based on bounding boxes of the elements that are not nested
  for (size_t i = 0; i < p_data->libraries.size(); ++i) {
    
    for (size_t j = 0; j < p_data->libraries[i]->elements.size(); ++j) {
      if (p_data->libraries[i]->elements[j]->nested)
        continue;

      p_data->libraries[i]->min = p_data->libraries[i]->elements[j]->min;
      p_data->libraries[i]->max = p_data->libraries[i]->elements[j]->max;

      if (p_data->libraries[i]->min.x > p_data->libraries[i]->elements[j]->min.x)
        p_data->libraries[i]->min.x = p_data->libraries[i]->elements[j]->min.x;
      if (p_data->libraries[i]->min.y > p_data->libraries[i]->elements[j]->min.y)
        p_data->libraries[i]->min.y = p_data->libraries[i]->elements[j]->min.y;
      if (p_data->libraries[i]->max.x > p_data->libraries[i]->elements[j]->max.x)
        p_data->libraries[i]->max.x = p_data->libraries[i]->elements[j]->max.x;
      if (p_data->libraries[i]->max.y > p_data->libraries[i]->elements[j]->max.y)
        p_data->libraries[i]->max.y = p_data->libraries[i]->elements[j]->max.y;
    }
  }

  return true;
}
