#include "ImageView.hpp"

#include <QColor>
#include <QDebug>
#include <QImage>
#include <QVBoxLayout>

ImageView::ImageView(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent, f) {
        this->setMinimumSize(1,1);
        setScaledContents(false);
        setAlignment(Qt::AlignCenter);
        clear();
    }

void ImageView::clear() {
    auto pixmap = QPixmap();
    setPixmap(pixmap);
}

void ImageView::load(QSize size, const image::U8 *data) {
    std::size_t w = size.width();
    std::size_t h = size.height();
    // QImage ctor without bytesPerLine requires the image buffer, and each row
    // of same are 32-bit aligned. We can't assume that so we specify bytes per
    // line explicitly (width * numChannels * sizeof(T)).
    QImage img(data, w, h, w*3, QImage::Format::Format_RGB888);
    setPixmap(QPixmap::fromImage(std::move(img)));
}

void ImageView::setPixmap(const QPixmap &p){
    pixmap = p;
    QLabel::setPixmap(scaledPixmap());
}

int ImageView::heightForWidth(int width) const {
    return pixmap.isNull() ? this->height() : ((qreal)pixmap.height() * width) / pixmap.width();
}

QSize ImageView::sizeHint() const {
    int w = this->width();
    return QSize(w, heightForWidth(w));
}

QPixmap ImageView::scaledPixmap() const {
    auto scaled = pixmap.scaled(this->size() * devicePixelRatioF(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    scaled.setDevicePixelRatio(devicePixelRatioF());
    return scaled;
}

void ImageView::resizeEvent(QResizeEvent *) {
    if (!pixmap.isNull()) {
        QLabel::setPixmap(scaledPixmap());
    }
}
