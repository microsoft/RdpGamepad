// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "resource.h"
#include "RdpGamepadPlugin_i.h"
#include "RdpGamepadProtocol.h"
#include "TimerManager.h"

using namespace ATL;

class ATL_NO_VTABLE CRdpGamepadChannel :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IWTSVirtualChannelCallback
{
public:
	CRdpGamepadChannel()
	{
	}

	DECLARE_NOT_AGGREGATABLE(CRdpGamepadChannel)

	BEGIN_COM_MAP(CRdpGamepadChannel)
		COM_INTERFACE_ENTRY(IWTSVirtualChannelCallback)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	void SetChannel(IWTSVirtualChannel* pChannel)
	{
		mChannel = pChannel;
	}

	// IWTSVirtualChannelCallback
	virtual HRESULT STDMETHODCALLTYPE OnDataReceived(ULONG cbSize, BYTE* pBuffer) override;
	virtual HRESULT STDMETHODCALLTYPE OnClose() override;

private:
	HRESULT HandleGetState(const RdpGamepad::RdpProtocolPacket& packet);
	HRESULT HandlePollState(const RdpGamepad::RdpProtocolPacket& packet);
	HRESULT HandleSetState(const RdpGamepad::RdpProtocolPacket& packet);
	HRESULT HandleGetCapabilities(const RdpGamepad::RdpProtocolPacket& packet);

	HRESULT SendControllerState(DWORD dwUserIndex);

	typedef HRESULT(CRdpGamepadChannel::*RdpProtocolHandlerFunction)(const RdpGamepad::RdpProtocolPacket& packet);
	static RdpProtocolHandlerFunction sProtocolHandlers[static_cast<int>(RdpGamepad::RdpMessageType::MessageTypeCount)];

	CComPtr<IWTSVirtualChannel> mChannel;
	TimerHandle mTimerPoll;
	TimerHandle mTimerPollTimeout;
};

class ATL_NO_VTABLE CRdpGamepadPlugin :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CRdpGamepadPlugin, &CLSID_RdpGamepadPlugin>,
	public IDispatchImpl<IRdpGamepadPlugin, &IID_IRdpGamepadPlugin, &LIBID_RdpGamepadPluginLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IWTSPlugin,
	public IWTSListenerCallback
{
public:
	CRdpGamepadPlugin()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_RDPGAMEPADPLUGIN)

	DECLARE_NOT_AGGREGATABLE(CRdpGamepadPlugin)

	BEGIN_COM_MAP(CRdpGamepadPlugin)
		COM_INTERFACE_ENTRY(IRdpGamepadPlugin)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IWTSPlugin)
		COM_INTERFACE_ENTRY(IWTSListenerCallback)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	// IWTSPlugin
	virtual HRESULT STDMETHODCALLTYPE Initialize(IWTSVirtualChannelManager* pChannelMgr) override;
	virtual HRESULT STDMETHODCALLTYPE Connected() override;
	virtual HRESULT STDMETHODCALLTYPE Disconnected(DWORD dwDisconnectCode) override;
	virtual HRESULT STDMETHODCALLTYPE Terminated() override;

	// IWTSListenerCallback
	virtual HRESULT STDMETHODCALLTYPE OnNewChannelConnection(IWTSVirtualChannel* pChannel, BSTR data, BOOL* pbAccept, IWTSVirtualChannelCallback** ppCallback) override;

private:
	CComPtr<IWTSListener> mListener;
};

OBJECT_ENTRY_AUTO(__uuidof(RdpGamepadPlugin), CRdpGamepadPlugin)
