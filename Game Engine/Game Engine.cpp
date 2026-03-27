// Game Engine.cpp : Defines the entry point for the application.
//

#include "Scene.h"
#include "DXDevice.h"
#include "RenderGlobals.h"
#include "Input.h"
#include "Timer.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <windows.h>
#define _CRTDBG_MAP_ALLOC // To allow checking for memory leaks 

#include <crtdbg.h>

#include <string>
#include <memory>

// Global Variables:


// Forward declarations of functions included in this code module:
HWND InitWindow(HINSTANCE hInstance, int nCmdShow, int windowWidth, int windowHeight);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void UpdateWindowTitle(HWND hwnd, float frameTime);

// Global Variable to see if we're using mouse control for the camera
bool gMouseControl = false;

//============================================
// Entry Function for Windows Application
//============================================


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
                     [[maybe_unused]] _In_ LPWSTR lpCmdLine,
                     _In_ int       nCmdShow)
{
    int windowWidth = 1760;
    int windowHeight = 990;

    HWND hWnd = InitWindow(hInstance, nCmdShow, windowWidth, windowHeight);

    if (hWnd == NULL)
    {
        MessageBoxA(hWnd, "Failure Creating Window", NULL, MB_OK);
        return 0;
    }

    // Initialise Direct3D and the Scene
    std::unique_ptr<Scene> scene;

    try
    {
        DX = std::make_unique<DXDevice>(hWnd);
        scene = std::make_unique<Scene>();
    }
    catch (std::runtime_error e)
    {
        MessageBoxA(hWnd, e.what(), NULL, MB_OK);
        return 0;
    }

    // Prepare Input Functions
    InitInput();

    // ImGUI
    //====================
    // Initialise ImGui
    //====================
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Set up dark mode
    ImGui::StyleColorsDark();

    // Setup Platform / Render bindings
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(DX->Device(), DX->Context());

    // Register for mouse input
    RAWINPUTDEVICE Rid[1];
    Rid[0].usUsagePage = 0x01; // HID_USAGE_PAGE_GENERIC
    Rid[0].usUsage = 0x02; // HID_USAGE_GENERIC_MOUSe
    Rid[0].dwFlags = 0;
    Rid[0].hwndTarget = 0;
    if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) return 0;
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    gMouseControl = false;

    // Start a timer for fram timing
    Timer timer;
    timer.Start();

    // Main message loop:
    MSG msg = {};

    while (msg.message != WM_QUIT) // As long as window is open
    {
        // Check for and deal with any window messages (input, window resizing, minimizing, etc.)
        // Actual message process happens in function WndProc below
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // Deal with messages
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // When no window messages left ot process then render and update scene
        else
        {
            // Update scene by amount of time since last frame
            float frameTime = timer.GetLapTime();
            scene->Update(frameTime);

            // Draw the scene
            scene->Render(frameTime);

            UpdateWindowTitle(hWnd, frameTime);

            // Toggle mouse capture
            if (KeyHit(Key_F9))
                gMouseControl = !gMouseControl;

            // This will close the window and trigger WM_QUIT message that will cause this loop to end
            if (KeyHit(Key_Escape))
                DestroyWindow(hWnd);
        }
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // Output any memory leaks to Visual Studios output window after app ends

    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_LEAK_CHECK_DF));

    return (int)msg.wParam;
}

//=========================================
// Create a window to display our scene
// returns false on failure
//=========================================
HWND InitWindow(HINSTANCE hInstance, int nCmdShow, int windowWidth, int windowHeight)
{
    // Get a stock icon to show on taskbar for this program
    SHSTOCKICONINFO stockIcon;
    stockIcon.cbSize = sizeof(stockIcon);

    if (SHGetStockIconInfo(SIID_APPLICATION, SHGSI_ICON, &stockIcon) != S_OK)
    {
        return NULL;
    }

    // Register window class. Defines various UI features of the window for the engine
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc; // Function to deal with window messages
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0; SIID_APPLICATION;
    wcex.hInstance = hInstance;
    wcex.hIcon = stockIcon.hIcon; // Which icon to use for the window
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // What cursor to use
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"Physics Engine";
    wcex.hIconSm = stockIcon.hIcon;

    if (!RegisterClassEx(&wcex))
    {
        return NULL;
    }

    // Select the type of window to show engine in
    DWORD windowStyle = WS_OVERLAPPEDWINDOW; // Standard Window

    // Calculate overall dimensions for the window. Will render inside of the window
    // Overall window will be larger if it include boarders, title bar etc. 
    // Code calculates overall size of window given for the chosen viewport size.
    RECT rc = { 0, 0, windowWidth, windowHeight };
    AdjustWindowRect(&rc, windowStyle, FALSE);

    // Create window, second parameter is the text that appears in the title bar
    HWND hwnd = CreateWindow(L"Physics Engine", L"Physics Engine", windowStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

    if (hwnd != NULL)
    {
        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);
    }

    return hwnd;
}

