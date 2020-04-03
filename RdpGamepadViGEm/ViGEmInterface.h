// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ViGEm/Client.h>
#include <Xinput.h>

class ViGEmClient : public std::enable_shared_from_this<ViGEmClient>
{
public:
	ViGEmClient();
	~ViGEmClient();
	std::unique_ptr<class ViGEmTarget360> CreateController();

	const PVIGEM_CLIENT GetHandle() const { return mClient; }

private:
	PVIGEM_CLIENT mClient;
};

class ViGEmTarget360
{
	friend class ViGEmClient;

private:
	ViGEmTarget360(std::shared_ptr<ViGEmClient> Client);

public:
	~ViGEmTarget360();
	void SetGamepadState(const XINPUT_GAMEPAD& Gamepad);
	bool GetVibration(XINPUT_VIBRATION& OutVibration);

private:
	std::shared_ptr<ViGEmClient> mClient;
	PVIGEM_TARGET mTarget;
	XINPUT_VIBRATION mPendingVibration{0};
	bool mHasPendingVibration = false;
	std::mutex mMutex;

	static void CALLBACK StaticControllerNotification(PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, UCHAR LedNumber, LPVOID Context);
};
