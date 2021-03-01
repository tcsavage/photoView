#pragma once

#include <QGraphicsView>
#include <QMouseEvent>
#include <QResizeEvent>

#include <app/canvas/CanvasScene.hpp>

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
