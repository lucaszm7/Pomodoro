#define OLC_PGE_APPLICATION 
#include "include/olcPixelGameEngine.h"
#include "include/utils.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

struct Item
{
private:
	uint32_t widht = 0;
	uint32_t heigth = 0;
	olc::vi2d coordinates = {0,0};
	olc::Pixel color;
public:
	Item(uint32_t height_value, uint32_t width_value)
		: widht(width_value), heigth(height_value) {
			color = olc::Pixel(rand() % 255, rand() % 255, rand() % 255);
			
			// Prevent a rectangle of having the same color as the bin background
			if (color == olc::BACK) color = olc::WHITE;
		}

	uint32_t getHeight(){
		return heigth;
	}
	uint32_t getWidth(){
		return widht;
	}

	bool moveItem(olc::vi2d newPosition){
		coordinates = newPosition;
		return true;
	}

	olc::vi2d getLeftUpperCorner() const {
		return coordinates;
	}

	olc::vi2d getRightBottomCorner() const {
		return olc::vi2d(coordinates.x + widht, coordinates.y + heigth);
	}

	olc::vi2d getSize() const {
		return olc::vi2d(widht, heigth);
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
	uint32_t getSize() const {
		return end - start;
	}
};

struct Line
{
private:
	std::vector<Segment> free_segments;
	uint32_t sum_of_segments_free = 0;
public:
	Line(Segment size) {
		free_segments.push_back(size);
		sum_of_segments_free = size.getSize();
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
			sum_of_segments_free -= segment_to_remove.getSize();
			return true;
		}
		return false;
	}

	// Returns the index of the first segment that can fit, returns -1 if the line doesnt contain the segment
	int containsSegment(Segment segment) {
		if (sum_of_segments_free < segment.getSize()) return -1;
		for (int c = 0; c < free_segments.size(); c++){
			if (free_segments[c].start <= segment.start && free_segments[c].end >= segment.end){
				return c;
			}
		}
		return -1;
	}

	uint32_t getSumOfSegmentsFree() const {
		return sum_of_segments_free;
	}
};

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


class BinPacking2D : public olc::PixelGameEngine
{
public:
	uint32_t bin_width = 128;
	uint32_t bin_height = 256;
	std::vector<Bin> bins;
	std::vector<Item> items_buffer;

	BinPacking2D()
	{
		sAppName = "BinPacking2D";
	}

public:
	bool Draw(float fElapsedTime)
	{
		Clear(olc::VERY_DARK_BLUE);

		for(auto& bin : bins)
		{
			olc::vd2d binTLbefore = bin.getLeftUpperCorner() - olc::vi2d(1,1);
			olc::vd2d binBRbefore = bin.getRightBottomCorner() + olc::vi2d(1,1);

			olc::vi2d binTLafter;
			olc::vi2d binBRafter;

			WorldToScreen(binTLbefore, binTLafter);
			WorldToScreen(binBRbefore, binBRafter);

			FillRect(binTLafter, binBRafter - binTLafter, olc::BLACK);
			DrawRect(binTLafter, binBRafter - binTLafter, olc::WHITE);

			olc::vi2d binBL(binTLafter.x, binBRafter.y + 10);
			double percentage = 100.0 * ((double)bin.getArea() - (double)bin.getAreaFree()) / (double)bin.getArea();
			std::stringstream percentageStream;
			percentageStream << std::fixed << std::setprecision(2) << percentage;
			std::string binPercentageUsage = "Usage: " + percentageStream.str() + "%";
			DrawString(binBL, binPercentageUsage, olc::YELLOW, 1 * (vScale.x + 0.5));

			olc::vi2d itemTLafter;
			olc::vi2d itemBRafter;
			for(const auto& item : bin.getItemsInBin())
			{
				WorldToScreen(item.getLeftUpperCorner(), itemTLafter);
				WorldToScreen(item.getRightBottomCorner(), itemBRafter);
				FillRect(itemTLafter, itemBRafter - itemTLafter, item.getColor());
			}
		}
		for(int c = 0; c < items_buffer.size(); c++)
		{
			olc::vi2d itemTLafter;
			olc::vi2d itemBRafter;
			WorldToScreen(items_buffer[c].getLeftUpperCorner(), itemTLafter);
			WorldToScreen(items_buffer[c].getRightBottomCorner(), itemBRafter);
			FillRect(itemTLafter, itemBRafter - itemTLafter, items_buffer[c].getColor());
		} 
			


		if(vRectDraw)
		{
			olc::vi2d newItemTLafter;
			olc::vi2d newItemBRafter;

			WorldToScreen(vRectStart, newItemTLafter);
			WorldToScreen(vRectNow, newItemBRafter);

			DrawRect(newItemTLafter, newItemBRafter - newItemTLafter, olc::RED);
		}

		return true;
	}

