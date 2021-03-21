#include <app/filters/FilterDialog.hpp>

#include <cassert>

using namespace image;

FilterDialog::FilterDialog(QWidget *parent, Qt::WindowFlags f) noexcept : QDialog(parent, f) {}

void FilterDialog::setFilter(AbstractFilterSpec *filter) noexcept {
    assert(filter);

    const auto &meta = filter->getMeta();
    setWindowTitle(QString::fromStdString(meta.name));

    if (filterWidget) { filterWidget = nullptr; }
    auto result = filterWidgetRegistry.create(filter->getMeta().id);
    if (result.hasValue()) {
        filterWidget = result->release();
        filterWidget->setFilter(filter);
        filterWidget->setParent(this);
        resize(filterWidget->sizeHint());
    }
    setMinimumSize(filterWidget->sizeHint());

    connect(filterWidget, &FilterWidget::filterUpdated, this, &FilterDialog::filterUpdated);
}
