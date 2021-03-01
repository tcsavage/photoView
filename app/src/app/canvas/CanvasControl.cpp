#include <app/canvas/CanvasControl.hpp>

CanvasControl::CanvasControl(CanvasScene *scene, QObject *parent) noexcept : QObject(parent), scene_(scene) {}

CanvasControl::~CanvasControl() {}

CanvasScene *CanvasControl::scene() const noexcept { return scene_; }
