#pragma once

#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QRectF>

class HandleDisplay : public QGraphicsItem {
public:
    explicit HandleDisplay(QGraphicsItem *parent = nullptr) noexcept : QGraphicsItem(parent) {
        setFlag(QGraphicsItem::GraphicsItemFlag::ItemIgnoresTransformations);
        setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
    }

    virtual ~HandleDisplay() {}

    virtual QRectF boundingRect(void) const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    qreal size_ { 8 };
};

class Handle : public QGraphicsObject {
public:
    explicit Handle(QGraphicsItem *parent = nullptr) noexcept;
    explicit Handle(QPointF startPos, QGraphicsItem *parent = nullptr) noexcept;

    virtual ~Handle() {}

    virtual QRectF boundingRect() const override { return QRectF(); }

    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    HandleDisplay *display;
};
