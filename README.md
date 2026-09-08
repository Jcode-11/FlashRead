# FlashRead

> 快速打开，安静阅读。

FlashRead 是一个面向 Windows 和 macOS 的原生 Markdown / 文本查看器。它不试图成为另一个编辑器、知识库或 IDE，而是专注于一件事：**双击一个文件后，尽快把可读内容放到你眼前。**

## 灵感与目的

很多 Markdown 工具功能很全面，但打开一个文件时，常常要先等待工作区、扩展、索引、浏览器内核和各种后台服务。对“只想读一篇文档”的场景来说，这些等待不应该是默认成本。

FlashRead 的灵感来自一个很朴素的想法：Markdown 本质上首先是文本，查看器应像打开一页纸一样直接、安静、可靠。

因此，项目刻意坚持以下边界：

- 不以内置浏览器内核作为默认阅读路径。
- 不启动时扫描文件夹、索引整个工作区或加载插件。
- 不把“功能丰富”误认为“阅读体验更好”。
- 优先优化冷启动、首屏内容和稳定的排版层级。

## 当前能力

- 原生 C++20 + Qt Quick 窗口与界面。
- 基于 MD4C 的 CommonMark 与常用 GFM 预览。
- 打开文件、拖入文件或拖入文件夹。
- 首次读取最多 256 KB，优先显示可读首屏。
- 左侧以当前工作区为中心：打开文件夹后递归列出 Markdown / 文本文件；打开单文件后只显示当前文件。
- 最近访问默认折叠，支持快捷键 `Ctrl+K` / `⌘K` 搜索、复制路径、在文件管理器显示和移除失效项目。
- 正文右上角可展开文档目录，按标题快速跳转。
- GitHub Light、GitHub Dark、VS Code Dark+、Paper 四套全局主题。
- 面向阅读的正文排版：标题、段落、列表、引用、代码块和表格拥有统一层级。
- 细窄的自绘滚动条与加快的正文滚动，内容无需滚动时自动隐藏。

## 设计原则

| 原则 | 说明 |
| --- | --- |
| 首屏优先 | 文件打开后先让用户看到内容，再考虑渐进处理。 |
| 阅读优先 | 正文宽度、行距、标题层级和代码块服务于长时间阅读。 |
| 全局主题 | 主题是应用级 token，不是只替换一个背景色。 |
| 按需工作区 | 只在主动打开文件夹时递归读取可读文件，不在启动阶段遍历磁盘。 |
| 原生默认 | 默认路径不依赖 Electron 或 WebView。 |

## 技术栈

- **C++20**：文件读取、解析调度、缓存与系统集成。
- **Qt 6 / Qt Quick**：原生窗口、控件和跨平台基础。
- **QML**：界面布局、主题 token 与交互。
- **MD4C 0.5.3**：轻量 Markdown 解析；CMake 在首次构建时拉取固定提交。

## 构建

### 环境

- CMake 3.24+、Ninja、Qt 6.8+（需要 Qt Quick 与 Quick Dialogs）。
- Windows：使用 Qt kit 配套的 MinGW；不能混用 MSVC 版 Qt。
- macOS：安装 Xcode Command Line Tools 与 macOS Qt kit。
- `QT_ROOT` 指向 Qt kit 目录；CMake、Ninja 和编译器需在 PATH 中。
- 首次配置需要联网下载 MD4C。Windows 与 macOS 共用 `qml/Main.qml` 和四套主题，正文与代码字体分别取系统常规字体和等宽字体。

### Windows / PowerShell

在项目根目录运行。下面路径是示例，请按本机 Qt 安装位置调整：

```powershell
$env:QT_ROOT = "C:\Qt\6.11.2\mingw_64"
$env:Path = "C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja;C:\Qt\Tools\mingw1310_64\bin;$env:QT_ROOT\bin;$env:Path"
cmake --preset mingw-release
cmake --build --preset mingw-release
.\build\mingw-release\FlashRead.exe .\tests\fixtures\sample.md
```

部署到独立目录，收集 Qt、QML 和编译器运行时依赖：

```powershell
cmake --install build/mingw-release --prefix build/package/windows
.\build\package\windows\bin\FlashRead.exe
```

分发时保留整个 `build/package/windows` 目录，不能只拷贝 EXE。当前不生成安装向导，也不自动注册 Windows 文件关联。

### macOS / Terminal

在项目根目录运行，`QT_ROOT` 按实际安装路径调整：

```sh
export QT_ROOT="$HOME/Qt/6.11.2/macos"
cmake --preset macos-release
cmake --build --preset macos-release
open build/macos-release/FlashRead.app
```

默认构建当前机器架构，Intel 与 Apple Silicon 应分别构建验证。需要通过终端打开指定文件时：

```sh
./build/macos-release/FlashRead.app/Contents/MacOS/FlashRead ./tests/fixtures/sample.md
```

部署包含 Qt 与 QML 依赖的 `.app`：

```sh
cmake --install build/macos-release --prefix build/package/macos
open build/package/macos/FlashRead.app
```

应用声明支持 Markdown 和 TXT，可通过 Finder 的“打开方式”选择 FlashRead，或将文件拖到 Dock 图标；应用已运行时也接收文件打开事件。不会主动更改系统默认打开方式。当前未配置 Developer ID 签名、公证或 DMG 制作。

### 两端验收

- 启动窗口，检查四套主题、中文、代码等宽字体和高 DPI 缩放。
- 通过文件对话框、命令行及拖拽打开文件，检查含中文和空格的路径。
- 打开文件夹，检查递归读取、当前工作区、最近访问折叠面板和主题是否正常。
- macOS 分别在应用退出和已运行时，通过 Finder／Dock 打开文件。
- 在未安装 Qt 的目标机器上验证部署目录，确认依赖完整。

构建配置与系统入口已适配；这份清单不代表两端已通过实机验收。

## 项目状态

FlashRead 仍处于早期开发阶段。当前重点是把“打开快、滚动稳、读起来舒服”的基础体验做扎实；Windows 文件关联、完整文件渐进渲染和正式发布包仍在后续计划中。

## License

本项目采用 [MIT License](LICENSE)。
