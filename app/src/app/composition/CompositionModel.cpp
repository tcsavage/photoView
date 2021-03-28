#include <app/composition/CompositionModel.hpp>

#include <cassert>
#include <iostream>

#include <QByteArray>
#include <QDebug>
#include <QHash>
#include <QMimeData>
#include <QStringList>

#include <image/Serialization.hpp>

using namespace image;

namespace internal {

    std::ostream &operator<<(std::ostream &out, const NodeType value) noexcept {
        const char *s = 0;

#define PROCESS_VAL(p) \
    case (p):          \
        s = #p;        \
        break;

        switch (value) {
            PROCESS_VAL(NodeType::Invalid);
            PROCESS_VAL(NodeType::Composition);
            PROCESS_VAL(NodeType::Layer);
            PROCESS_VAL(NodeType::Filters);
            PROCESS_VAL(NodeType::Mask);
            PROCESS_VAL(NodeType::Filter);
        }

#undef PROCESS_VAL

        return out << s;
    }

    QDebug operator<<(QDebug out, NodeType value) noexcept {
        QString s;

#define PROCESS_VAL(p) \
    case (p):          \
        s = #p;        \
        break;

        switch (value) {
            PROCESS_VAL(NodeType::Invalid);
            PROCESS_VAL(NodeType::Composition);
            PROCESS_VAL(NodeType::Layer);
            PROCESS_VAL(NodeType::Filters);
            PROCESS_VAL(NodeType::Mask);
            PROCESS_VAL(NodeType::Filter);
        }

#undef PROCESS_VAL

        return out << s;
    }

    Node *Node::addLayer() noexcept {
        auto &comp = root->get<Composition>();
        auto layer = std::make_shared<Layer>();
        comp.layers.push_back(layer);
        auto layerNode = root->addChild(layer.get());
        layerNode->addChild(layer->filters.get());
        return layerNode;
    }

    void Node::removeLayers(int startIdx, int count) noexcept {
        auto &comp = root->get<Composition>();
        {
            auto it = comp.layers.begin() + startIdx;
            comp.layers.erase(it, it + count);
        }
        {
            auto it = root->children.begin() + startIdx;
            root->children.erase(it, it + count);
        }
    }

    Node *Node::addFilter(std::unique_ptr<AbstractFilterSpec> &&filter) noexcept {
        assert(type == NodeType::Filters);
        auto &ref = get<Filters>().addFilter(std::move(filter));
        return addChild(ref.get());
    }

    Node *Node::addFilter(std::unique_ptr<AbstractFilterSpec> &&filter, int idx) noexcept {
        assert(type == NodeType::Filters);
        auto &ref = get<Filters>().addFilter(std::move(filter), idx);
        return addChild(ref.get(), idx);
    }

    void Node::removeFilters(int startIdx, int count) noexcept {
        assert(type == NodeType::Filters);
        auto &filters = get<Filters>();
        // Remove filters.
        filters.removeFilters(startIdx, count);
        // Remove corresponding child nodes.
        auto it = children.begin() + startIdx;
        children.erase(it, it + count);
    }

    Node *Node::addMask(std::unique_ptr<AbstractMaskGenerator> &&gen) noexcept {
        assert(type == NodeType::Layer);
        auto &layer = get<Layer>();
        layer.mask = std::make_shared<GeneratedMask>(std::move(gen));
        auto &comp = root->get<Composition>();
        layer.mask->update(*comp.inputImage.data);
        return addChild(layer.mask.get());
    }

    void Node::removeMask() noexcept {
        assert(type == NodeType::Layer);
        auto &layer = get<Layer>();
        assert(layer.mask);
        children.pop_back();  // Mask should be last child.
        layer.mask = nullptr;
    }

    Node::Node(std::shared_ptr<Composition> comp) noexcept : type(NodeTraits<Composition>::type), ptr(comp.get()) {
        root = this;
        parent = nullptr;

        for (auto &&layer : comp->layers) {
            auto layerNode = addChild(layer.get());
            auto filtersNode = layerNode->addChild(layer->filters.get());
            for (auto &&filterSpec : layer->filters->filterSpecs) {
                filtersNode->addChild(filterSpec.get());
            }
            if (layer->mask) { layerNode->addChild(layer->mask.get()); }
        }
    }

    Node::~Node() noexcept {
        // Clean-up children.
        for (auto &&child : children) {
            delete child;
        }
    }

}

using namespace internal;

void CompositionModel::setComposition(std::shared_ptr<Composition> composition) noexcept {
    beginResetModel();
    filterManagers_.clear();
    composition_ = composition;
    root_ = std::make_shared<Node>(composition);
    endResetModel();
}

