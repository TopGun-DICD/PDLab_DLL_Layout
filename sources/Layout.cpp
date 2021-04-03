#include "Layout.hpp"
#include "LayoutReader.hpp"
#include "LayoutWriter.hpp"

__declspec(dllexport) Layout *CreateLayoutFromFile(std::wstring fileName) {
  AbstractLayoutReader *p_reader = GetReader(fileName);

  if (!p_reader)
    return nullptr;

  Layout *p_layout = new Layout;
  p_layout->fileFormat = FileFormat::undefined;

  if (!p_reader->Read(p_layout)) {
    FreeReader(p_reader);
    delete p_layout;
    return nullptr;
  }
  FreeReader(p_reader);
  
  p_layout->fileName = fileName;
  return p_layout;
}

__declspec(dllexport) void ReadLayoutFromFile(std::wstring fileName, Layout *layout) {
  AbstractLayoutReader *p_reader = GetReader(fileName);

  if (!p_reader)
    return;

  layout->fileFormat = FileFormat::undefined;

  if (!p_reader->Read(layout)) {
    FreeReader(p_reader);
    return;
  }
  FreeReader(p_reader);
  
  layout->fileName = fileName;
}

__declspec(dllexport) void ClearLayout(Layout *layout) {
  for (size_t i = 0; i < layout->libraries.size(); ++i) {

    for (size_t j = 0; j < layout->libraries[i]->elements.size(); ++j) {

      for (size_t k = 0; k < layout->libraries[i]->elements[j]->items.size(); ++k) {
        delete layout->libraries[i]->elements[j]->items[k];
        layout->libraries[i]->elements[j]->items[k] = nullptr;
      }
      layout->libraries[i]->elements[j]->items.clear();

      delete layout->libraries[i]->elements[j];
      layout->libraries[i]->elements[j] = nullptr;
    }

    layout->libraries[i]->elements.clear();
    layout->libraries[i]->layers.clear();

    delete layout->libraries[i];
    layout->libraries[i] = nullptr;
  }
  layout->libraries.clear();
}

