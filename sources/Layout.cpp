#include "Layout.hpp"
#include "LayoutReader.hpp"

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

  GeometryItem_Boundary      *p_boundary = nullptr;
  GeometryItem_Path          *p_path = nullptr;
  GeometryItem_Text          *p_text = nullptr;
  GeometryItem_Box           *p_box = nullptr;
  GeometryItem_StructureRef  *p_sref = nullptr;
  
  for (size_t i = 0; i < src->libraries.size(); ++i) {
    Library *p_lib = new Library;
    p_lib->name = src->libraries[i]->name;
    p_lib->units = src->libraries[i]->units;
    dst->libraries.push_back(p_lib);
    for (size_t j = 0; j < src->libraries[i]->elements.size(); ++j) {
      Element *p_element = new Element;
      p_element->name = src->libraries[i]->elements[j]->name;
      dst->libraries[i]->elements.push_back(p_element);
      for (size_t k = 0; k < src->libraries[i]->elements[j]->items.size(); ++k) {
        switch (src->libraries[i]->elements[j]->items[k]->type) {
          case ItemType::boundary:
            p_boundary = new GeometryItem_Boundary;
            dst->libraries[i]->elements[j]->items.push_back(p_boundary);
            p_boundary->type = ItemType::boundary;
            p_boundary->layer = src->libraries[i]->elements[j]->items[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->properties.size(); ++l)
              p_boundary->properties.push_back(src->libraries[i]->elements[j]->items[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->coords.size(); ++l)
              p_boundary->coords.push_back(src->libraries[i]->elements[j]->items[k]->coords[l]);
            p_boundary->dataType = static_cast<GeometryItem_Boundary *>(src->libraries[i]->elements[j]->items[k])->dataType;
            break;
          case ItemType::box:
            p_box = new GeometryItem_Box;
            dst->libraries[i]->elements[j]->items.push_back(p_box);
            p_box->type = ItemType::box;
            p_box->layer = src->libraries[i]->elements[j]->items[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->properties.size(); ++l)
              p_box->properties.push_back(src->libraries[i]->elements[j]->items[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->coords.size(); ++l)
              p_box->coords.push_back(src->libraries[i]->elements[j]->items[k]->coords[l]);
            p_box->boxType = static_cast<GeometryItem_Box *>(src->libraries[i]->elements[j]->items[k])->boxType;
            break;
          case ItemType::path:
            p_path = new GeometryItem_Path;
            dst->libraries[i]->elements[j]->items.push_back(p_path);
            p_path->type = ItemType::path;
            p_path->layer = src->libraries[i]->elements[j]->items[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->properties.size(); ++l)
              p_path->properties.push_back(src->libraries[i]->elements[j]->items[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->coords.size(); ++l)
              p_path->coords.push_back(src->libraries[i]->elements[j]->items[k]->coords[l]);
            p_path->dataType = static_cast<GeometryItem_Path *>(src->libraries[i]->elements[j]->items[k])->dataType;
            p_path->pathType = static_cast<GeometryItem_Path *>(src->libraries[i]->elements[j]->items[k])->pathType;
            p_path->width = static_cast<GeometryItem_Path *>(src->libraries[i]->elements[j]->items[k])->width;
            break;
          case ItemType::text:
            p_text = new GeometryItem_Text;
            dst->libraries[i]->elements[j]->items.push_back(p_text);
            p_text->type = ItemType::text;
            p_text->layer = src->libraries[i]->elements[j]->items[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->properties.size(); ++l)
              p_text->properties.push_back(src->libraries[i]->elements[j]->items[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->coords.size(); ++l)
              p_text->coords.push_back(src->libraries[i]->elements[j]->items[k]->coords[l]);
            p_text->textType = static_cast<GeometryItem_Text *>(src->libraries[i]->elements[j]->items[k])->textType;
            p_text->pathType = static_cast<GeometryItem_Text *>(src->libraries[i]->elements[j]->items[k])->pathType;
            p_text->width = static_cast<GeometryItem_Text *>(src->libraries[i]->elements[j]->items[k])->width;
            p_text->flagsPresentation = static_cast<GeometryItem_Text *>(src->libraries[i]->elements[j]->items[k])->flagsPresentation;
            p_text->flagsTransformation = static_cast<GeometryItem_Text *>(src->libraries[i]->elements[j]->items[k])->flagsTransformation;
            p_text->magnification = static_cast<GeometryItem_Text *>(src->libraries[i]->elements[j]->items[k])->magnification;
            p_text->stringValue = static_cast<GeometryItem_Text *>(src->libraries[i]->elements[j]->items[k])->stringValue;
            break;
          case ItemType::structRef:
            p_sref = new GeometryItem_StructureRef;
            dst->libraries[i]->elements[j]->items.push_back(p_sref);
            p_sref->type = ItemType::structRef;
            p_sref->layer = src->libraries[i]->elements[j]->items[k]->layer;
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->properties.size(); ++l)
              p_sref->properties.push_back(src->libraries[i]->elements[j]->items[k]->properties[l]);
            for (size_t l = 0; l < src->libraries[i]->elements[j]->items[k]->coords.size(); ++l)
              p_sref->coords.push_back(src->libraries[i]->elements[j]->items[k]->coords[l]);
            p_sref->name = static_cast<GeometryItem_StructureRef *>(src->libraries[i]->elements[j]->items[k])->name;
            p_sref->pReference = static_cast<GeometryItem_StructureRef *>(src->libraries[i]->elements[j]->items[k])->pReference;
            p_sref->transformationFlags = static_cast<GeometryItem_StructureRef *>(src->libraries[i]->elements[j]->items[k])->transformationFlags;
            p_sref->magnification = static_cast<GeometryItem_StructureRef *>(src->libraries[i]->elements[j]->items[k])->magnification;
            break;
        }
      }
    }
  }

  bool refFound = false;
  for (size_t i = 0; i < dst->libraries.size(); ++i)
    for (size_t j = 0; j < dst->libraries[i]->elements.size(); ++j)
      for (size_t k = 0; k < dst->libraries[i]->elements[j]->items.size(); ++k) {
        if (dst->libraries[i]->elements[j]->items[k]->type != ItemType::structRef)
          continue;
        p_sref = static_cast<GeometryItem_StructureRef *>(dst->libraries[i]->elements[j]->items[k]);
        refFound = false;
        for (size_t l = 0; l < dst->libraries.size() && !refFound; ++l)
          for (size_t m = 0; m < dst->libraries[l]->elements.size() && !refFound; ++m) {
            if (dst->libraries[l]->elements[m]->name == p_sref->name) {
              p_sref->pReference = dst->libraries[l]->elements[m];
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
        if (p_lib->elements[j]->items[k]->type == ItemType::structRef)
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

