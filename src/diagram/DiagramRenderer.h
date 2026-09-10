#pragma once

#include "DiagramTypes.h"
#include <QHash>
#include <QImage>
#include <QMutex>
#include <QObject>
#include <QString>

namespace Diagram {

class DiagramRenderer : public QObject
{
    Q_OBJECT

public:
    static DiagramRenderer &instance();

    QString registerDiagram(const QString &source);
    QString getDiagramSource(const QString &id) const;

    QImage renderImage(const QString &id, const QString &themeId, qreal devicePixelRatio = 1.0);
    QString renderSvg(const QString &source, const QString &themeId, qreal *outW = nullptr, qreal *outH = nullptr);

    void clearCache();

private:
    DiagramRenderer() = default;
    ~DiagramRenderer() override = default;

    QString renderErrorSvg(const QString &error, const QString &source, const ThemeColors &theme);

    mutable QMutex m_mutex;
    QHash<QString, QString> m_sourceMap;       // id -> source
    QHash<QString, QImage> m_imageCache;       // (id + "_" + themeId + "_" + dpr) -> QImage
};

} // namespace Diagram
