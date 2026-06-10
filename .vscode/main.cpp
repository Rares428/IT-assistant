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