void CompositionModel::addLayer() noexcept {
    if (!composition_ || !root_) { return; }
    int newIdx = composition_->layers.size();
    beginInsertRows(QModelIndex(), newIdx, newIdx);
    root_->addLayer();
    endInsertRows();
    notifyCompositionUpdated();
}

void CompositionModel::addFilter(const QModelIndex &idx, std::unique_ptr<image::AbstractFilterSpec> &&filter) noexcept {
    if (!composition_ || !root_) { return; }
    // idx may not point to a Filters node. We need to find the best option.
    // TODO: can we move this into the Node structure?
    auto filtersIdx = findClosestFiltersNode(idx);
    auto node = nodeAtIndex(filtersIdx);
    auto &filters = node->get<Filters>();
    auto rowIdx = filters.filterSpecs.size();
    beginInsertRows(filtersIdx, rowIdx, rowIdx);
    node->addFilter(std::move(filter));
    endInsertRows();
    notifyCompositionUpdated();
}

void CompositionModel::addFilters(const QModelIndex &parent,
                                  int row,
                                  std::vector<std::unique_ptr<image::AbstractFilterSpec>> &&filters) noexcept {
    if (!composition_ || !root_) { return; }
    auto node = nodeAtIndex(parent);
    if (node->type != NodeType::Filters) { return; }
    beginInsertRows(parent, row, row);
    int idx = row;
    for (auto &&filter : filters) {
        node->addFilter(std::move(filter), idx);
        ++idx;
    }
    endInsertRows();
    notifyCompositionUpdated();
}

void CompositionModel::addLayerMask(const QModelIndex &idx, std::unique_ptr<AbstractMaskGenerator> &&gen) noexcept {
    if (!composition_ || !root_) { return; }
    auto node = nodeAtIndex(idx);
    // This should only ever be called on an index pointing to a layer.
    assert(node->type == NodeType::Layer);
    assert(!node->get<Layer>().mask);
    beginInsertRows(idx, 1, 1);  // Mask should always live at row 1 under a Layer.
    node->addMask(std::move(gen));
    endInsertRows();
    notifyCompositionUpdated();
}

Node *CompositionModel::nodeAtIndex(const QModelIndex &idx) const noexcept {
    if (idx.isValid()) {
        return static_cast<Node *>(idx.internalPointer());
    } else {
        return root_.get();
    }
}

FilterManager *CompositionModel::filterManagerAtIndex(const QModelIndex &idx) noexcept {
    auto node = nodeAtIndex(idx);
    if (node->type != NodeType::Filter) { return nullptr; }
    auto &filter = node->get<AbstractFilterSpec>();
    if (filterManagers_.contains(&filter)) {
        return filterManagers_.value(&filter).get();
    } else {
        auto manager = std::make_shared<FilterManager>(&filter, this);
        filterManagers_.insert(&filter, manager);
        connect(manager.get(), &FilterManager::filterUpdated, this, &CompositionModel::compositionUpdated);
        return manager.get();
    }
}

QModelIndex CompositionModel::findClosestFiltersNode(const QModelIndex &idx) const noexcept {
    auto node = nodeAtIndex(idx);
    switch (node->type) {
    case NodeType::Filters:
        return idx;
    case NodeType::Filter:
        return idx.parent();
    case NodeType::Mask:
        return findClosestFiltersNode(idx.parent());
    case NodeType::Layer:
        // A layer only has one or two children. The first should always be a filters. (The second is an optional mask.)
        return index(0, 0, idx);
    default:
        // This assumes the composition always has at least one layer.
        return findClosestFiltersNode(index(0, 0));
    }
}

QModelIndex CompositionModel::findClosestLayerNode(const QModelIndex &idx) const noexcept {
    if (!idx.isValid()) { return QModelIndex(); }
    auto node = nodeAtIndex(idx);
    switch (node->type) {
    case NodeType::Filters:
        return idx.parent();
    case NodeType::Filter:
        return findClosestLayerNode(idx.parent());
    case NodeType::Mask:
        return idx.parent();
    case NodeType::Layer:
        return idx;
    default:
        // This assumes the composition always has at least one layer.
        return index(0, 0);
    }
}

QModelIndex CompositionModel::index(int row, int column, const QModelIndex &parent) const {
    if (!composition_ || !root_) { return QModelIndex(); }
    Node *childNode = nullptr;
    Node *parentNode = nodeAtIndex(parent);
    if (parentNode) {
        childNode = parentNode->child(row);
    } else {
        childNode = root_->child(row);
    }
    return createIndex(row, column, childNode);
}

