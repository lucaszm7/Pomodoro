#pragma once

#include "olcPixelGameEngine.h"
#include "utils.hpp"
#include "item.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

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
