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
    Q_PROPERTY(QVariantList folderFiles READ folderFiles NOTIFY workspaceChanged)
    Q_PROPERTY(QString selectedFolder READ selectedFolder NOTIFY workspaceChanged)
    Q_PROPERTY(bool folderViewActive READ folderViewActive NOTIFY workspaceChanged)
    Q_PROPERTY(QString themeId READ themeId WRITE setThemeId NOTIFY themeChanged)

public:
    explicit WorkspaceController(DocumentController *documentController, QObject *parent = nullptr);

    QVariantList recentFiles() const;
    QVariantList folders() const;
    QVariantList folderFiles() const;
    QString selectedFolder() const;
    bool folderViewActive() const;
    QString themeId() const;

    Q_INVOKABLE void handleUrl(const QUrl &url);
    Q_INVOKABLE void openPath(const QString &path);
    Q_INVOKABLE void openFolder(const QString &path);
    Q_INVOKABLE void removeFolder(const QString &path);
    Q_INVOKABLE void setThemeId(const QString &themeId);

signals:
    void workspaceChanged();
    void themeChanged();

private:
    void addFolder(const QString &path);
    void rememberFile(const QString &path);
    void refreshFolderFiles();
    void loadSettings();
    void saveSettings();

    DocumentController *m_documentController;
    QStringList m_recentFilePaths;
    QStringList m_folderPaths;
    QVariantList m_folderFiles;
    QString m_selectedFolder;
    bool m_folderViewActive = false;
    QString m_themeId = QStringLiteral("github-light");
};
