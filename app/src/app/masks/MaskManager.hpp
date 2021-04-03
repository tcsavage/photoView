#pragma once

#include <memory>

#include <QObject>

#include <image/Mask.hpp>
#include <image/MaskProcessor.hpp>

#include <app/canvas/CanvasControl.hpp>
#include <app/canvas/MaskOverlayControl.hpp>

class MaskManager : public QObject {
    Q_OBJECT
public:
    explicit MaskManager(CanvasScene *scene, image::AbstractMaskGenerator *maskGen = nullptr, QObject *parent = nullptr) noexcept;

    ~MaskManager() { deactivateControls(); }

    void activateControls() noexcept;
    void deactivateControls() noexcept;

    void setOverlayEnabled(bool isEnabled) noexcept;

    inline image::AbstractMaskGenerator *maskGen() noexcept { return maskGen_; }
    void setMask(image::AbstractMaskGenerator *maskGen) noexcept;
    void clearMask() noexcept;

    void createOverlay() noexcept;

    void handleMaskGenerated(const image::AbstractMaskGenerator *maskGen, const image::Mask *maskBuf) noexcept;

signals:
    void maskUpdated();

private:
    image::AbstractMaskGenerator *maskGen_ { nullptr };
    const image::Mask *maskBuf_ { nullptr };
    CanvasScene *scene_;
    std::unique_ptr<CanvasControl> ctrl_;
    std::unique_ptr<MaskOverlayControl> overlay_;
    std::shared_ptr<image::MaskProcessor> maskProcessor_;
    bool isOverlayEnabled_ { false };
};
