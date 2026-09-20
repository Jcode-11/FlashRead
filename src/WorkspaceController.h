#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariantList>

class DocumentController;

class WorkspaceController final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList recentFiles READ recentFiles NOTIFY workspaceChanged)
    Q_PROPERTY(QVariantList folders READ folders NOTIFY workspaceChanged)
    Q_PROPERTY(QVariantList workspaceItems READ workspaceItems NOTIFY workspaceChanged)
    Q_PROPERTY(QVariantList recentItems READ recentItems NOTIFY workspaceChanged)
    Q_PROPERTY(QString workspaceTitle READ workspaceTitle NOTIFY workspaceChanged)
    Q_PROPERTY(QVariantList folderFiles READ folderFiles NOTIFY workspaceChanged)
    Q_PROPERTY(QString selectedFolder READ selectedFolder NOTIFY workspaceChanged)
    Q_PROPERTY(bool folderViewActive READ folderViewActive NOTIFY workspaceChanged)
    Q_PROPERTY(QString themeId READ themeId WRITE setThemeId NOTIFY themeChanged)
    Q_PROPERTY(int sidebarWidth READ sidebarWidth WRITE setSidebarWidth NOTIFY sidebarWidthChanged)
    Q_PROPERTY(bool sidebarVisible READ sidebarVisible WRITE setSidebarVisible NOTIFY sidebarVisibleChanged)

public:
    explicit WorkspaceController(DocumentController *documentController, QObject *parent = nullptr);

    QVariantList recentFiles() const;
    QVariantList folders() const;
    QVariantList workspaceItems() const;
    QVariantList recentItems() const;
    QString workspaceTitle() const;
    QVariantList folderFiles() const;
    QString selectedFolder() const;
    bool folderViewActive() const;
    QString themeId() const;
    int sidebarWidth() const;
    bool sidebarVisible() const;

    Q_INVOKABLE void handleUrl(const QUrl &url);
    Q_INVOKABLE void openPath(const QString &path);
    Q_INVOKABLE void openFolderDocument(const QString &path);
    Q_INVOKABLE void openFolder(const QString &path);
    Q_INVOKABLE void toggleFolder(const QString &path);
    Q_INVOKABLE void removeFolder(const QString &path);
    Q_INVOKABLE void removeRecentFile(const QString &path);
    Q_INVOKABLE void removeHistoryItem(const QString &path);
    Q_INVOKABLE void copyPath(const QString &path);
    Q_INVOKABLE void revealPath(const QString &path);
    Q_INVOKABLE QVariantList searchRecent(const QString &query) const;
    Q_INVOKABLE void setThemeId(const QString &themeId);
    Q_INVOKABLE void setSidebarWidth(int width);
    Q_INVOKABLE void setSidebarVisible(bool visible);

signals:
    void workspaceChanged();
    void themeChanged();
    void sidebarWidthChanged();
    void sidebarVisibleChanged();

private:
    void addFolder(const QString &path);
    void rememberFile(const QString &path);
    bool touchRecent(const QString &path, const QString &type, bool emitSignal = true);
    void refreshFolderFiles();
    void loadSettings();
    void saveSettings();

    DocumentController *m_documentController;
    QStringList m_recentFilePaths;
    QStringList m_folderPaths;
    QVariantList m_recentEntries;
    QVariantList m_visibleEntries;
    QVariantList m_folderFiles;
    QString m_selectedFolder;
    QString m_expandedFolder;
    QString m_workspaceFilePath;
    bool m_folderViewActive = false;
    QString m_themeId = QStringLiteral("github-light");
    int m_sidebarWidth = 240;
    bool m_sidebarVisible = true;
};
