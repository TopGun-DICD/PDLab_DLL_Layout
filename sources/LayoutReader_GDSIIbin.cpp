#include "LayoutReader_GDSIIbin.hpp"
#include "GDSIIHelperFunctions.hpp"

#pragma warning(disable: 26451) // Warning C26451 Arithmetic overflow : Using operator '+' on a 4 byte value and then casting the result to a 8 byte value. 
                                // Cast the value to the wider type before calling operator '+' to avoid overflow(io.2).

//!!! Don't touch these lines on comparison
// [1] http://boolean.klaasholwerda.nl/interface/bnf/gdsformat.html#recordover
// [2] https://github.com/HomerReid/libGDSII/blob/master/lib/ReadGDSIIFile.cc
// [3] https://github.com/icdop/gds2gdt/blob/main/libsrc/libgds/gdsdata.h
// [4] https://www.artwork.com/gdsii/gdsii/index.htm
// [5] https://github.com/KLayout/klayout/tree/master/src/plugins/streamers/gds2/db_plugin
// [6] https://github.com/KLayout/klayout/tree/master/src/plugins/streamers/gds2/db_plugin

LayoutReader_GDSIIbin::LayoutReader_GDSIIbin() : p_activeLibrary(nullptr), p_activeElement(nullptr), p_activeGeometry(nullptr) {
}

bool LayoutReader_GDSIIbin::IsMyFormat(const std::wstring &fName) {
  fileName = fName;

  if (fileName.substr(fileName.find_last_of(L".") + 1) != L"gds")
    if (fileName.substr(fileName.find_last_of(L".") + 1) != L"gdsii")
      return false;

  file.open(fileName, std::ios::in | std::ios::binary);
  if (!file.is_open())
    return false;
  GDSIIRecord gdsiiRecord;
  file.read(reinterpret_cast<char*>(&gdsiiRecord), sizeof(GDSIIRecord));
  Normalize_WORD((int16_t&)gdsiiRecord.length);
  gdsiiRecord.length -= sizeof(GDSIIRecord);
  if (gdsiiRecord.recordType != rt_HEADER || gdsiiRecord.dataType != DataType::WORD || gdsiiRecord.length != 2) {
    file.close();
    return false;
  }
  file.seekg(gdsiiRecord.length, std::ios_base::cur);
  file.read(reinterpret_cast<char*>(&gdsiiRecord), sizeof(GDSIIRecord));
  Normalize_WORD((int16_t&)gdsiiRecord.length);
  gdsiiRecord.length -= sizeof(GDSIIRecord);
  if (gdsiiRecord.recordType != rt_BGNLIB || gdsiiRecord.dataType != DataType::WORD || gdsiiRecord.length != 24) {
    file.close();
    return false;
  }
  file.close();
  return true;
}

bool LayoutReader_GDSIIbin::Read(Layout *layout, LayoutReaderOptions *options) {
  if (!layout)
    return false;
  p_layout = layout;
  this->options = options;

  file.open(fileName, std::ios::in | std::ios::binary);
  if (!file.is_open())
    return false;

  GDSIIRecord gdsiiRecord;
  while (true) {

    file.read(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
    if (file.eof())
      break;
    Normalize_WORD((int16_t&)gdsiiRecord.length);
    gdsiiRecord.length -= sizeof(GDSIIRecord);

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
      //case rt_PROPVALUE   : ReadSection_PROPVALUE(GDSIIRecord &_record);   break;
      case rt_BOX         : ReadSection_BOX(gdsiiRecord);             break;
      case rt_BOXTYPE     : ReadSection_BOXTYPE(gdsiiRecord);         break;
      //case rt_PLEX        : ReadSection_PLEX(GDSIIRecord &_record);        break;
      //case rt_BGNEXTN     : ReadSection_BGNEXTN(GDSIIRecord &_record);     break;
      //case ReadSection_ENDTEXTN(GDSIIRecord &_record);
      //case ReadSection_TAPENUM(GDSIIRecord &_record);
      //case ReadSection_TAPECODE(GDSIIRecord &_record);
      //case ReadSection_STRCLASS(GDSIIRecord &_record);
      //case ReadSection_RESERVED(GDSIIRecord &_record);
      //case ReadSection_FORMAT(GDSIIRecord &_record);
      //case ReadSection_MASK(GDSIIRecord &_record);
      //case ReadSection_ENDMASK(GDSIIRecord &_record);
      //case ReadSection_LIBDIRSIZE(GDSIIRecord &_record);
      //case ReadSection_SRFNAME(GDSIIRecord &_record);
      //case ReadSection_LIBSECUR(GDSIIRecord &_record);
      default:
        // wrong data
        file.seekg(gdsiiRecord.length, std::ios_base::cur);
      }
  }

  file .close();
  //std::clock_t stopReading = std::clock();
  layout->fileName    = fileName;
  layout->fileFormat  = FileFormat::GDSII_bin;
  return PostProcessLayout();
}

