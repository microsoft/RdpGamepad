// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define WIN32_LEAN_AND_MEAN

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

#include <TsVirtualChannels.h>
#include <Xinput.h>

#include <array>
#include <chrono>
#include <functional>
#include <map>
#include <type_traits>
#include <vector>