// Update window title with frame time / fps, averaged over past few frames
void UpdateWindowTitle(HWND hwnd, float frameTime)
{
    // Show frame time / FPS in window title
    const float fpsUpdateTime = 0.5f; // Time between window title updates
    static float totalFrameTime = 0;
    static int frameCount = 0;

    totalFrameTime += frameTime;
    frameCount++;

    if (totalFrameTime > fpsUpdateTime)
    {
        // Dispalys FPS rounded to nearest int, and frame time in milliseconds to 2dp
        gFPS = totalFrameTime / frameCount;

        std::ostringstream frameTimeMs;
        frameTimeMs.precision(2);
        frameTimeMs << std::fixed << gFPS * 1000;
        std::string windowTitle = "Physics Engine - Frame Time: " + frameTimeMs.str() +
            "ms, FPS: " + std::to_string(static_cast<int>(1 / gFPS + 0.5f));
        
        SetWindowTextA(hwnd, windowTitle.c_str());

        totalFrameTime = 0;
        frameCount = 0;
    }
}

//========================================================================================================
// Deal with a message from Windows. There are many possible mesages, such as keyboard or mouse inputs.
// resizing or minimizing windows, etc. Only deal with the messages were intrested in.
//========================================================================================================
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) // Passes user input to ImGUI
        return true;

    switch (message)
    {
    case WM_PAINT: // A message we must handle to ensure the window content is displayed
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY: // A message to deal with the window closing
    {
        PostQuitMessage(0);
        break;
    }

    // The WM_KEYXXXX messages report keyboard input to our window.
   // This application has added some simple functions (not DirectX) to process these messages (all in Input.cpp/h)
   // so you don't need to change this code. Instead simply use KeyHit, KeyHeld etc.
    case WM_KEYDOWN:
        KeyDownEvent(static_cast<KeyCode>(wParam));
        break;

    case WM_KEYUP:
        KeyUpEvent(static_cast<KeyCode>(wParam));
        break;


        // The following WM_XXXX messages report mouse movement and button presses
        // Use KeyHit to get mouse buttons, GetMouse for its position
    case WM_LBUTTONDOWN:
    {
        KeyDownEvent(Mouse_LButton);
        break;
    }
    case WM_LBUTTONUP:
    {
        KeyUpEvent(Mouse_LButton);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        KeyDownEvent(Mouse_RButton);
        break;
    }
    case WM_RBUTTONUP:
    {
        KeyUpEvent(Mouse_RButton);
        break;
    }
    case WM_MBUTTONDOWN:
    {
        KeyDownEvent(Mouse_MButton);
        break;
    }
    case WM_MBUTTONUP:
    {
        KeyUpEvent(Mouse_MButton);
        break;
    }
    // Using raw input for mouse movement so we can still get mouse movement even after the mouse cursor hits the edge of the scree
    case WM_INPUT:
    {

        UINT dwSize;
        RAWINPUTHEADER header;
        GetRawInputData((HRAWINPUT)lParam, RID_HEADER, &header, &dwSize, sizeof(RAWINPUTHEADER));
        if (header.dwType == RIM_TYPEMOUSE)
        {
            RAWINPUT rawInput;
            dwSize = sizeof(RAWINPUT);
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawInput, &dwSize, sizeof(RAWINPUTHEADER));
            if (gMouseControl)  MouseMoveEvent(rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY);
        }
        break;
    }

    // Any messages we don't handle are passed back to Windows default handling
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
    
}