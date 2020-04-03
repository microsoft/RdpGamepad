// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ViGEmInterface.h"

#pragma comment(lib, "setupapi.lib")

ViGEmTarget360::ViGEmTarget360(std::shared_ptr<ViGEmClient> Client)
{
	mClient = Client;
	mTarget = vigem_target_x360_alloc();
	vigem_target_add(mClient->GetHandle(), mTarget);
	vigem_target_x360_register_notification(mClient->GetHandle(), mTarget, &StaticControllerNotification, this);
}

ViGEmTarget360::~ViGEmTarget360()
{
	std::unique_lock<std::mutex> lock(mMutex);

	vigem_target_x360_unregister_notification(mTarget);
	vigem_target_remove(mClient->GetHandle(), mTarget);
	vigem_target_free(mTarget);
}

void ViGEmTarget360::SetGamepadState(const XINPUT_GAMEPAD& Gamepad)
{
	std::unique_lock<std::mutex> lock(mMutex);

	XUSB_REPORT report;
	report.wButtons = Gamepad.wButtons;
	report.bLeftTrigger = Gamepad.bLeftTrigger;
	report.bRightTrigger = Gamepad.bRightTrigger;
	report.sThumbLX = Gamepad.sThumbLX;
	report.sThumbLY = Gamepad.sThumbLY;
	report.sThumbRX = Gamepad.sThumbRX;
	report.sThumbRY = Gamepad.sThumbRY;
	vigem_target_x360_update(mClient->GetHandle(), mTarget, report);
}

bool ViGEmTarget360::GetVibration(XINPUT_VIBRATION& OutVibration)
{
	std::unique_lock<std::mutex> lock(mMutex);
	if (mHasPendingVibration)
	{
		OutVibration = mPendingVibration;
		mHasPendingVibration = false;
		return true;
	}
	return false;
}

void ViGEmTarget360::StaticControllerNotification(PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, UCHAR LedNumber, LPVOID Context)
{
	auto pThis = static_cast<ViGEmTarget360*>(Context);

	std::unique_lock<std::mutex> lock(pThis->mMutex);
	pThis->mPendingVibration.wLeftMotorSpeed = LargeMotor << 8;
	pThis->mPendingVibration.wRightMotorSpeed = SmallMotor << 8;
	pThis->mHasPendingVibration = true;
}

ViGEmClient::ViGEmClient()
{
	mClient = vigem_alloc();
	vigem_connect(mClient);
}

ViGEmClient::~ViGEmClient()
{
	vigem_free(mClient);
}

std::unique_ptr<ViGEmTarget360> ViGEmClient::CreateController()
{
	return std::unique_ptr<ViGEmTarget360>{new ViGEmTarget360{shared_from_this()}};
}
