; �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e
; $Id: setup_win.iss 1229 2015-01-30 12:22:01Z umezawa $

#include "../utv_core/version.h"

[Setup]
AppName=Ut Video Codec Suite
AppId=utvideo
AppVersion="{#UTVIDEO_VERSION_MAJOR}.{#UTVIDEO_VERSION_MINOR}.{#UTVIDEO_VERSION_BUGFIX}"
AppCopyright=Copyright (c) 2008-2015 UMEZAWA Takeshi
AppPublisher=UMEZAWA Takeshi
AppPublisherURL=http://umezawa.dyndns.info/
UninstallDisplayName=Ut Video Codec Suite
DefaultDirName={pf}\utvideo
DefaultGroupName=Ut Video Codec Suite
SolidCompression=yes
OutputBaseFilename="utvideo-{#UTVIDEO_VERSION_MAJOR}.{#UTVIDEO_VERSION_MINOR}.{#UTVIDEO_VERSION_BUGFIX}-win"
ArchitecturesInstallIn64BitMode=x64
LicenseFile=..\gplv2.rtf
InfoBeforeFile=before.en.rtf
InfoAfterFile=info.en.rtf

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl";
Name: "ja"; MessagesFile: "compiler:Languages\Japanese.isl"; InfoBeforeFile: "before.ja.rtf"; InfoAfterFile: "info.ja.rtf"

[CustomMessages]
MainFilesComponents=Main Files
ja.MainFilesComponents=���C���t�@�C��
QuickTimeComponents=QuickTime Components
ja.QuickTimeComponents=QuickTime�R���|�[�l���g

[Components]
Name: "main"; Description: "{cm:MainFilesComponents}"; Types: full compact custom; Flags: fixed;
Name: "qt";   Description: "{cm:QuickTimeComponents}"; Types: full;

[Files]
Source: "..\Release\utv_core.dll";     DestDir: "{sys}"; Flags: ignoreversion 32bit;
Source: "..\Release\utv_vcm.dll";      DestDir: "{sys}"; Flags: ignoreversion 32bit;
Source: "..\Release\utv_dmo.dll";      DestDir: "{sys}"; Flags: ignoreversion 32bit regserver;
Source: "..\Release\utv_mft.dll";      DestDir: "{sys}"; Flags: ignoreversion 32bit regserver;                            MinVersion: 0, 6.0
Source: "..\Release\utv_qt.qtx";       DestDir: "{pf32}\QuickTime\QTComponents"; Flags: ignoreversion 32bit; Components: qt;
Source: "..\x64\Release\utv_core.dll"; DestDir: "{sys}"; Flags: ignoreversion 64bit;           Check: Is64BitInstallMode
Source: "..\x64\Release\utv_vcm.dll";  DestDir: "{sys}"; Flags: ignoreversion 64bit;           Check: Is64BitInstallMode
Source: "..\x64\Release\utv_dmo.dll";  DestDir: "{sys}"; Flags: ignoreversion 64bit regserver; Check: Is64BitInstallMode
Source: "..\x64\Release\utv_mft.dll";  DestDir: "{sys}"; Flags: ignoreversion 64bit regserver; Check: Is64BitInstallMode; MinVersion: 0, 6.0
Source: "..\gplv2.rtf";                DestDir: "{app}"; Flags: ignoreversion;
Source: "..\gplv2.txt";                DestDir: "{app}"; Flags: ignoreversion;
Source: "..\gplv2.ja.sjis.txt";        DestDir: "{app}"; Flags: ignoreversion;
Source: "..\readme.en.html";           DestDir: "{app}"; Flags: ignoreversion;
Source: "..\readme.ja.html";           DestDir: "{app}"; Flags: ignoreversion;
Source: "utvideo-x86.reg";             DestDir: "{app}"; Flags: ignoreversion 32bit; DestName: "utvideo.reg"; Check: not Is64BitInstallMode
Source: "utvideo-x64.reg";             DestDir: "{app}"; Flags: ignoreversion 64bit; DestName: "utvideo.reg"; Check:     Is64BitInstallMode

[Registry]
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULRA"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULRG"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULH0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULH2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UQY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULRA"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULRG"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULH0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULH2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UQY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode

[Icons]
Name: "{group}\Global Configuration";             Filename: "{sys}\rundll32.exe"; Parameters: "{sys}\utv_core.dll,GlobalConfigDialog"
Name: "{group}\License Agreement (English, RTF)"; Filename: "{app}\gplv2.rtf"
Name: "{group}\License Agreement (English)";      Filename: "{app}\gplv2.txt"
Name: "{group}\License Agreement (Japanese)";     Filename: "{app}\gplv2.ja.sjis.txt"
Name: "{group}\Readme (English)";                 Filename: "{app}\readme.en.html"
Name: "{group}\Readme (Japanese)";                Filename: "{app}\readme.ja.html"

[Run]
Filename: "{app}\readme.en.html"; Description: "View readme";   Flags: postinstall shellexec skipifsilent; Languages: en
Filename: "{app}\readme.ja.html"; Description: "readme �̕\��"; Flags: postinstall shellexec skipifsilent; Languages: ja
