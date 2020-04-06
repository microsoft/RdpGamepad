# RdpGamepad - a Remote Desktop Plugin for Xbox Gamepads

A Remote Desktop plugin to enable remote use of Xbox gamepads across a Remote Desktop session.

## Download and Install

To use the Remote Desktop Plugin,

* On your _local computer_ you can download the **client** installer from the [latest release](https://github.com/Microsoft/RdpGamepad/releases/latest).  
To install, double-click `RdpGamepadClientInstall-{version}.exe` and follow the instructions presented.

* On _every remote computer_ that you connect to you can:
    1. Download and install the latest `ViGEmBus` drivers from the [ViGEmBus release page](https://github.com/ViGEm/ViGEmBus/releases/latest).  
    To install, double-click `ViGEmBus_Setup_{version}.exe` and follow the instructions presented.
    1. Download the **receiver** installer from the [latest release](https://github.com/Microsoft/RdpGamepad/releases/latest).  
    To install, double-click `RdpGamepadReceiverInstall-{version}.exe` and follow the instructions presented.

## How to use

You don't. The plugin is automatically loaded by Remote Desktop and the receiver connects to the plugin
automatically when you start a remote desktop session. A virtual Xbox 360 controller is attached to
the remote computer when the connection is established.

## About the Source

The `RdpGamepadPlugin` project contains the Remote Desktop plugin that reads the local gamepad
with [XInput](https://docs.microsoft.com/en-us/windows/win32/xinput/getting-started-with-xinput)
and makes the data available over a
[Remote Desktop Virtual Channel](https://docs.microsoft.com/en-us/windows/win32/termserv/terminal-services-virtual-channels).

The `RdpGamepadViGEm` project reads the gamepad data from the Remote Desktop Virtual Channel makes
it available through a virtual Xbox 360 controller using the [ViGEmBus](https://github.com/ViGEm/ViGEmBus)
kernel mode driver. It uses the [ViGEmClient](https://github.com/ViGEm/ViGEmClient) SDK to communicate with the driver.

## Build from Source

To build and install the Remote Desktop Gamepad Plugin yourself, clone the sources (including submodules),
open the solution file in Visual Studio 2019 ([Community Edition](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=16)
is fine), and build the solution for your platform architecture (Win32 or x64).
The output files will be under `bin\Release`.

To register the plugin with Remote Desktop, open and administrative command prompt and navigate to the
`bin\Release` folder. Use `regsvr32` to register the plugin dll for the appropriate architecture. For example:

```bat
regsvr32.exe /i RdpGamepadPlugin64.dll
```

## Contributing

There are many ways to contribute.

* [Submit bugs](https://github.com/Microsoft/RdpGamepad/issues) and help us verify fixes as they are checked in.
* Review [code changes](https://github.com/Microsoft/RdpGamepad/pulls).
* Contribute bug fixes and features.

### Code Contributions

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## License

Copyright (c) Microsoft Corporation. All rights reserved.

Licensed under the [MIT](LICENSE) license.
