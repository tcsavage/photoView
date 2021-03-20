#pragma once

#include <QWidget>

#include <app/filters/Filters.hpp>

class FilterWidget : public QWidget {
    Q_OBJECT
public:
    explicit FilterWidget(image::AbstractFilterSpec *filter, QWidget *parent = nullptr) noexcept
      : QWidget(parent)
      , filter_(filter) {}

    virtual ~FilterWidget() {}

    inline image::AbstractFilterSpec *filter() noexcept { return filter_; }

signals:
    void filterUpdated();

private:
    image::AbstractFilterSpec *filter_ { nullptr };
};

class ExposureFilterWidget : public FilterWidget {
    Q_OBJECT
public:
    explicit ExposureFilterWidget(image::AbstractFilterSpec *filter, QWidget *parent = nullptr) noexcept;

    virtual ~ExposureFilterWidget() {}

private:
    image::ExposureFilterSpec *filter_;
    int rangeEvs { 5 };
    int nDivisions { 9 };
};

class LutFilterWidget : public FilterWidget {
    Q_OBJECT
public:
    explicit LutFilterWidget(image::AbstractFilterSpec *filter, QWidget *parent = nullptr) noexcept;

    virtual ~LutFilterWidget() {}

private:
    image::LutFilterSpec *filter_;
};

class SaturationFilterWidget : public FilterWidget {
    Q_OBJECT
public:
    explicit SaturationFilterWidget(image::AbstractFilterSpec *filter, QWidget *parent = nullptr) noexcept;

    virtual ~SaturationFilterWidget() {}

private:
    image::SaturationFilterSpec *filter_;
};

class ContrastFilterWidget : public FilterWidget {
    Q_OBJECT
public:
    explicit ContrastFilterWidget(image::AbstractFilterSpec *filter, QWidget *parent = nullptr) noexcept;

    virtual ~ContrastFilterWidget() {}

private:
    image::ContrastFilterSpec *filter_;
};

inline FilterWidget *makeFilterWidget(image::AbstractFilterSpec *filter, QWidget *parent = nullptr) noexcept {
    const auto &meta = filter->getMeta();
    if (meta.id == "filters.exposure") {
        return new ExposureFilterWidget(filter, parent);
    } else if (meta.id == "filters.lut") {
        return new LutFilterWidget(filter, parent);
    } else if (meta.id == "filters.saturation") {
        return new SaturationFilterWidget(filter, parent);
    } else if (meta.id == "filters.contrast") {
        return new ContrastFilterWidget(filter, parent);
    } else {
        std::terminate();
    }
}