namespace {
    template <class T>
    int indexOf(const std::vector<T> &vec, T &target) {
        return vec.end() - std::find(vec.begin(), vec.end(), target) - 1;
    }
}

QModelIndex CompositionModel::parent(const QModelIndex &child) const {
    Node *node = nodeAtIndex(child);
    if (!node) { return QModelIndex(); }
    if (auto parent = node->parent) {
        std::size_t row;
        if (auto grandparent = parent->parent) {
            row = indexOf(grandparent->children, parent);
        } else {
            row = indexOf(root_->children, parent);
        }
        return createIndex(row, 0, parent);
    } else {
        return QModelIndex();
    }
}

int CompositionModel::rowCount(const QModelIndex &parent) const {
    if (!composition_ || !root_) { return 0; }
    auto node = nodeAtIndex(parent);
    return node->children.size();
}

int CompositionModel::columnCount(const QModelIndex &) const {
    if (!composition_) { return 0; }
    return 1;
}

QVariant CompositionModel::data(const QModelIndex &idx, int role) const {
    if (!idx.isValid() || !composition_) { return QVariant(); }

    auto node = nodeAtIndex(idx);

    if (node->type == NodeType::Filter) {
        auto &filterSpec = node->get<AbstractFilterSpec>();
        switch (role) {
        case Qt::DisplayRole:
        case Roles::NameRole:
            return QString::fromStdString(filterSpec.getMeta().name);
        case Qt::CheckStateRole:
            return filterSpec.isEnabled ? Qt::Checked : Qt::Unchecked;
        default:
            return QVariant();
        }
    }

    if (node->type == NodeType::Layer) {
        auto &layer = node->get<Layer>();
        switch (role) {
        case Qt::DisplayRole:
            return "Layer";
        case Qt::CheckStateRole:
            return layer.isEnabled ? Qt::Checked : Qt::Unchecked;
        default:
            return QVariant();
        }
    }

    if (node->type == NodeType::Filters) {
        switch (role) {
        case Qt::DisplayRole:
            return "Filters";
        default:
            return QVariant();
        }
    }

    if (node->type == NodeType::Mask) {
        auto &mask = node->get<GeneratedMask>();
        auto gen = mask.generator();
        switch (role) {
        case Qt::DisplayRole:
            return gen ? QString::fromStdString(gen->getMeta().name) : "Mask";
        case Qt::CheckStateRole:
            return mask.isEnabled ? Qt::Checked : Qt::Unchecked;
        default:
            return QVariant();
        }
    }

    if (node->type == NodeType::Composition) {
        switch (role) {
        case Qt::DisplayRole:
            return "Composition";
        default:
            return QVariant();
        }
    }

    if (node->type == NodeType::Invalid) {
        switch (role) {
        case Qt::DisplayRole:
            return "INVALID";
        default:
            return QVariant();
        }
    }

    return QVariant();  // __builtin_unreachable()?
}

bool CompositionModel::setData(const QModelIndex &idx, const QVariant &value, int role) {
    if (!idx.isValid() || !composition_) { return false; }

    auto node = nodeAtIndex(idx);

    if (node->type == NodeType::Filter) {
        auto &filter = node->get<AbstractFilterSpec>();
        switch (role) {
        case Qt::CheckStateRole:
            filter.isEnabled = value.value<Qt::CheckState>() == Qt::Checked;
            emit dataChanged(idx, idx);
            notifyCompositionUpdated();
            return true;
        default:
            return false;
        }
    }

    if (node->type == NodeType::Layer) {
        auto &layer = node->get<Layer>();
        switch (role) {
        case Qt::CheckStateRole:
            layer.isEnabled = value.value<Qt::CheckState>() == Qt::Checked;
            emit dataChanged(idx, idx);
            notifyCompositionUpdated();
            return true;
        default:
            return false;
        }
    }

    if (node->type == NodeType::Mask) {
        auto &mask = node->get<GeneratedMask>();
        switch (role) {
        case Qt::CheckStateRole:
            mask.isEnabled = value.value<Qt::CheckState>() == Qt::Checked;
            emit dataChanged(idx, idx);
            notifyCompositionUpdated();
            return true;
        default:
            return false;
        }
    }

    return true;
}

bool CompositionModel::removeRows(int row, int count, const QModelIndex &parent) {
    assert(composition_);
    auto node = nodeAtIndex(parent);
    if (node->type == NodeType::Filters) {
        beginRemoveRows(parent, row, row + count - 1);
        node->removeFilters(row, count);
        endRemoveRows();
        notifyCompositionUpdated();
        return true;
    }
    if (node->type == NodeType::Composition) {
        beginRemoveRows(parent, row, row + count - 1);
        node->removeLayers(row, count);
        endRemoveRows();
        notifyCompositionUpdated();
        return true;
    }
    if (node->type == NodeType::Layer) {
        assert(row == 1);  // Only the mask is deleteable, and always at row 1.
        assert(count == 1);
        beginRemoveRows(parent, 1, 1);
        node->removeMask();
        endRemoveRows();
        notifyCompositionUpdated();
        return true;
    }
    return false;
}