void LayoutReader_GDSIIbin::ReadSection_HEADER(GDSIIRecord &_record) {
  int16_t versionNumber = 0;
  file.read(reinterpret_cast<char *>(&versionNumber), sizeof(int16_t));
  Normalize_WORD(versionNumber);
}


void LayoutReader_GDSIIbin::ReadSection_BEGINLIBRARY(GDSIIRecord &_record) {
  if (p_activeLibrary) {
    //TODO: push error, library already started
    return;
  }
  p_activeLibrary = new Library;
  p_layout->libraries.push_back(p_activeLibrary);

  DateTime lastTimeModified, lastTimeAccessed;
  file.read(reinterpret_cast<char *>(&lastTimeModified), sizeof(DateTime));
  file.read(reinterpret_cast<char *>(&lastTimeAccessed), sizeof(DateTime));

  Normalize_WORD(lastTimeModified.year);
  Normalize_WORD(lastTimeModified.month);
  Normalize_WORD(lastTimeModified.day);
  Normalize_WORD(lastTimeModified.hour);
  Normalize_WORD(lastTimeModified.minute);
  Normalize_WORD(lastTimeModified.second);

  Normalize_WORD(lastTimeAccessed.year);
  Normalize_WORD(lastTimeAccessed.month);
  Normalize_WORD(lastTimeAccessed.day);
  Normalize_WORD(lastTimeAccessed.hour);
  Normalize_WORD(lastTimeAccessed.minute);
  Normalize_WORD(lastTimeAccessed.second);
}

void LayoutReader_GDSIIbin::ReadSection_LIBNAME(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    ////MessageManager::Get()->PushError("Format error. Found LIBNAME section outside of library.");
    return;
  }
  if (p_activeElement) {
    ////MessageManager::Get()->PushError("Format error. Found LIBNAME section inside of structure.");
    return;
  }
  if (p_activeGeometry) {
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

void LayoutReader_GDSIIbin::ReadSection_UNITS(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found UNITS section outside of library.");
    return;
  }
  if (p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found UNITS section inside of structure.");
    return;
  }
  if (p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found UNITS section inside of element.");
    return;
  }

  file.read(reinterpret_cast<char *>(&p_activeLibrary->units), sizeof(Units));

  Normalize_DOUBLE(p_activeLibrary->units.user);
  Normalize_DOUBLE(p_activeLibrary->units.physical);
}

void LayoutReader_GDSIIbin::ReadSection_ENDLIBRARY(GDSIIRecord &_record) {
  //file.seekg(_record.length, std::ios_base::cur);
  // Nothing to do

  p_activeLibrary = nullptr;
}

