#pragma once

#include <QObject>
#include <QPointF>

#include <app/canvas/CanvasItems.hpp>
#include <app/canvas/CanvasScene.hpp>

class LinearGradientControl : public QObject {
    Q_OBJECT
public:
    explicit LinearGradientControl(CanvasScene *scene,
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
    CanvasScene *scene;
    Handle startHandle;
    Handle endHandle;
};
