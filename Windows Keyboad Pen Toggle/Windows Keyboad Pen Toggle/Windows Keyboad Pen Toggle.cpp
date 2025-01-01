#include <windows.h>
#include <iostream>

HHOOK hKeyboardHook; // Global variable to store the keyboard hook
bool isPenActive = false; // Tracks whether the pen is touching the screen

// Function to enable the keyboard hook
void EnableKeyboardHook() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
        if (isPenActive && nCode >= 0 && wParam == WM_KEYDOWN) {
            return 1; // Suppress the keyboard input if pen is active
        }
        return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
        }, NULL, 0);

    if (hKeyboardHook == NULL) {
        std::cerr << "Failed to install keyboard hook. Error: " << GetLastError() << std::endl;
    }
}

// Function to disable the keyboard hook
void DisableKeyboardHook() {
    if (hKeyboardHook != NULL) {
        UnhookWindowsHookEx(hKeyboardHook);
        hKeyboardHook = NULL;
    }
}

// Window procedure to detect pen input
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_POINTERDOWN: {
        UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
        POINTER_INPUT_TYPE pointerType;
        if (GetPointerType(pointerId, &pointerType) && pointerType == PT_PEN) {
            isPenActive = true; // Pen is touching the screen
            std::cout << "Pen detected: Disabling keyboard input." << std::endl;
        }
        break;
    }
    case WM_POINTERUP: {
        UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
        POINTER_INPUT_TYPE pointerType;
        if (GetPointerType(pointerId, &pointerType) && pointerType == PT_PEN) {
            isPenActive = false; // Pen is lifted
            std::cout << "Pen lifted: Enabling keyboard input." << std::endl;
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
    // Set up the keyboard hook
    EnableKeyboardHook();

    // Create a hidden window to monitor pen input
    const wchar_t CLASS_NAME[] = L"PenKeyboardToggle";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Pen Keyboard Toggle",
        0,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
        nullptr, nullptr, hInstance, nullptr
    );

    if (hWnd == nullptr) {
        return 0;
    }

    ShowWindow(hWnd, SW_HIDE); // Hide the window to run silently in the background

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DisableKeyboardHook(); // Cleanup before exiting

    return 0;
}
