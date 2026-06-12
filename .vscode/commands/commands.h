#ifndef IT_ASSISTANT_COMMANDS_H
#define IT_ASSISTANT_COMMANDS_H

// Command database for the IT Assistant.
// Edit / add commands here; main.cpp includes this file.

#include <string>
#include <vector>
#include <sstream>

using std::string;
using std::vector;

// Local int->string helper (std::to_string is unreliable on old MinGW).
inline string numToStr(int n)
{
    std::ostringstream ss;
    ss << n;
    return ss.str();
}

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

inline vector<CommandEntry> getBaseCommands()
{
    vector<CommandEntry> table = {
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

        // ================= Folders (create / open special folders) =================
        {
            {"create folder", "new folder", "folder nou", "make folder"},
            {"Create a 'New Folder' on the Desktop and open it."},
            "$f = \"$env:USERPROFILE\\Desktop\\New Folder\"; New-Item -ItemType Directory -Force -Path $f | Out-Null; Start-Process explorer $f"
        },
        {
            {"pictures folder", "poze folder", "my pictures"},
            {"Open the Pictures folder."},
            "Start-Process explorer shell:My Pictures"
        },
        {
            {"music folder", "muzica folder"},
            {"Open the Music folder."},
            "Start-Process explorer shell:My Music"
        },
        {
            {"videos folder", "video folder", "filme folder"},
            {"Open the Videos folder."},
            "Start-Process explorer shell:My Video"
        },
        {
            {"startup folder", "folder pornire"},
            {"Open the Startup folder (programs that run at logon)."},
            "Start-Process explorer shell:Startup"
        },
        {
            {"recent files", "fisiere recente"},
            {"Open the Recent files folder."},
            "Start-Process explorer shell:Recent"
        },
        {
            {"sendto folder", "send to"},
            {"Open the SendTo folder."},
            "Start-Process explorer shell:SendTo"
        },
        {
            {"fonts folder", "folder fonturi"},
            {"Open the Fonts folder."},
            "Start-Process explorer shell:Fonts"
        },
        {
            {"user folder", "profile folder", "folder utilizator"},
            {"Open your user profile folder."},
            "Start-Process explorer shell:Profile"
        },
        {
            {"appdata folder", "roaming folder", "appdata"},
            {"Open the AppData (Roaming) folder."},
            "Start-Process explorer shell:AppData"
        },
        {
            {"local appdata", "localappdata"},
            {"Open the Local AppData folder."},
            "Start-Process explorer shell:LocalAppData"
        },
        {
            {"temp folder", "folder temporar"},
            {"Open the temporary files folder."},
            "Start-Process explorer $env:TEMP"
        },
        {
            {"program files", "folder program files"},
            {"Open the Program Files folder."},
            "Start-Process explorer 'C:\\Program Files'"
        },
        {
            {"program files x86", "program files 86"},
            {"Open the Program Files (x86) folder."},
            "Start-Process explorer 'C:\\Program Files (x86)'"
        },
        {
            {"windows folder", "folder windows"},
            {"Open the Windows folder."},
            "Start-Process explorer 'C:\\Windows'"
        },
        {
            {"system32", "system folder", "folder system"},
            {"Open the System32 folder."},
            "Start-Process explorer 'C:\\Windows\\System32'"
        },
        {
            {"start menu folder", "programs folder"},
            {"Open the Start Menu programs folder."},
            "Start-Process explorer shell:Programs"
        },
        {
            {"favorites folder", "favorite"},
            {"Open the Favorites folder."},
            "Start-Process explorer shell:Favorites"
        },
        {
            {"public folder", "folder public"},
            {"Open the Public folder."},
            "Start-Process explorer shell:Public"
        },
        {
            {"templates folder", "sabloane"},
            {"Open the Templates folder."},
            "Start-Process explorer shell:Templates"
        },
        {
            {"screenshots folder", "capturi folder"},
            {"Open the Screenshots folder."},
            "Start-Process explorer shell:Screenshots"
        },
        {
            {"camera roll", "camera folder"},
            {"Open the Camera Roll folder."},
            "Start-Process explorer shell:CameraRoll"
        },
        {
            {"saved games", "jocuri salvate"},
            {"Open the Saved Games folder."},
            "Start-Process explorer shell:SavedGames"
        },
        {
            {"links folder"},
            {"Open the Links folder."},
            "Start-Process explorer shell:Links"
        },
        {
            {"contacts folder", "contacte"},
            {"Open the Contacts folder."},
            "Start-Process explorer shell:Contacts"
        },
        {
            {"3d objects", "obiecte 3d"},
            {"Open the 3D Objects folder."},
            "Start-Process explorer shell:3D Objects"
        },
        {
            {"onedrive folder", "onedrive"},
            {"Open the OneDrive folder."},
            "Start-Process explorer shell:OneDrive"
        },
        {
            {"printers folder", "folder imprimante"},
            {"Open the Printers folder."},
            "Start-Process explorer shell:PrintersFolder"
        },
        {
            {"network folder", "retea folder"},
            {"Open the Network folder."},
            "Start-Process explorer shell:NetworkPlacesFolder"
        },

        // ================= More Settings pages =================
        {
            {"sound devices", "audio devices", "output device"},
            {"Open the Sound output/input devices settings."},
            "Start-Process ms-settings:sound-devices"
        },
        {
            {"volume mixer", "app volume", "mixer volum"},
            {"Open the per-app Volume Mixer settings."},
            "Start-Process ms-settings:apps-volume"
        },
        {
            {"battery saver", "economisire baterie"},
            {"Open the Battery Saver settings."},
            "Start-Process ms-settings:batterysaver"
        },
        {
            {"advanced display", "refresh rate", "rata refresh"},
            {"Open the Advanced Display settings (refresh rate)."},
            "Start-Process ms-settings:display-advanced"
        },
        {
            {"graphics settings", "gpu settings"},
            {"Open the Graphics (GPU preference) settings."},
            "Start-Process ms-settings:display-advancedgraphics"
        },
        {
            {"hdr settings", "hdr"},
            {"Open the HDR settings."},
            "Start-Process ms-settings:display-hdr"
        },
        {
            {"nearby sharing", "partajare apropiere"},
            {"Open the Nearby Sharing settings."},
            "Start-Process ms-settings:crossdevice"
        },
        {
            {"pen settings", "windows ink"},
            {"Open the Pen & Windows Ink settings."},
            "Start-Process ms-settings:pen"
        },
        {
            {"touchpad settings", "setari touchpad"},
            {"Open the Touchpad settings."},
            "Start-Process ms-settings:devices-touchpad"
        },
        {
            {"usb settings", "setari usb"},
            {"Open the USB settings."},
            "Start-Process ms-settings:usb"
        },
        {
            {"phone link", "mobile devices", "your phone"},
            {"Open the Phone Link / mobile devices settings."},
            "Start-Process ms-settings:mobile-devices"
        },
        {
            {"data usage", "consum date"},
            {"Open the Data Usage settings."},
            "Start-Process ms-settings:datausage"
        },
        {
            {"offline maps", "harti offline"},
            {"Open the Offline Maps settings."},
            "Start-Process ms-settings:maps"
        },
        {
            {"tablet mode", "mod tableta"},
            {"Open the Tablet settings."},
            "Start-Process ms-settings:tabletmode"
        },
        {
            {"eye control", "control ochi"},
            {"Open the Eye Control accessibility settings."},
            "Start-Process ms-settings:easeofaccess-eyecontrol"
        },
        {
            {"sticky keys", "keyboard accessibility"},
            {"Open the Keyboard accessibility settings."},
            "Start-Process ms-settings:easeofaccess-keyboard"
        },
        {
            {"mouse keys", "mouse accessibility"},
            {"Open the Mouse accessibility settings."},
            "Start-Process ms-settings:easeofaccess-mouse"
        },
        {
            {"mono audio", "audio accessibility"},
            {"Open the Audio accessibility settings."},
            "Start-Process ms-settings:easeofaccess-audio"
        },
        {
            {"speech recognition", "voice access"},
            {"Open the Speech Recognition settings."},
            "Start-Process ms-settings:easeofaccess-speechrecognition"
        },
        {
            {"video playback", "redare video"},
            {"Open the Video Playback settings."},
            "Start-Process ms-settings:videoplayback"
        },
        {
            {"search settings", "windows search"},
            {"Open the Windows Search settings."},
            "Start-Process ms-settings:cortana-windowssearch"
        },
        {
            {"family settings", "family group", "cont familie"},
            {"Open the Family settings."},
            "Start-Process ms-settings:family-group"
        },
        {
            {"other users", "add user", "adauga utilizator"},
            {"Open the Other Users settings."},
            "Start-Process ms-settings:otherusers"
        },
        {
            {"sync settings", "sincronizare"},
            {"Open the Sync your settings page."},
            "Start-Process ms-settings:sync"
        },
        {
            {"device encryption", "bitlocker", "criptare"},
            {"Open the Device Encryption settings."},
            "Start-Process ms-settings:deviceencryption"
        },
        {
            {"delivery optimization", "optimizare livrare"},
            {"Open the Delivery Optimization settings."},
            "Start-Process ms-settings:delivery-optimization"
        },
        {
            {"windows insider", "insider program"},
            {"Open the Windows Insider settings."},
            "Start-Process ms-settings:windowsinsider"
        },

        // ================= More tools =================
        {
            {"msconfig", "system configuration", "configurare sistem"},
            {"Open System Configuration."},
            "Start-Process msconfig"
        },
        {
            {"steps recorder", "psr", "inregistrare pasi"},
            {"Open the Steps Recorder."},
            "Start-Process psr"
        },
        {
            {"credential manager", "parole salvate", "saved passwords"},
            {"Open the Credential Manager."},
            "control /name Microsoft.CredentialManager"
        },
        {
            {"god mode", "all tasks", "all settings list"},
            {"Open the 'God Mode' master settings list."},
            "Start-Process explorer 'shell:::{ED7BA470-8E54-465E-825C-99712043E01C}'"
        },
        {
            {"quick assist", "asistenta rapida"},
            {"Open Quick Assist (remote help)."},
            "Start-Process quickassist"
        },
        {
            {"windows features", "turn windows features", "caracteristici windows"},
            {"Open Turn Windows features on or off."},
            "Start-Process OptionalFeatures"
        },
        {
            {"system restore ui", "rstrui", "restaurare sistem"},
            {"Open the System Restore wizard."},
            "Start-Process rstrui"
        },
        {
            {"color management", "gestionare culori"},
            {"Open Color Management."},
            "Start-Process colorcpl"
        },
        {
            {"microsoft store", "store", "magazin"},
            {"Open the Microsoft Store."},
            "Start-Process ms-windows-store:"
        },
        {
            {"clock app", "alarms", "ceas alarme"},
            {"Open the Clock / Alarms app."},
            "Start-Process ms-clock:"
        },
        {
            {"component services", "dcomcnfg"},
            {"Open Component Services."},
            "Start-Process dcomcnfg"
        },

        // ================= Diagnostics / info =================
        {
            {"computer model", "model laptop", "model pc"},
            {"Show the computer manufacturer and model."},
            "Get-CimInstance Win32_ComputerSystem | Select-Object Manufacturer, Model"
        },
        {
            {"cpu load", "cpu usage percent", "incarcare cpu"},
            {"Show the current CPU load percentage."},
            "(Get-CimInstance Win32_Processor).LoadPercentage"
        },
        {
            {"free memory", "memorie libera", "available ram"},
            {"Show the free physical memory (MB)."},
            "[math]::Round((Get-CimInstance Win32_OperatingSystem).FreePhysicalMemory/1024,0)"
        },
        {
            {"stopped services", "servicii oprite"},
            {"List the services that are stopped."},
            "Get-Service | Where-Object {$_.Status -eq 'Stopped'} | Select-Object Name, DisplayName"
        },
        {
            {"dns servers", "servere dns"},
            {"Show the configured DNS servers."},
            "Get-DnsClientServerAddress | Select-Object InterfaceAlias, ServerAddresses"
        },
        {
            {"routing table", "tabela rutare"},
            {"Show the IP routing table."},
            "Get-NetRoute | Select-Object DestinationPrefix, NextHop, RouteMetric"
        },
        {
            {"listening ports", "open ports", "porturi deschise"},
            {"Show the ports that are listening."},
            "Get-NetTCPConnection -State Listen | Select-Object LocalAddress, LocalPort, OwningProcess"
        },
        {
            {"firewall rules", "reguli firewall"},
            {"Show the first 50 enabled firewall rules."},
            "Get-NetFirewallRule | Where-Object {$_.Enabled -eq 'True'} | Select-Object DisplayName, Direction -First 50"
        },
        {
            {"keyboard layout", "aranjament tastatura"},
            {"Show the configured keyboard/input languages."},
            "Get-WinUserLanguageList | Select-Object LanguageTag, Autonym"
        },
        {
            {"antivirus status", "stare antivirus"},
            {"Show the registered antivirus products."},
            "Get-CimInstance -Namespace root\\SecurityCenter2 -ClassName AntiVirusProduct | Select-Object displayName"
        },
        {
            {"pagefile", "fisier paginare"},
            {"Show the page file usage."},
            "Get-CimInstance Win32_PageFileUsage | Select-Object Name, AllocatedBaseSize, CurrentUsage"
        },

        // ================= More admin tools =================
        {
            {"local users", "lusrmgr", "utilizatori locali"},
            {"Open the Local Users and Groups console."},
            "Start-Process lusrmgr.msc"
        },
        {
            {"security policy", "secpol", "politici securitate"},
            {"Open the Local Security Policy console."},
            "Start-Process secpol.msc"
        },
        {
            {"certificates", "certmgr", "certificate"},
            {"Open the Certificates console."},
            "Start-Process certmgr.msc"
        },
        {
            {"advanced firewall", "firewall avansat"},
            {"Open Windows Firewall with Advanced Security."},
            "Start-Process wf.msc"
        },
        {
            {"print management", "gestionare imprimante"},
            {"Open the Print Management console."},
            "Start-Process printmanagement.msc"
        },
        {
            {"defragment", "defrag tool", "defragmentare"},
            {"Open the Defragment and Optimize Drives tool."},
            "Start-Process dfrgui"
        },
        {
            {"memory diagnostic", "mdsched", "diagnostic memorie"},
            {"Open the Windows Memory Diagnostic tool."},
            "Start-Process mdsched"
        },
        {
            {"netplwiz", "advanced user accounts", "autologin"},
            {"Open the advanced User Accounts dialog (netplwiz)."},
            "Start-Process netplwiz"
        },
        {
            {"uac", "uac settings", "user account control"},
            {"Open the User Account Control settings."},
            "Start-Process useraccountcontrolsettings"
        },
        {
            {"internet options", "optiuni internet", "inetcpl"},
            {"Open the Internet Options dialog."},
            "Start-Process inetcpl.cpl"
        },
        {
            {"mouse properties", "proprietati mouse"},
            {"Open the Mouse Properties dialog."},
            "Start-Process main.cpl"
        },
        {
            {"game controllers", "joystick", "controllere"},
            {"Open the Game Controllers dialog."},
            "Start-Process joy.cpl"
        },
        {
            {"cleartype", "text tuner"},
            {"Open the ClearType Text Tuner."},
            "Start-Process cttune"
        },
        {
            {"duplicate screen", "duplica ecranul", "clone display"},
            {"Duplicate the display on all screens."},
            "Start-Process DisplaySwitch -ArgumentList '/clone'"
        },
        {
            {"extend screen", "extinde ecranul", "extend display"},
            {"Extend the desktop across screens."},
            "Start-Process DisplaySwitch -ArgumentList '/extend'"
        },
        {
            {"first screen only", "pc screen only", "doar ecranul principal"},
            {"Show only on the main (internal) screen."},
            "Start-Process DisplaySwitch -ArgumentList '/internal'"
        },
        {
            {"second screen only", "doar al doilea ecran"},
            {"Show only on the second (external) screen."},
            "Start-Process DisplaySwitch -ArgumentList '/external'"
        },
        {
            {"remote assistance", "msra", "asistenta la distanta"},
            {"Open Windows Remote Assistance."},
            "Start-Process msra"
        },
        {
            {"malware scan", "mrt", "malicious software"},
            {"Open the Malicious Software Removal Tool."},
            "Start-Process mrt"
        },
        {
            {"edit environment", "environment editor", "editare variabile"},
            {"Open the Environment Variables editor."},
            "rundll32.exe sysdm.cpl,EditEnvironmentVariables"
        },
        {
            {"system report", "raport sistem", "health report"},
            {"Generate a 60-second system diagnostics report."},
            "Start-Process perfmon -ArgumentList '/report'"
        },

        // ================= Quick actions =================
        {
            {"dark mode", "mod intunecat", "dark theme"},
            {"Switch Windows and apps to dark mode."},
            "Set-ItemProperty -Path HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize -Name AppsUseLightTheme -Value 0; Set-ItemProperty -Path HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize -Name SystemUsesLightTheme -Value 0"
        },
        {
            {"light mode", "mod luminos", "light theme"},
            {"Switch Windows and apps to light mode."},
            "Set-ItemProperty -Path HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize -Name AppsUseLightTheme -Value 1; Set-ItemProperty -Path HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize -Name SystemUsesLightTheme -Value 1"
        },
        {
            {"show extensions", "show file extensions", "arata extensii"},
            {"Show file extensions in Explorer (restarts Explorer)."},
            "Set-ItemProperty -Path HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced -Name HideFileExt -Value 0; Stop-Process -Name explorer -Force"
        },
        {
            {"hide extensions", "ascunde extensii"},
            {"Hide file extensions in Explorer (restarts Explorer)."},
            "Set-ItemProperty -Path HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced -Name HideFileExt -Value 1; Stop-Process -Name explorer -Force"
        },
        {
            {"show hidden files", "arata fisiere ascunse"},
            {"Show hidden files in Explorer (restarts Explorer)."},
            "Set-ItemProperty -Path HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced -Name Hidden -Value 1; Stop-Process -Name explorer -Force"
        },
        {
            {"hide hidden files", "ascunde fisiere ascunse"},
            {"Hide hidden files in Explorer (restarts Explorer)."},
            "Set-ItemProperty -Path HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced -Name Hidden -Value 2; Stop-Process -Name explorer -Force"
        },
        {
            {"screen off", "monitor off", "stinge ecranul"},
            {"Turn off the display (move the mouse to wake it)."},
            "Add-Type -MemberDefinition '[DllImport(\"user32.dll\")] public static extern int SendMessage(int hWnd, int Msg, int wParam, int lParam);' -Name U32 -Namespace W; [W.U32]::SendMessage(-1, 0x0112, 0xF170, 2)"
        },
        {
            {"take screenshot", "fa captura", "screenshot desktop"},
            {"Save a screenshot of the whole screen on the Desktop."},
            "Add-Type -AssemblyName System.Windows.Forms; Add-Type -AssemblyName System.Drawing; $b=[System.Windows.Forms.SystemInformation]::VirtualScreen; $bmp=New-Object System.Drawing.Bitmap($b.Width,$b.Height); $g=[System.Drawing.Graphics]::FromImage($bmp); $g.CopyFromScreen($b.X,$b.Y,0,0,$bmp.Size); $p=\"$env:USERPROFILE\\Desktop\\screenshot_$(Get-Date -Format yyyyMMdd_HHmmss).png\"; $bmp.Save($p); $g.Dispose(); $bmp.Dispose(); \"Saved: $p\""
        },
        {
            {"kill frozen", "not responding", "aplicatii blocate"},
            {"Force-close all applications that are not responding."},
            "taskkill /fi \"status eq not responding\" /f"
        },
        {
            {"restart audio", "audio service", "repara sunetul"},
            {"Restart the Windows Audio service (fixes sound issues)."},
            "Restart-Service Audiosrv -Force"
        },
        {
            {"reset network", "reset tcp ip", "resetare retea"},
            {"Reset the TCP/IP stack (needs a reboot afterwards)."},
            "netsh int ip reset"
        },
        {
            {"reset proxy"},
            {"Reset the WinHTTP proxy settings."},
            "netsh winhttp reset proxy"
        },
        {
            {"sync time", "sincronizeaza ora"},
            {"Synchronize the system clock with the time server."},
            "w32tm /resync"
        },
        {
            {"defrag analyze", "analiza defragmentare"},
            {"Analyze drive C: fragmentation."},
            "defrag C: /A"
        },
        {
            {"presentation mode", "mod prezentare"},
            {"Turn presentation mode ON (no sleep, no notifications)."},
            "Start-Process presentationsettings -ArgumentList '/start'"
        },
        {
            {"stop presentation", "opreste prezentarea"},
            {"Turn presentation mode OFF."},
            "Start-Process presentationsettings -ArgumentList '/stop'"
        },

        // ================= More diagnostics =================
        {
            {"weather", "vremea", "meteo"},
            {"Show the current weather (wttr.in)."},
            "Invoke-RestMethod -Uri 'https://wttr.in/?format=3'"
        },
        {
            {"random password", "generate password", "parola noua"},
            {"Generate a random 16-character password."},
            "-join ((48..57)+(65..90)+(97..122) | Get-Random -Count 16 | ForEach-Object {[char]$_})"
        },
        {
            {"powershell version", "versiune powershell"},
            {"Show the PowerShell version."},
            "$PSVersionTable.PSVersion"
        },
        {
            {"memory slots", "ram slots", "sloturi ram"},
            {"Show the RAM modules (slot, size, speed)."},
            "Get-CimInstance Win32_PhysicalMemory | Select-Object BankLabel, @{n='GB';e={[math]::Round($_.Capacity/1GB,1)}}, Speed, Manufacturer"
        },
        {
            {"audio devices", "dispozitive audio", "sound cards"},
            {"List the audio devices."},
            "Get-CimInstance Win32_SoundDevice | Select-Object Name, Status"
        },
        {
            {"monitors", "monitoare", "cate monitoare"},
            {"Show how many monitors are connected."},
            "(Get-CimInstance -Namespace root\\wmi -ClassName WmiMonitorID).Count"
        },
        {
            {"network speed", "link speed", "viteza retea"},
            {"Show the link speed of the network adapters."},
            "Get-NetAdapter | Select-Object Name, LinkSpeed, Status"
        },
        {
            {"test https", "test port", "test connection"},
            {"Test the HTTPS connection to google.com (port 443)."},
            "Test-NetConnection google.com -Port 443"
        },
        {
            {"hosts file", "fisier hosts"},
            {"Show the contents of the hosts file."},
            "Get-Content C:\\Windows\\System32\\drivers\\etc\\hosts"
        },
        {
            {"hosts folder", "etc folder"},
            {"Open the folder that contains the hosts file."},
            "Start-Process explorer C:\\Windows\\System32\\drivers\\etc"
        },
        {
            {"install date", "data instalarii"},
            {"Show when Windows was installed."},
            "(Get-CimInstance Win32_OperatingSystem).InstallDate"
        },
        {
            {"bios date", "data bios"},
            {"Show the BIOS release date."},
            "(Get-CimInstance Win32_BIOS).ReleaseDate"
        },
        {
            {"app errors", "application errors", "erori aplicatii"},
            {"Show the last 10 application errors from the event log."},
            "Get-EventLog -LogName Application -EntryType Error -Newest 10"
        },
        {
            {"default printer", "imprimanta implicita"},
            {"Show the default printer."},
            "Get-CimInstance Win32_Printer | Where-Object {$_.Default} | Select-Object Name"
        },
        {
            {"largest downloads", "fisiere mari", "big files"},
            {"Show the 10 largest files in Downloads."},
            "Get-ChildItem \"$env:USERPROFILE\\Downloads\" -File -Recurse -ErrorAction SilentlyContinue | Sort-Object Length -Descending | Select-Object -First 10 Name, @{n='MB';e={[math]::Round($_.Length/1MB,1)}}"
        },
        {
            {"activation status", "stare activare"},
            {"Show the Windows activation/license status."},
            "Get-CimInstance SoftwareLicensingProduct -Filter \"Name like 'Windows%' and PartialProductKey is not null\" | Select-Object Name, LicenseStatus"
        },
        {
            {"store apps", "aplicatii store", "appx"},
            {"List the installed Store apps (first 60)."},
            "Get-AppxPackage | Select-Object -First 60 Name"
        },
        {
            {"cpu temperature", "temperatura procesor"},
            {"Show the CPU temperature (if the hardware supports it)."},
            "Get-CimInstance -Namespace root/wmi -ClassName MSAcpi_ThermalZoneTemperature | Select-Object @{n='Celsius';e={[math]::Round($_.CurrentTemperature/10-273.15,1)}}"
        },

        // ================= More apps =================
        {
            {"notepad plus", "npp"},
            {"Open Notepad++."},
            "Start-Process notepad++"
        },
        {
            {"7zip", "seven zip"},
            {"Open 7-Zip File Manager."},
            "Start-Process 7zFM"
        },
        {
            {"winrar"},
            {"Open WinRAR."},
            "Start-Process winrar"
        },
        {
            {"putty"},
            {"Open PuTTY."},
            "Start-Process putty"
        },
        {
            {"filezilla"},
            {"Open FileZilla."},
            "Start-Process filezilla"
        },
        {
            {"gimp"},
            {"Open GIMP."},
            "Start-Process gimp"
        },
        {
            {"audacity"},
            {"Open Audacity."},
            "Start-Process audacity"
        },
        {
            {"obs", "obs studio"},
            {"Open OBS Studio."},
            "Start-Process obs64"
        },
        {
            {"privacy settings", "setari confidentialitate"},
            {"Open the Privacy settings (home)."},
            "Start-Process ms-settings:privacy"
        },
        {
            {"connected devices", "dispozitive conectate"},
            {"Open the Connected Devices settings."},
            "Start-Process ms-settings:connecteddevices"
        },
        {
            {"work school", "access work", "cont organizatie"},
            {"Open the Access work or school settings."},
            "Start-Process ms-settings:workplace"
        },
        {
            {"this pc", "my computer", "computerul meu"},
            {"Open This PC."},
            "Start-Process explorer shell:MyComputerFolder"
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

// ============================================================================
//  Generated command families. These loops mass-produce real entries so the
//  assistant recognizes as many user requests as possible: every brightness /
//  volume percentage, shutdown/restart timers for any minute count, dozens of
//  websites, "close <app>" for dozens of apps, privacy pages, ping targets.
// ============================================================================

inline void addEntry(vector<CommandEntry>& t, const vector<string>& keywords,
                     const string& step, const string& command)
{
    CommandEntry e;
    e.keywords = keywords;
    e.steps.push_back(step);
    e.command = command;
    t.push_back(e);
}

inline void appendGeneratedCommands(vector<CommandEntry>& t)
{
    // ---------------- Websites ----------------
    struct GenSite { const char* key; const char* alias; const char* url; };
    static const GenSite sites[] = {
        {"youtube", "yt", "https://www.youtube.com"},
        {"google", 0, "https://www.google.com"},
        {"gmail", 0, "https://mail.google.com"},
        {"facebook", "fb", "https://www.facebook.com"},
        {"instagram", "insta", "https://www.instagram.com"},
        {"tiktok", 0, "https://www.tiktok.com"},
        {"twitter", 0, "https://twitter.com"},
        {"reddit", 0, "https://www.reddit.com"},
        {"netflix", 0, "https://www.netflix.com"},
        {"hbo", "hbo max", "https://www.max.com"},
        {"disney", "disney plus", "https://www.disneyplus.com"},
        {"prime video", "prime", "https://www.primevideo.com"},
        {"whatsapp", 0, "https://web.whatsapp.com"},
        {"telegram web", 0, "https://web.telegram.org"},
        {"messenger", 0, "https://www.messenger.com"},
        {"twitch", 0, "https://www.twitch.tv"},
        {"github", 0, "https://github.com"},
        {"gitlab", 0, "https://gitlab.com"},
        {"stackoverflow", "stack overflow", "https://stackoverflow.com"},
        {"wikipedia", "wiki", "https://www.wikipedia.org"},
        {"amazon", 0, "https://www.amazon.com"},
        {"ebay", 0, "https://www.ebay.com"},
        {"aliexpress", 0, "https://www.aliexpress.com"},
        {"temu", 0, "https://www.temu.com"},
        {"emag", 0, "https://www.emag.ro"},
        {"olx", 0, "https://www.olx.ro"},
        {"linkedin", 0, "https://www.linkedin.com"},
        {"pinterest", 0, "https://www.pinterest.com"},
        {"snapchat", 0, "https://web.snapchat.com"},
        {"maps", "google maps", "https://maps.google.com"},
        {"waze", 0, "https://www.waze.com/live-map"},
        {"translate", "google translate", "https://translate.google.com"},
        {"google drive", "drive", "https://drive.google.com"},
        {"google docs", "docs", "https://docs.google.com"},
        {"google sheets", "sheets", "https://sheets.google.com"},
        {"google slides", "slides", "https://slides.google.com"},
        {"google calendar", "calendar", "https://calendar.google.com"},
        {"google meet", "meet", "https://meet.google.com"},
        {"zoom web", 0, "https://zoom.us/join"},
        {"slack", 0, "https://slack.com"},
        {"notion", 0, "https://www.notion.so"},
        {"trello", 0, "https://trello.com"},
        {"asana", 0, "https://app.asana.com"},
        {"canva", 0, "https://www.canva.com"},
        {"figma", 0, "https://www.figma.com"},
        {"chatgpt", "gpt", "https://chatgpt.com"},
        {"claude", 0, "https://claude.ai"},
        {"gemini", 0, "https://gemini.google.com"},
        {"copilot", 0, "https://copilot.microsoft.com"},
        {"bing", 0, "https://www.bing.com"},
        {"duckduckgo", 0, "https://duckduckgo.com"},
        {"yahoo", 0, "https://www.yahoo.com"},
        {"outlook web", 0, "https://outlook.live.com"},
        {"office", "office online", "https://www.office.com"},
        {"onedrive web", 0, "https://onedrive.live.com"},
        {"dropbox", 0, "https://www.dropbox.com"},
        {"mega", 0, "https://mega.nz"},
        {"wetransfer", 0, "https://wetransfer.com"},
        {"speedtest", "viteza internet", "https://www.speedtest.net"},
        {"fast", "fast com", "https://fast.com"},
        {"downdetector", 0, "https://downdetector.com"},
        {"imdb", 0, "https://www.imdb.com"},
        {"goodreads", 0, "https://www.goodreads.com"},
        {"soundcloud", 0, "https://soundcloud.com"},
        {"deezer", 0, "https://www.deezer.com"},
        {"spotify web", 0, "https://open.spotify.com"},
        {"bbc", 0, "https://www.bbc.com"},
        {"cnn", 0, "https://www.cnn.com"},
        {"reuters", 0, "https://www.reuters.com"},
        {"digi24", 0, "https://www.digi24.ro"},
        {"protv", "pro tv", "https://www.protv.ro"},
        {"booking", 0, "https://www.booking.com"},
        {"airbnb", 0, "https://www.airbnb.com"},
        {"skyscanner", 0, "https://www.skyscanner.net"},
        {"uber", 0, "https://m.uber.com"},
        {"bolt", 0, "https://bolt.eu"},
        {"glovo", 0, "https://glovoapp.com"},
        {"paypal", 0, "https://www.paypal.com"},
        {"revolut", 0, "https://app.revolut.com"},
        {"steam store", 0, "https://store.steampowered.com"},
        {"epic store", "epic games", "https://store.epicgames.com"}
    };
    for (size_t i = 0; i < sizeof(sites) / sizeof(sites[0]); ++i) {
        vector<string> kw;
        kw.push_back(sites[i].key);
        if (sites[i].alias) kw.push_back(sites[i].alias);
        addEntry(t, kw,
                 string("Open ") + sites[i].key + " in the browser.",
                 string("Start-Process ") + sites[i].url);
    }

    // ---------------- Close / kill applications ----------------
    struct GenApp { const char* key; const char* proc; };
    static const GenApp apps[] = {
        {"chrome", "chrome"}, {"edge", "msedge"}, {"firefox", "firefox"},
        {"opera", "opera"}, {"brave", "brave"}, {"notepad", "notepad"},
        {"word", "winword"}, {"excel", "excel"}, {"powerpoint", "powerpnt"},
        {"outlook", "outlook"}, {"onenote", "onenote"}, {"teams", "Teams"},
        {"discord", "Discord"}, {"spotify", "Spotify"}, {"steam", "steam"},
        {"epic", "EpicGamesLauncher"}, {"vlc", "vlc"}, {"paint", "mspaint"},
        {"calculator", "Calculator*"}, {"notepad plus", "notepad++"},
        {"photoshop", "Photoshop"}, {"illustrator", "Illustrator"},
        {"acrobat", "Acrobat*"}, {"telegram", "Telegram"},
        {"whatsapp", "WhatsApp"}, {"skype", "Skype"}, {"zoom", "Zoom"},
        {"obs", "obs64"}, {"vscode", "Code"}, {"code", "Code"},
        {"visual studio", "devenv"}, {"putty", "putty"},
        {"filezilla", "filezilla"}, {"gimp", "gimp"},
        {"audacity", "audacity"}, {"battle net", "Battle.net"},
        {"origin", "Origin"}, {"gog", "GalaxyClient"},
        {"league", "LeagueClient*"}, {"roblox", "RobloxPlayerBeta"},
        {"itunes", "iTunes"}, {"winrar", "WinRAR"}, {"7zip", "7zFM"}
    };
    for (size_t i = 0; i < sizeof(apps) / sizeof(apps[0]); ++i) {
        string k = apps[i].key;
        vector<string> kw;
        kw.push_back("close " + k);
        kw.push_back("kill " + k);
        kw.push_back("stop " + k);
        kw.push_back("inchide " + k);
        kw.push_back("opreste " + k);
        addEntry(t, kw,
                 string("Force-close ") + k + ".",
                 string("Stop-Process -Name '") + apps[i].proc + "' -Force -ErrorAction SilentlyContinue");
    }

    // ---------------- Brightness: any percentage 0..100 ----------------
    for (int p = 0; p <= 100; ++p) {
        string n = numToStr(p);
        vector<string> kw;
        kw.push_back("brightness " + n);
        kw.push_back("luminozitate " + n);
        addEntry(t, kw,
                 "Set the screen brightness to " + n + "%.",
                 "(Get-WmiObject -Namespace root/WMI -Class WmiMonitorBrightnessMethods).WmiSetBrightness(1," + n + ")");
    }

    // ---------------- Volume: any percentage 0..100 (media keys) ----------------
    for (int p = 0; p <= 100; ++p) {
        string n = numToStr(p);
        int ups = p / 2;   // each volume key press changes the level by 2
        string cmd = "$w = New-Object -ComObject WScript.Shell; "
                     "1..50 | ForEach-Object { $w.SendKeys([char]174) }";
        if (ups > 0) {
            cmd += "; 1.." + numToStr(ups) + " | ForEach-Object { $w.SendKeys([char]175) }";
        }
        vector<string> kw;
        kw.push_back("volume " + n);
        kw.push_back("volum " + n);
        kw.push_back("sound " + n);
        addEntry(t, kw, "Set the system volume to about " + n + "%.", cmd);
    }
    addEntry(t, {"mute", "fara sunet", "mute sound"},
             "Toggle mute on/off.",
             "$w = New-Object -ComObject WScript.Shell; $w.SendKeys([char]173)");
    addEntry(t, {"unmute", "cu sunet", "unmute sound"},
             "Toggle mute on/off.",
             "$w = New-Object -ComObject WScript.Shell; $w.SendKeys([char]173)");
    addEntry(t, {"volume up", "volum sus", "louder", "mai tare"},
             "Raise the volume by about 10%.",
             "$w = New-Object -ComObject WScript.Shell; 1..5 | ForEach-Object { $w.SendKeys([char]175) }");
    addEntry(t, {"volume down", "volum jos", "quieter", "mai incet"},
             "Lower the volume by about 10%.",
             "$w = New-Object -ComObject WScript.Shell; 1..5 | ForEach-Object { $w.SendKeys([char]174) }");

    // ---------------- Shutdown / restart timers ----------------
    for (int m = 1; m <= 240; ++m) {
        string n = numToStr(m);
        string sec = numToStr(m * 60);

        vector<string> kws;
        kws.push_back("shutdown " + n);
        kws.push_back("shutdown " + n + " minutes");
        kws.push_back("opreste " + n + " minute");
        addEntry(t, kws,
                 "Schedule a shutdown in " + n + " minute(s).",
                 "shutdown /s /t " + sec);

        vector<string> kwr;
        kwr.push_back("restart " + n);
        kwr.push_back("restart " + n + " minutes");
        kwr.push_back("reporneste " + n + " minute");
        addEntry(t, kwr,
                 "Schedule a restart in " + n + " minute(s).",
                 "shutdown /r /t " + sec);
    }
    for (int h = 1; h <= 24; ++h) {
        string n = numToStr(h);
        string sec = numToStr(h * 3600);

        vector<string> kws;
        kws.push_back("shutdown " + n + " hours");
        kws.push_back("opreste " + n + " ore");
        addEntry(t, kws,
                 "Schedule a shutdown in " + n + " hour(s).",
                 "shutdown /s /t " + sec);

        vector<string> kwr;
        kwr.push_back("restart " + n + " hours");
        kwr.push_back("reporneste " + n + " ore");
        addEntry(t, kwr,
                 "Schedule a restart in " + n + " hour(s).",
                 "shutdown /r /t " + sec);
    }
    addEntry(t, {"cancel shutdown", "abort shutdown", "anuleaza oprirea", "stop shutdown"},
             "Cancel a scheduled shutdown/restart.",
             "shutdown /a");

    // ---------------- Privacy permission pages ----------------
    struct GenPriv { const char* key; const char* uri; };
    static const GenPriv priv[] = {
        {"notifications", "privacy-notifications"},
        {"account info", "privacy-accountinfo"},
        {"contacts", "privacy-contacts"},
        {"calendar", "privacy-calendar"},
        {"call history", "privacy-callhistory"},
        {"email", "privacy-email"},
        {"tasks", "privacy-tasks"},
        {"messaging", "privacy-messaging"},
        {"radios", "privacy-radios"},
        {"background apps", "privacy-backgroundapps"},
        {"app diagnostics", "privacy-appdiagnostics"},
        {"documents", "privacy-documents"},
        {"pictures", "privacy-pictures"},
        {"videos", "privacy-customdevices"},
        {"voice activation", "privacy-voiceactivation"},
        {"phone calls", "privacy-phonecalls"},
        {"motion", "privacy-motion"},
        {"general", "privacy-general"}
    };
    for (size_t i = 0; i < sizeof(priv) / sizeof(priv[0]); ++i) {
        string k = priv[i].key;
        vector<string> kw;
        kw.push_back("privacy " + k);
        kw.push_back(k + " permissions");
        kw.push_back("permisiuni " + k);
        addEntry(t, kw,
                 string("Open the ") + k + " privacy settings.",
                 string("Start-Process ms-settings:") + priv[i].uri);
    }

    // ---------------- Ping targets ----------------
    struct GenPing { const char* key; const char* host; };
    static const GenPing pings[] = {
        {"ping cloudflare", "1.1.1.1"},
        {"ping dns", "8.8.8.8"},
        {"ping youtube", "youtube.com"},
        {"ping facebook", "facebook.com"},
        {"ping instagram", "instagram.com"},
        {"ping tiktok", "tiktok.com"},
        {"ping github", "github.com"}
    };
    for (size_t i = 0; i < sizeof(pings) / sizeof(pings[0]); ++i) {
        addEntry(t, {pings[i].key},
                 string("Ping ") + pings[i].host + ".",
                 string("ping ") + pings[i].host);
    }
    addEntry(t, {"ping router", "ping gateway"},
             "Ping the default gateway (your router).",
             "ping $((Get-NetRoute -DestinationPrefix 0.0.0.0/0 | Select-Object -First 1 -ExpandProperty NextHop))");
}

inline const vector<CommandEntry>& getCommandTable()
{
    static vector<CommandEntry> table;
    if (table.empty()) {
        table = getBaseCommands();
        appendGeneratedCommands(table);
    }
    return table;
}

#endif // IT_ASSISTANT_COMMANDS_H
