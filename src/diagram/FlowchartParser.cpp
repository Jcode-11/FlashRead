#include "FlowchartParser.h"
#include <QRegularExpression>
#include <QStringList>
#include <QMap>

namespace Diagram {

QString FlowchartParser::stripQuotes(const QString &str)
{
    QString trimmed = str.trimmed();
    if (trimmed.startsWith('"') && trimmed.endsWith('"') && trimmed.length() >= 2) {
        trimmed = trimmed.mid(1, trimmed.length() - 2);
    }
    return trimmed;
}

bool FlowchartParser::parseNodeSpec(const QString &token, Node &outNode, QString &outRest)
{
    QString s = token.trimmed();
    if (s.isEmpty()) return false;

    // Pattern to match ID followed by optional shape definition
    // Shapes:
    // 1. Stadium: id([label])
    // 2. Subroutine: id[[label]]
    // 3. Cylinder: id[(label)]
    // 4. Circle: id((label))
    // 5. Hexagon: id{{label}}
    // 6. Parallelogram: id[/label/]
    // 7. Rectangle: id[label]
    // 8. Rounded: id(label)
    // 9. Rhombus: id{label}
    // 10. Asymmetric: id>label]

    static const struct ShapePattern {
        ShapeType type;
        const char *openTag;
        const char *closeTag;
    } patterns[] = {
        { ShapeType::Stadium,       "([", "])" },
        { ShapeType::Subroutine,    "[[", "]]" },
        { ShapeType::Cylinder,      "[(", ")]" },
        { ShapeType::Circle,        "((", "))" },
        { ShapeType::Hexagon,       "{{", "}}" },
        { ShapeType::Parallelogram, "[/", "/]" },
        { ShapeType::Rectangle,     "[",  "]"  },
        { ShapeType::Rounded,       "(",  ")"  },
        { ShapeType::Rhombus,       "{",  "}"  },
        { ShapeType::Asymmetric,    ">",  "]"  }
    };

    // Extract ID first: letters, digits, underscores, dashes
    static const QRegularExpression idRegex(R"(^([A-Za-z0-9_\-]+))");
    auto match = idRegex.match(s);
    if (!match.hasMatch()) return false;

    QString id = match.captured(1);
    QString afterId = s.mid(id.length()).trimmed();

    for (const auto &p : patterns) {
        if (afterId.startsWith(p.openTag)) {
            int closeIdx = afterId.indexOf(p.closeTag, QString(p.openTag).length());
            if (closeIdx != -1) {
                QString label = afterId.mid(QString(p.openTag).length(), closeIdx - QString(p.openTag).length());
                outNode.id = id;
                outNode.label = stripQuotes(label);
                outNode.shape = p.type;
                outRest = afterId.mid(closeIdx + QString(p.closeTag).length()).trimmed();
                return true;
            }
        }
    }

    // Bare node ID
    outNode.id = id;
    outNode.label = id;
    outNode.shape = ShapeType::Rectangle;
    outRest = afterId;
    return true;
}

bool FlowchartParser::parse(const QString &source, FlowchartGraph &graph, QString *errorMessage)
{
    graph.nodes.clear();
    graph.edges.clear();
    graph.subgraphs.clear();
    graph.direction = Direction::TopToBottom;

    QMap<QString, int> nodeIndexMap;
    auto getOrCreateNode = [&](const Node &nodeSpec) -> QString {
        if (!nodeIndexMap.contains(nodeSpec.id)) {
            nodeIndexMap[nodeSpec.id] = graph.nodes.size();
            graph.nodes.append(nodeSpec);
        } else {
            // Update label/shape if current spec has non-default or explicit label
            int idx = nodeIndexMap[nodeSpec.id];
            if (nodeSpec.label != nodeSpec.id || nodeSpec.shape != ShapeType::Rectangle) {
                graph.nodes[idx].label = nodeSpec.label;
                graph.nodes[idx].shape = nodeSpec.shape;
            }
        }
        return nodeSpec.id;
    };

    QStringList lines = source.split('\n');
    bool headerParsed = false;
    Subgraph *currentSubgraph = nullptr;

    for (QString line : lines) {
        line = line.trimmed();
        if (line.isEmpty() || line.startsWith("%%")) {
            continue; // Skip comments and empty lines
        }

        // Header direction parsing
        if (!headerParsed) {
            static const QRegularExpression headerRegex(R"(^(graph|flowchart)\s+(TD|TB|BT|LR|RL))", QRegularExpression::CaseInsensitiveOption);
            auto hMatch = headerRegex.match(line);
            if (hMatch.hasMatch()) {
                QString dir = hMatch.captured(2).toUpper();
                if (dir == "TD" || dir == "TB") graph.direction = Direction::TopToBottom;
                else if (dir == "BT") graph.direction = Direction::BottomToTop;
                else if (dir == "LR") graph.direction = Direction::LeftToRight;
                else if (dir == "RL") graph.direction = Direction::RightToLeft;
                headerParsed = true;
                continue;
            }
            headerParsed = true; // allow implicit header
        }

        // Subgraph begin
        static const QRegularExpression subStartRegex(R"(^subgraph\s+([A-Za-z0-9_\-]+)?(?:\s*\[(.*)\]|\s+(.*))?)", QRegularExpression::CaseInsensitiveOption);
        if (line.startsWith("subgraph", Qt::CaseInsensitive)) {
            auto subMatch = subStartRegex.match(line);
            Subgraph sg;
            if (subMatch.hasMatch()) {
                sg.id = subMatch.captured(1).trimmed();
                QString title = !subMatch.captured(2).isEmpty() ? subMatch.captured(2) : subMatch.captured(3);
                sg.title = stripQuotes(title);
                if (sg.id.isEmpty()) sg.id = QString("sub_%1").arg(graph.subgraphs.size() + 1);
                if (sg.title.isEmpty()) sg.title = sg.id;
            }
            graph.subgraphs.append(sg);
            currentSubgraph = &graph.subgraphs.last();
            continue;
        }

        // Subgraph end
        if (line.compare("end", Qt::CaseInsensitive) == 0) {
            currentSubgraph = nullptr;
            continue;
        }

        // Parse node connections on this line
        // E.g. A[Start] -->|Yes| B(Process) --> C{Decision}
        QString current = line;
        Node lastNode;
        bool hasLastNode = false;

        while (!current.isEmpty()) {
            Node node;
            QString rest;
            if (!parseNodeSpec(current, node, rest)) {
                break;
            }

            QString nodeId = getOrCreateNode(node);
            if (currentSubgraph) {
                if (!currentSubgraph->nodeIds.contains(nodeId)) {
                    currentSubgraph->nodeIds.append(nodeId);
                }
            }

            current = rest.trimmed();
            if (current.isEmpty()) {
                break;
            }

            // Look for arrow connectors
            // Pattern: connector + optional label
            // E.g.: -->|label|, -- label -->, -.->|label|, ==>|label|, ---
            static const QRegularExpression arrowRegex(
                R"(^(-->|---|-.->|-.-|==>|===|--\s*([^-\n]+?)\s*-->|-\.\s*([^\.\n]+?)\s*\.->|==\s*([^=\n]+?)\s*==>)(?:\|([^|\n]+)\|)?)"
            );

            auto arrowMatch = arrowRegex.match(current);
            if (arrowMatch.hasMatch()) {
                QString fullMatch = arrowMatch.captured(0);
                QString rawConnector = arrowMatch.captured(1);
                QString midLabel1 = arrowMatch.captured(2);
                QString midLabel2 = arrowMatch.captured(3);
                QString midLabel3 = arrowMatch.captured(4);
                QString pipeLabel = arrowMatch.captured(5);

                QString label = !pipeLabel.isEmpty() ? pipeLabel :
                                !midLabel1.isEmpty() ? midLabel1 :
                                !midLabel2.isEmpty() ? midLabel2 : midLabel3;
                label = stripQuotes(label);

                EdgeType edgeType = EdgeType::SolidArrow;
                if (rawConnector.startsWith("-.->") || rawConnector.contains(".->")) edgeType = EdgeType::DottedArrow;
                else if (rawConnector.startsWith("-.-")) edgeType = EdgeType::DottedLine;
                else if (rawConnector.startsWith("==>") || rawConnector.contains("==>")) edgeType = EdgeType::ThickArrow;
                else if (rawConnector.startsWith("===")) edgeType = EdgeType::ThickLine;
                else if (rawConnector.startsWith("---")) edgeType = EdgeType::SolidLine;
                else edgeType = EdgeType::SolidArrow;

                current = current.mid(fullMatch.length()).trimmed();

                // Next node in chain
                Node nextNode;
                QString nextRest;
                if (parseNodeSpec(current, nextNode, nextRest)) {
                    QString nextId = getOrCreateNode(nextNode);
                    if (currentSubgraph && !currentSubgraph->nodeIds.contains(nextId)) {
                        currentSubgraph->nodeIds.append(nextId);
                    }

                    Edge edge;
                    edge.fromId = nodeId;
                    edge.toId = nextId;
                    edge.label = label;
                    edge.type = edgeType;
                    graph.edges.append(edge);

                    // For chaining: current becomes nextNode and continue
                    current = current; // will be parsed in next loop
                    // but we consumed arrow, so current starts at nextNode
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }

    if (graph.nodes.isEmpty()) {
        if (errorMessage) *errorMessage = "No valid nodes found in diagram.";
        return false;
    }

    return true;
}

} // namespace Diagram
