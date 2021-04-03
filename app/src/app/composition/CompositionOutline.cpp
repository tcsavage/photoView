#include <app/composition/CompositionOutline.hpp>

#include <cassert>

#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QClipboard>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMimeData>
#include <QPushButton>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>

#include <image/Filters.hpp>
#include <image/Serialization.hpp>

#include <app/filters/Filters.hpp>
#include <app/masks/MaskGenerators.hpp>

using namespace image;

CompositionTreeView::CompositionTreeView(QWidget *parent) : QTreeView(parent) {
    setDefaultDropAction(Qt::DropAction::MoveAction);
}

void CompositionTreeView::setModel(QAbstractItemModel *model) {
    QTreeView::setModel(model);
    auto compositionModel = static_cast<CompositionModel *>(model);
    connect(compositionModel, &CompositionModel::rowsInserted, this, &CompositionTreeView::handleRowsInserted);
}

void CompositionTreeView::handleRowsInserted(const QModelIndex &parent, int first, int) {
    auto idx = model()->index(first, 0, parent);
    scrollTo(idx);
}

void CompositionTreeView::currentChanged(const QModelIndex &current, const QModelIndex &) {
    emit currentIndexChanged(current);
}

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
    treeView = new CompositionTreeView();
    treeView->setHeaderHidden(true);
    layout->addWidget(treeView);
    treeView->setDragEnabled(true);
    treeView->setAcceptDrops(true);
    treeView->setDropIndicatorShown(true);

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

    connect(treeView, &CompositionTreeView::currentIndexChanged, this, [this](const QModelIndex &idx) {
        if (model_) {
            auto layerIdx = model_->findClosestLayerNode(idx);
            if (!layerIdx.isValid()) { return; }
            auto layerNode = model_->nodeAtIndex(layerIdx);
            auto &layer = layerNode->get<Layer>();
            image::AbstractMaskGenerator *activeMaskGen = nullptr;
            if (layer.maskGen) { activeMaskGen = layer.maskGen.get(); }
            if (activeMaskGen != lastActiveMaskGen_) {
                lastActiveMaskGen_ = activeMaskGen;
                emit activeMaskChanged(lastActiveMaskGen_);
            }
        }
    });

    connect(toggleFiltersEnabledAction, &QAction::toggled, this, &CompositionOutline::filtersEnabledChanged);

    syncWidgetEnabled();
}

void CompositionOutline::setCompositionModel(CompositionModel *model) noexcept {
    assert(model != nullptr);
    model_ = model;
    treeView->setModel(model);

    connect(model_, &QAbstractItemModel::modelReset, this, &CompositionOutline::syncWidgetEnabled);

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
            auto addMaskGeneratorMenu = makeMaskGeneratorMenu();
            addMaskGeneratorMenu->setTitle("Add &Mask");
            menu.addMenu(addMaskGeneratorMenu);
            if (node->get<Layer>().maskGen) { addMaskGeneratorMenu->setEnabled(false); }
            connect(addMaskGeneratorMenu, &QMenu::triggered, this, [&](QAction *action) {
                auto id = action->data().toString().toStdString();
                auto maskGeneratorResult = maskGeneratorRegistry.create(id);  // TODO: Could this result in an error?
                std::shared_ptr<AbstractMaskGenerator> maskGen { maskGeneratorResult.value().release() };
                model_->addLayerMask(idx, maskGen);
            });
        }
        if (node->type == internal::NodeType::Filter) {
            menu.addAction("&Copy Filters", [&] {
                std::vector<AbstractFilterSpec *> filters;
                for (auto &&idx : treeView->selectionModel()->selectedIndexes()) {
                    auto node = model_->nodeAtIndex(idx);
                    if (node->type == internal::NodeType::Filter) {
                        filters.push_back(&node->get<AbstractFilterSpec>());
                    }
                }
                if (filters.size() == 0) { return; }
                auto enc = serialization::encodeFilters(filters);
                auto clipboard = QGuiApplication::clipboard();
                QMimeData *mimeData = new QMimeData();
                mimeData->setData("application/x.photoView.filters+json", QByteArray::fromStdString(enc));
                clipboard->setMimeData(mimeData);
            });
        }
        if (node->type == internal::NodeType::Filter || node->type == internal::NodeType::Filters ||
            node->type == internal::NodeType::Layer) {
            auto clipboard = QGuiApplication::clipboard();
            auto mimeData = clipboard->mimeData();
            menu.addAction("&Paste Filters",
                           [&] {
                               auto data = mimeData->data("application/x.photoView.filters+json");
                               auto filterSpecs = serialization::decodeFilters(data.toStdString());
                               auto filtersIdx = model_->findClosestFiltersNode(idx);
                               int row = node->type == internal::NodeType::Filter ? idx.row() : 0;
                               model_->addFilters(filtersIdx, row, std::move(filterSpecs));
                           })
                ->setEnabled(mimeData->hasFormat("application/x.photoView.filters+json"));
        }
        if (node->type == internal::NodeType::Mask) {
            menu.addAction("&Copy Mask", [&] {
                auto enc = serialization::encodeMask(&node->get<AbstractMaskGenerator>());
                auto clipboard = QGuiApplication::clipboard();
                QMimeData *mimeData = new QMimeData();
                mimeData->setData("application/x.photoView.mask+json", QByteArray::fromStdString(enc));
                clipboard->setMimeData(mimeData);
            });
        }
        if (node->type == internal::NodeType::Layer) {
            auto clipboard = QGuiApplication::clipboard();
            auto mimeData = clipboard->mimeData();
            auto &layer = node->get<Layer>();
            menu.addAction("&Paste Mask", [&] {
                auto data = mimeData->data("application/x.photoView.mask+json");
                               auto mask = serialization::decodeMask(data.toStdString());
                               model_->addLayerMask(idx, mask);
            })->setEnabled(mimeData->hasFormat("application/x.photoView.mask+json") && !layer.maskGen);
        }
        if (node->type == internal::NodeType::Filter || node->type == internal::NodeType::Mask ||
            node->type == internal::NodeType::Layer) {
            menu.addAction("&Delete", [&] {
                model_->removeRow(idx.row(), idx.parent());

                // If we are deleting a mask, also notify that new active mask is nullptr.
                // There must be a better way to handle this.
                if (node->type == internal::NodeType::Mask) { emit activeMaskChanged(nullptr); }
            });
        }
        menu.exec(treeView->mapToGlobal(pos));
    });
}

void CompositionOutline::syncWidgetEnabled() noexcept { setEnabled(model_ && model_->composition()); }
