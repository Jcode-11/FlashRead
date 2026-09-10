#include "SequenceDiagramParser.h"

#include <QFont>
#include <QFontMetrics>
#include <QMap>
#include <QRegularExpression>
#include <QStringList>
#include <algorithm>

namespace Diagram {

QString SequenceDiagramParser::escapeXml(const QString &str)
{
    QString res = str;
    res.replace('&', "&amp;");
    res.replace('<', "&lt;");
    res.replace('>', "&gt;");
    res.replace('"', "&quot;");
    res.replace('\'', "&apos;");
    return res;
}

bool SequenceDiagramParser::parse(const QString &source, SequenceGraph &graph, QString *errorMessage)
{
    graph.participants.clear();
    graph.messages.clear();
    graph.notes.clear();
    graph.autonumber = false;

    QMap<QString, int> participantMap;
    auto getOrCreateParticipant = [&](const QString &id, const QString &label, bool isActor = false) {
        if (!participantMap.contains(id)) {
            participantMap[id] = graph.participants.size();
            SequenceParticipant p;
            p.id = id;
            p.label = label.isEmpty() ? id : label;
            p.isActor = isActor;
            graph.participants.append(p);
        } else {
            int idx = participantMap[id];
            if (!label.isEmpty() && graph.participants[idx].label == graph.participants[idx].id) {
                graph.participants[idx].label = label;
            }
            if (isActor) graph.participants[idx].isActor = true;
        }
    };

    QStringList lines = source.split('\n');
    for (QString line : lines) {
        line = line.trimmed();
        if (line.isEmpty() || line.startsWith("%%")) continue;

        if (line.startsWith("sequenceDiagram", Qt::CaseInsensitive)) {
            continue;
        }
        if (line.compare("autonumber", Qt::CaseInsensitive) == 0) {
            graph.autonumber = true;
            continue;
        }

        // participant / actor: participant Alice as Alice Smith / actor Bob
        static const QRegularExpression partRegex(R"(^(participant|actor)\s+([A-Za-z0-9_\-]+)(?:\s+as\s+(.*))?)", QRegularExpression::CaseInsensitiveOption);
        auto partMatch = partRegex.match(line);
        if (partMatch.hasMatch()) {
            bool isActor = partMatch.captured(1).compare("actor", Qt::CaseInsensitive) == 0;
            QString id = partMatch.captured(2);
            QString label = partMatch.captured(3).trimmed();
            getOrCreateParticipant(id, label, isActor);
            continue;
        }

        // Notes: Note over A,B: text / Note over A: text / Note right of A: text / Note left of A: text
        static const QRegularExpression noteRegex(R"(^Note\s+(?:over|right of|left of)\s+([A-Za-z0-9_\-,\s]+?)\s*:\s*(.*))", QRegularExpression::CaseInsensitiveOption);
        auto noteMatch = noteRegex.match(line);
        if (noteMatch.hasMatch()) {
            QString partsStr = noteMatch.captured(1).trimmed();
            QString text = noteMatch.captured(2).trimmed();

            QStringList noteParts = partsStr.split(',', Qt::SkipEmptyParts);
            for (QString &np : noteParts) np = np.trimmed();

            if (!noteParts.isEmpty()) {
                QString firstPart = noteParts.first();
                QString secondPart = noteParts.size() > 1 ? noteParts[1] : "";

                getOrCreateParticipant(firstPart, "");
                if (!secondPart.isEmpty()) {
                    getOrCreateParticipant(secondPart, "");
                }

                SequenceNote note;
                note.overParticipant = firstPart;
                note.toParticipant = secondPart;
                note.text = text;
                note.messageIndex = graph.messages.size();
                graph.notes.append(note);
            }
            continue;
        }

        // Messages: A->>B: text / A-->>B: text / A->B: text / A-->B: text
        static const QRegularExpression msgRegex(R"(^([A-Za-z0-9_]+)\s*(->>|-->>|->|-->)\s*([A-Za-z0-9_]+)\s*:\s*(.*))");
        auto msgMatch = msgRegex.match(line);
        if (msgMatch.hasMatch()) {
            QString fromId = msgMatch.captured(1);
            QString arrow = msgMatch.captured(2);
            QString toId = msgMatch.captured(3);
            QString text = msgMatch.captured(4).trimmed();

            getOrCreateParticipant(fromId, "");
            getOrCreateParticipant(toId, "");

            SequenceMessage msg;
            msg.fromId = fromId;
            msg.toId = toId;
            msg.text = text;
            msg.isDotted = arrow.startsWith("--");
            msg.hasArrow = arrow.endsWith(">>") || arrow.endsWith(">");
            graph.messages.append(msg);
            continue;
        }
    }

    if (graph.participants.isEmpty()) {
        if (errorMessage) *errorMessage = "No valid participants or messages found in sequence diagram.";
        return false;
    }

    return true;
}

QString SequenceDiagramParser::generateSvg(SequenceGraph &graph, const ThemeColors &theme, qreal *outWidth, qreal *outHeight)
{
    QFont font("Segoe UI", 10);
    font.setWeight(QFont::Medium);
    QFontMetrics fm(font);

    QMap<QString, int> idToIdx;
    qreal currentX = 40.0;
    const qreal participantSpacing = 160.0;
    const qreal participantH = 38.0;

    for (int i = 0; i < graph.participants.size(); ++i) {
        auto &p = graph.participants[i];
        idToIdx[p.id] = i;
        int textW = fm.horizontalAdvance(p.label);
        p.width = std::max(100.0, textW + 36.0);
        p.x = currentX + p.width / 2.0;
        currentX += std::max(p.width, participantSpacing);
    }

    qreal totalW = currentX + 20.0;
    const qreal messageStepY = 52.0;
    qreal startY = 80.0;
    qreal totalH = startY + (graph.messages.size() + graph.notes.size() + 1) * messageStepY + 60.0;

    if (outWidth) *outWidth = totalW;
    if (outHeight) *outHeight = totalH;

    QString svg;
    svg += QString(R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 %1 %2" width="%1" height="%2">)").arg(totalW).arg(totalH);
    svg += "\n<defs>\n";
    svg += QString(R"(
        <marker id="seq-arrow" viewBox="0 0 10 10" refX="8" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
            <path d="M 0 1.5 L 9 5 L 0 8.5 z" fill="%1" />
        </marker>
    )").arg(theme.accent.name());
    svg += "</defs>\n";

    // Draw Participant Lifelines & Header boxes
    for (const auto &p : graph.participants) {
        // Lifeline (dashed)
        svg += QString(R"(<line x1="%1" y1="%2" x2="%1" y2="%3" stroke="%4" stroke-width="1.2" stroke-dasharray="4,4" />)")
               .arg(p.x).arg(20.0 + participantH).arg(totalH - 30.0).arg(theme.border.name());

        // Header Box
        qreal bx = p.x - p.width / 2.0;
        qreal by = 20.0;
        svg += QString(R"(
            <rect x="%1" y="%2" width="%3" height="%4" rx="6" fill="%5" stroke="%6" stroke-width="1.6" />
            <text x="%7" y="%8" dominant-baseline="central" text-anchor="middle" font-family="'Segoe UI', sans-serif" font-size="12" font-weight="600" fill="%9">%10</text>
        )")
        .arg(bx).arg(by).arg(p.width).arg(participantH).arg(theme.nodeBg.name(), theme.nodeBorder.name())
        .arg(p.x).arg(by + participantH / 2.0).arg(theme.text.name(), escapeXml(p.label));
    }

    // Draw Messages and Notes chronologically
    qreal curY = startY;
    int msgNum = 1;

    for (int i = 0; i <= graph.messages.size(); ++i) {
        // Check if there are notes at this step
        for (const auto &note : graph.notes) {
            if (note.messageIndex == i && idToIdx.contains(note.overParticipant)) {
                const auto &p1 = graph.participants[idToIdx[note.overParticipant]];
                qreal noteCenterX = p1.x;
                qreal nw = 150.0;

                if (!note.toParticipant.isEmpty() && idToIdx.contains(note.toParticipant)) {
                    const auto &p2 = graph.participants[idToIdx[note.toParticipant]];
                    noteCenterX = (p1.x + p2.x) / 2.0;
                    nw = std::abs(p2.x - p1.x) + 80.0;
                }

                int noteTextW = fm.horizontalAdvance(note.text);
                nw = std::max(nw, static_cast<qreal>(noteTextW + 30.0));
                qreal nh = 32.0;

                svg += QString(R"(
                    <rect x="%1" y="%2" width="%3" height="%4" rx="4" fill="%5" stroke="%6" stroke-width="1.2" />
                    <text x="%7" y="%8" dominant-baseline="central" text-anchor="middle" font-family="'Segoe UI', sans-serif" font-size="11" fill="%9">%10</text>
                )")
                .arg(noteCenterX - nw / 2.0).arg(curY - nh / 2.0).arg(nw).arg(nh)
                .arg(theme.surface.name(), theme.border.name())
                .arg(noteCenterX).arg(curY).arg(theme.text.name(), escapeXml(note.text));

                curY += messageStepY;
            }
        }

        if (i >= graph.messages.size()) break;

        const auto &msg = graph.messages[i];
        if (!idToIdx.contains(msg.fromId) || !idToIdx.contains(msg.toId)) continue;

        const auto &pFrom = graph.participants[idToIdx[msg.fromId]];
        const auto &pTo = graph.participants[idToIdx[msg.toId]];

        qreal x1 = pFrom.x;
        qreal x2 = pTo.x;
        qreal y = curY;

        QString msgText = (graph.autonumber ? QString("%1. ").arg(msgNum++) : "") + msg.text;
        QString dash = msg.isDotted ? "stroke-dasharray=\"4,3\"" : "";
        QString marker = msg.hasArrow ? "marker-end=\"url(#seq-arrow)\"" : "";

        if (pFrom.id == pTo.id) {
            // Self loop message
            qreal loopW = 40.0;
            qreal loopH = 26.0;
            svg += QString(R"(
                <path d="M %1 %2 L %3 %2 L %3 %4 L %1 %4" stroke="%5" stroke-width="1.6" fill="none" %6 %7 />
                <text x="%8" y="%9" dominant-baseline="central" font-family="'Segoe UI', sans-serif" font-size="11" fill="%10">%11</text>
            )")
            .arg(x1).arg(y).arg(x1 + loopW).arg(y + loopH).arg(theme.accent.name(), dash, marker)
            .arg(x1 + loopW + 8).arg(y + loopH / 2.0).arg(theme.text.name(), escapeXml(msgText));
            curY += 15.0;
        } else {
            // Message line
            svg += QString(R"(
                <line x1="%1" y1="%2" x2="%3" y2="%2" stroke="%4" stroke-width="1.6" %5 %6 />
                <text x="%7" y="%8" dominant-baseline="auto" text-anchor="middle" font-family="'Segoe UI', sans-serif" font-size="11" fill="%9">%10</text>
            )")
            .arg(x1).arg(y).arg(x2).arg(theme.accent.name(), dash, marker)
            .arg((x1 + x2) / 2.0).arg(y - 6.0).arg(theme.text.name(), escapeXml(msgText));
        }

        curY += messageStepY;
    }

    svg += "</svg>";
    return svg;
}

} // namespace Diagram
