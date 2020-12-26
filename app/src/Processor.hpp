#pragma once

#include <QObject>

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/NDArray.hpp>
#include <image/luts/CubeFile.hpp>
#include <image/luts/FastInterpolator.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

struct LutLoadFailure {
    image::Path path;
    image::String reason;
};

struct ImageLoadFailure {
    image::Path path;
    image::String reason;
};

struct Processor : public QObject {
    Q_OBJECT
public:
    image::luts::FastInterpolator<image::luts::TetrahedralInterpolator, image::U8, image::U8> interp;
    image::luts::LUT lut;
    image::NDArray<image::ColorRGB<image::U8>> originalImage;
    image::NDArray<image::ColorRGB<image::U8>> image;
    int imageWidth;
    int imageHeight;
    bool lutLoaded { false };

    image::Expected<void, LutLoadFailure> loadLutFromFile(image::Path path);
    image::Expected<void, ImageLoadFailure> loadImageFromFile(image::Path path);

    void update();

    Processor(QObject *parent = nullptr);

signals:
    void imageChanged();
};
