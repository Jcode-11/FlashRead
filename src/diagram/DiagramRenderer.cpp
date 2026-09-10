#include "DiagramRenderer.h"
#include "FlowchartLayout.h"
#include "FlowchartParser.h"
#include "SequenceDiagramParser.h"

#include <QCryptographicHash>
#include <QPainter>
#include <QSvgRenderer>

namespace Diagram {

DiagramRenderer &DiagramRenderer::instance()
{
    static DiagramRenderer s_instance;
    return s_instance;
}

QString DiagramRenderer::registerDiagram(const QString &source)
{
    QMutexLocker locker(&m_mutex);
    QByteArray hash = QCryptographicHash::hash(source.toUtf8(), QCryptographicHash::Md5).toHex();
    QString id = QString::fromLatin1(hash);
    m_sourceMap[id] = source;
    return id;
}

QString DiagramRenderer::getDiagramSource(const QString &id) const
{
    QMutexLocker locker(&m_mutex);
    return m_sourceMap.value(id);
}

void DiagramRenderer::clearCache()
{
    QMutexLocker locker(&m_mutex);
    m_imageCache.clear();
}

QString DiagramRenderer::renderErrorSvg(const QString &error, const QString &source, const ThemeColors &theme)
{
    qreal w = 500;
    qreal h = 120;
    QString svg;
    svg += QString(R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 %1 %2" width="%1" height="%2">)").arg(w).arg(h);
    svg += QString(R"(
        <rect x="10" y="10" width="%1" height="%2" rx="6" fill="%3" stroke="#f85149" stroke-width="1.5" />
        <text x="24" y="38" font-family="'Segoe UI', sans-serif" font-size="13" font-weight="600" fill="#f85149">⚠️ Diagram Syntax Error: %4</text>
        <text x="24" y="66" font-family="Consolas, monospace" font-size="11" fill="%5">%6</text>
    )")
    .arg(w - 20).arg(h - 20).arg(theme.surface.name())
    .arg(error.toHtmlEscaped())
    .arg(theme.mutedText.name())
    .arg(source.left(80).toHtmlEscaped() + (source.length() > 80 ? "..." : ""));
    svg += "</svg>";
    return svg;
}

QString DiagramRenderer::renderSvg(const QString &source, const QString &themeId, qreal *outW, qreal *outH)
{
    ThemeColors theme = ThemeColors::fromThemeId(themeId);
    QString trimmed = source.trimmed();

    if (trimmed.startsWith("sequenceDiagram", Qt::CaseInsensitive)) {
        SequenceGraph graph;
        QString error;
        if (SequenceDiagramParser::parse(trimmed, graph, &error)) {
            return SequenceDiagramParser::generateSvg(graph, theme, outW, outH);
        } else {
            return renderErrorSvg(error, trimmed, theme);
        }
    } else {
        FlowchartGraph graph;
        QString error;
        if (FlowchartParser::parse(trimmed, graph, &error)) {
            return FlowchartLayout::generateSvg(graph, theme, outW, outH);
        } else {
            return renderErrorSvg(error, trimmed, theme);
        }
    }
}

QImage DiagramRenderer::renderImage(const QString &id, const QString &themeId, qreal devicePixelRatio)
{
    QString cacheKey = QString("%1_%2_%3").arg(id, themeId).arg(devicePixelRatio);
    {
        QMutexLocker locker(&m_mutex);
        if (m_imageCache.contains(cacheKey)) {
            return m_imageCache.value(cacheKey);
        }
    }

    QString source = getDiagramSource(id);
    if (source.isEmpty()) {
        return QImage();
    }

    qreal svgW = 400, svgH = 200;
    QString svgXml = renderSvg(source, themeId, &svgW, &svgH);

    QSvgRenderer svgRenderer(svgXml.toUtf8());
    if (!svgRenderer.isValid()) {
        return QImage();
    }

    qreal dpr = std::max(1.0, devicePixelRatio);
    int imgW = static_cast<int>(std::ceil(svgW * dpr));
    int imgH = static_cast<int>(std::ceil(svgH * dpr));

    QImage image(imgW, imgH, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    svgRenderer.render(&painter, QRectF(0, 0, imgW, imgH));
    painter.end();

    image.setDevicePixelRatio(dpr);

    {
        QMutexLocker locker(&m_mutex);
        m_imageCache[cacheKey] = image;
    }

    return image;
}

} // namespace Diagram
