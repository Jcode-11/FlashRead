#include "MarkdownRenderer.h"

#include <md4c-html.h>

namespace {
void appendHtml(const MD_CHAR *text, MD_SIZE size, void *userdata)
{
    auto *output = static_cast<QByteArray *>(userdata);
    output->append(text, static_cast<qsizetype>(size));
}
}

QString MarkdownRenderer::render(const QByteArray &source)
{
    QByteArray html;
    const int result = md_html(
        source.constData(),
        static_cast<MD_SIZE>(source.size()),
        appendHtml,
        &html,
        MD_DIALECT_GITHUB | MD_FLAG_NOHTML,
        MD_HTML_FLAG_SKIP_UTF8_BOM);

    if (result == 0) {
        return QString::fromUtf8(html);
    }

    return QString::fromUtf8(source).toHtmlEscaped().replace('\n', "<br>");
}
