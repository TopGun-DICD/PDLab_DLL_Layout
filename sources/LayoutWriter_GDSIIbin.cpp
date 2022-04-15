/*
 * LayoutWriter_GDSIIbin.cpp
 *
 * Calma binary GDS II file format writer by Dmitry A. Bulakh
 * 20.03.2021
 */

#include "LayoutWriter_GDSIIbin.hpp"
#include "GDSIITypes.hpp"
#include "GDSIIHelperFunctions.hpp"

#include <ctime>

LayoutWriter_GDSIIbin::LayoutWriter_GDSIIbin() {

}

bool LayoutWriter_GDSIIbin::Write(std::wstring fileName, Layout *layout) {
  if (!layout)
    return false;
  
  if (fileName.empty())
    return false;

  //std::clock_t startReading = std::clock();

  file.open(fileName, std::ios::out | std::ios::binary);
  if (!file.is_open())
    return false;

  p_data = layout;

  WriteSection_HEADER();
  WriteSection_BEGINLIBRARY();

  for (size_t i = 0; i < layout->libraries.size(); ++i) {
    WriteSection_LIBNAME(layout->libraries[i]);
    WriteSection_UNITS(layout->libraries[i]);
    for (size_t j = 0; j < layout->libraries[i]->elements.size(); ++j) {
      WriteSection_BEGINSTRUCTURE(layout->libraries[i]->elements[j]);
      WriteSection_STRNAME(layout->libraries[i]->elements[j]);

      for (size_t k = 0; k < layout->libraries[i]->elements[j]->geometries.size(); ++k) {
        switch (layout->libraries[i]->elements[j]->geometries[k]->type) {
          case GeometryType::rectangle:
            WriteSection_BOX(reinterpret_cast<Rectangle *>(layout->libraries[i]->elements[j]->geometries[k]));
            break;
          case GeometryType::polygon:
            WriteSection_BOUNDARY(reinterpret_cast<Polygon *>(layout->libraries[i]->elements[j]->geometries[k]));
            break;
          case GeometryType::reference:
            WriteSection_SREF(reinterpret_cast<Reference *>(layout->libraries[i]->elements[j]->geometries[k]));
            break;
          case GeometryType::path:
            WriteSection_PATH(reinterpret_cast<Path *>(layout->libraries[i]->elements[j]->geometries[k]));
            break;
        }
      }

      WriteSection_ENDSTRUCTURE(layout->libraries[i]->elements[j]);
    }

    WriteSection_ENDLIBRARY(layout->libraries[i]);
  }

  file.close();
  //std::clock_t stopReading = std::clock();

  return true;
}

void LayoutWriter_GDSIIbin::WriteSection_HEADER() {
  GDSIIRecord gdsiiRecord = { sizeof(GDSIIRecord) + sizeof(int16_t) , rt_HEADER , 2 };
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  // Write Data
  int16_t versionNumber = 600;
  DeNormalize_WORD(versionNumber);
  file.write(reinterpret_cast<char *>(&versionNumber), sizeof(int16_t));
}

void LayoutWriter_GDSIIbin::WriteSection_BEGINLIBRARY() {
  GDSIIRecord gdsiiRecord = { sizeof(GDSIIRecord) + 2 * sizeof(DateTime) , rt_BGNLIB , 2};
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  // Write Data
  DateTime lastTimeAccessed = { 2005,  3, 17, 14, 20, 51 };
  DateTime lastTimeModified = lastTimeAccessed;
  DeNormalize_WORD(lastTimeModified.year);
  DeNormalize_WORD(lastTimeModified.month);
  DeNormalize_WORD(lastTimeModified.day);
  DeNormalize_WORD(lastTimeModified.hour);
  DeNormalize_WORD(lastTimeModified.minute);
  DeNormalize_WORD(lastTimeModified.second);

  DeNormalize_WORD(lastTimeAccessed.year);
  DeNormalize_WORD(lastTimeAccessed.month);
  DeNormalize_WORD(lastTimeAccessed.day);
  DeNormalize_WORD(lastTimeAccessed.hour);
  DeNormalize_WORD(lastTimeAccessed.minute);
  DeNormalize_WORD(lastTimeAccessed.second);

  file.write(reinterpret_cast<char *>(&lastTimeModified), sizeof(DateTime));
  file.write(reinterpret_cast<char *>(&lastTimeAccessed), sizeof(DateTime));
}

