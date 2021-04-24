#include <app/NodeTree.hpp>

#include <iostream>

namespace image {

    namespace {

        template <class T>
        int indexOf(const std::vector<T> &vec, T &target) {
            return vec.end() - std::find(vec.begin(), vec.end(), target) - 1;
        }

    }

    DynamicRef Node::getDynamic() noexcept { return DynamicRef::makeUnchecked(ptr.get(), typeInfo); }

    Node *Node::addChildUnchecked(std::shared_ptr<void> ptr, AbstractTypeInfo *typeInfo, int idx) noexcept {
        auto node = new Node(ptr, typeInfo, nullptr);
        node->setParent(this, idx);
        return node;
    }

    void Node::addChild(Node *node, int idx) noexcept {
        node->parent = this;
        node->root = root;
        if (idx < 0) {
            children.push_back(node);
        } else {
            auto it = children.begin() + idx;
            children.insert(it, node);
        }
    }

    Node *Node::child(int idx) noexcept {
        if (idx >= static_cast<int>(children.size())) { return nullptr; }
        return children.at(idx);
    }

    void Node::setParent(Node *newParent, int idx) noexcept {
        if (parent) {
            auto it = std::find(parent->children.begin(), parent->children.end(), this);
            if (it != parent->children.end()) { children.erase(it); }
        }
        newParent->addChild(this, idx);
    }

    std::size_t Node::childIndex(Node *node) noexcept { return indexOf(children, node); }

    void Node::dumpStructure(int indent) const noexcept {
        std::cerr << String(indent, ' ');
        std::cerr << "Node: " << typeInfo->ident << " (" << children.size() << " children)\n";
        for (auto &&child : children) {
            child->dumpStructure(indent + 2);
        }
    }

    Node::~Node() {
        // Clean-up children.
        for (auto &&child : children) {
            delete child;
        }
    }

}
