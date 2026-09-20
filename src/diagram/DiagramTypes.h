#pragma once

#include <QColor>
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QString>

namespace Diagram {

enum class ShapeType {
    Rectangle,        // [text]
    Rounded,          // (text)
    Stadium,          // ([text])
    Subroutine,       // [[text]]
    Cylinder,         // [(text)]
    Circle,           // ((text))
    Asymmetric,       // >text]
    Rhombus,          // {text}
    Hexagon,          // {{text}}
    Parallelogram     // [/text/]
};

enum class EdgeType {
    SolidArrow,       // -->
    SolidLine,        // ---
    DottedArrow,      // -.->
    DottedLine,       // -.-
    ThickArrow,       // ==>
    ThickLine         // ===
};

enum class Direction {
    TopToBottom,      // TD, TB
    BottomToTop,      // BT
    LeftToRight,      // LR
    RightToLeft       // RL
};

struct Node {
    QString id;
    QString label;
    ShapeType shape = ShapeType::Rectangle;

    // Layout geometry
    qreal x = 0;
    qreal y = 0;
    qreal width = 120;
    qreal height = 44;
    int rank = 0;
    int order = 0;
};

struct Edge {
    QString fromId;
    QString toId;
    QString label;
    EdgeType type = EdgeType::SolidArrow;
    QList<QPointF> controlPoints;
};

struct Subgraph {
    QString id;
    QString title;
    QList<QString> nodeIds;
    QRectF bounds;
};

struct FlowchartGraph {
    Direction direction = Direction::TopToBottom;
    QList<Node> nodes;
    QList<Edge> edges;
    QList<Subgraph> subgraphs;
};

struct SequenceParticipant {
    QString id;
    QString label;
    bool isActor = false;
    qreal x = 0;
    qreal width = 100;
};

struct SequenceMessage {
    QString fromId;
    QString toId;
    QString text;
    bool isDotted = false;
    bool hasArrow = true;
};

struct SequenceNote {
    QString overParticipant;
    QString toParticipant;
    QString text;
    int messageIndex = 0;
};

struct SequenceGraph {
    QList<SequenceParticipant> participants;
    QList<SequenceMessage> messages;
    QList<SequenceNote> notes;
    bool autonumber = false;
};

struct ThemeColors {
    QColor background = QColor("#ffffff");
    QColor surface = QColor("#f6f8fa");
    QColor border = QColor("#d0d7de");
    QColor text = QColor("#1f2328");
    QColor mutedText = QColor("#656d76");
    QColor accent = QColor("#0969da");
    QColor edge = QColor("#57606a");
    QColor nodeBg = QColor("#ffffff");
    QColor nodeBorder = QColor("#0969da");
    QColor clusterBg = QColor("#f6f8fa");
    QColor clusterBorder = QColor("#d0d7de");

    static ThemeColors fromThemeId(const QString &themeId)
    {
        ThemeColors t;
        if (themeId == "github-dark") {
            t.background = QColor("#161b22");
            t.surface = QColor("#0d1117");
            t.border = QColor("#30363d");
            t.text = QColor("#e6edf3");
            t.mutedText = QColor("#8b949e");
            t.accent = QColor("#58a6ff");
            t.edge = QColor("#8b949e");
            t.nodeBg = QColor("#21262d");
            t.nodeBorder = QColor("#58a6ff");
            t.clusterBg = QColor("#0d1117");
            t.clusterBorder = QColor("#30363d");
        } else if (themeId == "dracula") {
            t.background = QColor("#1e1f29");
            t.surface = QColor("#282a36");
            t.border = QColor("#44475a");
            t.text = QColor("#f8f8f2");
            t.mutedText = QColor("#6272a4");
            t.accent = QColor("#bd93f9");
            t.edge = QColor("#8be9fd");
            t.nodeBg = QColor("#21222c");
            t.nodeBorder = QColor("#bd93f9");
            t.clusterBg = QColor("#282a36");
            t.clusterBorder = QColor("#44475a");
        } else if (themeId == "nord") {
            t.background = QColor("#242933");
            t.surface = QColor("#2e3440");
            t.border = QColor("#434c5e");
            t.text = QColor("#eceff4");
            t.mutedText = QColor("#7b88a1");
            t.accent = QColor("#88c0d0");
            t.edge = QColor("#81a1c1");
            t.nodeBg = QColor("#3b4252");
            t.nodeBorder = QColor("#88c0d0");
            t.clusterBg = QColor("#2e3440");
            t.clusterBorder = QColor("#434c5e");
        } else if (themeId == "one-dark") {
            t.background = QColor("#1e2227");
            t.surface = QColor("#282c34");
            t.border = QColor("#3e4451");
            t.text = QColor("#abb2bf");
            t.mutedText = QColor("#5c6370");
            t.accent = QColor("#61afef");
            t.edge = QColor("#98c379");
            t.nodeBg = QColor("#21252b");
            t.nodeBorder = QColor("#61afef");
            t.clusterBg = QColor("#282c34");
            t.clusterBorder = QColor("#3e4451");
        } else if (themeId == "vitepress") {
            t.background = QColor("#ffffff");
            t.surface = QColor("#f6f6f7");
            t.border = QColor("#e2e2e3");
            t.text = QColor("#213547");
            t.mutedText = QColor("#606067");
            t.accent = QColor("#10b981");
            t.edge = QColor("#606067");
            t.nodeBg = QColor("#ffffff");
            t.nodeBorder = QColor("#10b981");
            t.clusterBg = QColor("#f6f6f7");
            t.clusterBorder = QColor("#e2e2e3");
        } else if (themeId == "vscode-dark") {
            t.background = QColor("#1e1e1e");
            t.surface = QColor("#252526");
            t.border = QColor("#3c3c3c");
            t.text = QColor("#d4d4d4");
            t.mutedText = QColor("#9d9d9d");
            t.accent = QColor("#3794ff");
            t.edge = QColor("#9d9d9d");
            t.nodeBg = QColor("#2d2d2d");
            t.nodeBorder = QColor("#3794ff");
            t.clusterBg = QColor("#252526");
            t.clusterBorder = QColor("#3c3c3c");
        } else if (themeId == "paper") {
            t.background = QColor("#fffaf0");
            t.surface = QColor("#f3eddf");
            t.border = QColor("#d7c9b4");
            t.text = QColor("#3b3228");
            t.mutedText = QColor("#76695a");
            t.accent = QColor("#9a5b2d");
            t.edge = QColor("#76695a");
            t.nodeBg = QColor("#f7f0e1");
            t.nodeBorder = QColor("#9a5b2d");
            t.clusterBg = QColor("#f3eddf");
            t.clusterBorder = QColor("#d7c9b4");
        } else {
            // github-light default
            t.background = QColor("#ffffff");
            t.surface = QColor("#f6f8fa");
            t.border = QColor("#d0d7de");
            t.text = QColor("#1f2328");
            t.mutedText = QColor("#656d76");
            t.accent = QColor("#0969da");
            t.edge = QColor("#57606a");
            t.nodeBg = QColor("#ffffff");
            t.nodeBorder = QColor("#0969da");
            t.clusterBg = QColor("#f6f8fa");
            t.clusterBorder = QColor("#d0d7de");
        }
        return t;
    }
};

} // namespace Diagram
