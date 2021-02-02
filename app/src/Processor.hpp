#pragma once

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/Filters.hpp>
#include <image/ImageBuf.hpp>
#include <image/Look.hpp>

struct LutLoadFailure {
    image::Path path;
    image::String reason;
};

struct ImageLoadFailure {
    image::Path path;
    image::String reason;
};

struct ImageExportFailure {
    image::Path path;
    image::String reason;
};

struct Processor {
    image::ImageBuf<image::F32> input;
    image::ImageBuf<image::U8> output;
    std::shared_ptr<image::Look> identityLook;
    std::shared_ptr<image::Look> look;
    image::Processor proc;

    image::LutFilterSpec *lutFilter { nullptr };
    image::ExposureFilterSpec *exposureFilter { nullptr };

    bool lutLoaded { false };

    void loadHald(std::size_t lutSize);

    image::Expected<void, LutLoadFailure> loadLutFromFile(image::Path path);
    image::Expected<void, ImageLoadFailure> loadImageFromFile(image::Path path);
    image::Expected<void, ImageExportFailure> exportImageToFile(image::Path path) const;

    void setProcessingEnabled(bool processingEnabled);
    void setLutStrengthFactor(image::F32 factor);
    void setExposure(image::F32 exposure = 0.0f);

    Processor() noexcept;
};
