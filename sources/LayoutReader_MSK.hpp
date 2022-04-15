/*
 * LayoutReader_MSK.hpp
 *
 * Layout formats implementation:
 * MSK          - Mikhail S. Kotlyarov 
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "LayoutReader.hpp"
#include "Layout.hpp"

class LayoutReader_MSK:public LayoutReader
{
private:
	Library*			p_active_library;
	Element*			p_active_element;
	Geometry*     p_active_geometry_item;

public:
	LayoutReader_MSK() :p_active_library(nullptr),p_active_element(nullptr),p_active_geometry_item(nullptr) {}

	bool            IsMyFormat(const std::wstring& fName) override final;
	bool            Read(Layout *layout, LayoutReaderOptions *options) override final;
private:
	inline bool     read_Rectangle_coords(const std::string& line, Coord& left_bot, Coord& right_top, std::string& layer_name);
	void            fill_GeometryItem_box(Geometry* filling_box, const Coord& right_top, const Coord& left_bot, const uint16_t layer_num);
	inline int32_t  calculate_delta(const int32_t first, const int32_t second);
	int16_t         calculate_MSK_layer_num(const std::string& layer_name);
	inline int32_t  find_layer_num(const std::vector <Layer>& all_layers, const uint16_t layer_num);
	std::string     receive_element_name();

};
