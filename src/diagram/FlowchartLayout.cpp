#include "FlowchartLayout.h"

#include <QFontMetrics>
#include <QMap>
#include <QSet>
#include <algorithm>
#include <cmath>

namespace Diagram {

QString FlowchartLayout::escapeXml(const QString &str)
{
    QString res = str;
    res.replace('&', "&amp;");
    res.replace('<', "&lt;");
    res.replace('>', "&gt;");
    res.replace('"', "&quot;");
    res.replace('\'', "&apos;");
    return res;
}

void FlowchartLayout::layoutGraph(FlowchartGraph &graph, const QFont &font)
{
    QFontMetrics fm(font);
    QMap<QString, int> idToIndex;
    for (int i = 0; i < graph.nodes.size(); ++i) {
        idToIndex[graph.nodes[i].id] = i;
    }

    // 1. Calculate node dimensions
    for (auto &node : graph.nodes) {
        int textW = fm.horizontalAdvance(node.label);
        int textH = fm.height();

        switch (node.shape) {
        case ShapeType::Circle: {
            qreal r = std::max(28.0, (textW + 24.0) / 2.0);
            node.width = r * 2;
            node.height = r * 2;
            break;
        }
        case ShapeType::Rhombus: {
            node.width = std::max(110.0, textW + 64.0);
            node.height = std::max(56.0, textH + 40.0);
            break;
        }
        case ShapeType::Hexagon:
        case ShapeType::Parallelogram: {
            node.width = std::max(90.0, textW + 48.0);
            node.height = std::max(40.0, textH + 22.0);
            break;
        }
        case ShapeType::Stadium:
        case ShapeType::Rounded:
        case ShapeType::Rectangle:
        case ShapeType::Subroutine:
        case ShapeType::Cylinder:
        case ShapeType::Asymmetric:
        default: {
            node.width = std::max(80.0, textW + 36.0);
            node.height = std::max(38.0, textH + 20.0);
            break;
        }
        }
    }

    int n = graph.nodes.size();
    if (n == 0) return;

    // 2. Build adjacency for ranking
    QVector<QList<int>> outAdj(n), inAdj(n);
    for (const auto &edge : graph.edges) {
        if (idToIndex.contains(edge.fromId) && idToIndex.contains(edge.toId)) {
            int u = idToIndex[edge.fromId];
            int v = idToIndex[edge.toId];
            if (u != v) {
                outAdj[u].append(v);
                inAdj[v].append(u);
            }
        }
    }

    // 3. Cycle breaking & rank calculation (Longest Path with DFS back-edge detection)
    QVector<int> rank(n, 0);
    QVector<int> visited(n, 0); // 0=unvisited, 1=visiting, 2=visited
    QVector<QList<int>> dagOutAdj = outAdj;

    auto dfsBreakCycles = [&](auto &self, int u) -> void {
        visited[u] = 1;
        for (int i = 0; i < outAdj[u].size(); ++i) {
            int v = outAdj[u][i];
            if (visited[v] == 1) {
                // Back edge (cycle): remove from DAG adjacency
                dagOutAdj[u].removeOne(v);
            } else if (visited[v] == 0) {
                self(self, v);
            }
        }
        visited[u] = 2;
    };

    for (int i = 0; i < n; ++i) {
        if (visited[i] == 0) {
            dfsBreakCycles(dfsBreakCycles, i);
        }
    }

    // Recompute inAdj for DAG
    QVector<QList<int>> dagInAdj(n);
    for (int u = 0; u < n; ++u) {
        for (int v : dagOutAdj[u]) {
            dagInAdj[v].append(u);
        }
    }

    // Topological ranking relaxation
    for (int iter = 0; iter < n + 1; ++iter) {
        bool changed = false;
        for (int u = 0; u < n; ++u) {
            for (int v : dagOutAdj[u]) {
                if (rank[v] < rank[u] + 1) {
                    rank[v] = rank[u] + 1;
                    changed = true;
                }
            }
        }
        if (!changed) break;
    }

    // Assign rank to nodes
    int maxRank = 0;
    for (int i = 0; i < n; ++i) {
        graph.nodes[i].rank = rank[i];
        maxRank = std::max(maxRank, rank[i]);
    }

    // Group nodes by layer
    QVector<QList<int>> layers(maxRank + 1);
    for (int i = 0; i < n; ++i) {
        layers[rank[i]].append(i);
    }

    // 4. Crossing minimization (Barycenter sweep)
    for (int r = 1; r <= maxRank; ++r) {
        auto &currentLayer = layers[r];
        std::sort(currentLayer.begin(), currentLayer.end(), [&](int a, int b) {
            double posA = 0, posB = 0;
            int countA = 0, countB = 0;
            for (int p : dagInAdj[a]) {
                posA += graph.nodes[p].order;
                countA++;
            }
            for (int p : dagInAdj[b]) {
                posB += graph.nodes[p].order;
                countB++;
            }
            double avgA = countA > 0 ? (posA / countA) : 0;
            double avgB = countB > 0 ? (posB / countB) : 0;
            return avgA < avgB;
        });

        for (int i = 0; i < currentLayer.size(); ++i) {
            graph.nodes[currentLayer[i]].order = i;
        }
    }

    // 5. Position Assignment
    const qreal nodeSpacing = 32.0;
    const qreal rankSpacing = 54.0;
    const qreal margin = 40.0;

    const bool isHorizontal = (graph.direction == Direction::LeftToRight || graph.direction == Direction::RightToLeft);

    if (!isHorizontal) {
        // Vertical layout (TopToBottom / BottomToTop)
        qreal currentY = margin;
        qreal maxLayerWidth = 0;

        QVector<qreal> layerWidths(maxRank + 1, 0);
        for (int r = 0; r <= maxRank; ++r) {
            qreal w = 0;
            for (int idx : layers[r]) {
                w += graph.nodes[idx].width + nodeSpacing;
            }
            if (!layers[r].isEmpty()) w -= nodeSpacing;
            layerWidths[r] = w;
            maxLayerWidth = std::max(maxLayerWidth, w);
        }

        for (int r = 0; r <= maxRank; ++r) {
            int effectiveRank = (graph.direction == Direction::BottomToTop) ? (maxRank - r) : r;
            const auto &layer = layers[effectiveRank];

            qreal maxH = 0;
            for (int idx : layer) {
                maxH = std::max(maxH, graph.nodes[idx].height);
            }

            qreal startX = margin + (maxLayerWidth - layerWidths[effectiveRank]) / 2.0;
            qreal currentX = startX;

            for (int idx : layer) {
                auto &node = graph.nodes[idx];
                node.x = currentX + node.width / 2.0;
                node.y = currentY + maxH / 2.0;
                currentX += node.width + nodeSpacing;
            }

            currentY += maxH + rankSpacing;
        }
    } else {
        // Horizontal layout (LeftToRight / RightToLeft)
        qreal currentX = margin;
        qreal maxLayerHeight = 0;

        QVector<qreal> layerHeights(maxRank + 1, 0);
        for (int r = 0; r <= maxRank; ++r) {
            qreal h = 0;
            for (int idx : layers[r]) {
                h += graph.nodes[idx].height + nodeSpacing;
            }
            if (!layers[r].isEmpty()) h -= nodeSpacing;
            layerHeights[r] = h;
            maxLayerHeight = std::max(maxLayerHeight, h);
        }

        for (int r = 0; r <= maxRank; ++r) {
            int effectiveRank = (graph.direction == Direction::RightToLeft) ? (maxRank - r) : r;
            const auto &layer = layers[effectiveRank];

            qreal maxW = 0;
            for (int idx : layer) {
                maxW = std::max(maxW, graph.nodes[idx].width);
            }

            qreal startY = margin + (maxLayerHeight - layerHeights[effectiveRank]) / 2.0;
            qreal currentY = startY;

            for (int idx : layer) {
                auto &node = graph.nodes[idx];
                node.x = currentX + maxW / 2.0;
                node.y = currentY + node.height / 2.0;
                currentY += node.height + nodeSpacing;
            }

            currentX += maxW + rankSpacing;
        }
    }

    // 6. Compute Subgraph Bounding Boxes
    for (auto &sg : graph.subgraphs) {
        if (sg.nodeIds.isEmpty()) continue;
        qreal minX = 1e9, minY = 1e9, maxX = -1e9, maxY = -1e9;
        for (const QString &id : sg.nodeIds) {
            if (idToIndex.contains(id)) {
                const auto &node = graph.nodes[idToIndex[id]];
                minX = std::min(minX, node.x - node.width / 2.0);
                minY = std::min(minY, node.y - node.height / 2.0);
                maxX = std::max(maxX, node.x + node.width / 2.0);
                maxY = std::max(maxY, node.y + node.height / 2.0);
            }
        }
        if (minX <= maxX) {
            sg.bounds = QRectF(minX - 16, minY - 32, (maxX - minX) + 32, (maxY - minY) + 48);
        }
    }
}

QString FlowchartLayout::renderNodeShape(const Node &node, const ThemeColors &theme)
{
    qreal x = node.x - node.width / 2.0;
    qreal y = node.y - node.height / 2.0;
    qreal w = node.width;
    qreal h = node.height;

    QString strokeColor = theme.nodeBorder.name();
    QString fillColor = theme.nodeBg.name();
    QString textColor = theme.text.name();
    QString labelEsc = escapeXml(node.label);

    QString shapeXml;
    switch (node.shape) {
    case ShapeType::Rounded:
        shapeXml = QString(R"(<rect x="%1" y="%2" width="%3" height="%4" rx="10" ry="10" fill="%5" stroke="%6" stroke-width="1.8" />)")
                   .arg(x).arg(y).arg(w).arg(h).arg(fillColor, strokeColor);
        break;
    case ShapeType::Stadium:
        shapeXml = QString(R"(<rect x="%1" y="%2" width="%3" height="%4" rx="%5" ry="%5" fill="%6" stroke="%7" stroke-width="1.8" />)")
                   .arg(x).arg(y).arg(w).arg(h).arg(h / 2.0).arg(fillColor, strokeColor);
        break;
    case ShapeType::Circle:
        shapeXml = QString(R"(<circle cx="%1" cy="%2" r="%3" fill="%4" stroke="%5" stroke-width="1.8" />)")
                   .arg(node.x).arg(node.y).arg(w / 2.0).arg(fillColor, strokeColor);
        break;
    case ShapeType::Rhombus: {
        QString points = QString("%1,%2 %3,%4 %5,%6 %7,%8")
            .arg(node.x).arg(y)
            .arg(x + w).arg(node.y)
            .arg(node.x).arg(y + h)
            .arg(x).arg(node.y);
        shapeXml = QString(R"(<polygon points="%1" fill="%2" stroke="%3" stroke-width="1.8" />)")
                   .arg(points, fillColor, strokeColor);
        break;
    }
    case ShapeType::Hexagon: {
        qreal cut = w * 0.15;
        QString points = QString("%1,%2 %3,%4 %5,%6 %7,%8 %9,%10 %11,%12")
            .arg(x + cut).arg(y)
            .arg(x + w - cut).arg(y)
            .arg(x + w).arg(node.y)
            .arg(x + w - cut).arg(y + h)
            .arg(x + cut).arg(y + h)
            .arg(x).arg(node.y);
        shapeXml = QString(R"(<polygon points="%1" fill="%2" stroke="%3" stroke-width="1.8" />)")
                   .arg(points, fillColor, strokeColor);
        break;
    }
    case ShapeType::Parallelogram: {
        qreal slant = 14;
        QString points = QString("%1,%2 %3,%4 %5,%6 %7,%8")
            .arg(x + slant).arg(y)
            .arg(x + w).arg(y)
            .arg(x + w - slant).arg(y + h)
            .arg(x).arg(y + h);
        shapeXml = QString(R"(<polygon points="%1" fill="%2" stroke="%3" stroke-width="1.8" />)")
                   .arg(points, fillColor, strokeColor);
        break;
    }
    case ShapeType::Cylinder: {
        qreal eh = 8.0;
        shapeXml = QString(R"(
            <path d="M %1 %2 C %1 %3, %4 %3, %4 %2 L %4 %5 C %4 %6, %1 %6, %1 %5 Z" fill="%7" stroke="%8" stroke-width="1.8" />
            <ellipse cx="%9" cy="%2" rx="%10" ry="%11" fill="%7" stroke="%8" stroke-width="1.8" />
        )")
        .arg(x).arg(y + eh).arg(y + eh * 2).arg(x + w).arg(y + h - eh).arg(y + h).arg(fillColor, strokeColor)
        .arg(node.x).arg(w / 2.0).arg(eh);
        break;
    }
    case ShapeType::Subroutine:
        shapeXml = QString(R"(
            <rect x="%1" y="%2" width="%3" height="%4" rx="4" ry="4" fill="%5" stroke="%6" stroke-width="1.8" />
            <line x1="%7" y1="%2" x2="%7" y2="%8" stroke="%6" stroke-width="1.8" />
            <line x1="%9" y1="%2" x2="%9" y2="%8" stroke="%6" stroke-width="1.8" />
        )")
        .arg(x).arg(y).arg(w).arg(h).arg(fillColor, strokeColor)
        .arg(x + 10).arg(y + h).arg(x + w - 10);
        break;
    case ShapeType::Asymmetric: {
        qreal flagW = 12.0;
        shapeXml = QString(R"(<path d="M %1 %2 L %3 %2 L %4 %5 L %3 %6 L %1 %6 Z" fill="%7" stroke="%8" stroke-width="1.8" />)")
                   .arg(x + flagW).arg(y).arg(x + w).arg(x + w).arg(node.y).arg(y + h).arg(fillColor, strokeColor);
        break;
    }
    case ShapeType::Rectangle:
    default:
        shapeXml = QString(R"(<rect x="%1" y="%2" width="%3" height="%4" rx="5" ry="5" fill="%5" stroke="%6" stroke-width="1.8" />)")
                   .arg(x).arg(y).arg(w).arg(h).arg(fillColor, strokeColor);
        break;
    }

    // Node Text
    QString textXml = QString(R"(<text x="%1" y="%2" dominant-baseline="central" text-anchor="middle" font-family="'Segoe UI', -apple-system, sans-serif" font-size="13" font-weight="500" fill="%3">%4</text>)")
                      .arg(node.x).arg(node.y).arg(textColor, labelEsc);

    return QString("<g class=\"node\" id=\"node_%1\">\n%2\n%3\n</g>\n").arg(node.id, shapeXml, textXml);
}

QString FlowchartLayout::renderEdge(const Edge &edge, const Node &fromNode, const Node &toNode, Direction dir, const ThemeColors &theme)
{
    const bool isHorizontal = (dir == Direction::LeftToRight || dir == Direction::RightToLeft);

    qreal startX = 0, startY = 0, endX = 0, endY = 0;
    qreal c1x = 0, c1y = 0, c2x = 0, c2y = 0;

    if (!isHorizontal) {
        if (dir == Direction::TopToBottom) {
            startX = fromNode.x;
            startY = fromNode.y + fromNode.height / 2.0;
            endX = toNode.x;
            endY = toNode.y - toNode.height / 2.0;
        } else {
            startX = fromNode.x;
            startY = fromNode.y - fromNode.height / 2.0;
            endX = toNode.x;
            endY = toNode.y + toNode.height / 2.0;
        }
        qreal midY = (startY + endY) / 2.0;
        c1x = startX;
        c1y = midY;
        c2x = endX;
        c2y = midY;
    } else {
        if (dir == Direction::LeftToRight) {
            startX = fromNode.x + fromNode.width / 2.0;
            startY = fromNode.y;
            endX = toNode.x - toNode.width / 2.0;
            endY = toNode.y;
        } else {
            startX = fromNode.x - fromNode.width / 2.0;
            startY = fromNode.y;
            endX = toNode.x + toNode.width / 2.0;
            endY = toNode.y;
        }
        qreal midX = (startX + endX) / 2.0;
        c1x = midX;
        c1y = startY;
        c2x = midX;
        c2y = endY;
    }

    QString pathD = QString("M %1 %2 C %3 %4, %5 %6, %7 %8")
                    .arg(startX).arg(startY).arg(c1x).arg(c1y).arg(c2x).arg(c2y).arg(endX).arg(endY);

    QString edgeColor = theme.edge.name();
    QString strokeWidth = "1.6";
    QString strokeDash = "";
    QString markerEnd = "";

    switch (edge.type) {
    case EdgeType::SolidArrow:
        markerEnd = "url(#arrow)";
        break;
    case EdgeType::SolidLine:
        break;
    case EdgeType::DottedArrow:
        strokeDash = "stroke-dasharray=\"4,3\"";
        markerEnd = "url(#arrow)";
        break;
    case EdgeType::DottedLine:
        strokeDash = "stroke-dasharray=\"4,3\"";
        break;
    case EdgeType::ThickArrow:
        strokeWidth = "2.8";
        markerEnd = "url(#thick-arrow)";
        break;
    case EdgeType::ThickLine:
        strokeWidth = "2.8";
        break;
    }

    QString markerAttr = markerEnd.isEmpty() ? "" : QString("marker-end=\"%1\"").arg(markerEnd);

    QString edgeXml = QString(R"(<path d="%1" stroke="%2" stroke-width="%3" %4 fill="none" %5 />)")
                      .arg(pathD, edgeColor, strokeWidth, strokeDash, markerAttr);

    // Render edge label if present
    if (!edge.label.isEmpty()) {
        qreal labelX = (startX + endX) / 2.0;
        qreal labelY = (startY + endY) / 2.0;
        QFont font("Segoe UI", 9);
        QFontMetrics fm(font);
        int lw = fm.horizontalAdvance(edge.label) + 12;
        int lh = fm.height() + 4;

        QString labelEsc = escapeXml(edge.label);
        QString labelBox = QString(R"(
            <rect x="%1" y="%2" width="%3" height="%4" rx="3" fill="%5" stroke="%6" stroke-width="1" />
            <text x="%7" y="%8" dominant-baseline="central" text-anchor="middle" font-family="'Segoe UI', sans-serif" font-size="11" fill="%9">%10</text>
        )")
        .arg(labelX - lw / 2.0).arg(labelY - lh / 2.0).arg(lw).arg(lh).arg(theme.surface.name(), theme.border.name())
        .arg(labelX).arg(labelY).arg(theme.text.name(), labelEsc);

        edgeXml += "\n" + labelBox;
    }

    return QString("<g class=\"edge\">\n%1\n</g>\n").arg(edgeXml);
}

QString FlowchartLayout::generateSvg(FlowchartGraph &graph, const ThemeColors &theme, qreal *outWidth, qreal *outHeight)
{
    QFont font("Segoe UI", 10);
    font.setWeight(QFont::Medium);
    layoutGraph(graph, font);

    // Calculate total SVG bounding box
    qreal minX = 1e9, minY = 1e9, maxX = -1e9, maxY = -1e9;
    for (const auto &node : graph.nodes) {
        minX = std::min(minX, node.x - node.width / 2.0);
        minY = std::min(minY, node.y - node.height / 2.0);
        maxX = std::max(maxX, node.x + node.width / 2.0);
        maxY = std::max(maxY, node.y + node.height / 2.0);
    }
    for (const auto &sg : graph.subgraphs) {
        if (!sg.bounds.isEmpty()) {
            minX = std::min(minX, sg.bounds.left());
            minY = std::min(minY, sg.bounds.top());
            maxX = std::max(maxX, sg.bounds.right());
            maxY = std::max(maxY, sg.bounds.bottom());
        }
    }

    const qreal pad = 24.0;
    qreal svgW = std::max(120.0, (maxX - minX) + pad * 2);
    qreal svgH = std::max(80.0, (maxY - minY) + pad * 2);

    if (outWidth) *outWidth = svgW;
    if (outHeight) *outHeight = svgH;

    // Shift coordinates so (0,0) starts with padding
    qreal offsetX = pad - minX;
    qreal offsetY = pad - minY;

    for (auto &node : graph.nodes) {
        node.x += offsetX;
        node.y += offsetY;
    }
    for (auto &sg : graph.subgraphs) {
        sg.bounds.translate(offsetX, offsetY);
    }

    QMap<QString, Node> nodeMap;
    for (const auto &node : graph.nodes) {
        nodeMap[node.id] = node;
    }

    // Build SVG Document
    QString svg;
    svg += QString(R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 %1 %2" width="%1" height="%2">)").arg(svgW).arg(svgH);
    svg += "\n<defs>\n";
    svg += QString(R"(
        <marker id="arrow" viewBox="0 0 10 10" refX="8" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
            <path d="M 0 1.5 L 9 5 L 0 8.5 z" fill="%1" />
        </marker>
        <marker id="thick-arrow" viewBox="0 0 10 10" refX="8" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">
            <path d="M 0 1 L 9 5 L 0 9 z" fill="%1" />
        </marker>
    )").arg(theme.edge.name());
    svg += "</defs>\n";

    // Subgraphs
    for (const auto &sg : graph.subgraphs) {
        if (sg.bounds.isEmpty()) continue;
        svg += QString(R"(
            <g class="subgraph">
                <rect x="%1" y="%2" width="%3" height="%4" rx="8" fill="%5" stroke="%6" stroke-width="1.2" stroke-dasharray="5,4" />
                <text x="%7" y="%8" font-family="'Segoe UI', sans-serif" font-size="12" font-weight="600" fill="%9">%10</text>
            </g>
        )")
        .arg(sg.bounds.x()).arg(sg.bounds.y()).arg(sg.bounds.width()).arg(sg.bounds.height())
        .arg(theme.clusterBg.name(), theme.clusterBorder.name())
        .arg(sg.bounds.x() + 12).arg(sg.bounds.y() + 18)
        .arg(theme.mutedText.name(), escapeXml(sg.title));
    }

    // Edges
    for (const auto &edge : graph.edges) {
        if (nodeMap.contains(edge.fromId) && nodeMap.contains(edge.toId)) {
            svg += renderEdge(edge, nodeMap[edge.fromId], nodeMap[edge.toId], graph.direction, theme);
        }
    }

    // Nodes
    for (const auto &node : graph.nodes) {
        svg += renderNodeShape(node, theme);
    }

    svg += "</svg>";
    return svg;
}

} // namespace Diagram
