#include <app/filters/FilterDialog.hpp>

#include <cassert>

using namespace image;

FilterDialog::FilterDialog(QWidget *parent, Qt::WindowFlags f) noexcept : QDialog(parent, f) {}

void FilterDialog::setFilter(AbstractFilterSpec *filter) noexcept {
    assert(filter);

    const auto &meta = filter->getMeta();
    setWindowTitle(QString::fromStdString(meta.name));

    if (filterWidget) { filterWidget->deleteLater(); }
    filterWidget = makeFilterWidget(filter, this);
    setMinimumSize(filterWidget->sizeHint());

    connect(filterWidget, &FilterWidget::filterUpdated, this, &FilterDialog::filterUpdated);
}
