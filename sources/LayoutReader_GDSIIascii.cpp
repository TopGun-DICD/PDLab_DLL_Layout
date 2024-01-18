/*
 * LayoutReader_GDSIIascii.hpp
 *
 * CALMA GDSII stream format reader by Artem K. Efremov
 * 21.04.2023
 */
#include "LayoutReader_GDSIIascii.hpp"

#include <ctime>
#include <limits>

#include <iostream>

LayoutReader_GDSIIascii::LayoutReader_GDSIIascii() : p_activeLibrary(nullptr), p_activeElement(nullptr), p_activeGeometry(nullptr) {
}

bool LayoutReader_GDSIIascii::IsMyFormat(const std::wstring &fName) {
  fileName = fName;

  if (fName.substr(fName.find_last_of(L".") + 1) != L"gds")
      if (fName.substr(fName.find_last_of(L".") + 1) != L"gdt")
          if (fName.substr(fName.find_last_of(L".") + 1) != L"txt")
              return false;

  file.open(fileName);
  if (!file.is_open())
      return false;
  std::string tmp;
  file >> tmp;
  if (tmp != "HEADER") {
      file.close();
      return false;
  }
  file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  file >> tmp;
  if (tmp != "BGNLIB") {
      file.close();
      return false;
  }
  file.close();
  return true;
}

bool LayoutReader_GDSIIascii::Read(Layout *layout, LayoutReaderOptions *options) {
  if (!layout)
    return false;
  p_layout = layout;

  file.open(fileName, std::ios::binary);
  if (!file.is_open())
    return false;

  std::string section;
  while (true) {
      file >> section;

      if (file.eof())
          break;
      
      if (section == "HEADER") {
          ReadSection_HEADER();
      }
      else if (section == "BGNLIB") {
          ReadSection_BEGINLIBRARY();
      }
      else if (section == "LIBNAME") {
          ReadSection_LIBNAME();
      }
      else if (section == "UNITS") {
          ReadSection_UNITS();
      }
      else if (section == "ENDLIB") {
          ReadSection_ENDLIBRARY();
      }
      else if (section == "BGNSTR") {
          ReadSection_BEGINSTRUCTURE();
      }
      else if (section == "STRNAME") {
          ReadSection_STRNAME();
      }
      else if (section == "ENDSTR") {
          ReadSection_ENDSTRUCTURE();
      }
      else if (section == "BOUNDARY") {
          ReadSection_BOUNDARY();
      }
      else if (section == "PATH") {
          ReadSection_PATH();
      }
      else if (section == "SREF") {
          ReadSection_SREF();
      }
      else if (section == "TEXT") {
          ReadSection_TEXT();
      }
      else if (section == "LAYER") {
          ReadSection_LAYER();
      }
      else if (section == "DATATYPE") {
          ReadSection_DATATYPE();
      }
      else if (section == "WIDTH") {
          ReadSection_WIDTH();
      }
      else if (section == "XY") {
          ReadSection_XY();
      }
      else if (section == "ENDEL") {
          ReadSection_ENDELEMENT();
      }
      else if (section == "SNAME") {
          ReadSection_SNAME();
      }
      else if (section == "TEXTTYPE") {
          ReadSection_TEXTTYPE();
      }
      else if (section == "PRESENTATION") {
          ReadSection_PRESENTATION();
      }
      else if (section == "STRING") {
          ReadSection_STRING();
      }
      else if (section == "STRANS") {
          ReadSection_STRANS();
      }
      else if (section == "MAG") {
          ReadSection_MAG();
      }
      else if (section == "PATHTYPE") {
          ReadSection_PATHTYPE();
      }
      else if (section == "PROPATTR") {
          ReadSection_PROPATTR();
      }
      else if (section == "PROPVALUE") {
          ReadSection_PROPVALUE();
      }
      else if (section == "BOX") {
          ReadSection_BOX();
      }
      else if (section == "BOXTYPE") {
          ReadSection_BOXTYPE();
      }
      else {
          //Incorrect data
      }
  }

  file.close();
  layout->fileName = fileName;
  layout->fileFormat = FileFormat::GDSII_ascii;
  return PostProcessLayout();
}

