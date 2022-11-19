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

	olc::vu2d getLeftUpperCorner() const {
		return coordinates;
	}

	olc::vu2d getRightBottomCorner() const {
		return olc::vu2d(coordinates.x + widht, coordinates.y + heigth);
	}

	olc::vu2d getSize() const {
		return olc::vu2d(widht, heigth);
	}

	olc::Pixel getColor() const {
		return color;
	}

	uint32_t getArea() {
		return widht * heigth;
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

	uint32_t area_free = 0;

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
			for (uint32_t offset_y = 0; offset_y <= (height - item_height); offset_y++){
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


						area_free = area_free - item.getWidth() * item.getHeight();
						return true;
					}
				}
			}
		}

		return false;
	}

	olc::vu2d getLeftUpperCorner() const {
		return coordinates;
	}
	olc::vu2d getRightBottomCorner() const {
		return olc::vu2d(coordinates.x + width, coordinates.y + height);
	}
	olc::vu2d getSize() const {
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

	const std::vector<Item>& getItemsInBin() const {
		return items;
	}

	uint32_t getArea() {
		return width * height;
	}
};


class BinPacking2D : public olc::PixelGameEngine
{
public:
	uint32_t bin_width = 128;
	uint32_t bin_height = 256;
	std::vector<Bin> bins;

	BinPacking2D()
	{
		sAppName = "BinPacking2D";
	}

public:
	bool Draw(float fElapsedTime)
	{
		Clear(olc::VERY_DARK_BLUE);

		for(const auto& bin : bins)
		{
			olc::vd2d binTLbefore = bin.getLeftUpperCorner() - olc::vu2d(1,1);
			olc::vd2d binBRbefore = bin.getRightBottomCorner() + olc::vu2d(1,1);

			olc::vi2d binTLafter;
			olc::vi2d binBRafter;

			WorldToScreen(binTLbefore, binTLafter);
			WorldToScreen(binBRbefore, binBRafter);

			FillRect(binTLafter, binBRafter - binTLafter, olc::BLACK);
			DrawRect(binTLafter, binBRafter - binTLafter, olc::WHITE);

			olc::vi2d itemTLafter;
			olc::vi2d itemBRafter;
			for(const auto& item : bin.getItemsInBin())
			{
				WorldToScreen(item.getLeftUpperCorner(), binTLafter);
				WorldToScreen(item.getRightBottomCorner(), binBRafter);
				FillRect(binTLafter, binBRafter - binTLafter, item.getColor());
			}
		}
		return true;
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		int big_items_to_pack = 500;
		int medium_items_to_pack = 2000;
		int small_items_to_pack = 10000;
		srand(time(NULL));
		
		// Measure the time to pack
		Timer T = Timer();
		for (int c = 0; c < big_items_to_pack; c++){
			std::cout << "Inserted: " << insert(Item((rand() % (bin_height)) + 1, (rand() % (bin_width))+ 1)) << "\n";
		}
		for (int c = 0; c < medium_items_to_pack; c++){
			std::cout << "Inserted: " << insert(Item((rand() % (bin_height / 2)) + 1, (rand() % (bin_width / 2))+ 1)) << "\n";
		}
		for (int c = 0; c < small_items_to_pack; c++){
			std::cout << "Inserted: " << insert(Item((rand() % (bin_height / 4)) + 1, (rand() % (bin_width / 4))+ 1)) << "\n";
		}

		std::cout << "Time Taken: " << T.now() << "s\n";

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Draw(fElapsedTime);

		// Panning and Zoomig, credits to @OneLoneCoder who i'am inpired for
        olc::vd2d vMouse = {(double)GetMouseX(), (double)GetMouseY()};

        // Get the position of the mouse and move the world Final Pos - Inital Pos
        // This make us drag Around the Screen Space, with the OffSet variable
        if(GetMouse(0).bPressed)
        {
            vStartPan = vMouse;
        }

        if(GetMouse(0).bHeld)
        {
            vOffset -= (vMouse - vStartPan) / vScale;
            vStartPan = vMouse;
        }

        olc::vd2d vMouseBeforeZoom;
        ScreenToWorld(vMouse, vMouseBeforeZoom);

		if (GetKey(olc::Key::E).bHeld) vScale *= 1.1; 
		if (GetKey(olc::Key::Q).bHeld) vScale *= 0.9;
		
		olc::vd2d vMouseAfterZoom;
		ScreenToWorld(vMouse, vMouseAfterZoom);
		vOffset += (vMouseBeforeZoom - vMouseAfterZoom);

		return true;
	}


protected:
	protected:
    // Pan & Zoom variables
	olc::vd2d vOffset = { 0.0, 0.0 };
	olc::vd2d vStartPan = { 0.0, 0.0 };
	olc::vd2d vScale = { 1.0, 1.0 };

    void ScreenToWorld(const olc::vi2d& s, olc::vd2d& w)
	{
		w.x = (double)(s.x) / vScale.x + vOffset.x;
		w.y = (double)(s.y) / vScale.y + vOffset.y;
	}

    // Converte coords from Screen Space to World Space
    void WorldToScreen(const olc::vd2d& w, olc::vi2d &s)
	{
		s.x = (int)((w.x - vOffset.x) * vScale.x);
		s.y = (int)((w.y - vOffset.y) * vScale.y);
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
		
		new_bin.moveItem(olc::vu2d((bins.size() + 1) * (bin_width + 20), (uint32_t) ScreenHeight() / 2 - bin_height / 2));
		
		if (new_bin.insert(item) == false) return false;
		bins.push_back(new_bin);
		return true;
	}

};

int main()
{
	{
		BinPacking2D app;
		if (app.Construct(640, 360, 2, 2)){
			app.Start();
		}
	}
	return 0;
}