#pragma once

#include <array>
#include <concepts>

#include <glm/glm.hpp>

#include <image/Color.hpp>
#include <image/CoreTypes.hpp>

namespace image {

    struct ImageSize final : public glm::vec<2, std::size_t, glm::defaultp> {
        constexpr ImageSize() noexcept : glm::vec<2, std::size_t, glm::defaultp>(0) {}

        template <class... Args>
        constexpr ImageSize(Args &&... args) noexcept
          : glm::vec<2, std::size_t, glm::defaultp>(std::forward<Args>(args)...) {}
    };

    /**
     * @brief Identifies an image channel.
     */
    enum class Channel : U8 { Invalid, X, Red, Green, Blue, Alpha };

    /**
     * @brief Types satisfying this constraint describe the channels in an image.
     */
    template <class T>
    concept ChannelsSpec = requires(T s) {
        typename T::VectorType<int>;
        { T::numChannels() } -> std::same_as<std::size_t>;
    };

    namespace detail {
        template <class C, class... Cs>
        constexpr Channel channelAt(std::size_t i, C c, Cs... cs) noexcept {
            if (i == 0) {
                return c;
            } else {
                return channelAt(i - 1, cs...);
            }
        }

        template <class C, class... Cs>
        constexpr std::size_t channelIndex(std::size_t i, Channel target, C c, Cs... cs) noexcept {
            if (c == target) {
                return i;
            } else {
                return channelIndex(i + 1, target, cs...);
            }
        }
    }

    /**
     * @brief Describes channels in an image.
     * 
     * Includes helpers to interrogate the list.
     * 
     * @tparam Channels (e.g. Red, Green, Blue)
     */
    template <Channel... Channels>
    struct ChannelList {
        /**
         * @brief Gets the channel at index i.
         */
        constexpr static Channel at(std::size_t i) noexcept {
            static_assert(i < sizeof...(Channels), "Channel index out of bounds");
            return detail::channelAt(i, Channels...);
        }

        /**
         * @brief Gets the index of channel c.
         */
        constexpr static std::size_t index(Channel c) noexcept { return detail::channelIndex(0, c, Channels...); }

        /**
         * @brief Returns the totl number of channels.
         */
        constexpr static std::size_t numChannels() noexcept { return sizeof...(Channels); }

        /**
         * @brief Builds an array of the channel identifiers.
         */
        constexpr static std::array<Channel, sizeof...(Channels)> asArray() noexcept {
            return std::array { Channels... };
        }

        /**
         * @brief A type which can contain values for all channels.
         */
        template <class T>
        using VectorType = glm::vec<sizeof...(Channels), T, glm::defaultp>;
    };

    /**
     * @brief Specialization for the zero channel case.
     * 
     * For monochrome images, masks, etc. There are no channels and VectorType<T> ~ T.
     * Does not implement at or index.
     */
    template <>
    struct ChannelList<> {
        constexpr static std::size_t numChannels() noexcept { return 0; }
        constexpr static std::array<Channel, 0> asArray() noexcept { return std::array<Channel, 0> {}; }

        template <class T>
        using VectorType = T;
    };

    using Greyscale = ChannelList<>;
    using RGB = ChannelList<Channel::Red, Channel::Green, Channel::Blue>;
    using RGBA = ChannelList<Channel::Red, Channel::Green, Channel::Blue, Channel::Alpha>;
    using ARGB = ChannelList<Channel::Alpha, Channel::Red, Channel::Green, Channel::Blue>;

}
