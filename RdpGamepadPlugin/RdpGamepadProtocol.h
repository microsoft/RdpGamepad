// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifndef NTDDI_XP
#define NTDDI_XP NTDDI_WINXP /* bug in SDK */
#endif

#include <Xinput.h>
#include <pchannel.h>
#include <wtsapi32.h>
#include <type_traits>
#include <cstring>

#pragma comment(lib, "wtsapi32.lib")

namespace RdpGamepad
{
#pragma pack(push)
#pragma pack(1)

	#define RDPGAMEPAD_VIRTUAL_CHANNEL_NAME "Microsoft.RdpGamepad"

	const DWORD INVALID_USER = ~0;
	const HRESULT RDPGAMEPAD_E_PROTOCOL = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x200);
	const HRESULT RDPGAMEPAD_E_NOTIMPL = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x201);
	const HRESULT RDPGAMEPAD_E_TIMEOUT = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x202);

	enum RdpMessageType
	{
		Hearbeat,					// Just a heartbeat message
		GetStateRequest,			// Request the XINPUT_STATE for the controller
		PollStateRequest,			// Request continuous transmission of the XINPUT_STATE for the controller (with a timeout of a few seconds)
		SetStateRequest,			// Request to set a controller's XINPUT_VIBRATION
		GetCapabilitiesRequest,		// Requests the controller's XINPUT_CAPABILITIES
		GetStateResponse,			// Response with the XINPUT_STATE for the controller
		SetStateResponse,			// Response with the result of XInputSetState
		GetCapabilitiesResponse,	// Response with the XINPUT_CAPABILITIES of the controller

		MessageTypeCount
	};

	struct RdpProtocolHeader
	{
		UINT16              mMessageType;
		UINT16              mMessageSize;
		DWORD               mUserIndex;
	};

	struct RdpGetStateRequest : RdpProtocolHeader
	{
		static RdpGetStateRequest MakeRequest(DWORD userIndex)
		{
			RdpGetStateRequest retVal;
			retVal.mMessageType = RdpMessageType::GetStateRequest;
			retVal.mMessageSize = sizeof(retVal);
			retVal.mUserIndex   = userIndex;
			return retVal;
		}
	};

	struct RdpPollStateRequest : RdpProtocolHeader
	{
		static RdpPollStateRequest MakeRequest(DWORD userIndex)
		{
			RdpPollStateRequest retVal;
			retVal.mMessageType = RdpMessageType::PollStateRequest;
			retVal.mMessageSize = sizeof(retVal);
			retVal.mUserIndex   = userIndex;
			return retVal;
		}
	};

	struct RdpGetStateResponse : RdpProtocolHeader
	{
		DWORD               mResult;
		XINPUT_STATE        mState;

		static RdpGetStateResponse MakeResponse(DWORD userIndex, DWORD result, const XINPUT_STATE& state)
		{
			RdpGetStateResponse retVal;
			retVal.mMessageType = RdpMessageType::GetStateResponse;
			retVal.mMessageSize = sizeof(retVal);
			retVal.mUserIndex   = userIndex;
			retVal.mResult      = result;
			retVal.mState       = state;
			return retVal;
		}
	};

	struct RdpSetStateRequest : RdpProtocolHeader
	{
		XINPUT_VIBRATION    mVibration;

		static RdpSetStateRequest MakeRequest(DWORD userIndex, const XINPUT_VIBRATION& vibration)
		{
			RdpSetStateRequest retVal;
			retVal.mMessageType = RdpMessageType::SetStateRequest;
			retVal.mMessageSize = sizeof(retVal);
			retVal.mUserIndex   = userIndex;
			retVal.mVibration   = vibration;
			return retVal;
		}
	};

	struct RdpSetStateResponse : RdpProtocolHeader
	{
		DWORD               mResult;

		static RdpSetStateResponse MakeResponse(DWORD userIndex, DWORD result)
		{
			RdpSetStateResponse retVal;
			retVal.mMessageType = RdpMessageType::SetStateResponse;
			retVal.mMessageSize = sizeof(retVal);
			retVal.mUserIndex   = userIndex;
			retVal.mResult      = result;
			return retVal;
		}
	};

	struct RdpGetCapabilitiesRequest : RdpProtocolHeader
	{
		DWORD               mFlags;

		static RdpGetCapabilitiesRequest MakeRequest(DWORD userIndex, DWORD flags)
		{
			RdpGetCapabilitiesRequest retVal;
			retVal.mMessageType = RdpMessageType::GetCapabilitiesRequest;
			retVal.mMessageSize = sizeof(retVal);
			retVal.mUserIndex   = userIndex;
			retVal.mFlags       = flags;
			return retVal;
		}
	};

	struct RdpGetCapabilitiesResponse : RdpProtocolHeader
	{
		DWORD               mResult;
		XINPUT_CAPABILITIES mCapabilities;

		static RdpGetCapabilitiesResponse MakeResponse(DWORD userIndex, DWORD result, const XINPUT_CAPABILITIES& capabilities)
		{
			RdpGetCapabilitiesResponse retVal;
			retVal.mMessageType  = RdpMessageType::GetCapabilitiesResponse;
			retVal.mMessageSize  = sizeof(retVal);
			retVal.mUserIndex    = userIndex;
			retVal.mResult       = result;
			retVal.mCapabilities = capabilities;
			return retVal;
		}
	};

	const size_t kRdpMessageSizes[] =
	{
		sizeof(RdpProtocolHeader),           // Hearbeat
		sizeof(RdpGetStateRequest),          // GetStateRequest
		sizeof(RdpPollStateRequest),         // PollStateRequest
		sizeof(RdpSetStateRequest),          // SetStateRequest
		sizeof(RdpGetCapabilitiesRequest),   // GetCapabilitiesRequest
		sizeof(RdpGetStateResponse),         // GetStateResponse
		sizeof(RdpSetStateResponse),         // SetStateResponse
		sizeof(RdpGetCapabilitiesResponse),  // GetCapabilitiesResponse
	};
	static_assert(sizeof(kRdpMessageSizes)/sizeof(kRdpMessageSizes[0]) == RdpMessageType::MessageTypeCount, "kRdpMessageSizes has incorrect size");

	union RdpProtocolPacket
	{
		RdpProtocolHeader           mHeader;
		RdpGetStateRequest          mGetStateRequest;
		RdpPollStateRequest         mPollStateRequest;
		RdpSetStateRequest          mSetStateRequest;
		RdpGetCapabilitiesRequest   mGetCapabilitiesRequest;
		RdpGetStateResponse         mGetStateResponse;
		RdpSetStateResponse         mSetStateResponse;
		RdpGetCapabilitiesResponse  mGetCapabilitiesResponse;

		inline bool IsValid()
		{
			return
				(mHeader.mMessageType < RdpMessageType::MessageTypeCount) &&
				(mHeader.mMessageSize == kRdpMessageSizes[mHeader.mMessageType]);
		}
	};

	class RdpGamepadVirtualChannel
	{
	private:
		HANDLE mHandle;

		union PduAndRdpPacket
		{
			char mResponseBytes[CHANNEL_PDU_LENGTH];
			struct
			{
				CHANNEL_PDU_HEADER mPduHeader;
				RdpProtocolPacket  mRdpPacket;
			};

			bool IsValid(ULONG size);
		};
		static_assert(sizeof(PduAndRdpPacket) == CHANNEL_PDU_LENGTH, "RdpChannelPacket has incorrect size");

	public:
		RdpGamepadVirtualChannel()
			: mHandle(nullptr)
		{}

		bool Send(const RdpProtocolHeader& msg) const;
		bool Receive(RdpProtocolPacket* outPacket);
		bool Open();
		void Close();
		bool IsOpen() const;
	};

	inline bool RdpGamepadVirtualChannel::PduAndRdpPacket::IsValid(ULONG size)
	{
		// Check the size of the data matches the channel PDU data size.
		if (size < sizeof(mPduHeader) || size != (sizeof(mPduHeader) + mPduHeader.length))
		{
			return false;
		}

		// Ignore fragmented packets. Should never happen given size of protocol messages.
		if (mPduHeader.flags != CHANNEL_FLAG_ONLY)
		{
			return false;
		}

		// Make sure we get at least one headers worth of data
		if (mPduHeader.length < sizeof(RdpGamepad::RdpProtocolHeader))
		{
			return false;
		}

		return mRdpPacket.IsValid();
	}

	inline bool RdpGamepadVirtualChannel::Send(const RdpProtocolHeader& msg) const
	{
		ULONG bytesWritten = 0;
		if (!WTSVirtualChannelWrite(mHandle, (PCHAR)&msg, msg.mMessageSize, &bytesWritten) ||
			bytesWritten != msg.mMessageSize)
		{
			return false;
		}
		return true;
	}

	inline bool RdpGamepadVirtualChannel::Receive(RdpProtocolPacket* outPacket)
	{
		ULONG bytesRead = 0;
		PduAndRdpPacket channelPacket;
		if (!WTSVirtualChannelRead(mHandle, 0, channelPacket.mResponseBytes, sizeof(channelPacket.mResponseBytes), &bytesRead))
		{
			Close();
			return false;
		}
		else if (bytesRead == 0)
		{
			return false;
		}
		else if (!channelPacket.IsValid(bytesRead))
		{
			Close();
			return false;
		}
		static_assert(std::is_trivially_copyable<RdpProtocolPacket>::value, "RdpProtocolPacket must be trivially copyable for memcpy");
		std::memcpy(outPacket, &channelPacket.mRdpPacket, channelPacket.mRdpPacket.mHeader.mMessageSize);
		return true;
	}

	inline bool RdpGamepadVirtualChannel::Open()
	{
		if (mHandle == nullptr)
		{
			mHandle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, const_cast<LPSTR>(RDPGAMEPAD_VIRTUAL_CHANNEL_NAME), WTS_CHANNEL_OPTION_DYNAMIC);
			if (mHandle == nullptr)
			{
				return false;
			}
		}
		return true;
	}

	inline void RdpGamepadVirtualChannel::Close()
	{
		if (mHandle != nullptr)
		{
			WTSVirtualChannelClose(mHandle);
			mHandle = nullptr;
		}
	}

	inline bool RdpGamepadVirtualChannel::IsOpen() const
	{
		return (mHandle != nullptr);
	}

#pragma pack(pop)
}