	void OnHandleZoom(float fElapsedTime)
	{
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

		if (GetKey(olc::Key::E).bHeld) 
			vScale *= 1 + (1.1 * fElapsedTime);
		if (GetKey(olc::Key::Q).bHeld) 
			vScale *= 1 - (0.9 * fElapsedTime);
		
		olc::vd2d vMouseAfterZoom;
		ScreenToWorld(vMouse, vMouseAfterZoom);
		vOffset += (vMouseBeforeZoom - vMouseAfterZoom);
	}

	bool OnGui()
	{
		return true;
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		int big_items_to_pack = 10;
		int medium_items_to_pack = 20;
		int small_items_to_pack = 15;
		srand(time(NULL));
		
		// Measure the time to pack
		Timer T = Timer();
		for (int c = 0; c < big_items_to_pack; c++){
			// addItemToBuffer(Item((rand() % (bin_height)) + 1, (rand() % (bin_width))+ 1));
			std::cout << "Inserted: " << nextFit(Item((rand() % (bin_height)) + 1, (rand() % (bin_width))+ 1)) << "\n";
		}
		for (int c = 0; c < medium_items_to_pack; c++){
			// addItemToBuffer(Item((rand() % (bin_height / 2)) + 1, (rand() % (bin_width / 2))+ 1));
			std::cout << "Inserted: " << nextFit(Item((rand() % (bin_height / 2)) + 1, (rand() % (bin_width / 2))+ 1)) << "\n";
		}
		for (int c = 0; c < small_items_to_pack; c++){
			// addItemToBuffer(Item((rand() % (bin_height / 4)) + 1, (rand() % (bin_width / 4))+ 1));
			std::cout << "Inserted: " << nextFit(Item((rand() % (bin_height / 4)) + 1, (rand() % (bin_width / 4))+ 1)) << "\n";
		}

		std::cout << "Time Taken: " << T.now() << "s\n";

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Draw(fElapsedTime);
		OnHandleZoom(fElapsedTime);
		OnGui();

		olc::vd2d vMouseScreen = {(double)GetMouseX(), (double)GetMouseY()};
		olc::vd2d vMouseWorld;
        ScreenToWorld(vMouseScreen, vMouseWorld);

		if(GetMouse(1).bPressed)
        {
			vRectDraw = true;
            vRectStart = vMouseWorld;
			vRectNow = vRectStart;
        }

        if(GetMouse(1).bHeld)
        {
            vRectNow = vMouseWorld;
        }
		
		
		if (GetKey(olc::Key::A).bHeld)	vOffset -= (olc::vd2d(2.0, 0.0) / vScale) * fElapsedTime * 100;
		if (GetKey(olc::Key::D).bHeld)	vOffset += (olc::vd2d(2.0, 0.0) / vScale) * fElapsedTime * 100;
		if (GetKey(olc::Key::W).bHeld) 	vOffset -= (olc::vd2d(0.0, 2.0) / vScale) * fElapsedTime * 100;
		if (GetKey(olc::Key::S).bHeld)	vOffset += (olc::vd2d(0.0, 2.0) / vScale) * fElapsedTime * 100;

		// Add items to the buffer
		if (GetKey(olc::Key::H).bPressed) {
			addItemToBuffer(Item((rand() % (bin_height / 1)) + 1, (rand() % (bin_width / 1))+ 1));
		}
		if (GetKey(olc::Key::J).bPressed) {
			addItemToBuffer(Item((rand() % (bin_height / 2)) + 1, (rand() % (bin_width / 2))+ 1));
		}
		if (GetKey(olc::Key::K).bPressed) {
			addItemToBuffer(Item((rand() % (bin_height / 4)) + 1, (rand() % (bin_width / 4))+ 1));
		}
		if (GetKey(olc::Key::L).bPressed) {
			addItemToBuffer(Item((rand() % (bin_height / 8)) + 1, (rand() % (bin_width / 8))+ 1));
		}

		// Add 1d items to the buffer
		if (GetKey(olc::Key::B).bPressed) {
			addItemToBuffer(Item((rand() % (bin_height / 1)) + 1, bin_width ));
		}
		if (GetKey(olc::Key::N).bPressed) {
			addItemToBuffer(Item((rand() % (bin_height / 2)) + 1, bin_width ));
		}
		if (GetKey(olc::Key::M).bPressed) {
			addItemToBuffer(Item((rand() % (bin_height / 4)) + 1, bin_width ));
		}

		// Add all items in the buffer to the bins with first fit strategy
		if (GetKey(olc::Key::I).bPressed) {
			for (const auto& item: items_buffer){
				std::cout << "Inserted: " << firstFit(item) << "\n";
			}
			items_buffer.clear();
		}

		// Add all items in the buffer to the bins with next fit strategy
		if (GetKey(olc::Key::O).bPressed) {
			for (const auto& item: items_buffer){
				std::cout << "Inserted: " << nextFit(item) << "\n";
			}
			items_buffer.clear();
		}

		// Add all items in the buffer to the bins with best fit strategy
		if (GetKey(olc::Key::U).bPressed) {
			for (const auto& item: items_buffer){
				std::cout << "Inserted: " << bestFit(item) << "\n";
			}
			items_buffer.clear();
		}

		// Reset screen position
		if (GetKey(olc::Key::C).bPressed) resetScreenPosition();

		if(GetMouse(1).bReleased)
		{
			vRectDraw = false;
			addItemToBuffer(Item(std::abs(vRectStart.y - vRectNow.y), std::abs(vRectStart.x - vRectNow.x)));
			vRectStart = {0,0};
			vRectNow   = {0,0};
		}

		return true;
	}

