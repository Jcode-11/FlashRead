import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import QtQuick.Layouts
import FlashRead.WebView 1.0

ApplicationWindow {
    id: window

    width: 1180
    height: 760
    minimumWidth: 820
    minimumHeight: 520
    visible: true
    title: documentController.title.length > 0
        ? documentController.title + " - FlashRead" : "FlashRead"

    function themeFor(id) {
        if (id === "github-dark") {
            return { window: "#0d1117", chrome: "#161b22", panel: "#161b22", reader: "#0d1117",
                border: "#30363d", text: "#e6edf3", muted: "#848d97", accent: "#2f81f7",
                hover: "#21262d", pressed: "#30363d", drop: "#161b22", scroll: "#30363d",
                code: "#161b22", codeBorder: "#30363d", codeHeader: "#1c2128", codeLineBorder: "#262c36", quote: "#161b22", table: "#161b22",
                hlK: "#ff7b72", hlT: "#ffa657", hlS: "#a5d6ff", hlC: "#8b949e", hlN: "#79c0ff",
                hlF: "#d2a8ff", hlP: "#ff7b72", hlKey: "#7ee787", hlConst: "#79c0ff",
                hlDiffAdd: "#7ee787", hlDiffDel: "#ffa198", hlDiffHdr: "#d2a8ff" }
        }
        if (id === "dracula") {
            return { window: "#282a36", chrome: "#21222c", panel: "#21222c", reader: "#282a36",
                border: "#44475a", text: "#f8f8f2", muted: "#6272a4", accent: "#bd93f9",
                hover: "#343746", pressed: "#44475a", drop: "#21222c", scroll: "#44475a",
                code: "#1e1f29", codeBorder: "#44475a", codeHeader: "#242633", codeLineBorder: "#343746", quote: "#21222c", table: "#21222c",
                hlK: "#ff79c6", hlT: "#8be9fd", hlS: "#f1fa8c", hlC: "#6272a4", hlN: "#bd93f9",
                hlF: "#50fa7b", hlP: "#ffb86c", hlKey: "#ff79c6", hlConst: "#bd93f9",
                hlDiffAdd: "#50fa7b", hlDiffDel: "#ff5555", hlDiffHdr: "#bd93f9" }
        }
        if (id === "nord") {
            return { window: "#2e3440", chrome: "#3b4252", panel: "#3b4252", reader: "#2e3440",
                border: "#434c5e", text: "#eceff4", muted: "#7b88a1", accent: "#88c0d0",
                hover: "#434c5e", pressed: "#4c566a", drop: "#3b4252", scroll: "#434c5e",
                code: "#242933", codeBorder: "#434c5e", codeHeader: "#2b303c", codeLineBorder: "#353c4a", quote: "#3b4252", table: "#3b4252",
                hlK: "#81a1c1", hlT: "#8fbcbb", hlS: "#a3be8c", hlC: "#616e88", hlN: "#b48ead",
                hlF: "#88c0d0", hlP: "#d8dee9", hlKey: "#81a1c1", hlConst: "#b48ead",
                hlDiffAdd: "#a3be8c", hlDiffDel: "#bf616a", hlDiffHdr: "#88c0d0" }
        }
        if (id === "one-dark") {
            return { window: "#282c34", chrome: "#21252b", panel: "#21252b", reader: "#282c34",
                border: "#3e4451", text: "#abb2bf", muted: "#5c6370", accent: "#61afef",
                hover: "#2c313a", pressed: "#3e4451", drop: "#21252b", scroll: "#3e4451",
                code: "#1e2227", codeBorder: "#3e4451", codeHeader: "#23272e", codeLineBorder: "#2c313a", quote: "#21252b", table: "#21252b",
                hlK: "#c678dd", hlT: "#e5c07b", hlS: "#98c379", hlC: "#5c6370", hlN: "#d19a66",
                hlF: "#61afef", hlP: "#e06c75", hlKey: "#e06c75", hlConst: "#d19a66",
                hlDiffAdd: "#98c379", hlDiffDel: "#e06c75", hlDiffHdr: "#c678dd" }
        }
        if (id === "vitepress") {
            return { window: "#ffffff", chrome: "#f6f6f7", panel: "#f6f6f7", reader: "#ffffff",
                border: "#e2e2e3", text: "#213547", muted: "#606067", accent: "#10b981",
                hover: "#f1f1f2", pressed: "#e2e2e3", drop: "#f6f6f7", scroll: "#e2e2e3",
                code: "#f6f8fa", codeBorder: "#e2e8f0", codeHeader: "#eef0f3", codeLineBorder: "#e2e8f0", quote: "#f6f6f7", table: "#f6f6f7",
                hlK: "#cf222e", hlT: "#10b981", hlS: "#0a3069", hlC: "#64748b", hlN: "#0550ae",
                hlF: "#8250df", hlP: "#10b981", hlKey: "#10b981", hlConst: "#0550ae",
                hlDiffAdd: "#10b981", hlDiffDel: "#ef4444", hlDiffHdr: "#8250df" }
        }
        if (id === "vscode-dark") {
            return { window: "#1e1e1e", chrome: "#252526", panel: "#252526", reader: "#1e1e1e",
                border: "#3c3c3c", text: "#cccccc", muted: "#858585", accent: "#007acc",
                hover: "#2a2d2e", pressed: "#37373d", drop: "#252526", scroll: "#424242",
                code: "#1e1e1e", codeBorder: "#333333", codeHeader: "#252526", codeLineBorder: "#2a2d2e", quote: "#252526", table: "#252526",
                hlK: "#569cd6", hlT: "#4ec9b0", hlS: "#ce9178", hlC: "#6a9955", hlN: "#b5cea8",
                hlF: "#dcdcaa", hlP: "#c586c0", hlKey: "#9cdcfe", hlConst: "#569cd6",
                hlDiffAdd: "#89d185", hlDiffDel: "#f48771", hlDiffHdr: "#c586c0" }
        }
        if (id === "paper") {
            return { window: "#fffaf0", chrome: "#f7f0e1", panel: "#f7f0e1", reader: "#fffaf0",
                border: "#d7c9b4", text: "#3b3228", muted: "#76695a", accent: "#9a5b2d",
                hover: "#ece1ce", pressed: "#dfcdae", drop: "#f7f0e1", scroll: "#d7c9b4",
                code: "#f4ece0", codeBorder: "#d8ccb5", codeHeader: "#e9decb", codeLineBorder: "#ded2bd", quote: "#f7f0e1", table: "#f7f0e1",
                hlK: "#b82424", hlT: "#8f5902", hlS: "#1b7340", hlC: "#8c7e6d", hlN: "#c24e00",
                hlF: "#7030a0", hlP: "#9c27b0", hlKey: "#1b7340", hlConst: "#b82424",
                hlDiffAdd: "#1b7340", hlDiffDel: "#b82424", hlDiffHdr: "#7030a0" }
        }
        return { window: "#ffffff", chrome: "#f6f8fa", panel: "#f6f8fa", reader: "#ffffff",
            border: "#d0d7de", text: "#1f2328", muted: "#656d76", accent: "#0969da",
            hover: "#eaeef2", pressed: "#d0d7de", drop: "#f6f8fa", scroll: "#d0d7de",
            code: "#f8fafc", codeBorder: "#e2e8f0", codeHeader: "#f1f5f9", codeLineBorder: "#e2e8f0", quote: "#f8fafc", table: "#f8fafc",
            hlK: "#cf222e", hlT: "#953800", hlS: "#0a3069", hlC: "#64748b", hlN: "#0550ae",
            hlF: "#8250df", hlP: "#cf222e", hlKey: "#116329", hlConst: "#0550ae",
            hlDiffAdd: "#15803d", hlDiffDel: "#b91c1c", hlDiffHdr: "#8250df" }
    }

    property var theme: themeFor(workspaceController.themeId)
    property bool hasDocument: documentController.filePath.length > 0
    property bool outlineManuallyClosed: false
    property bool hasOutline: documentController.outline.length > 0
    property bool outlineVisible: hasOutline && !outlineManuallyClosed
    property bool recentExpanded: true
    property real outlineWidth: 224
    color: theme.window

    function activateHistoryItem(item) {
        if (!item.available) {
            missingItemDialog.targetPath = item.path
            missingItemDialog.targetTitle = item.title
            missingItemDialog.open()
            return
        }
        if (item.type === "folder") workspaceController.toggleFolder(item.path)
        else workspaceController.openPath(item.path)
    }

    Connections {
        target: documentController
        function onDocumentChanged() {
            outlineManuallyClosed = false
        }
    }

    // This is the inherited palette for standard Qt controls and menus.
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

    component ActionButton: Button {
        id: control
        implicitHeight: 30
        implicitWidth: 88
        font.pixelSize: 13
        contentItem: Label {
            text: control.text
            color: theme.text
            font: control.font
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            leftPadding: 8
        }
        background: Rectangle {
            radius: 4
            color: control.down ? theme.pressed : control.hovered ? theme.hover : "transparent"
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
        width: 8
        hoverEnabled: false
        padding: 2
        minimumSize: 0.025

        // Replace the platform scrollbar track entirely: no arrows, no end caps.
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
            opacity: control.size < 1.0 ? 0.48 : 0
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
        title: "添加文件夹"
        onAccepted: workspaceController.handleUrl(selectedFolder)
    }

    ContextMenu {
        id: workspaceFileMenu
        ContextMenuItem { text: "打开文件…"; onTriggered: fileDialog.open() }
        ContextMenuItem { text: "添加文件夹…"; onTriggered: folderDialog.open() }
    }

    ContextMenu {
        id: recentMenu
        property string targetPath: ""
        ContextMenuItem { text: "打开"; onTriggered: workspaceController.openPath(recentMenu.targetPath) }
        ContextMenuItem { text: "复制文件路径"; onTriggered: workspaceController.copyPath(recentMenu.targetPath) }
        ContextMenuItem { text: "在文件管理器显示"; onTriggered: workspaceController.revealPath(recentMenu.targetPath) }
        ContextMenuSeparator {}
        ContextMenuItem { text: "从最近列表移除"; onTriggered: workspaceController.removeRecentFile(recentMenu.targetPath) }
    }

    ContextMenu {
        id: folderFileMenu
        property string targetPath: ""
        ContextMenuItem { text: "打开"; onTriggered: workspaceController.openFolderDocument(folderFileMenu.targetPath) }
        ContextMenuItem { text: "复制文件路径"; onTriggered: workspaceController.copyPath(folderFileMenu.targetPath) }
        ContextMenuItem { text: "在文件管理器显示"; onTriggered: workspaceController.revealPath(folderFileMenu.targetPath) }
    }

    ContextMenu {
        id: folderMenu
        property string targetPath: ""
        ContextMenuItem { text: "打开文件夹"; onTriggered: workspaceController.openFolder(folderMenu.targetPath) }
        ContextMenuItem { text: "复制文件夹路径"; onTriggered: workspaceController.copyPath(folderMenu.targetPath) }
        ContextMenuItem { text: "在文件管理器显示"; onTriggered: workspaceController.revealPath(folderMenu.targetPath) }
        ContextMenuSeparator {}
        ContextMenuItem { text: "从工作区移除"; onTriggered: workspaceController.removeFolder(folderMenu.targetPath) }
    }

    ContextMenu {
        id: historyMenu
        property string targetPath: ""
        property string targetType: "file"
        property bool targetAvailable: true
        ContextMenuItem {
            text: historyMenu.targetType === "folder" ? "展开文件夹" : "打开"
            onTriggered: activateHistoryItem({ path: historyMenu.targetPath, type: historyMenu.targetType,
                available: historyMenu.targetAvailable, title: "" })
        }
        ContextMenuItem { text: "复制路径"; onTriggered: workspaceController.copyPath(historyMenu.targetPath) }
        ContextMenuItem { text: "在文件管理器显示"; onTriggered: workspaceController.revealPath(historyMenu.targetPath) }
        ContextMenuSeparator {}
        ContextMenuItem { text: "从最近访问移除"; onTriggered: workspaceController.removeHistoryItem(historyMenu.targetPath) }
    }

    ContextMenu {
        id: settingsMenu
        ContextMenuItem {
            text: "外观 ›"
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
                    placeholderText: "搜索最近访问的文件或文件夹"
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
                    Column {
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.right: parent.right
                        anchors.rightMargin: 70
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 2
                        Label { width: parent.width; text: modelData.title; elide: Text.ElideRight; color: modelData.available ? theme.text : theme.muted; font.pixelSize: 13 }
                        Label { width: parent.width; text: modelData.path; elide: Text.ElideMiddle; color: theme.muted; font.pixelSize: 11 }
                    }
                    Label {
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.available ? (modelData.type === "folder" ? "文件夹" : "文件") : "不可用"
                        color: modelData.available ? theme.muted : theme.accent
                        font.pixelSize: 11
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
            Label {
                visible: quickOpen.results.length === 0
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 24
                text: "没有匹配的最近访问"
                color: theme.muted
                font.pixelSize: 13
            }
        }
    }

    Shortcut { sequence: "Ctrl+K"; onActivated: quickOpen.open() }
    Shortcut { sequence: "Meta+K"; onActivated: quickOpen.open() }
    Shortcut { sequence: "Ctrl+B"; onActivated: workspaceController.sidebarVisible = !workspaceController.sidebarVisible }
    Shortcut { sequence: "Meta+B"; onActivated: workspaceController.sidebarVisible = !workspaceController.sidebarVisible }

    RowLayout {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 0

        Rectangle {
            id: sidebar
            Layout.fillHeight: true
            Layout.preferredWidth: workspaceController.sidebarVisible ? workspaceController.sidebarWidth : 0
            visible: workspaceController.sidebarVisible
            clip: true
            color: theme.panel

            Flickable {
                id: sidebarScroll
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: sidebarSettings.top
                anchors.leftMargin: 14
                anchors.rightMargin: 10
                anchors.topMargin: 18
                anchors.bottomMargin: 12
                contentWidth: width - 4
                contentHeight: sidebarContent.implicitHeight
                clip: true
                ScrollBar.vertical: AppScrollBar {}

                Column {
                    id: sidebarContent
                    width: sidebarScroll.contentWidth
                    spacing: 8

                    RowLayout {
                        width: parent.width
                        Label { text: "FlashRead"; color: theme.text; font.pixelSize: 15; font.weight: Font.DemiBold }
                        Item { Layout.fillWidth: true }
                        Label {
                            text: Qt.platform.os === "osx" ? "⌘K" : "Ctrl+K"
                            color: theme.muted
                            font.pixelSize: 10
                            rightPadding: 4
                        }
                        Button {
                            id: sidebarCollapseBtn
                            implicitWidth: 24
                            implicitHeight: 24
                            ToolTip.visible: hovered
                            ToolTip.text: "收起导航 (Ctrl+B)"
                            contentItem: Label {
                                text: "«"
                                color: parent.hovered ? theme.text : theme.muted
                                font.pixelSize: 14
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                radius: 4
                                color: parent.down ? theme.pressed : parent.hovered ? theme.hover : "transparent"
                            }
                            onClicked: workspaceController.sidebarVisible = false
                        }
                    }

                    ActionButton {
                        id: workspaceFileButton
                        width: parent.width
                        text: "+  文件"
                        onClicked: workspaceFileMenu.popup(workspaceFileButton, 0, workspaceFileButton.height + 4)
                    }

                    Label {
                        width: parent.width
                        topPadding: 12
                        bottomPadding: 2
                        text: workspaceController.workspaceTitle.length > 0
                            ? "当前工作区  ·  " + workspaceController.workspaceTitle : "当前工作区"
                        color: theme.muted
                        font.pixelSize: 11
                        font.letterSpacing: 0.6
                        elide: Text.ElideRight
                    }

                    Repeater {
                        model: workspaceController.workspaceItems
                        delegate: Rectangle {
                            required property var modelData
                            width: sidebarContent.width
                            height: 36
                            radius: 3
                            color: workspaceMouse.containsMouse
                                || (modelData.type !== "workspaceFolder" && documentController.filePath === modelData.path)
                                || modelData.type === "workspaceFolder"
                                ? theme.hover : "transparent"
                            Behavior on color { ColorAnimation { duration: 120 } }
                            Rectangle {
                                anchors.left: parent.left
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                width: 2
                                visible: documentController.filePath === modelData.path
                                color: theme.accent
                            }
                            Label {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter
                                text: modelData.type === "workspaceFolder" ? "⌄" : "•"
                                color: modelData.type === "workspaceFolder" ? theme.accent : theme.muted
                                font.pixelSize: 14
                            }
                            Label {
                                anchors.left: parent.left
                                anchors.leftMargin: modelData.type === "folderFile" ? 40 : 24
                                anchors.right: workspaceMore.left
                                anchors.rightMargin: 4
                                anchors.verticalCenter: parent.verticalCenter
                                text: modelData.title
                                elide: Text.ElideRight
                                color: modelData.available ? theme.text : theme.muted
                                font.pixelSize: 13
                            }
                            Label {
                                id: workspaceMore
                                anchors.right: parent.right
                                anchors.rightMargin: 8
                                anchors.verticalCenter: parent.verticalCenter
                                text: "···"
                                color: theme.muted
                                font.pixelSize: 15
                                visible: workspaceMouse.containsMouse
                            }
                            Label {
                                anchors.right: parent.right
                                anchors.rightMargin: 8
                                anchors.verticalCenter: parent.verticalCenter
                                text: "不可用"
                                color: theme.accent
                                font.pixelSize: 10
                                visible: !modelData.available && !workspaceMouse.containsMouse
                            }
                            MouseArea {
                                id: workspaceMouse
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                hoverEnabled: true
                                onClicked: function(mouse) {
                                    if (mouse.button === Qt.RightButton) {
                                        if (modelData.type === "workspaceFolder") {
                                            historyMenu.targetPath = modelData.path
                                            historyMenu.targetType = "folder"
                                            historyMenu.targetAvailable = modelData.available
                                            historyMenu.popup(workspaceMouse, mouse.x, mouse.y)
                                        } else if (modelData.type === "folderFile") {
                                            folderFileMenu.targetPath = modelData.path
                                            folderFileMenu.popup(workspaceMouse, mouse.x, mouse.y)
                                        } else {
                                            historyMenu.targetPath = modelData.path
                                            historyMenu.targetType = "file"
                                            historyMenu.targetAvailable = modelData.available
                                            historyMenu.popup(workspaceMouse, mouse.x, mouse.y)
                                        }
                                    } else if (modelData.type === "workspaceFolder") {
                                        workspaceController.toggleFolder(modelData.path)
                                    } else if (modelData.type === "folderFile") {
                                        workspaceController.openFolderDocument(modelData.path)
                                    } else {
                                        activateHistoryItem({ path: modelData.path, type: "file",
                                            available: modelData.available, title: modelData.title })
                                    }
                                }
                            }
                        }
                    }

                    Label {
                        visible: workspaceController.workspaceItems.length === 0
                        width: parent.width
                        topPadding: 6
                        text: "从“文件”开始打开内容"
                        color: theme.muted
                        font.pixelSize: 12
                    }

                    Item { width: parent.width; height: 8 }
                    Rectangle { width: parent.width; height: 1; color: theme.border }

                    Button {
                        id: recentToggle
                        width: parent.width
                        height: 32
                        text: (recentExpanded ? "⌄" : "›") + "  最近访问  " + workspaceController.recentItems.length
                        onClicked: recentExpanded = !recentExpanded
                        contentItem: Label {
                            text: recentToggle.text
                            color: theme.muted
                            font.pixelSize: 12
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 3
                            color: recentToggle.down ? theme.pressed : recentToggle.hovered ? theme.hover : "transparent"
                        }
                    }

                    Column {
                        id: recentHistory
                        width: parent.width
                        spacing: 2
                        height: recentExpanded ? implicitHeight : 0
                        opacity: recentExpanded ? 1 : 0
                        visible: recentExpanded || height > 0
                        clip: true
                        Behavior on height { NumberAnimation { duration: 170; easing.type: Easing.OutCubic } }
                        Behavior on opacity { NumberAnimation { duration: 120 } }
                        Repeater {
                            model: workspaceController.recentItems
                            delegate: Rectangle {
                                required property var modelData
                                width: sidebarContent.width
                                height: 32
                                radius: 3
                                color: recentMouse.containsMouse ? theme.hover : "transparent"
                                Behavior on color { ColorAnimation { duration: 120 } }
                                Label {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 8
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: modelData.type === "folder" ? "›" : "•"
                                    color: modelData.type === "folder" ? theme.accent : theme.muted
                                    font.pixelSize: 13
                                }
                                Label {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 24
                                    anchors.right: recentMore.left
                                    anchors.rightMargin: 4
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: modelData.title
                                    elide: Text.ElideRight
                                    color: modelData.available ? theme.text : theme.muted
                                    font.pixelSize: 12
                                }
                                Label {
                                    id: recentMore
                                    anchors.right: parent.right
                                    anchors.rightMargin: 8
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "···"
                                    color: theme.muted
                                    font.pixelSize: 14
                                    visible: recentMouse.containsMouse
                                }
                                MouseArea {
                                    id: recentMouse
                                    anchors.fill: parent
                                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                                    hoverEnabled: true
                                    onClicked: function(mouse) {
                                        if (mouse.button === Qt.RightButton) {
                                            historyMenu.targetPath = modelData.path
                                            historyMenu.targetType = modelData.type
                                            historyMenu.targetAvailable = modelData.available
                                            historyMenu.popup(recentMouse, mouse.x, mouse.y)
                                        } else {
                                            activateHistoryItem(modelData)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: sidebarSettings
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 48
                color: "transparent"
                Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: theme.border }
                Button {
                    id: settingsButton
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - 20
                    height: 28
                    text: "设置"
                    contentItem: Label {
                        text: parent.text
                        color: parent.hovered ? theme.text : theme.muted
                        font.pixelSize: 12
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: 4
                    }
                    background: Rectangle { radius: 3; color: parent.hovered ? theme.hover : "transparent" }
                    onClicked: settingsMenu.popup(settingsButton, 0, -settingsMenu.implicitHeight)
                }
            }
        }

        Item {
            id: sidebarSplitter
            Layout.fillHeight: true
            Layout.preferredWidth: 6
            visible: workspaceController.sidebarVisible
            z: 10

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

                onDoubleClicked: {
                    workspaceController.setSidebarWidth(240)
                }
            }
        }

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

            Rectangle {
                id: readerTopBar
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 38
                color: theme.chrome
                z: 10

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

                    Button {
                        id: sidebarToggleBtn
                        implicitWidth: 28
                        implicitHeight: 28
                        ToolTip.visible: hovered
                        ToolTip.text: workspaceController.sidebarVisible ? "收起导航 (Ctrl+B)" : "展开导航 (Ctrl+B)"
                        contentItem: Label {
                            text: "☰"
                            color: parent.hovered ? theme.text : theme.muted
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 4
                            color: parent.down ? theme.pressed : parent.hovered ? theme.hover : "transparent"
                        }
                        onClicked: workspaceController.sidebarVisible = !workspaceController.sidebarVisible
                    }

                    Label {
                        text: documentController.title.length > 0 ? documentController.title : "FlashRead"
                        color: theme.text
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    Button {
                        id: searchButton
                        implicitWidth: 28
                        implicitHeight: 28
                        ToolTip.visible: hovered
                        ToolTip.text: "快速查找 (Ctrl+K)"
                        contentItem: Label {
                            text: "🔍"
                            font.pixelSize: 12
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 4
                            color: parent.down ? theme.pressed : parent.hovered ? theme.hover : "transparent"
                        }
                        onClicked: quickOpen.open()
                    }

                    Button {
                        id: themeButton
                        implicitWidth: 28
                        implicitHeight: 28
                        ToolTip.visible: hovered
                        ToolTip.text: "切换主题外观"
                        contentItem: Label {
                            text: "🎨"
                            font.pixelSize: 13
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 4
                            color: parent.down ? theme.pressed : parent.hovered ? theme.hover : "transparent"
                        }
                        onClicked: appearanceMenu.popup(themeButton, -appearanceMenu.implicitWidth + themeButton.width, themeButton.height + 4)
                    }

                    Button {
                        id: outlineBtn
                        visible: hasOutline
                        implicitHeight: 28
                        implicitWidth: 68
                        ToolTip.visible: hovered
                        ToolTip.text: outlineVisible ? "收起目录" : "展开目录"
                        contentItem: RowLayout {
                            spacing: 4
                            Label {
                                text: "📑"
                                font.pixelSize: 12
                                verticalAlignment: Text.AlignVCenter
                            }
                            Label {
                                text: "目录"
                                color: outlineBtn.hovered || outlineVisible ? theme.text : theme.muted
                                font.pixelSize: 12
                                verticalAlignment: Text.AlignVCenter
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

            ColumnLayout {
                anchors.centerIn: parent
                width: Math.min(parent.width - 64, 480)
                visible: !hasDocument && !workspaceController.folderViewActive
                spacing: 10

                Label { text: "FlashRead"; color: theme.accent; font.pixelSize: 17; font.weight: Font.DemiBold; Layout.alignment: Qt.AlignHCenter }
                Label { text: "开始阅读"; color: theme.text; font.pixelSize: 28; font.weight: Font.DemiBold; Layout.alignment: Qt.AlignHCenter }
                Label { text: "拖入 Markdown、文本文件或文件夹"; color: theme.muted; font.pixelSize: 14; Layout.alignment: Qt.AlignHCenter }
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 14
                    ActionButton { text: "打开文件"; onClicked: fileDialog.open() }
                    ActionButton { text: "打开文件夹"; implicitWidth: 96; onClicked: folderDialog.open() }
                }
            }

            Rectangle {
                id: outlinePanel
                anchors.top: readerTopBar.bottom
                anchors.bottom: parent.bottom
                width: outlineWidth
                x: hasDocument && !workspaceController.folderViewActive && outlineVisible ? parent.width - outlineWidth : parent.width
                visible: x < parent.width
                clip: true
                color: theme.panel
                z: 5
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
                        Behavior on color { ColorAnimation { duration: 120 } }
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
                    anchors.margins: 14
                    spacing: 6
                    RowLayout {
                        width: parent.width
                        Label { text: "文档目录"; color: theme.text; font.pixelSize: 13; font.weight: Font.DemiBold }
                        Item { Layout.fillWidth: true }
                        Label { text: documentController.outline.length; color: theme.muted; font.pixelSize: 11; rightPadding: 4 }
                        Button {
                            implicitWidth: 22
                            implicitHeight: 22
                            ToolTip.visible: hovered
                            ToolTip.text: "关闭目录"
                            contentItem: Label {
                                text: "✕"
                                color: parent.hovered ? theme.text : theme.muted
                                font.pixelSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                radius: 4
                                color: parent.down ? theme.pressed : parent.hovered ? theme.hover : "transparent"
                            }
                            onClicked: outlineManuallyClosed = true
                        }
                    }
                    Label {
                        width: parent.width
                        text: "跳到对应章节"
                        color: theme.muted
                        font.pixelSize: 11
                        bottomPadding: 4
                    }
                    ListView {
                        id: outlineList
                        width: parent.width
                        height: parent.height - 52
                        clip: true
                        model: documentController.outline
                        spacing: 2
                        ScrollBar.vertical: AppScrollBar {}
                        delegate: Rectangle {
                            required property var modelData
                            width: outlineList.width - 10
                            height: 32
                            radius: 5
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

            MarkdownWebView {
                id: markdownWebView
                anchors.top: readerTopBar.bottom
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.rightMargin: hasDocument && !workspaceController.folderViewActive && outlineVisible ? outlineWidth : 0
                visible: hasDocument && !workspaceController.folderViewActive
                htmlContent: documentController.renderedContent
                themeId: workspaceController.themeId

                onLinkClicked: function(url) {
                    Qt.openUrlExternally(url)
                }
            }

            Rectangle {
                anchors.fill: parent
                color: theme.window
                visible: documentController.isLoading
                opacity: documentController.isLoading ? 0.7 : 0.0
                z: 15
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
