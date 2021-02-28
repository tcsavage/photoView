#include <app/masks/MaskManager.hpp>

#include <QDebug>

using namespace image;

MaskManager::MaskManager(GeneratedMask *mask, QObject *parent) noexcept : QObject(parent), mask_(mask) {}

void MaskManager::activateControl(CanvasScene *scene) noexcept {
    if (!mask_->generator()) { return; }
    if (mask_->generator()->getMeta().id == "maskGenerators.linearGradient") {
        auto gen = static_cast<LinearGradientMaskSpec *>(mask_->generator());
        ctrl_ = std::make_unique<LinearGradientControl>(
            scene, QPointF(gen->from.x, gen->from.y), QPointF(gen->to.x, gen->to.y), this);
        connect(ctrl_.get(), &LinearGradientControl::changed, this, [this, gen] {
            gen->from = glm::vec2 { ctrl_->start().x(), ctrl_->start().y() };
            gen->to = glm::vec2 { ctrl_->end().x(), ctrl_->end().y() };
            emit maskUpdated();
        });
    }
}

void MaskManager::deactivateControl() noexcept { ctrl_ = nullptr; }
