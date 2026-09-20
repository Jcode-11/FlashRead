#include "diagram/DiagramRenderer.h"
#include "diagram/DiagramTypes.h"
#include "diagram/FlowchartLayout.h"
#include "diagram/FlowchartParser.h"
#include "diagram/SequenceDiagramParser.h"
#include "MarkdownRenderer.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QImage>
#include <iostream>
#include <cassert>

using namespace Diagram;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    std::cout << "=== Running FlashRead Diagram Unit & Visual Tests ===" << std::endl;

    // 1. Test Flowchart TD Parsing & Generation
    {
        std::cout << "[Test 1] Testing Flowchart TD parsing & layout..." << std::endl;
        QString source = R"(
graph TD
    Start([开始阅读]) --> OpenFile[双击打开 Markdown]
    OpenFile --> Parse{是否包含图表?}
    Parse -->|是| RenderSVG[原生矢量 SVG 渲染]
    Parse -->|否| FastText[极速纯文本排版]
    RenderSVG --> Display[(呈现首屏内容)]
    FastText --> Display
    Display --> End([安静阅读])
)";
        FlowchartGraph graph;
        QString error;
        bool ok = FlowchartParser::parse(source, graph, &error);
        if (!ok) {
            std::cerr << "FAIL: FlowchartParser failed: " << error.toStdString() << std::endl;
            return 1;
        }

        std::cout << "  Nodes parsed: " << graph.nodes.size() << " (Expected >= 7)" << std::endl;
        std::cout << "  Edges parsed: " << graph.edges.size() << " (Expected >= 7)" << std::endl;
        assert(graph.nodes.size() >= 7);
        assert(graph.edges.size() >= 7);

        // Verify Theme SVG Rendering
        QStringList themes = {"github-light", "github-dark", "dracula", "nord", "one-dark", "vitepress", "vscode-dark", "paper"};
        for (const QString &themeId : themes) {
            qreal w = 0, h = 0;
            QString svg = DiagramRenderer::instance().renderSvg(source, themeId, &w, &h);
            assert(!svg.isEmpty());
            assert(svg.startsWith("<svg"));
            assert(w > 100 && h > 100);
            std::cout << "  Theme [" << themeId.toStdString() << "] SVG generated: " << w << "x" << h << std::endl;

            // Render to QImage
            QString diagramId = DiagramRenderer::instance().registerDiagram(source);
            QImage img = DiagramRenderer::instance().renderImage(diagramId, themeId, 2.0);
            assert(!img.isNull());
            assert(img.width() > 0 && img.height() > 0);

            // Save test output
            QDir().mkpath("build/test_images");
            QString imgPath = QString("build/test_images/flowchart_td_%1.png").arg(themeId);
            img.save(imgPath);
            std::cout << "  Saved test image to " << imgPath.toStdString() << " (" << img.width() << "x" << img.height() << ")" << std::endl;
        }
        std::cout << "  -> Flowchart TD: PASS" << std::endl;
    }

    // 2. Test Flowchart LR with Subgraphs & Shapes
    {
        std::cout << "\n[Test 2] Testing Flowchart LR with subgraphs & shapes..." << std::endl;
        QString source = R"(
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
)";
        FlowchartGraph graph;
        QString error;
        bool ok = FlowchartParser::parse(source, graph, &error);
        if (!ok) {
            std::cerr << "FAIL: Flowchart LR failed: " << error.toStdString() << std::endl;
            return 1;
        }

        std::cout << "  Subgraphs parsed: " << graph.subgraphs.size() << " (Expected 3)" << std::endl;
        assert(graph.subgraphs.size() == 3);
        assert(graph.direction == Direction::LeftToRight);

        QString diagramId = DiagramRenderer::instance().registerDiagram(source);
        QImage img = DiagramRenderer::instance().renderImage(diagramId, "github-light", 2.0);
        assert(!img.isNull());
        img.save("build/test_images/flowchart_lr_github-light.png");
        std::cout << "  Saved test image to build/test_images/flowchart_lr_github-light.png (" << img.width() << "x" << img.height() << ")" << std::endl;
        std::cout << "  -> Flowchart LR: PASS" << std::endl;
    }

    // 3. Test Sequence Diagram
    {
        std::cout << "\n[Test 3] Testing Sequence Diagram..." << std::endl;
        QString source = R"(
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
)";
        SequenceGraph graph;
        QString error;
        bool ok = SequenceDiagramParser::parse(source, graph, &error);
        if (!ok) {
            std::cerr << "FAIL: SequenceDiagram failed: " << error.toStdString() << std::endl;
            return 1;
        }

        std::cout << "  Participants: " << graph.participants.size() << " (Expected 3): ";
        for (const auto &p : graph.participants) std::cout << p.id.toStdString() << " (" << p.label.toStdString() << "), ";
        std::cout << std::endl;
        std::cout << "  Messages: " << graph.messages.size() << " (Expected 4)" << std::endl;
        std::cout << "  Notes: " << graph.notes.size() << " (Expected 1)" << std::endl;
        assert(graph.participants.size() == 3);
        assert(graph.messages.size() == 4);
        assert(graph.notes.size() == 1);

        QString diagramId = DiagramRenderer::instance().registerDiagram(source);
        QImage img = DiagramRenderer::instance().renderImage(diagramId, "github-dark", 2.0);
        assert(!img.isNull());
        img.save("build/test_images/sequence_github-dark.png");
        std::cout << "  Saved test image to build/test_images/sequence_github-dark.png (" << img.width() << "x" << img.height() << ")" << std::endl;
        std::cout << "  -> Sequence Diagram: PASS" << std::endl;
    }

    // 4. Test MarkdownRenderer full pipeline
    {
        std::cout << "\n[Test 4] Testing MarkdownRenderer integration..." << std::endl;
        QByteArray mdContent = R"(
# Title
Some regular paragraph before diagram.

```mermaid
graph TD
    A[Node A] --> B[Node B]
```

Some paragraph after diagram.
)";
        QString html = MarkdownRenderer::render(mdContent);
        std::cout << "  Rendered HTML snippet:\n" << html.toStdString() << std::endl;
        assert(html.contains("<img src=\"image://diagram/"));
        assert(html.contains("Some regular paragraph before diagram."));
        assert(html.contains("Some paragraph after diagram."));
        std::cout << "  -> MarkdownRenderer Integration: PASS" << std::endl;
    }

    // 5. Test Multi-Language Syntax Highlighting
    {
        std::cout << "\n[Test 5] Testing Multi-Language Syntax Highlighting..." << std::endl;
        QByteArray mdWithCode = R"(
# Code Highlighting Test

```cpp
#include <iostream>
#include <vector>

// Main entry point
int main() {
    std::vector<int> nums = {1, 2, 3};
    std::cout << "Hello FlashRead!" << std::endl;
    return 0;
}
```

```python
# Python calculation
def calculate(name: str, count: int = 10) -> bool:
    print(f"Processing {name}")
    return True
```

```json
{
  "name": "FlashRead",
  "version": "1.0.0",
  "native": true,
  "count": 42
}
```

```sql
SELECT id, username, email FROM users WHERE active = 1 ORDER BY created_at DESC;
```
)";
        QString html = MarkdownRenderer::render(mdWithCode);
        std::cout << "  Rendered C++ / Python / JSON / SQL HTML length: " << html.length() << " chars" << std::endl;
        
        // Assert modern card container, header tag, copy button, and line numbers
        assert(html.contains("class=\"code-box\""));
        assert(html.contains("class=\"code-header\""));
        assert(html.contains("class=\"code-lang-tag\">cpp ⌄</span>"));
        assert(html.contains("class=\"code-lang-tag\">python ⌄</span>"));
        assert(html.contains("class=\"code-lang-tag\">json ⌄</span>"));
        assert(html.contains("class=\"code-lang-tag\">sql ⌄</span>"));
        assert(html.contains("class=\"code-copy-btn\">"));
        assert(html.contains("class=\"code-line-nums\""));
        assert(html.contains("class=\"line-num-pre\">1"));

        // Assert token classes
        assert(html.contains("class=\"hl-p\">#include</span>"));
        assert(html.contains("class=\"hl-t\">int</span>"));
        assert(html.contains("class=\"hl-s\">&quot;Hello FlashRead!&quot;</span>"));
        assert(html.contains("class=\"hl-c\">// Main entry point</span>"));
        assert(html.contains("class=\"hl-k\">def</span>"));
        assert(html.contains("class=\"hl-k\">return</span>"));
        assert(html.contains("class=\"hl-const\">True</span>"));
        assert(html.contains("class=\"hl-key\">&quot;name&quot;</span>"));
        assert(html.contains("class=\"hl-k\">SELECT</span>"));
        assert(html.contains("class=\"hl-k\">FROM</span>"));

        std::cout << "  -> Multi-Language Syntax Highlighting: PASS" << std::endl;
    }

    std::cout << "\n==========================================" << std::endl;
    std::cout << "  ALL 5 TEST SUITES PASSED SUCCESSFULLY!  " << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}
