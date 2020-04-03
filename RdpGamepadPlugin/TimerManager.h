// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <chrono>
#include <functional>
#include <vector>
#include <windows.h>

struct TimerHandle
{
	TimerHandle();
	~TimerHandle();

	int mTimerId;
};

class TimerManager
{
public:
	void Initialize()
	{
	}

	void Terminate()
	{
		for (auto& timer : mTimers)
		{
			ClearTimer(timer);
		}
		mTimers = std::vector<Timer>();
	}

	static TimerManager& Get()
	{
		static TimerManager sSingleton;
		return sSingleton;
	}

	bool SetTimer(TimerHandle& timerHandle, std::function<void()> callback, std::chrono::milliseconds duration, bool repeat)
	{
		if (duration.count() > UINT_MAX)
		{
			return false;
		}

		Timer& timer = GetTimer(timerHandle);
		ClearTimer(timer);

		timer.mCallback = std::move(callback);
		timer.mInterval = static_cast<UINT>(duration.count());
		timer.mRepeat = repeat;
		return SetTimer(timer);
	}

	void ClearTimer(TimerHandle& timerHandle)
	{
		ClearTimer(GetTimer(timerHandle));
	}

private:
	struct Timer
	{
		Timer()
			: mCallback()
			, mWindowsId(0)
			, mInterval(0)
			, mRepeat(false)
		{}

		std::function<void()> mCallback;
		UINT_PTR mWindowsId;
		UINT mInterval;
		bool mRepeat;
	};

	static void CALLBACK StaticTimerCallback(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
	{
		Get().TimerCallback(idEvent);
	}

	void TimerCallback(UINT_PTR idEvent)
	{
		if (Timer* timer = GetTimerFromWindowsId(idEvent))
		{
			if (timer->mCallback)
			{
				timer->mCallback();
			}
			if (!timer->mRepeat)
			{
				ClearTimer(*timer);
			}
		}
	}

	Timer& GetTimer(TimerHandle& timerHandle)
	{
		if (timerHandle.mTimerId < 0)
		{
			timerHandle.mTimerId = static_cast<int>(mTimers.size());
			mTimers.push_back(Timer());
		}
		return mTimers[timerHandle.mTimerId];
	}

	Timer* GetTimerFromWindowsId(UINT_PTR windowsTimerId)
	{
		for (auto& timer : mTimers)
		{
			if (timer.mWindowsId == windowsTimerId)
			{
				return &timer;
			}
		}
		return nullptr;
	}

	bool SetTimer(Timer& timer)
	{
		if (timer.mWindowsId != 0)
		{
			::KillTimer(nullptr, timer.mWindowsId);
		}

		timer.mWindowsId = ::SetTimer(nullptr, 0, timer.mInterval, &TimerManager::StaticTimerCallback);
		return (timer.mWindowsId != 0);
	}

	void ClearTimer(Timer& timer)
	{
		if (timer.mWindowsId != 0)
		{
			::KillTimer(nullptr, timer.mWindowsId);
			timer.mWindowsId = 0;
		}

		timer = Timer();
	}

	std::vector<Timer> mTimers;
};

inline TimerHandle::TimerHandle() : mTimerId(-1)
{}

inline TimerHandle::~TimerHandle()
{
	TimerManager::Get().ClearTimer(*this);
}
