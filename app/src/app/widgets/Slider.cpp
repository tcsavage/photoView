#include <app/widgets/Slider.hpp>

Slider::Slider(QWidget *parent) noexcept : QSlider(parent) {}
Slider::Slider(Qt::Orientation orientation, QWidget *parent) noexcept : QSlider(orientation, parent) {}

void Slider::mouseDoubleClickEvent(QMouseEvent *event) {
    emit doubleClicked(event);
}