void LayoutReader_GDSIIbin::ReadSection_BEGINSTRUCTURE(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found BGNSTR section outside of library.");
    return;
  }
  if (p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found BGNSTR section inside of structure.");
    return;
  }
  if (p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found BGNSTR section inside of element.");
    return;
  }

  p_activeElement = new Element;

  DateTime lastTimeModified, lastTimeAccessed;
  file.read(reinterpret_cast<char *>(&lastTimeModified), sizeof(DateTime));
  file.read(reinterpret_cast<char *>(&lastTimeAccessed), sizeof(DateTime));

  Normalize_WORD(lastTimeModified.year);
  Normalize_WORD(lastTimeModified.month);
  Normalize_WORD(lastTimeModified.day);
  Normalize_WORD(lastTimeModified.hour);
  Normalize_WORD(lastTimeModified.minute);
  Normalize_WORD(lastTimeModified.second);

  Normalize_WORD(lastTimeAccessed.year);
  Normalize_WORD(lastTimeAccessed.month);
  Normalize_WORD(lastTimeAccessed.day);
  Normalize_WORD(lastTimeAccessed.hour);
  Normalize_WORD(lastTimeAccessed.minute);
  Normalize_WORD(lastTimeAccessed.second);
}

void LayoutReader_GDSIIbin::ReadSection_STRNAME(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found STRNAME section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found STRNAME section outside of structure.");
    return;
  }
  if (p_activeGeometry) {
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

void LayoutReader_GDSIIbin::ReadSection_ENDSTRUCTURE(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found ENDSTR section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found ENDSTR section outside of structure.");
    return;
  }
  if (p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found ENDSTR section inside of element.");
    return;
  }

  ApplyLayoutReaderOptionsToActiveElement();

  if (!p_activeElement)
    return;

  if (!p_activeElement->geometries.empty()) {
    p_activeElement->min = p_activeElement->geometries[0]->min;
    p_activeElement->max = p_activeElement->geometries[0]->max;

    for (size_t i = 1; i < p_activeElement->geometries.size(); ++i) {
      if (p_activeElement->min.x > p_activeElement->geometries[i]->min.x)
        p_activeElement->min.x = p_activeElement->geometries[i]->min.x;
      if (p_activeElement->min.y > p_activeElement->geometries[i]->min.y)
        p_activeElement->min.y = p_activeElement->geometries[i]->min.y;
      if (p_activeElement->max.x < p_activeElement->geometries[i]->max.x)
        p_activeElement->max.x = p_activeElement->geometries[i]->max.x;
      if (p_activeElement->max.y < p_activeElement->geometries[i]->max.y)
        p_activeElement->max.y = p_activeElement->geometries[i]->max.y;
    }

    p_activeLibrary->elements.push_back(p_activeElement);
  }
  else
    delete p_activeElement;
  p_activeElement = nullptr;
}

void LayoutReader_GDSIIbin::ReadSection_BOUNDARY(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found BOUNDARY section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found BOUNDARY section outside of structure.");
    return;
  }
  if (p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found BOUNDARY section inside of element.");
    return;
  }

  p_activeGeometry = new Polygon;
  p_activeElement->geometries.push_back(p_activeGeometry);
}

void LayoutReader_GDSIIbin::ReadSection_PATH(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found PATH section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found PATH section outside of structure.");
    return;
  }
  if (p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found PATH section inside of element.");
    return;
  }

  p_activeGeometry = new Path;
  p_activeElement->geometries.push_back(p_activeGeometry);
}

void LayoutReader_GDSIIbin::ReadSection_SREF(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found SREF section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found SREF section outside of structure.");
    return;
  }
  if (p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found SREF section inside of element.");
    return;
  }

  p_activeGeometry = new Reference;
  p_activeElement->geometries.push_back(p_activeGeometry);
  p_activeElement->isFlat = false;
}

//void ReadSection_AREF(GDSIIRecord &_record);

void LayoutReader_GDSIIbin::ReadSection_TEXT(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found TEXT section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found TEXT section outside of structure.");
    return;
  }
  if (p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found TEXT section inside of element.");
    return;
  }

  p_activeGeometry = new Text;
  p_activeElement->geometries.push_back(p_activeGeometry);
}

void LayoutReader_GDSIIbin::ReadSection_LAYER(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found LAYER section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found LAYER section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found LAYER section outside of element.");
    return;
  }

  file.read(reinterpret_cast<char *>(&p_activeGeometry->layer), sizeof(int16_t));
  Normalize_WORD(p_activeGeometry->layer);
}

