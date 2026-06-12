#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <cctype>

#if defined(__MINGW32__) && __GNUC__ < 5

namespace std
{
    template <typename T>
    string to_string(T value)
    {
        ostringstream stream;
        stream << value;
        return stream.str();
    }
}
#endif

using namespace std;

#if defined(__MINGW32__)
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN _popen
#define PCLOSE _pclose
#endif


string toLowerString(string value)
{
    transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value;
}

string trim(const string& value)
{
    size_t start = 0;
    while (start < value.size() && isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    size_t end = value.size();
    while (end > start && isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

string quoteArg(const string& value)
{
    string quoted = "\"";
    for (char ch : value) {
        if (ch == '"') quoted += "\\\"";
        else quoted += ch;
    }
    quoted += "\"";
    return quoted;
}

// Split a string into lowercase words on any non-alphanumeric character.
vector<string> splitWords(const string& text)
{
    string lower = toLowerString(text);
    vector<string> words;
    string token;
    for (char ch : lower) {
        if (isalnum(static_cast<unsigned char>(ch))) {
            token += ch;
        } else if (!token.empty()) {
            words.push_back(token);
            token.clear();
        }
    }
    if (!token.empty()) words.push_back(token);
    return words;
}


bool isSafe(const string& command)
{
    vector<string> dangerous = {
        "format-", "clear-disk",
        "reg delete",
        "add-localgroupmember",
        "invoke-expression", "iex ",
        "hklm:\\system", "hklm:\\sam"
    };

    string cmdLower = toLowerString(command);

    for (const auto& pattern : dangerous) {
        if (cmdLower.find(pattern) != string::npos) {
            cout << "Dangerous command detected: " << pattern << endl;
            return false;
        }
    }
    return true;
}

bool confirmShutdown(const string& command)
{
    string cmdLower = toLowerString(command);
    if (cmdLower.find("shutdown /a") != string::npos) return true;  // cancel needs no confirm
    if (cmdLower.find("shutdown") != string::npos ||
        cmdLower.find("stop-computer") != string::npos ||
        cmdLower.find("restart-computer") != string::npos)
    {
        cout << "\nDo you really want to run this power command (shutdown/restart/logoff)? [Y/N]: ";
        string answer;
        getline(cin, answer);
        answer = toLowerString(trim(answer));
        return (answer == "y" || answer == "yes");
    }
    return true;
}


#include "commands/commands.h"

// Filler words that carry no command meaning and are dropped before matching.
bool isStopWord(const string& word)
{
    static const set<string> stop = {
        // English
        "to", "the", "a", "an", "of", "for", "with", "and", "or", "my", "me",
        "you", "your", "please", "could", "would", "want", "wanna", "get",
        "give", "show", "tell", "do", "does", "is", "are", "it", "this", "that",
        "in", "into", "at", "by", "set", "make", "turn", "open", "run", "launch",
        "go", "now",
        // Romanian
        "la", "pe", "un", "o", "si", "sa", "vreau", "te", "rog", "imi", "mi",
        "pentru", "din", "al", "ale", "cu", "mai", "foarte", "de", "ca", "sau",
        "deschide", "arata", "da", "fa", "as", "vrea"
    };
    return stop.count(word) > 0;
}

// Reduce a message to its MAIN words: lowercase, split on non-alphanumeric,
// drop tiny tokens and stop words. Also folds "wi-fi"/"wi fi" into "wifi".
// "turn brightness to maximum" -> {brightness, maximum}
vector<string> extractMainWords(const string& message)
{
    string lower = toLowerString(message);

    // Normalize common spellings before tokenizing.
    size_t pos;
    while ((pos = lower.find("wi-fi")) != string::npos) lower.replace(pos, 5, "wifi");
    while ((pos = lower.find("wi fi")) != string::npos) lower.replace(pos, 5, "wifi");

    vector<string> words;
    set<string> seen;
    for (const auto& token : splitWords(lower)) {
        // keep single digits ("brightness 5"), drop other 1-char tokens
        if (token.size() < 2 && !isdigit(static_cast<unsigned char>(token[0]))) continue;
        if (isStopWord(token)) continue;
        if (seen.insert(token).second) {
            words.push_back(token);
        }
    }
    return words;
}

// True if every word of a (possibly multi-word) tag is present in mainWords.
bool tagMatches(const string& tag, const set<string>& mainWords, int& wordCount)
{
    vector<string> parts = splitWords(tag);
    wordCount = static_cast<int>(parts.size());
    if (parts.empty()) return false;

    for (const auto& part : parts) {
        if (mainWords.find(part) == mainWords.end()) return false;
    }
    return true;
}

// Find the command whose tags match the most of the message's main words.
// A multi-word tag scores as many points as it has words, so specific phrases
// beat generic single words. detectedOut receives the extracted main words.
const CommandEntry* matchKeywords(const string& message, vector<string>& detectedOut)
{
    detectedOut = extractMainWords(message);
    set<string> mainWords(detectedOut.begin(), detectedOut.end());

    const CommandEntry* best = nullptr;
    int bestScore = 0;

    for (const auto& entry : getCommandTable()) {
        int score = 0;
        for (const auto& tag : entry.keywords) {
            int wordCount = 0;
            if (tagMatches(tag, mainWords, wordCount)) {
                score += wordCount;
            }
        }
        if (score > bestScore) {
            bestScore = score;
            best = &entry;
        }
    }

    return best;
}

void printCapabilities()
{
    cout << "\nAvailable commands (sample keywords -> command):\n";
    for (const auto& entry : getCommandTable()) {
        cout << "  - ";
        for (size_t i = 0; i < entry.keywords.size(); ++i) {
            cout << entry.keywords[i];
            if (i + 1 < entry.keywords.size()) cout << ", ";
        }
        cout << "  ->  " << entry.command << "\n";
    }
    cout << "\n";
}


string createTempFile(const string& prefix, const string& content)
{
    char tempPath[MAX_PATH];
    char tempFile[MAX_PATH];

    if (GetTempPathA(MAX_PATH, tempPath) == 0) return "";
    if (GetTempFileNameA(tempPath, prefix.c_str(), 0, tempFile) == 0) return "";

    ofstream file(tempFile, ios::out | ios::binary);
    if (!file) return "";
    file << content;
    file.close();

    return string(tempFile);
}

// Prepended to every PowerShell script. When the app is elevated under a
// DIFFERENT admin account, the process USERPROFILE points at the admin, not the
// person actually logged in. This rewrites USERPROFILE/HOME/TEMP to the
// interactively logged-on user's profile so file actions land on their account.
static const char* kProfileFix = R"PS(
try {
  $__u = (Get-CimInstance Win32_ComputerSystem -ErrorAction Stop).UserName
  if ($__u) {
    $__sid = (New-Object System.Security.Principal.NTAccount($__u)).Translate([System.Security.Principal.SecurityIdentifier]).Value
    $__pp = (Get-ItemProperty "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ProfileList\$__sid" -ErrorAction Stop).ProfileImagePath
    if ($__pp -and (Test-Path $__pp)) {
      $env:USERPROFILE = $__pp
      $env:HOMEDRIVE   = $__pp.Substring(0,2)
      $env:HOMEPATH    = $__pp.Substring(2)
      $env:TEMP        = Join-Path $__pp 'AppData\Local\Temp'
      $env:TMP         = $env:TEMP
      Set-Variable -Name HOME -Value $__pp -Scope Global -Force -ErrorAction SilentlyContinue
    }
  }
} catch {}
)PS";

string runPowerShell(const string& command)
{
    if (trim(command).empty()) {
        return "Error: empty command.\n\n";
    }

    string scriptPath = createTempFile("itp", string(kProfileFix) + "\n" + command + "\n");
    if (scriptPath.empty()) return "Error at creating temporary PowerShell script";

    string ps1Path = scriptPath + ".ps1";
    if (!MoveFileA(scriptPath.c_str(), ps1Path.c_str())) {
        remove(scriptPath.c_str());
        return "Error at preparing temporary PowerShell script";
    }

    string fullCmd = "powershell.exe -NoProfile -ExecutionPolicy Bypass -File " +
        quoteArg(ps1Path) + " 2>&1";

    FILE* pipe = POPEN(fullCmd.c_str(), "r");
    if (!pipe) {
        remove(ps1Path.c_str());
        return "Error at opening PowerShell";
    }

    string result;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    PCLOSE(pipe);
    remove(ps1Path.c_str());

    return result;
}

bool commandRequiresAdmin(const string& command)
{
    string lower = toLowerString(command);

    vector<string> adminPatterns = {
        "admin=disabled",
        "admin=enabled",
        "disable-netadapter",
        "enable-netadapter",
        "restart-computer",
        "stop-computer",
        "clear-disk",
        "format-",
        "ipconfig /flushdns",
        "ipconfig /release",
        "ipconfig /renew",
        "advfirewall set allprofiles",
        "winsock reset",
        "sfc /scannow",
        "dism /online",
        "restart-service spooler",
        "restart-service audiosrv",
        "netsh int ip reset",
        "w32tm /resync",
        "defrag ",
        "checkpoint-computer"
    };

    for (const auto& pattern : adminPatterns) {
        if (lower.find(pattern) != string::npos) return true;
    }

    return false;
}

bool isRunningAsAdmin()
{
    string cmd =
        "powershell.exe -NoProfile -Command "
        "\"([Security.Principal.WindowsPrincipal] "
        "[Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole("
        "[Security.Principal.WindowsBuiltInRole]::Administrator)\" 2>nul";

    FILE* pipe = POPEN(cmd.c_str(), "r");
    if (!pipe) return false;

    string result;
    char buffer[64];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    PCLOSE(pipe);

    return toLowerString(trim(result)) == "true";
}


int runConsole()
{
    cout << "=== IT Assistant (console mode) ===\n";
    cout << "Type a task in plain words. Type 'help' to list keywords, 'exit' to quit.\n\n";

    while (true)
    {
        cout << "Your task: ";
        string input;
        getline(cin, input);

        string trimmed = toLowerString(trim(input));
        if (trimmed == "exit" || trimmed == "quit") break;
        if (trimmed.empty()) continue;
        if (trimmed == "help" || trimmed == "list") {
            printCapabilities();
            continue;
        }

        vector<string> detected;
        const CommandEntry* match = matchKeywords(input, detected);

        cout << "Detected keywords: ";
        if (detected.empty()) {
            cout << "(none)";
        } else {
            for (size_t i = 0; i < detected.size(); ++i) {
                cout << detected[i];
                if (i + 1 < detected.size()) cout << ", ";
            }
        }
        cout << endl;

        if (match == nullptr) {
            cout << "No matching command found. Type 'help' to see what I understand.\n\n";
            continue;
        }

        cout << "\nSteps:\n";
        for (const auto& step : match->steps) {
            cout << " - " << step << endl;
        }

        const string& command = match->command;
        cout << "\nCommand: " << command << endl;

        if (!isSafe(command)) {
            cout << "Command BLOCKED.\n\n";
            continue;
        }
        if (!confirmShutdown(command)) {
            cout << "Command cancelled by user.\n\n";
            continue;
        }

        if (commandRequiresAdmin(command) && !isRunningAsAdmin()) {
            cout << "This command requires Administrator rights. Run main.exe from an elevated PowerShell window.\n\n";
            continue;
        }

        cout << "\nExecuting...\n";
        string output = runPowerShell(command);
        cout << "Output:\n" << output << endl;
        cout << "------------------\n\n";
    }

    return 0;
}


// ============================================================================
//  Local web server (HTML/CSS UI in the browser)
//  Listens only on 127.0.0.1, reuses the same matching / safety / execution.
// ============================================================================

static int hexVal(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static string urlDecode(const string& s)
{
    string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '+') {
            out += ' ';
        } else if (s[i] == '%' && i + 2 < s.size()) {
            int hi = hexVal(s[i + 1]);
            int lo = hexVal(s[i + 2]);
            if (hi >= 0 && lo >= 0) {
                out += static_cast<char>(hi * 16 + lo);
                i += 2;
            } else {
                out += s[i];
            }
        } else {
            out += s[i];
        }
    }
    return out;
}

static string getQueryParam(const string& query, const string& key)
{
    string prefix = key + "=";
    size_t pos = 0;
    while (pos < query.size()) {
        size_t amp = query.find('&', pos);
        string pair = (amp == string::npos) ? query.substr(pos) : query.substr(pos, amp - pos);
        if (pair.compare(0, prefix.size(), prefix) == 0) {
            return urlDecode(pair.substr(prefix.size()));
        }
        if (amp == string::npos) break;
        pos = amp + 1;
    }
    return "";
}

static string jsonEscape(const string& s)
{
    string out;
    for (unsigned char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) {
                    char buf[8];
                    sprintf(buf, "\\u%04x", c);
                    out += buf;
                } else {
                    out += static_cast<char>(c);
                }
        }
    }
    return out;
}