Qt::ItemFlags CompositionModel::flags(const QModelIndex &index) const {
    if (!index.isValid() || !composition_) { return Qt::NoItemFlags; }

    return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsUserCheckable | Qt::ItemFlag::ItemIsDragEnabled |
           Qt::ItemFlag::ItemIsDropEnabled | Qt::ItemFlag::ItemIsSelectable;
}

QHash<int, QByteArray> CompositionModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[NodeTypeRole] = "nodeType";
    return roles;
}

Qt::DropActions CompositionModel::supportedDropActions() const {
    return Qt::DropAction::CopyAction | Qt::DropAction::MoveAction;
}

QStringList CompositionModel::mimeTypes() const {
    QStringList out { "application/x.photoView.filters+json" };
    return out;
}

QMimeData *CompositionModel::mimeData(const QModelIndexList &indexes) const {
    std::vector<AbstractFilterSpec *> filters;
    for (auto &&idx : indexes) {
        auto node = nodeAtIndex(idx);
        if (node->type == NodeType::Filter) { filters.push_back(&node->get<AbstractFilterSpec>()); }
    }
    if (filters.size() == 0) { return nullptr; }
    auto enc = serialization::encodeFilters(filters);
    QMimeData *mimeData = new QMimeData();
    mimeData->setData("application/x.photoView.filters+json", QByteArray::fromStdString(enc));
    return mimeData;
}

bool CompositionModel::dropMimeData(const QMimeData *data, Qt::DropAction, int row, int, const QModelIndex &parent) {
    // Ignore any data we can't handle.
    if (!data->hasFormat("application/x.photoView.filters+json")) { return false; }

    // Decode filters data. If empty, fail immediately.
    auto enc = data->data("application/x.photoView.filters+json").toStdString();
    auto filters = serialization::decodeFilters(enc);
    if (filters.size() == 0) { return false; }

    // Look-up parent node so we can decide what do do based on what it is.
    auto node = nodeAtIndex(parent);

    if (node->type == NodeType::Filters) {
        // If we dropped on the Filters node itself, insert the filters at the top.
        if (row < 0) { row = 0; }
        addFilters(parent, row, std::move(filters));
        return true;
    } else if (node->type == NodeType::Filter) {
        // We dropped on another filter. Insert in its place (i.e. above it.)
        // We ignore the value of row in this case. Filters can't have children, so it's expected row will always == -1.
        addFilters(parent.parent(), parent.row(), std::move(filters));
        return true;
    } else if (node->type == NodeType::Layer) {
        // We dropped on a layer. Again, we assume row always == -1.
        // Generate a new model index for the filters node. Assumes this is always at row 0.
        auto filtersIdx = index(0, 0, parent);
        // Inserts at top like if we dropped on filters node.
        addFilters(filtersIdx, 0, std::move(filters));
        return true;
    }
    return false;
}

bool CompositionModel::canDropMimeData(const QMimeData *data,
                                       Qt::DropAction,
                                       int,
                                       int,
                                       const QModelIndex &parent) const {
    if (data->hasFormat("application/x.photoView.filters+json")) {
        auto node = nodeAtIndex(parent);
        return node->type == NodeType::Filter || node->type == NodeType::Filters || node->type == NodeType::Layer;
    }
    return false;
}

void CompositionModel::notifyCompositionUpdated() noexcept {
    // To avoid unpleasentness when dealing with rapid repeating updates, this function uses a timer to
    // emit the event. The timer is started with a short timeout when requested, and then reset if requested
    // again before the timout expires.
    if (!updateNotificationTimer) {
        updateNotificationTimer = new QTimer(this);
        updateNotificationTimer->setSingleShot(true);
        updateNotificationTimer->callOnTimeout(this, &CompositionModel::compositionUpdated);
    }
    // Emit compositionUpdated in 10msec.
    updateNotificationTimer->start(10);
}

CompositionModel::CompositionModel(std::shared_ptr<image::Composition> composition, QObject *parent) noexcept
  : QAbstractItemModel(parent)
  , composition_(composition)
  , root_(std::make_shared<Node>(composition)) {}

CompositionModel::CompositionModel(const CompositionModel &other) noexcept
  : QAbstractItemModel(other.QObject::parent())
  , composition_(other.composition_)
  , root_(other.root_) {}