void LayoutReader_GDSIIbin::ReadSection_DATATYPE(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found DATATYPE section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found DATATYPE section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found DATATYPE section outside of element.");
    return;
  }

  int16_t dataType = 0;
  file.read(reinterpret_cast<char *>(&dataType), sizeof(int16_t));
  Normalize_WORD(dataType);

  switch (p_activeGeometry->type) {
    case GeometryType::polygon:
      static_cast<Polygon *>(p_activeGeometry)->dataType = dataType;
      break;
    case GeometryType::path:
      static_cast<Path *>(p_activeGeometry)->dataType = dataType;
      break;
    default:
      ;
      //TODO: push error that this element does not support property datatype
  }
}

void LayoutReader_GDSIIbin::ReadSection_WIDTH(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found WIDTH section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found WIDTH section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found WIDTH section outside of element.");
    return;
  }

  int32_t width = 0;
  file.read(reinterpret_cast<char *>(&width), sizeof(int32_t));
  Normalize_DWORD(width);

  switch (p_activeGeometry->type) {
    case GeometryType::path:
      static_cast<Path *>(p_activeGeometry)->width = width;
      break;
    case GeometryType::text:
      static_cast<Text *>(p_activeGeometry)->width = width;
      break;
    default:
      ;
      //MessageManager::Get()->PushError("Format error. Found WIDTH section given for inproper type of element.");
  }
}

void LayoutReader_GDSIIbin::ReadSection_XY(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found XY section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found XY section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found XY section outside of element.");
    return;
  }

  int numberOfCoors = _record.length / sizeof(Coord);

  Polygon    *p_boundary = nullptr;
  Path       *p_path = nullptr;
  Rectangle  *p_box = nullptr;
  Reference  *p_structRef = nullptr;
  Text       *p_text = nullptr;
  Coord       coord;
  int         i = 0;

  switch (p_activeGeometry->type) {
    case GeometryType::polygon:
      p_boundary = static_cast<Polygon *>(p_activeGeometry);
      p_boundary->coords.resize(numberOfCoors);
      for (i = 0; i < numberOfCoors; ++i) {
        file.read(reinterpret_cast<char *>(&coord), sizeof(Coord));

        Normalize_DWORD(coord.x);
        Normalize_DWORD(coord.y);

        p_boundary->coords[i] = coord;
      }
      break;
    case GeometryType::path:
      p_path = static_cast<Path *>(p_activeGeometry);
      p_path->coords.resize(numberOfCoors);
      for (i = 0; i < numberOfCoors; ++i) {
        file.read(reinterpret_cast<char *>(&coord), sizeof(Coord));

        Normalize_DWORD(coord.x);
        Normalize_DWORD(coord.y);

        p_path->coords[i] = coord;
      }
      break;
    case GeometryType::rectangle:
      p_box = static_cast<Rectangle *>(p_activeGeometry);
      p_box->coords.resize(numberOfCoors);
      for (i = 0; i < numberOfCoors; ++i) {
        file.read(reinterpret_cast<char *>(&coord), sizeof(Coord));

        Normalize_DWORD(coord.x);
        Normalize_DWORD(coord.y);

        p_box->coords[i] = coord;
      }
      break;
    case GeometryType::reference:
      p_structRef = static_cast<Reference *>(p_activeGeometry);

      file.read(reinterpret_cast<char *>(&coord), sizeof(Coord));
      Normalize_DWORD(coord.x);
      Normalize_DWORD(coord.y);

      p_structRef->coords.push_back(coord);
      break;
    case GeometryType::text:
      p_text = static_cast<Text *>(p_activeGeometry);

      file.read(reinterpret_cast<char *>(&coord), sizeof(Coord));
      Normalize_DWORD(coord.x);
      Normalize_DWORD(coord.y);

      p_text->coords.push_back(coord);
      break;
    default:
      ;
      //MessageManager::Get()->PushError("Format error. Found XY section given for inproper type of element.");
  }
  
  if (p_activeGeometry->coords.empty())
    return;

  p_activeGeometry->min = p_activeGeometry->max = p_activeGeometry->coords[0];
  for (size_t i = 1; i < p_activeGeometry->coords.size(); ++i) {
    if (p_activeGeometry->min.x > p_activeGeometry->coords[i].x)
      p_activeGeometry->min.x = p_activeGeometry->coords[i].x;
    if (p_activeGeometry->min.y > p_activeGeometry->coords[i].y)
      p_activeGeometry->min.y = p_activeGeometry->coords[i].y;
    if (p_activeGeometry->max.x < p_activeGeometry->coords[i].x)
      p_activeGeometry->max.x = p_activeGeometry->coords[i].x;
    if (p_activeGeometry->max.y < p_activeGeometry->coords[i].y)
      p_activeGeometry->max.y = p_activeGeometry->coords[i].y;
  }
}

