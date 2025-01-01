#include <windows.h>
#include <iostream>
#include <string>
#include <cstdlib>

// Path to devcon.exe
const std::string DEVCON_PATH = "C:\\Tools\\devcon.exe"; // Update this with the correct path to devcon.exe
const std::string KEYBOARD_HW_ID = "ACPI\\PNP0303";      // Replace with your keyboard's actual hardware ID

void ExecuteDevConCommand(const std::string& action) {
    std::string command = DEVCON_PATH + " " + action + " " + KEYBOARD_HW_ID;
    system(command.c_str());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_POINTERDOWN: {
        UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
        POINTER_INPUT_TYPE pointerType;
        if (GetPointerType(pointerId, &pointerType) && pointerType == PT_PEN) {
            // Pen detected: Disable the keyboard
            ExecuteDevConCommand("disable");
        }
        break;
    }
    case WM_POINTERUP: {
        UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
        POINTER_INPUT_TYPE pointerType;
        if (GetPointerType(pointerId, &pointerType) && pointerType == PT_PEN) {
            // Pen lifted: Enable the keyboard
            ExecuteDevConCommand("enable");
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"PenKeyboardToggle";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        L"Pen Keyboard Toggle",         // Window title (wide-character string)
        0,                              // No visible window (run in background)
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
        nullptr, nullptr, hInstance, nullptr);

    if (hWnd == nullptr) {
        return 0;
    }

    ShowWindow(hWnd, SW_HIDE); // Hide the window to run silently in the background

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