void LayoutReader_GDSIIascii::ReadSection_HEADER() {
    int16_t vesionNumber = 0;
    file >> vesionNumber;
}

void LayoutReader_GDSIIascii::ReadSection_BEGINLIBRARY() {
    if (p_activeLibrary) {
        //MessageManager::Get()->PushError("Format error. Found BGNLIB section inside of library.");
        return;
    }
    if (p_activeElement) {
        //MessageManager::Get()->PushError("Format error. Found BGNLIB section inside of structure.");
        return;
    }
    if (p_activeGeometry) {
        //MessageManager::Get()->PushError("Format error. Found BGNLIB section inside of element.");
        return;
    }

    p_activeLibrary = new Library;
    
    DateTime lastTimeModified, lastTimeAccessed;
    std::string date, time;

    file >> date;
    lastTimeModified.year = stoi(date.substr(date.find_last_of("/") + 1));
    date = date.substr(0, date.find_last_of("/"));
    lastTimeModified.day = stoi(date.substr(date.find_last_of("/") + 1));
    date = date.substr(0, date.find_last_of("/"));
    lastTimeModified.month = stoi(date);
    file >> time;
    lastTimeModified.second = stoi(time.substr(time.find_last_of(":") + 1));
    time = time.substr(0, time.find_last_of(":"));
    lastTimeModified.minute = stoi(time.substr(time.find_last_of(":") + 1));
    time = time.substr(0, time.find_last_of(":"));
    lastTimeModified.hour = stoi(time);

    file >> date;
    lastTimeAccessed.year = stoi(date.substr(date.find_last_of("/") + 1));
    date = date.substr(0, date.find_last_of("/"));
    lastTimeAccessed.day = stoi(date.substr(date.find_last_of("/") + 1));
    date = date.substr(0, date.find_last_of("/"));
    lastTimeAccessed.month = stoi(date);
    file >> time;
    lastTimeAccessed.second = stoi(time.substr(time.find_last_of(":") + 1));
    time = time.substr(0, time.find_last_of(":"));
    lastTimeAccessed.minute = stoi(time.substr(time.find_last_of(":") + 1));
    time = time.substr(0, time.find_last_of(":"));
    lastTimeAccessed.hour = stoi(time);
}

void LayoutReader_GDSIIascii::ReadSection_LIBNAME() {
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

    file >> p_activeLibrary->name;
}

void LayoutReader_GDSIIascii::ReadSection_UNITS() {
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

    file >> p_activeLibrary->units.user >> p_activeLibrary->units.physical;
}

void LayoutReader_GDSIIascii::ReadSection_ENDLIBRARY() {
    p_layout->libraries.push_back(p_activeLibrary);
    p_activeLibrary = nullptr;
}

void LayoutReader_GDSIIascii::ReadSection_BEGINSTRUCTURE() {
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
    std::string date, time;

    file >> date;
    lastTimeModified.year = stoi(date.substr(date.find_last_of("/") + 1));
    date = date.substr(0, date.find_last_of("/"));
    lastTimeModified.day = stoi(date.substr(date.find_last_of("/") + 1));
    date = date.substr(0, date.find_last_of("/"));
    lastTimeModified.month = stoi(date);
    file >> time;
    lastTimeModified.second = stoi(time.substr(time.find_last_of(":") + 1));
    time = time.substr(0, time.find_last_of(":"));
    lastTimeModified.minute = stoi(time.substr(time.find_last_of(":") + 1));
    time = time.substr(0, time.find_last_of(":"));
    lastTimeModified.hour = stoi(time);

    file >> date;
    lastTimeAccessed.year = stoi(date.substr(date.find_last_of("/") + 1));
    date = date.substr(0, date.find_last_of("/"));
    lastTimeAccessed.day = stoi(date.substr(date.find_last_of("/") + 1));
    date = date.substr(0, date.find_last_of("/"));
    lastTimeAccessed.month = stoi(date);
    file >> time;
    lastTimeAccessed.second = stoi(time.substr(time.find_last_of(":") + 1));
    time = time.substr(0, time.find_last_of(":"));
    lastTimeAccessed.minute = stoi(time.substr(time.find_last_of(":") + 1));
    time = time.substr(0, time.find_last_of(":"));
    lastTimeAccessed.hour = stoi(time);
}

