#pragma once

#include <memory>

#include <QAbstractListModel>
#include <QDebug>
#include <QMap>

#include <image/Composition.hpp>

#include <app/filters/FilterManager.hpp>

namespace internal {

    enum class NodeType { Invalid, Composition, Layer, Filters, Mask, Filter };

    std::ostream& operator<<(std::ostream& out, const NodeType value) noexcept;
    QDebug operator<<(QDebug out, NodeType value) noexcept;

    template <class T>
    struct NodeTraits {};

    template <>
    struct NodeTraits<image::Composition> {
        inline static const NodeType type = NodeType::Composition;
    };

    template <>
    struct NodeTraits<image::Layer> {
        inline static const NodeType type = NodeType::Layer;
    };

    template <>
    struct NodeTraits<image::Filters> {
        inline static const NodeType type = NodeType::Filters;
    };

    template <>
    struct NodeTraits<image::Mask> {
        inline static const NodeType type = NodeType::Mask;
    };

    template <>
    struct NodeTraits<image::AbstractFilterSpec> {
        inline static const NodeType type = NodeType::Filter;
    };

    /**
     * @brief Maintains a tree structure of the composition.
     * 
     * This acts as an interface for CompositionModel to manipulate the composition. The reason is two-fold: a) it helps simplify
     * the CompositionModel implementation, and b) it helps prevent the Qt-specific implementation from polluting libimage.
     * 
     * CompositionModel (and Qt code in general) should interact with this instead of image::Composition et.al. directly.
     * 
     * Any Node* values should be considered ephemeral and should not be stored. See also QModelIndex.
     * 
     * NB: The tree structure makes certain assumptions:
     *   - The root of the tree is a Composition
     *   - The children directly under the root are Layers
     *   - The first child of every Layer node is a Filters node
     *   - The second child of any Layer node (if any) is a Mask node
     *   - The children of a Filters node are Filter nodes
     */
    struct Node {
        NodeType type { NodeType::Invalid };
        void *ptr { nullptr };

        Node *parent { nullptr };
        Node *root { nullptr };
        std::vector<Node *> children;

        /**
         * @brief Returns a reference to the data pointed to by this Node.
         * 
         * @tparam T Must be the type of data this node points to
         */
        template <class T>
        T &get() noexcept {
            assert(type == NodeTraits<T>::type);
            return *static_cast<T*>(ptr);
        }

        /**
         * @brief Add a child node for some data.
         * 
         * @tparam T Must have a NodeTraits implementation
         * @param value A pointer to the data to reference
         * @return Node* A pointer to the newly created node
         */
        template <class T>
        Node *addChild(T *value) noexcept {
            auto node = new Node();
            node->type = NodeTraits<T>::type;
            node->ptr = value;
            node->parent = this;
            node->root = root;
            children.push_back(node);
            return node;
        }

        /**
         * @brief Returns child at idx or nullptr if out of range.
         */
        inline Node *child(int idx) noexcept {
            if (idx >= static_cast<int>(children.size())) {
                return nullptr;
            }
            return children.at(idx);
        }

        Node *addLayer() noexcept;
        void removeLayers(int startIdx, int count) noexcept;

        Node *addFilter(std::unique_ptr<image::AbstractFilterSpec> &&filter) noexcept;
        void removeFilters(int startIdx, int count) noexcept;

        Node *addMask() noexcept;
        void removeMask() noexcept;

        Node() noexcept {}
        explicit Node(std::shared_ptr<image::Composition> comp) noexcept;

        ~Node() noexcept;
    };

}

class CompositionModel : public QAbstractItemModel {
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        NodeTypeRole,
    };

    inline std::shared_ptr<image::Composition> composition() noexcept { return composition_; }
    void setComposition(std::shared_ptr<image::Composition> composition) noexcept;

    void addLayer() noexcept;
    void addFilter(const QModelIndex &idx, std::unique_ptr<image::AbstractFilterSpec> &&filter) noexcept;
    void addLayerMask(const QModelIndex &idx) noexcept;

    internal::Node *nodeAtIndex(const QModelIndex &idx) const noexcept;

    FilterManager *filterManagerAtIndex(const QModelIndex &idx) noexcept;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &index) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &idx, const QVariant &value, int role = Qt::EditRole) override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    CompositionModel() noexcept {}
    explicit CompositionModel(std::shared_ptr<image::Composition> composition, QObject *parent = nullptr) noexcept;
    explicit CompositionModel(const CompositionModel &other) noexcept;

    virtual ~CompositionModel() {}

signals:
    void compositionUpdated();

protected:
    QModelIndex findClosestFiltersNode(const QModelIndex &idx) const noexcept;

private:
    std::shared_ptr<image::Composition> composition_;
    std::shared_ptr<internal::Node> root_;
    QMap<image::AbstractFilterSpec *, FilterManager *> filterManagers_;
};
