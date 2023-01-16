#define OLC_PGE_APPLICATION 
#include "include/olcPixelGameEngine.h"

#define OLC_SOUNDWAVE
#include "include/olcSoundWaveEngine.h"

#include <iostream>
#include <algorithm>

#define M_PI 3.141592653589793238462643

struct Time
{
	int seconds = 0;
	int minutes = 0;
	int hours   = 0;
	friend std::ostream& operator << (std::ostream& os, const Time& tm);
	int GMT_Time = -3;

	int totalTime = 0;

	Time() = default;

	Time(int s)
	{
		totalTime = s;
		seconds = s % 60;
		minutes = (s / 60) % 60;
		hours = ((s / 3600) % 24);
	}

	Time(int s, int m, int h)
	{
		totalTime = h * 3600 + m * 60 + s;
		seconds = s;
		minutes = m;
		hours = h;
	}

	Time(const Time& t)
	{
		totalTime = t.totalTime;
		seconds = t.seconds;
		minutes = t.minutes;
		hours = t.hours;
	}

	inline Time operator - (const Time& other) const
	{
		return Time((int)(totalTime - other.totalTime));
	}

	inline Time operator + (const Time& other) const
	{
		return Time(totalTime + other.totalTime);
	}

	inline void operator = (const Time& t) 
	{
		totalTime = t.totalTime;
		seconds = t.seconds;
		minutes = t.minutes;
		hours = t.hours;
	}

	void SetTime(int s)
	{
		totalTime = s;
		seconds = s % 60;
		minutes = (s / 60) % 60;
		hours = ((s / 3600) % 24);
	}

	std::string ToString(bool GMT = true) const
	{
		std::string second  =  (seconds < 10) ? ("0" + std::to_string(seconds)) : std::to_string(seconds);
		std::string minute =   (minutes < 10) ? ("0" + std::to_string(minutes) + ":") : std::to_string(minutes) + ":";
		
		int nHours = hours;
		if(GMT) nHours += GMT_Time;
		if(nHours < 0) nHours += 24;
		std::string hour   =   (nHours < 10)   ? ("0" + std::to_string(hours)   + ":") : std::to_string(hours)   + ":";
		return hour + minute + second;
	}

	int GetHoursGMT()
	{
		int nHours = hours;
		nHours += GMT_Time;
		if(nHours < 0) nHours += 24;
		return nHours;
	}

};

inline std::ostream& operator << (std::ostream& out, const Time& tm)
{
	std::string second  =  (tm.seconds < 10) ? ("0" + std::to_string(tm.seconds)) : std::to_string(tm.seconds);
	std::string minutes =  (tm.minutes < 10) ? ("0" + std::to_string(tm.minutes) + ":") : std::to_string(tm.minutes) + ":";
	std::string hours   =  (tm.hours < 10)   ? ("0" + std::to_string(tm.hours)   + ":") : std::to_string(tm.hours)   + ":";
	out << hours << minutes << second;
	return out;
}

enum class STATUS
{
	RUNNING,
	PAUSED,
	RESET,
};

enum class PROGRESS
{
	FOCUS,
	REST
};

class Clock : public olc::PixelGameEngine
{
public: Clock() { sAppName = "Clock"; }

public:
	uint32_t radius = std::min(ScreenHeight(), ScreenWidth()) / 2;
	olc::Pixel color = olc::Pixel(255, 165, 0);

	std::vector<Time> marks;

	const Time focusTime = Time(6, 0, 0);
	Time focusStart;
	const Time restTime  = Time(4, 0, 0);
	Time restStart;

	Time timeLeft;
	Time timer;

	STATUS status = STATUS::RESET;
	PROGRESS progress = PROGRESS::FOCUS;

	olc::sound::WaveEngine soundEngine;
	olc::sound::Wave soundStartingFocus;
	olc::sound::Wave soundStartingRest;


	bool OnUserCreate() override
	{
		soundEngine.InitialiseAudio();
		if(!soundStartingFocus.LoadAudioWaveform("./res/notification.wav"))
		{
			if(!soundStartingFocus.LoadAudioWaveform("../res/notification.wav"))
				std::cout << "Sample not loaded...\n";
		}
		if(!soundStartingRest.LoadAudioWaveform("./res/trumpets.wav"))
		{
			if(!soundStartingRest.LoadAudioWaveform("../res/trumpets.wav"))
				std::cout << "Sample not loaded...\n";
		}

		return true;
	}

