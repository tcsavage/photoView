#pragma once

#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QObject>

#include <image/CoreTypes.hpp>

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

class LinearGradientControl : public QObject {
    Q_OBJECT
public:
    explicit LinearGradientControl(QGraphicsScene *scene,
                                   QPointF start,
                                   QPointF end,
                                   QObject *parent = nullptr) noexcept;

    virtual ~LinearGradientControl();

    inline const QPointF &start() const noexcept { return startPos; }
    inline const QPointF &end() const noexcept { return endPos; }

signals:
    void changed();

protected:
    QPointF projectToScene(const QPointF &point) const noexcept;
    QPointF projectFromScene(const QPointF &point) const noexcept;
    void reSyncPositions() noexcept;

private:
    QPointF startPos;
    QPointF endPos;
    QGraphicsScene *scene;
    Handle startHandle;
    Handle endHandle;
};

class CanvasScene : public QGraphicsScene {
    Q_OBJECT
public:
    CanvasScene(QObject *parent = nullptr) noexcept;

    virtual ~CanvasScene() {}

    void setImage(QSize size, const image::U8 *data) noexcept;
    void clearImage() noexcept;
    const QGraphicsPixmapItem *imageItem() const noexcept { return imageItem_; }

private:
    QGraphicsPixmapItem *imageItem_ { nullptr };
};

class CanvasView : public QGraphicsView {
    Q_OBJECT
public:
    CanvasView(QWidget *parent = nullptr) noexcept;

    virtual ~CanvasView() {}

    inline void setScene(CanvasScene *scene) noexcept {
        scene_ = scene;
        QGraphicsView::setScene(scene_);
    };

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    CanvasScene *scene_ { nullptr };
    bool scaleToFit_ { true };
};
