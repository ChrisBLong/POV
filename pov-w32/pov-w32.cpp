// pov-w32.cpp : Defines the entry point for the application.
//

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "shlwapi.lib")

#include "framework.h"
#include "Shlwapi.h"
#include "pov-w32.h"
#include <algorithm>

#include "PixelDisplay.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hwndStatus;                                // The handle of the status bar.
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ConfigProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void updateStatusBar();
void updateColourSliders(HWND hDlg, PixelDisplay* pd, char which);
void updatePenFromSlidersRgb(HWND hDlg, PixelDisplay* pd);
//void updatePenFromSlidersHsl(HWND hDlg, PixelDisplay* pd);
void updatePenFromSlidersOKLab(HWND hDlg, PixelDisplay* pd);
void showConfigDialog(HWND hWnd);
HWND hwndConfig = NULL;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_POVW32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_POVW32));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
      if (!IsWindow(hwndConfig) || !IsDialogMessage(hwndConfig, &msg)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEXW wcex = { 0 };

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style          = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc    = WndProc;
  wcex.cbClsExtra     = 0;
  wcex.cbWndExtra     = 0;
  wcex.hInstance      = hInstance;
  wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_POVW32));
  wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_POVW32);
  wcex.lpszClassName  = szWindowClass;
  wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Ensure that the common control DLL is loaded.
   InitCommonControls();

   // Create the status bar.
   hwndStatus = CreateWindowEx(
     0,                       // no extended styles
     STATUSCLASSNAME,         // name of status bar class
     (PCTSTR)NULL,            // no text when first created
     SBARS_SIZEGRIP |         // includes a sizing grip
     WS_CHILD | WS_VISIBLE,   // creates a visible child window
     0, 0, 0, 0,              // ignores size and position
     hWnd,                    // handle to parent window
     (HMENU)123,              // child window identifier
     hInstance,               // handle to application instance
     NULL);                   // no window creation data

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

PixelDisplay* pd;
int m_width = 1920 / 1;
int m_height = 1080 / 1;

// For the vertical POV-Halloween video.
//int m_width = 1080 / 4;
//int m_height = 1920 / 4;

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
      pd = new PixelDisplay(hWnd, m_width, m_height, IDR_OBJECT_FILE);
      pd->reset();
      break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case ID_FILE_CONFIGURATION:
                showConfigDialog(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_KEYUP:
        {
          auto delta = pd->getTranslateDelta();
          switch (wParam) {
          case 'R':
            pd->stopAnimation();
            pd->reset();
            pd->resetLine();
            pd->resetCube();
            pd->invalidate();
            break;
          case 'L':
            pd->addRandomLines(1);
            break;
          case 'M':
            pd->addRandomLines(10);
            break;
          case 'S':
            pd->toggleAnimation();
            updateStatusBar();
            break;
          case VK_LEFT:
            pd->translateCube(-delta, 0, 0);
            break;
          case VK_RIGHT:
            pd->translateCube(delta, 0, 0);
            break;
          case VK_UP:
            pd->translateCube(0, -delta, 0);
            break;
          case VK_DOWN:
            pd->translateCube(0, delta, 0);
            break;
          case 'I':
            pd->translateCube(0, 0, -delta);
            break;
          case 'O':
            pd->translateCube(0, 0, delta);
            break;
          case 'E':
            pd->toggleEraseBackground();
            updateStatusBar();
            break;
          case 'C':
            pd->toggleCubeEnabled();
            updateStatusBar();
            break;
          case 'N':
            pd->toggleLineEnabled();
            updateStatusBar();
            break;
          case 'T':
            pd->toggleTextEnabled();
            updateStatusBar();
            break;
          case 'P':
            pd->nextPen();
            updateStatusBar();
            break;
          case 'F':
            showConfigDialog(hWnd);
            updateStatusBar();
            break;
          }
        }
        break;

    case WM_TIMER:
      if (pd) pd->nextFrame();
      break;

    case WM_SIZE:
      SendMessage(hwndStatus, WM_SIZE, 0, 0);
      break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // Copy the offscreen DC to our main window.
            pd->copyToDC(hdc, hwndStatus);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        delete pd;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void updateStatusBar() {
  SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)pd->getStatusMessage().c_str());
}

