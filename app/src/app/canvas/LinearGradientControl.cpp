#include <app/canvas/LinearGradientControl.hpp>

LinearGradientControl::LinearGradientControl(CanvasScene *scene,
                                             QPointF start,
                                             QPointF end,
                                             QObject *parent) noexcept
  : CanvasControl(scene, parent)
  , startPos(start)
  , endPos(end)
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
    scene()->removeItem(&startHandle);
    scene()->removeItem(&endHandle);
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
    return point * scene()->sceneRect().size();
}
QPointF LinearGradientControl::projectFromScene(const QPointF &point) const noexcept {
    return point / scene()->sceneRect().size();
}

void LinearGradientControl::reSyncPositions() noexcept {
    startHandle.setPos(projectToScene(startPos));
    endHandle.setPos(projectToScene(endPos));
}
