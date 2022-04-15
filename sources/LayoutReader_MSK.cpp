/*
 * LayoutReader_MSK.cpp
 *
 * uWind (MicroWind) MSK file format reader by Mikhail S. Kotlyarov
 * 02.10.2021
 */

#include "LayoutReader_MSK.hpp"
#define _CRT_SECURE_NO_WARNINGS

#include <unordered_map>
#include <codecvt>

bool LayoutReader_MSK::IsMyFormat(const std::wstring& fName)
{
    size_t comma_pos = fName.find_last_of(L".");
    if (comma_pos == std::string::npos)
        return false;
        fileName = fName;
    std::wstring file_extention = fName.substr(comma_pos + 1, fName.length() - comma_pos);

    if (file_extention != L"MSK")
        if (file_extention != L"msk")
            return false;

    file.open(fName, std::ios::in);
    std::string line;

    std::getline(file, line);
    if (line.length() < 7) {
      file.close();
      return false;
    }
    if (line.substr(0, 7) != "VERSION") {
      file.close();
      return false;
    }
    file.close();
    return true;
}

int16_t	LayoutReader_MSK::calculate_MSK_layer_num(const std::string& layer_name)
{
    std::unordered_map <std::string, int16_t> all_layers =
    {
        {"NW",1},
        {"DN",16},
        {"DP",17},
        {"PO",13},
        {"CO",19},
        //{"P2",2},
        {"ME",23},
        {"M2",27},
        {"M3",34},
        {"M4",36},
        {"M5",53},
        {"M6",55},
    };
    auto it = all_layers.find(layer_name);
    if (it == all_layers.end())
        return -1;
    return it->second;
    
}
inline int32_t LayoutReader_MSK::find_layer_num(const std::vector <Layer>& all_layers, const uint16_t layer_num)
{
    for (size_t i = 0; i < all_layers.size(); i++)
    {
        if (all_layers[i].layer == layer_num)
        {
            return static_cast<int32_t>(i);
        }
    }

    return -1;
}

// URL: https://microeducate.tech/convert-wstring-to-string-encoded-in-utf-8/
std::string wstring_to_utf8(const std::wstring& str)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
  return myconv.to_bytes(str);
}

std::string LayoutReader_MSK::receive_element_name()
{
    const std::wstring& temp_fName = fileName;
    const size_t last_comma_pos = temp_fName.find_last_of(L".");
    const size_t start_name_pos = temp_fName.find_last_of(L"/");
    if (start_name_pos == std::string::npos) {
      //std::wstring tmp = temp_fName.substr(0, last_comma_pos);
      //std::string  retValue(tmp.begin(), tmp.end());
      return wstring_to_utf8(temp_fName.substr(0, last_comma_pos));
    }
    else {
      //std::wstring tmp = temp_fName.substr(start_name_pos + 1, last_comma_pos - start_name_pos - 1);//0 терминатор, 3 символа под расширение и 1 под точку
      //std::string  retValue(tmp.begin(), tmp.end());
      //return retValue;
      return wstring_to_utf8(temp_fName.substr(start_name_pos + 1, last_comma_pos - start_name_pos - 1));
    }
}
bool LayoutReader_MSK::Read(Layout* layout, LayoutReaderOptions* options)
{
    if (!layout)
        return false;

    file.open(fileName);
    if (!file.is_open())
        return false;
    p_layout = layout;
  
    p_active_library = new Library;
    p_active_element = new Element;
    
    p_layout->fileName = this->fileName;
    p_active_element->name = receive_element_name();
    p_active_library->name = wstring_to_utf8(fileName);
    //Переменная для хранения одной строки из файла
    std::string line_from_file;
    while (std::getline(file, line_from_file))
    {
        Coord left_bot;
        Coord right_top;
        std::string layer_name;

        if (read_Rectangle_coords(line_from_file, left_bot, right_top, layer_name))
        {
            bool is_layer_exist = true;
            Geometry* current_box = new Rectangle;
            const int16_t layer_num = calculate_MSK_layer_num(layer_name);
            if (layer_num == -1) is_layer_exist = false;


            fill_GeometryItem_box(current_box, left_bot, right_top, layer_num);
            p_active_element->geometries.push_back(current_box);

            const int32_t founded_index = find_layer_num(p_active_library->layers, layer_num);

            if (founded_index == -1 && is_layer_exist)
            {
                Layer current_layer;
                current_layer.layer = current_box->layer;
                current_layer.name = layer_name;
                current_layer.geometries.push_back(current_box);
                p_active_library->layers.push_back(current_layer);
            }
            else if(is_layer_exist)
            {
                p_active_library->layers.at(founded_index).geometries.push_back(current_box);
            }
        }

    }
  
    p_active_library->elements.push_back(p_active_element);
    p_layout->libraries.push_back(p_active_library);
   
    file.close();

    layout->fileName = fileName;
    layout->fileFormat = FileFormat::MSK;

    layout->libraries[0]->elements[0]->min = layout->libraries[0]->elements[0]->geometries[0]->min;
    layout->libraries[0]->elements[0]->max = layout->libraries[0]->elements[0]->geometries[0]->max;

    for (size_t i = 1; i < layout->libraries[0]->elements[0]->geometries.size(); ++i) {
      if (layout->libraries[0]->elements[0]->min.x > layout->libraries[0]->elements[0]->geometries[i]->min.x)
        layout->libraries[0]->elements[0]->min.x = layout->libraries[0]->elements[0]->geometries[i]->min.x;
      if (layout->libraries[0]->elements[0]->min.y > layout->libraries[0]->elements[0]->geometries[i]->min.y)
        layout->libraries[0]->elements[0]->min.y = layout->libraries[0]->elements[0]->geometries[i]->min.y;
      if (layout->libraries[0]->elements[0]->max.x < layout->libraries[0]->elements[0]->geometries[i]->max.x)
        layout->libraries[0]->elements[0]->max.x = layout->libraries[0]->elements[0]->geometries[i]->max.x;
      if (layout->libraries[0]->elements[0]->max.y < layout->libraries[0]->elements[0]->geometries[i]->max.y)
        layout->libraries[0]->elements[0]->max.y = layout->libraries[0]->elements[0]->geometries[i]->max.y;
    }

    layout->libraries[0]->min = layout->libraries[0]->elements[0]->min;
    layout->libraries[0]->max = layout->libraries[0]->elements[0]->max;

    return true; 
}


