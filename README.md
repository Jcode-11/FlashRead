# FlashRead

> 快速打开，安静阅读。

FlashRead 是一个 Windows 优先的原生 Markdown / 文本查看器。它不试图成为另一个编辑器、知识库或 IDE，而是专注于一件事：**双击一个文件后，尽快把可读内容放到你眼前。**

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
- 最近文件与轻量文件夹工作区。
- 文件夹仅在主动打开时列出第一层 Markdown / 文本文件；不递归扫描。
- GitHub Light、GitHub Dark、VS Code Dark+、Paper 四套全局主题。
- 面向阅读的正文排版：标题、段落、列表、引用、代码块和表格拥有统一层级。
- 自绘滚动条：没有系统箭头，内容无需滚动时自动隐藏。

## 设计原则

| 原则 | 说明 |
| --- | --- |
| 首屏优先 | 文件打开后先让用户看到内容，再考虑渐进处理。 |
| 阅读优先 | 正文宽度、行距、标题层级和代码块服务于长时间阅读。 |
| 全局主题 | 主题是应用级 token，不是只替换一个背景色。 |
| 按需工作区 | 记住入口，不在启动阶段遍历磁盘。 |
| 原生默认 | 默认路径不依赖 Electron 或 WebView。 |

## 技术栈

- **C++20**：文件读取、解析调度、缓存与系统集成。
- **Qt 6 / Qt Quick**：原生窗口、控件和跨平台基础。
- **QML**：界面布局、主题 token 与交互。
- **MD4C 0.5.3**：轻量 Markdown 解析；CMake 在首次构建时拉取固定提交。

## 构建

### 环境

- CMake 3.24+
- Qt 6.11.2 MinGW 64-bit kit
- Qt Online Installer 随 kit 安装的 MinGW、CMake 和 Ninja

### Windows / PowerShell

```powershell
cd C:\JcodeWorkSpace\Jcodedevelop\FlashRead

C:\Qt\Tools\CMake_64\bin\cmake.exe --preset mingw-release
C:\Qt\Tools\CMake_64\bin\cmake.exe --build --preset mingw-release
```

运行：

```powershell
$env:Path = "C:\Qt\6.11.2\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;$env:Path"
.\build\mingw-release\FlashRead.exe
```

也可以直接传入文件路径：

```powershell
.\build\mingw-release\FlashRead.exe .\tests\fixtures\sample.md
```

## 项目状态

FlashRead 仍处于早期开发阶段。当前重点是把“打开快、滚动稳、读起来舒服”的基础体验做扎实；文件关联、完整文件渐进渲染、搜索和正式发布包仍在后续计划中。

## License

本项目采用 [MIT License](LICENSE)。
