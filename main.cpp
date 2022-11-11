#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

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
	unsigned int m_capacity = 10;
	std::vector<unsigned int> items;

public:
	Bin(unsigned int cap)
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

int main()
{
	std::vector<Bin> containers;
	std::vector<unsigned int> weigth {5,4,3,6,3,1,6,3,9};

	containers.emplace_back(10);
	unsigned int count = 0;
	int i = 0;
	while(i < weigth.size())
	{
		if(!containers[count].insert(weigth[i]))
		{
			containers.emplace_back(10);
			count++;
			continue;
		}
		++i;
	} 

	for(const auto& c : containers)
	{
		std::cout << "Bin: " << c << "\n";
	}

	{
		Example demo;
		if (demo.Construct(256, 240, 4, 4))
			demo.Start();
	}

	return 0;
}