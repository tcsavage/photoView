#pragma once

#include <QObject>

#include <app/canvas/CanvasScene.hpp>

class CanvasControl : public QObject {
    Q_OBJECT
public:
    explicit CanvasControl(CanvasScene *scene, QObject *parent = nullptr) noexcept;

    virtual ~CanvasControl();

    CanvasScene *scene() const noexcept;

signals:
    void changed();

private:
    CanvasScene *scene_ { nullptr };
};
