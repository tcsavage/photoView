#include "DataAPIInterface.hpp"

namespace image::serialization {

    void addData(pt::ptree &tree, const String &key, Data &&data) noexcept {
        if (std::holds_alternative<String>(data)) {
            tree.add(key, std::move(std::get<String>(data)));
        } else if (std::holds_alternative<I32>(data)) {
            tree.add(key, std::move(std::get<I32>(data)));
        } else if (std::holds_alternative<F32>(data)) {
            tree.add(key, std::move(std::get<F32>(data)));
        } else {
            tree.add_child(key, std::move(std::get<pt::ptree>(data)));
        }
    }

    void writeFilter(const WriteContext &ctx, pt::ptree &filterTree, AbstractFilterSpec &filterSpec) noexcept {
        auto ref = dynFilterRef(&filterSpec);
        filterTree = std::get<pt::ptree>(ifaceImpl<Serialization>(ref)->encode(ctx, ref));
    }

}
