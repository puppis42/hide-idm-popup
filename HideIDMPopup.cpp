#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

void HideConsole() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_HIDE);
}

DWORD GetProcessIdByName(const wchar_t* processName) {
    DWORD processId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(PROCESSENTRY32W);
        if (Process32FirstW(hSnapshot, &pe)) {
            do {
                if (wcscmp(pe.szExeFile, processName) == 0) {
                    processId = pe.th32ProcessID;
                    break;
                }
            } while (Process32NextW(hSnapshot, &pe));
        }
        CloseHandle(hSnapshot);
    }
    return processId;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == lParam) {
        wchar_t className[80];
        GetClassNameW(hwnd, className, sizeof(className) / sizeof(wchar_t));
        if (wcscmp(className, L"#32770") == 0) {
            wchar_t windowTitle[256];
            GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));
            if (wcslen(windowTitle) == 0) {
                ShowWindow(hwnd, SW_HIDE);
                std::wcout << L"[+] Window hidden." << std::endl;
            }
            else {
                std::wcout << L"[+] Window title is not empty, not hiding." << std::endl;
            }
        }
    }
    return TRUE;
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    const wchar_t* processName = L"IDMan.exe";
    DWORD processId = GetProcessIdByName(processName);
    if (processId != 0) {
        EnumWindows(EnumWindowsProc, processId);
    }
    else {
        std::wcout << L"[-] Process not found!!!" << std::endl;
    }
}

int main() {
    HideConsole(); // Hide console

    // Set a timer to call TimerProc every 1000 milliseconds (1 second)
    UINT_PTR timerId = SetTimer(NULL, 0, 1000, (TIMERPROC)TimerProc);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    KillTimer(NULL, timerId);

    return 0;
}
