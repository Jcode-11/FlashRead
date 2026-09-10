#pragma once

#include "DiagramTypes.h"
#include <QString>

namespace Diagram {

class SequenceDiagramParser
{
public:
    static bool parse(const QString &source, SequenceGraph &graph, QString *errorMessage = nullptr);
    static QString generateSvg(SequenceGraph &graph, const ThemeColors &theme, qreal *outWidth = nullptr, qreal *outHeight = nullptr);

private:
    static QString escapeXml(const QString &str);
};

} // namespace Diagram
