# RdpGamepad - a Remote Desktop Plugin for Xbox Gamepads

A Remote Desktop plugin to enable remote usage of gamepads across a Remote Desktop session.

## About

The `RdpGamepadPlugin` project contains the Remote Desktop plugin that reads the local gamepad
with [XInput](https://docs.microsoft.com/en-us/windows/win32/xinput/getting-started-with-xinput)
and makes the data available over a
[Remote Desktop Virtual Channel](https://docs.microsoft.com/en-us/windows/win32/termserv/terminal-services-virtual-channels).

The `RdpGamepadViGEm` project reads the gamepad data from the Remote Desktop Virtual Channel makes
it available through a virtual Xbox 360 controller using the [ViGEmBus](https://github.com/ViGEm/ViGEmBus)
kernel mode driver. It uses the [ViGEmClient](https://github.com/ViGEm/ViGEmClient) SDK to communicate with the driver.

## Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## License

Copyright (c) Microsoft Corporation. All rights reserved.

Licensed under the [MIT](LICENSE) license.
