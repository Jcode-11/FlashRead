#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariantList>
#include <atomic>

class DocumentController final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString content READ content NOTIFY documentChanged)
    Q_PROPERTY(QString renderedContent READ renderedContent NOTIFY documentChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY documentChanged)
    Q_PROPERTY(QString title READ title NOTIFY documentChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY documentChanged)
    Q_PROPERTY(bool truncated READ truncated NOTIFY documentChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY documentChanged)
    Q_PROPERTY(QVariantList outline READ outline NOTIFY documentChanged)
    Q_PROPERTY(bool isModified READ isModified WRITE setModified NOTIFY modifiedChanged)
    Q_PROPERTY(int wordCount READ wordCount NOTIFY documentChanged)
    Q_PROPERTY(int lineCount READ lineCount NOTIFY documentChanged)

public:
    explicit DocumentController(QObject *parent = nullptr);

    [[nodiscard]] QString content() const;
    [[nodiscard]] QString renderedContent() const;
    [[nodiscard]] QString filePath() const;
    [[nodiscard]] QString title() const;
    [[nodiscard]] QString statusMessage() const;
    [[nodiscard]] bool truncated() const;
    [[nodiscard]] bool isLoading() const;
    [[nodiscard]] QVariantList outline() const;
    [[nodiscard]] bool isModified() const;
    [[nodiscard]] int wordCount() const;
    [[nodiscard]] int lineCount() const;

    Q_INVOKABLE void openUrl(const QUrl &url);
    void openPath(const QString &path);
    Q_INVOKABLE void newUntitled(const QString &defaultTitle = QString());
    Q_INVOKABLE void updateContent(const QString &newContent);
    Q_INVOKABLE bool saveContent(const QString &content);
    Q_INVOKABLE bool saveContentAs(const QString &newPath, const QString &content);
    Q_INVOKABLE bool saveCurrentDocument();
    Q_INVOKABLE void setModified(bool modified);
    Q_INVOKABLE void reload();

signals:
    void documentChanged();
    void modifiedChanged();
    void documentLoaded();

private:
    QString m_content;
    QString m_renderedContent;
    QString m_filePath;
    QString m_title;
    QString m_statusMessage;
    bool m_truncated = false;
    bool m_isLoading = false;
    bool m_isModified = false;
    QVariantList m_outline;
    std::atomic<quint64> m_currentRequestId {0};
};
