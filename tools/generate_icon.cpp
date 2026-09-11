#include <QBuffer>
#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <iostream>
#include <vector>

const char *svgContent = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <linearGradient id="bgGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#182035"/>
      <stop offset="100%" stop-color="#0a0e18"/>
    </linearGradient>
    <linearGradient id="docGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#283556"/>
      <stop offset="100%" stop-color="#1a233a"/>
    </linearGradient>
    <linearGradient id="flashGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#ffe600"/>
      <stop offset="50%" stop-color="#ff9900"/>
      <stop offset="100%" stop-color="#ff5500"/>
    </linearGradient>
    <linearGradient id="cyanGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#58a6ff"/>
      <stop offset="100%" stop-color="#0969da"/>
    </linearGradient>
    <filter id="shadow" x="-10%" y="-10%" width="120%" height="120%">
      <feDropShadow dx="0" dy="12" stdDeviation="16" flood-color="#000000" flood-opacity="0.5"/>
    </filter>
  </defs>

  <!-- Background App Squircle Tile -->
  <rect x="28" y="28" width="456" height="456" rx="108" fill="url(#bgGrad)" stroke="#38476e" stroke-width="6" filter="url(#shadow)"/>
  
  <!-- Subtle Internal Glow Ring -->
  <rect x="34" y="34" width="444" height="444" rx="102" fill="none" stroke="#58a6ff" stroke-width="2" stroke-opacity="0.3"/>

  <!-- Document Silhouette -->
  <g filter="url(#shadow)">
    <!-- Base Page -->
    <path d="M 130 110 L 305 110 L 382 187 L 382 402 L 130 402 Z" fill="url(#docGrad)" stroke="#4a5d8a" stroke-width="4" />
    <!-- Page Corner Fold -->
    <path d="M 305 110 L 305 187 L 382 187 Z" fill="#3a4a70" stroke="#4a5d8a" stroke-width="2" />

    <!-- Markdown Text Lines -->
    <rect x="170" y="210" width="160" height="14" rx="7" fill="#7890ba" opacity="0.85" />
    <rect x="170" y="246" width="120" height="14" rx="7" fill="#7890ba" opacity="0.85" />
    <rect x="170" y="282" width="145" height="14" rx="7" fill="#7890ba" opacity="0.85" />
    <rect x="170" y="318" width="90" height="14" rx="7" fill="#7890ba" opacity="0.85" />
    <rect x="170" y="354" width="135" height="14" rx="7" fill="#7890ba" opacity="0.85" />
  </g>

  <!-- Fast Flash Lightning Bolt -->
  <g filter="url(#shadow)">
    <path d="M 310 80 L 175 265 L 265 265 L 210 432 L 355 230 L 265 230 Z" 
          fill="url(#flashGrad)" stroke="#ffffff" stroke-width="5" stroke-linejoin="round" />
  </g>
</svg>)SVG";

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QByteArray svgData(svgContent);
    QSvgRenderer renderer(svgData);
    if (!renderer.isValid()) {
        std::cerr << "Invalid SVG!" << std::endl;
        return 1;
    }

    std::vector<int> sizes = {256, 128, 64, 48, 32, 16};
    std::vector<QByteArray> pngBuffers;

    for (int s : sizes) {
        QImage img(s, s, QImage::Format_ARGB32_Premultiplied);
        img.fill(Qt::transparent);

        QPainter painter(&img);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        renderer.render(&painter);
        painter.end();

        QByteArray bytes;
        {
            QBuffer buf(&bytes);
            buf.open(QIODevice::WriteOnly);
            img.save(&buf, "PNG");
        }
        pngBuffers.push_back(bytes);

        if (s == 256) {
            img.save("resources/FlashRead.png");
            std::cout << "Saved resources/FlashRead.png (256x256)" << std::endl;
        }
    }

    // Write Windows .ICO file format
    QFile icoFile("resources/FlashRead.ico");
    if (!icoFile.open(QIODevice::WriteOnly)) {
        std::cerr << "Failed to open resources/FlashRead.ico" << std::endl;
        return 1;
    }

    // 1. Header (6 bytes)
    QDataStream out(&icoFile);
    out.setByteOrder(QDataStream::LittleEndian);
    out << static_cast<quint16>(0); // Reserved
    out << static_cast<quint16>(1); // Type 1 = ICO
    out << static_cast<quint16>(sizes.size()); // Count

    // Calculate initial offset for image data: 6 + 16 * count
    quint32 offset = 6 + 16 * sizes.size();

    // 2. Directory Entries (16 bytes each)
    for (size_t i = 0; i < sizes.size(); ++i) {
        int s = sizes[i];
        quint8 width = (s >= 256) ? 0 : static_cast<quint8>(s);
        quint8 height = (s >= 256) ? 0 : static_cast<quint8>(s);
        quint8 colorCount = 0;
        quint8 reserved = 0;
        quint16 planes = 1;
        quint16 bitCount = 32;
        quint32 bytesInRes = pngBuffers[i].size();

        out << width << height << colorCount << reserved;
        out << planes << bitCount;
        out << bytesInRes;
        out << offset;

        offset += bytesInRes;
    }

    // 3. Image Data
    for (const auto &bytes : pngBuffers) {
        icoFile.write(bytes);
    }
    icoFile.close();

    std::cout << "Successfully generated resources/FlashRead.ico with 6 resolutions (16 to 256px)!" << std::endl;
    return 0;
}
