// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

bool LoadXInput();
void UnloadXInput();

DWORD ThunkXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState);
DWORD ThunkXInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
DWORD ThunkXInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
