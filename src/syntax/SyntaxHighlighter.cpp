#include "SyntaxHighlighter.h"

#include <QRegularExpression>
#include <QStringConverter>

namespace Syntax {

namespace {

QString escapeHtml(const QString &str)
{
    QString escaped;
    escaped.reserve(str.size() + 16);
    for (const QChar ch : str) {
        if (ch == QLatin1Char('&')) escaped.append(QStringLiteral("&amp;"));
        else if (ch == QLatin1Char('<')) escaped.append(QStringLiteral("&lt;"));
        else if (ch == QLatin1Char('>')) escaped.append(QStringLiteral("&gt;"));
        else if (ch == QLatin1Char('"')) escaped.append(QStringLiteral("&quot;"));
        else escaped.append(ch);
    }
    return escaped;
}

bool isIdentStart(QChar ch)
{
    return ch.isLetter() || ch == QLatin1Char('_') || ch == QLatin1Char('$');
}

bool isIdentChar(QChar ch)
{
    return ch.isLetterOrNumber() || ch == QLatin1Char('_') || ch == QLatin1Char('$');
}

bool isNumberStart(QChar ch, QChar next)
{
    if (ch.isDigit()) return true;
    if (ch == QLatin1Char('.') && next.isDigit()) return true;
    return false;
}

} // namespace

Language SyntaxHighlighter::detectLanguage(const QString &langName)
{
    const QString lower = langName.trimmed().toLower();
    if (lower.isEmpty()) return Language::Plain;
    if (lower == "cpp" || lower == "c++" || lower == "cxx" || lower == "h" || lower == "hpp" || lower == "cc") return Language::Cpp;
    if (lower == "c") return Language::C;
    if (lower == "cs" || lower == "csharp" || lower == "c#") return Language::CSharp;
    if (lower == "java") return Language::Java;
    if (lower == "kotlin" || lower == "kt" || lower == "kts") return Language::Kotlin;
    if (lower == "go" || lower == "golang") return Language::Go;
    if (lower == "rust" || lower == "rs") return Language::Rust;
    if (lower == "python" || lower == "py") return Language::Python;
    if (lower == "javascript" || lower == "js" || lower == "jsx" || lower == "mjs" || lower == "cjs") return Language::JavaScript;
    if (lower == "typescript" || lower == "ts" || lower == "tsx" || lower == "mts" || lower == "cts") return Language::TypeScript;
    if (lower == "json" || lower == "jsonc") return Language::Json;
    if (lower == "yaml" || lower == "yml") return Language::Yaml;
    if (lower == "toml") return Language::Toml;
    if (lower == "html" || lower == "htm" || lower == "xhtml" || lower == "svg") return Language::Html;
    if (lower == "xml" || lower == "plist") return Language::Xml;
    if (lower == "css") return Language::Css;
    if (lower == "scss" || lower == "sass" || lower == "less") return Language::Scss;
    if (lower == "sql" || lower == "mysql" || lower == "pgsql" || lower == "postgres" || lower == "sqlite" || lower == "plsql") return Language::Sql;
    if (lower == "bash" || lower == "sh" || lower == "zsh") return Language::Bash;
    if (lower == "powershell" || lower == "ps1" || lower == "psm1") return Language::PowerShell;
    if (lower == "bat" || lower == "cmd" || lower == "batch") return Language::Batch;
    if (lower == "diff" || lower == "patch") return Language::Diff;
    if (lower == "markdown" || lower == "md") return Language::Markdown;
    if (lower == "php" || lower == "phtml") return Language::Php;
    if (lower == "ruby" || lower == "rb") return Language::Ruby;
    return Language::Unknown;
}

QString SyntaxHighlighter::languageDisplayName(Language lang)
{
    switch (lang) {
    case Language::Cpp: return QStringLiteral("C++");
    case Language::C: return QStringLiteral("C");
    case Language::CSharp: return QStringLiteral("C#");
    case Language::Java: return QStringLiteral("Java");
    case Language::Kotlin: return QStringLiteral("Kotlin");
    case Language::Go: return QStringLiteral("Go");
    case Language::Rust: return QStringLiteral("Rust");
    case Language::Python: return QStringLiteral("Python");
    case Language::JavaScript: return QStringLiteral("JavaScript");
    case Language::TypeScript: return QStringLiteral("TypeScript");
    case Language::Json: return QStringLiteral("JSON");
    case Language::Yaml: return QStringLiteral("YAML");
    case Language::Toml: return QStringLiteral("TOML");
    case Language::Html: return QStringLiteral("HTML");
    case Language::Xml: return QStringLiteral("XML");
    case Language::Css: return QStringLiteral("CSS");
    case Language::Scss: return QStringLiteral("SCSS");
    case Language::Sql: return QStringLiteral("SQL");
    case Language::Bash: return QStringLiteral("Bash");
    case Language::PowerShell: return QStringLiteral("PowerShell");
    case Language::Batch: return QStringLiteral("Batch");
    case Language::Diff: return QStringLiteral("Diff");
    case Language::Markdown: return QStringLiteral("Markdown");
    case Language::Php: return QStringLiteral("PHP");
    case Language::Ruby: return QStringLiteral("Ruby");
    default: return QString();
    }
}

QString SyntaxHighlighter::highlightToHtml(const QString &source, const QString &langName)
{
    Language lang = detectLanguage(langName);
    QString highlighted = highlightCode(source, lang);

    QString langTag = langName.trimmed().toLower();
    if (langTag.isEmpty()) {
        langTag = QStringLiteral("code");
    }
    QString displayTag = QStringLiteral("%1 ⌄").arg(langTag);

    // Calculate line count
    int lineCount = 1;
    for (const QChar &ch : source) {
        if (ch == QLatin1Char('\n')) ++lineCount;
    }
    if (source.endsWith(QLatin1Char('\n')) && lineCount > 1) {
        --lineCount;
    }

    QString lineNumbers;
    lineNumbers.reserve(lineCount * 4);
    for (int i = 1; i <= lineCount; ++i) {
        lineNumbers.append(QString::number(i));
        if (i < lineCount) lineNumbers.append(QLatin1Char('\n'));
    }

    QString html;
    html.reserve(highlighted.size() + lineNumbers.size() + 512);
    html.append(QStringLiteral(R"(<table class="code-box" width="100%" cellspacing="0" cellpadding="0">)"));
    html.append(QStringLiteral(R"(<tr><td colspan="2" class="code-header">)"));
    html.append(QStringLiteral(R"(<table width="100%" cellspacing="0" cellpadding="0"><tr>)"));
    html.append(QStringLiteral(R"(<td align="left"><span class="code-lang-tag">)")).append(escapeHtml(displayTag)).append(QStringLiteral(R"(</span></td>)"));
    html.append(QStringLiteral(R"(<td align="right"><span class="code-copy-btn">&#x2398;</span></td>)"));
    html.append(QStringLiteral(R"(</tr></table></td></tr>)"));

    html.append(QStringLiteral(R"(<tr>)"));
    html.append(QStringLiteral(R"(<td class="code-line-nums" valign="top" align="right"><pre class="line-num-pre">)"))
        .append(lineNumbers)
        .append(QStringLiteral(R"(</pre></td>)"));
    html.append(QStringLiteral(R"(<td class="code-content" valign="top" align="left"><pre class="code-pre">)"))
        .append(highlighted)
        .append(QStringLiteral(R"(</pre></td>)"));
    html.append(QStringLiteral(R"(</tr></table>)"));
    return html;
}

QString SyntaxHighlighter::highlightCode(const QString &source, Language lang)
{
    switch (lang) {
    case Language::Json:
        return highlightJson(source);
    case Language::Yaml:
    case Language::Toml:
        return highlightYaml(source);
    case Language::Html:
    case Language::Xml:
        return highlightHtmlXml(source);
    case Language::Css:
    case Language::Scss:
        return highlightCss(source);
    case Language::Sql:
        return highlightSql(source);
    case Language::Bash:
        return highlightBash(source);
    case Language::PowerShell:
        return highlightPowerShell(source);
    case Language::Diff:
        return highlightDiff(source);
    case Language::Markdown:
        return highlightMarkdown(source);
    case Language::Cpp: {
        static const QSet<QString> keywords = {
            "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor",
            "break", "case", "catch", "class", "compl", "concept", "const", "consteval",
            "constexpr", "constinit", "const_cast", "continue", "co_await", "co_return",
            "co_yield", "decltype", "default", "delete", "do", "dynamic_cast", "else",
            "enum", "explicit", "export", "extern", "final", "for", "friend", "goto", "if",
            "import", "inline", "module", "mutable", "namespace", "new", "noexcept",
            "not", "not_eq", "operator", "or", "or_eq", "override", "private", "protected",
            "public", "register", "reinterpret_cast", "requires", "return", "sizeof",
            "static", "static_assert", "static_cast", "struct", "switch", "template",
            "this", "thread_local", "throw", "try", "typedef", "typeid", "typename",
            "union", "using", "virtual", "volatile", "while", "xor", "xor_eq"
        };
        static const QSet<QString> types = {
            "bool", "char", "char8_t", "char16_t", "char32_t", "double", "float",
            "int", "long", "short", "signed", "unsigned", "void", "wchar_t",
            "size_t", "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t",
            "uint32_t", "uint64_t", "intptr_t", "uintptr_t", "ptrdiff_t", "QString",
            "QByteArray", "QList", "QVector", "QMap", "QHash", "QSet", "QObject",
            "QWidget", "std", "string", "vector", "map", "unordered_map", "set",
            "pair", "tuple", "shared_ptr", "unique_ptr", "weak_ptr", "optional", "variant"
        };
        static const QSet<QString> constants = { "true", "false", "nullptr", "NULL" };
        return highlightGeneric(source, keywords, types, constants, true, false);
    }
    case Language::C: {
        static const QSet<QString> keywords = {
            "auto", "break", "case", "const", "continue", "default", "do", "else",
            "enum", "extern", "for", "goto", "if", "inline", "register", "restrict",
            "return", "sizeof", "static", "struct", "switch", "typedef", "union",
            "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool",
            "_Complex", "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"
        };
        static const QSet<QString> types = {
            "char", "double", "float", "int", "long", "short", "signed", "unsigned", "void",
            "size_t", "ssize_t", "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t",
            "uint16_t", "uint32_t", "uint64_t", "intptr_t", "uintptr_t", "ptrdiff_t"
        };
        static const QSet<QString> constants = { "true", "false", "NULL" };
        return highlightGeneric(source, keywords, types, constants, true, false);
    }
    case Language::CSharp: {
        static const QSet<QString> keywords = {
            "abstract", "as", "async", "await", "base", "break", "case", "catch", "checked",
            "class", "const", "continue", "default", "delegate", "do", "else", "enum",
            "event", "explicit", "extern", "finally", "fixed", "for", "foreach", "goto",
            "if", "implicit", "in", "interface", "internal", "is", "lock", "namespace",
            "new", "operator", "out", "override", "params", "private", "protected", "public",
            "readonly", "record", "ref", "return", "sealed", "sizeof", "stackalloc",
            "static", "struct", "switch", "this", "throw", "try", "typeof", "unchecked",
            "unsafe", "using", "virtual", "void", "volatile", "while", "yield", "var"
        };
        static const QSet<QString> types = {
            "bool", "byte", "char", "decimal", "double", "float", "int", "long", "object",
            "sbyte", "short", "string", "uint", "ulong", "ushort", "dynamic", "Task",
            "List", "Dictionary", "IEnumerable", "Action", "Func"
        };
        static const QSet<QString> constants = { "true", "false", "null" };
        return highlightGeneric(source, keywords, types, constants, true, false);
    }
    case Language::Java: {
        static const QSet<QString> keywords = {
            "abstract", "assert", "break", "case", "catch", "class", "const", "continue",
            "default", "do", "else", "enum", "extends", "final", "finally", "for", "goto",
            "if", "implements", "import", "instanceof", "interface", "native", "new",
            "package", "private", "protected", "public", "return", "static", "strictfp",
            "super", "switch", "synchronized", "this", "throw", "throws", "transient",
            "try", "void", "volatile", "while", "record", "sealed", "permits", "var"
        };
        static const QSet<QString> types = {
            "boolean", "byte", "char", "double", "float", "int", "long", "short",
            "String", "Object", "Integer", "Long", "Boolean", "Double", "Float",
            "List", "ArrayList", "Map", "HashMap", "Set", "HashSet", "Optional"
        };
        static const QSet<QString> constants = { "true", "false", "null" };
        return highlightGeneric(source, keywords, types, constants, false, false);
    }
    case Language::Kotlin: {
        static const QSet<QString> keywords = {
            "as", "break", "class", "continue", "do", "else", "for", "fun", "if", "in",
            "interface", "is", "object", "package", "return", "super", "this", "throw",
            "try", "typealias", "val", "var", "when", "while", "by", "constructor",
            "delegate", "dynamic", "field", "file", "get", "init", "param", "property",
            "receiver", "set", "setparam", "where", "actual", "abstract", "annotation",
            "companion", "const", "crossinline", "data", "enum", "expect", "external",
            "final", "infix", "inline", "inner", "internal", "lateinit", "noinline",
            "open", "operator", "out", "override", "private", "protected", "public",
            "reified", "sealed", "suspend", "tailrec", "vararg"
        };
        static const QSet<QString> types = {
            "Byte", "Short", "Int", "Long", "Float", "Double", "Boolean", "Char",
            "String", "Array", "List", "Map", "Set", "Any", "Unit", "Nothing"
        };
        static const QSet<QString> constants = { "true", "false", "null" };
        return highlightGeneric(source, keywords, types, constants, false, false);
    }
    case Language::Go: {
        static const QSet<QString> keywords = {
            "break", "case", "chan", "const", "continue", "default", "defer", "else",
            "fallthrough", "for", "func", "go", "goto", "if", "import", "interface",
            "map", "package", "range", "return", "select", "struct", "switch", "type", "var"
        };
        static const QSet<QString> types = {
            "bool", "byte", "complex64", "complex128", "error", "float32", "float64",
            "int", "int8", "int16", "int32", "int64", "rune", "string",
            "uint", "uint8", "uint16", "uint32", "uint64", "uintptr", "any"
        };
        static const QSet<QString> constants = { "true", "false", "iota", "nil" };
        return highlightGeneric(source, keywords, types, constants, false, false);
    }
    case Language::Rust: {
        static const QSet<QString> keywords = {
            "as", "async", "await", "break", "const", "continue", "crate", "dyn", "else",
            "enum", "extern", "fn", "for", "if", "impl", "in", "let", "loop", "match",
            "mod", "move", "mut", "pub", "ref", "return", "self", "Self", "static",
            "struct", "super", "trait", "type", "unsafe", "use", "where", "while"
        };
        static const QSet<QString> types = {
            "bool", "char", "str", "u8", "u16", "u32", "u64", "u128", "usize",
            "i8", "i16", "i32", "i64", "i128", "isize", "f32", "f64",
            "String", "Vec", "Option", "Result", "Box", "Rc", "Arc", "RefCell", "Mutex"
        };
        static const QSet<QString> constants = { "true", "false", "Some", "None", "Ok", "Err" };
        return highlightGeneric(source, keywords, types, constants, false, false);
    }
    case Language::Python: {
        static const QSet<QString> keywords = {
            "and", "as", "assert", "async", "await", "break", "class", "continue", "def",
            "del", "elif", "else", "except", "finally", "for", "from", "global", "if",
            "import", "in", "is", "lambda", "nonlocal", "not", "or", "pass", "raise",
            "return", "try", "while", "with", "yield", "match", "case", "self", "cls"
        };
        static const QSet<QString> types = {
            "int", "float", "complex", "list", "tuple", "range", "str", "bytes", "bytearray",
            "memoryview", "set", "frozenset", "dict", "bool", "type", "object", "Any",
            "Optional", "Union", "Callable", "List", "Dict", "Set", "Tuple"
        };
        static const QSet<QString> constants = { "True", "False", "None", "Ellipsis", "NotImplemented" };
        return highlightGeneric(source, keywords, types, constants, false, true);
    }
    case Language::JavaScript:
    case Language::TypeScript: {
        static const QSet<QString> keywords = {
            "abstract", "arguments", "as", "async", "await", "break", "case", "catch",
            "class", "const", "continue", "debugger", "default", "delete", "do", "else",
            "enum", "export", "extends", "finally", "for", "from", "function", "get", "if",
            "implements", "import", "in", "instanceof", "interface", "is", "keyof", "let",
            "new", "null", "of", "package", "private", "protected", "public", "readonly",
            "return", "set", "static", "super", "switch", "this", "throw", "try", "typeof",
            "undefined", "var", "void", "while", "with", "yield", "type", "declare", "namespace"
        };
        static const QSet<QString> types = {
            "any", "boolean", "constructor", "declare", "get", "module", "require",
            "number", "set", "string", "symbol", "type", "from", "of", "unknown", "never",
            "bigint", "object", "Promise", "Array", "Record", "Map", "Set", "Function"
        };
        static const QSet<QString> constants = { "true", "false", "null", "undefined", "NaN", "Infinity" };
        return highlightGeneric(source, keywords, types, constants, false, false);
    }
    case Language::Php: {
        static const QSet<QString> keywords = {
            "abstract", "and", "array", "as", "break", "callable", "case", "catch", "class",
            "clone", "const", "continue", "declare", "default", "die", "do", "echo", "else",
            "elseif", "empty", "enddeclare", "endfor", "endforeach", "endif", "endswitch",
            "endwhile", "eval", "exit", "extends", "final", "finally", "fn", "for", "foreach",
            "function", "global", "goto", "if", "implements", "include", "include_once",
            "instanceof", "insteadof", "interface", "isset", "list", "match", "namespace",
            "new", "or", "print", "private", "protected", "public", "readonly", "require",
            "require_once", "return", "static", "switch", "throw", "trait", "try", "unset",
            "use", "var", "while", "xor", "yield"
        };
        static const QSet<QString> types = {
            "int", "float", "bool", "string", "void", "iterable", "object", "mixed", "never"
        };
        static const QSet<QString> constants = { "true", "false", "null", "TRUE", "FALSE", "NULL" };
        return highlightGeneric(source, keywords, types, constants, false, true);
    }
    case Language::Ruby: {
        static const QSet<QString> keywords = {
            "BEGIN", "END", "alias", "and", "begin", "break", "case", "class", "def",
            "defined?", "do", "else", "elsif", "end", "ensure", "for", "if", "in",
            "module", "next", "nil", "not", "or", "redo", "rescue", "retry", "return",
            "self", "super", "then", "undef", "unless", "until", "when", "while", "yield"
        };
        static const QSet<QString> types = {
            "Array", "Hash", "String", "Integer", "Float", "Symbol", "Object", "Class"
        };
        static const QSet<QString> constants = { "true", "false", "nil" };
        return highlightGeneric(source, keywords, types, constants, false, true);
    }
    default: {
        static const QSet<QString> genericKw = {
            "if", "else", "for", "while", "do", "return", "class", "struct", "function",
            "def", "fn", "import", "from", "export", "let", "var", "const", "new", "this"
        };
        static const QSet<QString> genericTypes = { "int", "float", "double", "string", "bool", "void" };
        static const QSet<QString> genericConst = { "true", "false", "null", "nil", "None" };
        return highlightGeneric(source, genericKw, genericTypes, genericConst, true, true);
    }
    }
}

QString SyntaxHighlighter::highlightGeneric(const QString &source, const QSet<QString> &keywords,
                                            const QSet<QString> &types, const QSet<QString> &constants,
                                            bool hasPreprocessor, bool hasHashComments)
{
    QString out;
    out.reserve(source.size() + source.size() / 2);

    const int len = source.size();
    int i = 0;

    while (i < len) {
        const QChar ch = source.at(i);
        const QChar next = (i + 1 < len) ? source.at(i + 1) : QChar();

        // 1. Line comment // or #
        if ((ch == QLatin1Char('/') && next == QLatin1Char('/')) || (hasHashComments && ch == QLatin1Char('#'))) {
            int start = i;
            while (i < len && source.at(i) != QLatin1Char('\n')) ++i;
            out.append(QStringLiteral(R"(<span class="hl-c">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // 2. Block comment /* ... */
        if (ch == QLatin1Char('/') && next == QLatin1Char('*')) {
            int start = i;
            i += 2;
            while (i < len && !(source.at(i) == QLatin1Char('*') && i + 1 < len && source.at(i + 1) == QLatin1Char('/'))) {
                ++i;
            }
            if (i < len) i += 2;
            out.append(QStringLiteral(R"(<span class="hl-c">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // 3. Preprocessor directive (#include, #define) or Decorator (@decorator)
        if ((hasPreprocessor && ch == QLatin1Char('#') && (i == 0 || source.at(i - 1) == QLatin1Char('\n') || source.at(i - 1).isSpace()))
            || (ch == QLatin1Char('@') && isIdentStart(next))) {
            int start = i;
            ++i;
            while (i < len && isIdentChar(source.at(i))) ++i;
            out.append(QStringLiteral(R"(<span class="hl-p">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // 4. Strings ("..." or '...' or `...` or triple quotes)
        if (ch == QLatin1Char('"') || ch == QLatin1Char('\'') || ch == QLatin1Char('`')) {
            const QChar quote = ch;
            // Check for triple quotes
            bool isTriple = (i + 2 < len && source.at(i + 1) == quote && source.at(i + 2) == quote);
            int start = i;
            if (isTriple) {
                i += 3;
                while (i < len) {
                    if (source.at(i) == quote && i + 2 < len && source.at(i + 1) == quote && source.at(i + 2) == quote) {
                        i += 3;
                        break;
                    }
                    if (source.at(i) == QLatin1Char('\\') && i + 1 < len) i += 2;
                    else ++i;
                }
            } else {
                ++i;
                while (i < len && source.at(i) != quote && source.at(i) != QLatin1Char('\n')) {
                    if (source.at(i) == QLatin1Char('\\') && i + 1 < len) i += 2;
                    else ++i;
                }
                if (i < len && source.at(i) == quote) ++i;
            }
            out.append(QStringLiteral(R"(<span class="hl-s">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // 5. Numbers (hex, float, int)
        if (isNumberStart(ch, next) && (i == 0 || !isIdentChar(source.at(i - 1)))) {
            int start = i;
            if (ch == QLatin1Char('0') && (next == QLatin1Char('x') || next == QLatin1Char('X') || next == QLatin1Char('b') || next == QLatin1Char('B'))) {
                i += 2;
                while (i < len && (source.at(i).isLetterOrNumber() || source.at(i) == QLatin1Char('.'))) ++i;
            } else {
                while (i < len && (source.at(i).isDigit() || source.at(i) == QLatin1Char('.') || source.at(i).isLetter())) ++i;
            }
            out.append(QStringLiteral(R"(<span class="hl-n">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // 6. Identifiers (Keywords, Types, Constants, Functions, Variables)
        if (isIdentStart(ch)) {
            int start = i;
            while (i < len && isIdentChar(source.at(i))) ++i;
            QString word = source.mid(start, i - start);

            // Check if followed by '(' -> Function call
            int peek = i;
            while (peek < len && source.at(peek).isSpace() && source.at(peek) != QLatin1Char('\n')) ++peek;
            bool isFunc = (peek < len && source.at(peek) == QLatin1Char('('));

            if (keywords.contains(word)) {
                out.append(QStringLiteral(R"(<span class="hl-k">)"))
                   .append(escapeHtml(word))
                   .append(QStringLiteral(R"(</span>)"));
            } else if (constants.contains(word)) {
                out.append(QStringLiteral(R"(<span class="hl-const">)"))
                   .append(escapeHtml(word))
                   .append(QStringLiteral(R"(</span>)"));
            } else if (types.contains(word) || (word.at(0).isUpper() && word.size() > 1 && !isFunc)) {
                out.append(QStringLiteral(R"(<span class="hl-t">)"))
                   .append(escapeHtml(word))
                   .append(QStringLiteral(R"(</span>)"));
            } else if (isFunc) {
                out.append(QStringLiteral(R"(<span class="hl-f">)"))
                   .append(escapeHtml(word))
                   .append(QStringLiteral(R"(</span>)"));
            } else {
                out.append(escapeHtml(word));
            }
            continue;
        }

        // 7. Operators & Special Characters
        if (ch == QLatin1Char('&')) out.append(QStringLiteral("&amp;"));
        else if (ch == QLatin1Char('<')) out.append(QStringLiteral("&lt;"));
        else if (ch == QLatin1Char('>')) out.append(QStringLiteral("&gt;"));
        else if (ch == QLatin1Char('"')) out.append(QStringLiteral("&quot;"));
        else out.append(ch);
        ++i;
    }

    return out;
}

QString SyntaxHighlighter::highlightJson(const QString &source)
{
    QString out;
    out.reserve(source.size() + source.size() / 2);
    const int len = source.size();
    int i = 0;

    while (i < len) {
        QChar ch = source.at(i);

        // String (key or value)
        if (ch == QLatin1Char('"')) {
            int start = i;
            ++i;
            while (i < len && source.at(i) != QLatin1Char('"') && source.at(i) != QLatin1Char('\n')) {
                if (source.at(i) == QLatin1Char('\\') && i + 1 < len) i += 2;
                else ++i;
            }
            if (i < len && source.at(i) == QLatin1Char('"')) ++i;
            QString str = source.mid(start, i - start);

            // Check if this string is a JSON key (followed by ':')
            int peek = i;
            while (peek < len && source.at(peek).isSpace()) ++peek;
            bool isKey = (peek < len && source.at(peek) == QLatin1Char(':'));

            if (isKey) {
                out.append(QStringLiteral(R"(<span class="hl-key">)"))
                   .append(escapeHtml(str))
                   .append(QStringLiteral(R"(</span>)"));
            } else {
                out.append(QStringLiteral(R"(<span class="hl-s">)"))
                   .append(escapeHtml(str))
                   .append(QStringLiteral(R"(</span>)"));
            }
            continue;
        }

        // Numbers
        if (ch.isDigit() || (ch == QLatin1Char('-') && i + 1 < len && source.at(i + 1).isDigit())) {
            int start = i;
            ++i;
            while (i < len && (source.at(i).isDigit() || source.at(i) == QLatin1Char('.') || source.at(i) == QLatin1Char('e') || source.at(i) == QLatin1Char('E') || source.at(i) == QLatin1Char('+') || source.at(i) == QLatin1Char('-'))) {
                ++i;
            }
            out.append(QStringLiteral(R"(<span class="hl-n">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // Identifiers (true, false, null)
        if (ch.isLetter()) {
            int start = i;
            while (i < len && source.at(i).isLetter()) ++i;
            QString word = source.mid(start, i - start);
            if (word == "true" || word == "false" || word == "null") {
                out.append(QStringLiteral(R"(<span class="hl-const">)")).append(word).append(QStringLiteral(R"(</span>)"));
            } else {
                out.append(escapeHtml(word));
            }
            continue;
        }

        // Punctuation
        if (ch == QLatin1Char('&')) out.append(QStringLiteral("&amp;"));
        else if (ch == QLatin1Char('<')) out.append(QStringLiteral("&lt;"));
        else if (ch == QLatin1Char('>')) out.append(QStringLiteral("&gt;"));
        else out.append(ch);
        ++i;
    }

    return out;
}

QString SyntaxHighlighter::highlightYaml(const QString &source)
{
    QString out;
    out.reserve(source.size() + source.size() / 2);
    const int len = source.size();
    int i = 0;

    while (i < len) {
        QChar ch = source.at(i);

        // Comment #
        if (ch == QLatin1Char('#')) {
            int start = i;
            while (i < len && source.at(i) != QLatin1Char('\n')) ++i;
            out.append(QStringLiteral(R"(<span class="hl-c">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // Strings
        if (ch == QLatin1Char('"') || ch == QLatin1Char('\'')) {
            QChar quote = ch;
            int start = i;
            ++i;
            while (i < len && source.at(i) != quote && source.at(i) != QLatin1Char('\n')) {
                if (source.at(i) == QLatin1Char('\\') && i + 1 < len) i += 2;
                else ++i;
            }
            if (i < len && source.at(i) == quote) ++i;
            out.append(QStringLiteral(R"(<span class="hl-s">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // Keys: (identifier before ':')
        if (isIdentStart(ch) || ch == QLatin1Char('-')) {
            int start = i;
            while (i < len && (isIdentChar(source.at(i)) || source.at(i) == QLatin1Char('-') || source.at(i) == QLatin1Char('_') || source.at(i) == QLatin1Char('.'))) ++i;
            QString word = source.mid(start, i - start);

            int peek = i;
            while (peek < len && source.at(peek).isSpace() && source.at(peek) != QLatin1Char('\n')) ++peek;
            bool isKey = (peek < len && source.at(peek) == QLatin1Char(':'));

            if (isKey) {
                out.append(QStringLiteral(R"(<span class="hl-key">)")).append(escapeHtml(word)).append(QStringLiteral(R"(</span>)"));
            } else if (word == "true" || word == "false" || word == "null" || word == "yes" || word == "no" || word == "on" || word == "off") {
                out.append(QStringLiteral(R"(<span class="hl-const">)")).append(escapeHtml(word)).append(QStringLiteral(R"(</span>)"));
            } else {
                out.append(escapeHtml(word));
            }
            continue;
        }

        if (ch == QLatin1Char('&')) out.append(QStringLiteral("&amp;"));
        else if (ch == QLatin1Char('<')) out.append(QStringLiteral("&lt;"));
        else if (ch == QLatin1Char('>')) out.append(QStringLiteral("&gt;"));
        else out.append(ch);
        ++i;
    }

    return out;
}

QString SyntaxHighlighter::highlightHtmlXml(const QString &source)
{
    QString out;
    out.reserve(source.size() + source.size() / 2);
    const int len = source.size();
    int i = 0;

    while (i < len) {
        // Comment <!-- ... -->
        if (source.mid(i, 4) == "<!--") {
            int start = i;
            i += 4;
            while (i < len && source.mid(i, 3) != "-->") ++i;
            if (i < len) i += 3;
            out.append(QStringLiteral(R"(<span class="hl-c">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // Tag: <tag ... > or </tag>
        if (source.at(i) == QLatin1Char('<')) {
            int start = i;
            out.append(QStringLiteral("&lt;"));
            ++i;

            if (i < len && (source.at(i) == QLatin1Char('/') || source.at(i) == QLatin1Char('!'))) {
                out.append(source.at(i));
                ++i;
            }

            // Tag name
            int tagStart = i;
            while (i < len && isIdentChar(source.at(i))) ++i;
            QString tagName = source.mid(tagStart, i - tagStart);
            if (!tagName.isEmpty()) {
                out.append(QStringLiteral(R"(<span class="hl-k">)")).append(escapeHtml(tagName)).append(QStringLiteral(R"(</span>)"));
            }

            // Inside tag attributes
            while (i < len && source.at(i) != QLatin1Char('>')) {
                QChar attrCh = source.at(i);
                if (attrCh.isSpace()) {
                    out.append(attrCh);
                    ++i;
                } else if (isIdentStart(attrCh)) {
                    int aStart = i;
                    while (i < len && (isIdentChar(source.at(i)) || source.at(i) == QLatin1Char('-'))) ++i;
                    QString attrName = source.mid(aStart, i - aStart);
                    out.append(QStringLiteral(R"(<span class="hl-t">)")).append(escapeHtml(attrName)).append(QStringLiteral(R"(</span>)"));
                } else if (attrCh == QLatin1Char('"') || attrCh == QLatin1Char('\'')) {
                    QChar quote = attrCh;
                    int sStart = i;
                    ++i;
                    while (i < len && source.at(i) != quote) ++i;
                    if (i < len) ++i;
                    out.append(QStringLiteral(R"(<span class="hl-s">)")).append(escapeHtml(source.mid(sStart, i - sStart))).append(QStringLiteral(R"(</span>)"));
                } else {
                    if (attrCh == QLatin1Char('&')) out.append(QStringLiteral("&amp;"));
                    else if (attrCh == QLatin1Char('=')) out.append(QStringLiteral(R"(<span class="hl-o">=</span>)"));
                    else out.append(attrCh);
                    ++i;
                }
            }

            if (i < len && source.at(i) == QLatin1Char('>')) {
                out.append(QStringLiteral("&gt;"));
                ++i;
            }
            continue;
        }

        // Text outside tags
        if (source.at(i) == QLatin1Char('&')) out.append(QStringLiteral("&amp;"));
        else out.append(source.at(i));
        ++i;
    }

    return out;
}

QString SyntaxHighlighter::highlightCss(const QString &source)
{
    static const QSet<QString> keywords = {
        "@import", "@media", "@keyframes", "@supports", "@font-face", "important"
    };
    static const QSet<QString> types = {
        "color", "background", "margin", "padding", "border", "font", "display", "position",
        "top", "left", "right", "bottom", "width", "height", "flex", "grid", "align-items",
        "justify-content", "opacity", "z-index", "transform", "transition", "animation"
    };
    static const QSet<QString> constants = { "none", "auto", "inherit", "initial", "unset", "block", "inline", "flex" };
    return highlightGeneric(source, keywords, types, constants, true, false);
}

QString SyntaxHighlighter::highlightSql(const QString &source)
{
    static const QSet<QString> keywords = {
        "SELECT", "FROM", "WHERE", "INSERT", "INTO", "VALUES", "UPDATE", "SET", "DELETE",
        "CREATE", "TABLE", "ALTER", "DROP", "INDEX", "VIEW", "JOIN", "INNER", "LEFT",
        "RIGHT", "FULL", "OUTER", "ON", "GROUP", "BY", "ORDER", "HAVING", "LIMIT", "OFFSET",
        "UNION", "ALL", "DISTINCT", "AS", "AND", "OR", "NOT", "IN", "LIKE", "IS", "NULL",
        "EXISTS", "BETWEEN", "CASE", "WHEN", "THEN", "ELSE", "END", "PRIMARY", "KEY",
        "FOREIGN", "REFERENCES", "DATABASE", "SCHEMA", "TRANSACTION", "COMMIT", "ROLLBACK",
        "select", "from", "where", "insert", "into", "values", "update", "set", "delete",
        "create", "table", "alter", "drop", "join", "left", "right", "inner", "on", "group", "by", "order"
    };
    static const QSet<QString> types = {
        "INT", "INTEGER", "BIGINT", "SMALLINT", "TINYINT", "VARCHAR", "CHAR", "TEXT",
        "DATE", "DATETIME", "TIMESTAMP", "TIME", "FLOAT", "DOUBLE", "DECIMAL", "NUMERIC",
        "BOOLEAN", "BLOB", "JSON", "UUID"
    };
    static const QSet<QString> constants = { "TRUE", "FALSE", "NULL", "true", "false", "null" };
    return highlightGeneric(source, keywords, types, constants, false, false);
}

QString SyntaxHighlighter::highlightBash(const QString &source)
{
    QString out;
    out.reserve(source.size() + source.size() / 2);

    const int len = source.size();
    int i = 0;

    static const QSet<QString> commands = {
        "npm", "yarn", "pnpm", "node", "npx", "git", "docker", "cargo", "pip", "python",
        "cd", "ls", "mkdir", "rm", "cp", "mv", "curl", "wget", "cat", "echo", "chmod",
        "chown", "sudo", "export", "source", "make", "cmake", "ninja", "brew", "systemctl",
        "grep", "find", "sed", "awk", "which", "if", "then", "else", "elif", "fi",
        "for", "while", "do", "done", "case", "esac", "return", "exit"
    };

    static const QSet<QString> subcommands = {
        "install", "run", "dev", "build", "start", "test", "publish", "commit",
        "push", "pull", "add", "clone", "checkout", "status", "diff", "log", "branch",
        "merge", "init", "create", "update", "upgrade", "remove", "clean"
    };

    while (i < len) {
        const QChar ch = source.at(i);
        const QChar next = (i + 1 < len) ? source.at(i + 1) : QChar();

        // 1. Comments #
        if (ch == QLatin1Char('#')) {
            int start = i;
            while (i < len && source.at(i) != QLatin1Char('\n')) ++i;
            out.append(QStringLiteral(R"(<span class="hl-c">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // 2. Strings
        if (ch == QLatin1Char('"') || ch == QLatin1Char('\'') || ch == QLatin1Char('`')) {
            const QChar quote = ch;
            int start = i;
            ++i;
            while (i < len && source.at(i) != quote && source.at(i) != QLatin1Char('\n')) {
                if (source.at(i) == QLatin1Char('\\') && i + 1 < len) i += 2;
                else ++i;
            }
            if (i < len && source.at(i) == quote) ++i;
            out.append(QStringLiteral(R"(<span class="hl-s">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // 3. Flags like --registry=https://... or -rf
        if (ch == QLatin1Char('-') && (next == QLatin1Char('-') || next.isLetter())) {
            int start = i;
            while (i < len && !source.at(i).isSpace() && source.at(i) != QLatin1Char('=')) ++i;
            QString flag = source.mid(start, i - start);
            out.append(QStringLiteral(R"(<span class="hl-p">)")).append(escapeHtml(flag)).append(QStringLiteral(R"(</span>)"));
            if (i < len && source.at(i) == QLatin1Char('=')) {
                out.append(QLatin1Char('='));
                ++i;
                int vStart = i;
                while (i < len && !source.at(i).isSpace()) ++i;
                QString val = source.mid(vStart, i - vStart);
                if (val.startsWith("http://") || val.startsWith("https://")) {
                    out.append(QStringLiteral(R"(<span class="hl-url">)")).append(escapeHtml(val)).append(QStringLiteral(R"(</span>)"));
                } else {
                    out.append(QStringLiteral(R"(<span class="hl-s">)")).append(escapeHtml(val)).append(QStringLiteral(R"(</span>)"));
                }
            }
            continue;
        }

        // 4. Variables ($VAR, ${VAR})
        if (ch == QLatin1Char('$')) {
            int start = i;
            ++i;
            if (i < len && source.at(i) == QLatin1Char('{')) {
                while (i < len && source.at(i) != QLatin1Char('}')) ++i;
                if (i < len) ++i;
            } else {
                while (i < len && (isIdentChar(source.at(i)) || source.at(i) == QLatin1Char('?') || source.at(i) == QLatin1Char('@'))) ++i;
            }
            out.append(QStringLiteral(R"(<span class="hl-t">)"))
               .append(escapeHtml(source.mid(start, i - start)))
               .append(QStringLiteral(R"(</span>)"));
            continue;
        }

        // 5. Identifiers (Commands / Subcommands / URLs / Words)
        if (isIdentStart(ch) || ch.isDigit()) {
            int start = i;
            while (i < len && (isIdentChar(source.at(i)) || source.at(i) == QLatin1Char(':') || source.at(i) == QLatin1Char('.') || source.at(i) == QLatin1Char('/'))) ++i;
            QString word = source.mid(start, i - start);

            if (word.startsWith("http://") || word.startsWith("https://")) {
                out.append(QStringLiteral(R"(<span class="hl-url">)")).append(escapeHtml(word)).append(QStringLiteral(R"(</span>)"));
            } else if (commands.contains(word)) {
                out.append(QStringLiteral(R"(<span class="hl-k">)")).append(escapeHtml(word)).append(QStringLiteral(R"(</span>)"));
            } else if (subcommands.contains(word) || word.startsWith("build:") || word.startsWith("run:")) {
                out.append(QStringLiteral(R"(<span class="hl-f">)")).append(escapeHtml(word)).append(QStringLiteral(R"(</span>)"));
            } else {
                out.append(escapeHtml(word));
            }
            continue;
        }

        // Punctuation
        if (ch == QLatin1Char('&')) out.append(QStringLiteral("&amp;"));
        else if (ch == QLatin1Char('<')) out.append(QStringLiteral("&lt;"));
        else if (ch == QLatin1Char('>')) out.append(QStringLiteral("&gt;"));
        else if (ch == QLatin1Char('"')) out.append(QStringLiteral("&quot;"));
        else out.append(ch);
        ++i;
    }

    return out;
}

QString SyntaxHighlighter::highlightPowerShell(const QString &source)
{
    static const QSet<QString> keywords = {
        "if", "else", "elseif", "switch", "while", "for", "foreach", "do", "until",
        "break", "continue", "return", "function", "filter", "workflow", "param",
        "try", "catch", "finally", "throw", "trap", "data", "dynamicparam", "in", "class", "enum"
    };
    static const QSet<QString> types = { "Get-Process", "Set-Item", "Get-ChildItem", "Write-Host", "Write-Output", "Test-Path" };
    static const QSet<QString> constants = { "$true", "$false", "$null" };
    return highlightGeneric(source, keywords, types, constants, false, true);
}

QString SyntaxHighlighter::highlightDiff(const QString &source)
{
    QString out;
    out.reserve(source.size() + 128);
    const QStringList lines = source.split('\n');

    for (int idx = 0; idx < lines.size(); ++idx) {
        const QString &line = lines.at(idx);
        if (line.startsWith('+') && !line.startsWith("+++")) {
            out.append(QStringLiteral(R"(<span class="hl-diff-add">)"))
               .append(escapeHtml(line))
               .append(QStringLiteral(R"(</span>)"));
        } else if (line.startsWith('-') && !line.startsWith("---")) {
            out.append(QStringLiteral(R"(<span class="hl-diff-del">)"))
               .append(escapeHtml(line))
               .append(QStringLiteral(R"(</span>)"));
        } else if (line.startsWith('@') || line.startsWith("diff ") || line.startsWith("index ")) {
            out.append(QStringLiteral(R"(<span class="hl-diff-hdr">)"))
               .append(escapeHtml(line))
               .append(QStringLiteral(R"(</span>)"));
        } else {
            out.append(escapeHtml(line));
        }
        if (idx + 1 < lines.size()) out.append(QLatin1Char('\n'));
    }
    return out;
}

QString SyntaxHighlighter::highlightMarkdown(const QString &source)
{
    QString out;
    out.reserve(source.size() + 128);
    const QStringList lines = source.split('\n');

    for (int idx = 0; idx < lines.size(); ++idx) {
        const QString &line = lines.at(idx);
        if (line.trimmed().startsWith('#')) {
            out.append(QStringLiteral(R"(<span class="hl-k">)")).append(escapeHtml(line)).append(QStringLiteral(R"(</span>)"));
        } else if (line.trimmed().startsWith('>') || line.trimmed().startsWith('-') || line.trimmed().startsWith('*')) {
            out.append(QStringLiteral(R"(<span class="hl-t">)")).append(escapeHtml(line)).append(QStringLiteral(R"(</span>)"));
        } else {
            out.append(escapeHtml(line));
        }
        if (idx + 1 < lines.size()) out.append(QLatin1Char('\n'));
    }
    return out;
}

} // namespace Syntax
