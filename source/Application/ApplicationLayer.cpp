#include "applicationlayer.h"

#define IDI_ICON  101

ApplicationLayer::ApplicationLayer() : 
    
    windowHandle(NULL),
    deviceContext(NULL),
    
    displayWidth(1024),
    displayHeight(800),
    
    isPaused(false),
    isActive(true),
    
    mIsWindowRunning(false)
{
}

void ApplicationLayer::Pause(void) {
    isPaused = !isPaused;
    return;
}

HWND ApplicationLayer::CreateWindowHandle(std::string className, std::string windowName, HWND parentHandle) {
    assert(windowHandle == NULL);
    
    isPaused = false;
    isActive = true;
    
    wClassEx.lpszClassName   = className.c_str();
    wClassEx.cbSize          = sizeof(WNDCLASSEX);
    wClassEx.style           = CS_OWNDC;
    wClassEx.lpfnWndProc     = WindowProc;
    wClassEx.cbClsExtra      = 0;
    wClassEx.cbWndExtra      = 0;
    wClassEx.hInstance       = hInstance;
    wClassEx.lpszMenuName    = NULL;
    wClassEx.hCursor         = LoadCursor(NULL, IDC_ARROW);
    wClassEx.hIcon           = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wClassEx.hIconSm         = LoadIcon(hInstance, IDI_APPLICATION);
    wClassEx.hbrBackground   = (HBRUSH)GetStockObject(BLACK_BRUSH);
    
    assert( RegisterClassEx(&wClassEx) );
    
    windowHandle = CreateWindowEx(0, className.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parentHandle, NULL, hInstance, NULL);
    ShowWindow(windowHandle, true);
    assert(windowHandle != NULL);
    
    deviceContext = GetDC(windowHandle);
    
    displayWidth  = GetDeviceCaps(deviceContext, HORZRES);
    displayHeight = GetDeviceCaps(deviceContext, VERTRES);
    
    mIsWindowRunning = true;
    return windowHandle;
}

void ApplicationLayer::DestroyWindowHandle(void) {
    assert(windowHandle != NULL);
    DestroyWindow(windowHandle);
    windowHandle = NULL;
    mIsWindowRunning = false;
    return;
}

void ApplicationLayer::SetWindowCenter(void) {
    
    int DisplayWidth  = GetDeviceCaps(deviceContext, HORZRES);
    int DisplayHeight = GetDeviceCaps(deviceContext, VERTRES);
    
    RECT windowSz;
    GetWindowRect(windowHandle, &windowSz);
    
    Viewport WindowSz;
    WindowSz.w = windowSz.right  - windowSz.left;
    WindowSz.h = windowSz.bottom - windowSz.top;
    
    WindowSz.x = (DisplayWidth  / 2) - (WindowSz.w / 2);
    WindowSz.y = (DisplayHeight / 2) - (WindowSz.h / 2);
    
    SetWindowPos(windowHandle, NULL, WindowSz.x, WindowSz.y, WindowSz.w, WindowSz.h, SWP_SHOWWINDOW);
    return;
}

void ApplicationLayer::SetWindowCenterScale(float width, float height) {
    
    RECT windowSz;
    GetWindowRect(windowHandle, &windowSz);
    
    Viewport newWindowSz;
    newWindowSz.x = windowSz.left;
    newWindowSz.y = windowSz.top;
    newWindowSz.w = (windowSz.right  - windowSz.left) * width;
    newWindowSz.h = (windowSz.bottom - windowSz.top)  * height;
    
    SetWindowPosition(newWindowSz);
    SetWindowCenter();
    return;
}

void ApplicationLayer::SetWindowPosition(Viewport windowSize) {
    assert(windowHandle != NULL);
    SetWindowPos(windowHandle, NULL, windowSize.x, windowSize.y, windowSize.w, windowSize.h, SWP_SHOWWINDOW);
    return;
}

Viewport ApplicationLayer::GetWindowArea(void) {
    RECT windowSz;
    GetWindowRect(windowHandle, &windowSz);
    
    Viewport area;
    area.x = windowSz.left;
    area.y = windowSz.top;
    area.w = (windowSz.right  - windowSz.left);
    area.h = (windowSz.bottom - windowSz.top);
    
    return area;
}

void ApplicationLayer::HideWindowHandle(void) {
    assert(windowHandle != NULL);
    ShowWindow(windowHandle, true);
    return;
}

void ApplicationLayer::ShowWindowHandle(void) {
    assert(windowHandle != NULL);
    ShowWindow(windowHandle, false);
    return;
}
