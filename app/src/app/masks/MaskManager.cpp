#include <app/masks/MaskManager.hpp>

#include <QDebug>

#include <app/canvas/LinearGradientControl.hpp>

using namespace image;

MaskManager::MaskManager(GeneratedMask *mask, QObject *parent) noexcept : QObject(parent), mask_(mask) {}

void MaskManager::activateControl(CanvasScene *scene) noexcept {
    if (!mask_->generator()) { return; }

    if (mask_->generator()->getMeta().id == "maskGenerators.linearGradient") {
        auto gen = static_cast<LinearGradientMaskSpec *>(mask_->generator());
        auto ctrl = std::make_unique<LinearGradientControl>(
            scene, QPointF(gen->from.x, gen->from.y), QPointF(gen->to.x, gen->to.y), this);
        connect(ctrl.get(), &CanvasControl::changed, this, [this, gen, ctrlPtr = ctrl.get()] {
            gen->from = glm::vec2 { ctrlPtr->start().x(), ctrlPtr->start().y() };
            gen->to = glm::vec2 { ctrlPtr->end().x(), ctrlPtr->end().y() };
            emit maskUpdated();
        });
        ctrl_ = std::move(ctrl);
    }
}

void MaskManager::deactivateControl() noexcept { ctrl_ = nullptr; }
