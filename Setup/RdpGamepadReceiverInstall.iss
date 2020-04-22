; Copyright (c) Microsoft Corporation.
; Licensed under the MIT License.

#define binDir "..\bin"

#define MyAppName "Microsoft Remote Desktop Gamepad Receiver"
#define MyAppPublisher "Microsoft Corporation"
#define MyAppPublisherURL "https://www.microsoft.com"
#define MyAppURL "https://github.com/Microsoft/RdpGamepad"
#define MyAppVersion "1.0.1"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{A0DCF855-D488-4BCA-8CF5-BEF426BAB34B}
AppMutex=RdpGamepadViGEmApp
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppPublisherURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
AppContact={#MyAppURL}
AppCopyright=Copyright © Microsoftt 2019
VersionInfoVersion={#MyAppVersion}
DefaultDirName={autopf}\{#MyAppName}
DisableDirPage=yes
DisableReadyPage=yes
DisableProgramGroupPage=yes
OutputDir={#binDir}
OutputBaseFilename=RdpGamepadReceiverInstall
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#binDir}\Release\RdpGamepadViGEm64.exe";   DestDir: "{app}"; Check: Is64BitInstallMode; Flags: ignoreversion 64bit uninsrestartdelete
; Download from https://aka.ms/vs/16/release/vc_redist.x86.exe
Source: "{#binDir}\Redist\vc_redist.x64.exe";        DestDir: "{tmp}"; Check: Is64BitInstallMode; Flags: deleteafterinstall
Source: "{#binDir}\Release\RdpGamepadViGEm32.exe";   DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: ignoreversion 32bit solidbreak uninsrestartdelete
; Download from https://aka.ms/vs/16/release/vc_redist.x64.exe
Source: "{#binDir}\Redist\vc_redist.x86.exe";        DestDir: "{tmp}"; Check: not Is64BitInstallMode; Flags: deleteafterinstall

[Icons]
Name: "{autoprograms}\Microsoft Remote Desktop Gamepad Receiver"; Filename: "{app}\RdpGamepadViGEm64.exe"; Check: Is64BitInstallMode
Name: "{autoprograms}\Microsoft Remote Desktop Gamepad Receiver"; Filename: "{app}\RdpGamepadViGEm32.exe"; Check: not Is64BitInstallMode
Name: "{autostartup}\Start Microsoft Remote Desktop Gamepad Receiver"; Filename: "{app}\RdpGamepadViGEm64.exe"; Check: Is64BitInstallMode
Name: "{autostartup}\Start Microsoft Remote Desktop Gamepad Receiver"; Filename: "{app}\RdpGamepadViGEm32.exe"; Check: not Is64BitInstallMode

[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing MSVC Redistributable 64 bit..."; Check: Is64BitInstallMode
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing MSVC Redistributable 32 bit..."; Check: not Is64BitInstallMode
Filename: "{app}\RdpGamepadViGEm64.exe"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; Check: Is64BitInstallMode
Filename: "{app}\RdpGamepadViGEm32.exe"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; Check: not Is64BitInstallMode