#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <string>
#include <vector>

volatile BOOL g_bRunning = TRUE;
CRITICAL_SECTION g_cs;

std::wstring GenerateRandomGibberish() {
    int len = 20 + rand() % 81;
    std::wstring result;
    for (int i = 0; i < len; ++i) {
        wchar_t ch = 32 + rand() % 95;
        result.push_back(ch);
    }
    return result;
}

unsigned int WINAPI RandomIconsThread(LPVOID lpParam) {
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int iconW = GetSystemMetrics(SM_CXICON);
    int iconH = GetSystemMetrics(SM_CYICON);
    HICON hIcons[] = {
        LoadIcon(NULL, IDI_ERROR),
        LoadIcon(NULL, IDI_WARNING),
        LoadIcon(NULL, IDI_INFORMATION),
        LoadIcon(NULL, IDI_QUESTION)
    };
    srand(GetTickCount());

    while (g_bRunning) {
        POINT pos[4];
        for (int i = 0; i < 4; ++i) {
            pos[i].x = rand() % (screenW - iconW + 1);
            pos[i].y = rand() % (screenH - iconH + 1);
        }
        EnterCriticalSection(&g_cs);
        HDC hdc = GetDC(NULL);
        if (hdc) {
            for (int i = 0; i < 4; ++i) {
                int idx = rand() % 4;
                DrawIcon(hdc, pos[i].x, pos[i].y, hIcons[idx]);
            }
            ReleaseDC(NULL, hdc);
        }
        LeaveCriticalSection(&g_cs);
        Sleep(80);
    }
    return 0;
}

unsigned int WINAPI InvertScreenThread(LPVOID lpParam) {
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    while (g_bRunning) {
        EnterCriticalSection(&g_cs);
        HDC hdc = GetDC(NULL);
        if (hdc) {
            BitBlt(hdc, 0, 0, w, h, hdc, 0, 0, NOTSRCCOPY);
            ReleaseDC(NULL, hdc);
        }
        LeaveCriticalSection(&g_cs);
        Sleep(200);
    }
    return 0;
}

unsigned int WINAPI RotateScreenThread(LPVOID lpParam) {
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    double angle = 0.0;
    const double angleStep = 0.05;

    HDC hdcMem = CreateCompatibleDC(NULL);
    HBITMAP hBmp = CreateCompatibleBitmap(GetDC(NULL), w, h);
    SelectObject(hdcMem, hBmp);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);

    while (g_bRunning) {
        HDC hdcScreen = GetDC(NULL);
        if (hdcScreen) {
            BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
            ReleaseDC(NULL, hdcScreen);
        }
        double rad = angle;
        double cosA = cos(rad);
        double sinA = sin(rad);
        int cx = w / 2;
        int cy = h / 2;
        int x1 = -cx, y1 = -cy;
        int x2 =  cx, y2 = -cy;
        int x3 = -cx, y3 =  cy;
        POINT dst[3];
        dst[0].x = (int)(cx + (x1 * cosA - y1 * sinA));
        dst[0].y = (int)(cy + (x1 * sinA + y1 * cosA));
        dst[1].x = (int)(cx + (x2 * cosA - y2 * sinA));
        dst[1].y = (int)(cy + (x2 * sinA + y2 * cosA));
        dst[2].x = (int)(cx + (x3 * cosA - y3 * sinA));
        dst[2].y = (int)(cy + (x3 * sinA + y3 * cosA));

        HDC hdcDest = GetDC(NULL);
        if (hdcDest) {
            PlgBlt(hdcDest, dst, hdcMem, 0, 0, w, h, 0, 0, 0);
            ReleaseDC(NULL, hdcDest);
        }

        angle += angleStep;
        if (angle > 2 * 3.1415926) angle -= 2 * 3.1415926;
        Sleep(50);
    }

    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    return 0;
}

unsigned int WINAPI TVStaticThread(LPVOID lpParam) {
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    srand(GetTickCount());
    while (g_bRunning) {
        EnterCriticalSection(&g_cs);
        HDC hdc = GetDC(NULL);
        if (hdc) {
            int dots = 2500;
            for (int i = 0; i < dots; ++i) {
                int x = rand() % w;
                int y = rand() % h;
                COLORREF color = (rand() % 2) ? RGB(255,255,255) : RGB(0,0,0);
                SetPixel(hdc, x, y, color);
            }
            ReleaseDC(NULL, hdc);
        }
        LeaveCriticalSection(&g_cs);
        Sleep(30);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    srand((unsigned int)time(NULL));
    InitializeCriticalSection(&g_cs);

    HANDLE hThreads[4];
    hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, RandomIconsThread, NULL, 0, NULL);
    hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, InvertScreenThread, NULL, 0, NULL);
    hThreads[2] = (HANDLE)_beginthreadex(NULL, 0, RotateScreenThread, NULL, 0, NULL);
    hThreads[3] = (HANDLE)_beginthreadex(NULL, 0, TVStaticThread, NULL, 0, NULL);

    Sleep(100);

    std::wstring gibberish = GenerateRandomGibberish();
    MessageBoxW(NULL, gibberish.c_str(), L"FATAL - Windows Terminal", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

    while (1) {
        Sleep(1000);
    }

    DeleteCriticalSection(&g_cs);
    for (int i = 0; i < 4; ++i) CloseHandle(hThreads[i]);
    return 0;
}