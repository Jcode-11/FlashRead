#include "DiagramImageProvider.h"
#include "DiagramRenderer.h"

#include <QUrlQuery>

namespace Diagram {

DiagramImageProvider::DiagramImageProvider(const QString &defaultThemeId)
    : QQuickImageProvider(QQuickImageProvider::Image)
    , m_themeId(defaultThemeId)
{
}

void DiagramImageProvider::setThemeId(const QString &themeId)
{
    m_themeId = themeId;
}

QImage DiagramImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QString diagramId = id;
    QString theme = m_themeId;

    if (id.contains('?')) {
        int queryIdx = id.indexOf('?');
        diagramId = id.left(queryIdx);
        QUrlQuery query(id.mid(queryIdx + 1));
        if (query.hasQueryItem("theme")) {
            theme = query.queryItemValue("theme");
        }
    }

    QImage img = DiagramRenderer::instance().renderImage(diagramId, theme, 2.0); // Render at 2.0x for crisp retina display
    if (size) {
        *size = img.size() / img.devicePixelRatio();
    }

    if (!requestedSize.isEmpty()) {
        return img.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return img;
}

} // namespace Diagram