	bool addItemToBuffer(Item item){
		if (items_buffer.size() == 0) {
			item.moveItem(olc::vi2d(0, -bin_height));
		}
		else {
			item.moveItem(olc::vi2d(items_buffer[items_buffer.size() - 1].getRightBottomCorner().x + 10, -bin_height));	
		}
		items_buffer.push_back(item);
		return true;
	}

	void resetScreenPosition(){
		vOffset = { 0.0, 0.0 };
		vStartPan = { 0.0, 0.0 };
		vScale = { 1.0, 1.0 };
	}

protected:
	bool vRectDraw = false;
	olc::vd2d vRectStart;
	olc::vd2d vRectNow;

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

	// First fit iterates trhought all bins and tries to add in all of them
	bool firstFit(Item item){
		if (item.getHeight() > bin_height || item.getWidth() > bin_width) return false;
		for (int c = 0; c < bins.size(); c++){
			if (bins[c].insert(item)) {
				return true;
			}
		}

		createNewBin();
		if (bins[(bins.size() - 1)].insert(item) == false) return false;
		return true;
	}

	// Next fit only considers the last bin
	bool nextFit(Item item){
		if (item.getHeight() > bin_height || item.getWidth() > bin_width) return false;
		if (bins.size() != 0) {
			if (bins[bins.size() - 1].insert(item)) {
				return true;
			}
		}

		// Creates a new bin and add to the bins vector
		createNewBin();
		if (bins[(bins.size() - 1)].insert(item) == false) return false;
		return true;
	}

	// Best fit add to the bin that will have the least amount of space unused after the insertion
	bool bestFit(Item item){
		if (item.getHeight() > bin_height || item.getWidth() > bin_width) return false;
		
		// Order the vector of bins by decreasing order of used area
		// Falta ordenar os bins por ordem decrescente de area usada 
		//  std::sort(bins.begin(), bins.end(), compareBinsByAreaUsed);

		return firstFit(item);
	}

	bool createNewBin() {
		// Create new Bin
		Bin new_bin = Bin(bin_height, bin_width);
		
		new_bin.moveItem(olc::vi2d((bins.size() + 1) * (bin_width + 20), (uint32_t) ScreenHeight() / 2 - bin_height / 2));
		
		bins.push_back(new_bin);
		// olc::vu2d new_bin_coord((bins.size() + 1) * (bin_width + 20), (uint32_t) ScreenHeight() / 2 - bin_height / 2);
		// olc::vu2d new_bin_size(bin_width, bin_height);
		// bins.emplace_back(new_bin_coord, new_bin_size);
		// if (!bins.back().insert(item)) return false;
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