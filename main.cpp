#define OLC_PGE_APPLICATION 
#include "olcPixelGameEngine.h"

#include <iostream>
#include <algorithm>
#include "utils.hpp"
#include <cmath>

struct Item
{
private:
	uint32_t widht = 0;
	uint32_t heigth = 0;
	olc::vu2d coordinates = {0,0};
	olc::Pixel color;
public:
	Item(uint32_t height_value, uint32_t width_value)
		: widht(width_value), heigth(height_value) {
			color = olc::Pixel(rand() % 255, rand() % 255, rand() % 255);
		}

	uint32_t getHeight(){
		return heigth;
	}
	uint32_t getWidth(){
		return widht;
	}

	bool moveItem(olc::vu2d newPosition){
		coordinates = newPosition;
		return true;
	}

	olc::vu2d getLeftUpperCorner(){
		return coordinates;
	}

	olc::vu2d getRightBottomCorner(){
		return olc::vu2d(coordinates.x + widht, coordinates.y + heigth);
	}

	olc::vu2d getSize(){
		return olc::vu2d(widht, heigth);
	}

	olc::Pixel getColor() {
		return color;
	}
};

struct Segment
{
public:
	uint32_t start = 0;
	uint32_t end = 0;
	Segment(uint32_t start, uint32_t end)
		:start(start), end(end)	{}
};

struct Line
{
private:
	std::vector<Segment> free_segments;
public:
	Line(Segment size) {
		free_segments.push_back(size);
	}

	bool removeSegment(Segment segment_to_remove){
		int indexSegment = containsSegment(segment_to_remove);
		if (indexSegment != -1){
			// Check if the segment to remove is in the start 
			if (segment_to_remove.start == free_segments[indexSegment].start){
				free_segments[indexSegment].start = segment_to_remove.end;
			}
			// Check if the segment to remove is in the finish
			else if (segment_to_remove.end == free_segments[indexSegment].end){
				free_segments[indexSegment].end = segment_to_remove.start;
			}
			// Split the segment in two
			else {
				free_segments.push_back(Segment(segment_to_remove.end, free_segments[indexSegment].end));
				free_segments[indexSegment].end = segment_to_remove.start;
			}
			return true;
		}
		return false;
	}

	// Returns the index of the first segment that can fit, returns -1 if the line doesnt contain the segment
	int containsSegment(Segment segment) {
		for (int c = 0; c < free_segments.size(); c++){
			if (free_segments[c].start <= segment.start && free_segments[c].end >= segment.end){
				return c;
			}
		}
		return -1;
	}
};

struct Bin
{
private:
	uint32_t height = 0;
	uint32_t width = 0;

	olc::vu2d coordinates = {0, 0};
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
	}

	bool insert (Item item)
	{
		uint32_t item_width = item.getWidth();
		uint32_t item_height = item.getHeight();
		if (item_height > height || item_width > width) return false;
		if (item_height == 0 || item_width == 0) return false;

		uint32_t offset_x = 0;
		uint32_t offset_y = 0;
		for (offset_x = 0; offset_x <= (width - item_width); offset_x++) {
			for (offset_y = 0; offset_y <= (height - item_height); offset_y++){
				Segment cur_segment = Segment(offset_y, offset_y + item_height);
			
				// If this line has the segment free, then search on the adjacent lines for the same segment
				if (lines[offset_x].containsSegment(cur_segment) != -1){
					bool adjacentLinesHaveSpace = true;
					for(int c = 0; c < item_width; c++) {
						if (lines[c + offset_x].containsSegment(cur_segment) == -1) {
							adjacentLinesHaveSpace = false;
							break;
						}
					}

					if (adjacentLinesHaveSpace){
						item.moveItem(olc::vu2d(offset_x+coordinates.x, offset_y+coordinates.y));
						items.push_back(item);

						// remove the segment from all 
						for (int c = 0; c < item_width; c++){
							lines[c + offset_x].removeSegment(cur_segment);
						}

						return true;
					}
				}
			}
		}

		return false;
	}

	olc::vu2d getLeftUpperCorner(){
		return coordinates;
	}
	olc::vu2d getRightBottomCorner(){
		return olc::vu2d(coordinates.x + width, coordinates.y + height);
	}
	olc::vu2d getSize(){
		return olc::vu2d(width, height);
	}

	bool moveItem(olc::vu2d newPosition){
		int32_t offset_x = newPosition.x - coordinates.x;
		int32_t offset_y = newPosition.y - coordinates.y;
		coordinates = newPosition;
		// Have to move all the items
		for (int c = 0; c < items.size(); c++){
			items[c].moveItem(
				olc::vu2d(
					items[c].getLeftUpperCorner().x + offset_x,
					items[c].getLeftUpperCorner().y + offset_y
				)
			);
		}
		return true;
	}

	std::vector<Item>& getItemsInBin(){
		return items;
	}
};


class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "BinPack 2D";
	}

public:
	uint32_t bin_width = 64;
	uint32_t bin_height = 64;
	std::vector<Bin> bins;

	bool draw(float fElapsedTime)
	{	
		for (int c = 0; c < bins.size(); c++){
			Bin bin = bins[c];
			// Draw the bin
			DrawRect(
				bin.getLeftUpperCorner() - olc::vu2d(1, 1),
				bin.getSize() + olc::vu2d(1, 1)
			);

			// Draw each item in the bin
			std::vector<Item> items = bin.getItemsInBin();

			for (int c = 0; c < items.size(); c++){
				FillRect(
					items[c].getLeftUpperCorner(),
					items[c].getSize(),
					items[c].getColor()
				);
			}
		}
		return true;
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		int items_to_pack = 20;
		srand(time(NULL));
		for (int c = 0; c < items_to_pack; c++){
			std::cout << "Inserted: " << insert(Item((rand() % bin_height) + 1, (rand() % bin_width)+ 1)) << "\n";
		}
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		draw(fElapsedTime);
		return true;
	}

	bool insert(Item item){
		if (item.getHeight() > bin_height || item.getWidth() > bin_width) return false;
		for (int c = 0; c < bins.size(); c++){
			if (bins[c].insert(item)) {
				return true;
			}
		}

		// Create new Bin
		Bin new_bin = Bin(bin_height, bin_width);
		
		// Move all bins before
		uint32_t positions = (uint32_t) ScreenWidth() / (bins.size() + 2);
		for (int c = 0; c < bins.size(); c++){
			bins[c].moveItem(
				olc::vu2d(
					positions * (c + 1) - bin_width / 2, 
					(uint32_t) ScreenHeight() / 2 - bin_height / 2
				)
			);
		}
		
		new_bin.moveItem(
			olc::vu2d(
				positions * (bins.size() + 1) - bin_width / 2,  
				(uint32_t) ScreenHeight() / 2 - bin_height / 2
			)
		);
		if (new_bin.insert(item) == false) return false;
		bins.push_back(new_bin);
		return true;
	}
};

int main()
{
	{
		Example app;
		if (app.Construct(640, 360, 2, 2)){
			app.Start();
		}
	}
	return 0;
}