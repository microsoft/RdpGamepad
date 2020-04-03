// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace RdpGamepad
{
	class RdpGamepadVirtualChannel;
}

class ViGEmClient;
class ViGEmTarget360;

class RdpGamepadProcessor
{
public:
	RdpGamepadProcessor();
	~RdpGamepadProcessor();

	void Start();
	void Stop();

private:
	std::unique_ptr<RdpGamepad::RdpGamepadVirtualChannel> mRdpGamepadChannel;
	std::shared_ptr<ViGEmClient> mViGEmClient;
	std::shared_ptr<ViGEmTarget360> mViGEmTarget360;
	std::thread mThread;
	std::mutex mMutex;
	unsigned int mRdpGamepadOpenRetry = 0;
	unsigned int mRdpGamepadPollTicks = 0;
	unsigned int mLastGetStateResponseTicks = 0;
	bool mRdpGamepadConnected = false;
	bool mKeepRunning = false;

	void Run();
	void RdpGamepadTidy();
	void RdpGamepadProcess();
};