void LayoutReader_GDSIIascii::ReadSection_STRNAME() {
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

    file >> p_activeElement->name;
}

void LayoutReader_GDSIIascii::ReadSection_ENDSTRUCTURE() {
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

    if (!p_activeElement->geometries.empty()) {
        p_activeElement->min = p_activeElement->geometries[0]->min;
        p_activeElement->max = p_activeElement->geometries[0]->max;

        for (const auto& i : p_activeElement->geometries) {
            if (p_activeElement->min.x > i->min.x)
                p_activeElement->min.x = i->min.x;
            if (p_activeElement->min.y > i->min.y)
                p_activeElement->min.y = i->min.y;
            if (p_activeElement->max.x < i->max.x)
                p_activeElement->max.x = i->max.x;
            if (p_activeElement->max.y < i->max.y)
                p_activeElement->max.y = i->max.y;
        }
    }

    p_activeLibrary->elements.push_back(p_activeElement);
    p_activeElement = nullptr;
}

void LayoutReader_GDSIIascii::ReadSection_BOUNDARY() {
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
}

void LayoutReader_GDSIIascii::ReadSection_PATH() {
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
}

void LayoutReader_GDSIIascii::ReadSection_SREF() {
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
    p_activeElement->isFlat = false;
}

void LayoutReader_GDSIIascii::ReadSection_TEXT() {
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
}

void LayoutReader_GDSIIascii::ReadSection_LAYER() {
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

    file >> p_activeGeometry->layer;
}

void LayoutReader_GDSIIascii::ReadSection_DATATYPE() {
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

    switch (p_activeGeometry->type) {
    case GeometryType::polygon:
        file >> static_cast<Polygon*>(p_activeGeometry)->dataType;
        break;
    case GeometryType::path:
        file >> static_cast<Path*>(p_activeGeometry)->dataType;
        break;
    default:
        //MessageManager::Get()->PushError("Format error. Found DATATYPE section given for inproper type of element.");
        break;
    }
}

void LayoutReader_GDSIIascii::ReadSection_WIDTH() {
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

    switch (p_activeGeometry->type) {
    case GeometryType::path:
        file >> static_cast<Path*>(p_activeGeometry)->width;
        break;
    case GeometryType::text:
        file >> static_cast<Text*>(p_activeGeometry)->width;
        break;
    default:
        //MessageManager::Get()->PushError("Format error. Found WIDTH section given for inproper type of element.");
        break;
    }
}

