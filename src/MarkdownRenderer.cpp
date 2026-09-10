#include "MarkdownRenderer.h"
#include "diagram/DiagramRenderer.h"

#include <md4c-html.h>
#include <QRegularExpression>

namespace {
void appendHtml(const MD_CHAR *text, MD_SIZE size, void *userdata)
{
    auto *output = static_cast<QByteArray *>(userdata);
    output->append(text, static_cast<qsizetype>(size));
}
} // namespace

QString MarkdownRenderer::render(const QByteArray &source)
{
    QString utf8Text = QString::fromUtf8(source);

    // Intercept ```mermaid or ```flowchart code blocks
    // Pattern: ```mermaid ... ``` or ~~~mermaid ... ~~~
    static const QRegularExpression mermaidRegex(
        R"((?:^|\n)(`{3,}|~{3,})\s*(?:mermaid|flowchart)\s*\n([\s\S]*?)\n\1)",
        QRegularExpression::CaseInsensitiveOption
    );

    QString processedText = utf8Text;
    QRegularExpressionMatchIterator it = mermaidRegex.globalMatch(utf8Text);

    // We collect replacements first or do replace from back to front
    struct Replacement {
        qsizetype start;
        qsizetype length;
        QString token;
    };
    QList<Replacement> replacements;

    while (it.hasNext()) {
        auto match = it.next();
        QString diagramCode = match.captured(2).trimmed();
        if (!diagramCode.isEmpty()) {
            QString diagramId = Diagram::DiagramRenderer::instance().registerDiagram(diagramCode);
            Replacement rep;
            rep.start = match.capturedStart();
            rep.length = match.capturedLength();
            // Ensure placeholder is separated by empty lines so Markdown treats it as its own block
            rep.token = QString("\n\nFLASHREADDIAGRAMTOKEN_%1_ENDTOKEN\n\n").arg(diagramId);
            replacements.append(rep);
        }
    }

    for (int i = replacements.size() - 1; i >= 0; --i) {
        const auto &rep = replacements[i];
        processedText.replace(rep.start, rep.length, rep.token);
    }

    QByteArray processedBytes = processedText.toUtf8();
    QByteArray html;
    const int result = md_html(
        processedBytes.constData(),
        static_cast<MD_SIZE>(processedBytes.size()),
        appendHtml,
        &html,
        MD_DIALECT_GITHUB | MD_FLAG_NOHTML,
        MD_HTML_FLAG_SKIP_UTF8_BOM);

    QString finalHtml;
    if (result == 0) {
        finalHtml = QString::fromUtf8(html);
    } else {
        finalHtml = QString::fromUtf8(processedBytes).toHtmlEscaped().replace('\n', "<br>");
    }

    // Replace diagram tokens with HTML image tags
    static const QRegularExpression tokenRegex(R"(<p>\s*FLASHREADDIAGRAMTOKEN_([a-f0-9]+)_ENDTOKEN\s*<\/p>|FLASHREADDIAGRAMTOKEN_([a-f0-9]+)_ENDTOKEN)");
    finalHtml.replace(tokenRegex, R"(<p align="center" style="margin: 22px 0;"><img src="image://diagram/\1\2" /></p>)");

    return finalHtml;
}
