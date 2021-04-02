#include <app/composition/CompositionManager.hpp>

#include <cassert>
#include <iostream>

#include <image/Serialization.hpp>
#include <image/IO.hpp>
#include <image/opencl/Manager.hpp>

#include <app/masks/MaskGenerators.hpp>

using namespace image;

namespace {

    template <class T>
    void allocOpenCL(ImageBuf<T> &img) noexcept {
        img.pixelArray.buffer()->setDevice(opencl::Manager::the()->bufferDevice);
        img.pixelArray.buffer()->deviceMalloc();
    }

    template <class T>
    void writeToOpenCL(ImageBuf<T> &img) noexcept {
        img.pixelArray.buffer()->copyHostToDevice();
    }

    template <class T>
    void readFrom(ImageBuf<T> &img) noexcept {
        img.pixelArray.buffer()->copyDeviceToHost();
    }

}

CompositionManager::CompositionManager(QObject *parent) noexcept
  : QObject(parent)
  , compositionModel_(new CompositionModel()) {
    connect(compositionModel_, &CompositionModel::compositionUpdated, this, [this] { process(); });
    connect(compositionModel_, &CompositionModel::maskChanged, this, &CompositionManager::notifyMaskChanged);
}

void CompositionManager::openComposition(const QString &qPath) noexcept {
    emit imageStartedLoading(qPath);
    std::cerr << "[CompositionManager] Opening composition: " << qPath.toStdString() << "\n";
    Path path = qPath.toStdString();
    auto compResult = serialization::loadFromFile(path, &filterRegistry, &maskGeneratorRegistry);
    if (compResult.hasError()) {
        std::cerr << "[CompositionManager] Error opening composition: " << compResult.error().message << "\n";
        // TODO: Error state
        return;
    }
    composition_ = std::make_shared<Composition>(std::move(compResult.value()));
    allocOpenCL(*composition_->inputImage.data);
    writeToOpenCL(*composition_->inputImage.data);
    ensureOutput();
    emit imageLoaded(qPath);

    resetProcessor();
    process();

    compositionModel_->setComposition(composition_);
    emit compositionChanged();
    emit compositionPathChanged(qPath);
}

void CompositionManager::saveComposition(const QString &qPath) noexcept {
    Path path = qPath.toStdString();
    serialization::saveToFile(path, *composition_);

    emit compositionPathChanged(qPath);
}

void CompositionManager::openImage(const QString &qPath) noexcept {
    emit imageStartedLoading(qPath);
    std::cerr << "[CompositionManager] Opening image: " << qPath.toStdString() << "\n";
    Path path = qPath.toStdString();
    auto compResult = Composition::newFromPath(path);
    composition_ = std::make_shared<Composition>(std::move(compResult.value()));
    allocOpenCL(*composition_->inputImage.data);
    writeToOpenCL(*composition_->inputImage.data);
    ensureOutput();
    emit imageLoaded(qPath);

    resetProcessor();
    process();

    compositionModel_->setComposition(composition_);
    emit compositionChanged();
}

void CompositionManager::exportImage(const QString &qPath) noexcept {
    std::cerr << "[CompositionManager] Exporting image to: " << qPath.toStdString() << "\n";
    Path path = qPath.toStdString();
    writeImageBufToFile(path, output_);
}

void CompositionManager::notifyMaskChanged(image::GeneratedMask *mask) noexcept {
    mask->update(*composition_->inputImage.data);
    process();
}

void CompositionManager::ensureOutput() noexcept {
    assert(composition_);
    assert(composition_->inputImage.data);
    auto &input = *composition_->inputImage.data;
    if (output_.width() != input.width() || output_.height() != input.height()) {
        output_ = ImageBuf<U8> { input.width(), input.height() };
        allocOpenCL(output_);
    }
}

void CompositionManager::resetProcessor() noexcept {
    if (!processor_) {
        processor_ = std::make_shared<Processor>();
        processor_->init();
    }
    processor_->setComposition(composition_);
}

void CompositionManager::process() noexcept {
    assert(composition_);
    assert(processor_);
    processor_->update();
    processor_->process(output_);
    // TODO: Read back from OpenCL here? Currently process() handles that for us.
    emit imageChanged();
}

void CompositionManager::setFiltersEnabled(bool isEnabled) noexcept {
    if (composition_ && processor_) {
        processor_->areFiltersEnabled = isEnabled;
        process();
    }
}