void updateColourSliders(HWND hDlg, PixelDisplay* pd, char which)
{
  // Get the color of the current pen.
  COLORREF col = pd->getCurrentPenColour();

  if (which == 'R' || which == 'A') {
    // Updating the RGB sliders is easy.
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_RED), TBM_SETPOS, TRUE, (LPARAM)GetRValue(col));
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_GREEN), TBM_SETPOS, TRUE, (LPARAM)GetGValue(col));
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_BLUE), TBM_SETPOS, TRUE, (LPARAM)GetBValue(col));
  }

  //// Get the HLS values for this colour.
  //WORD h, l, s;
  //ColorRGBToHLS(col, &h, &l, &s);

  //// Updating the HLS sliders is also easy.
  //SendMessage(GetDlgItem(hDlg, IDC_COLOUR_HUE), TBM_SETPOS, TRUE, (LPARAM)h);
  //SendMessage(GetDlgItem(hDlg, IDC_COLOUR_SAT), TBM_SETPOS, TRUE, (LPARAM)s);
  //SendMessage(GetDlgItem(hDlg, IDC_COLOUR_LUM), TBM_SETPOS, TRUE, (LPARAM)l);

  if (which == 'O' || which == 'A') {
    // Get the OKLab values for this colour.
    float red = GetRValue(col) / 255.0f;
    float green = GetGValue(col) / 255.0f;
    float blue = GetBValue(col) / 255.0f;

    csRGB rgb{ red, green, blue };
    csLab lab = PixelDisplay::srgb_to_oklab(rgb);

    // Convert the Lab values to fit the scaling of the sliders.
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_HUE), TBM_SETPOS, TRUE, (LPARAM)(lab.L * 100.0));
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_SAT), TBM_SETPOS, TRUE, (LPARAM)(lab.a * 100.0) + 40);
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_LUM), TBM_SETPOS, TRUE, (LPARAM)(lab.b * 100.0) + 40);
  }

}

void updatePenFromSlidersRgb(HWND hDlg, PixelDisplay* pd)
{
  auto red = SendMessage(GetDlgItem(hDlg, IDC_COLOUR_RED), TBM_GETPOS, 0, 0);
  auto green = SendMessage(GetDlgItem(hDlg, IDC_COLOUR_GREEN), TBM_GETPOS, 0, 0);
  auto blue = SendMessage(GetDlgItem(hDlg, IDC_COLOUR_BLUE), TBM_GETPOS, 0, 0);
  auto colour = RGB(red, green, blue);
  pd->setCurrentPenColour(colour);
}

//void updatePenFromSlidersHsl(HWND hDlg, PixelDisplay* pd)
//{
//  auto hue = (WORD)SendMessage(GetDlgItem(hDlg, IDC_COLOUR_HUE), TBM_GETPOS, 0, 0);
//  auto sat = (WORD)SendMessage(GetDlgItem(hDlg, IDC_COLOUR_SAT), TBM_GETPOS, 0, 0);
//  auto lum = (WORD)SendMessage(GetDlgItem(hDlg, IDC_COLOUR_LUM), TBM_GETPOS, 0, 0);
//  auto colour = ColorHLSToRGB(hue, lum, sat);
//  pd->setCurrentPenColour(colour);
//}

void updatePenFromSlidersOKLab(HWND hDlg, PixelDisplay* pd)
{
  auto raw_L = (WORD)SendMessage(GetDlgItem(hDlg, IDC_COLOUR_OK_L), TBM_GETPOS, 0, 0);
  auto raw_a = (WORD)SendMessage(GetDlgItem(hDlg, IDC_COLOUR_OK_A), TBM_GETPOS, 0, 0);
  auto raw_b = (WORD)SendMessage(GetDlgItem(hDlg, IDC_COLOUR_OK_B), TBM_GETPOS, 0, 0);
  float L = raw_L / 100.0f;
  float a = (raw_a - 40) / 100.0f;
  float b = (raw_b - 40) / 100.0f;
  csLab ok { L, a, b };
  csRGB rgb = PixelDisplay::oklab_to_srgb(ok);
  pd->writeLabValue(ok);
  pd->writeRgbValue(rgb);
  pd->setCurrentPenColour(RGB(rgb.r*255, rgb.g*255, rgb.b*255));
}