void LayoutWriter_GDSIIbin::WriteSection_LIBNAME(Library *lib) {
  GDSIIRecord gdsiiRecord = { static_cast <int16_t>(sizeof(GDSIIRecord)) + static_cast<int16_t>(lib->name.length()) , rt_LIBNAME , 6 };
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  // Write Data
  file.write(lib->name.data(), lib->name.length());
}

void LayoutWriter_GDSIIbin::WriteSection_UNITS(Library *lib) {
  GDSIIRecord gdsiiRecord = { sizeof(GDSIIRecord) + sizeof(Units) , rt_UNITS , 5};
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  // Write Data
  Units units = lib->units;
  DeNormalize_DOUBLE(units.physical);
  DeNormalize_DOUBLE(units.user);

  file.write(reinterpret_cast<char *>(&units), sizeof(Units));
}

void LayoutWriter_GDSIIbin::WriteSection_ENDLIBRARY(Library *lib) {
  GDSIIRecord gdsiiRecord = { sizeof(GDSIIRecord) , rt_ENDLIB , 0 };
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
}

void LayoutWriter_GDSIIbin::WriteSection_BEGINSTRUCTURE(Element *element) {
  GDSIIRecord gdsiiRecord = { sizeof(GDSIIRecord) + 2 * sizeof(DateTime) , rt_BGNSTR , 2 };
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  // Write Data
  DateTime lastTimeModified = { 2001, 11, 11, 04, 43, 34 };
  DateTime lastTimeAccessed = { 2004,  8, 31, 13, 39, 37 };
  DeNormalize_WORD(lastTimeModified.year);
  DeNormalize_WORD(lastTimeModified.month);
  DeNormalize_WORD(lastTimeModified.day);
  DeNormalize_WORD(lastTimeModified.hour);
  DeNormalize_WORD(lastTimeModified.minute);
  DeNormalize_WORD(lastTimeModified.second);

  DeNormalize_WORD(lastTimeAccessed.year);
  DeNormalize_WORD(lastTimeAccessed.month);
  DeNormalize_WORD(lastTimeAccessed.day);
  DeNormalize_WORD(lastTimeAccessed.hour);
  DeNormalize_WORD(lastTimeAccessed.minute);
  DeNormalize_WORD(lastTimeAccessed.second);

  file.write(reinterpret_cast<char *>(&lastTimeModified), sizeof(DateTime));
  file.write(reinterpret_cast<char *>(&lastTimeAccessed), sizeof(DateTime));
}

void LayoutWriter_GDSIIbin::WriteSection_STRNAME(Element *element) {
  size_t actualNameLength = element->name.length();             //static_cast<int>(element->name.length());
  if(element->name.length() % 4 != 0)
    actualNameLength = ((element->name.length() / 4) + 1) * 4;  //((static_cast<int>(element->name.length()) / 4) + 1) * 4;
  GDSIIRecord gdsiiRecord = { static_cast <int16_t>(sizeof(GDSIIRecord)) + static_cast<int16_t>(actualNameLength) , rt_STRNAME , 6 };
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  // Write Data
  file.write(element->name.data(), element->name.length());
  if (element->name.length() % 4 != 0) {
    char zeroByte = 0;
    for (size_t i = 0; i < actualNameLength - element->name.length(); ++i)
      file.write(&zeroByte, 1);
  }
}

void LayoutWriter_GDSIIbin::WriteSection_ENDSTRUCTURE(Element *element) {
  GDSIIRecord gdsiiRecord = { sizeof(GDSIIRecord) , rt_ENDSTR , 0 };
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
}

