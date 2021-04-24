#pragma once

#include <memory>
#include <vector>

#include <image/data/Ref.hpp>

namespace image {

    /**
     * @brief Represents a tree of dynamic objects.
     *
     * Each node owns the lifetime of its children.
     */
    struct Node {
        std::shared_ptr<void> ptr;
        AbstractTypeInfo *typeInfo { nullptr };

        Node *parent;
        Node *root;
        std::vector<Node *> children;

        template <DynamicType T>
        std::shared_ptr<T> get() noexcept {
            return std::static_pointer_cast<T>(ptr);
        }

        DynamicRef getDynamic() noexcept;

        Node *addChildUnchecked(std::shared_ptr<void> ptr, AbstractTypeInfo *typeInfo, int idx = -1) noexcept;

        template <DynamicType T>
        Node *addChild(std::shared_ptr<T> value, int idx = -1) noexcept {
            auto node = new Node(value, this, idx);
            return node;
        }

        void addChild(Node *node, int idx = -1) noexcept;
        Node *child(int idx) noexcept;

        void setParent(Node *newParent, int idx = -1) noexcept;

        std::size_t childIndex(Node *node) noexcept;

        void dumpStructure(int indent = 0) const noexcept;

        template <DynamicType T>
        explicit Node(std::shared_ptr<T> value) noexcept
          : Node(value, nullptr) {}

        template <DynamicType T>
        explicit Node(std::shared_ptr<T> value, Node *parent) noexcept
          : Node(value, &dynInfo<T>(), parent) {
              if (parent) { parent->children.push_back(this); }
          }

        explicit Node(std::shared_ptr<void> ptr, AbstractTypeInfo *typeInfo, Node *parent) noexcept
          : ptr(ptr)
          , typeInfo(typeInfo)
          , parent(parent)
          , root(parent ? parent->root : this) {
              if (parent) { parent->children.push_back(this); }
          }

        ~Node() noexcept;
    };

}
