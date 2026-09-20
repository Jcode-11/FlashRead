#include "WorkspaceController.h"

#include "DocumentController.h"

#include <QDir>
#include <QClipboard>
#include <QDesktopServices>
#include <QDirIterator>
#include <QFileInfo>
#include <QGuiApplication>
#include <QSettings>
#include <QTimer>
#include <QVariantMap>

#include <algorithm>

namespace {
constexpr qsizetype FolderLimit = 8;
constexpr qsizetype RecentHistoryLimit = 30;
constexpr qsizetype VisibleHistoryLimit = 12;

bool isKnownTheme(const QString &themeId)
{
    return themeId == "github-light" || themeId == "github-dark"
        || themeId == "dracula" || themeId == "nord"
        || themeId == "one-dark" || themeId == "vitepress"
        || themeId == "vscode-dark" || themeId == "paper";
}

QString displayName(const QFileInfo &fileInfo)
{
    const QFileInfo cleanInfo(QDir::cleanPath(fileInfo.absoluteFilePath()));
    const QString name = cleanInfo.fileName();
    return name.isEmpty() ? cleanInfo.absoluteFilePath() : name;
}

bool isBrowsableDocument(const QDir &root, const QFileInfo &fileInfo)
{
    const QStringList components = root.relativeFilePath(fileInfo.absoluteFilePath()).split('/');
    for (const QString &component : components) {
        if (component.startsWith('.') || component == "build" || component == "dist"
            || component == "node_modules" || component == "_deps" || component == "CMakeFiles") {
            return false;
        }
    }
    return true;
}

QVariantMap recentEntry(const QString &path, const QString &type)
{
    return QVariantMap {{"path", QFileInfo(path).absoluteFilePath()}, {"type", type}};
}

bool entryMatches(const QVariant &entry, const QString &path)
{
    return entry.toMap().value("path").toString() == QFileInfo(path).absoluteFilePath();
}

}

WorkspaceController::WorkspaceController(DocumentController *documentController, QObject *parent)
    : QObject(parent)
    , m_documentController(documentController)
{
    loadSettings();
}

QVariantList WorkspaceController::recentFiles() const
{
    QVariantList files;
    for (const QVariant &entry : m_visibleEntries) {
        const QVariantMap item = entry.toMap();
        if (item.value("type").toString() != "file") continue;
        const QFileInfo fileInfo(item.value("path").toString());
        if (!fileInfo.isFile()) {
            continue;
        }
        files.append(QVariantMap {{"title", displayName(fileInfo)}, {"path", fileInfo.absoluteFilePath()},
            {"location", fileInfo.absolutePath()}});
    }
    return files;
}

QVariantList WorkspaceController::folders() const
{
    QVariantList folders;
    for (const QVariant &entry : m_visibleEntries) {
        const QVariantMap item = entry.toMap();
        if (item.value("type").toString() != "folder") continue;
        const QFileInfo folderInfo(item.value("path").toString());
        if (!folderInfo.isDir()) {
            continue;
        }
        folders.append(QVariantMap {{"title", displayName(folderInfo)}, {"path", folderInfo.absoluteFilePath()}});
    }
    return folders;
}

QVariantList WorkspaceController::workspaceItems() const
{
    QVariantList items;
    if (!m_expandedFolder.isEmpty()) {
        const QFileInfo folderInfo(m_expandedFolder);
        const bool available = folderInfo.isDir();
        items.append(QVariantMap {{"title", displayName(folderInfo)}, {"path", folderInfo.absoluteFilePath()},
            {"type", "workspaceFolder"}, {"available", available}, {"expanded", true}});
        if (!available) return items;
        for (const QVariant &file : m_folderFiles) {
            const QVariantMap fileItem = file.toMap();
            items.append(QVariantMap {{"title", fileItem.value("title")}, {"path", fileItem.value("path")},
                {"type", "folderFile"}, {"kind", fileItem.value("kind")}, {"available", true}});
        }
        return items;
    }

    if (m_workspaceFilePath.isEmpty()) return items;
    const QFileInfo fileInfo(m_workspaceFilePath);
    items.append(QVariantMap {{"title", displayName(fileInfo)}, {"path", fileInfo.absoluteFilePath()},
        {"type", "workspaceFile"}, {"available", fileInfo.isFile()}});
    return items;
}

