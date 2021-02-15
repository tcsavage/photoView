#include "LookFilters.hpp"

#include <cassert>

#include <QAction>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>

#include <image/Filters.hpp>

#include "Filters.hpp"

QSize FilterItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const {
    return QSize { 100, 50 };
}

LookFilters::LookFilters(QWidget *parent) noexcept : QWidget(parent) {
    auto layout = new QVBoxLayout();
    setLayout(layout);

    // Layout for actions on top.
    auto topLayout = new QHBoxLayout();

    // Add filter actions and menu.
    addFilterAction = new QAction("Add filter");
    addFilterMenu = makeFilterMenu();

    // Set-up add filter button.
    auto addFilterBtn = new QToolButton();
    addFilterBtn->setPopupMode(QToolButton::InstantPopup);
    addFilterBtn->setDefaultAction(addFilterAction);
    addFilterBtn->setMenu(addFilterMenu);
    topLayout->addWidget(addFilterBtn);

    // Set-up enabled toggle.
    auto toggleLookEnabledAction = new QAction("Enabled");
    toggleLookEnabledAction->setCheckable(true);
    toggleLookEnabledAction->setChecked(true);
    auto toggleLookEnabledBtn = new QToolButton();
    toggleLookEnabledBtn->setDefaultAction(toggleLookEnabledAction);
    topLayout->addWidget(toggleLookEnabledBtn);

    // Add top-layout.
    layout->addLayout(topLayout);

    // Set-up filter list.
    delegate = new FilterItemDelegate();
    listView = new QListView();
    listView->setItemDelegate(delegate);
    layout->addWidget(listView);

    // Set-up connections.

    connect(addFilterBtn, &QToolButton::triggered, this, [this](QAction *action) {
        if (model_) {
            image::String id = action->data().toString().toStdString();
            auto filterResult = filterRegistry.create(id);
            // TODO: Could this result in an error?
            model_->addFilter(std::move(*filterResult));
        }
    });

    connect(listView, &QAbstractItemView::doubleClicked, this, [this](const QModelIndex &idx) {
        if (model_) {
            auto filterManager = model_->filterManagerAtIndex(idx);
            filterManager->showDialog(this);
        }
    });

    connect(toggleLookEnabledAction, &QAction::toggled, this, &LookFilters::lookEnabledChanged);

    syncWidgetEnabled();
}

void LookFilters::setLookFiltersModel(LookFiltersModel *model) noexcept {
    assert(model != nullptr);
    model_ = model;
    listView->setModel(model);

    connect(model_, &QAbstractItemModel::modelReset, this, &LookFilters::syncWidgetEnabled);
    syncWidgetEnabled();
}

void LookFilters::syncWidgetEnabled() noexcept { setEnabled(model_ && model_->look()); }
