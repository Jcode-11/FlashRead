import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic as Basic
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
            return { window: "#0d1117", chrome: "#161b22", panel: "#0d1117", reader: "#161b22",
                border: "#30363d", text: "#e6edf3", muted: "#8b949e", accent: "#58a6ff",
                hover: "#21262d", pressed: "#30363d", drop: "#10233b", scroll: "#768390",
                code: "#0d1117", quote: "#21262d", table: "#161b22" }
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
        return { window: "#f6f8fa", chrome: "#ffffff", panel: "#f6f8fa", reader: "#ffffff",
            border: "#d8dee4", text: "#1f2328", muted: "#656d76", accent: "#0969da",
            hover: "#eaeef2", pressed: "#d8dee4", drop: "#ddf4ff", scroll: "#8c959f",
            code: "#f6f8fa", quote: "#f6f8fa", table: "#f6f8fa" }
    }

    function documentHtml() {
        const css = "<style>"
            + "body { color:" + theme.text + "; font-family:'Segoe UI'; font-size:15px; line-height:1.62; }"
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
            + "; border:1px solid " + theme.border + "; font-family:Consolas, monospace; line-height:1.45; }"
            + "code { font-family:Consolas, monospace; background-color:" + theme.code + "; }"
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
    color: theme.window

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
        implicitHeight: 32
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
            radius: 6
            border.width: 1
            border.color: theme.border
            color: control.down ? theme.pressed : control.hovered ? theme.hover : theme.chrome
        }
    }

    component AppScrollBar: Basic.ScrollBar {
        id: control

        policy: ScrollBar.AsNeeded
        width: 10
        hoverEnabled: false
        padding: 0

        // Replace the platform scrollbar track entirely: no arrows, no end caps.
        background: Rectangle {
            implicitWidth: control.width
            implicitHeight: control.height
            color: "transparent"
        }

        contentItem: Rectangle {
            implicitWidth: 6
            radius: 3
            color: theme.scroll
            opacity: control.size < 1.0 ? 0.68 : 0
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

    Rectangle {
        id: header
        anchors.top: parent.top
        width: parent.width
        height: 56
        color: theme.chrome

        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: theme.border
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 18
            anchors.rightMargin: 18
            spacing: 10

            Rectangle {
                Layout.preferredWidth: 28
                Layout.preferredHeight: 28
                radius: 8
                color: theme.accent
                Label {
                    anchors.centerIn: parent
                    text: "F"
                    color: "white"
                    font.pixelSize: 16
                    font.weight: Font.Bold
                }
            }

            Label {
                text: "FlashRead"
                color: theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Rectangle {
                Layout.preferredWidth: 1
                Layout.preferredHeight: 20
                color: theme.border
            }

            Label {
                Layout.fillWidth: true
                text: hasDocument ? documentController.filePath
                    : workspaceController.folderViewActive ? workspaceController.selectedFolder
                    : "轻量 Markdown 阅读器"
                elide: Text.ElideMiddle
                font.pixelSize: 13
                color: theme.muted
            }

            ActionButton { text: "打开文件"; onClicked: fileDialog.open() }
            ActionButton { text: "打开文件夹"; implicitWidth: 96; onClicked: folderDialog.open() }
            ActionButton {
                id: themeButton
                implicitWidth: 116
                text: workspaceController.themeId === "github-dark" ? "GitHub Dark"
                    : workspaceController.themeId === "vscode-dark" ? "VS Code Dark+"
                    : workspaceController.themeId === "paper" ? "Paper" : "GitHub Light"
                onClicked: themeMenu.open()
            }
        }
    }

    Menu {
        id: themeMenu
        x: themeButton.x
        y: header.height - 4
        MenuItem { text: "GitHub Light"; onTriggered: workspaceController.setThemeId("github-light") }
        MenuItem { text: "GitHub Dark"; onTriggered: workspaceController.setThemeId("github-dark") }
        MenuItem { text: "VS Code Dark+"; onTriggered: workspaceController.setThemeId("vscode-dark") }
        MenuItem { text: "Paper"; onTriggered: workspaceController.setThemeId("paper") }
    }

    RowLayout {
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 0

        Rectangle {
            id: sidebar
            Layout.fillHeight: true
            Layout.preferredWidth: 252
            color: theme.panel

            Rectangle {
                anchors.right: parent.right
                width: 1
                height: parent.height
                color: theme.border
            }

            Flickable {
                id: sidebarScroll
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 8
                anchors.topMargin: 16
                anchors.bottomMargin: 16
                contentWidth: width - 4
                contentHeight: sidebarContent.implicitHeight
                clip: true
                ScrollBar.vertical: AppScrollBar {}

                Column {
                    id: sidebarContent
                    width: sidebarScroll.contentWidth
                    spacing: 8

                    Label {
                        text: "工作区"
                        color: theme.muted
                        font.pixelSize: 11
                        font.letterSpacing: 1.2
                    }

                    Label {
                        text: "最近打开"
                        color: theme.text
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }

                    Repeater {
                        model: workspaceController.recentFiles
                        delegate: Rectangle {
                            required property var modelData
                            width: sidebarContent.width
                            height: 48
                            radius: 6
                            color: recentMouse.containsMouse ? theme.hover : "transparent"
                            Column {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                anchors.rightMargin: 10
                                anchors.topMargin: 7
                                spacing: 2
                                Label { width: parent.width; text: modelData.title; elide: Text.ElideRight; color: theme.text; font.pixelSize: 13; font.weight: Font.Medium }
                                Label { width: parent.width; text: modelData.location; elide: Text.ElideMiddle; color: theme.muted; font.pixelSize: 11 }
                            }
                            MouseArea {
                                id: recentMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: workspaceController.openPath(modelData.path)
                            }
                        }
                    }

                    Label {
                        visible: workspaceController.recentFiles.length === 0
                        text: "打开过的文件会显示在这里"
                        color: theme.muted
                        font.pixelSize: 12
                    }

                    Item { width: 1; height: 12 }

                    Label {
                        text: "文件夹"
                        color: theme.text
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }

                    Repeater {
                        model: workspaceController.folders
                        delegate: Rectangle {
                            required property var modelData
                            width: sidebarContent.width
                            height: 34
                            radius: 6
                            color: folderMouse.containsMouse ? theme.hover : "transparent"
                            Label {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                anchors.rightMargin: 10
                                verticalAlignment: Text.AlignVCenter
                                text: "▸  " + modelData.title
                                elide: Text.ElideRight
                                color: theme.text
                                font.pixelSize: 13
                            }
                            MouseArea {
                                id: folderMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: workspaceController.openFolder(modelData.path)
                                onPressAndHold: workspaceController.removeFolder(modelData.path)
                            }
                        }
                    }

                    Label {
                        visible: workspaceController.folders.length === 0
                        width: parent.width
                        text: "添加文件夹后，可按需浏览第一层文件"
                        wrapMode: Text.Wrap
                        color: theme.muted
                        font.pixelSize: 12
                    }
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
                spacing: 14

                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 56
                    Layout.preferredHeight: 56
                    radius: 16
                    color: theme.accent
                    Label { anchors.centerIn: parent; text: "F"; color: "white"; font.pixelSize: 27; font.weight: Font.Bold }
                }
                Label { text: "打开你要读的内容"; color: theme.text; font.pixelSize: 24; font.weight: Font.DemiBold; Layout.alignment: Qt.AlignHCenter }
                Label { text: "拖入 Markdown、文本文件或文件夹"; color: theme.muted; font.pixelSize: 14; Layout.alignment: Qt.AlignHCenter }
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 8
                    ActionButton { text: "打开文件"; onClicked: fileDialog.open() }
                    ActionButton { text: "添加文件夹"; implicitWidth: 96; onClicked: folderDialog.open() }
                }
            }

            Rectangle {
                anchors.fill: parent
                anchors.margins: 24
                visible: workspaceController.folderViewActive
                radius: 10
                color: theme.reader
                border.width: 1
                border.color: theme.border

                Column {
                    anchors.fill: parent
                    anchors.margins: 22
                    spacing: 8
                    Label { width: parent.width; text: workspaceController.selectedFolder; elide: Text.ElideMiddle; color: theme.text; font.pixelSize: 20; font.weight: Font.DemiBold }
                    Label { text: workspaceController.folderFiles.length + " 个文件"; color: theme.muted; font.pixelSize: 13 }
                    ListView {
                        id: folderList
                        width: parent.width
                        height: parent.height - 60
                        clip: true
                        model: workspaceController.folderFiles
                        spacing: 4
                        ScrollBar.vertical: AppScrollBar {}
                        delegate: Rectangle {
                            required property var modelData
                            width: folderList.width - 12
                            height: 44
                            radius: 6
                            color: fileMouse.containsMouse ? theme.hover : "transparent"
                            Label { anchors.left: parent.left; anchors.leftMargin: 12; anchors.verticalCenter: parent.verticalCenter; text: modelData.title; color: theme.text; font.pixelSize: 14 }
                            Label { anchors.right: parent.right; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter; text: modelData.kind; color: theme.muted; font.pixelSize: 11 }
                            MouseArea { id: fileMouse; anchors.fill: parent; hoverEnabled: true; onClicked: workspaceController.openPath(modelData.path) }
                        }
                    }
                }
            }

            Flickable {
                id: readerScroll
                anchors.fill: parent
                visible: hasDocument && !workspaceController.folderViewActive
                contentWidth: width
                contentHeight: readerPage.height + 48
                clip: true
                ScrollBar.vertical: AppScrollBar {}

                Item {
                    width: readerScroll.width
                    height: readerPage.height + 48
                    Rectangle {
                        id: readerPage
                        width: Math.min(parent.width - 48, 900)
                        height: documentText.implicitHeight + 72
                        x: (parent.width - width) / 2
                        y: 24
                        radius: 10
                        color: theme.reader
                        border.width: 1
                        border.color: theme.border
                        Text {
                            id: documentText
                            anchors.fill: parent
                            anchors.margins: 36
                            text: documentHtml()
                            wrapMode: Text.Wrap
                            textFormat: Text.RichText
                            font.family: "Segoe UI"
                            font.pixelSize: 15
                            lineHeight: 1.52
                            color: theme.text
                        }
                    }
                }
            }
        }
    }
}
