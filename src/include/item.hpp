#pragma once

#include "olcPixelGameEngine.h"
#include "utils.hpp"

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

	uint32_t getHeight() const{
		return heigth;
	}
	uint32_t getWidth() const{
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