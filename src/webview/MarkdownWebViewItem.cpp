#include "MarkdownWebViewItem.h"
#include "WebViewCallback.h"
#include "src/web/IndexHtmlTemplate.h"

#include <QQuickWindow>
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <cmath>

#ifdef _WIN32
#include <wrl/client.h>
#include <shlwapi.h>
#include "WebView2.h"

typedef HRESULT(STDAPICALLTYPE *CreateCoreWebView2EnvironmentWithOptionsFn)(
    PCWSTR browserExecutableFolder,
    PCWSTR userDataFolder,
    ICoreWebView2EnvironmentOptions *environmentOptions,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler *environmentCreatedHandler);
#endif

MarkdownWebViewItem::MarkdownWebViewItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, false);
    setAcceptedMouseButtons(Qt::AllButtons);
    connect(this, &QQuickItem::visibleChanged, this, &MarkdownWebViewItem::updateNativeBounds);
    connect(this, &QQuickItem::xChanged, this, &MarkdownWebViewItem::updateNativeBounds);
    connect(this, &QQuickItem::yChanged, this, &MarkdownWebViewItem::updateNativeBounds);
    connect(this, &QQuickItem::widthChanged, this, &MarkdownWebViewItem::updateNativeBounds);
    connect(this, &QQuickItem::heightChanged, this, &MarkdownWebViewItem::updateNativeBounds);
}

MarkdownWebViewItem::~MarkdownWebViewItem()
{
#ifdef _WIN32
    if (m_webview && m_messageToken.value != 0) {
        m_webview->remove_WebMessageReceived(m_messageToken);
    }
    if (m_webview && m_navToken.value != 0) {
        m_webview->remove_NavigationCompleted(m_navToken);
    }
    if (m_controller) {
        m_controller->Close();
        m_controller = nullptr;
    }
    m_webview = nullptr;
    m_environment = nullptr;
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
#endif
}

void MarkdownWebViewItem::itemChange(ItemChange change, const ItemChangeData &value)
{
    QQuickItem::itemChange(change, value);

    if (change == ItemSceneChange) {
        if (window()) {
            connect(window(), &QQuickWindow::xChanged, this, &MarkdownWebViewItem::updateNativeBounds, Qt::UniqueConnection);
            connect(window(), &QQuickWindow::yChanged, this, &MarkdownWebViewItem::updateNativeBounds, Qt::UniqueConnection);
            connect(window(), &QQuickWindow::widthChanged, this, &MarkdownWebViewItem::updateNativeBounds, Qt::UniqueConnection);
            connect(window(), &QQuickWindow::heightChanged, this, &MarkdownWebViewItem::updateNativeBounds, Qt::UniqueConnection);
            connect(window(), &QQuickWindow::visibleChanged, this, &MarkdownWebViewItem::updateNativeBounds, Qt::UniqueConnection);
            
            if (!m_hwnd && !m_isInitializing) {
                initWebView();
            }
        }
    } else if (change == ItemVisibleHasChanged) {
        updateNativeBounds();
    }
}

void MarkdownWebViewItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    updateNativeBounds();
}

void MarkdownWebViewItem::setHtmlContent(const QString &html)
{
    qDebug() << "[MarkdownWebViewItem] setHtmlContent length:" << html.size();
    if (m_htmlContent == html) return;
    m_htmlContent = html;
    emit htmlContentChanged();
    sendContentToWebview();
}

void MarkdownWebViewItem::setThemeId(const QString &theme)
{
    if (m_themeId == theme) return;
    m_themeId = theme;
    emit themeIdChanged();
    
    if (m_isReady) {
        executeScript(QStringLiteral("if (window.setTheme) window.setTheme('%1');").arg(theme));
    }
}

void MarkdownWebViewItem::scrollToHeading(const QString &headingId)
{
    if (!m_isReady) return;
    QJsonDocument doc(QJsonObject{{"id", headingId}});
    QString jsonStr = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    executeScript(QStringLiteral("if (window.scrollToHeading) window.scrollToHeading(%1.id);").arg(jsonStr));
}

void MarkdownWebViewItem::scrollToProgress(qreal progress)
{
    if (!m_isReady) return;
    executeScript(QStringLiteral("if (window.scrollToProgress) window.scrollToProgress(%1);").arg(progress));
}

void MarkdownWebViewItem::reloadDocument()
{
    sendContentToWebview();
}