void LayoutWriter_GDSIIbin::WriteSection_BOUNDARY(Polygon *polygon) {
  GDSIIRecord gdsiiRecord = { sizeof(GDSIIRecord) , rt_BOUNDARY , 0 };
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  // LAYER
  gdsiiRecord.recordType = rt_LAYER;
  gdsiiRecord.length = sizeof(GDSIIRecord) + sizeof(int16_t);
  gdsiiRecord.dataType = 2;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  int16_t layerNumber = polygon->layer;
  DeNormalize_WORD(layerNumber);
  file.write(reinterpret_cast<char *>(&layerNumber), sizeof(int16_t));
  // DATATYPE
  gdsiiRecord.recordType = rt_DATATYPE;
  gdsiiRecord.length = sizeof(GDSIIRecord) + sizeof(int16_t);
  gdsiiRecord.dataType = 2;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  int16_t dataType = polygon->dataType;
  DeNormalize_WORD(dataType);
  file.write(reinterpret_cast<char *>(&dataType), sizeof(int16_t));
  // XY
  gdsiiRecord.recordType = rt_XY;
  gdsiiRecord.length = sizeof(GDSIIRecord) + static_cast<int16_t>(polygon->coords.size()) * sizeof(Coord);
  gdsiiRecord.dataType = 3;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  for (size_t i = 0; i < polygon->coords.size(); ++i) {
    Coord coord = polygon->coords[i];
    DeNormalize_DWORD(coord.x);
    DeNormalize_DWORD(coord.y);
    file.write(reinterpret_cast<char *>(&coord), sizeof(Coord));
  }
  // ENDEL
  gdsiiRecord.recordType = rt_ENDEL;
  gdsiiRecord.length = sizeof(GDSIIRecord);
  gdsiiRecord.dataType = 0;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
}

void LayoutWriter_GDSIIbin::WriteSection_BOX(Rectangle *rectangle) {
  GDSIIRecord gdsiiRecord = { sizeof(GDSIIRecord) , rt_BOUNDARY , 0 };
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char*>(&gdsiiRecord), sizeof(GDSIIRecord));
  // LAYER
  gdsiiRecord.recordType = rt_LAYER;
  gdsiiRecord.length = sizeof(GDSIIRecord) + sizeof(int16_t);
  gdsiiRecord.dataType = 2;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char*>(&gdsiiRecord), sizeof(GDSIIRecord));
  int16_t layerNumber = rectangle->layer;
  DeNormalize_WORD(layerNumber);
  file.write(reinterpret_cast<char*>(&layerNumber), sizeof(int16_t));
  // DATATYPE
  gdsiiRecord.recordType = rt_DATATYPE;
  gdsiiRecord.length = sizeof(GDSIIRecord) + sizeof(int16_t);
  gdsiiRecord.dataType = 2;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char*>(&gdsiiRecord), sizeof(GDSIIRecord));
  int16_t dataType = rectangle->rectType;
  DeNormalize_WORD(dataType);
  file.write(reinterpret_cast<char*>(&dataType), sizeof(int16_t));
  // XY
  gdsiiRecord.recordType = rt_XY;
  gdsiiRecord.length = sizeof(GDSIIRecord) + static_cast<int16_t>(rectangle->coords.size()) * sizeof(Coord);
  gdsiiRecord.dataType = 3;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char*>(&gdsiiRecord), sizeof(GDSIIRecord));
  for (size_t i = 0; i < rectangle->coords.size(); ++i) {
    Coord coord = rectangle->coords[i];
    DeNormalize_DWORD(coord.x);
    DeNormalize_DWORD(coord.y);
    file.write(reinterpret_cast<char*>(&coord), sizeof(Coord));
  }
  // ENDEL
  gdsiiRecord.recordType = rt_ENDEL;
  gdsiiRecord.length = sizeof(GDSIIRecord);
  gdsiiRecord.dataType = 0;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char*>(&gdsiiRecord), sizeof(GDSIIRecord));
}

