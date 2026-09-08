#pragma once

#include <QByteArray>
#include <QString>

class MarkdownRenderer final
{
public:
    [[nodiscard]] static QString render(const QByteArray &source);
};
