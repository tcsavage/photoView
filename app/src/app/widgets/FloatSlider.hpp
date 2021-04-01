#pragma once

#include <QMouseEvent>

#include <app/widgets/Slider.hpp>

class FloatSlider : public Slider {
    Q_OBJECT
public:
    void setRange(float min, float max) noexcept;
    void setDefault(float value) noexcept;
    void setValue(float value) noexcept;
    void setValue(float value, bool useAsDefault) noexcept;

    void setNumSubdivisions(int n);
    void reset();

    constexpr float minValue() const noexcept { return minValue_; }
    constexpr float maxValue() const noexcept { return maxValue_; }
    constexpr float value() const noexcept { return currentValue_; }
    constexpr float defaultValue() const noexcept { return defaultValue_; }

    FloatSlider(QWidget *parent = nullptr) noexcept;
    FloatSlider(Qt::Orientation orientation, QWidget *parent = nullptr) noexcept;

    virtual ~FloatSlider() noexcept {}

signals:
    void valueChanged(float value);

protected:
    constexpr int mapFloatToInt(float value) const noexcept {
        auto scaledRange = static_cast<float>(numSubdivisions_) / (maxValue_ - minValue_);
        return static_cast<int>(scaledRange * value);
    }
    
    constexpr float mapIntToFloat(int value) const noexcept {
        auto scaledRange = static_cast<float>(numSubdivisions_) / (maxValue_ - minValue_);
        return static_cast<float>(value) / scaledRange;
    }

private:
    void sync();

    float minValue_ { 0.0f };
    float maxValue_ { 1.0f };
    float defaultValue_ { 0.0f };
    float currentValue_ { 0.0f };
    int numSubdivisions_ { 128 };
};
