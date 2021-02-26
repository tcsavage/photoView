#include <app/composition/CompositionOutline.hpp>

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

#include <app/filters/Filters.hpp>

using namespace image;

CompositionOutline::CompositionOutline(QWidget *parent) noexcept : QWidget(parent) {
    auto layout = new QVBoxLayout();
    setLayout(layout);

    // Layout for actions on top.
    auto topLayout = new QHBoxLayout();

    // Set-up new layer button.
    auto newLayerAction = new QAction("New Layer");
    auto newLayerBtn = new QToolButton();
    newLayerBtn->setDefaultAction(newLayerAction);
    topLayout->addWidget(newLayerBtn);

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
    auto toggleFiltersEnabledAction = new QAction("Enabled");
    toggleFiltersEnabledAction->setCheckable(true);
    toggleFiltersEnabledAction->setChecked(true);
    auto toggleFiltersEnabledBtn = new QToolButton();
    toggleFiltersEnabledBtn->setDefaultAction(toggleFiltersEnabledAction);
    topLayout->addWidget(toggleFiltersEnabledBtn);

    // Add top-layout.
    layout->addLayout(topLayout);

    // Set-up filter list.
    treeView = new QTreeView();
    treeView->setHeaderHidden(true);
    layout->addWidget(treeView);

    // Context menu.
    setupContextMenu();

    // Set-up connections.

    connect(newLayerAction, &QAction::triggered, this, [this]() {
        if (model_) { model_->addLayer(); }
    });

    connect(addFilterBtn, &QToolButton::triggered, this, [this](QAction *action) {
        if (model_) {
            image::String id = action->data().toString().toStdString();
            auto filterResult = filterRegistry.create(id);  // TODO: Could this result in an error?

            auto idx = treeView->currentIndex();
            model_->addFilter(idx, std::move(*filterResult));
        }
    });

    connect(treeView, &QAbstractItemView::doubleClicked, this, [this](const QModelIndex &idx) {
        if (model_) {
            // filterManagerAtIndex will return nullptr if idx doesn't point to a filter.
            if (auto filterManager = model_->filterManagerAtIndex(idx)) { filterManager->showDialog(this); }
        }
    });

    connect(toggleFiltersEnabledAction, &QAction::toggled, this, &CompositionOutline::filtersEnabledChanged);

    syncWidgetEnabled();
}

void CompositionOutline::setCompositionModel(CompositionModel *model) noexcept {
    assert(model != nullptr);
    model_ = model;
    treeView->setModel(model);
    treeView->expandAll();

    connect(model_, &QAbstractItemModel::modelReset, this, &CompositionOutline::syncWidgetEnabled);

    connect(model_, &CompositionModel::rowsInserted, this, [this](const QModelIndex &parent, int first, int) {
        // Scroll to the position of the first inserted row.
        auto idx = model_->index(first, 0, parent);
        treeView->scrollTo(idx);
        treeView->setCurrentIndex(idx);
    });

    syncWidgetEnabled();
}

void CompositionOutline::setupContextMenu() noexcept {
    treeView->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(treeView, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        auto idx = treeView->indexAt(pos);
        auto node = model_->nodeAtIndex(idx);
        if (!idx.isValid()) { return; }
        QMenu menu;
        if (node->type == internal::NodeType::Layer) {
            menu.addAction("Add &Mask", [&] { model_->addLayerMask(idx); });
        }
        if (node->type == internal::NodeType::Filter || node->type == internal::NodeType::Mask ||
            node->type == internal::NodeType::Layer) {
            menu.addAction("&Delete", [&] { model_->removeRow(idx.row(), idx.parent()); });
        }
        menu.exec(treeView->mapToGlobal(pos));
    });
}

void CompositionOutline::syncWidgetEnabled() noexcept { setEnabled(model_ && model_->composition()); }
