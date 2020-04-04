; Copyright (c) Microsoft Corporation.
; Licensed under the MIT License.

#define binDir "..\bin"

#define MyAppName "Microsoft Remote Desktop Gamepad Plugin"
#define MyAppPublisher "Microsoft Corporation"
#define MyAppPublisherURL "https://www.microsoft.com"
#define MyAppURL "https://github.com/Microsoft/RdpGamepad"
#define MyAppVersion "1.0.0"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{1C23AC78-6182-4A6B-9E0D-2D3A523C122F}
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
OutputBaseFilename=RdpGamepadClientInstall
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#binDir}\Release\RdpGamepadPlugin64.dll";  DestDir: "{app}"; Check: Is64BitInstallMode; Flags: ignoreversion 64bit uninsrestartdelete
; Download from https://aka.ms/vs/16/release/vc_redist.x86.exe
Source: "{#binDir}\Redist\vc_redist.x64.exe";        DestDir: "{tmp}"; Check: Is64BitInstallMode; Flags: deleteafterinstall
Source: "{#binDir}\Release\RdpGamepadPlugin32.dll";  DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: ignoreversion 32bit solidbreak uninsrestartdelete
; Download from https://aka.ms/vs/16/release/vc_redist.x64.exe
Source: "{#binDir}\Redist\vc_redist.x86.exe";        DestDir: "{tmp}"; Check: not Is64BitInstallMode; Flags: deleteafterinstall

[Registry]
Root: HKCR; Subkey: "RdpGamepadPlugin.RdpGamepadPlugin.1"; ValueType: string; ValueName: ""; ValueData: "RdpGamepadPlugin Class"
Root: HKCR; Subkey: "RdpGamepadPlugin.RdpGamepadPlugin.1\CLSID"; ValueType: string; ValueName: ""; ValueData: "{{F0A2B015-0002-419B-87AB-E0F6A6A3BD00}"
Root: HKCR; Subkey: "RdpGamepadPlugin.RdpGamepadPlugin"; ValueType: string; ValueName: ""; ValueData: "RdpGamepadPlugin Class"
Root: HKCR; Subkey: "RdpGamepadPlugin.RdpGamepadPlugin\CurVer"; ValueType: string; ValueName: ""; ValueData: "RdpGamepadPlugin.RdpGamepadPlugin.1"
Root: HKCR; Subkey: "CLSID\{{F0A2B015-0002-419B-87AB-E0F6A6A3BD00}"; ValueType: string; ValueName: ""; ValueData: "RdpGamepadPlugin Class"
Root: HKCR; Subkey: "CLSID\{{F0A2B015-0002-419B-87AB-E0F6A6A3BD00}\ProgID"; ValueType: string; ValueName: ""; ValueData: "RdpGamepadPlugin.RdpGamepadPlugin.1"
Root: HKCR; Subkey: "CLSID\{{F0A2B015-0002-419B-87AB-E0F6A6A3BD00}\VersionIndependentProgID"; ValueType: string; ValueName: ""; ValueData: "RdpGamepadPlugin.RdpGamepadPlugin"
Root: HKCR; Subkey: "CLSID\{{F0A2B015-0002-419B-87AB-E0F6A6A3BD00}\Programmable";
Root: HKCR; Subkey: "CLSID\{{F0A2B015-0002-419B-87AB-E0F6A6A3BD00}\InprocServer32"; ValueType: string; ValueName: ""; ValueData: "{app}\RdpGamepadPlugin32.dll"; Check: not Is64BitInstallMode
Root: HKCR; Subkey: "CLSID\{{F0A2B015-0002-419B-87AB-E0F6A6A3BD00}\InprocServer32"; ValueType: string; ValueName: ""; ValueData: "{app}\RdpGamepadPlugin64.dll"; Check: Is64BitInstallMode
Root: HKCR; Subkey: "CLSID\{{F0A2B015-0002-419B-87AB-E0F6A6A3BD00}\InprocServer32"; ValueType: string; ValueName: "ThreadingModel"; ValueData: "Free"
Root: HKCR; Subkey: "CLSID\{{F0A2B015-0002-419B-87AB-E0F6A6A3BD00}\Version"; ValueType: string; ValueName: ""; ValueData: "1.0"
Root: HKLM; Subkey: "Software\Microsoft\Terminal Server Client\Default\AddIns\RdpGamepadPlugin"; ValueType: string; ValueName: "Name"; ValueData: "{{F0A2B015-0002-419B-87AB-E0F6A6A3BD00}"

[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing MSVC Redistributable 64 bit..."; Check: Is64BitInstallMode
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing MSVC Redistributable 32 bit..."; Check: not Is64BitInstallMode
