/**
 * @file
 * @brief Basic Image IO functions for ImageBufs.
 */

#pragma once

#include <OpenImageIO/imageio.h>

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/ImageBuf.hpp>
#include <image/Mask.hpp>

namespace image {

    struct ImageIOError {
        Path path;
        String reason;
    };

    template <class T>
    Expected<ImageBuf<T>, ImageIOError> readImageBufFromFile(const Path &path) {
        OIIO::ImageSpec spec;
        auto iin = OIIO::ImageInput::create(path.string());
        if (!iin) {
            return Unexpected(ImageIOError(path, OIIO::geterror()));
        }
        if (!iin->open(path, spec, spec)) {
            return Unexpected(ImageIOError(path, OIIO::geterror()));
        }
        auto imageBuf = ImageBuf<T>(spec.width, spec.height);
        // Assumes a packed, interleaved, RGB layout.
        iin->read_image(
            0, 0, 0, 4, // Sub-image 0, Mip-level 0, channels [0, 4) (RGB)
            OIIO::TypeDescFromC<T>::value(), imageBuf.data(),
            OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride,
            nullptr, nullptr
        );
        return imageBuf;
    }

    template <class T>
    Expected<void, ImageIOError> writeImageBufToFile(const Path &path, const ImageBuf<T> &imageBuf) {
        auto iout = OIIO::ImageOutput::create(path.string());
        OIIO::ImageSpec spec(imageBuf.width(), imageBuf.height(), 3, OIIO::TypeDescFromC<T>::value());
        if (!iout->open(path.string(), spec)) {
            return Unexpected(ImageIOError(path, OIIO::geterror()));
        }
        if (!iout->write_image(OIIO::TypeDescFromC<T>::value(), imageBuf.data())) {
            return Unexpected(ImageIOError(path, OIIO::geterror()));
        }
        iout->close();
        return success;
    }

    inline Expected<void, ImageIOError> writeMaskToFile(const Path &path, const Mask &mask) {
        auto iout = OIIO::ImageOutput::create(path.string());
        OIIO::ImageSpec spec(mask.width(), mask.height(), 1, OIIO::TypeDescFromC<F32>::value());
        if (!iout->open(path.string(), spec)) {
            return Unexpected(ImageIOError(path, OIIO::geterror()));
        }
        if (!iout->write_image(OIIO::TypeDescFromC<F32>::value(), mask.data())) {
            return Unexpected(ImageIOError(path, OIIO::geterror()));
        }
        iout->close();
        return success;
    }

}
