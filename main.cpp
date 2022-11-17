#define OLC_PGE_APPLICATION 
#include "olcPixelGameEngine.h"

#include <iostream>
#include <algorithm>
#include "utils.hpp"
#include <cmath>

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

public:
	int counter;
	uint raio;

	bool draw(float fElapsedTime)
	{
		// called once per frame
		Clear(olc::DARK_GREEN);
		FillCircle(olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2), raio, olc::GREY);
		DrawCircle(olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2), raio, olc::BLUE);
		DrawLine(
			olc::vi2d(
				ScreenWidth() / 2,
				ScreenHeight() / 2
			),
			olc::vi2d(
				ScreenWidth() / 2 + (raio * cos(counter * M_PI / 60.0)), 
				ScreenHeight() / 2 + (raio * sin(counter * M_PI / 60.0))
			),
			olc::BLACK
		);

		counter = time(0);
		return true;
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		raio = ScreenHeight() / 5;
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		draw(fElapsedTime);
		return true;
	}
};

struct Bin
{
private:
	unsigned int m_capacity;
	std::vector<unsigned int> items;

public:
	Bin(unsigned int cap = 50)
		: m_capacity(cap) {}

	bool insert (unsigned int item)
	{
		if(m_capacity >= item) 
		{
			items.push_back(item);
			m_capacity -= item;
			return true;
		}
		return false;
	}

	int spaceLeft(int item)
	{
		return m_capacity - item;
	}

	friend auto operator<<(std::ostream& out, Bin const& bin) -> std::ostream& 
	{
		for(const auto& i : bin.items)
		{
			out << i << ", ";
		}
		return out;
	}

};

// 2-aproximativo
void next_fit(std::vector<Bin>& bins, const std::vector<unsigned int>& items)
{
	int i = 0;
	int j = 0;
	for(; j < items.size(); ++j)
	{
		if(!bins[i].insert(items[j]))
		{
			bins.emplace_back();
			bins[++i].insert(items[j]);
		} 
	}
}

// 1.7-aproximativo
void first_fit(std::vector<Bin>& bins, const std::vector<unsigned int>& items)
{
	int j = 0;
	for(; j < items.size(); ++j)
	{
		bool found = false;
		for(auto& bin : bins)
		{
			if(bin.insert(items[j])){ found = true; break;}
		}
		if(!found)
		{
			bins.emplace_back();
			bins.back().insert(items[j]);
		}
	}
}

// 1.7-aproximativo
void best_fit(std::vector<Bin>& bins, const std::vector<unsigned int>& items)
{
	for(int j = 0; j < items.size(); ++j)
	{
		bool found = false;
		std::pair<int, int> min_value = {-1, INT_MAX};
		for(int i = 0; i < bins.size(); ++i)
		{
			unsigned int left = bins[i].spaceLeft(items[j]);
			if(left >= 0 && left < min_value.second) { found = true; min_value = {i, left};}
		}
		if(found)
		{
			bins[min_value.first].insert(items[j]);
		}
		else
		{
			bins.emplace_back();
			bins.back().insert(items[j]);
		}
	}
}

int main()
{
	std::vector<Bin> containers_next;
	std::vector<Bin> containers_first;
	std::vector<Bin> containers_best;
	std::vector<unsigned int> items = RandList<unsigned int>(1, 30, 20);

	containers_next.emplace_back();
	containers_first.emplace_back();
	containers_best.emplace_back();

	first_fit(containers_first, items);
	next_fit(containers_next, items);
	best_fit(containers_best, items);

	std::cout << "Next("<< containers_next.size() <<"): \n";
	for(const auto& c : containers_next)
	{
		std::cout << "\tBin: " << c << "\n";
	}
	
	std::cout << "First("<< containers_first.size() <<"): \n";
	for(const auto& c : containers_first)
	{
		std::cout << "\tBin: " << c << "\n";
	}
	
	std::cout << "Best("<< containers_best.size() <<"): \n";
	for(const auto& c : containers_best)
	{
		std::cout << "\tBin: " << c << "\n";
	}

	{
		Example app;
		if (app.Construct(256, 256, 2, 2)){
			app.Start();
		}
	}
	return 0;
}