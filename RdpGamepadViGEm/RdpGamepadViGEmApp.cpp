// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "RdpGamepadProcessor.h"
#include "resource.h"

#include <shellapi.h>

static constexpr UINT RDPGAMEPAD_NOTIFY_MESSAGE = WM_APP + 100;
static constexpr UINT RDPGAMEPAD_CONNECTION_UPDATE_MESSAGE = WM_APP + 101;

class RdpGamepadViGEmApp
{
public:
	RdpGamepadViGEmApp()
		: mRdpProcessor(std::bind(&RdpGamepadViGEmApp::ConnectionStateChanged, this))
	{
		mRdpProcessor.Start();
	}

	~RdpGamepadViGEmApp()
	{
		mRdpProcessor.Stop();
	}

	int Run(HINSTANCE hInstance)
	{
		if (!CreateSingleAppMutex())
		{
			return 0;
		}

		mInstance = hInstance;
		RegisterWindowClass();
		CreateMainWindow();
		InitNotifyIcon();
		ShowNotifyIcon();

		int ret = MessagePump();

		HideNotifyIcon();
		ReleaseSingleAppMutex();

		mWnd = nullptr;
		return ret;
	}

private:
	RdpGamepadProcessor mRdpProcessor;
	NOTIFYICONDATA mNotifyIconData = {0};
	HINSTANCE mInstance = nullptr;
	HWND mWnd = nullptr;
	HANDLE mGlobalMutex = nullptr;

	bool CreateSingleAppMutex()
	{
		static constexpr TCHAR MUTEX_NAME[] = TEXT("RdpGamepadViGEmApp");

		mGlobalMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, MUTEX_NAME);
		if (mGlobalMutex == nullptr)
		{
			mGlobalMutex = CreateMutex(nullptr, FALSE, MUTEX_NAME);
			return true;
		}
		else
		{
			CloseHandle(mGlobalMutex);
			mGlobalMutex = nullptr;
			return false;
		}
	}

	void ReleaseSingleAppMutex()
	{
		ReleaseMutex(mGlobalMutex);
		CloseHandle(mGlobalMutex);
		mGlobalMutex = nullptr;
	}

	void RegisterWindowClass()
	{
		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = StaticWindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = mInstance;
		wcex.hIcon = LoadIcon(mInstance, MAKEINTRESOURCE(IDI_CONTROLLER));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = TEXT("RdpGamepadViGEmApp");
		wcex.hIconSm = LoadIcon(mInstance, MAKEINTRESOURCE(IDI_CONTROLLER));
		RegisterClassExW(&wcex);
	}

	void CreateMainWindow()
	{
		mWnd = CreateWindowW(
			TEXT("RdpGamepadViGEmApp"),
			TEXT("RdpGamepadViGEmApp"),
			0,
			CW_USEDEFAULT,
			0,
			CW_USEDEFAULT,
			0,
			nullptr,
			nullptr,
			mInstance,
			this);

		ShowWindow(mWnd, SW_HIDE);
		UpdateWindow(mWnd);
	}

	void InitNotifyIcon()
	{
		mNotifyIconData.cbSize = sizeof(mNotifyIconData);
		mNotifyIconData.hWnd = mWnd;
		mNotifyIconData.uVersion = NOTIFYICON_VERSION_4;
		mNotifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
		mNotifyIconData.uID = 1;
		mNotifyIconData.uCallbackMessage = RDPGAMEPAD_NOTIFY_MESSAGE;
		mNotifyIconData.hIcon = (HICON)LoadImage(mInstance,
												 MAKEINTRESOURCE(IDI_CONTROLLER),
												 IMAGE_ICON,
												 GetSystemMetrics(SM_CXSMICON),
												 GetSystemMetrics(SM_CYSMICON),
												 LR_DEFAULTCOLOR);

		UpdateToolTip();
	}

	void UpdateToolTip()
	{
		LPCWSTR szTip = mRdpProcessor.IsConnected() ? L"Microsoft Remote Desktop Gamepad Receiver - Connected"
													: L"Microsoft Remote Desktop Gamepad Receiver - Disconnected";
		StringCchCopy(mNotifyIconData.szTip, ARRAYSIZE(mNotifyIconData.szTip), szTip);
	}

	void ShowNotifyIcon()
	{
		Shell_NotifyIcon(NIM_ADD, &mNotifyIconData);
		Shell_NotifyIcon(NIM_SETVERSION, &mNotifyIconData);
	}

	void HideNotifyIcon()
	{
		Shell_NotifyIcon(NIM_DELETE, &mNotifyIconData);
	}

	void UpdateNotifyIcon()
	{
		Shell_NotifyIcon(NIM_MODIFY, &mNotifyIconData);
	}

	void ConnectionStateChanged()
	{
		if (mWnd != nullptr)
		{
			PostMessage(mWnd, RDPGAMEPAD_CONNECTION_UPDATE_MESSAGE, 0, 0);
		}
	}

	void ShowPopupMenu()
	{
		HMENU hMenu = LoadMenu(mInstance, MAKEINTRESOURCE(IDR_POPUPMENU));
		HMENU hPopupMenu = GetSubMenu(hMenu, 0);
		POINT cursor;
		GetCursorPos(&cursor);
		TrackPopupMenuEx(hPopupMenu, TPM_RIGHTALIGN | TPM_RIGHTBUTTON, cursor.x, cursor.y, mWnd, nullptr);
		DestroyMenu(hMenu);
	}

	int MessagePump()
	{
		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return (int)msg.wParam;
	}

	static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		RdpGamepadViGEmApp* pThis;

		if (message == WM_CREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pThis = reinterpret_cast<RdpGamepadViGEmApp*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreate->lpCreateParams));
		}
		else
		{
			pThis = reinterpret_cast<RdpGamepadViGEmApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		if (pThis)
		{
			return pThis->HandleMessage(message, wParam, lParam);
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case RDPGAMEPAD_NOTIFY_MESSAGE:
			switch (LOWORD(lParam))
			{
			case WM_RBUTTONDOWN:
			case WM_CONTEXTMENU:
				ShowPopupMenu();
				break;
			}
			break;

		case RDPGAMEPAD_CONNECTION_UPDATE_MESSAGE:
			UpdateToolTip();
			UpdateNotifyIcon();
			break;

		case WM_DESTROY:
			HideNotifyIcon();
			PostQuitMessage(0);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case ID_MENU_EXIT:
				HideNotifyIcon();
				PostQuitMessage(0);
				break;
			}
			break;

		default:
			return DefWindowProc(mWnd, message, wParam, lParam);
		}
		return 0;
	}
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	RdpGamepadViGEmApp TheApp;
	return TheApp.Run(hInstance);
}
