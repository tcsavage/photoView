#pragma once

#include <QWidget>

#include <image/Histogram.hpp>

class Histogram : public QWidget {
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = nullptr) noexcept;

    virtual ~Histogram() {}

    void generate(const image::ImageBuf<image::U8, image::RGB> &img) noexcept;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    image::Histogram<image::U8> histogram_;
};
