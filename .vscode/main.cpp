#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include "json.hpp"
using namespace std;

using json = nlohmann::json;

const std::string LLMA_PATH = "llama\\llama-cli.exe ";
const std::string MODEL_PATH = "llma\\qwen2.5-1.5b-instruct-q4_k_m.gguf";

bool isDange(const string& command)
{
    vector<string> dangerous = { "remove-item", "rm -", "rd /s",
        "format-",     "clear-disk",
        "stop-computer", "restart-computer", "shutdown",
        "reg delete",  "remove-itemproperty",
        "set-executionpolicy unrestricted",
        "add-localgroupmember",
        "net user",    "net localgroup",
        "invoke-expression", "iex ",
        "downloadstring", "downloadfile",
        "hklm:\\system", "hklm:\\sam"};


string cmdLower = comand;
transform(cmdLower.begin(), cmdLower.end(), cmdLower.begin(), ::tolower);

for (const auto& d : dangerous) {
    if (cmdLower.find(d) != string::npos) {
        cout<< "Dangerous command detected: " << pattern << endl;
        return false;
    }
}   
return true;
}

string runPowerShell(const string& command)
{
    string fullCmd= "powershell.exe -NoProfile -Command \"" + command + "\" 2>&1";
    FILE* pipe= _popen(fullCmd.c_str(), "r");
    if (!pipe) return "Eror at opening PowerShell";

    string result;
    char buffer[256];
    while(fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}

string askLlama(const strin& userTask)
{
    string prompt = "You are a Windows IT assistant. "
        "Respond ONLY with valid JSON like this: "
        "{\\\"steps\\\": [\\\"step1\\\", \\\"step2\\\"], "
        "\\\"command\\\": \\\"powershell command\\\"}. "
        "No text outside JSON. No destructive commands. "
        "Task: " + userTask;

        string cmd = LLMA_Path +" " + "-m " + MODEL_PATH + " " "--no-display-prompt " + "-n 256" + "--log-disable " + "-p \" + prompt + "\" 2>nul";
        if (!pipe) return "";

        string result;
        char buffer[256];
        while(fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        _pclose(pipe);
        return result;
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    cout <<"=== IT Asistant ===\n\n";
    
    while (true)
    {
        cout<< "Your task: ";
        string input;
        getline(cin, input);

        if(input == "exit" || input == "quit") break;
        if(input.empty()) continue;

        cout<< "Generating solution...\n";
        string raw = askLlama(input);

        size_t start = raw.find('{');
        size_t end = raw.rfind('}');
        if (start == string::npos || end == string::npos)
        {
            cout<< "Error: Invalid response format.\n";
            continue;
        }
    }

    json parsed;
    try {
        parsed = json::parse(raw.substr(start, end - start +1));

    }catch (...){
        cout<< "Error at parsing JSON.\n";
        continue;
    }
<
    cout<< "\nSteps:\n";
    for(const auto& step : parse["steps"])
    {
        cout<< " - " << step.get<string>() << endl;
    }

    string command = parsed["command"].get<string>();
    cout << "\nCommand: " << command << endl;

    if (!isSafe(command))
    {
        cout << "\nCommand BLOCKED. \n\n";
        continue;
    }

    cout << "\nIs executing...\n";
    string output = runPowerShell(command);
    cout << "------------------\n\n";

    return 0;
}