static string jsonArray(const vector<string>& items)
{
    string out = "[";
    for (size_t i = 0; i < items.size(); ++i) {
        out += "\"" + jsonEscape(items[i]) + "\"";
        if (i + 1 < items.size()) out += ",";
    }
    out += "]";
    return out;
}

static bool isShutdownLike(const string& command)
{
    string l = toLowerString(command);
    if (l.find("shutdown /a") != string::npos) return false;  // cancel needs no confirm
    return l.find("shutdown") != string::npos ||
           l.find("stop-computer") != string::npos ||
           l.find("restart-computer") != string::npos;
}

// Run a task and return the result as a JSON string for the web UI.
static string processTaskJson(const string& task, bool confirmed)
{
    vector<string> detected;
    const CommandEntry* m = matchKeywords(task, detected);

    string j = "{\"detected\":" + jsonArray(detected) + ",";

    if (m == nullptr) {
        j += "\"matched\":false,\"message\":\"No matching command found.\"}";
        return j;
    }

    string command = m->command;
    j += "\"matched\":true,";
    j += "\"steps\":" + jsonArray(m->steps) + ",";
    j += "\"command\":\"" + jsonEscape(command) + "\",";

    if (!isSafe(command)) {
        j += "\"blocked\":true,\"message\":\"Command BLOCKED (unsafe).\"}";
        return j;
    }
    if (isShutdownLike(command) && !confirmed) {
        j += "\"needsConfirm\":true,\"message\":\"This is a power command. Confirm to run.\"}";
        return j;
    }
    if (commandRequiresAdmin(command) && !isRunningAsAdmin()) {
        j += "\"needsAdmin\":true,\"message\":\"Requires Administrator. Start the app from an elevated window.\"}";
        return j;
    }

    string output = runPowerShell(command);
    j += "\"executed\":true,\"output\":\"" + jsonEscape(output) + "\",\"message\":\"Executed.\"}";
    return j;
}

