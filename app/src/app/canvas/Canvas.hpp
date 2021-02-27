#pragma once

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QObject>

#include <image/CoreTypes.hpp>

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
