#define OLC_PGE_APPLICATION 
#include "include/olcPixelGameEngine.h"
#include <iostream>
#define M_PI 3.141592653589793238462643

class Clock : public olc::PixelGameEngine
{
public:
	Clock()
	{
		sAppName = "Clock";
	}

public:
	int counter;
	uint32_t raio;
	olc::Pixel color = olc::Pixel(255, 165, 0);
	bool draw(float fElapsedTime)
	{
		// called once per frame
		Clear(olc::Pixel(64, 128, 128, (counter % 128) + 127));

		FillCircle(olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2), raio, olc::Pixel(155, 155, 155, 255));
		DrawCircle(olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2), raio + 0, olc::CYAN);
		DrawCircle(olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2), raio + 1, olc::BLUE);
		DrawCircle(olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2), raio + 2, olc::BLACK);

		// Seconds
		uint8_t second = counter % 60;
		DrawLine(
			olc::vi2d(
				ScreenWidth() / 2,
				ScreenHeight() / 2
			),
			olc::vi2d(
				ScreenWidth() / 2 + (raio * sin((((second) * 6.0) ) * M_PI / 180.0)), 
				ScreenHeight() / 2 + (raio * -cos((((second) * 6.0) ) * M_PI / 180.0))
			),
			color
		);

		// Minutes
		uint8_t minute = (counter / 60) % 60;
		DrawLine(
			olc::vi2d(
				ScreenWidth() / 2,
				ScreenHeight() / 2
			),
			olc::vi2d(
				ScreenWidth() / 2 + ((raio * 3/4) * sin((((minute) * 6.0)) * M_PI / 180.0)), 
				ScreenHeight() / 2 + ((raio * 3/4) * -cos((((minute) * 6.0)) * M_PI / 180.0))
			),
			olc::BACK
		);

		// Hours
		#define GMT_OFFSET 3 
		uint8_t hour = ((counter / (60 * 60) - GMT_OFFSET) % 12);
		DrawLine(
			olc::vi2d(
				ScreenWidth() / 2,
				ScreenHeight() / 2
			),
			olc::vi2d(
				ScreenWidth() / 2 + ((raio / 2) * sin(((hour * 30)) * M_PI / 180.0)), 
				ScreenHeight() / 2 + ((raio / 2) * -cos(((hour * 30)) * M_PI / 180.0))
			),
			olc::DARK_MAGENTA
		);

		std::ostringstream stream;
		stream << std::setw(2) << std::setfill('0') << (int) hour << ":" << std::setw(2) << std::setfill('0') << (int) minute << ":" << std::setw(2) << std::setfill('0') << (int) second;
		std::string digital_hour = stream.str();

		DrawString(
			olc::vi2d(
				ScreenWidth() / 2  - raio,
				ScreenHeight() / 2 + raio + 10
			),
			digital_hour
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