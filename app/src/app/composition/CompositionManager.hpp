#pragma once

#include <memory>

#include <QObject>

#include <image/Composition.hpp>
#include <image/ImageBuf.hpp>
#include <image/Processor.hpp>

#include <app/composition/CompositionModel.hpp>

class CompositionManager final : public QObject {
    Q_OBJECT
public:
    explicit CompositionManager(QObject *parent = nullptr) noexcept;

    /**
     * @brief Load the composition at path
     * 
     * @param path The composition to load
     */
    void openComposition(const QString &path) noexcept;

    /**
     * @brief Saves the composition to path
     * 
     * @param path Where to save the composition
     */
    void saveComposition(const QString &path) noexcept;

    /**
     * @brief Open an image into a new composition.
     *
     * @param path The image to load
     */
    void openImage(const QString &path) noexcept;

    /**
     * @brief Export the output buffer to a file.
     *
     * @param path Where to save the image
     */
    void exportImage(const QString &path) noexcept;

    void notifyMaskChanged(image::AbstractMaskGenerator *maskGen) noexcept;

    void ensureOutput() noexcept;
    void resetProcessor() noexcept;
    void process() noexcept;

    inline std::shared_ptr<image::Composition> composition() noexcept { return composition_; }
    inline std::shared_ptr<image::Processor> processor() noexcept { return processor_; }
    inline image::ImageBuf<image::U8> &output() noexcept { return output_; }
    inline CompositionModel *compositionModel() noexcept { return compositionModel_; }

    void setFiltersEnabled(bool isEnabled) noexcept;

signals:
    /**
     * @brief Emitted when the image is being loaded.
     */
    void imageStartedLoading(const QString &path);

    /**
     * @brief Emitted when the image has finished loading.
     */
    void imageLoaded(const QString &path);

    /**
     * @brief Emitted whenever the image changes.
     */
    void imageChanged();

    /**
     * @brief Emitted whenever the composition changes.
     */
    void compositionChanged();

    /**
     * @brief Emitted when the composition is loaded/saved from/to a file.
     */
    void compositionPathChanged(const QString &path);

    /**
     * @brief Emitted when a mask has been re-generated.
     */
    void maskGenerated(const image::AbstractMaskGenerator *maskGen, const image::Mask *maskBuf);

private:
    std::shared_ptr<image::Composition> composition_;
    std::shared_ptr<image::Processor> processor_;
    image::ImageBuf<image::U8> output_;
    CompositionModel *compositionModel_ { nullptr };
};
