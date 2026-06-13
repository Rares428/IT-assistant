IT Asistant:
-A program that understands text and speech commands and executes them using powershell
-Functions: -It executes commands for both admin and user (whith a restricted list of commands)
            -It has an interactive and user-friendly interface
            -It runs locally on 127.0.0.1 local host, so it can work without internet
            -It has safety checks for every command so the program won't destroy the system
            -If the prompt is altering with the OS state, the user has to confirm the command
-Requirements: -Windows 10/11
               -MinGW (g++) - I got it from Code Blocks files
               - A compatible browser (Microsoft Edge or Google Chrome)
-Compilation in powershell: g++ main.cpp -o ITassistant.exe -std=gnu++11 -mwindows -static `-static-libgcc -static-libstdc++ -lws2_32 -lshell32
-How to use: doble click on the executable file(ITassistant.exe) or in the console "ITassistant.exe --console"
-Exampeles of tasks: - "open task manager"
                    - "deschide setări wifi"
                    - "turn brightness to maximum"
-Warning: -this program uses powershell comands that run directly on your os system and can ask for admin permissions. Run at your own risk. Use it only on your computer.        

## Demo Video
[Watch the demo](demo1.mp4)
