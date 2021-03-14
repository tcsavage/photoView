#pragma once

#include <QObject>

#include <image/Filters.hpp>

#include <app/filters/FilterDialog.hpp>

class FilterManager : public QObject {
    Q_OBJECT
public:
    explicit FilterManager(image::AbstractFilterSpec *filter, QObject *parent = nullptr)
      : QObject(parent)
      , filter_(filter) {}

    virtual ~FilterManager();

    inline image::AbstractFilterSpec *filter() noexcept { return filter_; }

    void showDialog(QWidget *parent) noexcept;

signals:
    void filterUpdated();

private:
    image::AbstractFilterSpec *filter_ { nullptr };
    FilterDialog *dialog { nullptr };
};
