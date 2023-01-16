#define OLC_PGE_APPLICATION 
#include "include/olcPixelGameEngine.h"
#include <iostream>
#include <complex>

#define M_PI 3.141592653589793238462643

struct Time
{
	int seconds = 0;
	int minutes = 0;
	int hours   = 0;
	friend std::ostream& operator << (std::ostream& os, const Time& tm);
	int GMT = -3;

	Time(int s)
	{
		seconds = s % 60;
		minutes = (s / 60) % 60;
		hours = ((s / 3600) % 24) + GMT;
	}
};

inline std::ostream& operator << (std::ostream& out, const Time& tm)
{
	std::string second =  (tm.seconds < 10) ? ("0" + std::to_string(tm.seconds) + ":") : std::to_string(tm.seconds);
	std::string minutes = (tm.minutes < 10) ? ("0" + std::to_string(tm.minutes) + ":") : std::to_string(tm.minutes) + ":";
	std::string hours =   (tm.hours < 10)   ? ("0" + std::to_string(tm.hours)   + ":") : std::to_string(tm.hours)   + ":";
	out << hours << minutes << second;
	return out;
}

struct Date
{
	int day;
	int month;
	int year;
};

class Clock : public olc::PixelGameEngine
{
public:
	Clock()
	{
		sAppName = "Clock";
	}

public:
	uint32_t radius;
	olc::Pixel color = olc::Pixel(255, 165, 0);

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		radius = ScreenHeight() / 5;
		return true;
	}

	inline olc::vi2d CenterOfScreen()
	{
		return olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2);
	}

	bool Draw(float fElapsedTime)
	{
		Time timer(time(0));

		Clear(olc::Pixel(64, 128, 128, (timer.seconds % 128) + 127));

		FillCircle(CenterOfScreen(), radius, olc::Pixel(155, 155, 155, 255));
		DrawCircle(CenterOfScreen(), radius + 0, olc::CYAN);

		DrawLine(CenterOfScreen(), CenterOfScreen() + olc::vi2d(radius * cos(((timer.seconds * 6.0) - 90) * (M_PI / 180.0)), 
																radius * sin(((timer.seconds * 6.0) - 90) * (M_PI / 180.0))), 
																olc::MAGENTA);
		
		DrawLine(CenterOfScreen(), CenterOfScreen() + olc::vi2d(radius/2 * cos(((timer.minutes * 6.0) - 90) * (M_PI / 180.0)), 
																radius/2 * sin(((timer.minutes * 6.0) - 90) * (M_PI / 180.0))), 
																olc::RED);
		
		DrawLine(CenterOfScreen(), CenterOfScreen() + olc::vi2d(radius/4 * cos(((timer.hours * 30.0) - 90) * (M_PI / 180.0)), 
																radius/4 * sin(((timer.hours * 30.0) - 90) * (M_PI / 180.0))), 
																olc::BLUE);
		

		std::ostringstream digitalTime;
		digitalTime << timer;

		DrawString(olc::vi2d(ScreenWidth() / 2  - radius, ScreenHeight() / 2 + radius + 10),digitalTime.str());

		return true;
	}
	
	bool OnUserUpdate(float fElapsedTime) override
	{
		Draw(fElapsedTime);
		return true;
	}
};

int main()
{
	{
		Clock app;
		if (app.Construct(640, 360, 1, 1)){
			app.Start();
		}
	}
	return 0;
}