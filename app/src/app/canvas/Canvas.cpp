#include <app/canvas/Canvas.hpp>

#include <cmath>

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QLinearGradient>
#include <QPointF>
#include <QResizeEvent>

QRectF HandleDisplay::boundingRect(void) const { return QRectF { -size_, -size_, size_ * 2, size_ * 2 }; }

void HandleDisplay::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QPen pen(QColor::fromRgbF(1.0, 1.0, 1.0, 0.5), 2);
    QBrush brush(QColor::fromRgbF(0.5, 0.5, 0.5, 0.3));
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(QPointF(), size_, size_);
}

void HandleDisplay::mousePressEvent(QGraphicsSceneMouseEvent *) {
    // Pass mouse control directly to parent.
    parentItem()->grabMouse();
}

Handle::Handle(QGraphicsItem *parent) noexcept : QGraphicsObject(parent), display(new HandleDisplay(this)) {}

Handle::Handle(QPointF startPos, QGraphicsItem *parent) noexcept : Handle(parent) { setPos(startPos); }

void Handle::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) {}

void Handle::mousePressEvent(QGraphicsSceneMouseEvent *) { grabMouse(); }
void Handle::mouseReleaseEvent(QGraphicsSceneMouseEvent *) { ungrabMouse(); }

void Handle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) { setPos(event->scenePos()); }

LinearGradientControl::LinearGradientControl(QGraphicsScene *scene,
                                             QPointF start,
                                             QPointF end,
                                             QObject *parent) noexcept
  : QObject(parent)
  , startPos(start)
  , endPos(end)
  , scene(scene)
  , startHandle(projectToScene(start))
  , endHandle(projectToScene(end)) {
    scene->addItem(&startHandle);
    scene->addItem(&endHandle);
    startHandle.setZValue(50);
    endHandle.setZValue(50);

    connect(scene, &QGraphicsScene::sceneRectChanged, this, [this](const QRectF &) { reSyncPositions(); });

    connect(&startHandle, &QGraphicsObject::xChanged, this, [this] {
        startPos = projectFromScene(startHandle.pos());
        emit changed();
    });
    connect(&startHandle, &QGraphicsObject::yChanged, this, [this] {
        startPos = projectFromScene(startHandle.pos());
        emit changed();
    });

    connect(&endHandle, &QGraphicsObject::xChanged, this, [this] {
        endPos = projectFromScene(endHandle.pos());
        emit changed();
    });
    connect(&endHandle, &QGraphicsObject::yChanged, this, [this] {
        endPos = projectFromScene(endHandle.pos());
        emit changed();
    });
}

LinearGradientControl::~LinearGradientControl() {
    scene->removeItem(&startHandle);
    scene->removeItem(&endHandle);
}

namespace {
    QPointF operator*(const QPointF &lhs, const QSizeF &rhs) noexcept {
        return QPointF { lhs.x() * rhs.width(), lhs.y() * rhs.height() };
    }

    QPointF operator/(const QPointF &lhs, const QSizeF &rhs) noexcept {
        return QPointF { lhs.x() / rhs.width(), lhs.y() / rhs.height() };
    }
}

QPointF LinearGradientControl::projectToScene(const QPointF &point) const noexcept {
    return point * scene->sceneRect().size();
}
QPointF LinearGradientControl::projectFromScene(const QPointF &point) const noexcept {
    return point / scene->sceneRect().size();
}

void LinearGradientControl::reSyncPositions() noexcept {
    startHandle.setPos(projectToScene(startPos));
    endHandle.setPos(projectToScene(endPos));
}

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
