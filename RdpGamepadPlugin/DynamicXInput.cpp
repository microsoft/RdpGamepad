// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicXInput.h"

class CFunctionPtr
{
public:
	explicit CFunctionPtr(FARPROC pFunction)
		: mFunction(pFunction)
	{}

	template<typename FunctionType, typename = std::enable_if<std::is_function<FunctionType>::value>::type>
	operator FunctionType*() const
	{
		return reinterpret_cast<FunctionType*>(mFunction);
	}

private:
	FARPROC mFunction;
};

class CDllHelper
{
public:
	CDllHelper() = default;
	~CDllHelper()
	{
		Unload();
	}

	bool Load(LPCWSTR pModuleName)
	{
		Unload();
		mHandle = LoadLibraryW(pModuleName);
		return mHandle != nullptr;
	}

	void Unload()
	{
		if (mHandle != nullptr)
		{
			FreeLibrary(mHandle);
			mHandle = nullptr;
		}
	}

	CFunctionPtr GetProc(LPCSTR pFunctionName)
	{
		return CFunctionPtr(GetProcAddress(mHandle, pFunctionName));
	}

	bool IsLoaded() const
	{
		return mHandle != nullptr;
	}

private:
	HMODULE mHandle = nullptr;
};

class CDynamicXInput
{
public:
	CDynamicXInput() = default;
	~CDynamicXInput()
	{
		Unload();
	}

	bool Load()
	{
		if (!mXInputDll.IsLoaded())
		{
			static LPCWSTR const dllNamesToTry[] = {L"xinput1_4.dll", L"xinput9_1_0.dll"};
			for (LPCWSTR dll : dllNamesToTry)
			{
				if (mXInputDll.Load(dll))
				{
					mXInputGetState = mXInputDll.GetProc("XInputGetState");
					mXInputSetState = mXInputDll.GetProc("XInputSetState");
					mXInputGetCapabilities = mXInputDll.GetProc("XInputGetCapabilities");

					if (mXInputGetState && mXInputSetState && mXInputGetCapabilities)
					{
						break;
					}

					mXInputDll.Unload();
				}
			}
		}

		if (!mXInputDll.IsLoaded())
		{
			SetLastError(ERROR_DELAY_LOAD_FAILED);
			return FALSE;
		}

		return true;
	}

	void Unload()
	{
		mXInputGetState = nullptr;
		mXInputSetState = nullptr;
		mXInputGetCapabilities = nullptr;
		mXInputDll.Unload();
	}

	DWORD GetState(DWORD dwUserIndex, XINPUT_STATE* pState)
	{
		if (mXInputGetState)
		{
			return mXInputGetState(dwUserIndex, pState);
		}

		return ERROR_DELAY_LOAD_FAILED;
	}

	DWORD SetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
	{
		if (mXInputSetState)
		{
			return mXInputSetState(dwUserIndex, pVibration);
		}

		return ERROR_DELAY_LOAD_FAILED;
	}

	DWORD GetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
	{
		if (mXInputGetCapabilities)
		{
			return mXInputGetCapabilities(dwUserIndex, dwFlags, pCapabilities);
		}

		return ERROR_DELAY_LOAD_FAILED;
	}

private:
	CDllHelper mXInputDll;
	decltype(XInputGetState)* mXInputGetState = nullptr;
	decltype(XInputSetState)* mXInputSetState = nullptr;
	decltype(XInputGetCapabilities)* mXInputGetCapabilities = nullptr;
};

static CDynamicXInput DynamicXInput;

bool LoadXInput()
{
	return DynamicXInput.Load();
}

void UnloadXInput()
{
	DynamicXInput.Unload();
}

DWORD ThunkXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	return DynamicXInput.GetState(dwUserIndex, pState);
}

DWORD ThunkXInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
	return DynamicXInput.SetState(dwUserIndex, pVibration);
}

DWORD ThunkXInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
{
	return DynamicXInput.GetCapabilities(dwUserIndex, dwFlags, pCapabilities);
}
