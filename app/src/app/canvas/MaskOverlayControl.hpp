#pragma once

#include <QGraphicsPixmapItem>

#include <image/Mask.hpp>
#include <image/MaskProcessor.hpp>

#include <app/canvas/CanvasControl.hpp>
#include <app/canvas/CanvasScene.hpp>

class MaskOverlayControl : public CanvasControl {
    Q_OBJECT
public:
    explicit MaskOverlayControl(CanvasScene *scene,
                                std::shared_ptr<image::MaskProcessor> maskProcessor,
                                QObject *parent = nullptr) noexcept;

    virtual ~MaskOverlayControl();

    void clearMask() noexcept;
    void setMask(const image::Mask &data) noexcept;

private:
    QGraphicsPixmapItem *maskOverlayItem_ { nullptr };
    std::shared_ptr<image::MaskProcessor> maskProcessor_;
    image::ImageBuf<image::U8, image::RGBA> overlayImageBuf_;
};
