#include "CompositionManager.hpp"

#include <cassert>
#include <iostream>

#include <image/IO.hpp>
#include <image/opencl/Manager.hpp>

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
  , lookFiltersModel_(new LookFiltersModel()) {
    connect(lookFiltersModel_, &LookFiltersModel::lookUpdated, this, [this] { process(); });
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

    lookFiltersModel_->setLook(composition_->look);
    emit lookChanged();
}

void CompositionManager::exportImage(const QString &qPath) noexcept {
    std::cerr << "[CompositionManager] Exporting image to: " << qPath.toStdString() << "\n";
    Path path = qPath.toStdString();
    writeImageBufToFile(path, output_);
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
        processor_ = std::make_shared<Processor>(composition_ && isLookEnabled ? composition_->look : nullptr);
        processor_->init();
    } else {
        processor_->look = composition_ && isLookEnabled ? composition_->look : nullptr;
    }
}

void CompositionManager::process() noexcept {
    assert(composition_);
    assert(processor_);
    processor_->update();
    processor_->process(*composition_->inputImage.data, output_);
    // TODO: Read back from OpenCL here? Currently process() handles that for us.
    emit imageChanged();
}

void CompositionManager::setLookEnabled(bool isEnabled) noexcept {
    isLookEnabled = isEnabled;
    if (composition_ && processor_) {
        processor_->look = isEnabled ? composition_->look : nullptr;
        process();
    }
}
