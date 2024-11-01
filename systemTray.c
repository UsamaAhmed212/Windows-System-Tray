// systemTray.c
#include <windows.h>
#include <shellapi.h>
#include "resource.h"  // Include custom resources
#include <stdio.h>

// Global variables for tray icon and images
NOTIFYICONDATA nid;    // Structure for the notification icon
HICON currentIcon;     // Current icon displayed in the tray
HICON banglaIcon;      // Bangla keyboard icon
HICON exitIcon;        // Icon for the exit menu item

// Enum for menu item identifiers
enum MenuItems {
    ITEM_OPEN_1, // Automatically assigned value 0
    ITEM_OPEN_2, // Automatically assigned value 1
    ITEM_EXIT    // Automatically assigned value 2
};

// Function to convert HICON to HBITMAP with specified width and height
HBITMAP IconToBitmap(HICON hIcon, int width, int height) {
    // Create a compatible DC
    HDC hDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hDC);
    
    // Create a compatible bitmap with the specified size
    HBITMAP hBitmap = CreateCompatibleBitmap(hDC, width, height);
    SelectObject(hMemDC, hBitmap);

    // Set the stretch mode to HALFTONE for better quality
    SetStretchBltMode(hMemDC, HALFTONE);

    // Clear the memory DC with a transparent background (optional)
    FillRect(hMemDC, &(RECT){0, 0, width, height}, (HBRUSH)(COLOR_WINDOW + 1)); // Fill with white or transparent

    // Get the original icon dimensions
    ICONINFO iconInfo;
    GetIconInfo(hIcon, &iconInfo);
    int originalWidth = iconInfo.xHotspot * 2; // width
    int originalHeight = iconInfo.yHotspot * 2; // height
    DeleteObject(iconInfo.hbmMask); // Clean up

    // Calculate the aspect ratio
    float aspectRatio = (float)originalWidth / (float)originalHeight;
    int newWidth, newHeight;

    // Maintain aspect ratio
    if (width / aspectRatio > height) {
        newHeight = height;
        newWidth = (int)(height * aspectRatio);
    } else {
        newWidth = width;
        newHeight = (int)(width / aspectRatio);
    }

    // Stretch the icon to the new size
    DrawIconEx(hMemDC, (width - newWidth) / 2, (height - newHeight) / 2, hIcon, newWidth, newHeight, 0, NULL, DI_NORMAL);

    // Cleanup
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);

    return hBitmap; // Return the resized bitmap
}


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

// In the CreateContextMenu function, update the bitmap creation
HMENU CreateContextMenu() {
    HMENU hMenu = CreatePopupMenu(); // Create a new popup menu

    // Append menu items with identifiers and text
    AppendMenu(hMenu, MF_STRING | MF_BYPOSITION, ITEM_OPEN_1, "Open Item 1"); // Item 1
    AppendMenu(hMenu, MF_STRING | MF_BYPOSITION, ITEM_OPEN_2, "Open Item 2"); // Item 2
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL); // Separator line
    AppendMenu(hMenu, MF_STRING | MF_BYPOSITION, ITEM_EXIT, "Exit"); // Exit item

    // Set up menu item information for icons
    MENUITEMINFO mii = { 0 }; // Initialize MENUITEMINFO structure
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_ID | MIIM_BITMAP; // Specify ID and Bitmap

    // Set the bitmap for the Exit item (16x16 size)
    mii.wID = ITEM_EXIT;  // ID for the Exit item
    mii.hbmpItem = IconToBitmap(exitIcon, 16, 16); // Convert exitIcon to HBITMAP with size 16x16
    SetMenuItemInfo(hMenu, ITEM_EXIT, FALSE, &mii); // Set the menu item info for Exit item

    // Set the bitmap for Item 1 (16x16 size)
    mii.wID = ITEM_OPEN_1; // ID for Item 1
    mii.hbmpItem = IconToBitmap(banglaIcon, 16, 16); // Convert banglaIcon to HBITMAP with size 16x16
    SetMenuItemInfo(hMenu, ITEM_OPEN_1, FALSE, &mii); // Set the menu item info for Item 1

    // Set the bitmap for Item 2 (16x16 size)
    mii.wID = ITEM_OPEN_2; // ID for Item 2
    mii.hbmpItem = IconToBitmap(banglaIcon, 16, 16); // Convert banglaIcon to HBITMAP with size 16x16
    SetMenuItemInfo(hMenu, ITEM_OPEN_2, FALSE, &mii); // Set the menu item info for Item 2

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
                case ITEM_OPEN_1:
                    MessageBox(NULL, "You clicked Open Item 1!", "Info", MB_OK); // Info message for Item 1
                    break;
                case ITEM_OPEN_2:
                    MessageBox(NULL, "You clicked Open Item 2!", "Info", MB_OK); // Info message for Item 2
                    break;
                case ITEM_EXIT:
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

    // Load the icon for the exit menu item
    exitIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXIT_ICON)); // Load exit icon


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
    ShowWindow(hwnd, SW_HIDE); // Hide the window

    // Message loop for the application
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Translate message
        DispatchMessage(&msg); // Dispatch message
    }

    // Cleanup icons before exiting
    DestroyIcon(currentIcon);
    DestroyIcon(banglaIcon);
    DestroyIcon(exitIcon);

    return (int)msg.wParam; // Return message parameter
}
