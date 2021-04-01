#include <app/widgets/FloatSlider.hpp>

FloatSlider::FloatSlider(QWidget *parent) noexcept : FloatSlider(Qt::Orientation::Horizontal, parent) {}

FloatSlider::FloatSlider(Qt::Orientation orientation, QWidget *parent) noexcept : Slider(orientation, parent) {
    connect(this, &QSlider::valueChanged, this, [this](int value) {
        currentValue_ = mapIntToFloat(value);
        emit valueChanged(currentValue_);
    });

    connect(this, &Slider::doubleClicked, this, [this](QMouseEvent *) { reset(); });

    setTickInterval(16);
}

void FloatSlider::setRange(float min, float max) noexcept {
    minValue_ = min;
    maxValue_ = max;
    sync();
}

void FloatSlider::setDefault(float value) noexcept {
    defaultValue_ = value;
}

void FloatSlider::setValue(float value) noexcept {
    currentValue_ = value;
    QSlider::setValue(mapFloatToInt(currentValue_));
}

void FloatSlider::setValue(float value, bool useAsDefault) noexcept {
    setValue(value);
    if (useAsDefault) {
        setDefault(value);
    }
}

void FloatSlider::setNumSubdivisions(int n) {
    numSubdivisions_ = n;
    sync();
}

void FloatSlider::reset() {
    setValue(defaultValue_);
}

void FloatSlider::sync() {
    QSlider::setRange(mapFloatToInt(minValue_), mapFloatToInt(maxValue_));
    QSlider::setValue(mapFloatToInt(currentValue_));
}
