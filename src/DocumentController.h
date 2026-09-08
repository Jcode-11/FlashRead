#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariantList>

class DocumentController final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString content READ content NOTIFY documentChanged)
    Q_PROPERTY(QString renderedContent READ renderedContent NOTIFY documentChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY documentChanged)
    Q_PROPERTY(QString title READ title NOTIFY documentChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY documentChanged)
    Q_PROPERTY(bool truncated READ truncated NOTIFY documentChanged)
    Q_PROPERTY(QVariantList outline READ outline NOTIFY documentChanged)

public:
    explicit DocumentController(QObject *parent = nullptr);

    [[nodiscard]] QString content() const;
    [[nodiscard]] QString renderedContent() const;
    [[nodiscard]] QString filePath() const;
    [[nodiscard]] QString title() const;
    [[nodiscard]] QString statusMessage() const;
    [[nodiscard]] bool truncated() const;
    [[nodiscard]] QVariantList outline() const;

    Q_INVOKABLE void openUrl(const QUrl &url);
    void openPath(const QString &path);

signals:
    void documentChanged();

private:
    QString m_content;
    QString m_renderedContent;
    QString m_filePath;
    QString m_title;
    QString m_statusMessage;
    bool m_truncated = false;
    QVariantList m_outline;
};
