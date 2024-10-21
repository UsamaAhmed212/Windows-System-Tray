#include <windows.h>
#include <shellapi.h>
#include "resource.h"  // Include custom resources
#include <stdio.h>

// Global variables for tray icon and images
NOTIFYICONDATA nid;    // Structure for the notification icon
HICON currentIcon;     // Current icon displayed in the tray
HICON banglaIcon;      // Bangla keyboard icon
HBITMAP exitBitmap;    // Bitmap for the exit menu item


// Function to toggle the tray icon between two states
void ToggleIcon() {
    HINSTANCE hInstance = GetModuleHandle(NULL); // Get the handle of the current module

    // Check current icon and toggle
    if (currentIcon == (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_keyboard))) {
        currentIcon = banglaIcon; // Switch to Bangla icon
    } else {
        currentIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_keyboard)); // Switch back to the default icon
    }

    nid.hIcon = currentIcon; // Update the icon in the NOTIFYICONDATA structure
    Shell_NotifyIcon(NIM_MODIFY, &nid); // Modify the tray icon
}


// Function to create the context menu for the tray icon
HMENU CreateContextMenu() {
    HMENU hMenu = CreatePopupMenu(); // Create a new popup menu

    // Append menu items with identifiers and text
    AppendMenu(hMenu, MF_STRING, 1, "Open Item 1"); // Item 1
    AppendMenu(hMenu, MF_STRING, 2, "Open Item 2"); // Item 2
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // Separator line
    AppendMenu(hMenu, MF_STRING, 3, "Exit"); // Exit item

    // Set up menu item information for icons
    MENUITEMINFO mii = { 0 }; // Initialize MENUITEMINFO structure
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_BITMAP | MIIM_ID; // Specify bitmap and ID

    // Set the exit bitmap for menu items
    mii.wID = 1;  // ID for Open Item 1
    mii.hbmpItem = exitBitmap; // Set the bitmap for Item 1
    SetMenuItemInfo(hMenu, 1, FALSE, &mii);

    mii.wID = 2;  // ID for Open Item 2
    mii.hbmpItem = exitBitmap; // Set the bitmap for Item 2
    SetMenuItemInfo(hMenu, 2, FALSE, &mii);

    mii.wID = 3;  // ID for the Exit item
    mii.hbmpItem = exitBitmap; // Set the bitmap for the Exit item
    SetMenuItemInfo(hMenu, 3, FALSE, &mii);

    return hMenu; // Return the created menu
}


// Window Procedure to handle messages for the application window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_APP:
            // Handle right-click to show the context menu
            if (lParam == WM_RBUTTONUP) {
                POINT cursorPos;
                GetCursorPos(&cursorPos); // Get current mouse cursor position

                HMENU hMenu = CreateContextMenu(); // Create the context menu
                SetForegroundWindow(hwnd); // Bring the window to the foreground
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursorPos.x, cursorPos.y, 0, hwnd, NULL); // Show the menu
                DestroyMenu(hMenu); // Clean up the menu after use
            } else if (lParam == WM_LBUTTONUP) {
                ToggleIcon(); // Toggle the tray icon on left-click
            }
            break;
        case WM_COMMAND:
            // Handle menu item commands
            switch (LOWORD(wParam)) {
                case 1:
                    MessageBox(NULL, "You clicked Open Item 1!", "Info", MB_OK); // Info message for Item 1
                    break;
                case 2:
                    MessageBox(NULL, "You clicked Open Item 2!", "Info", MB_OK); // Info message for Item 2
                    break;
                case 3:
                    Shell_NotifyIcon(NIM_DELETE, &nid); // Remove the icon from the tray
                    PostQuitMessage(0); // Quit the application
                    break;
            }
            break;
        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &nid); // Clean up icon on window destroy
            PostQuitMessage(0); // Exit the application
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam); // Default window procedure
    }
    return 0; // Return 0 to indicate message handled
}



int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL); // Get the instance handle of the current application
    
    // Load icons for the tray application
    currentIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_keyboard));
    banglaIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_bangla_keyboard));
    
    // Load bitmap for exit menu item
    exitBitmap = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_EXIT_BUTTON), IMAGE_BITMAP, 15, 15, LR_LOADTRANSPARENT);
  
    // Register window class
    WNDCLASS wc = {0}; // Initialize WNDCLASS structure
    wc.lpfnWndProc = WindowProc; // Set window procedure
    wc.hInstance = hInstance; // Set application instance
    wc.lpszClassName = "TrayApp"; // Set class name
    RegisterClass(&wc); // Register the window class
    
    // Create the application window
    HWND hwnd = CreateWindow("TrayApp", "Tray Application", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, NULL, hInstance, NULL);
    
    // Set up notification icon data
    nid.cbSize = sizeof(NOTIFYICONDATA); // Set size of NOTIFYICONDATA
    nid.hWnd = hwnd; // Set the window handle
    nid.uID = 1; // Set icon ID
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; // Set icon, message, and tooltip
    nid.hIcon = currentIcon; // Set the current icon
    nid.uCallbackMessage = WM_APP; // Set callback message
    strcpy(nid.szTip, "Keyboard Application"); // Set tooltip text
    
    Shell_NotifyIcon(NIM_ADD, &nid); // Add the icon to the system tray
    ShowWindow(hwnd, SW_HIDE); // Hide the main window

    // Message loop for processing messages
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Translate keyboard messages
        DispatchMessage(&msg); // Dispatch messages to the window procedure
    }

    // Clean up resources before exiting
    DestroyIcon(currentIcon); // Destroy current icon
    DestroyIcon(banglaIcon); // Destroy Bangla icon
    DeleteObject(exitBitmap); // Delete exit bitmap

    return 0; // Return 0 to indicate successful execution
}
