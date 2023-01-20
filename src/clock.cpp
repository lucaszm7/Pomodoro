#define OLC_SOUNDWAVE
#include "include/olcSoundWaveEngine.h"

#define OLC_PGE_APPLICATION 
#include "include/olcPixelGameEngine.h"

#include <iostream>
#include <algorithm>
#include <chrono>
#include <ctime>

#include <Windows.h>
#include "resource.h"
#include "include/csv.hpp"

#define M_PI 3.141592653589793238462643

class Resource {
public:
	struct Parameters {
		std::size_t size_bytes = 0;
		void* ptr = nullptr;
	};
private:
	HRSRC hResource = nullptr;
	HGLOBAL hMemory = nullptr;

	Parameters p;

public:

	HMODULE GCM()
	{ // NB: XP+ solution!
		HMODULE hModule = NULL;
		GetModuleHandleEx(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			nullptr,
			&hModule);

		return hModule;
	}

	Resource(int resource_id, int resource_class) {
		hResource = FindResource(GCM(), MAKEINTRESOURCE(resource_id), MAKEINTRESOURCE(resource_class));
		hMemory = LoadResource(GCM(), hResource);

		p.size_bytes = SizeofResource(GCM(), hResource);
		p.ptr = LockResource(hMemory);
	}

	auto& GetResource() const {
		return p;
	}
};

