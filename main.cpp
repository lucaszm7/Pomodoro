#define OLC_PGE_APPLICATION 
#include "olcPixelGameEngine.h"

#include <iostream>
#include "utils.hpp"

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

public:

	bool draw(float fElapsedTime)
	{
		// called once per frame
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand()% 255));	
		return true;
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
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
	Bin(unsigned int cap = 10)
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

	friend auto operator<<(std::ostream& out, Bin const& bin) -> std::ostream& 
	{
		for(const auto& i : bin.items)
		{
			out << i << ", ";
		}
		return out;
	}

};

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

int main()
{
	std::vector<Bin> containers_next;
	std::vector<Bin> containers_first;
	std::vector<unsigned int> items = RandList<unsigned int>(1, 10, 20);

	containers_next.emplace_back();
	containers_first.emplace_back();

	first_fit(containers_first, items);
	next_fit(containers_next, items);

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

	return 0;
}