#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>

namespace image {

    struct NotFoundInRegistry {
        String id;
    };

    template <typename Base, typename Meta>
    class Registry {
    public:
        using Id = String;
        using FactoryFn = std::function<std::unique_ptr<Base>()>;
        using IdCollection = std::vector<Id>;

        constexpr Registry() = default;
        constexpr ~Registry() = default;

        Registry(const Registry &) = delete;
        constexpr Registry(Registry &&) = default;
        Registry &operator=(const Registry &) = delete;
        constexpr Registry &operator=(Registry &&) = default;

        template <typename Sub, typename... Args>
        constexpr void registerType(Id id, Meta &&meta, Args &&... args) noexcept {
            FactoryFn factory = [... args = std::forward<Args>(args)]() { return std::make_unique<Sub>(args...); };
            _factories[id] = std::move(factory);
            _metas[id] = std::move(meta);
        }

        constexpr Expected<std::unique_ptr<Base>, NotFoundInRegistry> create(Id id) noexcept {
            auto iter = _factories.find(id);
            if (iter != _factories.end()) { return iter->second(); }

            return Unexpected(NotFoundInRegistry { id });
        }

        constexpr Expected<Meta *, NotFoundInRegistry> getMeta(Id id) noexcept {
            auto iter = _metas.find(id);
            if (iter != _metas.end()) { return &(iter->second); }

            return Unexpected(NotFoundInRegistry { id });
        }

        constexpr IdCollection registeredIds() const noexcept {
            IdCollection coll;
            for (auto &&[id, factory] : _factories) {
                coll.push_back(id);
            }
            return coll;
        }

    private:
        std::unordered_map<Id, FactoryFn> _factories;
        std::unordered_map<Id, Meta> _metas;
    };

}
