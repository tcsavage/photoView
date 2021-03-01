#pragma once

#include <QPointF>

#include <app/canvas/CanvasControl.hpp>
#include <app/canvas/CanvasItems.hpp>
#include <app/canvas/CanvasScene.hpp>

class LinearGradientControl : public CanvasControl {
    Q_OBJECT
public:
    explicit LinearGradientControl(CanvasScene *scene,
                                   QPointF start,
                                   QPointF end,
                                   QObject *parent = nullptr) noexcept;

    virtual ~LinearGradientControl();

    inline const QPointF &start() const noexcept { return startPos; }
    inline const QPointF &end() const noexcept { return endPos; }

protected:
    QPointF projectToScene(const QPointF &point) const noexcept;
    QPointF projectFromScene(const QPointF &point) const noexcept;
    void reSyncPositions() noexcept;

private:
    QPointF startPos;
    QPointF endPos;
    Handle startHandle;
    Handle endHandle;
};
