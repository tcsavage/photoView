#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include <image/Assert.hpp>
#include <image/CoreTypes.hpp>

namespace image {

    template <class Base>
    class StaticFactoryRegistry {
        using FactoryFn = std::function<std::unique_ptr<Base>()>;

    public:
        StaticFactoryRegistry() = delete;

        template <typename Sub, typename... Args>
        constexpr static void add(StringView id, Args &&... args) noexcept {
            FactoryFn factory = [... args = std::forward<Args>(args)]() { return std::make_unique<Sub>(args...); };
            map_[id] = std::move(factory);
        }

        constexpr static std::unique_ptr<Base> construct(StringView id) noexcept {
            ASSERT(map_.contains(id), "Requested item not found in registry");
            return map_.at(id)();
        }

        constexpr static std::vector<StringView> registeredIds() noexcept {
            std::vector<StringView> coll;
            for (auto &&[id, factory] : map_) {
                coll.push_back(id);
            }
            return coll;
        }

    private:
        inline static std::map<StringView, FactoryFn> map_;
    };

}

#define PHOTO_VIEW_INTERNAL__REGISTER_FACTORY(registryType, typeParam, key)  \
    struct registryType##_##typeParam##_registration {                       \
        inline static bool isRegistered = [] { registryType::add<typeParam>(key); return true; }(); \
    }
