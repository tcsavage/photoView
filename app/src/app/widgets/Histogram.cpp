#include <app/widgets/Histogram.hpp>

#include <QPainter>
#include <QPainterPath>

Histogram::Histogram(QWidget *parent) noexcept : QWidget(parent) {
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    histogram_.generateTest();
}

void Histogram::generate(const image::ImageBuf<image::U8, image::RGB> &img) noexcept {
    histogram_.generate(img);
    update();
}

QSize Histogram::sizeHint() const { return QSize(256, 150); }
QSize Histogram::minimumSizeHint() const { return QSize(256, 150); }

void Histogram::paintEvent(QPaintEvent *) {
    qreal bgBrightness = 0.15;
    QColor red = QColor::fromRgbF(1, 0, 0);
    QColor green = QColor::fromRgbF(0, 1, 0);
    QColor blue = QColor::fromRgbF(0, 0, 1);
    qreal fillOpacity = 0.3;
    qreal penWidth = 1.5;

    QPainter painter(this);
    painter.fillRect(rect(), QColor::fromRgbF(bgBrightness, bgBrightness, bgBrightness));
    painter.setCompositionMode(QPainter::CompositionMode_Plus);  // Make overlapping areas add their color
    painter.setRenderHint(QPainter::Antialiasing);
    auto widgetHeight = height();
    auto widgetWidth = width();

    // Red
    QPen redPen(red, penWidth);
    QColor redBrushColor = red;
    redBrushColor.setAlphaF(fillOpacity);
    QBrush redBrush(redBrushColor);
    QPainterPath redPath(rect().bottomLeft());
    for (image::memory::Size i = 0; i < histogram_.numBuckets(); i++) {
        auto barHeight = (1 - histogram_.red[i]) * widgetHeight;
        redPath.lineTo(i * widgetWidth / 256, barHeight);
    }
    redPath.lineTo(widgetWidth, widgetHeight);
    painter.fillPath(redPath, redBrush);
    painter.setPen(redPen);
    painter.drawPath(redPath);

    // Green
    QPen greenPen(green, penWidth);
    QColor greenBrushColor = green;
    greenBrushColor.setAlphaF(fillOpacity);
    QBrush greenBrush(greenBrushColor);
    QPainterPath greenPath(rect().bottomLeft());
    for (image::memory::Size i = 0; i < histogram_.numBuckets(); i++) {
        auto barHeight = (1 - histogram_.green[i]) * widgetHeight;
        greenPath.lineTo(i * widgetWidth / 256, barHeight);
    }
    greenPath.lineTo(widgetWidth, widgetHeight);
    painter.fillPath(greenPath, greenBrush);
    painter.setPen(greenPen);
    painter.drawPath(greenPath);

    // Blue
    QPen bluePen(blue, penWidth);
    QColor blueBrushColor = blue;
    blueBrushColor.setAlphaF(fillOpacity);
    QBrush blueBrush(blueBrushColor);
    QPainterPath bluePath(rect().bottomLeft());
    for (image::memory::Size i = 0; i < histogram_.numBuckets(); i++) {
        auto barHeight = (1 - histogram_.blue[i]) * widgetHeight;
        bluePath.lineTo(i * widgetWidth / 256, barHeight);
    }
    bluePath.lineTo(widgetWidth, widgetHeight);
    painter.fillPath(bluePath, blueBrush);
    painter.setPen(bluePen);
    painter.drawPath(bluePath);
}