void LayoutWriter_GDSIIbin::WriteSection_SREF(Reference *reference) {
  GDSIIRecord gdsiiRecord = { sizeof(GDSIIRecord) , rt_SREF , 0 };
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  // SNAME
  gdsiiRecord.recordType = rt_SNAME;
  size_t actualNameLength = reference->name.length();             //static_cast<int>(reference->name.length());
  if(reference->name.length() % 4 != 0)
    actualNameLength = ((reference->name.length() / 4) + 1) * 4;  //static_cast<int>(((reference->name.length() / 4) + 1) * 4);
  gdsiiRecord.length = sizeof(GDSIIRecord) + static_cast<int16_t>(actualNameLength);
  gdsiiRecord.dataType = 6;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  file.write(reference->name.data(), reference->name.length());
  if (reference->name.length() % 4 != 0) {
    char zeroByte = 0;
    for (size_t i = 0; i < actualNameLength - reference->name.length(); ++i)
      file.write(&zeroByte, 1);
  }
  // XY
  gdsiiRecord.recordType = rt_XY;
  gdsiiRecord.length = sizeof(GDSIIRecord) + sizeof(Coord);
  gdsiiRecord.dataType = 3;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  Coord coord = reference->coords[0];
  DeNormalize_DWORD(coord.x);
  DeNormalize_DWORD(coord.y);
  file.write(reinterpret_cast<char *>(&coord), sizeof(Coord));
  // ENDEL
  gdsiiRecord.recordType = rt_ENDEL;
  gdsiiRecord.length = sizeof(GDSIIRecord);
  gdsiiRecord.dataType = 0;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
}

void LayoutWriter_GDSIIbin::WriteSection_PATH(Path *path) {
  GDSIIRecord gdsiiRecord = { sizeof(GDSIIRecord) , rt_PATH , 0 };
  // Write Header
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  // LAYER
  gdsiiRecord.recordType = rt_LAYER;
  gdsiiRecord.length = sizeof(GDSIIRecord) + sizeof(int16_t);
  gdsiiRecord.dataType = 2;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  int16_t layerNumber = path->layer;
  DeNormalize_WORD(layerNumber);
  file.write(reinterpret_cast<char *>(&layerNumber), sizeof(int16_t));
  // DATATYPE
  gdsiiRecord.recordType = rt_DATATYPE;
  gdsiiRecord.length = sizeof(GDSIIRecord) + sizeof(int16_t);
  gdsiiRecord.dataType = 2;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  int16_t dataType = path->dataType;
  DeNormalize_WORD(dataType);
  file.write(reinterpret_cast<char *>(&dataType), sizeof(int16_t));
  // DATATYPE
  gdsiiRecord.recordType = rt_PATHTYPE;
  gdsiiRecord.length = sizeof(GDSIIRecord) + sizeof(int16_t);
  gdsiiRecord.dataType = 2;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  int16_t pathType = path->pathType;
  DeNormalize_WORD(pathType);
  file.write(reinterpret_cast<char *>(&pathType), sizeof(int16_t));
  // WIDTH
  gdsiiRecord.recordType = rt_WIDTH;
  gdsiiRecord.length = sizeof(GDSIIRecord) + sizeof(int32_t);
  gdsiiRecord.dataType = 3;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  int32_t pathWidth = path->width;
  DeNormalize_DWORD(pathWidth);
  file.write(reinterpret_cast<char *>(&pathWidth), sizeof(int32_t));
  // XY
  gdsiiRecord.recordType = rt_XY;
  gdsiiRecord.length = sizeof(GDSIIRecord) + static_cast<int16_t>(path->coords.size()) * sizeof(Coord);
  gdsiiRecord.dataType = 3;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
  for (size_t i = 0; i < path->coords.size(); ++i) {
    Coord coord = path->coords[i];
    DeNormalize_DWORD(coord.x);
    DeNormalize_DWORD(coord.y);
    file.write(reinterpret_cast<char *>(&coord), sizeof(Coord));
  }
  // ENDEL
  gdsiiRecord.recordType = rt_ENDEL;
  gdsiiRecord.length = sizeof(GDSIIRecord);
  gdsiiRecord.dataType = 0;
  DeNormalize_WORD(gdsiiRecord.length);
  file.write(reinterpret_cast<char *>(&gdsiiRecord), sizeof(GDSIIRecord));
}


