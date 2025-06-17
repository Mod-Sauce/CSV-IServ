[Setup]
AppName=CSV-IServ-Converter
AppVersion=1.1
DefaultDirName={pf}\CSV-IServ-Converter
DefaultGroupName=CSV-IServ-Converter
OutputBaseFilename=CSV-IServ-Converter-Setup
SetupIconFile=dist\CSV-IServ-Converter\icon.ico
Compression=lzma
SolidCompression=yes

[Files]
Source: "dist\CSV-IServ-Converter\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\CSV-IServ-Converter"; Filename: "{app}\CSV-IServ-Converter.exe"
Name: "{commondesktop}\CSV-IServ-Converter"; Filename: "{app}\CSV-IServ-Converter.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop icon"; GroupDescription: "Additional icons:"
