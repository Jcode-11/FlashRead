#include "WorkspaceController.h"

#include "DocumentController.h"

#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QTimer>
#include <QVariantMap>

namespace {
constexpr qsizetype RecentFileLimit = 12;
constexpr qsizetype FolderLimit = 8;

bool isKnownTheme(const QString &themeId)
{
    return themeId == "github-light" || themeId == "github-dark"
        || themeId == "vscode-dark" || themeId == "paper";
}

QString displayName(const QFileInfo &fileInfo)
{
    const QString name = fileInfo.fileName();
    return name.isEmpty() ? fileInfo.absoluteFilePath() : name;
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
    for (const QString &path : m_recentFilePaths) {
        const QFileInfo fileInfo(path);
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
    for (const QString &path : m_folderPaths) {
        const QFileInfo folderInfo(path);
        if (!folderInfo.isDir()) {
            continue;
        }
        folders.append(QVariantMap {{"title", displayName(folderInfo)}, {"path", folderInfo.absoluteFilePath()}});
    }
    return folders;
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
    rememberFile(fileInfo.absoluteFilePath());
}

void WorkspaceController::openFolder(const QString &path)
{
    const QFileInfo folderInfo(path);
    if (!folderInfo.isDir()) return;
    addFolder(folderInfo.absoluteFilePath());
    m_selectedFolder = folderInfo.absoluteFilePath();
    m_folderViewActive = true;
    refreshFolderFiles();
    emit workspaceChanged();
}

void WorkspaceController::removeFolder(const QString &path)
{
    const QString normalizedPath = QFileInfo(path).absoluteFilePath();
    if (!m_folderPaths.removeAll(normalizedPath)) return;
    if (m_selectedFolder == normalizedPath) {
        m_selectedFolder.clear();
        m_folderFiles.clear();
        m_folderViewActive = false;
    }
    emit workspaceChanged();
    QTimer::singleShot(0, this, [this]() { saveSettings(); });
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
    m_recentFilePaths.removeAll(path);
    m_recentFilePaths.prepend(path);
    while (m_recentFilePaths.size() > RecentFileLimit) m_recentFilePaths.removeLast();
    emit workspaceChanged();
    QTimer::singleShot(0, this, [this]() { saveSettings(); });
}

void WorkspaceController::refreshFolderFiles()
{
    m_folderFiles.clear();
    const QDir folder(m_selectedFolder);
    const QFileInfoList entries = folder.entryInfoList({"*.md", "*.markdown", "*.mdown", "*.txt"},
        QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
    constexpr qsizetype FileLimit = 200;
    for (const QFileInfo &fileInfo : entries) {
        if (m_folderFiles.size() == FileLimit) break;
        m_folderFiles.append(QVariantMap {{"title", displayName(fileInfo)}, {"path", fileInfo.absoluteFilePath()},
            {"kind", fileInfo.suffix().toUpper()}});
    }
}

void WorkspaceController::loadSettings()
{
    QSettings settings;
    m_recentFilePaths = settings.value("workspace/recentFiles").toStringList();
    m_folderPaths = settings.value("workspace/folders").toStringList();
    const QString savedTheme = settings.value("appearance/themeId", m_themeId).toString();
    if (isKnownTheme(savedTheme)) m_themeId = savedTheme;
}

void WorkspaceController::saveSettings()
{
    QSettings settings;
    settings.setValue("workspace/recentFiles", m_recentFilePaths);
    settings.setValue("workspace/folders", m_folderPaths);
    settings.setValue("appearance/themeId", m_themeId);
}
