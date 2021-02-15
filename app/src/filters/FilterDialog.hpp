#pragma once

#include <QDialog>

#include <image/Filters.hpp>

#include "FilterWidget.hpp"

class FilterDialog : public QDialog {
    Q_OBJECT
public:
    explicit FilterDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) noexcept;

    virtual ~FilterDialog() {}

    void setFilter(image::AbstractFilterSpec *filter) noexcept;

signals:
    void filterUpdated();

private:
    FilterWidget *filterWidget { nullptr };
};