void LayoutReader_GDSIIbin::ReadSection_ENDELEMENT(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found ENDELEMENT section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found ENDELEMENT section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found ENDELEMENT section outside of element.");
    return;
  }

  p_activeGeometry = nullptr;
}

void LayoutReader_GDSIIbin::ReadSection_SNAME(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found SNAME section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found SNAME section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found SNAME section outside of element.");
    return;
  }
  if (p_activeGeometry->type != GeometryType::reference) {
    //MessageManager::Get()->PushError("Format error. Found SNAME section given for inproper type of element.");
    return;
  }

  char *str = new char[_record.length + 1];
  memset(str, 0, _record.length + 1);
  file.read(str, _record.length);
  static_cast<Reference *>(p_activeGeometry)->name = str;
  delete[] str;
  str = nullptr;
}

//void LayoutReader_GDSIIbin::ReadSection_COLROW(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_TEXTNODE(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_NODE(GDSIIRecord &_record) {}

void LayoutReader_GDSIIbin::ReadSection_TEXTTYPE(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found TEXTTYPE section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found TEXTTYPE section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found TEXTTYPE section outside of element.");
    return;
  }
  if (p_activeGeometry->type != GeometryType::text) {
    //MessageManager::Get()->PushError("Format error. Found TEXTTYPE section given for inproper type of element.");
    return;
  }

  int16_t type;
  file.read(reinterpret_cast<char *>(&type), sizeof(int16_t));
  Normalize_WORD(type);

  static_cast<Text *>(p_activeGeometry)->textType = type;
}

void LayoutReader_GDSIIbin::ReadSection_PRESENTATION(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found PRESENTATION section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found PRESENTATION section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found PRESENTATION section outside of element.");
    return;
  }
  if (p_activeGeometry->type != GeometryType::text) {
    //MessageManager::Get()->PushError("Format error. Found PRESENTATION section given for inproper type of element.");
    return;
  }

  int16_t flags;
  file.read(reinterpret_cast<char *>(&flags), sizeof(int16_t));
  Normalize_WORD(flags);

  static_cast<Text *>(p_activeGeometry)->flagsPresentation = flags;
}

// UNUSED

void LayoutReader_GDSIIbin::ReadSection_STRING(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found STRING section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found STRING section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found STRING section outside of element.");
    return;
  }
  if (p_activeGeometry->type != GeometryType::text) {
    //MessageManager::Get()->PushError("Format error. Found STRING section given for inproper type of element.");
    return;
  }

  char *str = new char[_record.length + 1];
  memset(str, 0, _record.length + 1);
  file.read(str, _record.length);
  static_cast<Text *>(p_activeGeometry)->stringValue = str;
  delete[] str;
  str = nullptr;
}