__declspec(dllexport) void CopyLayout(Layout *src, Layout *dst) {
  if (!src)
    return;
  if (!dst)
    return;
  ClearLayout(dst);

  dst->fileName = src->fileName;
  dst->fileFormat = src->fileFormat;

  Geometry_Polygon   *p_polygon = nullptr;
  Geometry_Path      *p_path    = nullptr;
  Geometry_Text      *p_text    = nullptr;
  Geometry_Box       *p_box     = nullptr;
  Geometry_Reference *p_ref     = nullptr;
  
  for (size_t i = 0; i < src->libraries.size(); ++i) {
    Library *p_lib = new Library;
    p_lib->name = src->libraries[i]->name;
    p_lib->units = src->libraries[i]->units;
    dst->libraries.push_back(p_lib);
    for (size_t j = 0; j < src->libraries[i]->elements.size(); ++j) {
      Element *p_element = new Element;
      p_element->name = src->libraries[i]->elements[j]->name;
      p_element->nested = src->libraries[i]->elements[j]->nested;
      p_element->min = src->libraries[i]->elements[j]->min;
      p_element->max = src->libraries[i]->elements[j]->max;
      dst->libraries[i]->elements.push_back(p_element);
      for (size_t k = 0; k < src->libraries[i]->elements[j]->items.size(); ++k) {
        switch (src->libraries[i]->elements[j]->items[k]->type) {
          case GeometryType::polygon:
            p_polygon = new Geometry_Polygon;
            dst->libraries[i]->elements[j]->items.push_back(p_polygon);
            p_polygon->type = GeometryType::polygon;
            p_polygon->layer = src->libraries[i]->elements[j]->items[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->properties.size(); ++l)
              p_polygon->properties.push_back(src->libraries[i]->elements[j]->items[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->coords.size(); ++l)
              p_polygon->coords.push_back(src->libraries[i]->elements[j]->items[k]->coords[l]);
            p_polygon->dataType = static_cast<Geometry_Polygon *>(src->libraries[i]->elements[j]->items[k])->dataType;
            break;
          case GeometryType::box:
            p_box = new Geometry_Box;
            dst->libraries[i]->elements[j]->items.push_back(p_box);
            p_box->type = GeometryType::box;
            p_box->layer = src->libraries[i]->elements[j]->items[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->properties.size(); ++l)
              p_box->properties.push_back(src->libraries[i]->elements[j]->items[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->coords.size(); ++l)
              p_box->coords.push_back(src->libraries[i]->elements[j]->items[k]->coords[l]);
            p_box->boxType = static_cast<Geometry_Box *>(src->libraries[i]->elements[j]->items[k])->boxType;
            break;
          case GeometryType::path:
            p_path = new Geometry_Path;
            dst->libraries[i]->elements[j]->items.push_back(p_path);
            p_path->type = GeometryType::path;
            p_path->layer = src->libraries[i]->elements[j]->items[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->properties.size(); ++l)
              p_path->properties.push_back(src->libraries[i]->elements[j]->items[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->coords.size(); ++l)
              p_path->coords.push_back(src->libraries[i]->elements[j]->items[k]->coords[l]);
            p_path->dataType = static_cast<Geometry_Path *>(src->libraries[i]->elements[j]->items[k])->dataType;
            p_path->pathType = static_cast<Geometry_Path *>(src->libraries[i]->elements[j]->items[k])->pathType;
            p_path->width = static_cast<Geometry_Path *>(src->libraries[i]->elements[j]->items[k])->width;
            break;
          case GeometryType::text:
            p_text = new Geometry_Text;
            dst->libraries[i]->elements[j]->items.push_back(p_text);
            p_text->type = GeometryType::text;
            p_text->layer = src->libraries[i]->elements[j]->items[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->properties.size(); ++l)
              p_text->properties.push_back(src->libraries[i]->elements[j]->items[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->coords.size(); ++l)
              p_text->coords.push_back(src->libraries[i]->elements[j]->items[k]->coords[l]);
            p_text->textType = static_cast<Geometry_Text *>(src->libraries[i]->elements[j]->items[k])->textType;
            p_text->pathType = static_cast<Geometry_Text *>(src->libraries[i]->elements[j]->items[k])->pathType;
            p_text->width = static_cast<Geometry_Text *>(src->libraries[i]->elements[j]->items[k])->width;
            p_text->flagsPresentation = static_cast<Geometry_Text *>(src->libraries[i]->elements[j]->items[k])->flagsPresentation;
            p_text->flagsTransformation = static_cast<Geometry_Text *>(src->libraries[i]->elements[j]->items[k])->flagsTransformation;
            p_text->magnification = static_cast<Geometry_Text *>(src->libraries[i]->elements[j]->items[k])->magnification;
            p_text->stringValue = static_cast<Geometry_Text *>(src->libraries[i]->elements[j]->items[k])->stringValue;
            break;
          case GeometryType::reference:
            p_ref = new Geometry_Reference;
            dst->libraries[i]->elements[j]->items.push_back(p_ref);
            p_ref->type = GeometryType::reference;
            p_ref->layer = src->libraries[i]->elements[j]->items[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->properties.size(); ++l)
              p_ref->properties.push_back(src->libraries[i]->elements[j]->items[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->coords.size(); ++l)
              p_ref->coords.push_back(src->libraries[i]->elements[j]->items[k]->coords[l]);
            p_ref->name = static_cast<Geometry_Reference *>(src->libraries[i]->elements[j]->items[k])->name;
            p_ref->referenceTo = static_cast<Geometry_Reference *>(src->libraries[i]->elements[j]->items[k])->referenceTo;
            p_ref->transformationFlags = static_cast<Geometry_Reference *>(src->libraries[i]->elements[j]->items[k])->transformationFlags;
            p_ref->magnification = static_cast<Geometry_Reference *>(src->libraries[i]->elements[j]->items[k])->magnification;
            break;
        }
      }
    }
  }

  bool refFound = false;
  for (size_t i = 0; i < dst->libraries.size(); ++i)
    for (size_t j = 0; j < dst->libraries[i]->elements.size(); ++j)
      for (size_t k = 0; k < dst->libraries[i]->elements[j]->items.size(); ++k) {
        if (dst->libraries[i]->elements[j]->items[k]->type != GeometryType::reference)
          continue;
        p_ref = static_cast<Geometry_Reference *>(dst->libraries[i]->elements[j]->items[k]);
        refFound = false;
        for (size_t l = 0; l < dst->libraries.size() && !refFound; ++l)
          for (size_t m = 0; m < dst->libraries[l]->elements.size() && !refFound; ++m) {
            if (dst->libraries[l]->elements[m]->name == p_ref->name) {
              p_ref->referenceTo = dst->libraries[l]->elements[m];
              refFound = true;
              break;
            }
          }
        if (!refFound) {
          ClearLayout(dst);
          return;
        }
      }

  Library *p_lib = nullptr;
  for (size_t i = 0; i < dst->libraries.size(); ++i) {
    p_lib = dst->libraries[i];
    for (size_t j = 0; j < p_lib->elements.size(); ++j) {
      for (size_t k = 0; k < p_lib->elements[j]->items.size(); ++k) {
        if (p_lib->elements[j]->items[k]->type == GeometryType::reference)
          continue;
        int layer = p_lib->elements[j]->items[k]->layer;
        size_t l = 0;
        for (; l < p_lib->layers.size(); ++l)
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

  for (size_t i = 0; i < dst->libraries.size(); ++i)
    for (size_t j = 0; j < dst->libraries[i]->layers.size(); ++j)
      if (!src->libraries[i]->layers[j].name.empty())
        dst->libraries[i]->layers[j].name = src->libraries[i]->layers[j].name;

  // Calculate bounding box for the library based on bounding boxes of the elements that are not nested
  for (size_t i = 0; i < dst->libraries.size(); ++i) {

    for (size_t j = 0; j < dst->libraries[i]->elements.size(); ++j) {
      if (dst->libraries[i]->elements[j]->nested)
        continue;

      dst->libraries[i]->min = dst->libraries[i]->elements[j]->min;
      dst->libraries[i]->max = dst->libraries[i]->elements[j]->max;

      if (dst->libraries[i]->min.x > dst->libraries[i]->elements[j]->min.x)
        dst->libraries[i]->min.x = dst->libraries[i]->elements[j]->min.x;
      if (dst->libraries[i]->min.y > dst->libraries[i]->elements[j]->min.y)
        dst->libraries[i]->min.y = dst->libraries[i]->elements[j]->min.y;
      if (dst->libraries[i]->max.x > dst->libraries[i]->elements[j]->max.x)
        dst->libraries[i]->max.x = dst->libraries[i]->elements[j]->max.x;
      if (dst->libraries[i]->max.y > dst->libraries[i]->elements[j]->max.y)
        dst->libraries[i]->max.y = dst->libraries[i]->elements[j]->max.y;
    }
  }

}

__declspec(dllexport) bool WriteLayout(Layout *layout, std::wstring fileName, FileFormat format) {
  AbstractLayoutWriter *p_writer = GetWriter(format);

  if (!p_writer)
    return false;

  bool retCode = p_writer->Write(fileName, layout);

  delete p_writer;
  p_writer = nullptr;
  return retCode;
}

__declspec(dllexport) void FreeLayout(Layout **layout) {
  if (!layout)
    return;
  if (!*layout)
    return;

  ClearLayout(*layout);

  delete (*layout);
  (*layout) = nullptr;
}

