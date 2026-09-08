import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import QtQuick.Layouts

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
            return { window: "#171816", chrome: "#20211e", panel: "#1c1d1a", reader: "#171816",
                border: "#363832", text: "#e6e7e1", muted: "#9b9d94", accent: "#79b68f",
                hover: "#292b26", pressed: "#34362f", drop: "#1d3427", scroll: "#85887e",
                code: "#20211e", quote: "#20211e", table: "#1d1e1b" }
        }
        if (id === "vscode-dark") {
            return { window: "#181818", chrome: "#222222", panel: "#1e1e1e", reader: "#1e1e1e",
                border: "#3c3c3c", text: "#d4d4d4", muted: "#9d9d9d", accent: "#3794ff",
                hover: "#2a2d2e", pressed: "#37373d", drop: "#15395b", scroll: "#797979",
                code: "#252526", quote: "#252526", table: "#252526" }
        }
        if (id === "paper") {
            return { window: "#f3eddf", chrome: "#ece3d1", panel: "#f7f0e1", reader: "#fffaf0",
                border: "#d7c9b4", text: "#3b3228", muted: "#76695a", accent: "#9a5b2d",
                hover: "#e9ddc7", pressed: "#dfcdae", drop: "#f1dfbf", scroll: "#ad9474",
                code: "#f1e8d8", quote: "#f6eddd", table: "#f8f0e2" }
        }
        return { window: "#f8f8f5", chrome: "#f0f0ec", panel: "#f3f3ef", reader: "#f8f8f5",
            border: "#dfdfd8", text: "#242520", muted: "#74766d", accent: "#2f6b4f",
            hover: "#e8e9e2", pressed: "#dcddd5", drop: "#e5f1e9", scroll: "#989990",
            code: "#efefeb", quote: "#f0f1eb", table: "#f2f2ee" }
    }

    function documentHtml() {
        const css = "<style>"
            + "body { color:" + theme.text + "; font-family:'" + systemFontFamily + "'; font-size:15px; line-height:1.62; }"
            + "p { margin-top:0; margin-bottom:18px; }"
            + "h1 { font-size:28px; margin-top:0; margin-bottom:22px; padding-bottom:12px; border-bottom:1px solid " + theme.border + "; }"
            + "h2 { font-size:22px; margin-top:34px; margin-bottom:16px; padding-bottom:8px; border-bottom:1px solid " + theme.border + "; }"
            + "h3 { font-size:18px; margin-top:28px; margin-bottom:12px; }"
            + "h4, h5, h6 { margin-top:24px; margin-bottom:10px; }"
            + "ul, ol { margin-top:0; margin-bottom:18px; }"
            + "li { margin-bottom:6px; }"
            + "blockquote { margin:0 0 18px 0; padding:10px 14px; color:" + theme.muted
            + "; background-color:" + theme.quote + "; border-left:4px solid " + theme.accent + "; }"
            + "pre { margin:0 0 18px 0; padding:14px; background-color:" + theme.code
            + "; border:1px solid " + theme.border + "; font-family:'" + monospaceFontFamily + "', monospace; line-height:1.45; }"
            + "code { font-family:'" + monospaceFontFamily + "', monospace; background-color:" + theme.code + "; }"
            + "table { margin:0 0 18px 0; border:1px solid " + theme.border + "; border-collapse:collapse; }"
            + "th { background-color:" + theme.table + "; font-weight:bold; }"
            + "td, th { padding:8px; border:1px solid " + theme.border + "; }"
            + "a { color:" + theme.accent + "; }"
            + "hr { color:" + theme.border + "; }"
            + "</style>"
        return css + "<div>" + documentController.renderedContent + "</div>"
    }

    property var theme: themeFor(workspaceController.themeId)
    property bool hasDocument: documentController.filePath.length > 0
    property bool outlineManuallyClosed: false
    property bool hasOutline: documentController.outline.length > 0
    property bool outlineVisible: hasOutline && !outlineManuallyClosed
    property bool recentExpanded: false
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
            readerScroll.contentY = 0
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

    component OutlineToggle: Button {
        id: control
        implicitWidth: 30
        implicitHeight: 30
        ToolTip.visible: control.hovered
        ToolTip.text: outlineVisible ? "关闭目录" : "打开目录"
        contentItem: Item {
            Repeater {
                model: 3
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 9 + index * 5
                    width: 12
                    height: 1
                    color: control.hovered ? theme.text : theme.muted
                }
            }
        }
        background: Rectangle {
            radius: 6
            color: control.down ? theme.pressed : control.hovered ? theme.hover : "transparent"
        }
    }

    component ContextMenu: Menu {
        popupType: Popup.Item
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
        ContextMenuItem { text: "GitHub Light"; onTriggered: workspaceController.setThemeId("github-light") }
        ContextMenuItem { text: "GitHub Dark"; onTriggered: workspaceController.setThemeId("github-dark") }
        ContextMenuItem { text: "VS Code Dark+"; onTriggered: workspaceController.setThemeId("vscode-dark") }
        ContextMenuItem { text: "Paper"; onTriggered: workspaceController.setThemeId("paper") }
    }

    Popup {
        id: missingItemDialog
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

    RowLayout {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 0

        Rectangle {
            id: sidebar
            Layout.fillHeight: true
            Layout.preferredWidth: 240
            color: theme.panel

            Rectangle {
                anchors.right: parent.right
                width: 1
                height: parent.height
                color: theme.border
            }

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
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                width: hasDocument && !workspaceController.folderViewActive && outlineVisible ? 224 : 0
                visible: width > 0
                clip: true
                color: theme.panel
                Behavior on width { NumberAnimation { duration: 170; easing.type: Easing.OutCubic } }

                Rectangle {
                    anchors.left: parent.left
                    width: 1
                    height: parent.height
                    color: theme.border
                }

                Column {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 6
                    RowLayout {
                        width: parent.width
                        Label { text: "文档目录"; color: theme.text; font.pixelSize: 13; font.weight: Font.DemiBold }
                        Item { Layout.fillWidth: true }
                        Label { text: documentController.outline.length; color: theme.muted; font.pixelSize: 11 }
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
                        height: parent.height - 48
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
                                onClicked: readerScroll.contentY = Math.max(0,
                                    modelData.progress * Math.max(0, readerScroll.contentHeight - readerScroll.height))
                            }
                        }
                    }
                }
            }

            Flickable {
                id: readerScroll
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.right: outlinePanel.width > 0 ? outlinePanel.left : parent.right
                visible: hasDocument && !workspaceController.folderViewActive
                contentWidth: width
                contentHeight: readerPage.height + 96
                clip: true
                ScrollBar.vertical: AppScrollBar {}
                maximumFlickVelocity: 4200
                flickDeceleration: 1800

                WheelHandler {
                    target: readerScroll
                    blocking: true
                    onWheel: function(event) {
                        const delta = event.pixelDelta.y !== 0
                            ? event.pixelDelta.y * 1.6 : event.angleDelta.y / 120 * 88
                        const maximum = Math.max(0, readerScroll.contentHeight - readerScroll.height)
                        readerScroll.contentY = Math.max(0, Math.min(maximum, readerScroll.contentY - delta))
                        event.accepted = true
                    }
                }

                Item {
                    width: readerScroll.width
                    height: readerPage.height + 96
                    Item {
                        id: readerPage
                        width: Math.min(parent.width - 96, 780)
                        height: documentText.implicitHeight + 112
                        x: (parent.width - width) / 2
                        y: 48
                        Text {
                            id: documentText
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20
                            anchors.topMargin: 36
                            anchors.bottomMargin: 36
                            text: documentHtml()
                            wrapMode: Text.Wrap
                            textFormat: Text.RichText
                            font.family: systemFontFamily
                            font.pixelSize: 15
                            lineHeight: 1.52
                            color: theme.text
                        }
                    }
                }
            }

            OutlineToggle {
                anchors.top: readerScroll.top
                anchors.right: readerScroll.right
                anchors.topMargin: 14
                anchors.rightMargin: 16
                z: 2
                visible: hasDocument && !workspaceController.folderViewActive && hasOutline
                onClicked: outlineManuallyClosed = !outlineManuallyClosed
            }
        }
    }
}
