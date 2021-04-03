#include <app/masks/MaskManager.hpp>

#include <QDebug>

#include <app/canvas/LinearGradientControl.hpp>

using namespace image;

MaskManager::MaskManager(CanvasScene *scene, AbstractMaskGenerator *maskGen, QObject *parent) noexcept
  : QObject(parent)
  , maskGen_(maskGen)
  , scene_(scene)
  , maskProcessor_(std::make_shared<image::MaskProcessor>()) {}

void MaskManager::activateControls() noexcept {
    deactivateControls();

    if (!maskGen_) { return; }

    if (maskGen_->getMeta().id == "maskGenerators.linearGradient") {
        auto gen = static_cast<LinearGradientMaskSpec *>(maskGen_);
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

void MaskManager::setMask(image::AbstractMaskGenerator *maskGen) noexcept {
    qDebug() << "Setting active mask:" << maskGen;
    maskGen_ = maskGen;
    maskBuf_ = nullptr;
    if (!maskGen_) {
        clearMask();
        return;
    }
    activateControls();
}

void MaskManager::clearMask() noexcept {
    deactivateControls();
    if (overlay_) { overlay_->clearMask(); }
}

void MaskManager::setOverlayEnabled(bool isEnabled) noexcept {
    if (isOverlayEnabled_ == isEnabled) { return; }
    isOverlayEnabled_ = isEnabled;

    if (isOverlayEnabled_ && !overlay_) { createOverlay(); }
    if (!isOverlayEnabled_ && overlay_) { overlay_->clearMask(); }

    if (!maskGen_) { return; }

    if (isOverlayEnabled_ && maskBuf_) { overlay_->setMask(*maskBuf_); }
}

void MaskManager::createOverlay() noexcept {
    assert(scene_);
    overlay_ = std::make_unique<MaskOverlayControl>(scene_, maskProcessor_, this);
}

void MaskManager::handleMaskGenerated(const image::AbstractMaskGenerator *maskGen, const image::Mask *maskBuf) noexcept {
    qDebug() << "Handling mask generated message. Current mask gen:" << maskGen_ << "Event gen:" << maskGen << "Buf:" << maskBuf;
    if (maskGen != maskGen_) { return; }
    maskBuf_ = maskBuf;
    if (isOverlayEnabled_) {
        if (maskGen_) {
            overlay_->setMask(*maskBuf_);
        } else {
            overlay_->clearMask();
        }
    }
}