void LayoutReader_GDSIIascii::ReadSection_XY() {
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

    Polygon*        p_boundary = nullptr;
    Path*           p_path = nullptr;
    Rectangle*      p_box = nullptr;
    Reference*      p_structRef = nullptr;
    Text*           p_text = nullptr;
    Coord           coord;
    std::string     x;
    std::streampos  x_start;

    switch (p_activeGeometry->type) {
    case GeometryType::polygon:
        p_boundary = static_cast<Polygon*>(p_activeGeometry);

        x_start = file.tellg();
        file >> x;
        while (!std::isalpha(x[0])) {
            x.pop_back();
            coord.x = stoi(x);
            file >> coord.y;
            p_boundary->coords.push_back(coord);
            x_start = file.tellg();
            file >> x;
        }
        file.seekg(x_start, std::ios::beg);
        break;
    case GeometryType::path:
        p_path = static_cast<Path*>(p_activeGeometry);

        x_start = file.tellg();
        file >> x;
        while (!std::isalpha(x[0])) {
            x.pop_back();
            coord.x = stoi(x);
            file >> coord.y;
            p_path->coords.push_back(coord);
            x_start = file.tellg();
            file >> x;
        }
        file.seekg(x_start);
        break;
    case GeometryType::rectangle:
        p_box = static_cast<Rectangle*>(p_activeGeometry);

        x_start = file.tellg();
        file >> x;
        while (!std::isalpha(x[0])) {
            x.pop_back();
            coord.x = stoi(x);
            file >> coord.y;
            p_box->coords.push_back(coord);
            x_start = file.tellg();
            file >> x;
        }
        file.seekg(x_start);
        break;
    case GeometryType::reference:
        p_structRef = static_cast<Reference*>(p_activeGeometry);

        x_start = file.tellg();
        file >> x;
        if (!std::isalpha(x[0])) {
            x.pop_back();
            coord.x = stoi(x);
            file >> coord.y;
            p_structRef->coords.push_back(coord);
        }
        file.seekg(x_start);
        break;
    case GeometryType::text:
        p_text = static_cast<Text*>(p_activeGeometry);

        x_start = file.tellg();
        file >> x;
        if (!std::isalpha(x[0])) {
            x.pop_back();
            coord.x = stoi(x);
            file >> coord.y;
            p_text->coords.push_back(coord);
        }
        file.seekg(x_start);
        break;
    default:
        //MessageManager::Get()->PushError("Format error. Found XY section given for inproper type of element.");
        break;
    }

    if (p_activeGeometry->coords.empty())
        return;

    p_activeGeometry->min = p_activeGeometry->max = p_activeGeometry->coords[0];
    for (const auto& i : p_activeGeometry->coords) {
        if (p_activeGeometry->min.x > i.x)
            p_activeGeometry->min.x = i.x;
        if (p_activeGeometry->min.y > i.y)
            p_activeGeometry->min.y = i.y;
        if (p_activeGeometry->max.x < i.x)
            p_activeGeometry->max.x = i.x;
        if (p_activeGeometry->max.y < i.y)
            p_activeGeometry->max.y = i.y;
    }
}

void LayoutReader_GDSIIascii::ReadSection_ENDELEMENT() {
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

    p_activeElement->geometries.push_back(p_activeGeometry);
    p_activeGeometry = nullptr;
}

void LayoutReader_GDSIIascii::ReadSection_SNAME(){
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

    file >> static_cast<Reference*>(p_activeGeometry)->name;
}

void LayoutReader_GDSIIascii::ReadSection_TEXTTYPE()
{
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

    file >> static_cast<Text*>(p_activeGeometry)->textType;
}

void LayoutReader_GDSIIascii::ReadSection_PRESENTATION()
{
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

    file >> static_cast<Text*>(p_activeGeometry)->flagsPresentation;
}

void LayoutReader_GDSIIascii::ReadSection_STRING()
{
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

    file >> static_cast<Text*>(p_activeGeometry)->stringValue;
}

void LayoutReader_GDSIIascii::ReadSection_STRANS()
{
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

    switch (p_activeGeometry->type) {
    case GeometryType::reference:
        file >> static_cast<Reference *>(p_activeGeometry)->transformationFlags;
        break;
    //case it_arrayRef:
          //static_cast<GDSII_ArrayRef *>(p_activeGeometry)-> = flags;
          //break;
    case GeometryType::text:
        file >> static_cast<Text*>(p_activeGeometry)->flagsTransformation;
        break;
    default:
        //MessageManager::Get()->PushError("Format error. Found STRANS section given for inproper type of element."); //// Error in bin reader!!!
        break;
    }
}

void LayoutReader_GDSIIascii::ReadSection_MAG()
{
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

    switch (p_activeGeometry->type) {
    case GeometryType::reference:
        file >> static_cast<Reference*>(p_activeGeometry)->magnification;
        break;
    //case et_arrayRef:
        //static_cast<GDSII_ArrayRef *>(p_activeGeometry)->magnification = mag;
        //break;
    case GeometryType::text:
        file >> static_cast<Text*>(p_activeGeometry)->magnification;
        break;
    default:
        //MessageManager::Get()->PushError("Format error. Found MAG section given for inproper type of element.");
        break;
    }
}

void LayoutReader_GDSIIascii::ReadSection_PATHTYPE()
{
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

    switch (p_activeGeometry->type) {
    case GeometryType::path:
        file >> static_cast<Path*>(p_activeGeometry)->pathType;
        break;
    case GeometryType::text:
        file >> static_cast<Text*>(p_activeGeometry)->pathType;
        break;
    default:
        //MessageManager::Get()->PushError("Format error. Found PATHTYPE section given for inproper type of element.");
        break;
    }
}

