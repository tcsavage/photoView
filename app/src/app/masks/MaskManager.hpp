#pragma once

#include <memory>

#include <QObject>

#include <image/Mask.hpp>

#include <app/canvas/LinearGradientControl.hpp>

class MaskManager : public QObject {
    Q_OBJECT
public:
    explicit MaskManager(image::GeneratedMask *mask, QObject *parent = nullptr) noexcept;

    ~MaskManager() { deactivateControl(); }

    void activateControl(CanvasScene *scene) noexcept;
    void deactivateControl() noexcept;

    inline image::GeneratedMask *mask() noexcept { return mask_; }

signals:
    void maskUpdated();
    
private:
    image::GeneratedMask *mask_;
    std::unique_ptr<LinearGradientControl> ctrl_;
};
