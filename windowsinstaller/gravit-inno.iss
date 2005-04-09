; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=Gravit
AppVerName=Gravit 0.3.0
OutputBaseFilename=gravit-0.3.0-win32-installer
AppPublisherURL=http://gravit.slowchop.com
AppSupportURL=http://gravit.slowchop.com
AppUpdatesURL=http://gravit.slowchop.com
DefaultDirName={pf}\gravit
DefaultGroupName=Gravit
AllowNoIcons=yes
LicenseFile=D:\src\gravit\windowsinstaller\files\COPYING
InfoAfterFile=D:\src\gravit\windowsinstaller\files\README.txt
OutputDir=D:\src\gravit\dist
; SetupIconFile=D:\src\gravit\windowsinstaller\files\gravit.ico
Compression=lzma
SolidCompression=yes

[Tasks]
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "D:\src\gravit\windowsinstaller\files\gravit.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\src\gravit\windowsinstaller\files\*.cfg"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\src\gravit\windowsinstaller\files\*.png"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\src\gravit\windowsinstaller\files\*.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\src\gravit\windowsinstaller\files\*.ttf"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\src\gravit\windowsinstaller\files\COPYING"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\src\gravit\windowsinstaller\files\*.DLL"; DestDir: "{sys}"; Flags: sharedfile

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[INI]
Filename: "{app}\gravit.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://gravit.slowchop.com"

[Icons]
Name: "{group}\Gravit"; Filename: "{app}\gravit.exe"; WorkingDir: "{app}";
Name: "{group}\Gravit (Compatible Mode)"; Filename: "{app}\gravit.exe"; Parameters: "-e ""particlerendermode 0"""; WorkingDir: "{app}";
Name: "{group}\Install Gravit Screen Saver"; Filename: "{app}\gravit.exe"; Parameters: "-e installscreensaver -e quit"; WorkingDir: "{app}";
Name: "{group}\Configure\Customise Gravit (gravit.cfg)"; Filename: "{win}\notepad.exe"; Parameters: "{app}\gravit.cfg";
Name: "{group}\Configure\Customise Screen Saver (screensaver.cfg)"; Filename: "{win}\notepad.exe"; Parameters: "{app}\screensaver.cfg";
Name: "{group}\Visit Gravit on the Internet"; Filename: "{app}\gravit.url"
Name: "{group}\{cm:UninstallProgram,Gravit}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\Gravit"; Filename: "{app}\gravit.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\Gravit"; Filename: "{app}\gravit.exe"; Tasks: quicklaunchicon

[Run]
Filename: "{win}\notepad.exe"; Parameters: "{app}\gravit.cfg"; Description: "Modify gravit.cfg (Recommended)"; Flags: nowait postinstall skipifsilent
Filename: "{app}\gravit.exe"; Description: "Install Gravit Screensaver"; Parameters: "-einstallscreensaver -equit"; Flags: postinstall skipifsilent
Filename: "{app}\gravit.exe"; Description: "{cm:LaunchProgram,Gravit}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\gravit.url"