QVariantList WorkspaceController::recentItems() const
{
    QVariantList items;
    for (const QVariant &entry : m_visibleEntries) {
        const QVariantMap entryMap = entry.toMap();
        const QString type = entryMap.value("type").toString();
        const QFileInfo itemInfo(entryMap.value("path").toString());
        items.append(QVariantMap {{"title", displayName(itemInfo)}, {"path", itemInfo.absoluteFilePath()},
            {"type", type}, {"available", type == "folder" ? itemInfo.isDir() : itemInfo.isFile()}});
    }
    return items;
}

QString WorkspaceController::workspaceTitle() const
{
    if (!m_expandedFolder.isEmpty()) return displayName(QFileInfo(m_expandedFolder));
    if (!m_workspaceFilePath.isEmpty()) return displayName(QFileInfo(m_workspaceFilePath));
    return {};
}

QVariantList WorkspaceController::folderFiles() const { return m_folderFiles; }
QString WorkspaceController::selectedFolder() const { return m_selectedFolder; }
bool WorkspaceController::folderViewActive() const { return m_folderViewActive; }
QString WorkspaceController::themeId() const { return m_themeId; }

void WorkspaceController::handleUrl(const QUrl &url)
{
    if (!url.isLocalFile()) return;
    const QFileInfo fileInfo(url.toLocalFile());
    if (fileInfo.isDir()) openFolder(fileInfo.absoluteFilePath());
    else openPath(fileInfo.absoluteFilePath());
}

void WorkspaceController::openPath(const QString &path)
{
    const QFileInfo fileInfo(path);
    m_folderViewActive = false;
    if (!fileInfo.isFile()) {
        m_documentController->openPath(path);
        emit workspaceChanged();
        return;
    }
    m_documentController->openPath(fileInfo.absoluteFilePath());
    m_workspaceFilePath = fileInfo.absoluteFilePath();
    m_selectedFolder.clear();
    m_expandedFolder.clear();
    m_folderFiles.clear();
    rememberFile(fileInfo.absoluteFilePath());
    emit workspaceChanged();
}

void WorkspaceController::openFolderDocument(const QString &path)
{
    const QFileInfo fileInfo(path);
    m_folderViewActive = false;
    if (!fileInfo.isFile()) {
        m_documentController->openPath(path);
        emit workspaceChanged();
        return;
    }

    m_documentController->openPath(fileInfo.absoluteFilePath());
    emit workspaceChanged();
}

void WorkspaceController::openFolder(const QString &path)
{
    toggleFolder(path);
}

void WorkspaceController::toggleFolder(const QString &path)
{
    const QFileInfo folderInfo(path);
    if (!folderInfo.isDir()) return;
    const QString normalizedPath = folderInfo.absoluteFilePath();
    addFolder(normalizedPath);
    touchRecent(normalizedPath, "folder", false);
    m_folderViewActive = false;
    m_workspaceFilePath.clear();
    if (m_expandedFolder == normalizedPath) {
        m_expandedFolder.clear();
        m_selectedFolder.clear();
        m_folderFiles.clear();
    } else {
        m_expandedFolder = normalizedPath;
        m_selectedFolder = normalizedPath;
        refreshFolderFiles();
    }
    emit workspaceChanged();
}

void WorkspaceController::removeFolder(const QString &path)
{
    const QString normalizedPath = QFileInfo(path).absoluteFilePath();
    if (!m_folderPaths.removeAll(normalizedPath)) return;
    if (m_selectedFolder == normalizedPath) {
        m_selectedFolder.clear();
        m_expandedFolder.clear();
        m_folderFiles.clear();
        m_folderViewActive = false;
    }
    emit workspaceChanged();
    QTimer::singleShot(0, this, [this]() { saveSettings(); });
}

