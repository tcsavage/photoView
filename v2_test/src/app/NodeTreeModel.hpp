#pragma once

#include <QAbstractItemModel>

#include <app/NodeTree.hpp>

namespace image {

    class NodeTreeModel : public QAbstractItemModel {
        Q_OBJECT
    public:
        Node *nodeAtIndex(const QModelIndex &idx) const noexcept;

        virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
        virtual QModelIndex parent(const QModelIndex &index) const override;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        virtual QHash<int, QByteArray> roleNames() const override;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
        virtual QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;

        const Node &root() const noexcept { return *rootNode; }

    protected:
        void setRoot(std::unique_ptr<Node> &&newRoot) noexcept;

        explicit NodeTreeModel(QObject *parent = nullptr) noexcept : QAbstractItemModel(parent) {}
        explicit NodeTreeModel(std::unique_ptr<Node> &&rootNode, QObject *parent = nullptr) noexcept
          : QAbstractItemModel(parent)
          , rootNode(std::move(rootNode)) {}

    private:
        std::unique_ptr<Node> rootNode;
    };

}