auto& GetFile(int NAME, int CLASS) {
	Resource very_important(NAME, CLASS);
	return very_important.GetResource();
}

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

	inline bool operator > (const Time& other) const
	{
		if(this->hours == other.hours)
		{
			if(this->minutes == other.minutes)
			{
				return this->seconds > other.seconds;
			}
			return this->minutes > other.minutes;
		}
		return this->hours > other.hours;
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

	std::string ToString(bool GMT) const
	{
		std::string second  =  (seconds < 10) ? ("0" + std::to_string(seconds)) : std::to_string(seconds);
		std::string minute =   (minutes < 10) ? ("0" + std::to_string(minutes) + ":") : std::to_string(minutes) + ":";
		
		int nHours = hours;
		if(GMT) nHours += GMT_Time;
		if(nHours < 0) nHours += 24;
		std::string hour   =   (nHours < 10)   ? ("0" + std::to_string(nHours)   + ":") : std::to_string(nHours)   + ":";
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
	std::string second  =  (tm.seconds < 10) ? ("0" + std::to_string(tm.seconds))       : std::to_string(tm.seconds);
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

enum class TYPE
{
	STUDY,
	WORK
};

std::string ToString(PROGRESS pg)
{
	if      (pg == PROGRESS::FOCUS) return "FOCUS";
	else if (pg == PROGRESS::REST)  return "REST!";
	else                            return "";
}

std::string ToString(TYPE pg)
{
	if      (pg == TYPE::WORK)  return "WORK";
	else if (pg == TYPE::STUDY) return "STUDY";
	else                            return "";
}

class Clock : public olc::PixelGameEngine
{
public: Clock() { sAppName = "Clock"; }

public:
	uint32_t radius = std::min(ScreenHeight(), ScreenWidth()) / 2;
	olc::Pixel color = olc::Pixel(255, 165, 0);

	std::vector<Time> marks;

	Time totalTimeStart;
	Time totalTime;

	const Time focusTime = Time(0, 25, 0);
	Time focusStart;
	const Time restTime  = Time(0, 5, 0);
	Time restStart;

	Time timeLeft;
	Time timer;

	STATUS status = STATUS::RESET;
	PROGRESS progress = PROGRESS::FOCUS;
	TYPE type = TYPE::WORK;

	olc::sound::WaveEngine soundEngine;
	olc::sound::Wave soundStartingFocus;
	olc::sound::Wave soundStartingRest;


	bool OnUserCreate() override
	{
		soundEngine.InitialiseAudio();
		auto t = GetFile(IDR_AUDIO1, AUDIO);
		if(t.size_bytes > 0)
			if(!soundStartingFocus.LoadAudioWaveform("", (char*)t.ptr, t.size_bytes))
				std::cout << "Error with sound\n";

		t = GetFile(IDR_AUDIO2, AUDIO);
		if(t.size_bytes > 0)
			if(!soundStartingRest.LoadAudioWaveform("", (char*)t.ptr, t.size_bytes))
				std::cout << "Error with sound\n";

		std::cout << t.size_bytes << '\n';

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
		
		DrawLine(CenterOfScreen() - olc::vi2d(ScreenWidth()/4, 0), CenterOfScreen() - olc::vi2d(ScreenWidth() / 4, 0) + olc::vi2d(radius/4 * cos(((timer.GetHoursGMT() * 30.0) - 90) * (M_PI / 180.0)), 
																																  radius/4 * sin(((timer.GetHoursGMT() * 30.0) - 90) * (M_PI / 180.0))), 
																olc::BLUE);
		

		DrawString(CenterOfScreen() - olc::vi2d(+ScreenWidth()/4 + radius/2, -(radius + 20)), timer.ToString(true), olc::WHITE, 2);

		return true;
	}
	
	void KeyboardInput()
	{
		if(GetKey(olc::SPACE).bReleased)
		{
			if(status == STATUS::RESET)
			{
				focusStart = timer;
				restStart = timer;
				totalTimeStart = timer;
				status = STATUS::RUNNING;
			}
			else if(status == STATUS::PAUSED)
			{
				focusStart = timer;
				restStart = timer;
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
			DrawString(CenterOfScreen() + olc::vi2d(0, - radius + 100 + (20 * i)), std::to_string(i + 1) + ". " + marks[i].ToString(true) + " - " + focusTime.ToString(false), olc::CYAN, 2);
		}

		if(status == STATUS::RUNNING)
		{
			Time progressTimer = progress == PROGRESS::FOCUS ? focusTime : restTime;
			Time progressStart = progress == PROGRESS::FOCUS ? focusStart : restStart;

			DrawString(CenterOfScreen() - olc::vi2d(ScreenWidth()/4 + radius - 40, radius + 40), ToString(progress) + " " + ToString(type), olc::WHITE, 4);

			Time dif = timer - progressStart;
			timeLeft = progressTimer - dif;
			totalTime = timer - totalTimeStart;
			DrawString(CenterOfScreen() + olc::vi2d(0, - radius + 20), timeLeft.ToString(false) + " - " + progressStart.ToString(true), olc::YELLOW , 2);
			DrawString(CenterOfScreen() + olc::vi2d(0, - radius + 60), totalTime.ToString(false), olc::GREEN, 2);

			if(timeLeft.seconds < 0)
			{
				if(progress == PROGRESS::FOCUS)
				{
					marks.push_back(focusStart);
					soundEngine.PlayWaveform(&soundStartingRest);
					restStart = timer;
					progress = PROGRESS::REST;
					status = STATUS::PAUSED;
				}
				
				else if(progress == PROGRESS::REST)
				{
					soundEngine.PlayWaveform(&soundStartingFocus);
					focusStart = timer;
					progress = PROGRESS::FOCUS;
					status = STATUS::PAUSED;
				}
			}
		}

		else
		{
			DrawString( CenterOfScreen() - olc::vi2d(ScreenWidth()/4, 40), "P A U S E D",   olc::Pixel(255, 255, 255, 64) , 4);			
			DrawString( CenterOfScreen() - olc::vi2d(ScreenWidth()/4 + 20, -40),"PRESS 'SPACE'", olc::Pixel(255, 255, 255, 64) , 4);			
		}

		if(status == STATUS::RESET)
		{
			if(GetKey(olc::K1).bReleased)
				type = TYPE::WORK;
			if(GetKey(olc::K2).bReleased)
				type = TYPE::STUDY;
			olc::Pixel selectedColor    = olc::DARK_GREEN;
			olc::Pixel notSelectedColor = olc::Pixel(255, 255, 255, 64);
			olc::Pixel workColor  = (type == TYPE::WORK) ? selectedColor : notSelectedColor;
			olc::Pixel studyColor = (type == TYPE::STUDY) ? selectedColor : notSelectedColor;
			DrawString( CenterOfScreen() - olc::vi2d(ScreenWidth()/4 + 80, 160), "1.WORK",  workColor, 4);			
			DrawString( CenterOfScreen() - olc::vi2d(ScreenWidth()/4 - 200, 160), "2.STUDY", studyColor, 4);			
		}

		return true;
	}
};

int main()
{
	{
		std::time_t date_started = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

		Clock app;
		if (app.Construct(700, 400, 1, 1)){
			app.Start();
		}

		if(app.totalTime > Time(59, 24, 0))
		{
			csvfile csv("marks.csv");
			if(!csv.alreadyExist)
				csv << "Total Time" << "Started" << "Finished" << endrow;
			std::time_t date_finished = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::string date_started_name = std::ctime(&date_started);
			date_started_name.pop_back();
			std::string date_finished_name = std::ctime(&date_finished);
			date_finished_name.pop_back();
			csv << app.totalTime;
			csv << date_started_name;
			csv << date_finished_name;
			csv << ToString(app.type);
			csv << endrow;
		}
	}
	return 0;
}