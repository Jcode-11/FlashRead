#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QDebug>
#include <windows.h>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    HWND hwnd = nullptr;
    
    HWND top = GetTopWindow(nullptr);
    while (top) {
        wchar_t title[256];
        GetWindowTextW(top, title, 256);
        if (wcsstr(title, L"FlashRead") && IsWindowVisible(top)) {
            hwnd = top;
            break;
        }
        top = GetNextWindow(top, GW_HWNDNEXT);
    }
    
    RECT rect;
    if (hwnd && GetWindowRect(hwnd, &rect)) {
        int w = rect.right - rect.left;
        int h = rect.bottom - rect.top;
        qDebug() << "Found FlashRead HWND:" << hwnd << "rect:" << rect.left << rect.top << w << h;
        
        ShowWindow(hwnd, SW_RESTORE);
        SetForegroundWindow(hwnd);
        Sleep(500);

        QScreen *screen = app.primaryScreen();
        if (screen) {
            QPixmap pix = screen->grabWindow(0, rect.left, rect.top, w, h);
            pix.save(QStringLiteral("C:/Users/Administrator/.gemini/antigravity/brain/3fd8f9a7-b14b-4528-8d54-9636b3461bf3/screen_capture.png"));
            qDebug() << "Saved window grab to screen_capture.png:" << pix.size();
        }
    } else {
        qDebug() << "FlashRead window not found, grabbing full screen";
        QScreen *screen = app.primaryScreen();
        if (screen) {
            QPixmap pix = screen->grabWindow(0);
            pix.save(QStringLiteral("C:/Users/Administrator/.gemini/antigravity/brain/3fd8f9a7-b14b-4528-8d54-9636b3461bf3/screen_capture.png"));
            qDebug() << "Saved full screen grab:" << pix.size();
        }
    }
    return 0;
}
