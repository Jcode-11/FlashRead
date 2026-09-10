#pragma once

#include "DiagramTypes.h"
#include <QString>

namespace Diagram {

class FlowchartParser
{
public:
    static bool parse(const QString &source, FlowchartGraph &graph, QString *errorMessage = nullptr);

private:
    static bool parseNodeSpec(const QString &token, Node &outNode, QString &outRest);
    static QString stripQuotes(const QString &str);
};

} // namespace Diagram
