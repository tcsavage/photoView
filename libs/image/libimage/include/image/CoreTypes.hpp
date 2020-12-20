#pragma once

// #include <glm/glm.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <filesystem>

namespace image {

    using U8 = uint8_t;
    using U16 = uint16_t;
    using U32 = uint32_t;
    using U64 = uint64_t;
    using I8 = int8_t;
    using I16 = int16_t;
    using I32 = int32_t;
    using I64 = int64_t;
    //using F16 = half;
    using F32 = float;
    using F64 = double;
    using String = std::string;
    using StringView = std::string_view;
    using Path = std::filesystem::path;
    using Instant = std::chrono::system_clock::time_point;

    // using Size = glm::u64vec2;

}
