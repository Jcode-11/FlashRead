#pragma once

#include <QQuickItem>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <wrl/client.h>
#include "WebView2.h"
#endif

class MarkdownWebViewItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString htmlContent READ htmlContent WRITE setHtmlContent NOTIFY htmlContentChanged)
    Q_PROPERTY(QString themeId READ themeId WRITE setThemeId NOTIFY themeIdChanged)
    Q_PROPERTY(qreal scrollProgress READ scrollProgress NOTIFY scrollProgressChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)

public:
    explicit MarkdownWebViewItem(QQuickItem *parent = nullptr);
    ~MarkdownWebViewItem() override;

    QString htmlContent() const { return m_htmlContent; }
    void setHtmlContent(const QString &html);

    QString themeId() const { return m_themeId; }
    void setThemeId(const QString &theme);

    qreal scrollProgress() const { return m_scrollProgress; }
    bool isReady() const { return m_isReady; }

    Q_INVOKABLE void scrollToHeading(const QString &headingId);
    Q_INVOKABLE void scrollToProgress(qreal progress);
    Q_INVOKABLE void reloadDocument();
    Q_INVOKABLE void capturePreviewToFile(const QString &filePath);

signals:
    void htmlContentChanged();
    void themeIdChanged();
    void scrollProgressChanged();
    void readyChanged();
    void linkClicked(const QString &url);
    void headingReached(const QString &id);

protected:
    void itemChange(ItemChange change, const ItemChangeData &value) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    void initWebView();
    void updateNativeBounds();
    void executeScript(const QString &js);
    void sendContentToWebview();
    void handleWebMessage(const QString &messageJson);

    QString m_htmlContent;
    QString m_themeId = QStringLiteral("github-light");
    qreal m_scrollProgress = 0.0;
    bool m_isReady = false;
    bool m_isInitializing = false;
    bool m_pendingContentUpdate = false;

#ifdef _WIN32
    HWND m_hwnd = nullptr;
    Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_environment;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
    Microsoft::WRL::ComPtr<ICoreWebView2> m_webview;
    EventRegistrationToken m_messageToken{};
    EventRegistrationToken m_navToken{};
#endif
};
