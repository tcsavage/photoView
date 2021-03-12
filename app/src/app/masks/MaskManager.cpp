#include <app/masks/MaskManager.hpp>

#include <QDebug>

#include <app/canvas/LinearGradientControl.hpp>

using namespace image;

MaskManager::MaskManager(CanvasScene *scene, GeneratedMask *mask, QObject *parent) noexcept
  : QObject(parent)
  , mask_(mask)
  , scene_(scene)
  , maskProcessor_(std::make_shared<image::MaskProcessor>()) {}

void MaskManager::activateControls() noexcept {
    deactivateControls();

    if (!mask_->generator()) { return; }

    if (mask_->generator()->getMeta().id == "maskGenerators.linearGradient") {
        auto gen = static_cast<LinearGradientMaskSpec *>(mask_->generator());
        auto ctrl = std::make_unique<LinearGradientControl>(
            scene_, QPointF(gen->from.x, gen->from.y), QPointF(gen->to.x, gen->to.y), this);
        connect(ctrl.get(), &CanvasControl::changed, this, [this, gen, ctrlPtr = ctrl.get()] {
            gen->from = glm::vec2 { ctrlPtr->start().x(), ctrlPtr->start().y() };
            gen->to = glm::vec2 { ctrlPtr->end().x(), ctrlPtr->end().y() };
            emit maskUpdated();
        });
        ctrl_ = std::move(ctrl);
    }
}

void MaskManager::deactivateControls() noexcept { ctrl_ = nullptr; }

void MaskManager::setMask(image::GeneratedMask *mask) noexcept {
    mask_ = mask;
    if (!mask_) {
        clearMask();
        return;
    }
    activateControls();
    if (isOverlayEnabled_) {
        if (!overlay_) { createOverlay(); }
        overlay_->setMask(*mask_->mask());
    }
}

void MaskManager::clearMask() noexcept {
    deactivateControls();
    if (overlay_) { overlay_->clearMask(); }
}

void MaskManager::setOverlayEnabled(bool isEnabled) noexcept {
    if (isOverlayEnabled_ == isEnabled) { return; }
    isOverlayEnabled_ = isEnabled;

    if (!mask_) { return; }

    if (!isOverlayEnabled_ && overlay_) { overlay_->clearMask(); }

    if (isOverlayEnabled_ && !overlay_) { createOverlay(); }

    if (isOverlayEnabled_ && mask_) { overlay_->setMask(*mask_->mask()); }
}

void MaskManager::createOverlay() noexcept {
    assert(scene_);
    overlay_ = std::make_unique<MaskOverlayControl>(scene_, maskProcessor_, this);
    overlay_->setMask(*mask_->mask());
    connect(this, &MaskManager::maskUpdated, this, [this] {
        if (isOverlayEnabled_) {
            if (mask_) {
                overlay_->setMask(*mask_->mask());
            } else {
                overlay_->clearMask();
            }
        }
    });
}
