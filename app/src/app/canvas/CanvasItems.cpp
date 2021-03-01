#include <app/canvas/CanvasItems.hpp>

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

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
