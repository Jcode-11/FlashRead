const fs = require('fs');
const path = require('path');

const baseDir = path.resolve(__dirname, '../src/web');
const cssContent = fs.readFileSync(path.join(baseDir, 'themes.css'), 'utf-8');
const prismContent = fs.readFileSync(path.join(baseDir, 'prism.min.js'), 'utf-8');
const jsContent = fs.readFileSync(path.join(baseDir, 'app.js'), 'utf-8');

const indexHtml = `<!DOCTYPE html>
<html lang="zh-CN" data-theme="github-light">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>FlashRead</title>
  <style>
${cssContent}
  </style>
</head>
<body>
  <div id="markdown-container"></div>
  <script>
${prismContent}
  </script>
  <script>
${jsContent}
  </script>
</body>
</html>
`;

fs.writeFileSync(path.join(baseDir, 'index.html'), indexHtml, 'utf-8');

const templateH = `#pragma once

namespace FlashRead::Web {
inline const char INDEX_HTML[] = R"HTML_TEMPLATE(
${indexHtml})HTML_TEMPLATE";
}
`;

fs.writeFileSync(path.join(baseDir, 'IndexHtmlTemplate.h'), templateH, 'utf-8');
console.log('Successfully bundled index.html and IndexHtmlTemplate.h');