void MarkdownWebViewItem::capturePreviewToFile(const QString &filePath)
{
#ifdef _WIN32
    if (!m_webview) {
        qWarning() << "[MarkdownWebViewItem] capturePreviewToFile: webview is null";
        return;
    }
    IStream *stream = nullptr;
    QString nativePath = QDir::toNativeSeparators(filePath);
    HRESULT hr = SHCreateStreamOnFileEx(
        reinterpret_cast<PCWSTR>(nativePath.utf16()),
        STGM_READWRITE | STGM_CREATE | STGM_SHARE_DENY_NONE,
        FILE_ATTRIBUTE_NORMAL,
        TRUE,
        nullptr,
        &stream
    );
    qDebug() << "[MarkdownWebViewItem] SHCreateStreamOnFileEx path:" << nativePath << "hr:" << hr << "stream:" << (stream != nullptr);
    if (FAILED(hr) || !stream) {
        qWarning() << "[MarkdownWebViewItem] SHCreateStreamOnFileEx failed:" << hr;
        return;
    }

    auto handler = FlashRead::WebView::MakeHandler<ICoreWebView2CapturePreviewCompletedHandler, HRESULT>(
        [stream, filePath](HRESULT res) -> HRESULT {
            stream->Release();
            qDebug() << "[MarkdownWebViewItem] CapturePreview finished res:" << res << "saved to:" << filePath;
            return S_OK;
        }
    );

    HRESULT capHr = m_webview->CapturePreview(COREWEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT_PNG, stream, handler.Get());
    qDebug() << "[MarkdownWebViewItem] m_webview->CapturePreview returned hr:" << capHr;
#endif
}

void MarkdownWebViewItem::executeScript(const QString &js)
{
#ifdef _WIN32
    if (!m_webview) return;
    auto handler = FlashRead::WebView::MakeHandler<ICoreWebView2ExecuteScriptCompletedHandler, HRESULT, PCWSTR>(
        [](HRESULT hr, PCWSTR resultJson) -> HRESULT {
            if (FAILED(hr)) {
                qWarning() << "[MarkdownWebViewItem] ExecuteScript error HR:" << hr;
            }
            return S_OK;
        }
    );
    m_webview->ExecuteScript(reinterpret_cast<PCWSTR>(js.utf16()), handler.Get());
#endif
}

void MarkdownWebViewItem::sendContentToWebview()
{
    qDebug() << "[MarkdownWebViewItem] sendContentToWebview webview:" << (m_webview != nullptr) << "isReady:" << m_isReady << "html len:" << m_htmlContent.size();
    if (!m_webview) {
        m_pendingContentUpdate = true;
        return;
    }

    QJsonObject obj;
    obj[QStringLiteral("html")] = m_htmlContent;
    obj[QStringLiteral("theme")] = m_themeId;

    QJsonDocument doc(obj);
    QString jsonStr = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    QString script = QStringLiteral(
        "(function() {\n"
        "    const data = %1;\n"
        "    window.__latestDoc = data;\n"
        "    if (window.updateDocument) {\n"
        "        window.updateDocument(data.html, data.theme);\n"
        "    }\n"
        "})();"
    ).arg(jsonStr);

    executeScript(script);
}