void WorkspaceController::removeRecentFile(const QString &path)
{
    removeHistoryItem(path);
}

void WorkspaceController::removeHistoryItem(const QString &path)
{
    bool removed = false;
    for (auto iterator = m_recentEntries.begin(); iterator != m_recentEntries.end();) {
        if (entryMatches(*iterator, path)) {
            iterator = m_recentEntries.erase(iterator);
            removed = true;
        } else {
            ++iterator;
        }
    }
    for (auto iterator = m_visibleEntries.begin(); iterator != m_visibleEntries.end();) {
        if (entryMatches(*iterator, path)) {
            iterator = m_visibleEntries.erase(iterator);
            removed = true;
        } else {
            ++iterator;
        }
    }
    m_recentFilePaths.removeAll(QFileInfo(path).absoluteFilePath());
    m_folderPaths.removeAll(QFileInfo(path).absoluteFilePath());
    if (!removed) return;
    for (const QVariant &entry : m_recentEntries) {
        if (m_visibleEntries.size() >= VisibleHistoryLimit) break;
        bool isVisible = false;
        for (const QVariant &visibleEntry : m_visibleEntries) {
            if (entryMatches(visibleEntry, entry.toMap().value("path").toString())) {
                isVisible = true;
                break;
            }
        }
        if (!isVisible) m_visibleEntries.append(entry);
    }
    emit workspaceChanged();
    QTimer::singleShot(0, this, [this]() { saveSettings(); });
}

void WorkspaceController::copyPath(const QString &path)
{
    QGuiApplication::clipboard()->setText(QFileInfo(path).absoluteFilePath());
}

void WorkspaceController::revealPath(const QString &path)
{
    const QFileInfo fileInfo(path);
    const QString folderPath = fileInfo.isDir() ? fileInfo.absoluteFilePath() : fileInfo.absolutePath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
}

QVariantList WorkspaceController::searchRecent(const QString &query) const
{
    const QString needle = query.trimmed();
    QVariantList matches;
    for (const QVariant &entry : m_recentEntries) {
        const QVariantMap entryMap = entry.toMap();
        const QString path = entryMap.value("path").toString();
        const QString type = entryMap.value("type").toString();
        const QFileInfo itemInfo(path);
        const QString title = displayName(itemInfo);
        if (!needle.isEmpty() && !title.contains(needle, Qt::CaseInsensitive)
            && !path.contains(needle, Qt::CaseInsensitive)) {
            continue;
        }
        matches.append(QVariantMap {{"title", title}, {"path", itemInfo.absoluteFilePath()}, {"type", type},
            {"available", type == "folder" ? itemInfo.isDir() : itemInfo.isFile()}});
        if (matches.size() == 12) break;
    }
    return matches;
}

void WorkspaceController::setThemeId(const QString &themeId)
{
    if (!isKnownTheme(themeId) || m_themeId == themeId) return;
    m_themeId = themeId;
    emit themeChanged();
    QTimer::singleShot(0, this, [this]() { saveSettings(); });
}

void WorkspaceController::addFolder(const QString &path)
{
    const QString normalizedPath = QFileInfo(path).absoluteFilePath();
    m_folderPaths.removeAll(normalizedPath);
    m_folderPaths.prepend(normalizedPath);
    while (m_folderPaths.size() > FolderLimit) m_folderPaths.removeLast();
    QTimer::singleShot(0, this, [this]() { saveSettings(); });
}

void WorkspaceController::rememberFile(const QString &path)
{
    touchRecent(path, "file", false);
    QTimer::singleShot(0, this, [this]() { saveSettings(); });
}

bool WorkspaceController::touchRecent(const QString &path, const QString &type, bool emitSignal)
{
    const QVariantMap item = recentEntry(path, type);
    for (auto iterator = m_recentEntries.begin(); iterator != m_recentEntries.end();) {
        if (entryMatches(*iterator, path)) iterator = m_recentEntries.erase(iterator);
        else ++iterator;
    }
    m_recentEntries.prepend(item);
    while (m_recentEntries.size() > RecentHistoryLimit) m_recentEntries.removeLast();

    for (const QVariant &visibleItem : m_visibleEntries) {
        if (entryMatches(visibleItem, path)) return false;
    }
    if (m_visibleEntries.size() >= VisibleHistoryLimit) return false;
    m_visibleEntries.append(item);
    if (emitSignal) emit workspaceChanged();
    return true;
}