void showConfigDialog(HWND hWnd) {
  if (!IsWindow(hwndConfig))
  {
    hwndConfig = CreateDialog(NULL, MAKEINTRESOURCE(IDD_CONFIGURATION), hWnd, (DLGPROC)ConfigProc);
    ShowWindow(hwndConfig, SW_SHOW);
  }
}

std::vector<HWND> colourSlidersRGB;
//std::vector<HWND> colourSlidersHSL;
std::vector<HWND> colourSlidersOKLab;

// Message handler for the configuration dialog.
INT_PTR CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    // Set up the sliders.
    SendMessage(GetDlgItem(hDlg, IDC_RESOLUTION), TBM_SETRANGE, TRUE, MAKELPARAM(1, 20));
    SendMessage(GetDlgItem(hDlg, IDC_RESOLUTION), TBM_SETTICFREQ, 4, 0);
    SendMessage(GetDlgItem(hDlg, IDC_RESOLUTION), TBM_SETPOS, TRUE, (LPARAM)pd->getPixelRatio());

    SendMessage(GetDlgItem(hDlg, IDC_FRAME_RATE), TBM_SETRANGE, FALSE, MAKELPARAM(1, 60));
    SendMessage(GetDlgItem(hDlg, IDC_FRAME_RATE), TBM_SETTICFREQ, 3, 0);
    SendMessage(GetDlgItem(hDlg, IDC_FRAME_RATE), TBM_SETPOS, TRUE, (LPARAM)pd->getFrameRate());

    SendMessage(GetDlgItem(hDlg, IDC_LINE_WIDTH), TBM_SETRANGE, FALSE, MAKELPARAM(0, 30));
    SendMessage(GetDlgItem(hDlg, IDC_LINE_WIDTH), TBM_SETTICFREQ, 4, 0);
    SendMessage(GetDlgItem(hDlg, IDC_LINE_WIDTH), TBM_SETPOS, TRUE, (LPARAM)pd->getLineWidth());

    // Set up the check boxes.
    Button_SetCheck(GetDlgItem(hDlg, IDC_ERASE_BACKGROUND), pd->getEraseBackground());
    Button_SetCheck(GetDlgItem(hDlg, IDC_SHOW_CUBE), pd->getCubeEnabled());
    Button_SetCheck(GetDlgItem(hDlg, IDC_SHOW_LINE), pd->getLineEnabled());
    Button_SetCheck(GetDlgItem(hDlg, IDC_SHOW_TEXT), pd->getTextEnabled());
    Button_SetCheck(GetDlgItem(hDlg, IDC_FILL_TEXT), pd->getTextFill());
    Button_SetCheck(GetDlgItem(hDlg, IDC_INTEGRAL), pd->getIntegralRotation());
    Button_SetCheck(GetDlgItem(hDlg, IDC_WIREFRAME), pd->getWireframe());

    // Populate the text box.
    Edit_SetText(GetDlgItem(hDlg, IDC_TEXT_STRING), pd->getTextString());

    // Populate the object list box.
    for (const auto& o : pd->getObjectList()) {
      ComboBox_AddString(GetDlgItem(hDlg, IDC_OBJECT_LIST), o.objectName().c_str());
    }

    // Set the current selection.
    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_OBJECT_LIST), pd->getCurrentObjectIndex());

    // Set up the rotation speed slider.
    SendMessage(GetDlgItem(hDlg, IDC_ROTATION_SPEED), TBM_SETRANGE, TRUE, MAKELPARAM(5, 30));
    SendMessage(GetDlgItem(hDlg, IDC_ROTATION_SPEED), TBM_SETTICFREQ, 5, 0);
    SendMessage(GetDlgItem(hDlg, IDC_ROTATION_SPEED), TBM_SETPOS, TRUE, (LPARAM)(pd->getRotationSpeed() * 10));

    // Set up the colour sliders.
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_RED), TBM_SETRANGE, FALSE, MAKELPARAM(0, 255));
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_RED), TBM_SETTICFREQ, 16, 0);

    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_GREEN), TBM_SETRANGE, FALSE, MAKELPARAM(0, 255));
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_GREEN), TBM_SETTICFREQ, 16, 0);

    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_BLUE), TBM_SETRANGE, FALSE, MAKELPARAM(0, 255));
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_BLUE), TBM_SETTICFREQ, 16, 0);

    // OKLab lightness varies between 0 and 1.
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_OK_L), TBM_SETRANGE, FALSE, MAKELPARAM(0, 100));
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_OK_L), TBM_SETTICFREQ, 10, 0);

    // OKLab a varies between -0.4 and +0.4 (I think).
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_OK_A), TBM_SETRANGE, FALSE, MAKELPARAM(0, 80));
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_OK_A), TBM_SETTICFREQ, 8, 0);

    // OKLab b varies between -0.4 and +0.4 (I think).
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_OK_B), TBM_SETRANGE, FALSE, MAKELPARAM(0, 80));
    SendMessage(GetDlgItem(hDlg, IDC_COLOUR_OK_B), TBM_SETTICFREQ, 8, 0);


    // Populate the list of HWNDs that identify the six colour sliders.
    colourSlidersRGB.push_back(GetDlgItem(hDlg, IDC_COLOUR_RED));
    colourSlidersRGB.push_back(GetDlgItem(hDlg, IDC_COLOUR_GREEN));
    colourSlidersRGB.push_back(GetDlgItem(hDlg, IDC_COLOUR_BLUE));

    colourSlidersOKLab.push_back(GetDlgItem(hDlg, IDC_COLOUR_OK_L));
    colourSlidersOKLab.push_back(GetDlgItem(hDlg, IDC_COLOUR_OK_A));
    colourSlidersOKLab.push_back(GetDlgItem(hDlg, IDC_COLOUR_OK_B));

    // Set up the sliders for the current pen.
    updateColourSliders(hDlg, pd, 'A');

    return (INT_PTR)TRUE;

  case WM_WINDOWPOSCHANGED:
    // Update the colour swatch. This is done here because this message is sent AFTER the dialog is
    // shown on screen. Calling fillWindow from WM_INITDIALOG doesn't work because the dialog
    // hasn't been shown at that point.
    pd->fillWindowWithCurrentPen(hDlg, GetDlgItem(hDlg, IDC_LINE_COLOUR));
    return (INT_PTR)TRUE;

  case WM_HSCROLL:
    // Which slider sent this?
    if (lParam == (LPARAM)GetDlgItem(hDlg, IDC_RESOLUTION)) {
      int newRatio = SendMessage(GetDlgItem(hDlg, IDC_RESOLUTION), TBM_GETPOS, 0, 0);
      pd->resizeOffscreenDCs(newRatio);
      pd->fillWindowWithCurrentPen(hDlg, GetDlgItem(hDlg, IDC_LINE_COLOUR));
    }
    if (lParam == (LPARAM)GetDlgItem(hDlg, IDC_FRAME_RATE)) {
      int newRate = SendMessage(GetDlgItem(hDlg, IDC_FRAME_RATE), TBM_GETPOS, 0, 0);
      pd->setFrameRate(newRate);
    }
    if (lParam == (LPARAM)GetDlgItem(hDlg, IDC_LINE_WIDTH)) {
      int newWidth = SendMessage(GetDlgItem(hDlg, IDC_LINE_WIDTH), TBM_GETPOS, 0, 0);
      pd->setLineWidth(newWidth);
    }
    if (lParam == (LPARAM)GetDlgItem(hDlg, IDC_ROTATION_SPEED)) {
      int newSpeed = SendMessage(GetDlgItem(hDlg, IDC_ROTATION_SPEED), TBM_GETPOS, 0, 0);
      pd->setRotationSpeed(0.1 * newSpeed);
    }
    if (std::find(colourSlidersRGB.begin(), colourSlidersRGB.end(), (HWND)lParam) != colourSlidersRGB.end()) {
      updatePenFromSlidersRgb(hDlg, pd);
      pd->fillWindowWithCurrentPen(hDlg, GetDlgItem(hDlg, IDC_LINE_COLOUR));
      // Update the OKLab sliders.
      updateColourSliders(hDlg, pd, 'O');
    }
    if (std::find(colourSlidersOKLab.begin(), colourSlidersOKLab.end(), (HWND)lParam) != colourSlidersOKLab.end()) {
      updatePenFromSlidersOKLab(hDlg, pd);
      pd->fillWindowWithCurrentPen(hDlg, GetDlgItem(hDlg, IDC_LINE_COLOUR));
      // Update the RGB sliders.
      updateColourSliders(hDlg, pd, 'R');
    }
    updateStatusBar();
    break;

  case WM_COMMAND:
    if (HIWORD(wParam) == BN_CLICKED) {
      if (LOWORD(wParam) == IDC_ERASE_BACKGROUND) {
        pd->setEraseBackground(Button_GetCheck(GetDlgItem(hDlg, IDC_ERASE_BACKGROUND)));
        updateStatusBar();
        return (INT_PTR)TRUE;
      }
      if (LOWORD(wParam) == IDC_SHOW_CUBE) {
        pd->enableCube(Button_GetCheck(GetDlgItem(hDlg, IDC_SHOW_CUBE)));
        updateStatusBar();
        return (INT_PTR)TRUE;
      }
      if (LOWORD(wParam) == IDC_SHOW_LINE) {
        pd->enableLine(Button_GetCheck(GetDlgItem(hDlg, IDC_SHOW_LINE)));
        updateStatusBar();
        return (INT_PTR)TRUE;
      }
      if (LOWORD(wParam) == IDC_SHOW_TEXT) {
        pd->enableText(Button_GetCheck(GetDlgItem(hDlg, IDC_SHOW_TEXT)));
        updateStatusBar();
        return (INT_PTR)TRUE;
      }
      if (LOWORD(wParam) == IDC_FILL_TEXT) {
        pd->setTextFill(Button_GetCheck(GetDlgItem(hDlg, IDC_FILL_TEXT)));
        updateStatusBar();
        return (INT_PTR)TRUE;
      }
      if (LOWORD(wParam) == IDC_INTEGRAL) {
        pd->setIntegralRotation(Button_GetCheck(GetDlgItem(hDlg, IDC_INTEGRAL)));
        updateStatusBar();
        return (INT_PTR)TRUE;
      }
      if (LOWORD(wParam) == IDC_WIREFRAME) {
        pd->setWireframe(Button_GetCheck(GetDlgItem(hDlg, IDC_WIREFRAME)));
        return (INT_PTR)TRUE;
      }
      if (LOWORD(wParam) == IDC_ONE_FRAME) {
        pd->stopAnimation();
        pd->nextFrame();
        return (INT_PTR)TRUE;
      }
    }

    if (HIWORD(wParam) == EN_CHANGE) {
      if (LOWORD(wParam) == IDC_TEXT_STRING) {
        wchar_t buffer[128];
        Edit_GetText(GetDlgItem(hDlg, IDC_TEXT_STRING), buffer, sizeof(buffer)/sizeof(wchar_t));
        pd->setTextString(buffer);
        return (INT_PTR)TRUE;
      }
    }
    
    if (HIWORD(wParam) == CBN_SELCHANGE) {
      if (LOWORD(wParam) == IDC_OBJECT_LIST) {
        auto index = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_OBJECT_LIST));
        pd->setCurrentObject(index);
        return (INT_PTR)TRUE;
      }
    }

    switch (LOWORD(wParam)) {
    case IDOK:
    case IDCANCEL:
      EndDialog(hDlg, LOWORD(wParam));
      // Clear the global handle variable to show that we're closing.
      hwndConfig = NULL;
      return (INT_PTR)TRUE;
    case IDC_RESET:
      pd->reset();
      updateStatusBar();
      return (INT_PTR)TRUE;
    case IDC_TEST_PATTERN:
      pd->testPattern();
      updateStatusBar();
      return (INT_PTR)TRUE;
    case IDC_START:
      pd->startAnimation();
      updateStatusBar();
      return (INT_PTR)TRUE;
    case IDC_STOP:
      pd->stopAnimation();
      updateStatusBar();
      return (INT_PTR)TRUE;
    case IDC_TOGGLE_ANIMATION:
      pd->toggleAnimation();
      updateStatusBar();
      return (INT_PTR)TRUE;
    case IDC_NEXT_COLOUR:
      pd->nextPen();
      // Update the dialog.
      pd->fillWindowWithCurrentPen(hDlg, GetDlgItem(hDlg, IDC_LINE_COLOUR));
      updateColourSliders(hDlg, pd, 'A');
      updateStatusBar();
      return (INT_PTR)TRUE;
    case IDC_PREV_COLOUR:
      pd->prevPen();
      // Update the dialog.
      pd->fillWindowWithCurrentPen(hDlg, GetDlgItem(hDlg, IDC_LINE_COLOUR));
      updateColourSliders(hDlg, pd, 'A');
      updateStatusBar();
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

