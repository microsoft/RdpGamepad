// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "RdpGamepadPlugin.h"
#include "RdpGamepadProtocol.h"
#include "TimerManager.h"
#include "DynamicXInput.h"

template <typename T, std::size_t N>
constexpr std::size_t array_size(T(&)[N]) { return N; }

CRdpGamepadChannel::RdpProtocolHandlerFunction CRdpGamepadChannel::sProtocolHandlers[] =
{
	nullptr,										// Hearbeat
	&CRdpGamepadChannel::HandleGetState,			// GetStateRequest
	&CRdpGamepadChannel::HandlePollState,			// PollStateRequest
	&CRdpGamepadChannel::HandleSetState,			// SetStateRequest
	&CRdpGamepadChannel::HandleGetCapabilities,		// GetCapabilitiesRequest
	nullptr,										// GetStateResponse
	nullptr,										// SetStateResponse
	nullptr,										// GetCapabilitiesResponse
};

//////////////////////////////////////////////////////////////////////////
// IWTSPlugin

HRESULT CRdpGamepadPlugin::Initialize(IWTSVirtualChannelManager* pChannelMgr)
{
	TimerManager::Get().Initialize();
	HRESULT hr = pChannelMgr->CreateListener(RDPGAMEPAD_VIRTUAL_CHANNEL_NAME, 0, this, &mListener);
	return hr;
}

HRESULT CRdpGamepadPlugin::Connected()
{
	return S_OK;
}

HRESULT CRdpGamepadPlugin::Disconnected(DWORD dwDisconnectCode)
{
	return S_OK;
}

HRESULT CRdpGamepadPlugin::Terminated()
{
	TimerManager::Get().Terminate();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// IWTSListenerCallback

HRESULT CRdpGamepadPlugin::OnNewChannelConnection(IWTSVirtualChannel* pChannel, BSTR data, BOOL* pbAccept, IWTSVirtualChannelCallback** ppCallback)
{
	CComObject<CRdpGamepadChannel>* endPoint;

	HRESULT hr = CComObject<CRdpGamepadChannel>::CreateInstance(&endPoint);
	if (SUCCEEDED(hr))
	{
		endPoint->AddRef();
		endPoint->SetChannel(pChannel);

		*pbAccept = TRUE;
		*ppCallback = endPoint;
	}
	else
	{
		*pbAccept = FALSE;
		*ppCallback = nullptr;
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////
// IWTSVirtualChannelCallback

HRESULT CRdpGamepadChannel::OnDataReceived(ULONG cbSize, BYTE* pBuffer)
{
	static_assert(array_size(sProtocolHandlers) == RdpGamepad::RdpMessageType::MessageTypeCount, "sProtocolHandlers has incorrect number of elements.");

	if (cbSize < sizeof(RdpGamepad::RdpProtocolHeader))
	{
		return RdpGamepad::RDPGAMEPAD_E_PROTOCOL;
	}

	if (cbSize > sizeof(RdpGamepad::RdpProtocolPacket)) 
	{
		return RdpGamepad::RDPGAMEPAD_E_PROTOCOL;
	}
	
	// We know nothing of the alignment of pBuffer so we need to copy it into our protocol packet instead of just casting it.
	RdpGamepad::RdpProtocolPacket packet;
	std::memcpy(&packet, pBuffer, cbSize);

	if (!packet.IsValid())
	{
		return RdpGamepad::RDPGAMEPAD_E_PROTOCOL;
	}

	if (RdpProtocolHandlerFunction handler = sProtocolHandlers[packet.mHeader.mMessageType])
	{
		return (this->*handler)(packet);
	}
	else
	{
		// Message types that we don't handle are ignored with S_OK
		return S_OK;
	}
}

HRESULT CRdpGamepadChannel::OnClose()
{
	TimerManager::Get().ClearTimer(mTimerPoll);
	TimerManager::Get().ClearTimer(mTimerPollTimeout);
	return S_OK;
}

HRESULT CRdpGamepadChannel::HandleGetState(const RdpGamepad::RdpProtocolPacket& packet)
{
	const auto& request = packet.mGetStateRequest;

	return SendControllerState(request.mUserIndex);
}

HRESULT CRdpGamepadChannel::HandlePollState(const RdpGamepad::RdpProtocolPacket& packet)
{
	const auto& request = packet.mPollStateRequest;

	HRESULT hr = SendControllerState(request.mUserIndex);
	if (SUCCEEDED(hr))
	{
		DWORD dwUserIndex = request.mUserIndex;
		TimerManager::Get().SetTimer(mTimerPoll, [dwUserIndex, this]() { SendControllerState(dwUserIndex); }, std::chrono::seconds(1) / 30, true);
		TimerManager::Get().SetTimer(mTimerPollTimeout, [this]() { TimerManager::Get().ClearTimer(mTimerPoll); }, std::chrono::seconds(2), false);
	}

	return S_OK;
}

HRESULT CRdpGamepadChannel::HandleSetState(const RdpGamepad::RdpProtocolPacket& packet)
{
	const auto& request = packet.mSetStateRequest;

	DWORD result = ThunkXInputSetState(request.mUserIndex, const_cast<XINPUT_VIBRATION*>(&request.mVibration));

	auto response = RdpGamepad::RdpSetStateResponse::MakeResponse(request.mUserIndex, result);
	return mChannel->Write(sizeof(response), reinterpret_cast<BYTE*>(&response), nullptr);
}

HRESULT CRdpGamepadChannel::HandleGetCapabilities(const RdpGamepad::RdpProtocolPacket& packet)
{
	const auto& request = packet.mGetCapabilitiesRequest;

	XINPUT_CAPABILITIES capabilities;
	DWORD result = ThunkXInputGetCapabilities(request.mUserIndex, request.mFlags, &capabilities);

	auto response = RdpGamepad::RdpGetCapabilitiesResponse::MakeResponse(request.mUserIndex, result, capabilities);
	return mChannel->Write(sizeof(response), reinterpret_cast<BYTE*>(&response), nullptr);
}

HRESULT CRdpGamepadChannel::SendControllerState(DWORD dwUserIndex)
{
	XINPUT_STATE state;
	DWORD result = ThunkXInputGetState(dwUserIndex, &state);

	auto response = RdpGamepad::RdpGetStateResponse::MakeResponse(dwUserIndex, result, state);
	return mChannel->Write(sizeof(response), reinterpret_cast<BYTE*>(&response), nullptr);
}
