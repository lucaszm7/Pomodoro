#pragma once

#include "olcPixelGameEngine.h"
#include "utils.hpp"
#include "item.hpp"
#include "line.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

struct Bin
{
private:
	uint32_t height = 0;
	uint32_t width = 0;

	uint32_t area_free = 0;

	olc::vi2d coordinates = {0, 0};
	std::vector<Item> items;
	std::vector<Line> lines;

public:
	Bin(uint32_t height, uint32_t width) 
		: height(height), width(width)
	{
		// Create the vertical lines
		for (int c = 0; c < width; c++) {
			lines.push_back(Line(Segment(0, height)));
		}
		area_free = width * height;
	}

	Bin(olc::vu2d coor, olc::vu2d size) 
		: coordinates(coor), width(size.x), height(size.y)
	{
		for (int c = 0; c < width; c++) {
			lines.push_back(Line(Segment(0, height)));
		}
		area_free = width * height;
	}

	bool insert (Item item)
	{
		uint32_t item_width = item.getWidth();
		uint32_t item_height = item.getHeight();
		if (item_height > height || item_width > width) return false;
		if (item_height == 0 || item_width == 0) return false;
		if (area_free < item.getArea()) return false;

		for (uint32_t offset_x = 0; offset_x <= (width - item_width); offset_x++) {
			
			// Checks if the adjacent lines have at least the height that is needed to fit the item
			bool allLinesHaveSpace = true;
			for (int c = 0; c < item_width; c++) {
				if (lines[c + offset_x].getSumOfSegmentsFree() < item_height) {
					allLinesHaveSpace = false;
					offset_x = c + offset_x;
					break;
				}
			}

			if (allLinesHaveSpace){
				for (uint32_t offset_y = 0; offset_y <= (height - item_height); offset_y++){
					Segment cur_segment = Segment(offset_y, offset_y + item_height);

					// If this line has the segment free, then search on the adjacent lines for the same segment
					bool allLinesHaveSegment = true;
					for(int c = 0; c < item_width; c++) {
						if (lines[c + offset_x].containsSegment(cur_segment) == -1) {
							allLinesHaveSegment = false;
							break;
						}
					}
					if (allLinesHaveSegment){
						item.moveItem(olc::vi2d(offset_x+coordinates.x, offset_y+coordinates.y));
						items.push_back(item);

						// remove the segment from all lines
						for (int c = 0; c < item_width; c++){
							lines[c + offset_x].removeSegment(cur_segment);
						}

						area_free = area_free - item.getWidth() * item.getHeight();
						return true;
					}
				}
			}
		}

		return false;
	}

	olc::vi2d getLeftUpperCorner() const {
		return coordinates;
	}
	olc::vi2d getRightBottomCorner() const {
		return olc::vi2d(coordinates.x + width, coordinates.y + height);
	}
	olc::vi2d getSize() const {
		return olc::vi2d(width, height);
	}

	bool moveItem(olc::vi2d newPosition){
		int32_t offset_x = newPosition.x - coordinates.x;
		int32_t offset_y = newPosition.y - coordinates.y;
		coordinates = newPosition;
		// Have to move all the items
		for (int c = 0; c < items.size(); c++){
			items[c].moveItem(
				olc::vi2d(
					items[c].getLeftUpperCorner().x + offset_x,
					items[c].getLeftUpperCorner().y + offset_y
				)
			);
		}
		return true;
	}

	const std::vector<Item>& getItemsInBin() const {
		return items;
	}

	uint32_t getArea() const
	{
		return width * height;
	}

	uint32_t getAreaFree() {
		return area_free;
	}

	uint32_t getAreaUsed() {
		return (width * height) - area_free;
	}
};
