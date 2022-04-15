#include "Layout.hpp"
#include "LayoutReader.hpp"
#include "LayoutWriter.hpp"

__declspec(dllexport) Layout *CreateLayoutFromFile(std::wstring fileName, LayoutReaderOptions *options) {
  LayoutReader *p_reader = GetReader(fileName);

  if (!p_reader)
    return nullptr;

  Layout *p_layout = new Layout;
  p_layout->fileFormat = FileFormat::undefined;

  if (!p_reader->Read(p_layout, options)) {
    FreeReader(p_reader);
    delete p_layout;
    return nullptr;
  }
  FreeReader(p_reader);
  
  p_layout->fileName = fileName;
  return p_layout;
}

__declspec(dllexport) void ReadLayoutFromFile(std::wstring fileName, Layout *layout, LayoutReaderOptions *options) {
  LayoutReader *p_reader = GetReader(fileName);

  if (!p_reader)
    return;

  layout->fileFormat = FileFormat::undefined;

  if (!p_reader->Read(layout, options)) {
    FreeReader(p_reader);
    return;
  }
  FreeReader(p_reader);
  
  layout->fileName = fileName;
}

__declspec(dllexport) void ClearLayout(Layout *layout) {
  for (size_t i = 0; i < layout->libraries.size(); ++i) {

    for (size_t j = 0; j < layout->libraries[i]->elements.size(); ++j) {

      for (size_t k = 0; k < layout->libraries[i]->elements[j]->geometries.size(); ++k) {
        delete layout->libraries[i]->elements[j]->geometries[k];
        layout->libraries[i]->elements[j]->geometries[k] = nullptr;
      }
      layout->libraries[i]->elements[j]->geometries.clear();

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

  Polygon   *p_polygon = nullptr;
  Path      *p_path    = nullptr;
  Text      *p_text    = nullptr;
  Rectangle *p_rect    = nullptr;
  Reference *p_ref     = nullptr;
  
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
      for (size_t k = 0; k < src->libraries[i]->elements[j]->geometries.size(); ++k) {
        switch (src->libraries[i]->elements[j]->geometries[k]->type) {
          case GeometryType::polygon:
            p_polygon = new Polygon;
            dst->libraries[i]->elements[j]->geometries.push_back(p_polygon);
            p_polygon->type = GeometryType::polygon;
            p_polygon->layer = src->libraries[i]->elements[j]->geometries[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->geometries[k]->properties.size(); ++l)
              p_polygon->properties.push_back(src->libraries[i]->elements[j]->geometries[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->geometries[k]->coords.size(); ++l)
              p_polygon->coords.push_back(src->libraries[i]->elements[j]->geometries[k]->coords[l]);
            p_polygon->dataType = static_cast<Polygon *>(src->libraries[i]->elements[j]->geometries[k])->dataType;
            break;
          case GeometryType::rectangle:
            p_rect = new Rectangle;
            dst->libraries[i]->elements[j]->geometries.push_back(p_rect);
            p_rect->type = GeometryType::rectangle;
            p_rect->layer = src->libraries[i]->elements[j]->geometries[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->geometries[k]->properties.size(); ++l)
              p_rect->properties.push_back(src->libraries[i]->elements[j]->geometries[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->geometries[k]->coords.size(); ++l)
              p_rect->coords.push_back(src->libraries[i]->elements[j]->geometries[k]->coords[l]);
            p_rect->rectType = static_cast<Rectangle *>(src->libraries[i]->elements[j]->geometries[k])->rectType;
            break;
          case GeometryType::path:
            p_path = new Path;
            dst->libraries[i]->elements[j]->geometries.push_back(p_path);
            p_path->type = GeometryType::path;
            p_path->layer = src->libraries[i]->elements[j]->geometries[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->geometries[k]->properties.size(); ++l)
              p_path->properties.push_back(src->libraries[i]->elements[j]->geometries[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->geometries[k]->coords.size(); ++l)
              p_path->coords.push_back(src->libraries[i]->elements[j]->geometries[k]->coords[l]);
            p_path->dataType = static_cast<Path *>(src->libraries[i]->elements[j]->geometries[k])->dataType;
            p_path->pathType = static_cast<Path *>(src->libraries[i]->elements[j]->geometries[k])->pathType;
            p_path->width = static_cast<Path *>(src->libraries[i]->elements[j]->geometries[k])->width;
            break;
          case GeometryType::text:
            p_text = new Text;
            dst->libraries[i]->elements[j]->geometries.push_back(p_text);
            p_text->type = GeometryType::text;
            p_text->layer = src->libraries[i]->elements[j]->geometries[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->geometries[k]->properties.size(); ++l)
              p_text->properties.push_back(src->libraries[i]->elements[j]->geometries[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->geometries[k]->coords.size(); ++l)
              p_text->coords.push_back(src->libraries[i]->elements[j]->geometries[k]->coords[l]);
            p_text->textType = static_cast<Text *>(src->libraries[i]->elements[j]->geometries[k])->textType;
            p_text->pathType = static_cast<Text *>(src->libraries[i]->elements[j]->geometries[k])->pathType;
            p_text->width = static_cast<Text *>(src->libraries[i]->elements[j]->geometries[k])->width;
            p_text->flagsPresentation = static_cast<Text *>(src->libraries[i]->elements[j]->geometries[k])->flagsPresentation;
            p_text->flagsTransformation = static_cast<Text *>(src->libraries[i]->elements[j]->geometries[k])->flagsTransformation;
            p_text->magnification = static_cast<Text *>(src->libraries[i]->elements[j]->geometries[k])->magnification;
            p_text->stringValue = static_cast<Text *>(src->libraries[i]->elements[j]->geometries[k])->stringValue;
            break;
          case GeometryType::reference:
            p_ref = new Reference;
            dst->libraries[i]->elements[j]->geometries.push_back(p_ref);
            p_ref->type = GeometryType::reference;
            p_ref->layer = src->libraries[i]->elements[j]->geometries[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->geometries[k]->properties.size(); ++l)
              p_ref->properties.push_back(src->libraries[i]->elements[j]->geometries[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->geometries[k]->coords.size(); ++l)
              p_ref->coords.push_back(src->libraries[i]->elements[j]->geometries[k]->coords[l]);
            p_ref->name = static_cast<Reference *>(src->libraries[i]->elements[j]->geometries[k])->name;
            p_ref->referenceTo = static_cast<Reference *>(src->libraries[i]->elements[j]->geometries[k])->referenceTo;
            p_ref->transformationFlags = static_cast<Reference *>(src->libraries[i]->elements[j]->geometries[k])->transformationFlags;
            p_ref->magnification = static_cast<Reference *>(src->libraries[i]->elements[j]->geometries[k])->magnification;
            break;
        }
      }
    }
  }

  bool refFound = false;
  for (size_t i = 0; i < dst->libraries.size(); ++i)
    for (size_t j = 0; j < dst->libraries[i]->elements.size(); ++j)
      for (size_t k = 0; k < dst->libraries[i]->elements[j]->geometries.size(); ++k) {
        if (dst->libraries[i]->elements[j]->geometries[k]->type != GeometryType::reference)
          continue;
        p_ref = static_cast<Reference *>(dst->libraries[i]->elements[j]->geometries[k]);
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
      for (size_t k = 0; k < p_lib->elements[j]->geometries.size(); ++k) {
        if (p_lib->elements[j]->geometries[k]->type == GeometryType::reference)
          continue;
        int layer = p_lib->elements[j]->geometries[k]->layer;
        size_t l = 0;
        for (; l < p_lib->layers.size(); ++l)
          if (p_lib->layers[l].layer == layer) {
            p_lib->layers[l].geometries.push_back(p_lib->elements[j]->geometries[k]);
            break;
          }
        if (l < p_lib->layers.size())
          continue;
        Layer li;
        li.layer = layer;
        li.geometries.push_back(p_lib->elements[j]->geometries[k]);
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
  LayoutWriter *p_writer = GetWriter(format);

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