void MarkdownWebViewItem::handleWebMessage(const QString &messageJson)
{
    qDebug() << "[MarkdownWebViewItem] handleWebMessage raw:" << messageJson;
    QJsonParseError err{};
    QJsonDocument doc = QJsonDocument::fromJson(messageJson.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return;

    QJsonObject obj = doc.object();
    QString type = obj.value(QStringLiteral("type")).toString();

    if (type == QStringLiteral("webviewReady")) {
        m_isReady = true;
        emit readyChanged();
        if (m_pendingContentUpdate || !m_htmlContent.isEmpty()) {
            sendContentToWebview();
        }
    } else if (type == QStringLiteral("openExternal")) {
        QString url = obj.value(QStringLiteral("url")).toString();
        emit linkClicked(url);
    } else if (type == QStringLiteral("headingVisible")) {
        QString id = obj.value(QStringLiteral("id")).toString();
        emit headingReached(id);
    } else if (type == QStringLiteral("scrollProgress")) {
        m_scrollProgress = obj.value(QStringLiteral("progress")).toDouble();
        emit scrollProgressChanged();
    }
}

void MarkdownWebViewItem::initWebView()
{
#ifdef _WIN32
    if (m_hwnd || m_isInitializing || !window()) return;
    m_isInitializing = true;

    HWND parentHwnd = reinterpret_cast<HWND>(window()->winId());
    if (!parentHwnd) {
        m_isInitializing = false;
        return;
    }

    // Set WS_CLIPCHILDREN on Qt Window so DirectX swapchain doesn't overwrite child HWND
    LONG_PTR style = GetWindowLongPtrW(parentHwnd, GWL_STYLE);
    SetWindowLongPtrW(parentHwnd, GWL_STYLE, style | WS_CLIPCHILDREN);

    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    static bool s_classRegistered = false;
    if (!s_classRegistered) {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
        wc.lpfnWndProc = DefWindowProcW;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"FlashReadWebViewHostClass";
        wc.hbrBackground = nullptr;
        RegisterClassExW(&wc);
        s_classRegistered = true;
    }

    qreal dpr = window()->devicePixelRatio();
    QPointF scenePos = mapToScene(QPointF(0, 0));
    int initX = static_cast<int>(std::round(scenePos.x() * dpr));
    int initY = static_cast<int>(std::round(scenePos.y() * dpr));
    int initW = static_cast<int>(std::round(std::max(10.0, width() * dpr)));
    int initH = static_cast<int>(std::round(std::max(10.0, height() * dpr)));

    m_hwnd = CreateWindowExW(
        WS_EX_NOPARENTNOTIFY,
        L"FlashReadWebViewHostClass",
        L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        initX, initY, initW, initH,
        parentHwnd,
        nullptr,
        hInstance,
        nullptr
    );

    if (!m_hwnd) {
        qWarning() << "[MarkdownWebViewItem] CreateWindowExW failed:" << GetLastError();
        m_isInitializing = false;
        return;
    }

    // Locate WebView2Loader.dll
    QString appDir = QCoreApplication::applicationDirPath();
    QString loaderPath = appDir + QStringLiteral("/WebView2Loader.dll");
    if (!QFile::exists(loaderPath)) {
        loaderPath = appDir + QStringLiteral("/../3rdparty/webview2/build/native/x64/WebView2Loader.dll");
    }
    if (!QFile::exists(loaderPath)) {
        loaderPath = QStringLiteral("WebView2Loader.dll");
    }

    HMODULE hLoader = LoadLibraryW(reinterpret_cast<PCWSTR>(loaderPath.utf16()));
    if (!hLoader) {
        hLoader = LoadLibraryW(L"WebView2Loader.dll");
    }

    if (!hLoader) {
        qWarning() << "[MarkdownWebViewItem] Failed to load WebView2Loader.dll from" << loaderPath;
        m_isInitializing = false;
        return;
    }

    auto pfnCreate = reinterpret_cast<CreateCoreWebView2EnvironmentWithOptionsFn>(
        GetProcAddress(hLoader, "CreateCoreWebView2EnvironmentWithOptions")
    );

    if (!pfnCreate) {
        qWarning() << "[MarkdownWebViewItem] CreateCoreWebView2EnvironmentWithOptions not found in DLL";
        m_isInitializing = false;
        return;
    }

    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/WebView2_Data");
    QDir().mkpath(dataPath);

    auto envHandler = FlashRead::WebView::MakeHandler<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler, HRESULT, ICoreWebView2Environment*>(
        [this](HRESULT res, ICoreWebView2Environment *env) -> HRESULT {
            if (FAILED(res) || !env) {
                qWarning() << "[MarkdownWebViewItem] CreateCoreWebView2Environment failed:" << res;
                m_isInitializing = false;
                return S_OK;
            }

            m_environment = env;
            auto controllerHandler = FlashRead::WebView::MakeHandler<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler, HRESULT, ICoreWebView2Controller*>(
                [this](HRESULT ctrlRes, ICoreWebView2Controller *controller) -> HRESULT {
                    m_isInitializing = false;
                    if (FAILED(ctrlRes) || !controller) {
                        qWarning() << "[MarkdownWebViewItem] CreateCoreWebView2Controller failed:" << ctrlRes;
                        return S_OK;
                    }

                    m_controller = controller;
                    m_controller->get_CoreWebView2(&m_webview);

                    if (!m_webview) {
                        qWarning() << "[MarkdownWebViewItem] get_CoreWebView2 returned null";
                        return S_OK;
                    }

                    // Settings
                    Microsoft::WRL::ComPtr<ICoreWebView2Settings> settings;
                    if (SUCCEEDED(m_webview->get_Settings(&settings)) && settings) {
                        settings->put_IsScriptEnabled(TRUE);
                        settings->put_AreDefaultContextMenusEnabled(FALSE);
                        settings->put_AreDevToolsEnabled(FALSE);
                        settings->put_IsStatusBarEnabled(FALSE);
                        settings->put_IsBuiltInErrorPageEnabled(FALSE);
                        settings->put_AreDefaultScriptDialogsEnabled(TRUE);
                    }

                    // Web message handler
                    auto msgHandler = FlashRead::WebView::MakeHandler<ICoreWebView2WebMessageReceivedEventHandler, ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs*>(
                        [this](ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs *args) -> HRESULT {
                            LPWSTR rawMsg = nullptr;
                            if (SUCCEEDED(args->get_WebMessageAsJson(&rawMsg)) && rawMsg) {
                                QString msg = QString::fromWCharArray(rawMsg);
                                CoTaskMemFree(rawMsg);
                                handleWebMessage(msg);
                            }
                            return S_OK;
                        }
                    );
                    m_webview->add_WebMessageReceived(msgHandler.Get(), &m_messageToken);

                    // Navigation completed
                    auto navHandler = FlashRead::WebView::MakeHandler<ICoreWebView2NavigationCompletedEventHandler, ICoreWebView2*, ICoreWebView2NavigationCompletedEventArgs*>(
                        [this](ICoreWebView2*, ICoreWebView2NavigationCompletedEventArgs *args) -> HRESULT {
                            BOOL success = FALSE;
                            args->get_IsSuccess(&success);
                            if (success) {
                                m_isReady = true;
                                emit readyChanged();
                                if (m_pendingContentUpdate || !m_htmlContent.isEmpty()) {
                                    sendContentToWebview();
                                }
                            }
                            return S_OK;
                        }
                    );
                    m_webview->add_NavigationCompleted(navHandler.Get(), &m_navToken);

                    // Load self-contained template
                    QString templateHtml = QString::fromUtf8(FlashRead::Web::INDEX_HTML);
                    m_webview->NavigateToString(reinterpret_cast<PCWSTR>(templateHtml.utf16()));

                    updateNativeBounds();
                    return S_OK;
                }
            );

            m_environment->CreateCoreWebView2Controller(m_hwnd, controllerHandler.Get());
            return S_OK;
        }
    );

    HRESULT createHr = pfnCreate(
        nullptr,
        reinterpret_cast<PCWSTR>(QDir::toNativeSeparators(dataPath).utf16()),
        nullptr,
        envHandler.Get()
    );
    qDebug() << "[MarkdownWebViewItem] CreateCoreWebView2EnvironmentWithOptions returned:" << createHr;
    if (FAILED(createHr)) {
        QString fallbackDataPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QStringLiteral("/FlashRead_WV2");
        QDir().mkpath(fallbackDataPath);
        HRESULT retryHr = pfnCreate(
            nullptr,
            reinterpret_cast<PCWSTR>(QDir::toNativeSeparators(fallbackDataPath).utf16()),
            nullptr,
            envHandler.Get()
        );
        qDebug() << "[MarkdownWebViewItem] Retry with fallback data path returned:" << retryHr;
    }
#endif
}

void MarkdownWebViewItem::updateNativeBounds()
{
#ifdef _WIN32
    if (!m_hwnd || !window()) return;

    bool shouldBeVisible = isVisible() && window()->isVisible() && width() > 1 && height() > 1;
    qDebug() << "[MarkdownWebViewItem] updateNativeBounds shouldBeVisible:" << shouldBeVisible << "width:" << width() << "height:" << height() << "isVisible():" << isVisible() << "windowVisible:" << window()->isVisible();

    if (!shouldBeVisible) {
        ShowWindow(m_hwnd, SW_HIDE);
        if (m_controller) {
            m_controller->put_IsVisible(FALSE);
        }
        return;
    }

    qreal dpr = window()->devicePixelRatio();
    QPointF scenePos = mapToScene(QPointF(0, 0));

    int x = static_cast<int>(std::round(scenePos.x() * dpr));
    int y = static_cast<int>(std::round(scenePos.y() * dpr));
    int w = static_cast<int>(std::round(width() * dpr));
    int h = static_cast<int>(std::round(height() * dpr));
    qDebug() << "[MarkdownWebViewItem] SetWindowPos x:" << x << "y:" << y << "w:" << w << "h:" << h << "dpr:" << dpr;

    SetWindowPos(m_hwnd, HWND_TOP, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    ShowWindow(m_hwnd, SW_SHOW);

    if (m_controller) {
        m_controller->put_Bounds(RECT{ 0, 0, w, h });
        m_controller->put_IsVisible(TRUE);
    }
#endif
}
