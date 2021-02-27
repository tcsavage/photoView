#include <app/canvas/Canvas.hpp>

#include <cmath>

#include <QDebug>
#include <QGraphicsTextItem>
#include <QResizeEvent>

CanvasScene::CanvasScene(QObject *parent) noexcept : QGraphicsScene(parent) {}

void CanvasScene::setImage(QSize size, const image::U8 *data) noexcept {
    clearImage();
    std::size_t w = size.width();
    std::size_t h = size.height();
    // QImage ctor without bytesPerLine requires the image buffer, and each row
    // of same are 32-bit aligned. We can't assume that so we specify bytes per
    // line explicitly (width * numChannels * sizeof(T)).
    QImage img(data, w, h, w * 3, QImage::Format::Format_RGB888);
    imageItem_ = addPixmap(QPixmap::fromImage(std::move(img)));
    setSceneRect(QRectF { QPointF { 0, 0 }, QSizeF { size } });
}

void CanvasScene::clearImage() noexcept {
    if (imageItem_) {
        removeItem(imageItem_);
        delete imageItem_;
    }
}

CanvasView::CanvasView(QWidget *parent) noexcept : QGraphicsView(parent) {
    setMinimumSize(800, 600);
    setDragMode(QGraphicsView::DragMode::NoDrag);
}

void CanvasView::resizeEvent(QResizeEvent *) {
    if (!scene_ || !scene_->imageItem()) { return; }
    if (scaleToFit_) { fitInView(scene_->imageItem()->boundingRect(), Qt::AspectRatioMode::KeepAspectRatio); }
}

void CanvasView::mouseDoubleClickEvent(QMouseEvent *event) {
    scaleToFit_ = !scaleToFit_;
    if (scaleToFit_) {
        fitInView(scene_->imageItem()->boundingRect(), Qt::AspectRatioMode::KeepAspectRatio);
        setDragMode(QGraphicsView::DragMode::NoDrag);
    } else {
        auto point = mapToScene(event->pos());
        resetTransform();
        centerOn(point);
        setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    }
}