static string listJson()
{
    const vector<CommandEntry>& table = getCommandTable();
    string j = "[";
    for (size_t i = 0; i < table.size(); ++i) {
        j += "{\"keywords\":" + jsonArray(table[i].keywords) +
             ",\"command\":\"" + jsonEscape(table[i].command) + "\"}";
        if (i + 1 < table.size()) j += ",";
    }
    j += "]";
    return j;
}

static const char* HTML_PAGE = R"PAGE(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>IT Assistant</title>
<style>
  :root{--bg:#0f172a;--card:#1e293b;--accent:#38bdf8;--text:#e2e8f0;--muted:#94a3b8;}
  *{box-sizing:border-box}
  body{margin:0;font-family:'Segoe UI',system-ui,sans-serif;background:var(--bg);color:var(--text);}
  .wrap{max-width:820px;margin:0 auto;padding:28px 18px;}
  h1{font-size:24px;margin:0 0 4px;}
  .sub{color:var(--muted);margin-bottom:20px;}
  .row{display:flex;gap:10px;}
  input[type=text]{flex:1;padding:12px 14px;border-radius:10px;border:1px solid #334155;background:#0b1220;color:var(--text);font-size:15px;}
  button{padding:12px 18px;border:0;border-radius:10px;background:var(--accent);color:#06283d;font-weight:600;cursor:pointer;font-size:15px;}
  .card{background:var(--card);border:1px solid #334155;border-radius:12px;padding:16px 18px;margin-top:18px;}
  .label{color:var(--muted);font-size:12px;text-transform:uppercase;letter-spacing:.05em;margin-bottom:6px;}
  .tags span{display:inline-block;background:#0b1220;border:1px solid #334155;border-radius:999px;padding:3px 10px;margin:0 6px 6px 0;font-size:13px;}
  code{display:block;background:#0b1220;border:1px solid #334155;border-radius:8px;padding:10px 12px;white-space:pre-wrap;word-break:break-word;color:#7dd3fc;}
  pre{background:#0b1220;border:1px solid #334155;border-radius:8px;padding:12px;white-space:pre-wrap;word-break:break-word;max-height:320px;overflow:auto;margin:0;}
  ul{margin:6px 0 0;padding-left:18px;}
  .msg{margin-top:10px;}
  .warn{color:#fbbf24;} .err{color:#f87171;} .ok{color:#4ade80;}
  details{margin-top:22px;}
  summary{cursor:pointer;color:var(--muted);}
  .cmd-item{padding:8px 0;border-bottom:1px solid #1e293b;font-size:14px;}
  .cmd-item b{color:var(--accent);}
  .hint{color:var(--muted);font-size:13px;margin-top:8px;}
  .badge{font-size:12px;padding:3px 10px;border-radius:999px;vertical-align:middle;margin-left:8px;}
  .badge.admin{background:#064e3b;color:#4ade80;}
  .badge.restricted{background:#3f1d1d;color:#fbbf24;}
</style>
</head>
<body>
  <div class="wrap">
    <h1>IT Assistant <span id="mode" class="badge"></span></h1>
    <div class="sub">Type a task in plain words (e.g. "turn brightness to maximum", "open task manager", "contrast settings").</div>
    <div class="row">
      <input id="task" type="text" placeholder="What do you want to do?" autofocus>
      <button onclick="run(false)">Run</button>
      <button onclick="quit()" style="background:#334155;color:var(--text)">Quit</button>
    </div>
    <div class="hint">Press Enter to run. Power commands (shutdown/restart) ask for confirmation.</div>
    <div id="result"></div>
    <details>
      <summary>Show all available commands</summary>
      <div id="list"></div>
    </details>
  </div>
<script>
const taskEl = document.getElementById('task');
const resultEl = document.getElementById('result');
taskEl.addEventListener('keydown', e => { if (e.key === 'Enter') run(false); });
function esc(s){ const d=document.createElement('div'); d.textContent=s; return d.innerHTML; }
async function run(confirmed){
  const t = taskEl.value.trim();
  if(!t) return;
  resultEl.innerHTML = '<div class="card">Working...</div>';
  let url = '/api/run?task=' + encodeURIComponent(t);
  if(confirmed) url += '&confirm=1';
  try{
    const r = await fetch(url);
    const d = await r.json();
    render(d);
    if(d.needsConfirm){
      if(confirm('This is a power command (shutdown/restart/logoff). Run it?')) run(true);
    }
  }catch(err){
    resultEl.innerHTML = '<div class="card err">Request failed: ' + esc(String(err)) + '</div>';
  }
}
function render(d){
  let h = '<div class="card">';
  if(d.detected && d.detected.length){
    h += '<div class="label">Detected keywords</div><div class="tags">';
    h += d.detected.map(k => '<span>' + esc(k) + '</span>').join('');
    h += '</div>';
  }
  if(!d.matched){
    h += '<div class="msg warn">' + esc(d.message || 'No match.') + '</div></div>';
    resultEl.innerHTML = h; return;
  }
  if(d.steps && d.steps.length){
    h += '<div class="label" style="margin-top:10px">Steps</div><ul>';
    h += d.steps.map(s => '<li>' + esc(s) + '</li>').join('');
    h += '</ul>';
  }
  h += '<div class="label" style="margin-top:10px">Command</div><code>' + esc(d.command || '') + '</code>';
  if(d.blocked) h += '<div class="msg err">' + esc(d.message) + '</div>';
  else if(d.needsAdmin) h += '<div class="msg warn">' + esc(d.message) + '</div>';
  else if(d.needsConfirm) h += '<div class="msg warn">' + esc(d.message) + '</div>';
  else if(d.executed){
    h += '<div class="label" style="margin-top:10px">Output</div><pre>' + esc(d.output || '(no output)') + '</pre>';
    h += '<div class="msg ok">' + esc(d.message) + '</div>';
  }
  h += '</div>';
  resultEl.innerHTML = h;
}
async function loadList(){
  try{
    const r = await fetch('/api/list');
    const arr = await r.json();
    document.getElementById('list').innerHTML = arr.map(e =>
      '<div class="cmd-item"><b>' + esc(e.keywords.join(', ')) + '</b><br>' + esc(e.command) + '</div>'
    ).join('');
  }catch(e){}
}
loadList();
async function loadStatus(){
  try{
    const r = await fetch('/api/status');
    const d = await r.json();
    const el = document.getElementById('mode');
    if(d.admin){ el.textContent = 'Administrator'; el.className = 'badge admin'; }
    else { el.textContent = 'Restricted'; el.className = 'badge restricted'; }
  }catch(e){}
}
loadStatus();
async function quit(){
  try{ await fetch('/api/quit'); }catch(e){}
  document.body.innerHTML = '<div class="wrap"><div class="card ok">IT Assistant stopped. You can close this window.</div></div>';
}
</script>
</body>
</html>)PAGE";

static void sendResponse(SOCKET c, const string& status, const string& ctype, const string& body)
{
    string resp = "HTTP/1.1 " + status + "\r\n";
    resp += "Content-Type: " + ctype + "\r\n";
    resp += "Content-Length: " + to_string((unsigned long)body.size()) + "\r\n";
    resp += "Cache-Control: no-store\r\n";
    resp += "Connection: close\r\n\r\n";
    resp += body;
    send(c, resp.c_str(), (int)resp.size(), 0);
}

static string readRequest(SOCKET c)
{
    string data;
    char buf[4096];
    while (data.find("\r\n\r\n") == string::npos) {
        int n = recv(c, buf, sizeof(buf), 0);
        if (n <= 0) break;
        data.append(buf, n);
        if (data.size() > 65536) break;
    }
    return data;
}

static string requestTarget(const string& req)
{
    size_t sp1 = req.find(' ');
    if (sp1 == string::npos) return "/";
    size_t sp2 = req.find(' ', sp1 + 1);
    if (sp2 == string::npos) return "/";
    return req.substr(sp1 + 1, sp2 - (sp1 + 1));
}

static void handleClient(SOCKET c)
{
    string req = readRequest(c);
    string target = requestTarget(req);

    string path = target, query = "";
    size_t q = target.find('?');
    if (q != string::npos) {
        path = target.substr(0, q);
        query = target.substr(q + 1);
    }

    if (path == "/" || path == "/index.html") {
        sendResponse(c, "200 OK", "text/html; charset=utf-8", HTML_PAGE);
    } else if (path == "/api/run") {
        string task = getQueryParam(query, "task");
        bool confirmed = (getQueryParam(query, "confirm") == "1");
        sendResponse(c, "200 OK", "application/json; charset=utf-8", processTaskJson(task, confirmed));
    } else if (path == "/api/list") {
        sendResponse(c, "200 OK", "application/json; charset=utf-8", listJson());
    } else if (path == "/api/status") {
        string admin = isRunningAsAdmin() ? "true" : "false";
        sendResponse(c, "200 OK", "application/json; charset=utf-8", "{\"admin\":" + admin + "}");
    } else if (path == "/api/quit") {
        sendResponse(c, "200 OK", "application/json; charset=utf-8", "{\"ok\":true}");
        closesocket(c);
        ExitProcess(0);
    } else if (path == "/favicon.ico") {
        sendResponse(c, "204 No Content", "text/plain", "");
    } else {
        sendResponse(c, "404 Not Found", "text/plain", "Not found");
    }

    closesocket(c);
}

// Open the UI like a desktop app: Edge/Chrome in "app mode" (no tabs / address
// bar). Falls back to the default browser if neither is available.
static void openUI(const string& url)
{
    string appArg = "--app=" + url;
    HINSTANCE h = ShellExecuteA(NULL, "open", "msedge.exe", appArg.c_str(), NULL, SW_SHOWNORMAL);
    if ((INT_PTR)h <= 32) {
        h = ShellExecuteA(NULL, "open", "chrome.exe", appArg.c_str(), NULL, SW_SHOWNORMAL);
    }
    if ((INT_PTR)h <= 32) {
        ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}

int runWebServer()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Failed to initialize Winsock.\n";
        return 1;
    }

    SOCKET srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv == INVALID_SOCKET) {
        cout << "Failed to create socket.\n";
        WSACleanup();
        return 1;
    }

    BOOL reuse = TRUE;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));

    sockaddr_in addr = sockaddr_in();
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int port = 0;
    for (int p = 8755; p < 8775; ++p) {
        addr.sin_port = htons((u_short)p);
        if (bind(srv, (sockaddr*)&addr, sizeof(addr)) == 0) {
            port = p;
            break;
        }
    }
    if (port == 0) {
        cout << "Could not bind to a local port.\n";
        closesocket(srv);
        WSACleanup();
        return 1;
    }

    if (listen(srv, SOMAXCONN) != 0) {
        cout << "Listen failed.\n";
        closesocket(srv);
        WSACleanup();
        return 1;
    }

    string url = "http://127.0.0.1:" + to_string((unsigned long)port) + "/";
    cout << "=== IT Assistant ===\n";
    cout << "Web UI running at " << url << "\n";
    openUI(url);

    while (true) {
        SOCKET c = accept(srv, NULL, NULL);
        if (c == INVALID_SOCKET) continue;
        handleClient(c);
    }

    closesocket(srv);
    WSACleanup();
    return 0;
}

// Ask for Administrator rights at startup. If the user accepts, a new elevated
// instance is launched and this one exits. If the user declines (or it fails),
// we keep running in restricted mode (admin-only commands will be blocked).
static void maybeElevate(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i) {
        string a = argv[i];
        if (a == "--noelevate") return;   // already a child / attempt made
    }
    if (isRunningAsAdmin()) return;        // already elevated, nothing to do

    char path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) return;

    SHELLEXECUTEINFOA sei;
    ZeroMemory(&sei, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.lpVerb = "runas";                  // triggers the UAC prompt
    sei.lpFile = path;
    sei.lpParameters = "--noelevate";      // guard against an elevation loop
    sei.nShow = SW_SHOWNORMAL;

    if (ShellExecuteExA(&sei)) {
        ExitProcess(0);                    // elevated instance took over
    }
    // ShellExecuteExA failed (e.g. user clicked "No" -> ERROR_CANCELLED):
    // fall through and run this non-elevated instance in restricted mode.
}

int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);
    maybeElevate(argc, argv);

    for (int i = 1; i < argc; ++i) {
        string a = argv[i];
        if (a == "--console" || a == "-c") {
            AllocConsole();                 // GUI build has no console by default
            freopen("CONOUT$", "w", stdout);
            freopen("CONIN$", "r", stdin);
            SetConsoleOutputCP(CP_UTF8);
            return runConsole();
        }
    }

    return runWebServer();
}
