#include "ImageView.hpp"

#include <QColor>
#include <QDebug>
#include <QImage>
#include <QVBoxLayout>

ImageView::ImageView()
    : label(new QLabel()) {
        auto layout = new QVBoxLayout();
        layout->addWidget(label);
        setLayout(layout);

        label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        label->setScaledContents(true);
        label->setMinimumSize(100, 100);

        clear();
    }

void ImageView::clear() {
    pixmap = QPixmap(QSize { 50, 50 });
    pixmap.fill(Qt::magenta);
    label->setPixmap(pixmap);
}

void ImageView::load(QSize size, const image::U8 *data) {
    qDebug() << "Loading image data into ImageView:" << size;
    std::size_t w = size.width();
    std::size_t h = size.height();
    QImage img(data, w, h, QImage::Format::Format_RGB888);
    pixmap = QPixmap::fromImage(std::move(img));
    label->setPixmap(pixmap);
}