inline bool LayoutReader_MSK::read_Rectangle_coords(const std::string& line, Coord& left_bot, Coord& right_top, std::string& layer_name)
{
    char c_layer_name[4];
    int width = 0, height = 0;

    if (!sscanf_s(line.c_str(), "REC(%d,%d,%d,%d,%s)", &left_bot.x, &left_bot.y, &width, &height, c_layer_name,4))
    {
        return false;
    }

    c_layer_name[strlen(c_layer_name) - 1] = '\0';
    layer_name = c_layer_name;
    right_top.x = left_bot.x + width;
    right_top.y = left_bot.y + height;

    
   
    return true;
}

void LayoutReader_MSK::fill_GeometryItem_box(Geometry* filling_box, const Coord& left_bot, const Coord& right_top, const uint16_t layer_num)
{
    Coord curr_coord;
    int32_t dx = calculate_delta(left_bot.x, right_top.x);
    int32_t dy = calculate_delta(left_bot.y, right_top.y);

    //Left top
    curr_coord.x = right_top.x - dx;
    curr_coord.y = right_top.y;
    filling_box->coords.push_back(curr_coord);
    
    //Right top
    filling_box->coords.push_back(right_top);

    //Right bot
    curr_coord.x = right_top.x;
    curr_coord.y = right_top.y - dy;
    filling_box->coords.push_back(curr_coord);

    //Left bot
    curr_coord.x = right_top.x - dx;
    curr_coord.y = right_top.y - dy;
    filling_box->coords.push_back(curr_coord);

    //Left top
    curr_coord.x = right_top.x - dx;
    curr_coord.y = right_top.y;
    filling_box->coords.push_back(curr_coord);

    filling_box->layer = layer_num;

    filling_box->min = filling_box->max = filling_box->coords[0];
    for (size_t i = 1; i < filling_box->coords.size(); ++i) {
      if (filling_box->min.x > filling_box->coords[i].x)
        filling_box->min.x = filling_box->coords[i].x;
      if (filling_box->min.y > filling_box->coords[i].y)
        filling_box->min.y = filling_box->coords[i].y;
      if (filling_box->max.x < filling_box->coords[i].x)
        filling_box->max.x = filling_box->coords[i].x;
      if (filling_box->max.y < filling_box->coords[i].y)
        filling_box->max.y = filling_box->coords[i].y;
    }
}

inline int32_t LayoutReader_MSK::calculate_delta(const int32_t first,const int32_t second)
{
    return second - first;
}
