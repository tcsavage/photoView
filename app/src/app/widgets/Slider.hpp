#pragma once

#include <QMouseEvent>
#include <QSlider>

class Slider : public QSlider {
    Q_OBJECT
public:
    Slider(QWidget *parent = nullptr) noexcept;
    Slider(Qt::Orientation orientation, QWidget *parent = nullptr) noexcept;

    virtual ~Slider() noexcept {}

signals:
    void doubleClicked(QMouseEvent *event);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};
