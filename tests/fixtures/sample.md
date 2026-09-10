# FlashRead 流程图与图表测试

本文档用于测试 FlashRead 原生 Markdown 查看器对 **Mermaid 流程图与时序图** 的极速原生渲染支持。

## 1. 基础业务流程图 (Flowchart TD)

```mermaid
graph TD
    Start([开始阅读]) --> OpenFile[双击打开 Markdown]
    OpenFile --> Parse{是否包含图表?}
    Parse -->|是| RenderSVG[原生矢量 SVG 渲染]
    Parse -->|否| FastText[极速纯文本排版]
    RenderSVG --> Display[(呈现首屏内容)]
    FastText --> Display
    Display --> End([安静阅读])
```

## 2. 横向架构与多形状节点 (Flowchart LR)

```mermaid
flowchart LR
    subgraph Client [客户端]
        UI[QML 视图] --> Controller[[DocumentController]]
    end

    subgraph Core [原生核心]
        Controller --> Parser[FlowchartParser]
        Parser --> Layout{Sugiyama 布局}
        Layout --> Engine[(QSvgRenderer)]
    end

    subgraph Output [呈现层]
        Engine --> Display([矢量高保真渲染])
    end
```

## 3. 交互时序图 (Sequence Diagram)

```mermaid
sequenceDiagram
    autonumber
    actor User as 用户
    participant App as FlashRead
    participant Engine as DiagramEngine
    
    User->>App: 打开 markdown 文档
    App->>Engine: 解析 ```mermaid 代码块
    Engine-->>App: 返回原生 SVG 矢量图
    App-->>User: 极速渲染并显示图表
    Note over User,App: 主题切换时自动更新配色
```

## 4. 常规文本与代码块混合

```cpp
constexpr auto goal = "show content first, fast and quiet";
```

- 测试列表项目 1
- 测试列表项目 2
- **加粗文本** 与 *斜体文本*
