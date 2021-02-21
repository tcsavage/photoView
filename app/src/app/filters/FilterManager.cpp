#include <app/filters/FilterManager.hpp>

void FilterManager::showDialog(QWidget *parent) noexcept {
    if (dialog == nullptr) {
        dialog = new FilterDialog(parent);
        dialog->setFilter(filter_);
        connect(dialog, &FilterDialog::filterUpdated, this, &FilterManager::filterUpdated);
    }
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}