	inline olc::vi2d CenterOfScreen()
	{
		return olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2);
	}

	bool OnDraw(float fElapsedTime)
	{
		timer.SetTime(time(0));

		Clear(olc::Pixel(128, 64, 128, (timer.seconds % 128) + 127));

		FillCircle(CenterOfScreen() - olc::vi2d(ScreenWidth()/4, 0), radius, olc::Pixel(155, 155, 155, 255));
		DrawCircle(CenterOfScreen() - olc::vi2d(ScreenWidth()/4, 0), radius + 0, olc::CYAN);

		DrawLine(CenterOfScreen() - olc::vi2d(ScreenWidth()/4, 0), CenterOfScreen() - olc::vi2d(ScreenWidth() / 4, 0) + olc::vi2d(radius * cos(((timer.seconds * 6.0) - 90) * (M_PI / 180.0)), 
																radius * sin(((timer.seconds * 6.0) - 90) * (M_PI / 180.0))), 
																olc::MAGENTA);
		
		DrawLine(CenterOfScreen() - olc::vi2d(ScreenWidth()/4, 0), CenterOfScreen() - olc::vi2d(ScreenWidth() / 4, 0) + olc::vi2d(radius/2 * cos(((timer.minutes * 6.0) - 90) * (M_PI / 180.0)), 
																radius/2 * sin(((timer.minutes * 6.0) - 90) * (M_PI / 180.0))), 
																olc::RED);
		
		DrawLine(CenterOfScreen() - olc::vi2d(ScreenWidth()/4, 0), CenterOfScreen() - olc::vi2d(ScreenWidth() / 4, 0) + olc::vi2d(radius/4 * cos(((timer.hours * 30.0) - 90) * (M_PI / 180.0)), 
																radius/4 * sin((((timer.GetHoursGMT()) * 30.0) - 90) * (M_PI / 180.0))), 
																olc::BLUE);
		

		DrawString(CenterOfScreen() + olc::vi2d(0, - radius), timer.ToString(), olc::WHITE, 2);

		return true;
	}
	
	void KeyboardInput()
	{
		if(GetKey(olc::SPACE).bReleased)
		{
			if(status == STATUS::RESET)
			{
				focusStart = timer;
				status = STATUS::RUNNING;
			}
			else if(status == STATUS::PAUSED)
			{
				status = STATUS::RUNNING;
			}
			else if(status == STATUS::RUNNING)
			{
				status = STATUS::PAUSED;
			}
		}
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		OnDraw(fElapsedTime);
		KeyboardInput();

		for(int i = 0; i < marks.size() && i < 5; ++i)
		{
			DrawString(CenterOfScreen() + olc::vi2d(0, - radius + 80 + (20 * i)), std::to_string(i + 1) + ". " + marks[i].ToString() + " - " + focusTime.ToString(), olc::CYAN, 2);
		}

		if(status == STATUS::RUNNING)
		{
			Time progressTimer = progress == PROGRESS::FOCUS ? focusTime : restTime;
			Time progressStart = progress == PROGRESS::FOCUS ? focusStart : restStart;

			Time dif = timer - progressStart;
			timeLeft = progressTimer - dif;
			DrawString(CenterOfScreen() + olc::vi2d(0, - radius + 20), progressStart.ToString(), olc::YELLOW, 2);
			DrawString(CenterOfScreen() + olc::vi2d(0, - radius + 40), timeLeft.ToString(),      olc::GREEN , 2);

			if(timeLeft.seconds < 0)
			{
				if(progress == PROGRESS::FOCUS)
				{
					marks.push_back(focusStart);
					soundEngine.PlayWaveform(&soundStartingRest);
					restStart = timer;
					progress = PROGRESS::REST;
				}
				
				else if(progress == PROGRESS::REST)
				{
					soundEngine.PlayWaveform(&soundStartingFocus);
					focusStart = timer;
					progress = PROGRESS::FOCUS;
				}
			}
		}

		else
		{
			DrawString(olc::vi2d(ScreenWidth()/2 - radius + 40, 20), "P A U S E D", olc::Pixel(255, 255, 255, 64) , 2);
		}

		return true;
	}
};

int main()
{
	{
		Clock app;
		if (app.Construct(700, 400, 1, 1)){
			app.Start();
		}
	}
	return 0;
}