void LayoutReader_GDSIIascii::ReadSection_PROPATTR()
{
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
    file >> prop.index;

    p_activeGeometry->properties.push_back(prop);
}

void LayoutReader_GDSIIascii::ReadSection_PROPVALUE()
{
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

    file >> p_activeGeometry->properties[p_activeGeometry->properties.size() - 1].value;
}

void LayoutReader_GDSIIascii::ReadSection_BOX()
{
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
}

void LayoutReader_GDSIIascii::ReadSection_BOXTYPE()
{
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

    file >> static_cast<Rectangle*>(p_activeGeometry)->rectType;
}

bool LayoutReader_GDSIIascii::PostProcessLayout() {

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

bool LayoutReader_GDSIIascii::ResolveStructureReference(Reference* p_reference) {
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

bool LayoutReader_GDSIIascii::EnumerateLayers() {
    //Library* lib = nullptr;
    //for (size_t i = 0; i < p_layout->libraries.size(); ++i) {
    for (auto & lib : p_layout->libraries) {
        //lib = p_layout->libraries[i];
        for (size_t j = 0; j < lib->elements.size(); ++j) {
            for (size_t k = 0; k < lib->elements[j]->geometries.size(); ++k) {
                if (lib->elements[j]->geometries[k]->type == GeometryType::reference)
                    continue;
                int layer = lib->elements[j]->geometries[k]->layer;
                int dtype = 0;

                switch (lib->elements[j]->geometries[k]->type) {
                case GeometryType::polygon:
                    dtype = static_cast<Polygon*>(lib->elements[j]->geometries[k])->dataType;
                    break;
                case GeometryType::path:
                    dtype = static_cast<Path*>(lib->elements[j]->geometries[k])->dataType;
                    break;
                }

                size_t l = 0;
                for (; l < lib->layers.size(); ++l)
                    if (lib->layers[l].layer == layer && lib->layers[l].dataType == dtype) {
                        //p_lib->layers[l].geometries.push_back(p_lib->elements[j]->geometries[k]);
                        break;
                    }
                if (l < lib->layers.size())
                    continue;
                Layer li;
                li.layer = layer;
                li.dataType = dtype;
                //li.geometries.push_back(p_lib->elements[j]->geometries[k]);
                lib->layers.push_back(li);
            }
        }
    }

    //Re-order layers
    for (auto& lib : p_layout->libraries) {
        //lib = p_layout->libraries[i];
        for (size_t j = 0; j < lib->layers.size(); ++j) {
            for (size_t k = 1; k < lib->layers.size(); ++k) {
                if (lib->layers[k].layer < lib->layers[k - 1].layer)
                    std::swap(lib->layers[k].layer, lib->layers[k - 1].layer);
                if ((lib->layers[k].layer == lib->layers[k - 1].layer) && (lib->layers[k].dataType < lib->layers[k - 1].dataType))
                    std::swap(lib->layers[k].layer, lib->layers[k - 1].layer);
            }
        }
    }
    return true;
}

void LayoutReader_GDSIIascii::EvaluateBoundingBox() {
    //Library* p_lib = nullptr;
    for (auto& lib : p_layout->libraries) {
        //p_lib = p_layout->libraries[i];

        for (size_t j = 0; j < lib->elements.size(); ++j) {
            if (lib->elements[j]->nested)
                continue;

            lib->min = lib->elements[j]->min;
            lib->max = lib->elements[j]->max;

            if (lib->min.x > lib->elements[j]->min.x)
                lib->min.x = lib->elements[j]->min.x;
            if (lib->min.y > lib->elements[j]->min.y)
                lib->min.y = lib->elements[j]->min.y;
            if (lib->max.x > lib->elements[j]->max.x)
                lib->max.x = lib->elements[j]->max.x;
            if (lib->max.y > lib->elements[j]->max.y)
                lib->max.y = lib->elements[j]->max.y;
        }
    }
}
