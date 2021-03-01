#pragma once

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QSize>

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