void LayoutReader_GDSIIbin::ReadSection_STRANS(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found STRANS section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found STRANS section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found STRANS section outside of element.");
    return;
  }

  int16_t flags;
  file.read(reinterpret_cast<char *>(&flags), sizeof(int16_t));
  Normalize_WORD(flags);

  switch (p_activeGeometry->type) {
    case GeometryType::reference:
      //static_cast<GDSII_StructureRef *>(p_activeGeometry)-> = flags;
      break;
    //case it_arrayRef:
      //static_cast<GDSII_ArrayRef *>(p_activeGeometry)-> = flags;
      //break;
    case GeometryType::text:
      static_cast<Text *>(p_activeGeometry)->flagsTransformation = flags;
      break;
    default:
      ;
      //MessageManager::Get()->PushError("Format error. Found STRING section given for inproper type of element.");
  }
}

void LayoutReader_GDSIIbin::ReadSection_MAG(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found MAG section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found MAG section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found MAG section outside of element.");
    return;
  }

  double mag = 0.0;
  file.read(reinterpret_cast<char *>(&mag), sizeof(double));
  Normalize_DOUBLE(mag);

  switch (p_activeGeometry->type) {
    case GeometryType::reference:
      static_cast<Reference *>(p_activeGeometry)->magnification = mag;
      break;
    //case et_arrayRef:
      //static_cast<GDSII_ArrayRef *>(p_activeGeometry)->magnification = mag;
      //break;
    case GeometryType::text:
      static_cast<Text *>(p_activeGeometry)->magnification = mag;
      break;
    default:
      ;
      //MessageManager::Get()->PushError("Format error. Found MAG section given for inproper type of element.");
  }
}

//void LayoutReader_GDSIIbin::ReadSection_ANGLE(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_LINKTYPE(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_LINKKEYS(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_REFLIBS(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_FONTS(GDSIIRecord &_record) {}

void LayoutReader_GDSIIbin::ReadSection_PATHTYPE(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found PATHTYPE section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found PATHTYPE section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found PATHTYPE section outside of element.");
    return;
  }

  int16_t type;
  file.read(reinterpret_cast<char *>(&type), sizeof(int16_t));
  Normalize_WORD(type);

  switch (p_activeGeometry->type) {
    case GeometryType::path:
      static_cast<Path *>(p_activeGeometry)->pathType = type;
      break;
    case GeometryType::text:
      static_cast<Text *>(p_activeGeometry)->pathType = type;
      break;
    default:
      ;
      //MessageManager::Get()->PushError("Format error. Found PATHTYPE section given for inproper type of element.");
  }
}

//void LayoutReader_GDSIIbin::ReadSection_GENERATIONS(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_ATTRTABLE(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_STYPTABLE(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_STRTYPE(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_ELFLAGS(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_ELKEY(GDSIIRecord &_record) {}
// UNUSED
// UNUSED
//void LayoutReader_GDSIIbin::ReadSection_NODETYPE(GDSIIRecord &_record) {}

void LayoutReader_GDSIIbin::ReadSection_PROPATTR(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found PROPATTR section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found PROPATTR section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found PROPATTR section outside of element.");
    return;
  }
  if (!p_activeGeometry->properties.empty() && p_activeGeometry->properties.back().value.empty()) {
    //MessageManager::Get()->PushError("Format error. Found next PROPATTR section before previous PROPVALUE section was met.");
    return;
  }

  Property prop = { 0, "" };
  file.read(reinterpret_cast<char *>(&prop.index), sizeof(int16_t));
  Normalize_WORD(prop.index);

  p_activeGeometry->properties.push_back(prop);
  //p_activeGeometry->readingProperty = true;
}

void LayoutReader_GDSIIbin::ReadSection_PROPVALUE(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found PROPVALUE section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found PROPVALUE section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found PROPVALUE section outside of element.");
    return;
  }
  if (!p_activeGeometry->properties.empty() && p_activeGeometry->properties.back().value.empty()) {
    //MessageManager::Get()->PushError("Format error. Found PROPVALUE section before PROPATTR section was met.");
    return;
  }

  char *str = new char[_record.length + 1];
  memset(str, 0, _record.length + 1);
  file.read(str, _record.length);
  p_activeGeometry->properties[p_activeGeometry->properties.size() - 1].value = str;
  delete[] str;
  str = nullptr;

  //p_activeGeometry->readingProperty = false;
}

