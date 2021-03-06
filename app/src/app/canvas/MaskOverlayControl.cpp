#include <app/canvas/MaskOverlayControl.hpp>

using namespace image;

MaskOverlayControl::MaskOverlayControl(CanvasScene *scene,
                                       std::shared_ptr<MaskProcessor> maskProcessor,
                                       QObject *parent) noexcept
  : CanvasControl(scene, parent)
  , maskProcessor_(maskProcessor) {}

MaskOverlayControl::~MaskOverlayControl() { clearMask(); }

void MaskOverlayControl::clearMask() noexcept {
    if (maskOverlayItem_) {
        scene()->removeItem(maskOverlayItem_);
        delete maskOverlayItem_;
        maskOverlayItem_ = nullptr;
    }
}

void MaskOverlayControl::setMask(const Mask &data) noexcept {
    memory::Size w = data.width();
    memory::Size h = data.height();
    if (overlayImageBuf_.shape() != Shape { w, h, 4 }) { overlayImageBuf_ = maskProcessor_->makeOverlayImageBuf(data); }
    maskProcessor_->generateOverlayImage(data, overlayImageBuf_);

    QImage img(overlayImageBuf_.data(), w, h, w * 4, QImage::Format::Format_RGBA8888);
    if (!maskOverlayItem_) {
        maskOverlayItem_ = scene()->addPixmap(QPixmap::fromImage(std::move(img)));
        maskOverlayItem_->setZValue(5);
    } else {
        maskOverlayItem_->setPixmap(QPixmap::fromImage(std::move(img)));
    }
}
