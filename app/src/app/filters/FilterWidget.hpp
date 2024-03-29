#pragma once

#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QWidget>

#include <image/Registry.hpp>

#include <app/filters/Filters.hpp>
#include <app/widgets/FileChooser.hpp>
#include <app/widgets/FloatSlider.hpp>

class FilterWidget : public QWidget {
    Q_OBJECT
public:
    explicit FilterWidget(QWidget *parent = nullptr) noexcept
      : QWidget(parent) {}

    explicit FilterWidget(image::AbstractFilterSpec *, QWidget *parent = nullptr) noexcept
      : QWidget(parent) {}

    virtual ~FilterWidget() {}

    virtual image::AbstractFilterSpec *filter() noexcept = 0;
    virtual void setFilter(image::AbstractFilterSpec *filter) noexcept = 0;

signals:
    void filterUpdated();
};

class SimpleSliderFilterWidget : public FilterWidget {
    Q_OBJECT
public:
    explicit SimpleSliderFilterWidget(QWidget *parent = nullptr) noexcept;

    void setValue(image::F32 value) noexcept;

    virtual QString title() const noexcept = 0;

    virtual image::F32 defaultValue() const noexcept { return 0.0; }
    virtual image::F32 minValue() const noexcept { return -1.0; }
    virtual image::F32 maxValue() const noexcept { return 1.0; }
    virtual int numSubdivisions() const noexcept { return 128; }
    virtual int tickInterval() const noexcept { return 16; }

    virtual QString formatValueLabel(image::F32 value) const noexcept;

signals:
    void valueChanged(image::F32 value);

protected:
    void setup() noexcept;

    FloatSlider *slider;
    QLabel *widgetLabel;
    QLabel *valueLabel;
};

class ExposureFilterWidget : public SimpleSliderFilterWidget {
    Q_OBJECT
public:
    explicit ExposureFilterWidget(QWidget *parent = nullptr) noexcept;

    virtual ~ExposureFilterWidget() {}

    virtual QString title() const noexcept override { return tr("Exposure"); }

    virtual image::F32 minValue() const noexcept override { return -4.0; }
    virtual image::F32 maxValue() const noexcept override { return 4.0; }

    virtual QString formatValueLabel(image::F32 value) const noexcept override;

    virtual image::AbstractFilterSpec *filter() noexcept override { return filter_; }
    virtual void setFilter(image::AbstractFilterSpec *filter) noexcept override;

private:
    image::ExposureFilterSpec *filter_;
};

class LutFilterWidget : public FilterWidget {
    Q_OBJECT
public:
    explicit LutFilterWidget(QWidget *parent = nullptr) noexcept;

    virtual ~LutFilterWidget() {}

    virtual image::AbstractFilterSpec *filter() noexcept override { return filter_; }
    virtual void setFilter(image::AbstractFilterSpec *filter) noexcept override;

private:
    image::LutFilterSpec *filter_;

    FloatSlider *slider { nullptr };
    FileChooser *fileChooser { nullptr };
};

class SaturationFilterWidget : public SimpleSliderFilterWidget {
    Q_OBJECT
public:
    explicit SaturationFilterWidget(QWidget *parent = nullptr) noexcept;

    virtual ~SaturationFilterWidget() {}

    virtual QString title() const noexcept override { return tr("Saturation"); }

    virtual image::F32 defaultValue() const noexcept override { return 1.0; }
    virtual image::F32 minValue() const noexcept override { return 0.0; }
    virtual image::F32 maxValue() const noexcept override { return 2.0; }

    virtual image::AbstractFilterSpec *filter() noexcept override { return filter_; }
    virtual void setFilter(image::AbstractFilterSpec *filter) noexcept override;

private:
    image::SaturationFilterSpec *filter_;
};

class ContrastFilterWidget : public SimpleSliderFilterWidget {
    Q_OBJECT
public:
    explicit ContrastFilterWidget(QWidget *parent = nullptr) noexcept;

    virtual ~ContrastFilterWidget() {}

    virtual QString title() const noexcept override { return tr("Contrast"); }

    virtual image::F32 defaultValue() const noexcept override { return 1.0; }
    virtual image::F32 minValue() const noexcept override { return 0.8; }
    virtual image::F32 maxValue() const noexcept override { return 1.2; }

    virtual image::AbstractFilterSpec *filter() noexcept override { return filter_; }
    virtual void setFilter(image::AbstractFilterSpec *filter) noexcept override;

private:
    image::ContrastFilterSpec *filter_;
};

class ChannelMixerFilterWidget : public FilterWidget {
    Q_OBJECT
public:
    explicit ChannelMixerFilterWidget(QWidget *parent = nullptr) noexcept;

    virtual ~ChannelMixerFilterWidget() {}

    virtual image::AbstractFilterSpec *filter() noexcept override { return filter_; }
    virtual void setFilter(image::AbstractFilterSpec *filter) noexcept override;

protected:
    void handleValueChanged(int row, int column, float value) noexcept;

private:
    image::ChannelMixerFilterSpec *filter_;

    FloatSlider *redOutRedIn { nullptr };
    FloatSlider *redOutGreenIn { nullptr };
    FloatSlider *redOutBlueIn { nullptr };

    FloatSlider *greenOutRedIn { nullptr };
    FloatSlider *greenOutGreenIn { nullptr };
    FloatSlider *greenOutBlueIn { nullptr };

    FloatSlider *blueOutRedIn { nullptr };
    FloatSlider *blueOutGreenIn { nullptr };
    FloatSlider *blueOutBlueIn { nullptr };

    QCheckBox *preserveLuminosity { nullptr };
};

using FilterWidgetRegistry = image::Registry<FilterWidget, image::FilterMeta>;

namespace {
    template <class Filter, class Widget>
    void registerFilterWidget(FilterWidgetRegistry &reg) noexcept {
        image::FilterMeta meta = Filter::meta;
        reg.registerType<Widget>(Filter::meta.id, std::move(meta));
    }
}

inline FilterWidgetRegistry makeFilterWidgetRegistry() noexcept {
    FilterWidgetRegistry reg;

    registerFilterWidget<image::ExposureFilterSpec, ExposureFilterWidget>(reg);
    registerFilterWidget<image::LutFilterSpec, LutFilterWidget>(reg);
    registerFilterWidget<image::SaturationFilterSpec, SaturationFilterWidget>(reg);
    registerFilterWidget<image::ContrastFilterSpec, ContrastFilterWidget>(reg);
    registerFilterWidget<image::ChannelMixerFilterSpec, ChannelMixerFilterWidget>(reg);

    return reg;
}

inline FilterWidgetRegistry filterWidgetRegistry { makeFilterWidgetRegistry() };