void LayoutReader_GDSIIbin::ReadSection_BOX(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found BOX section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found BOX section outside of structure.");
    return;
  }
  if (p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found BOX section inside of element.");
    return;
  }

  p_activeGeometry = new Rectangle;
  p_activeElement->geometries.push_back(p_activeGeometry);
}

void LayoutReader_GDSIIbin::ReadSection_BOXTYPE(GDSIIRecord &_record) {
  if (!p_activeLibrary) {
    //MessageManager::Get()->PushError("Format error. Found BOXTYPE section outside of library.");
    return;
  }
  if (!p_activeElement) {
    //MessageManager::Get()->PushError("Format error. Found BOXTYPE section outside of structure.");
    return;
  }
  if (!p_activeGeometry) {
    //MessageManager::Get()->PushError("Format error. Found BOXTYPE section outside of element.");
    return;
  }
  if (p_activeGeometry->type != GeometryType::rectangle) {
    //MessageManager::Get()->PushError("Format error. Found BOXTYPE section given for wrong type of element.");
    return;
  }

  int16_t type = 0;
  file.read(reinterpret_cast<char *>(&type), sizeof(int16_t));
  Normalize_WORD(type);

  static_cast<Rectangle *>(p_activeGeometry)->rectType = type;
}

//void LayoutReader_GDSIIbin::ReadSection_PLEX(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_BGNEXTN(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_ENDTEXTN(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_TAPENUM(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_TAPECODE(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_STRCLASS(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_RESERVED(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_FORMAT(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_MASK(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_ENDMASK(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_LIBDIRSIZE(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_SRFNAME(GDSIIRecord &_record) {}
//void LayoutReader_GDSIIbin::ReadSection_LIBSECUR(GDSIIRecord &_record) {}

void LayoutReader_GDSIIbin::ApplyLayoutReaderOptionsToActiveElement() {
  // Check if current element is in the list of "elements to read" or not
  if (!options->cellsToRead.empty()) {
    size_t i = 0;
    for (; i < options->cellsToRead.size(); ++i)
      if (options->cellsToRead[i] == p_activeElement->name)
        break;
    if (i != options->cellsToRead.size())
      p_activeLibrary->elements.push_back(p_activeElement);
    else {
      delete p_activeElement;
      p_activeElement = nullptr;
      return;
    }
  }

  // Check if layers of current element is in list of "layers to ignore"
  if (options->layersToRead.empty() && !options->layersToIgnore.empty()) {
    for (size_t i = 0; i < options->layersToIgnore.size(); ++i) {
      for (size_t j = 1; j < p_activeElement->geometries.size(); ++j) {
        if (p_activeElement->geometries[j]->layer != options->layersToIgnore[i])
          continue;
        p_activeElement->geometries.erase(p_activeElement->geometries.begin() + j);
        --j;
      }
      if (p_activeElement->geometries[0]->layer == options->layersToIgnore[i])
        p_activeElement->geometries.erase(p_activeElement->geometries.begin());
    }
  }

  // Check if we have to convert polygons to rectangles
  if (options->convertPolysToRects) {
    for (size_t i = 0; i < p_activeElement->geometries.size(); ++i) {
      if (p_activeElement->geometries[i]->type != GeometryType::polygon)
        continue;
      if (p_activeElement->geometries[i]->coords.size() != 5)
        continue;
      //TODO: Check if we have a stright rectangle
    }
  }

  // Check if we have to ignore paths with zero widths
  if (options->ignoreZeroWidthPaths) {
    for (size_t i = 1; i < p_activeElement->geometries.size(); ++i) {
      if (p_activeElement->geometries[i]->type != GeometryType::path)
        continue;
      Path *p_path = static_cast<Path *>(p_activeElement->geometries[i]);
      if (p_path->width != 0)
        continue;
      p_activeElement->geometries.erase(p_activeElement->geometries.begin() + i);
    }
    if (p_activeElement->geometries[0]->type == GeometryType::path) {
      Path* p_path = static_cast<Path*>(p_activeElement->geometries[0]);
      if (p_path->width == 0)
        p_activeElement->geometries.erase(p_activeElement->geometries.begin());
    }
  }
}

