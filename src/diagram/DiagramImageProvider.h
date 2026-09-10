#pragma once

#include <QQuickImageProvider>
#include <QString>

namespace Diagram {

class DiagramImageProvider : public QQuickImageProvider
{
public:
    explicit DiagramImageProvider(const QString &defaultThemeId = QStringLiteral("github-light"));

    void setThemeId(const QString &themeId);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    QString m_themeId;
};

} // namespace Diagram
