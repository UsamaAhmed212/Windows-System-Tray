#include <windows.h>
#include <shellapi.h>
#include "resource.h"  // Include your resource header
#include <stdio.h>

NOTIFYICONDATA nid;
HICON currentIcon;  // Change to HICON type
HICON banglaIcon;   // New icon for Bangla keyboard

// Window Procedure to handle messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_APP:
            if (lParam == WM_RBUTTONUP) {
                // Handle right-click on the tray icon
                MessageBox(NULL, "Right-click on the tray icon!", "Info", MB_OK);
            } else if (lParam == WM_LBUTTONUP) {
                // Handle single left-click on the tray icon and toggle the icon
                if (currentIcon == (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_keyboard))) {
                    // Switch to Bangla keyboard icon
                    currentIcon = banglaIcon; 
                } else {
                    // Switch back to default icon
                    currentIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_keyboard)); 
                }

                // Update the tray icon
                nid.hIcon = currentIcon;
                Shell_NotifyIcon(NIM_MODIFY, &nid); // Update the tray icon
            }
            break;
        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &nid); // Remove the icon
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}



int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    
    // Load the icons
    currentIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_keyboard)); // Load default icon
    banglaIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_bangla_keyboard)); // Load Bangla keyboard icon

    // Register the window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "TrayApp";
    RegisterClass(&wc);
    
    // Create the hidden window
    HWND hwnd = CreateWindow("TrayApp", "Tray Application", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, NULL, hInstance, NULL);
    
    // Set up the tray icon
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.hIcon = currentIcon; // Set initial icon
    nid.uCallbackMessage = WM_APP;
    strcpy(nid.szTip, "Keyboard Application"); // Tooltip text
    
    Shell_NotifyIcon(NIM_ADD, &nid); // Add the icon to the tray
    
    // Hide the window
    ShowWindow(hwnd, SW_HIDE);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up
    DestroyIcon(currentIcon);
    DestroyIcon(banglaIcon);

    return 0;
}