void WorkspaceController::refreshFolderFiles()
{
    m_folderFiles.clear();
    const QDir folder(m_selectedFolder);
    QFileInfoList entries;
    QDirIterator iterator(m_selectedFolder, {"*.md", "*.markdown", "*.mdown", "*.txt"},
        QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        if (isBrowsableDocument(folder, iterator.fileInfo())) entries.append(iterator.fileInfo());
    }
    std::sort(entries.begin(), entries.end(), [folder](const QFileInfo &left, const QFileInfo &right) {
        return folder.relativeFilePath(left.absoluteFilePath()).compare(
            folder.relativeFilePath(right.absoluteFilePath()), Qt::CaseInsensitive) < 0;
    });

    constexpr qsizetype FileLimit = 500;
    for (const QFileInfo &fileInfo : entries) {
        if (m_folderFiles.size() >= FileLimit) break;
        m_folderFiles.append(QVariantMap {{"title", displayName(fileInfo)}, {"path", fileInfo.absoluteFilePath()},
            {"kind", fileInfo.suffix().toUpper()},
            {"relativePath", folder.relativeFilePath(fileInfo.absoluteFilePath())}});
    }
}

void WorkspaceController::loadSettings()
{
    QSettings settings;
    m_recentFilePaths = settings.value("workspace/recentFiles").toStringList();
    m_folderPaths = settings.value("workspace/folders").toStringList();
    m_recentEntries = settings.value("workspace/recentEntries").toList();
    if (m_recentEntries.isEmpty()) {
        for (const QString &path : m_recentFilePaths) m_recentEntries.append(recentEntry(path, "file"));
        for (const QString &path : m_folderPaths) m_recentEntries.append(recentEntry(path, "folder"));
    }
    for (const QVariant &entry : m_recentEntries) {
        if (m_visibleEntries.size() == VisibleHistoryLimit) break;
        m_visibleEntries.append(entry);
    }
    const QString savedTheme = settings.value("appearance/themeId", m_themeId).toString();
    if (isKnownTheme(savedTheme)) m_themeId = savedTheme;
    m_sidebarWidth = settings.value("workspace/sidebarWidth", 240).toInt();
    if (m_sidebarWidth < 180 || m_sidebarWidth > 500) m_sidebarWidth = 240;
    m_sidebarVisible = settings.value("workspace/sidebarVisible", true).toBool();
}

void WorkspaceController::saveSettings()
{
    QSettings settings;
    settings.setValue("workspace/recentFiles", m_recentFilePaths);
    settings.setValue("workspace/folders", m_folderPaths);
    settings.setValue("workspace/recentEntries", m_recentEntries);
    settings.setValue("appearance/themeId", m_themeId);
    settings.setValue("workspace/sidebarWidth", m_sidebarWidth);
    settings.setValue("workspace/sidebarVisible", m_sidebarVisible);
}

int WorkspaceController::sidebarWidth() const { return m_sidebarWidth; }
bool WorkspaceController::sidebarVisible() const { return m_sidebarVisible; }

void WorkspaceController::setSidebarWidth(int width)
{
    int clamped = std::clamp(width, 180, 500);
    if (m_sidebarWidth != clamped) {
        m_sidebarWidth = clamped;
        emit sidebarWidthChanged();
        QTimer::singleShot(0, this, [this]() { saveSettings(); });
    }
}

void WorkspaceController::setSidebarVisible(bool visible)
{
    if (m_sidebarVisible != visible) {
        m_sidebarVisible = visible;
        emit sidebarVisibleChanged();
        QTimer::singleShot(0, this, [this]() { saveSettings(); });
    }
}
