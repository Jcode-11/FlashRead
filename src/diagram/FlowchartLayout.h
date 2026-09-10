#pragma once

#include "DiagramTypes.h"
#include <QString>
#include <QFont>

namespace Diagram {

class FlowchartLayout
{
public:
    static QString generateSvg(FlowchartGraph &graph, const ThemeColors &theme, qreal *outWidth = nullptr, qreal *outHeight = nullptr);

private:
    static void layoutGraph(FlowchartGraph &graph, const QFont &font);
    static QString escapeXml(const QString &str);
    static QString renderNodeShape(const Node &node, const ThemeColors &theme);
    static QString renderEdge(const Edge &edge, const Node &fromNode, const Node &toNode, Direction dir, const ThemeColors &theme);
};

} // namespace Diagram
