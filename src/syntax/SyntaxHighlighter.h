#pragma once

#include <QString>
#include <QStringList>
#include <QSet>

namespace Syntax {

enum class Language {
    Unknown,
    Plain,
    Cpp,
    C,
    CSharp,
    Java,
    Kotlin,
    Go,
    Rust,
    Python,
    JavaScript,
    TypeScript,
    Json,
    Yaml,
    Toml,
    Html,
    Xml,
    Css,
    Scss,
    Sql,
    Bash,
    PowerShell,
    Batch,
    Diff,
    Markdown,
    Php,
    Ruby
};

class SyntaxHighlighter
{
public:
    static Language detectLanguage(const QString &langName);
    static QString languageDisplayName(Language lang);
    static QString highlightToHtml(const QString &source, const QString &langName);

private:
    static QString highlightCode(const QString &source, Language lang);
    static QString highlightGeneric(const QString &source, const QSet<QString> &keywords,
                                   const QSet<QString> &types, const QSet<QString> &constants,
                                   bool hasPreprocessor, bool hasHashComments);
    static QString highlightJson(const QString &source);
    static QString highlightYaml(const QString &source);
    static QString highlightHtmlXml(const QString &source);
    static QString highlightCss(const QString &source);
    static QString highlightSql(const QString &source);
    static QString highlightBash(const QString &source);
    static QString highlightPowerShell(const QString &source);
    static QString highlightDiff(const QString &source);
    static QString highlightMarkdown(const QString &source);
};

} // namespace Syntax
