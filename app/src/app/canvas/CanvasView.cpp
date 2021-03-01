#include <app/canvas/CanvasView.hpp>

CanvasView::CanvasView(QWidget *parent) noexcept : QGraphicsView(parent) {
    setMinimumSize(800, 600);
    setDragMode(QGraphicsView::DragMode::NoDrag);
    setRenderHint(QPainter::RenderHint::Antialiasing);
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
