#pragma once

#include <span>

#include <QLabel>
#include <QPixmap>
#include <QWidget>

#include <image/CoreTypes.hpp>

class ImageView : public QWidget {
    Q_OBJECT
public:
    void clear();
    void load(QSize size, std::span<image::U8> data);

    ImageView();

private:
    QLabel *label;
    QPixmap pixmap;
};
