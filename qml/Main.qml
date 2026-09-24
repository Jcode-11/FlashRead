import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import QtQuick.Layouts
import FlashRead.WebView 1.0

ApplicationWindow {
    id: window

    width: 1240
    height: 800
    minimumWidth: 860
    minimumHeight: 540
    visible: true
    title: (documentController.title.length > 0 ? documentController.title : "FlashRead")
        + (documentController.isModified ? " • (未保存)" : "") + " - FlashRead"

    function themeFor(id) {
        if (id === "github-dark") {
            return { window: "#0d1117", chrome: "#161b22", panel: "#161b22", reader: "#0d1117",
                border: "#30363d", text: "#e6edf3", muted: "#848d97", accent: "#2f81f7",
                hover: "#21262d", pressed: "#30363d", drop: "#1c2128", scroll: "#30363d",
                code: "#161b22", codeBorder: "#30363d", codeHeader: "#1c2128", codeLineBorder: "#262c36", quote: "#161b22", table: "#161b22" }
        }
        if (id === "dracula") {
            return { window: "#282a36", chrome: "#21222c", panel: "#21222c", reader: "#282a36",
                border: "#44475a", text: "#f8f8f2", muted: "#6272a4", accent: "#bd93f9",
                hover: "#343746", pressed: "#44475a", drop: "#282a36", scroll: "#44475a",
                code: "#1e1f29", codeBorder: "#44475a", codeHeader: "#242633", codeLineBorder: "#343746", quote: "#21222c", table: "#21222c" }
        }
        if (id === "nord") {
            return { window: "#2e3440", chrome: "#3b4252", panel: "#3b4252", reader: "#2e3440",
                border: "#434c5e", text: "#eceff4", muted: "#7b88a1", accent: "#88c0d0",
                hover: "#434c5e", pressed: "#4c566a", drop: "#3b4252", scroll: "#434c5e",
                code: "#242933", codeBorder: "#434c5e", codeHeader: "#2b303c", codeLineBorder: "#353c4a", quote: "#3b4252", table: "#3b4252" }
        }
        if (id === "one-dark") {
            return { window: "#282c34", chrome: "#21252b", panel: "#21252b", reader: "#282c34",
                border: "#3e4451", text: "#abb2bf", muted: "#5c6370", accent: "#61afef",
                hover: "#2c313a", pressed: "#3e4451", drop: "#21252b", scroll: "#3e4451",
                code: "#1e2227", codeBorder: "#3e4451", codeHeader: "#23272e", codeLineBorder: "#2c313a", quote: "#21252b", table: "#21252b" }
        }
        if (id === "vitepress") {
            return { window: "#ffffff", chrome: "#f6f6f7", panel: "#f6f6f7", reader: "#ffffff",
                border: "#e2e2e3", text: "#213547", muted: "#606067", accent: "#10b981",
                hover: "#f1f1f2", pressed: "#e2e2e3", drop: "#f6f6f7", scroll: "#e2e2e3",
                code: "#f6f8fa", codeBorder: "#e2e8f0", codeHeader: "#eef0f3", codeLineBorder: "#e2e8f0", quote: "#f6f6f7", table: "#f6f6f7" }
        }
        if (id === "vscode-dark") {
            return { window: "#1e1e1e", chrome: "#252526", panel: "#252526", reader: "#1e1e1e",
                border: "#3c3c3c", text: "#cccccc", muted: "#858585", accent: "#007acc",
                hover: "#2a2d2e", pressed: "#37373d", drop: "#252526", scroll: "#424242",
                code: "#1e1e1e", codeBorder: "#333333", codeHeader: "#252526", codeLineBorder: "#2a2d2e", quote: "#252526", table: "#252526" }
        }
        if (id === "paper") {
            return { window: "#fffaf0", chrome: "#f7f0e1", panel: "#f7f0e1", reader: "#fffaf0",
                border: "#d7c9b4", text: "#3b3228", muted: "#76695a", accent: "#9a5b2d",
                hover: "#ece1ce", pressed: "#dfcdae", drop: "#f7f0e1", scroll: "#d7c9b4",
                code: "#f4ece0", codeBorder: "#d8ccb5", codeHeader: "#e9decb", codeLineBorder: "#ded2bd", quote: "#f7f0e1", table: "#f7f0e1" }
        }
        return { window: "#ffffff", chrome: "#f6f8fa", panel: "#f6f8fa", reader: "#ffffff",
            border: "#d0d7de", text: "#1f2328", muted: "#656d76", accent: "#0969da",
            hover: "#eaeef2", pressed: "#d0d7de", drop: "#f6f8fa", scroll: "#d0d7de",
            code: "#f8fafc", codeBorder: "#e2e8f0", codeHeader: "#f1f5f9", codeLineBorder: "#e2e8f0", quote: "#f8fafc", table: "#f8fafc" }
    }

    property var theme: themeFor(workspaceController.themeId)
    property bool hasDocument: documentController.filePath.length > 0 || documentController.content.length > 0
    property bool outlineManuallyClosed: false
    property bool hasOutline: documentController.outline.length > 0
    property bool outlineVisible: hasOutline && !outlineManuallyClosed
    property real outlineWidth: 224

    // Interaction Modes: "read" | "split" | "edit"
    property string viewMode: "read"
    // Sidebar Tabs: "workspace" | "recent"
    property string sidebarTab: "workspace"

    // Split View Editor Width
    property real editorWidth: Math.max(300, Math.min(contentArea.width - 240, 520))

    // State for creating and renaming
    property string creatingType: "" // "" | "file" | "folder"
    property string creatingTargetFolder: ""
    property string renamingPath: ""

    function startCreate(type, folderPath) {
        creatingTargetFolder = (folderPath && folderPath.length > 0) ? folderPath : workspaceController.currentFolderPath
        creatingType = type
        sidebarTab = "workspace"
    }

    function cancelCreate() {
        creatingType = ""
        creatingTargetFolder = ""
    }

    function startRename(path, title) {
        renamingPath = path
    }

    function cancelRename() {
        renamingPath = ""
    }

    function triggerNewFile() {
        if (workspaceController.hasActiveFolder) {
            startCreate("file", workspaceController.currentFolderPath)
        } else {
            workspaceController.newUntitledDocument()
            if (viewMode === "read") viewMode = "split"
        }
    }

    function triggerNewFolder() {
        if (workspaceController.hasActiveFolder) {
            startCreate("folder", workspaceController.currentFolderPath)
        } else {
            folderDialog.open()
        }
    }

    function confirmDelete(path, title) {
        deleteConfirmDialog.targetPath = path
        deleteConfirmDialog.targetTitle = title
        deleteConfirmDialog.open()
    }

    function saveDocument() {
        if (viewMode !== "read" && editorTextArea) {
            documentController.updateContent(editorTextArea.text)
        }
        if (documentController.filePath.length === 0) {
            saveAsDialog.open()
        } else {
            documentController.saveContent(editorTextArea.text)
        }
    }

    function formatText(prefix, suffix, defaultPlaceholder) {
        if (viewMode === "read") viewMode = "split"
        let start = editorTextArea.selectionStart
        let end = editorTextArea.selectionEnd
        let sel = editorTextArea.selectedText
        if (sel.length === 0) sel = defaultPlaceholder
        let replacement = prefix + sel + suffix
        editorTextArea.remove(start, end)
        editorTextArea.insert(start, replacement)
        editorTextArea.select(start + prefix.length, start + prefix.length + sel.length)
        editorTextArea.forceActiveFocus()
    }

    function activateHistoryItem(item) {
        if (!item.available) {
            missingItemDialog.targetPath = item.path
            missingItemDialog.targetTitle = item.title
            missingItemDialog.open()
            return
        }
        if (item.type === "folder") {
            workspaceController.openFolder(item.path)
            sidebarTab = "workspace"
        } else {
            workspaceController.openPath(item.path)
        }
    }

    Connections {
        target: documentController
        function onDocumentChanged() {
            outlineManuallyClosed = false
        }
        function onDocumentLoaded() {
            if (editorTextArea) {
                editorTextArea.text = documentController.content
                if (viewMode !== "read") {
                    editorTextArea.forceActiveFocus()
                }
            }
        }
    }

    Component.onCompleted: {
        if (documentController.content.length > 0 && editorTextArea) {
            editorTextArea.text = documentController.content
        }
    }

    onViewModeChanged: {
        if (viewMode !== "read" && editorTextArea) {
            editorTextArea.forceActiveFocus()
        }
    }

    palette {
        window: theme.window
        windowText: theme.text
        base: theme.reader
        text: theme.text
        button: theme.chrome
        buttonText: theme.text
        highlight: theme.accent
        highlightedText: "white"
        mid: theme.border
    }

    // Modern Custom Vector Icon Component
    component AppIcon: Canvas {
        id: canvasIcon
        property string name: "file"
        property color color: theme.text
        property real size: 16
        implicitWidth: size
        implicitHeight: size
        width: size
        height: size
        renderTarget: Canvas.Image
        renderStrategy: Canvas.Immediate

        onColorChanged: requestPaint()
        onNameChanged: requestPaint()
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.clearRect(0, 0, width, height)
            var s = width / 16.0
            ctx.scale(s, s)
            ctx.lineWidth = 1.4
            ctx.lineCap = "round"
            ctx.lineJoin = "round"
            ctx.strokeStyle = color
            ctx.fillStyle = color

            if (name === "file") {
                ctx.beginPath();
                ctx.moveTo(3.5, 2); ctx.lineTo(10, 2); ctx.lineTo(13, 5); ctx.lineTo(13, 14); ctx.lineTo(3.5, 14); ctx.closePath();
                ctx.stroke();
                ctx.beginPath();
                ctx.moveTo(10, 2); ctx.lineTo(10, 5); ctx.lineTo(13, 5);
                ctx.stroke();
            } else if (name === "file-plus") {
                ctx.beginPath();
                ctx.moveTo(3.5, 2); ctx.lineTo(10, 2); ctx.lineTo(13, 5); ctx.lineTo(13, 14); ctx.lineTo(3.5, 14); ctx.closePath();
                ctx.stroke();
                ctx.beginPath();
                ctx.moveTo(10, 2); ctx.lineTo(10, 5); ctx.lineTo(13, 5);
                ctx.moveTo(8.25, 7.5); ctx.lineTo(8.25, 11.5);
                ctx.moveTo(6.25, 9.5); ctx.lineTo(10.25, 9.5);
                ctx.stroke();
            } else if (name === "folder") {
                ctx.beginPath();
                ctx.moveTo(2, 4); ctx.lineTo(6, 4); ctx.lineTo(7.5, 5.5); ctx.lineTo(14, 5.5); ctx.lineTo(14, 13); ctx.lineTo(2, 13); ctx.closePath();
                ctx.stroke();
            } else if (name === "folder-plus") {
                ctx.beginPath();
                ctx.moveTo(2, 4); ctx.lineTo(6, 4); ctx.lineTo(7.5, 5.5); ctx.lineTo(14, 5.5); ctx.lineTo(14, 13); ctx.lineTo(2, 13); ctx.closePath();
                ctx.moveTo(8, 7.5); ctx.lineTo(8, 11);
                ctx.moveTo(6.25, 9.25); ctx.lineTo(9.75, 9.25);
                ctx.stroke();
            } else if (name === "folder-open") {
                ctx.beginPath();
                ctx.moveTo(2, 4); ctx.lineTo(6, 4); ctx.lineTo(7.5, 5.5); ctx.lineTo(13, 5.5);
                ctx.moveTo(2, 7); ctx.lineTo(4, 13); ctx.lineTo(14, 13); ctx.lineTo(15, 7); ctx.closePath();
                ctx.stroke();
            } else if (name === "refresh") {
                ctx.beginPath();
                ctx.arc(8, 8, 4.8, -0.4, Math.PI * 1.55);
                ctx.stroke();
                ctx.beginPath();
                ctx.moveTo(6, 1.5); ctx.lineTo(9.2, 2.8); ctx.lineTo(8, 6);
                ctx.stroke();
            } else if (name === "sidebar") {
                ctx.strokeRect(2.5, 2.5, 11, 11);
                ctx.beginPath();
                ctx.moveTo(6.5, 2.5); ctx.lineTo(6.5, 13.5);
                ctx.stroke();
            } else if (name === "save") {
                ctx.beginPath();
                ctx.moveTo(3, 2.5); ctx.lineTo(11, 2.5); ctx.lineTo(13, 4.5); ctx.lineTo(13, 13.5); ctx.lineTo(3, 13.5); ctx.closePath();
                ctx.stroke();
                ctx.strokeRect(5.5, 8.5, 5, 5);
                ctx.beginPath();
                ctx.moveTo(5.5, 2.5); ctx.lineTo(5.5, 5.5); ctx.lineTo(10.5, 5.5); ctx.lineTo(10.5, 2.5);
                ctx.stroke();
            } else if (name === "search") {
                ctx.beginPath();
                ctx.arc(6.8, 6.8, 4, 0, Math.PI * 2);
                ctx.stroke();
                ctx.beginPath();
                ctx.moveTo(9.8, 9.8); ctx.lineTo(13.5, 13.5);
                ctx.stroke();
            } else if (name === "mode-read") {
                ctx.beginPath();
                ctx.moveTo(2, 8);
                ctx.quadraticCurveTo(8, 3, 14, 8);
                ctx.quadraticCurveTo(8, 13, 2, 8);
                ctx.stroke();
                ctx.beginPath();
                ctx.arc(8, 8, 1.8, 0, Math.PI * 2);
                ctx.stroke();
            } else if (name === "mode-split") {
                ctx.strokeRect(2.5, 2.5, 11, 11);
                ctx.beginPath();
                ctx.moveTo(8, 2.5); ctx.lineTo(8, 13.5);
                ctx.stroke();
            } else if (name === "mode-edit") {
                ctx.beginPath();
                ctx.moveTo(10.5, 3.5); ctx.lineTo(12.5, 5.5);
                ctx.moveTo(3, 13); ctx.lineTo(3.5, 10.5); ctx.lineTo(10.5, 3.5); ctx.lineTo(12.5, 5.5); ctx.lineTo(5.5, 12.5); ctx.closePath();
                ctx.stroke();
            } else if (name === "palette") {
                ctx.beginPath();
                ctx.arc(8, 8, 5.5, 0, Math.PI * 2);
                ctx.stroke();
                ctx.beginPath();
                ctx.arc(5.5, 6, 0.8, 0, Math.PI * 2);
                ctx.arc(8, 5, 0.8, 0, Math.PI * 2);
                ctx.arc(10.5, 6.5, 0.8, 0, Math.PI * 2);
                ctx.fill();
            } else if (name === "toc") {
                ctx.beginPath();
                ctx.moveTo(6, 4); ctx.lineTo(13.5, 4);
                ctx.moveTo(6, 8); ctx.lineTo(11.5, 8);
                ctx.moveTo(6, 12); ctx.lineTo(13, 12);
                ctx.moveTo(3, 4); ctx.lineTo(3.5, 4);
                ctx.moveTo(3, 8); ctx.lineTo(3.5, 8);
                ctx.moveTo(3, 12); ctx.lineTo(3.5, 12);
                ctx.stroke();
            } else if (name === "history") {
                ctx.beginPath();
                ctx.arc(8, 8, 5.5, 0, Math.PI * 2);
                ctx.stroke();
                ctx.beginPath();
                ctx.moveTo(8, 5); ctx.lineTo(8, 8); ctx.lineTo(10.5, 9.5);
                ctx.stroke();
            } else if (name === "trash") {
                ctx.beginPath();
                ctx.moveTo(3.5, 5); ctx.lineTo(12.5, 5);
                ctx.moveTo(4.8, 5); ctx.lineTo(5.2, 13); ctx.lineTo(10.8, 13); ctx.lineTo(11.2, 5);
                ctx.moveTo(6.5, 3); ctx.lineTo(9.5, 3);
                ctx.stroke();
            } else if (name === "close") {
                ctx.beginPath();
                ctx.moveTo(4, 4); ctx.lineTo(12, 12);
                ctx.moveTo(12, 4); ctx.lineTo(4, 12);
                ctx.stroke();
            } else if (name === "check") {
                ctx.beginPath();
                ctx.moveTo(3.5, 8.5); ctx.lineTo(6.5, 11.5); ctx.lineTo(12.5, 4.5);
                ctx.stroke();
            } else if (name === "more") {
                ctx.beginPath();
                ctx.arc(4, 8, 1.1, 0, Math.PI * 2);
                ctx.arc(8, 8, 1.1, 0, Math.PI * 2);
                ctx.arc(12, 8, 1.1, 0, Math.PI * 2);
                ctx.fill();
            } else if (name === "chevron-down") {
                ctx.beginPath();
                ctx.moveTo(4.5, 6.5); ctx.lineTo(8, 10); ctx.lineTo(11.5, 6.5);
                ctx.stroke();
            } else if (name === "chevron-right") {
                ctx.beginPath();
                ctx.moveTo(6.5, 4.5); ctx.lineTo(10, 8); ctx.lineTo(6.5, 11.5);
                ctx.stroke();
            } else if (name === "bold") {
                ctx.beginPath();
                ctx.moveTo(5, 3); ctx.lineTo(9, 3); ctx.arc(9, 5.5, 2.5, -Math.PI/2, Math.PI/2);
                ctx.lineTo(9.5, 8); ctx.arc(9.5, 10.5, 2.5, -Math.PI/2, Math.PI/2); ctx.lineTo(5, 13); ctx.closePath();
                ctx.stroke();
            } else if (name === "italic") {
                ctx.beginPath();
                ctx.moveTo(9, 3); ctx.lineTo(6, 13);
                ctx.moveTo(7, 3); ctx.lineTo(11, 3);
                ctx.moveTo(4, 13); ctx.lineTo(8, 13);
                ctx.stroke();
            } else if (name === "code") {
                ctx.beginPath();
                ctx.moveTo(5.5, 5); ctx.lineTo(2.5, 8); ctx.lineTo(5.5, 11);
                ctx.moveTo(10.5, 5); ctx.lineTo(13.5, 8); ctx.lineTo(10.5, 11);
                ctx.stroke();
            } else if (name === "quote") {
                ctx.beginPath();
                ctx.moveTo(3.5, 3); ctx.lineTo(3.5, 13);
                ctx.moveTo(6.5, 6); ctx.lineTo(12.5, 6);
                ctx.moveTo(6.5, 10); ctx.lineTo(10.5, 10);
                ctx.stroke();
            } else if (name === "list") {
                ctx.beginPath();
                ctx.moveTo(6, 4.5); ctx.lineTo(13.5, 4.5);
                ctx.moveTo(6, 8.5); ctx.lineTo(13.5, 8.5);
                ctx.moveTo(6, 12.5); ctx.lineTo(13.5, 12.5);
                ctx.arc(3.5, 4.5, 0.9, 0, Math.PI * 2);
                ctx.arc(3.5, 8.5, 0.9, 0, Math.PI * 2);
                ctx.arc(3.5, 12.5, 0.9, 0, Math.PI * 2);
                ctx.stroke();
            }
        }
    }

    // Reusable Icon Button
    component IconButton: Button {
        id: iconBtn
        property string iconName: "file"
        property real iconSize: 14
        property string tooltip: ""
        property color iconColor: hovered ? theme.text : theme.muted
        implicitWidth: 28
        implicitHeight: 28
        ToolTip.visible: hovered && tooltip.length > 0
        ToolTip.text: tooltip
        contentItem: AppIcon {
            anchors.centerIn: parent
            name: iconBtn.iconName
            size: iconBtn.iconSize
            color: iconBtn.iconColor
        }
        background: Rectangle {
            radius: 4
            color: iconBtn.down ? theme.pressed : (iconBtn.hovered ? theme.hover : "transparent")
        }
    }

    component ActionButton: Button {
        id: control
        implicitHeight: 30
        implicitWidth: 88
        font.pixelSize: 13
        contentItem: Label {
            text: control.text
            color: theme.text
            font: control.font
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 5
            color: control.down ? theme.pressed : control.hovered ? theme.hover : "transparent"
            border.width: 1
            border.color: theme.border
        }
    }

    component ContextMenu: Menu {
        popupType: Popup.Window
        implicitWidth: 190
        padding: 6
        background: Rectangle {
            radius: 8
            color: theme.chrome
            border.width: 1
            border.color: theme.border
        }
    }

    component ContextMenuItem: MenuItem {
        id: control
        implicitHeight: 34
        leftPadding: 10
        rightPadding: 10
        contentItem: Label {
            text: control.text
            color: control.enabled ? theme.text : theme.muted
            font.pixelSize: 12
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 5
            color: control.highlighted ? theme.hover : "transparent"
        }
    }

    component ContextMenuSeparator: MenuSeparator {
        topPadding: 5
        bottomPadding: 5
        contentItem: Rectangle {
            implicitHeight: 1
            color: theme.border
        }
    }

    component AppScrollBar: ScrollBar {
        id: control
        policy: ScrollBar.AsNeeded
        width: 6
        hoverEnabled: false
        padding: 1
        minimumSize: 0.025
        background: Rectangle {
            implicitWidth: control.width
            implicitHeight: control.height
            color: "transparent"
        }
        contentItem: Rectangle {
            implicitWidth: 4
            height: Math.max(20, control.visualSize * control.availableHeight * 0.72)
            radius: 2
            color: theme.scroll
            opacity: control.size < 1.0 ? 0.45 : 0
            border.width: 0
        }
    }

    FileDialog {
        id: fileDialog
        title: "打开 Markdown 或文本"
        nameFilters: ["Markdown / 文本 (*.md *.markdown *.mdown *.txt)", "所有文件 (*)"]
        onAccepted: workspaceController.handleUrl(selectedFile)
    }

    FolderDialog {
        id: folderDialog
        title: "选择工作区文件夹"
        onAccepted: workspaceController.handleUrl(selectedFolder)
    }

    FileDialog {
        id: saveAsDialog
        title: "保存 Markdown 文档"
        fileMode: FileDialog.SaveFile
        nameFilters: ["Markdown 文档 (*.md)", "所有文件 (*)"]
        defaultSuffix: "md"
        onAccepted: {
            let path = selectedFile.toString()
            if (path.startsWith("file:///")) {
                path = decodeURIComponent(path.substring(8))
            }
            documentController.saveContentAs(path, editorTextArea.text)
        }
    }

    ContextMenu {
        id: workspaceFileMenu
        ContextMenuItem { text: "打开文件…"; onTriggered: fileDialog.open() }
        ContextMenuItem { text: "打开文件夹作为工作区…"; onTriggered: folderDialog.open() }
        ContextMenuSeparator {}
        ContextMenuItem { text: "新建 Markdown 文档 (Ctrl+N)"; onTriggered: triggerNewFile() }
    }

    ContextMenu {
        id: recentMenu
        property string targetPath: ""
        property string targetType: ""
        ContextMenuItem {
            text: "打开"
            onTriggered: {
                if (recentMenu.targetType === "folder") {
                    workspaceController.openFolder(recentMenu.targetPath)
                    sidebarTab = "workspace"
                } else {
                    workspaceController.openPath(recentMenu.targetPath)
                }
            }
        }
        ContextMenuItem {
            text: "进入编辑模式"
            visible: recentMenu.targetType === "file"
            onTriggered: {
                workspaceController.openPath(recentMenu.targetPath)
                viewMode = "split"
                if (editorTextArea) {
                    editorTextArea.forceActiveFocus()
                }
            }
        }
        ContextMenuItem { text: "复制文件路径"; onTriggered: workspaceController.copyPath(recentMenu.targetPath) }
        ContextMenuItem { text: "在文件管理器显示"; onTriggered: workspaceController.revealPath(recentMenu.targetPath) }
        ContextMenuSeparator {}
        ContextMenuItem { text: "从最近列表移除"; onTriggered: workspaceController.removeRecentFile(recentMenu.targetPath) }
    }

    ContextMenu {
        id: folderFileMenu
        property string targetPath: ""
        property string targetTitle: ""
        ContextMenuItem { text: "打开阅读"; onTriggered: workspaceController.openFolderDocument(folderFileMenu.targetPath) }
        ContextMenuItem { text: "进入编辑模式"; onTriggered: { workspaceController.openFolderDocument(folderFileMenu.targetPath); viewMode = "split"; } }
        ContextMenuItem { text: "重命名…"; onTriggered: startRename(folderFileMenu.targetPath, folderFileMenu.targetTitle) }
        ContextMenuItem { text: "复制文件路径"; onTriggered: workspaceController.copyPath(folderFileMenu.targetPath) }
        ContextMenuItem { text: "在文件管理器显示"; onTriggered: workspaceController.revealPath(folderFileMenu.targetPath) }
        ContextMenuSeparator {}
        ContextMenuItem { text: "删除文件"; onTriggered: confirmDelete(folderFileMenu.targetPath, folderFileMenu.targetTitle) }
    }

    ContextMenu {
        id: folderMenu
        property string targetPath: ""
        property string targetTitle: ""
        ContextMenuItem { text: "在此文件夹新建文档 (Ctrl+N)"; onTriggered: startCreate("file", folderMenu.targetPath) }
        ContextMenuItem { text: "新建子文件夹"; onTriggered: startCreate("folder", folderMenu.targetPath) }
        ContextMenuSeparator {}
        ContextMenuItem { text: "重命名…"; onTriggered: startRename(folderMenu.targetPath, folderMenu.targetTitle) }
        ContextMenuItem { text: "复制文件夹路径"; onTriggered: workspaceController.copyPath(folderMenu.targetPath) }
        ContextMenuItem { text: "在文件管理器显示"; onTriggered: workspaceController.revealPath(folderMenu.targetPath) }
        ContextMenuSeparator {}
        ContextMenuItem { text: "关闭工作区文件夹"; onTriggered: workspaceController.closeWorkspaceFolder() }
    }

    ContextMenu {
        id: settingsMenu
        ContextMenuItem {
            text: "外观主题 ›"
            onTriggered: {
                settingsMenu.close()
                appearanceMenu.popup(settingsButton, settingsButton.width + 4, -appearanceMenu.implicitHeight)
            }
        }
    }

    ContextMenu {
        id: appearanceMenu
        ContextMenuItem { text: "GitHub Light (经典浅色)"; onTriggered: workspaceController.setThemeId("github-light") }
        ContextMenuItem { text: "GitHub Dark (现代深色)"; onTriggered: workspaceController.setThemeId("github-dark") }
        ContextMenuItem { text: "VitePress (文档绿白)"; onTriggered: workspaceController.setThemeId("vitepress") }
        ContextMenuItem { text: "One Dark Pro (经典暗黑)"; onTriggered: workspaceController.setThemeId("one-dark") }
        ContextMenuItem { text: "Dracula (吸血鬼暗色)"; onTriggered: workspaceController.setThemeId("dracula") }
        ContextMenuItem { text: "Nord (北欧极光冷杉)"; onTriggered: workspaceController.setThemeId("nord") }
        ContextMenuSeparator {}
        ContextMenuItem { text: "VS Code Dark+"; onTriggered: workspaceController.setThemeId("vscode-dark") }
        ContextMenuItem { text: "Paper (暖色纸张)"; onTriggered: workspaceController.setThemeId("paper") }
    }

    Popup {
        id: missingItemDialog
        popupType: Popup.Window
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: 360
        modal: true
        focus: true
        padding: 18
        property string targetPath: ""
        property string targetTitle: ""
        background: Rectangle {
            radius: 10
            color: theme.chrome
            border.width: 1
            border.color: theme.border
        }
        contentItem: ColumnLayout {
            spacing: 12
            Label { text: "这个项目已无法打开"; color: theme.text; font.pixelSize: 16; font.weight: Font.DemiBold }
            Label {
                Layout.fillWidth: true
                text: missingItemDialog.targetTitle + " 的路径已不存在。要从最近访问中删除吗？"
                color: theme.muted
                wrapMode: Text.Wrap
                font.pixelSize: 13
            }
            RowLayout {
                Layout.fillWidth: true
                Item { Layout.fillWidth: true }
                ActionButton { text: "保留"; implicitWidth: 64; onClicked: missingItemDialog.close() }
                ActionButton {
                    text: "删除"
                    implicitWidth: 64
                    onClicked: {
                        workspaceController.removeHistoryItem(missingItemDialog.targetPath)
                        missingItemDialog.close()
                    }
                }
            }
        }
    }

    Popup {
        id: deleteConfirmDialog
        popupType: Popup.Window
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: 360
        modal: true
        focus: true
        padding: 18
        property string targetPath: ""
        property string targetTitle: ""
        background: Rectangle {
            radius: 10
            color: theme.chrome
            border.width: 1
            border.color: theme.border
        }
        contentItem: ColumnLayout {
            spacing: 12
            Label { text: "确认删除文件"; color: theme.text; font.pixelSize: 16; font.weight: Font.DemiBold }
            Label {
                Layout.fillWidth: true
                text: "确定要永久删除 \"" + deleteConfirmDialog.targetTitle + "\" 吗？此操作无法撤销。"
                color: theme.muted
                wrapMode: Text.Wrap
                font.pixelSize: 13
            }
            RowLayout {
                Layout.fillWidth: true
                Item { Layout.fillWidth: true }
                ActionButton { text: "取消"; implicitWidth: 64; onClicked: deleteConfirmDialog.close() }
                Button {
                    implicitHeight: 30
                    implicitWidth: 64
                    text: "删除"
                    font.pixelSize: 13
                    contentItem: Label {
                        text: parent.text
                        color: "#ff7b72"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        radius: 4
                        color: parent.down ? theme.pressed : parent.hovered ? theme.hover : "transparent"
                        border.width: 1
                        border.color: "#ff7b72"
                    }
                    onClicked: {
                        workspaceController.deletePath(deleteConfirmDialog.targetPath)
                        deleteConfirmDialog.close()
                    }
                }
            }
        }
    }

    Popup {
        id: quickOpen
        popupType: Popup.Window
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(560, window.width - 48)
        height: 390
        modal: true
        focus: true
        padding: 0
        property var results: workspaceController.searchRecent(quickOpenInput.text)
        property int selectedIndex: 0
        onOpened: {
            quickOpenInput.text = ""
            selectedIndex = 0
            quickOpenInput.forceActiveFocus()
        }
        background: Rectangle {
            radius: 12
            color: theme.chrome
            border.width: 1
            border.color: theme.border
        }
        contentItem: ColumnLayout {
            spacing: 0
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 66
                color: "transparent"
                TextField {
                    id: quickOpenInput
                    anchors.fill: parent
                    anchors.margins: 14
                    placeholderText: "快速跳转：输入文件名或路径..."
                    font.pixelSize: 15
                    background: Rectangle { radius: 7; color: theme.reader; border.width: 1; border.color: theme.border }
                    onTextChanged: quickOpen.selectedIndex = 0
                    onAccepted: {
                        if (quickOpen.results.length > 0) {
                            activateHistoryItem(quickOpen.results[quickOpen.selectedIndex])
                            quickOpen.close()
                        }
                    }
                    Keys.onDownPressed: function(event) {
                        if (quickOpen.results.length > 0)
                            quickOpen.selectedIndex = Math.min(quickOpen.selectedIndex + 1, quickOpen.results.length - 1)
                        event.accepted = true
                    }
                    Keys.onUpPressed: function(event) {
                        if (quickOpen.results.length > 0)
                            quickOpen.selectedIndex = Math.max(quickOpen.selectedIndex - 1, 0)
                        event.accepted = true
                    }
                    Keys.onEscapePressed: function(event) {
                        quickOpen.close()
                        event.accepted = true
                    }
                }
            }
            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: theme.border }
            Label {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.topMargin: 10
                text: quickOpenInput.text.length === 0 ? "最近访问" : "搜索结果"
                color: theme.muted
                font.pixelSize: 11
            }
            ListView {
                id: quickOpenList
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 8
                clip: true
                model: quickOpen.results
                currentIndex: quickOpen.selectedIndex
                delegate: Rectangle {
                    required property var modelData
                    width: quickOpenList.width
                    height: 52
                    radius: 6
                    color: quickMouse.containsMouse || ListView.isCurrentItem ? theme.hover : "transparent"
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        spacing: 10

                        AppIcon {
                            name: modelData.type === "folder" ? "folder" : "file"
                            size: 16
                            color: theme.accent
                        }

                        Column {
                            Layout.fillWidth: true
                            spacing: 2
                            Label { width: parent.width; text: modelData.title; elide: Text.ElideRight; color: modelData.available ? theme.text : theme.muted; font.pixelSize: 13 }
                            Label { width: parent.width; text: modelData.path; elide: Text.ElideMiddle; color: theme.muted; font.pixelSize: 11 }
                        }
                    }
                    MouseArea {
                        id: quickMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            activateHistoryItem(modelData)
                            quickOpen.close()
                        }
                    }
                }
            }
        }
    }

    // Global Shortcuts
    Shortcut { sequence: "Ctrl+K"; onActivated: quickOpen.open() }
    Shortcut { sequence: "Meta+K"; onActivated: quickOpen.open() }
    Shortcut { sequence: "Ctrl+B"; onActivated: workspaceController.sidebarVisible = !workspaceController.sidebarVisible }
    Shortcut { sequence: "Meta+B"; onActivated: workspaceController.sidebarVisible = !workspaceController.sidebarVisible }
    Shortcut { sequence: "Ctrl+N"; onActivated: triggerNewFile() }
    Shortcut { sequence: "Meta+N"; onActivated: triggerNewFile() }
    Shortcut { sequence: "Ctrl+S"; onActivated: saveDocument() }
    Shortcut { sequence: "Meta+S"; onActivated: saveDocument() }
    Shortcut {
        sequence: "Ctrl+E"
        onActivated: {
            if (viewMode === "read") viewMode = "split"
            else if (viewMode === "split") viewMode = "edit"
            else viewMode = "read"
        }
    }
    Shortcut {
        sequence: "Meta+E"
        onActivated: {
            if (viewMode === "read") viewMode = "split"
            else if (viewMode === "split") viewMode = "edit"
            else viewMode = "read"
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ==========================================
        // LEFT SIDEBAR: Redesigned with Segmented Tab
        // ==========================================
        Rectangle {
            id: sidebar
            Layout.fillHeight: true
            Layout.preferredWidth: workspaceController.sidebarVisible ? workspaceController.sidebarWidth : 0
            visible: workspaceController.sidebarVisible
            clip: true
            color: theme.panel

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // 1. Sidebar Brand Header
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 46
                    color: "transparent"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 14
                        anchors.rightMargin: 10
                        spacing: 8

                        Rectangle {
                            width: 8
                            height: 8
                            radius: 4
                            color: theme.accent
                        }

                        Label {
                            text: "FlashRead"
                            color: theme.text
                            font.pixelSize: 14
                            font.weight: Font.DemiBold
                            Layout.fillWidth: true
                        }

                        IconButton {
                            iconName: "sidebar"
                            iconSize: 14
                            tooltip: "收起侧栏 (Ctrl+B)"
                            onClicked: workspaceController.sidebarVisible = false
                        }
                    }
                }

                // 2. Segmented Pill Tab Switcher: [ 工作区 | 最近访问 ]
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 34
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Layout.bottomMargin: 8
                    radius: 6
                    color: theme.chrome
                    border.width: 1
                    border.color: theme.border

                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 5
                            color: sidebarTab === "workspace" ? theme.hover : "transparent"
                            border.width: sidebarTab === "workspace" ? 1 : 0
                            border.color: theme.border

                            RowLayout {
                                anchors.centerIn: parent
                                spacing: 6
                                AppIcon {
                                    name: "folder"
                                    size: 13
                                    color: sidebarTab === "workspace" ? theme.accent : theme.muted
                                }
                                Label {
                                    text: "工作区"
                                    font.pixelSize: 12
                                    font.weight: sidebarTab === "workspace" ? Font.DemiBold : Font.Normal
                                    color: sidebarTab === "workspace" ? theme.text : theme.muted
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: sidebarTab = "workspace"
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 5
                            color: sidebarTab === "recent" ? theme.hover : "transparent"
                            border.width: sidebarTab === "recent" ? 1 : 0
                            border.color: theme.border

                            RowLayout {
                                anchors.centerIn: parent
                                spacing: 6
                                AppIcon {
                                    name: "history"
                                    size: 13
                                    color: sidebarTab === "recent" ? theme.accent : theme.muted
                                }
                                Label {
                                    text: "最近访问"
                                    font.pixelSize: 12
                                    font.weight: sidebarTab === "recent" ? Font.DemiBold : Font.Normal
                                    color: sidebarTab === "recent" ? theme.text : theme.muted
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: sidebarTab = "recent"
                            }
                        }
                    }
                }

                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: theme.border }

                // 3. Main Workspace / Recents Content
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // --- VIEW 1: WORKSPACE (Folder tree + Drag and Drop support) ---
                    DropArea {
                        id: workspaceDropArea
                        anchors.fill: parent
                        visible: sidebarTab === "workspace"
                        onDropped: function(drop) {
                            if (drop.urls.length > 0) {
                                workspaceController.handleUrl(drop.urls[0])
                            }
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: workspaceDropArea.containsDrag ? theme.drop : "transparent"
                            border.width: workspaceDropArea.containsDrag ? 2 : 0
                            border.color: theme.accent
                        }

                        // Has Active Folder
                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0
                            visible: workspaceController.hasActiveFolder

                            // Active Folder Header Bar
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 38
                                color: theme.chrome
                                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: theme.border }

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 10
                                    anchors.rightMargin: 8
                                    spacing: 4

                                    AppIcon {
                                        name: "folder-open"
                                        size: 14
                                        color: theme.accent
                                    }

                                    Label {
                                        Layout.fillWidth: true
                                        text: workspaceController.workspaceTitle
                                        color: theme.text
                                        font.pixelSize: 12
                                        font.weight: Font.DemiBold
                                        elide: Text.ElideRight
                                        ToolTip.visible: folderTitleMouse.containsMouse
                                        ToolTip.text: workspaceController.currentFolderPath
                                        MouseArea { id: folderTitleMouse; anchors.fill: parent; hoverEnabled: true }
                                    }

                                    IconButton {
                                        iconName: "file-plus"
                                        iconSize: 13
                                        tooltip: "新建文档 (Ctrl+N)"
                                        onClicked: startCreate("file", workspaceController.currentFolderPath)
                                    }

                                    IconButton {
                                        iconName: "folder-plus"
                                        iconSize: 13
                                        tooltip: "新建文件夹"
                                        onClicked: startCreate("folder", workspaceController.currentFolderPath)
                                    }

                                    IconButton {
                                        iconName: "refresh"
                                        iconSize: 12
                                        tooltip: "刷新工作区"
                                        onClicked: workspaceController.refreshWorkspace()
                                    }

                                    IconButton {
                                        iconName: "close"
                                        iconSize: 11
                                        tooltip: "关闭工作区文件夹"
                                        onClicked: workspaceController.closeWorkspaceFolder()
                                    }
                                }
                            }

                            // Inline Create Input Box
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 34
                                Layout.margins: 6
                                radius: 4
                                visible: creatingType.length > 0
                                color: theme.hover
                                border.width: 1
                                border.color: theme.accent

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 8
                                    anchors.rightMargin: 4
                                    spacing: 6

                                    AppIcon {
                                        name: creatingType === "folder" ? "folder" : "file"
                                        size: 13
                                        color: theme.accent
                                    }

                                    TextField {
                                        id: inlineCreateInput
                                        Layout.fillWidth: true
                                        placeholderText: creatingType === "folder" ? "新建文件夹名称 (Enter确认)" : "新建文档名称.md (Enter确认)"
                                        font.pixelSize: 12
                                        color: theme.text
                                        padding: 0
                                        verticalAlignment: Text.AlignVCenter
                                        background: Item {}

                                        onVisibleChanged: {
                                            if (visible) {
                                                text = ""
                                                forceActiveFocus()
                                            }
                                        }

                                        onAccepted: {
                                            let name = text.trim()
                                            if (name.length > 0) {
                                                if (creatingType === "folder") {
                                                    workspaceController.createFolder(creatingTargetFolder, name)
                                                } else {
                                                    workspaceController.createFile(creatingTargetFolder, name)
                                                    viewMode = "split"
                                                    if (editorTextArea) {
                                                        editorTextArea.forceActiveFocus()
                                                    }
                                                }
                                            }
                                            cancelCreate()
                                        }

                                        Keys.onEscapePressed: cancelCreate()
                                    }

                                    IconButton {
                                        iconName: "check"
                                        iconSize: 12
                                        tooltip: "确认创建"
                                        onClicked: inlineCreateInput.accepted()
                                    }

                                    IconButton {
                                        iconName: "close"
                                        iconSize: 11
                                        tooltip: "取消"
                                        onClicked: cancelCreate()
                                    }
                                }
                            }

                            // Folder Tree List
                            ListView {
                                id: folderTreeList
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                clip: true
                                model: workspaceController.workspaceItems
                                ScrollBar.vertical: AppScrollBar {}

                                delegate: Rectangle {
                                    required property var modelData
                                    width: folderTreeList.width
                                    height: 32
                                    property bool isRenaming: renamingPath === modelData.path
                                    color: itemMouse.containsMouse || (documentController.filePath === modelData.path) ? theme.hover : "transparent"

                                    Rectangle {
                                        anchors.left: parent.left
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom
                                        width: 2
                                        visible: documentController.filePath === modelData.path && !isRenaming
                                        color: theme.accent
                                    }

                                    // Item Display
                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.leftMargin: modelData.type === "folderFile" ? 26 : 8
                                        anchors.rightMargin: 8
                                        spacing: 6
                                        visible: !isRenaming

                                        AppIcon {
                                            name: modelData.type === "workspaceFolder" ? (workspaceController.folderExpanded ? "chevron-down" : "chevron-right") : "file"
                                            size: 13
                                            color: modelData.type === "workspaceFolder" ? theme.accent : theme.muted
                                        }

                                        Label {
                                            Layout.fillWidth: true
                                            text: modelData.title
                                            elide: Text.ElideRight
                                            color: modelData.available ? theme.text : theme.muted
                                            font.pixelSize: 13
                                            font.weight: modelData.type === "workspaceFolder" ? Font.DemiBold : Font.Normal
                                        }

                                        IconButton {
                                            iconName: "more"
                                            iconSize: 12
                                            tooltip: "操作"
                                            visible: itemMouse.containsMouse
                                            onClicked: {
                                                if (modelData.type === "workspaceFolder") {
                                                    folderMenu.targetPath = modelData.path
                                                    folderMenu.targetTitle = modelData.title
                                                    folderMenu.popup(this, 0, height)
                                                } else {
                                                    folderFileMenu.targetPath = modelData.path
                                                    folderFileMenu.targetTitle = modelData.title
                                                    folderFileMenu.popup(this, 0, height)
                                                }
                                            }
                                        }
                                    }

                                    // Inline Rename
                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.leftMargin: modelData.type === "folderFile" ? 24 : 8
                                        anchors.rightMargin: 6
                                        spacing: 4
                                        visible: isRenaming

                                        TextField {
                                            id: inlineRenameInput
                                            Layout.fillWidth: true
                                            text: modelData.title
                                            font.pixelSize: 12
                                            color: theme.text
                                            background: Rectangle { radius: 3; color: theme.reader; border.width: 1; border.color: theme.accent }

                                            Component.onCompleted: {
                                                forceActiveFocus()
                                                selectAll()
                                            }

                                            onAccepted: {
                                                let newName = text.trim()
                                                if (newName.length > 0 && newName !== modelData.title) {
                                                    workspaceController.renamePath(modelData.path, newName)
                                                }
                                                cancelRename()
                                            }

                                            Keys.onEscapePressed: cancelRename()
                                        }

                                        IconButton { iconName: "check"; iconSize: 11; onClicked: inlineRenameInput.accepted() }
                                        IconButton { iconName: "close"; iconSize: 10; onClicked: cancelRename() }
                                    }

                                    MouseArea {
                                        id: itemMouse
                                        anchors.fill: parent
                                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                                        hoverEnabled: true
                                        onClicked: function(mouse) {
                                            if (mouse.button === Qt.RightButton) {
                                                if (modelData.type === "workspaceFolder") {
                                                    folderMenu.targetPath = modelData.path
                                                    folderMenu.targetTitle = modelData.title
                                                    folderMenu.popup(itemMouse, mouse.x, mouse.y)
                                                } else {
                                                    folderFileMenu.targetPath = modelData.path
                                                    folderFileMenu.targetTitle = modelData.title
                                                    folderFileMenu.popup(itemMouse, mouse.x, mouse.y)
                                                }
                                            } else if (modelData.type === "workspaceFolder") {
                                                workspaceController.toggleFolderExpanded()
                                            } else {
                                                workspaceController.openFolderDocument(modelData.path)
                                                if (viewMode === "read") {
                                                    viewMode = "split"
                                                }
                                                if (editorTextArea) {
                                                    editorTextArea.forceActiveFocus()
                                                }
                                            }
                                        }
                                        onDoubleClicked: function(mouse) {
                                            if (mouse.button === Qt.LeftButton) {
                                                if (modelData.type === "workspaceFolder") {
                                                    workspaceController.toggleFolderExpanded()
                                                } else {
                                                    workspaceController.openFolderDocument(modelData.path)
                                                    viewMode = "split"
                                                    if (editorTextArea) {
                                                        editorTextArea.forceActiveFocus()
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // No Active Folder: Modern Drag & Drop Zone Card
                        ColumnLayout {
                            anchors.centerIn: parent
                            width: Math.min(parent.width - 24, 260)
                            spacing: 16
                            visible: !workspaceController.hasActiveFolder

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 180
                                radius: 8
                                color: workspaceDropArea.containsDrag ? theme.drop : theme.chrome
                                border.width: 1
                                border.color: workspaceDropArea.containsDrag ? theme.accent : theme.border

                                ColumnLayout {
                                    anchors.centerIn: parent
                                    spacing: 10

                                    AppIcon {
                                        Layout.alignment: Qt.AlignHCenter
                                        name: "folder-open"
                                        size: 38
                                        color: theme.accent
                                    }

                                    Label {
                                        Layout.alignment: Qt.AlignHCenter
                                        text: "拖入文件夹载入工作区"
                                        color: theme.text
                                        font.pixelSize: 13
                                        font.weight: Font.DemiBold
                                    }

                                    Label {
                                        Layout.alignment: Qt.AlignHCenter
                                        Layout.preferredWidth: 200
                                        text: "支持直接拖入任何文档目录，便于快速管理章节与新建笔记。"
                                        color: theme.muted
                                        font.pixelSize: 11
                                        wrapMode: Text.Wrap
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }
                            }

                            RowLayout {
                                Layout.alignment: Qt.AlignHCenter
                                spacing: 8

                                ActionButton {
                                    text: "打开文件夹"
                                    implicitWidth: 92
                                    implicitHeight: 30
                                    onClicked: folderDialog.open()
                                }

                                ActionButton {
                                    text: "新建草稿"
                                    implicitWidth: 84
                                    implicitHeight: 30
                                    onClicked: triggerNewFile()
                                }
                            }
                        }
                    }

                    // --- VIEW 2: RECENTS (Clean Chronological History List) ---
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0
                        visible: sidebarTab === "recent"

                        // Recents Header
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 36
                            color: theme.chrome
                            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: theme.border }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 8
                                spacing: 6

                                Label {
                                    Layout.fillWidth: true
                                    text: "历史打开记录 (" + workspaceController.recentItems.length + ")"
                                    color: theme.muted
                                    font.pixelSize: 11
                                    font.weight: Font.DemiBold
                                }

                                IconButton {
                                    iconName: "trash"
                                    iconSize: 12
                                    tooltip: "清空所有最近记录"
                                    visible: workspaceController.recentItems.length > 0
                                    onClicked: workspaceController.clearRecent()
                                }
                            }
                        }

                        // Recents List
                        ListView {
                            id: recentListView
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            model: workspaceController.recentItems
                            ScrollBar.vertical: AppScrollBar {}

                            delegate: Rectangle {
                                required property var modelData
                                width: recentListView.width
                                height: 38
                                color: recentMouse.containsMouse ? theme.hover : "transparent"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 12
                                    anchors.rightMargin: 8
                                    spacing: 8

                                    AppIcon {
                                        name: modelData.type === "folder" ? "folder" : "file"
                                        size: 13
                                        color: modelData.available ? theme.accent : theme.muted
                                    }

                                    Column {
                                        Layout.fillWidth: true
                                        spacing: 2
                                        Label {
                                            width: parent.width
                                            text: modelData.title
                                            elide: Text.ElideRight
                                            color: modelData.available ? theme.text : theme.muted
                                            font.pixelSize: 12
                                        }
                                        Label {
                                            width: parent.width
                                            text: modelData.path
                                            elide: Text.ElideMiddle
                                            color: theme.muted
                                            font.pixelSize: 10
                                        }
                                    }

                                    IconButton {
                                        iconName: "close"
                                        iconSize: 10
                                        tooltip: "从历史中移除"
                                        visible: recentMouse.containsMouse
                                        onClicked: workspaceController.removeHistoryItem(modelData.path)
                                    }
                                }

                                MouseArea {
                                    id: recentMouse
                                    anchors.fill: parent
                                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                                    hoverEnabled: true
                                    onClicked: function(mouse) {
                                        if (mouse.button === Qt.RightButton) {
                                            recentMenu.targetPath = modelData.path
                                            recentMenu.targetType = modelData.type
                                            recentMenu.popup(recentMouse, mouse.x, mouse.y)
                                        } else {
                                            activateHistoryItem(modelData)
                                        }
                                    }
                                    onDoubleClicked: function(mouse) {
                                        if (mouse.button === Qt.LeftButton) {
                                            if (modelData.type === "folder") {
                                                workspaceController.openFolder(modelData.path)
                                                sidebarTab = "workspace"
                                            } else {
                                                workspaceController.openPath(modelData.path)
                                                viewMode = "split"
                                                if (editorTextArea) {
                                                    editorTextArea.forceActiveFocus()
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // Empty Recents State
                        Label {
                            visible: workspaceController.recentItems.length === 0
                            Layout.alignment: Qt.AlignHCenter
                            Layout.topMargin: 40
                            text: "暂无最近访问记录"
                            color: theme.muted
                            font.pixelSize: 12
                        }
                    }
                }

                // 4. Sidebar Bottom Settings Bar
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    color: theme.chrome
                    Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: theme.border }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 10
                        spacing: 6

                        IconButton {
                            iconName: "search"
                            iconSize: 13
                            tooltip: "快速跳转 (Ctrl+K)"
                            onClicked: quickOpen.open()
                        }

                        Label {
                            text: "Ctrl+K 快速跳转"
                            color: theme.muted
                            font.pixelSize: 11
                            Layout.fillWidth: true
                        }

                        Button {
                            id: settingsButton
                            implicitHeight: 26
                            implicitWidth: 26
                            ToolTip.visible: hovered
                            ToolTip.text: "外观主题设置"
                            contentItem: AppIcon {
                                anchors.centerIn: parent
                                name: "palette"
                                size: 14
                                color: parent.hovered ? theme.text : theme.muted
                            }
                            background: Rectangle {
                                radius: 4
                                color: parent.hovered ? theme.hover : "transparent"
                            }
                            onClicked: settingsMenu.popup(settingsButton, 0, -settingsMenu.implicitHeight)
                        }
                    }
                }
            }
        }

        // Sidebar Draggable Splitter
        Item {
            id: sidebarSplitter
            Layout.fillHeight: true
            Layout.preferredWidth: 5
            visible: workspaceController.sidebarVisible
            z: 20

            Rectangle {
                anchors.centerIn: parent
                width: 1
                height: parent.height
                color: splitterMouse.pressed || splitterMouse.containsMouse ? theme.accent : theme.border
                Behavior on color { ColorAnimation { duration: 120 } }
            }

            MouseArea {
                id: splitterMouse
                anchors.fill: parent
                anchors.leftMargin: -2
                anchors.rightMargin: -2
                hoverEnabled: true
                cursorShape: Qt.SplitHCursor
                preventStealing: true

                property real startX: 0
                property real startWidth: 0

                onPressed: function(mouse) {
                    startX = mouse.x
                    startWidth = workspaceController.sidebarWidth
                }

                onPositionChanged: function(mouse) {
                    if (pressed) {
                        let delta = mouse.x - startX
                        let maxAllowed = Math.min(500, Math.floor(window.width * 0.45))
                        let newWidth = Math.max(180, Math.min(maxAllowed, startWidth + delta))
                        workspaceController.setSidebarWidth(newWidth)
                    }
                }
            }
        }

        // ==========================================
        // MAIN CONTENT AREA: With View Modes (Read, Split, Edit)
        // ==========================================
        DropArea {
            id: contentArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            onDropped: function(drop) {
                if (drop.urls.length > 0) workspaceController.handleUrl(drop.urls[0])
            }

            Rectangle {
                anchors.fill: parent
                color: contentArea.containsDrag ? theme.drop : theme.window
            }

            // Top Bar
            Rectangle {
                id: readerTopBar
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 40
                color: theme.chrome
                z: 15

                Rectangle {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 1
                    color: theme.border
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 12
                    spacing: 8

                    IconButton {
                        iconName: "sidebar"
                        iconSize: 14
                        tooltip: workspaceController.sidebarVisible ? "收起侧栏 (Ctrl+B)" : "展开侧栏 (Ctrl+B)"
                        onClicked: workspaceController.sidebarVisible = !workspaceController.sidebarVisible
                    }

                    // Document Title + Unsaved dot
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        Label {
                            text: documentController.title.length > 0 ? documentController.title : "FlashRead"
                            color: theme.text
                            font.pixelSize: 13
                            font.weight: Font.DemiBold
                            elide: Text.ElideRight
                        }

                        Rectangle {
                            width: 6
                            height: 6
                            radius: 3
                            color: theme.accent
                            visible: documentController.isModified
                        }

                        Label {
                            text: "(未保存)"
                            color: theme.accent
                            font.pixelSize: 11
                            visible: documentController.isModified
                        }

                        IconButton {
                            iconName: "mode-edit"
                            iconSize: 13
                            tooltip: "进入编辑模式 (Ctrl+E)"
                            visible: viewMode === "read" && hasDocument
                            onClicked: {
                                viewMode = "split"
                                if (editorTextArea) {
                                    editorTextArea.forceActiveFocus()
                                }
                            }
                        }
                    }

                    // --- Mode Switcher Pill: [ 阅读 | 分屏 | 编辑 ] ---
                    Rectangle {
                        height: 28
                        width: 204
                        radius: 6
                        color: theme.window
                        border.width: 1
                        border.color: theme.border

                        RowLayout {
                            anchors.fill: parent
                            spacing: 0

                            Repeater {
                                model: [
                                    { id: "read", text: "阅读", icon: "mode-read", tip: "纯阅读模式 (Ctrl+E 切换)" },
                                    { id: "split", text: "分屏", icon: "mode-split", tip: "分屏编辑与实时预览 (Ctrl+E 切换)" },
                                    { id: "edit", text: "编辑", icon: "mode-edit", tip: "全屏编辑模式 (Ctrl+E 切换)" }
                                ]

                                delegate: Rectangle {
                                    id: modeRect
                                    required property var modelData
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    radius: 5
                                    color: viewMode === modelData.id ? theme.hover : "transparent"
                                    border.width: viewMode === modelData.id ? 1 : 0
                                    border.color: theme.border
                                    ToolTip.visible: modeMouse.containsMouse
                                    ToolTip.text: modelData.tip

                                    RowLayout {
                                        anchors.centerIn: parent
                                        spacing: 4

                                        AppIcon {
                                            name: modelData.icon
                                            size: 13
                                            color: viewMode === modelData.id ? theme.accent : theme.muted
                                        }

                                        Label {
                                            text: modelData.text
                                            font.pixelSize: 11
                                            font.weight: viewMode === modelData.id ? Font.DemiBold : Font.Normal
                                            color: viewMode === modelData.id ? theme.text : theme.muted
                                        }
                                    }

                                    MouseArea {
                                        id: modeMouse
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: viewMode = modelData.id
                                    }
                                }
                            }
                        }
                    }

                    // Save Button (Ctrl+S)
                    Button {
                        id: saveDocBtn
                        implicitHeight: 28
                        implicitWidth: 68
                        visible: hasDocument || viewMode !== "read"
                        ToolTip.visible: hovered
                        ToolTip.text: "保存修改 (Ctrl+S)"
                        contentItem: RowLayout {
                            anchors.centerIn: parent
                            spacing: 4
                            AppIcon {
                                name: "save"
                                size: 13
                                color: documentController.isModified ? theme.accent : theme.muted
                            }
                            Label {
                                text: "保存"
                                color: documentController.isModified ? theme.accent : theme.muted
                                font.pixelSize: 12
                                font.weight: documentController.isModified ? Font.DemiBold : Font.Normal
                            }
                        }
                        background: Rectangle {
                            radius: 4
                            color: documentController.isModified ? theme.hover : "transparent"
                            border.width: documentController.isModified ? 1 : 0
                            border.color: theme.accent
                        }
                        onClicked: saveDocument()
                    }

                    // Theme Picker Button
                    IconButton {
                        id: themeButton
                        iconName: "palette"
                        iconSize: 14
                        tooltip: "切换外观主题"
                        onClicked: appearanceMenu.popup(themeButton, -appearanceMenu.implicitWidth + themeButton.width, themeButton.height + 4)
                    }

                    // Outline TOC Button
                    Button {
                        id: outlineBtn
                        visible: hasOutline
                        implicitHeight: 28
                        implicitWidth: 68
                        ToolTip.visible: hovered
                        ToolTip.text: outlineVisible ? "收起目录" : "展开目录"
                        contentItem: RowLayout {
                            anchors.centerIn: parent
                            spacing: 4
                            AppIcon {
                                name: "toc"
                                size: 13
                                color: outlineBtn.hovered || outlineVisible ? theme.text : theme.muted
                            }
                            Label {
                                text: "目录"
                                color: outlineBtn.hovered || outlineVisible ? theme.text : theme.muted
                                font.pixelSize: 12
                            }
                        }
                        background: Rectangle {
                            radius: 4
                            color: outlineVisible ? theme.hover : (outlineBtn.down ? theme.pressed : (outlineBtn.hovered ? theme.hover : "transparent"))
                            border.width: outlineVisible ? 1 : 0
                            border.color: theme.border
                        }
                        onClicked: outlineManuallyClosed = !outlineManuallyClosed
                    }
                }
            }

            // Welcome Guide (when no document loaded and in read mode)
            ColumnLayout {
                anchors.centerIn: parent
                width: Math.min(parent.width - 64, 480)
                visible: !hasDocument && viewMode === "read"
                spacing: 12

                AppIcon {
                    Layout.alignment: Qt.AlignHCenter
                    name: "mode-edit"
                    size: 42
                    color: theme.accent
                }

                Label { text: "FlashRead"; color: theme.accent; font.pixelSize: 18; font.weight: Font.DemiBold; Layout.alignment: Qt.AlignHCenter }
                Label { text: "开始极速阅读与记录"; color: theme.text; font.pixelSize: 26; font.weight: Font.DemiBold; Layout.alignment: Qt.AlignHCenter }
                Label { text: "支持拖入 Markdown 文件或文件夹，随时快捷书写。"; color: theme.muted; font.pixelSize: 13; Layout.alignment: Qt.AlignHCenter }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 12
                    ActionButton { text: "打开文件"; implicitWidth: 92; onClicked: fileDialog.open() }
                    ActionButton { text: "新建文档"; implicitWidth: 92; onClicked: triggerNewFile() }
                }
            }

            // --- EDITOR PANE (Visible in Split or Edit mode) ---
            Rectangle {
                id: editorContainer
                anchors.top: readerTopBar.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                width: viewMode === "edit" ? parent.width : (viewMode === "split" ? editorWidth : 0)
                visible: viewMode !== "read"
                color: theme.window
                clip: true

                // Formatting Toolbar
                Rectangle {
                    id: formatBar
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 32
                    color: theme.chrome
                    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: theme.border }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        spacing: 2

                        IconButton {
                            iconName: "bold"
                            iconSize: 13
                            tooltip: "加粗 (**文本**)"
                            onClicked: formatText("**", "**", "加粗文本")
                        }

                        IconButton {
                            iconName: "italic"
                            iconSize: 13
                            tooltip: "斜体 (*文本*)"
                            onClicked: formatText("*", "*", "斜体文本")
                        }

                        IconButton {
                            iconName: "code"
                            iconSize: 13
                            tooltip: "代码块 (```)"
                            onClicked: formatText("\n```cpp\n", "\n```\n", "// 在此编写代码")
                        }

                        IconButton {
                            iconName: "quote"
                            iconSize: 13
                            tooltip: "引用区块 (> 文本)"
                            onClicked: formatText("\n> ", "\n", "引用内容")
                        }

                        IconButton {
                            iconName: "list"
                            iconSize: 13
                            tooltip: "无序列表 (- 条目)"
                            onClicked: formatText("\n- ", "\n", "列表项")
                        }

                        Item { Layout.fillWidth: true }

                        Label {
                            text: "Ctrl+S 保存 · Tab 缩进"
                            color: theme.muted
                            font.pixelSize: 10
                            rightPadding: 4
                        }
                    }
                }

                // Main Writing Area (between formatBar and editorStatusBar)
                Item {
                    id: editorMainArea
                    anchors.top: formatBar.bottom
                    anchors.bottom: editorStatusBar.top
                    anchors.left: parent.left
                    anchors.right: parent.right

                    // Line Numbers Gutter
                    Rectangle {
                        id: lineGutter
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        width: 42
                        color: theme.chrome
                        clip: true
                        z: 2

                        Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: theme.border }

                        Column {
                            y: -editorFlickable.contentY + editorTextArea.topPadding
                            width: parent.width - 8
                            spacing: 0

                            Repeater {
                                model: Math.max(1, Math.min(2500, editorTextArea.lineCount))
                                delegate: Label {
                                    width: parent.width
                                    height: 22
                                    text: (index + 1)
                                    color: theme.muted
                                    opacity: 0.45
                                    font.family: editorTextArea.font.family
                                    font.pixelSize: 11
                                    horizontalAlignment: Text.AlignRight
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: editorTextArea.forceActiveFocus()
                        }
                    }

                    // Editor Scrollable Canvas
                    Flickable {
                        id: editorFlickable
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.left: lineGutter.right
                        anchors.right: parent.right
                        clip: true
                        contentWidth: width
                        contentHeight: Math.max(height, editorTextArea.implicitHeight + 80)
                        boundsBehavior: Flickable.StopAtBounds

                        TextArea.flickable: TextArea {
                            id: editorTextArea
                            width: editorFlickable.width
                            height: Math.max(editorFlickable.height, implicitHeight)
                            leftPadding: 16
                            rightPadding: 16
                            topPadding: 12
                            bottomPadding: 64
                            color: theme.text
                            selectionColor: theme.accent
                            selectedTextColor: "white"
                            placeholderText: "在此开始书写 Markdown 内容..."
                            placeholderTextColor: theme.muted
                            font.family: "'JetBrains Mono', 'Cascadia Code', 'Consolas', 'Segoe UI', monospace"
                            font.pixelSize: 13
                            wrapMode: TextEdit.Wrap
                            textFormat: TextEdit.PlainText
                            selectByMouse: true
                            focus: true
                            activeFocusOnPress: true
                            background: Rectangle {
                                color: theme.window
                            }

                            Timer {
                                id: previewDebounceTimer
                                interval: 120
                                repeat: false
                                onTriggered: {
                                    documentController.updateContent(editorTextArea.text)
                                }
                            }

                            onTextChanged: {
                                if (activeFocus) {
                                    previewDebounceTimer.restart()
                                }
                            }

                            Keys.onTabPressed: function(event) {
                                insert(cursorPosition, "    ")
                                event.accepted = true
                            }
                        }

                        ScrollBar.vertical: AppScrollBar {}
                    }

                    // Click anywhere in empty area to focus editor
                    MouseArea {
                        anchors.fill: parent
                        z: -1
                        onClicked: {
                            editorTextArea.forceActiveFocus()
                        }
                    }
                }

                // Editor Bottom Mini Status Bar
                Rectangle {
                    id: editorStatusBar
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 24
                    color: theme.chrome
                    Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: theme.border }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        spacing: 12

                        Label {
                            text: "字数: " + documentController.wordCount
                            color: theme.muted
                            font.pixelSize: 11
                        }

                        Label {
                            text: "行数: " + documentController.lineCount
                            color: theme.muted
                            font.pixelSize: 11
                        }

                        Label {
                            text: documentController.isModified ? "• 未保存修改" : "✓ 已保存"
                            color: documentController.isModified ? theme.accent : theme.muted
                            font.pixelSize: 11
                            font.weight: documentController.isModified ? Font.DemiBold : Font.Normal
                        }

                        Item { Layout.fillWidth: true }

                        Label {
                            text: "Markdown (UTF-8)"
                            color: theme.muted
                            font.pixelSize: 11
                        }
                    }
                }
            }

            // Splitter between Editor and WebView (visible in Split mode)
            Item {
                id: editorSplitter
                width: 6
                anchors.top: readerTopBar.bottom
                anchors.bottom: parent.bottom
                x: editorWidth - 3
                visible: viewMode === "split"
                z: 20

                Rectangle {
                    anchors.centerIn: parent
                    width: 1
                    height: parent.height
                    color: editorSplitterMouse.pressed || editorSplitterMouse.containsMouse ? theme.accent : theme.border
                }

                MouseArea {
                    id: editorSplitterMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.SplitHCursor
                    preventStealing: true

                    property real startX: 0
                    property real startWidth: 0

                    onPressed: function(mouse) {
                        startX = mouse.x
                        startWidth = editorWidth
                    }

                    onPositionChanged: function(mouse) {
                        if (pressed) {
                            let delta = mouse.x - startX
                            let maxW = Math.min(contentArea.width - 240, Math.max(260, startWidth + delta))
                            editorWidth = maxW
                        }
                    }
                }
            }

            // --- WEBVIEW RENDER PANE (Visible in Read or Split mode) ---
            MarkdownWebView {
                id: markdownWebView
                anchors.top: readerTopBar.bottom
                anchors.left: viewMode === "split" ? editorSplitter.right : parent.left
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.rightMargin: hasDocument && !workspaceController.folderViewActive && outlineVisible ? outlineWidth : 0
                visible: hasDocument && viewMode !== "edit"
                htmlContent: documentController.renderedContent
                themeId: workspaceController.themeId

                onLinkClicked: function(url) {
                    Qt.openUrlExternally(url)
                }
            }

            // Outline Table of Contents Panel (Right side)
            Rectangle {
                id: outlinePanel
                anchors.top: readerTopBar.bottom
                anchors.bottom: parent.bottom
                width: outlineWidth
                x: hasDocument && !workspaceController.folderViewActive && outlineVisible ? parent.width - outlineWidth : parent.width
                visible: x < parent.width
                clip: true
                color: theme.panel
                z: 25
                opacity: hasDocument && !workspaceController.folderViewActive && outlineVisible ? 1.0 : 0.0
                Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutCubic } }
                Behavior on opacity { NumberAnimation { duration: 130 } }

                Item {
                    id: outlineSplitter
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 6
                    z: 10

                    Rectangle {
                        anchors.left: parent.left
                        width: 1
                        height: parent.height
                        color: outlineSplitterMouse.pressed || outlineSplitterMouse.containsMouse ? theme.accent : theme.border
                    }

                    MouseArea {
                        id: outlineSplitterMouse
                        anchors.fill: parent
                        anchors.leftMargin: -2
                        anchors.rightMargin: -2
                        hoverEnabled: true
                        cursorShape: Qt.SplitHCursor
                        preventStealing: true

                        property real startX: 0
                        property real startWidth: 0

                        onPressed: function(mouse) {
                            startX = mouse.x
                            startWidth = outlineWidth
                        }

                        onPositionChanged: function(mouse) {
                            if (pressed) {
                                let delta = startX - mouse.x
                                let maxAllowed = Math.min(420, Math.floor(window.width * 0.4))
                                outlineWidth = Math.max(180, Math.min(maxAllowed, startWidth + delta))
                            }
                        }

                        onDoubleClicked: {
                            outlineWidth = 224
                        }
                    }
                }

                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 6

                    RowLayout {
                        width: parent.width
                        Label { text: "文档大纲"; color: theme.text; font.pixelSize: 13; font.weight: Font.DemiBold }
                        Item { Layout.fillWidth: true }
                        Label { text: documentController.outline.length; color: theme.muted; font.pixelSize: 11; rightPadding: 4 }
                        IconButton {
                            iconName: "close"
                            iconSize: 10
                            tooltip: "关闭目录"
                            onClicked: outlineManuallyClosed = true
                        }
                    }

                    ListView {
                        id: outlineList
                        width: parent.width
                        height: parent.height - 40
                        clip: true
                        model: documentController.outline
                        spacing: 2
                        ScrollBar.vertical: AppScrollBar {}
                        delegate: Rectangle {
                            required property var modelData
                            width: outlineList.width - 6
                            height: 30
                            radius: 4
                            color: outlineMouse.containsMouse ? theme.hover : "transparent"

                            Label {
                                anchors.left: parent.left
                                anchors.leftMargin: 8 + Math.min((modelData.level - 1) * 10, 30)
                                anchors.right: parent.right
                                anchors.rightMargin: 8
                                anchors.verticalCenter: parent.verticalCenter
                                text: modelData.title
                                elide: Text.ElideRight
                                color: theme.text
                                font.pixelSize: 12
                                font.weight: modelData.level === 1 ? Font.DemiBold : Font.Normal
                            }

                            MouseArea {
                                id: outlineMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    if (modelData.title && modelData.title.length > 0) {
                                        markdownWebView.scrollToHeading(modelData.title)
                                    } else {
                                        markdownWebView.scrollToProgress(modelData.progress)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Loading Mask
            Rectangle {
                anchors.fill: parent
                color: theme.window
                visible: documentController.isLoading
                opacity: documentController.isLoading ? 0.7 : 0.0
                z: 30
                Behavior on opacity { NumberAnimation { duration: 100 } }

                Label {
                    anchors.centerIn: parent
                    text: "正在极速加载…"
                    color: theme.muted
                    font.pixelSize: 13
                }
            }
        }
    }
}