bool LayoutReader_GDSIIbin::PostProcessLayout() {

  // Resolve structure references
  for (size_t i = 0; i < p_layout->libraries.size(); ++i)
    for (size_t j = 0; j < p_layout->libraries[i]->elements.size(); ++j)
      for (size_t k = 0; k < p_layout->libraries[i]->elements[j]->geometries.size(); ++k) {
        if (p_layout->libraries[i]->elements[j]->geometries[k]->type != GeometryType::reference)
          continue;
        if (!ResolveStructureReference(static_cast<Reference*>(p_layout->libraries[i]->elements[j]->geometries[k])))
          return false;
      }

  // Fill layer information
  if (!EnumerateLayers())
    return false;

  // Calculate bounding box for the library based on bounding boxes of the elements that are not nested
  EvaluateBoundingBox();

  return true;
}

bool LayoutReader_GDSIIbin::ResolveStructureReference(Reference* p_reference) {
  for (size_t i = 0; i < p_layout->libraries.size(); ++i)
    for (size_t j = 0; j < p_layout->libraries[i]->elements.size(); ++j) {
      if (p_layout->libraries[i]->elements[j]->name == p_reference->name) {
        p_reference->referenceTo = p_layout->libraries[i]->elements[j];
        p_layout->libraries[i]->elements[j]->nested = true;
        return true;
      }
    }
  return false;
}

bool LayoutReader_GDSIIbin::EnumerateLayers() {
  Library *p_lib = nullptr;
  for (size_t i = 0; i < p_layout->libraries.size(); ++i) {
    p_lib = p_layout->libraries[i];
    for (size_t j = 0; j < p_lib->elements.size(); ++j) {
      for (size_t k = 0; k < p_lib->elements[j]->geometries.size(); ++k) {
        if (p_lib->elements[j]->geometries[k]->type == GeometryType::reference)
          continue;
        int layer = p_lib->elements[j]->geometries[k]->layer;
        int dtype = 0;

        switch (p_lib->elements[j]->geometries[k]->type) {
          case GeometryType::polygon:
            dtype = static_cast<Polygon *>(p_lib->elements[j]->geometries[k])->dataType;
            break;
          case GeometryType::path:
            dtype = static_cast<Path *>(p_lib->elements[j]->geometries[k])->dataType;
            break;
        }

        size_t l = 0;
        for (; l < p_lib->layers.size(); ++l)
          if (p_lib->layers[l].layer == layer && p_lib->layers[l].dataType == dtype) {
            //p_lib->layers[l].geometries.push_back(p_lib->elements[j]->geometries[k]);
            break;
          }
        if (l < p_lib->layers.size())
          continue;
        Layer li;
        li.layer = layer;
        li.dataType = dtype;
        //li.geometries.push_back(p_lib->elements[j]->geometries[k]);
        p_lib->layers.push_back(li);
      }
    }
  }
  return true;
}

void LayoutReader_GDSIIbin::EvaluateBoundingBox() {
  Library* p_lib = nullptr;
  for (size_t i = 0; i < p_layout->libraries.size(); ++i) {
    p_lib = p_layout->libraries[i];

    for (size_t j = 0; j < p_lib->elements.size(); ++j) {
      if (p_lib->elements[j]->nested)
        continue;

      p_lib->min = p_lib->elements[j]->min;
      p_lib->max = p_lib->elements[j]->max;

      if (p_lib->min.x > p_lib->elements[j]->min.x)
        p_lib->min.x = p_lib->elements[j]->min.x;
      if (p_lib->min.y > p_lib->elements[j]->min.y)
        p_lib->min.y = p_lib->elements[j]->min.y;
      if (p_lib->max.x > p_lib->elements[j]->max.x)
        p_lib->max.x = p_lib->elements[j]->max.x;
      if (p_lib->max.y > p_lib->elements[j]->max.y)
        p_lib->max.y = p_lib->elements[j]->max.y;
    }
  }
}
