#include <iostream>
#include <windows.h>
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
        if (token.size() < 2) continue;
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

string runPowerShell(const string& command)
{
    if (trim(command).empty()) {
        return "Error: empty command.\n\n";
    }

    string scriptPath = createTempFile("itp", command + "\n");
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


int main()
{
    SetConsoleOutputCP(CP_UTF8);
    cout << "=== IT Assistant ===\n";
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
