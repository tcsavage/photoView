#include <app/NodeTreeModel.hpp>

#include <iostream>

#include <QDebug>

namespace image {

    namespace {

        Node *indexNode(const QModelIndex &idx) noexcept {
            if (idx.isValid()) {
                return static_cast<Node *>(idx.internalPointer());
            } else {
                return nullptr;
            }
        }

    }

    Node *NodeTreeModel::nodeAtIndex(const QModelIndex &idx) const noexcept {
        if (auto node = indexNode(idx)) {
            return node;
        }
        return rootNode.get();
    }

    QModelIndex NodeTreeModel::index(int row, int column, const QModelIndex &parent) const {
        if (!rootNode) { return QModelIndex(); }
        Node *childNode = nullptr;
        Node *parentNode = nodeAtIndex(parent);
        if (parentNode) {
            childNode = parentNode->child(row);
        } else {
            childNode = rootNode->child(row);
        }
        return createIndex(row, column, childNode);
    }

    QModelIndex NodeTreeModel::parent(const QModelIndex &index) const {
        Node *node = nodeAtIndex(index);
        if (!node) { return QModelIndex(); }
        if (auto parent = node->parent) {
            std::size_t row;
            if (auto grandparent = parent->parent) {
                row = grandparent->childIndex(parent);
            } else {
                row = rootNode->childIndex(parent);
            }
            return createIndex(row, 0, parent);
        } else {
            return QModelIndex();
        }
    }

    int NodeTreeModel::rowCount(const QModelIndex &parent) const {
        if (!rootNode) { return 0; }
        auto node = nodeAtIndex(parent);
        auto size =  node->children.size();
        return size;
    }

    int NodeTreeModel::columnCount(const QModelIndex &) const {
        if (!rootNode) { return 0; }
        return 1;
    }

    QHash<int, QByteArray> NodeTreeModel::roleNames() const {
        QHash<int, QByteArray> roles;
        return roles;
    }

    Qt::ItemFlags NodeTreeModel::flags(const QModelIndex &index) const {
        if (!index.isValid() || !rootNode) { return Qt::NoItemFlags; }

        return Qt::ItemFlag::ItemIsEnabled;
    }

    QVariant NodeTreeModel::data(const QModelIndex &idx, int role) const {
        if (!idx.isValid() || !rootNode) { return QVariant(); }

        auto node = nodeAtIndex(idx);
        (void)node;

        switch (role) {
        case Qt::DisplayRole:
            return QString::fromStdString(String(node->typeInfo->ident));
        }

        return QVariant();
    }

    void NodeTreeModel::setRoot(std::unique_ptr<Node> &&newRoot) noexcept {
        beginResetModel();
        rootNode = std::move(newRoot);
        std::cerr << "New root node:\n";
        rootNode->dumpStructure();
        endResetModel();
    }

}
