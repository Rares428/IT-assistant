#ifndef IT_ASSISTANT_COMMANDS_H
#define IT_ASSISTANT_COMMANDS_H

// Command database for the IT Assistant.
// Edit / add commands here; main.cpp includes this file.

#include <string>
#include <vector>

using std::string;
using std::vector;

// ============================================================================
//  Keyword -> PowerShell command table (replaces the LLM)
//
//  Each entry lists "tags". A tag can be one word ("notepad") or several words
//  ("task manager"). The message is reduced to its main words (see
//  extractMainWords); a tag matches only if ALL of its words are present, and a
//  multi-word tag scores as many points as it has words. The entry with the
//  highest total score wins, so longer/more specific phrases beat generic ones:
//
//    "turn brightness to maximum" -> {brightness, maximum} -> brightness-max (2)
//    "restart explorer"           -> {restart, explorer}   -> "restart explorer" (2)
//                                                              beats reboot's "restart" (1)
// ============================================================================

struct CommandEntry
{
    vector<string> keywords;  // one- or multi-word tags
    vector<string> steps;     // human readable explanation
    string command;           // the PowerShell / cmd / netsh command to run
};

inline const vector<CommandEntry>& getCommandTable()
{
    static const vector<CommandEntry> table = {
        // ---------------- Network ----------------
        {
            {"ip", "ipconfig", "ip address", "adresa ip"},
            {"Show the network configuration (IP, mask, gateway)."},
            "ipconfig"
        },
        {
            {"ipconfig all", "full ip", "ip detailed"},
            {"Show the full network configuration."},
            "ipconfig /all"
        },
        {
            {"flush dns", "clear dns", "goleste dns"},
            {"Clear the DNS resolver cache."},
            "ipconfig /flushdns"
        },
        {
            {"display dns", "dns cache", "show dns"},
            {"Show the contents of the DNS resolver cache."},
            "ipconfig /displaydns"
        },
        {
            {"release ip", "release"},
            {"Release the current IP address (DHCP)."},
            "ipconfig /release"
        },
        {
            {"renew ip", "renew"},
            {"Renew the IP address (DHCP)."},
            "ipconfig /renew"
        },
        {
            {"public ip", "external ip", "my ip online"},
            {"Show the public (external) IP address."},
            "Invoke-RestMethod -Uri 'https://ifconfig.me/ip'"
        },
        {
            {"disable wifi", "off wifi", "dezactiveaza wifi", "opreste wifi", "inchide wifi"},
            {"Disable the Wi-Fi network adapter."},
            "netsh interface set interface name=\"Wi-Fi\" admin=disabled"
        },
        {
            {"enable wifi", "on wifi", "activeaza wifi", "porneste wifi"},
            {"Enable the Wi-Fi network adapter."},
            "netsh interface set interface name=\"Wi-Fi\" admin=enabled"
        },
        {
            {"available wifi", "scan wifi", "wifi networks", "retele wifi"},
            {"List the available Wi-Fi networks."},
            "netsh wlan show networks"
        },
        {
            {"wifi password", "wifi profiles", "parola wifi", "profiluri wifi"},
            {"Show the saved Wi-Fi profiles."},
            "netsh wlan show profiles"
        },
        {
            {"wifi signal", "wifi interface", "signal", "semnal"},
            {"Show the current Wi-Fi connection details and signal."},
            "netsh wlan show interfaces"
        },
        {
            {"network adapters", "adapters", "adaptoare", "netadapter"},
            {"List all network adapters and their status."},
            "Get-NetAdapter"
        },
        {
            {"mac address", "getmac", "physical address", "adresa mac"},
            {"Show the physical (MAC) addresses of the adapters."},
            "getmac"
        },
        {
            {"ping", "test internet", "internet connection", "conexiune"},
            {"Ping google.com to test the internet connection."},
            "ping google.com"
        },
        {
            {"tracert", "traceroute", "trace route"},
            {"Trace the route to google.com."},
            "tracert google.com"
        },
        {
            {"nslookup", "dns lookup", "resolve"},
            {"Resolve a DNS name (google.com)."},
            "nslookup google.com"
        },
        {
            {"netstat", "connections", "ports", "porturi"},
            {"Show active network connections and listening ports."},
            "netstat -ano"
        },
        {
            {"arp", "arp table"},
            {"Show the ARP table (IP to MAC mappings)."},
            "arp -a"
        },
        {
            {"winsock reset", "reset winsock"},
            {"Reset the Winsock catalog (fixes network issues)."},
            "netsh winsock reset"
        },
        {
            {"firewall status", "firewall"},
            {"Show the firewall status for each profile."},
            "Get-NetFirewallProfile | Select-Object Name, Enabled"
        },
        {
            {"disable firewall", "firewall off"},
            {"Turn the firewall OFF for all profiles."},
            "netsh advfirewall set allprofiles state off"
        },
        {
            {"enable firewall", "firewall on"},
            {"Turn the firewall ON for all profiles."},
            "netsh advfirewall set allprofiles state on"
        },

        // ---------------- System info ----------------
        {
            {"systeminfo", "system info", "specs", "specificatii"},
            {"Show detailed system information."},
            "systeminfo"
        },
        {
            {"hostname", "computer name", "numecalculator"},
            {"Show the computer name."},
            "hostname"
        },
        {
            {"os", "windows version", "operating system", "versiune windows"},
            {"Show the operating system version."},
            "Get-CimInstance Win32_OperatingSystem | Select-Object Caption, Version, BuildNumber"
        },
        {
            {"cpu", "processor", "procesor"},
            {"Show CPU information."},
            "Get-CimInstance Win32_Processor | Select-Object Name, NumberOfCores, NumberOfLogicalProcessors"
        },
        {
            {"ram", "memory", "memorie"},
            {"Show the total installed RAM in GB."},
            "[math]::Round((Get-CimInstance Win32_ComputerSystem).TotalPhysicalMemory/1GB,2)"
        },
        {
            {"gpu", "graphics", "video card", "placa video"},
            {"Show the graphics card information."},
            "Get-CimInstance Win32_VideoController | Select-Object Name, DriverVersion"
        },
        {
            {"resolution", "screen resolution", "rezolutie"},
            {"Show the current screen resolution."},
            "Get-CimInstance Win32_VideoController | Select-Object CurrentHorizontalResolution, CurrentVerticalResolution"
        },
        {
            {"motherboard", "baseboard", "placa baza"},
            {"Show the motherboard information."},
            "Get-CimInstance Win32_BaseBoard | Select-Object Manufacturer, Product"
        },
        {
            {"bios"},
            {"Show the BIOS information."},
            "Get-CimInstance Win32_BIOS | Select-Object Manufacturer, SMBIOSBIOSVersion"
        },
        {
            {"serial number", "serial", "serie"},
            {"Show the machine serial number."},
            "(Get-CimInstance Win32_BIOS).SerialNumber"
        },
        {
            {"product key", "windows key", "licenta", "cheie windows"},
            {"Show the Windows product key."},
            "(Get-CimInstance SoftwareLicensingService).OA3xOriginalProductKey"
        },
        {
            {"uptime", "last boot", "ultima pornire"},
            {"Show when the computer was last started."},
            "(Get-CimInstance Win32_OperatingSystem).LastBootUpTime"
        },
        {
            {"tpm"},
            {"Show the TPM (security chip) status."},
            "Get-Tpm"
        },
        {
            {"locale", "culture", "limba"},
            {"Show the current locale/culture."},
            "Get-Culture"
        },

        // ---------------- Power ----------------
        {
            {"shutdown", "power off", "inchide calculatorul", "opreste calculatorul"},
            {"Shut down the computer immediately."},
            "shutdown /s /t 0"
        },
        {
            {"restart", "reboot", "reporneste"},
            {"Restart the computer immediately."},
            "shutdown /r /t 0"
        },
        {
            {"hibernate", "sleep", "hiberneaza"},
            {"Put the computer into hibernation."},
            "shutdown /h"
        },
        {
            {"logoff", "logout", "sign out", "deconecteaza"},
            {"Log off the current user."},
            "shutdown /l"
        },
        {
            {"lock", "blocheaza", "lock workstation"},
            {"Lock the workstation."},
            "rundll32.exe user32.dll,LockWorkStation"
        },
        {
            {"battery", "baterie"},
            {"Show the remaining battery charge in percent."},
            "(Get-WmiObject Win32_Battery).EstimatedChargeRemaining"
        },
        {
            {"battery report", "raport baterie"},
            {"Generate a battery health report (HTML)."},
            "powercfg /batteryreport"
        },
        {
            {"power plan", "plan energie"},
            {"List the available power plans."},
            "powercfg /list"
        },

        // ---------------- Display / brightness ----------------
        {
            {"brightness", "luminozitate", "maximum", "max", "high", "maxima", "100"},
            {"Set the screen brightness to 100%."},
            "(Get-WmiObject -Namespace root/WMI -Class WmiMonitorBrightnessMethods).WmiSetBrightness(1,100)"
        },
        {
            {"brightness", "luminozitate", "minimum", "min", "low", "minima", "scazuta"},
            {"Set the screen brightness to 30%."},
            "(Get-WmiObject -Namespace root/WMI -Class WmiMonitorBrightnessMethods).WmiSetBrightness(1,30)"
        },
        {
            {"brightness", "luminozitate", "medium", "mid", "medie", "50"},
            {"Set the screen brightness to 50%."},
            "(Get-WmiObject -Namespace root/WMI -Class WmiMonitorBrightnessMethods).WmiSetBrightness(1,50)"
        },

        // ---------------- Processes / services ----------------
        {
            {"processes", "procese", "tasklist"},
            {"List the running processes."},
            "tasklist"
        },
        {
            {"services", "servicii"},
            {"List the services that are currently running."},
            "Get-Service | Where-Object {$_.Status -eq 'Running'}"
        },
        {
            {"top cpu", "heaviest cpu", "cpu usage"},
            {"Show the top 10 processes by CPU usage."},
            "Get-Process | Sort-Object CPU -Descending | Select-Object -First 10 Name, CPU"
        },
        {
            {"top memory", "top ram", "memory usage"},
            {"Show the top 10 processes by memory usage."},
            "Get-Process | Sort-Object WS -Descending | Select-Object -First 10 Name, @{Name='MB';Expression={[math]::Round($_.WS/1MB,1)}}"
        },
        {
            {"startup", "startup programs", "autostart", "pornire automata"},
            {"List the programs that start automatically."},
            "Get-CimInstance Win32_StartupCommand | Select-Object Name, Command"
        },
        {
            {"scheduled tasks", "taskuri planificate"},
            {"List the ready scheduled tasks."},
            "Get-ScheduledTask | Where-Object {$_.State -eq 'Ready'} | Select-Object TaskName"
        },
        {
            {"restart explorer", "explorer restart", "refresh taskbar"},
            {"Restart Windows Explorer (refreshes the taskbar)."},
            "Stop-Process -Name explorer -Force"
        },
        {
            {"restart spooler", "print spooler", "spooler"},
            {"Restart the Print Spooler service."},
            "Restart-Service spooler"
        },

        // ---------------- Disk / maintenance ----------------
        {
            {"disk space", "free space", "spatiu disc", "drive space"},
            {"Show the free space on each disk drive."},
            "Get-PSDrive -PSProvider FileSystem"
        },
        {
            {"volumes", "volume", "partitions", "partitii"},
            {"List the disk volumes."},
            "Get-Volume"
        },
        {
            {"disk health", "smart", "sanatate disc"},
            {"Show the physical disk health status."},
            "Get-PhysicalDisk | Select-Object FriendlyName, HealthStatus, MediaType"
        },
        {
            {"chkdsk", "check disk"},
            {"Check the disk for errors (read-only)."},
            "chkdsk"
        },
        {
            {"sfc", "scannow", "system files", "fisiere sistem"},
            {"Scan and repair protected system files."},
            "sfc /scannow"
        },
        {
            {"dism", "restore health", "repair image"},
            {"Repair the Windows image."},
            "DISM /Online /Cleanup-Image /RestoreHealth"
        },
        {
            {"temp", "temporary files", "fisiere temporare"},
            {"Delete the files from the temporary folder."},
            "Remove-Item $env:TEMP\\* -Recurse -Force -ErrorAction SilentlyContinue"
        },
        {
            {"recycle bin", "empty trash", "cos gunoi", "goleste cosul"},
            {"Empty the Recycle Bin."},
            "Clear-RecycleBin -Force"
        },
        {
            {"disk cleanup", "cleanmgr", "curatare disc"},
            {"Open Disk Cleanup."},
            "Start-Process cleanmgr"
        },
        {
            {"restore point", "system restore", "punct restaurare"},
            {"Create a system restore point."},
            "Checkpoint-Computer -Description \"ITAssistant\" -RestorePointType MODIFY_SETTINGS"
        },

        // ---------------- Devices ----------------
        {
            {"drivers", "driverquery", "drivere"},
            {"List the installed device drivers."},
            "driverquery"
        },
        {
            {"usb", "usb devices", "dispozitive usb"},
            {"List the connected USB devices."},
            "Get-PnpDevice -PresentOnly -Class USB | Select-Object FriendlyName, Status"
        },
        {
            {"bluetooth"},
            {"List the Bluetooth devices."},
            "Get-PnpDevice -Class Bluetooth | Select-Object FriendlyName, Status"
        },
        {
            {"printers", "imprimante", "printer"},
            {"List the installed printers."},
            "Get-Printer"
        },

        // ---------------- Applications ----------------
        {
            {"notepad"},
            {"Open Notepad."},
            "Start-Process notepad"
        },
        {
            {"wordpad"},
            {"Open WordPad."},
            "Start-Process write"
        },
        {
            {"calculator", "calc"},
            {"Open the Calculator."},
            "Start-Process calc"
        },
        {
            {"paint", "mspaint"},
            {"Open Paint."},
            "Start-Process mspaint"
        },
        {
            {"word", "winword"},
            {"Open Microsoft Word."},
            "Start-Process winword"
        },
        {
            {"excel"},
            {"Open Microsoft Excel."},
            "Start-Process excel"
        },
        {
            {"powerpoint", "powerpnt"},
            {"Open Microsoft PowerPoint."},
            "Start-Process powerpnt"
        },
        {
            {"outlook"},
            {"Open Microsoft Outlook."},
            "Start-Process outlook"
        },
        {
            {"onenote"},
            {"Open Microsoft OneNote."},
            "Start-Process onenote"
        },
        {
            {"teams"},
            {"Open Microsoft Teams."},
            "Start-Process ms-teams:"
        },
        {
            {"cmd", "command prompt"},
            {"Open the Command Prompt."},
            "Start-Process cmd"
        },
        {
            {"powershell", "pwsh"},
            {"Open PowerShell."},
            "Start-Process powershell"
        },
        {
            {"vscode", "visual studio code"},
            {"Open Visual Studio Code."},
            "Start-Process code"
        },
        {
            {"task manager", "taskmgr", "manager activitati"},
            {"Open Task Manager."},
            "Start-Process taskmgr"
        },
        {
            {"control panel", "panou control"},
            {"Open the Control Panel."},
            "control"
        },
        {
            {"device manager", "devmgmt", "manager dispozitive"},
            {"Open Device Manager."},
            "Start-Process devmgmt.msc"
        },
        {
            {"registry editor", "regedit", "registru"},
            {"Open the Registry Editor."},
            "Start-Process regedit"
        },
        {
            {"explorer", "file explorer", "fisiere"},
            {"Open File Explorer."},
            "Start-Process explorer"
        },
        {
            {"snipping tool", "screenshot", "captura"},
            {"Open the Snipping Tool."},
            "Start-Process snippingtool"
        },
        {
            {"character map", "charmap", "caractere"},
            {"Open the Character Map."},
            "Start-Process charmap"
        },
        {
            {"magnifier", "magnify", "lupa"},
            {"Open the Magnifier."},
            "Start-Process magnify"
        },
        {
            {"on screen keyboard", "osk", "tastatura virtuala"},
            {"Open the On-Screen Keyboard."},
            "Start-Process osk"
        },
        {
            {"narrator", "narator"},
            {"Open Narrator."},
            "Start-Process narrator"
        },
        {
            {"camera", "webcam"},
            {"Open the Camera app."},
            "Start-Process microsoft.windows.camera:"
        },
        {
            {"photos", "poze", "fotografii"},
            {"Open the Photos app."},
            "Start-Process ms-photos:"
        },
        {
            {"media player", "wmplayer"},
            {"Open Windows Media Player."},
            "Start-Process wmplayer"
        },
        {
            {"edge", "browser"},
            {"Open Microsoft Edge."},
            "Start-Process msedge"
        },
        {
            {"chrome"},
            {"Open Google Chrome."},
            "Start-Process chrome"
        },
        {
            {"firefox"},
            {"Open Mozilla Firefox."},
            "Start-Process firefox"
        },
        {
            {"vlc"},
            {"Open VLC media player."},
            "Start-Process vlc"
        },
        {
            {"spotify"},
            {"Open Spotify."},
            "Start-Process spotify"
        },
        {
            {"discord"},
            {"Open Discord."},
            "Start-Process discord"
        },
        {
            {"steam"},
            {"Open Steam."},
            "Start-Process steam"
        },
        {
            {"remote desktop", "mstsc", "rdp"},
            {"Open the Remote Desktop client."},
            "Start-Process mstsc"
        },

        // ---------------- Management consoles / control panels ----------------
        {
            {"event viewer", "eventvwr", "evenimente"},
            {"Open the Event Viewer."},
            "Start-Process eventvwr"
        },
        {
            {"services console", "services msc"},
            {"Open the Services console."},
            "Start-Process services.msc"
        },
        {
            {"disk management", "diskmgmt", "partitionare"},
            {"Open Disk Management."},
            "Start-Process diskmgmt.msc"
        },
        {
            {"computer management", "compmgmt"},
            {"Open Computer Management."},
            "Start-Process compmgmt.msc"
        },
        {
            {"group policy", "gpedit", "politici grup"},
            {"Open the Group Policy Editor."},
            "Start-Process gpedit.msc"
        },
        {
            {"task scheduler", "taskschd", "planificator"},
            {"Open the Task Scheduler."},
            "Start-Process taskschd.msc"
        },
        {
            {"resource monitor", "resmon", "monitor resurse"},
            {"Open the Resource Monitor."},
            "Start-Process resmon"
        },
        {
            {"performance monitor", "perfmon"},
            {"Open the Performance Monitor."},
            "Start-Process perfmon"
        },
        {
            {"system properties", "sysdm"},
            {"Open System Properties."},
            "Start-Process sysdm.cpl"
        },
        {
            {"uninstall", "programs features", "appwiz"},
            {"Open Programs and Features (uninstall a program)."},
            "Start-Process appwiz.cpl"
        },
        {
            {"network connections", "ncpa"},
            {"Open Network Connections."},
            "Start-Process ncpa.cpl"
        },
        {
            {"sound settings", "sound", "sunet"},
            {"Open the Sound settings."},
            "Start-Process mmsys.cpl"
        },
        {
            {"power options", "powercfg cpl"},
            {"Open the Power Options."},
            "Start-Process powercfg.cpl"
        },
        {
            {"system information", "msinfo", "msinfo32"},
            {"Open the System Information tool."},
            "msinfo32"
        },

        // ---------------- Users / security ----------------
        {
            {"users", "user accounts", "utilizatori", "conturi"},
            {"List the local user accounts."},
            "Get-LocalUser"
        },
        {
            {"whoami", "current user", "utilizator curent"},
            {"Show the current user name."},
            "whoami"
        },
        {
            {"groups", "local groups", "grupuri"},
            {"List the local groups."},
            "Get-LocalGroup"
        },
        {
            {"logged users", "active sessions", "sesiuni"},
            {"Show the users currently logged on."},
            "query user"
        },
        {
            {"updates", "hotfix", "actualizari"},
            {"List the installed Windows updates."},
            "Get-HotFix"
        },
        {
            {"installed programs", "programs", "programe instalate"},
            {"List the installed applications."},
            "Get-CimInstance Win32_Product | Select-Object Name, Version"
        },
        {
            {"gpupdate", "refresh policy", "actualizeaza politici"},
            {"Refresh the Group Policy settings."},
            "gpupdate /force"
        },
        {
            {"shares", "shared folders", "partajari"},
            {"List the shared folders."},
            "Get-SmbShare"
        },

        // ---------------- Misc ----------------
        {
            {"date", "time", "ora", "data"},
            {"Show the current date and time."},
            "Get-Date"
        },
        {
            {"environment variables", "env vars", "variabile mediu"},
            {"List the environment variables."},
            "Get-ChildItem Env:"
        },
        {
            {"clipboard", "clipboardul"},
            {"Show the current clipboard content."},
            "Get-Clipboard"
        },
        {
            {"clear clipboard", "goleste clipboard"},
            {"Clear the clipboard."},
            "Set-Clipboard -Value ' '"
        },
        {
            {"timezone", "fus orar"},
            {"Show the current time zone."},
            "Get-TimeZone"
        },
        {
            {"winver"},
            {"Show the Windows version dialog."},
            "winver"
        },
        {
            {"dxdiag", "directx"},
            {"Open the DirectX diagnostic tool."},
            "dxdiag"
        },
        {
            {"system errors", "event errors", "erori sistem"},
            {"Show the last 10 system errors from the event log."},
            "Get-EventLog -LogName System -EntryType Error -Newest 10"
        },
        {
            {"downloads", "downloads folder", "descarcari"},
            {"Open the Downloads folder."},
            "Start-Process explorer \"$env:USERPROFILE\\Downloads\""
        },
        {
            {"documents", "documents folder", "documente"},
            {"Open the Documents folder."},
            "Start-Process explorer \"$env:USERPROFILE\\Documents\""
        },
        {
            {"desktop folder", "open desktop", "folder birou"},
            {"Open the Desktop folder."},
            "Start-Process explorer \"$env:USERPROFILE\\Desktop\""
        },

        // ================= Windows Settings pages (ms-settings:) =================

        // ----- Accessibility / Ease of Access -----
        {
            {"contrast", "high contrast", "contrast themes", "teme contrast"},
            {"Open the Contrast Themes (High Contrast) settings."},
            "Start-Process ms-settings:easeofaccess-highcontrast"
        },
        {
            {"color filters", "filtre culoare"},
            {"Open the Color Filters settings."},
            "Start-Process ms-settings:easeofaccess-colorfilter"
        },
        {
            {"night light", "lumina nocturna", "blue light"},
            {"Open the Night Light settings."},
            "Start-Process ms-settings:nightlight"
        },
        {
            {"text size", "marime text", "font size"},
            {"Open the Text Size accessibility settings."},
            "Start-Process ms-settings:easeofaccess-display"
        },
        {
            {"mouse pointer", "pointer size", "cursor size"},
            {"Open the Mouse Pointer settings."},
            "Start-Process ms-settings:easeofaccess-mousepointer"
        },
        {
            {"text cursor", "cursor text"},
            {"Open the Text Cursor settings."},
            "Start-Process ms-settings:easeofaccess-cursor"
        },
        {
            {"magnifier settings", "setari lupa"},
            {"Open the Magnifier settings."},
            "Start-Process ms-settings:easeofaccess-magnifier"
        },
        {
            {"narrator settings", "setari narator"},
            {"Open the Narrator settings."},
            "Start-Process ms-settings:easeofaccess-narrator"
        },
        {
            {"visual effects", "efecte vizuale", "animations"},
            {"Open the Visual Effects settings."},
            "Start-Process ms-settings:easeofaccess-visualeffects"
        },
        {
            {"captions", "subtitrari", "closed captions"},
            {"Open the Captions settings."},
            "Start-Process ms-settings:easeofaccess-closedcaptioning"
        },

        // ----- System -----
        {
            {"display settings", "setari afisaj", "screen settings"},
            {"Open the Display settings."},
            "Start-Process ms-settings:display"
        },
        {
            {"notifications", "notificari"},
            {"Open the Notifications settings."},
            "Start-Process ms-settings:notifications"
        },
        {
            {"focus assist", "do not disturb", "nu deranja"},
            {"Open the Focus Assist settings."},
            "Start-Process ms-settings:quiethours"
        },
        {
            {"power settings", "battery settings", "setari energie"},
            {"Open the Power & Battery settings."},
            "Start-Process ms-settings:powersleep"
        },
        {
            {"storage settings", "storage sense", "setari stocare"},
            {"Open the Storage settings."},
            "Start-Process ms-settings:storagesense"
        },
        {
            {"multitasking", "snap windows"},
            {"Open the Multitasking settings."},
            "Start-Process ms-settings:multitasking"
        },
        {
            {"clipboard settings", "clipboard history", "istoric clipboard"},
            {"Open the Clipboard settings."},
            "Start-Process ms-settings:clipboard"
        },
        {
            {"about pc", "about settings", "despre sistem"},
            {"Open the About (system) settings page."},
            "Start-Process ms-settings:about"
        },
        {
            {"projecting", "project screen", "proiectare"},
            {"Open the Projecting to this PC settings."},
            "Start-Process ms-settings:project"
        },
        {
            {"remote desktop settings", "setari desktop la distanta"},
            {"Open the Remote Desktop settings."},
            "Start-Process ms-settings:remotedesktop"
        },

        // ----- Devices -----
        {
            {"bluetooth settings", "setari bluetooth"},
            {"Open the Bluetooth & devices settings."},
            "Start-Process ms-settings:bluetooth"
        },
        {
            {"printer settings", "printers settings", "setari imprimanta"},
            {"Open the Printers & scanners settings."},
            "Start-Process ms-settings:printers"
        },
        {
            {"mouse settings", "touchpad", "setari mouse"},
            {"Open the Mouse settings."},
            "Start-Process ms-settings:mousetouchpad"
        },
        {
            {"typing settings", "typing", "tastare"},
            {"Open the Typing settings."},
            "Start-Process ms-settings:typing"
        },
        {
            {"autoplay", "redare automata"},
            {"Open the AutoPlay settings."},
            "Start-Process ms-settings:autoplay"
        },

        // ----- Network -----
        {
            {"network settings", "network status", "setari retea"},
            {"Open the Network & Internet status settings."},
            "Start-Process ms-settings:network-status"
        },
        {
            {"wifi settings", "setari wifi"},
            {"Open the Wi-Fi settings."},
            "Start-Process ms-settings:network-wifi"
        },
        {
            {"ethernet"},
            {"Open the Ethernet settings."},
            "Start-Process ms-settings:network-ethernet"
        },
        {
            {"vpn"},
            {"Open the VPN settings."},
            "Start-Process ms-settings:network-vpn"
        },
        {
            {"hotspot", "mobile hotspot"},
            {"Open the Mobile Hotspot settings."},
            "Start-Process ms-settings:network-mobilehotspot"
        },
        {
            {"airplane mode", "mod avion"},
            {"Open the Airplane Mode settings."},
            "Start-Process ms-settings:network-airplanemode"
        },
        {
            {"proxy"},
            {"Open the Proxy settings."},
            "Start-Process ms-settings:network-proxy"
        },

        // ----- Personalization -----
        {
            {"personalization", "personalizare"},
            {"Open the Personalization settings."},
            "Start-Process ms-settings:personalization"
        },
        {
            {"wallpaper", "background image", "imagine fundal"},
            {"Open the Background (wallpaper) settings."},
            "Start-Process ms-settings:personalization-background"
        },
        {
            {"accent color", "colors settings", "culori"},
            {"Open the Colors settings."},
            "Start-Process ms-settings:personalization-colors"
        },
        {
            {"themes", "teme"},
            {"Open the Themes settings."},
            "Start-Process ms-settings:themes"
        },
        {
            {"lock screen settings", "setari ecran blocare"},
            {"Open the Lock Screen settings."},
            "Start-Process ms-settings:lockscreen"
        },
        {
            {"start menu", "start settings", "meniu start"},
            {"Open the Start menu settings."},
            "Start-Process ms-settings:personalization-start"
        },
        {
            {"taskbar", "bara activitati"},
            {"Open the Taskbar settings."},
            "Start-Process ms-settings:taskbar"
        },
        {
            {"fonts", "fonturi"},
            {"Open the Fonts settings."},
            "Start-Process ms-settings:fonts"
        },

        // ----- Apps -----
        {
            {"apps settings", "installed apps", "aplicatii instalate"},
            {"Open the Installed apps settings."},
            "Start-Process ms-settings:appsfeatures"
        },
        {
            {"default apps", "aplicatii implicite"},
            {"Open the Default apps settings."},
            "Start-Process ms-settings:defaultapps"
        },
        {
            {"optional features", "caracteristici optionale"},
            {"Open the Optional features settings."},
            "Start-Process ms-settings:optionalfeatures"
        },
        {
            {"startup apps", "aplicatii pornire"},
            {"Open the Startup apps settings."},
            "Start-Process ms-settings:startupapps"
        },

        // ----- Accounts -----
        {
            {"your info", "account info", "info cont"},
            {"Open the Your info account settings."},
            "Start-Process ms-settings:yourinfo"
        },
        {
            {"sign in options", "sign in", "windows hello", "optiuni conectare"},
            {"Open the Sign-in options settings."},
            "Start-Process ms-settings:signinoptions"
        },
        {
            {"email accounts", "conturi email"},
            {"Open the Email & accounts settings."},
            "Start-Process ms-settings:emailandaccounts"
        },

        // ----- Time & language -----
        {
            {"date time settings", "time settings", "setari ora"},
            {"Open the Date & time settings."},
            "Start-Process ms-settings:dateandtime"
        },
        {
            {"language settings", "add language", "limba windows"},
            {"Open the Language settings."},
            "Start-Process ms-settings:regionlanguage"
        },
        {
            {"region settings", "setari regiune"},
            {"Open the Region settings."},
            "Start-Process ms-settings:regionformatting"
        },
        {
            {"speech settings", "setari voce"},
            {"Open the Speech settings."},
            "Start-Process ms-settings:speech"
        },

        // ----- Gaming -----
        {
            {"game bar", "xbox game bar"},
            {"Open the Xbox Game Bar settings."},
            "Start-Process ms-settings:gaming-gamebar"
        },
        {
            {"game mode", "mod joc"},
            {"Open the Game Mode settings."},
            "Start-Process ms-settings:gaming-gamemode"
        },
        {
            {"game captures", "game dvr", "capturi joc"},
            {"Open the Game Captures settings."},
            "Start-Process ms-settings:gaming-gamedvr"
        },

        // ----- Privacy & security / Update -----
        {
            {"windows update", "update windows", "actualizare windows"},
            {"Open the Windows Update settings."},
            "Start-Process ms-settings:windowsupdate"
        },
        {
            {"windows security", "windows defender", "antivirus"},
            {"Open Windows Security."},
            "Start-Process windowsdefender:"
        },
        {
            {"activation", "activare windows"},
            {"Open the Activation settings."},
            "Start-Process ms-settings:activation"
        },
        {
            {"recovery", "recuperare"},
            {"Open the Recovery settings."},
            "Start-Process ms-settings:recovery"
        },
        {
            {"troubleshoot", "depanare"},
            {"Open the Troubleshoot settings."},
            "Start-Process ms-settings:troubleshoot"
        },
        {
            {"backup settings", "windows backup", "copie rezerva"},
            {"Open the Windows Backup settings."},
            "Start-Process ms-settings:backup"
        },
        {
            {"developer settings", "for developers"},
            {"Open the For developers settings."},
            "Start-Process ms-settings:developers"
        },
        {
            {"camera privacy", "camera permissions"},
            {"Open the Camera privacy settings."},
            "Start-Process ms-settings:privacy-webcam"
        },
        {
            {"microphone privacy", "mic permissions"},
            {"Open the Microphone privacy settings."},
            "Start-Process ms-settings:privacy-microphone"
        },
        {
            {"location settings", "location privacy", "setari locatie"},
            {"Open the Location privacy settings."},
            "Start-Process ms-settings:privacy-location"
        },
        {
            {"find my device", "gaseste dispozitivul"},
            {"Open the Find my device settings."},
            "Start-Process ms-settings:findmydevice"
        },

        // Generic Settings home — kept LAST so that any specific "<x> settings"
        // entry above wins on a tie and only a bare "settings" lands here.
        {
            {"settings", "open settings", "windows settings", "setari"},
            {"Open Windows Settings (home)."},
            "Start-Process ms-settings:"
        }
    };

    return table;
}

#endif // IT_ASSISTANT_COMMANDS_H
