#include <app/filters/FilterWidget.hpp>

#include <QColor>
#include <QDebug>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>
#include <QVBoxLayout>

#include <glm/ext/scalar_constants.hpp>

#include <app/widgets/FileChooser.hpp>

using namespace image;

namespace {

    QString formatEvsLabel(F32 evs) noexcept {
        QString out;
        QTextStream ts(&out);
        ts.setRealNumberPrecision(2);
        ts << Qt::forcesign << Qt::forcepoint << evs << " evs";
        return out;
    }

    // QString formatSignedFloat(F32 value) noexcept {
    //     QString out;
    //     QTextStream ts(&out);
    //     ts.setRealNumberPrecision(2);
    //     ts << Qt::forcesign << Qt::forcepoint << value;
    //     return out;
    // }

    QString formatFloat(F32 value, int precision = 2) noexcept {
        QString out;
        QTextStream ts(&out);
        ts.setRealNumberPrecision(precision);
        ts << Qt::forcepoint << value;
        return out;
    }

}

SimpleSliderFilterWidget::SimpleSliderFilterWidget(QWidget *parent) noexcept : FilterWidget(parent) {}

void SimpleSliderFilterWidget::setup() noexcept {
    auto layout = new QVBoxLayout();
    setLayout(layout);

    auto topLayout = new QHBoxLayout();
    layout->addLayout(topLayout);

    widgetLabel = new QLabel(title());
    topLayout->addWidget(widgetLabel);

    topLayout->addStretch();

    valueLabel = new QLabel();
    topLayout->addWidget(valueLabel);

    slider = new FloatSlider();
    slider->setRange(this->minValue(), this->maxValue());
    slider->setDefault(this->defaultValue());
    slider->setTickInterval(this->tickInterval());
    slider->setTickPosition(QSlider::TickPosition::TicksBelow);
    slider->setFixedWidth(300);
    layout->addWidget(slider);

    connect(slider, &FloatSlider::valueChanged, this, &SimpleSliderFilterWidget::valueChanged);

    setValue(this->defaultValue());

    connect(this, &SimpleSliderFilterWidget::valueChanged, this, [this](F32 value) {
        valueLabel->setText(formatValueLabel(value));
    });
}

void SimpleSliderFilterWidget::setValue(image::F32 value) noexcept { slider->setValue(value); }

QString SimpleSliderFilterWidget::formatValueLabel(F32 value) const noexcept { return formatFloat(value); }

ExposureFilterWidget::ExposureFilterWidget(QWidget *parent) noexcept : SimpleSliderFilterWidget(parent) {
    setup();
    connect(this, &SimpleSliderFilterWidget::valueChanged, this, [this](F32 value) {
        valueLabel->setText(formatValueLabel(value));
        if (filter_) {
            filter_->exposureEvs = value;
            filter_->update();
            emit filterUpdated();
        }
    });
}

QString ExposureFilterWidget::formatValueLabel(image::F32 value) const noexcept { return formatEvsLabel(value); }

void ExposureFilterWidget::setFilter(image::AbstractFilterSpec *filter) noexcept {
    filter_ = static_cast<ExposureFilterSpec *>(filter);
    if (filter_) { setValue(filter_->exposureEvs); }
}

LutFilterWidget::LutFilterWidget(QWidget *parent) noexcept : FilterWidget(parent) {
    auto layout = new QFormLayout();
    setLayout(layout);

    auto fileDialog = new QFileDialog(this, "Open LUT");
    fileDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    fileDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    fileDialog->setNameFilter("3D LUTs (*.cube)");
    fileDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    fileChooser = new FileChooser(fileDialog, this);
    layout->addRow("3D Lut file", fileChooser);

    slider = new FloatSlider();
    slider->setRange(0.0f, 1.0f);
    slider->setDefault(1.0f);
    layout->addRow("Strength", slider);

    connect(fileChooser, &FileChooser::fileChosen, this, [this](const QString &path) {
        qDebug() << "Loading LUT into filter:" << path;
        filter_->lut.setPath(path.toStdString());
        filter_->lut.load();
        // TODO; Should check for success.
        filter_->update();
        emit filterUpdated();
    });

    connect(slider, &FloatSlider::valueChanged, this, [this](float value) {
        filter_->strength = value;
        // Updating strength doesn't require a filter update.
        emit filterUpdated();
    });
}

void LutFilterWidget::setFilter(image::AbstractFilterSpec *filter) noexcept {
    filter_ = static_cast<LutFilterSpec *>(filter);
    if (filter_) {
        fileChooser->setPath(QString::fromStdString(filter_->lut.filePath.value_or("")));
        slider->setValue(static_cast<int>(filter_->strength * 100.0f));
    }
}

SaturationFilterWidget::SaturationFilterWidget(QWidget *parent) noexcept : SimpleSliderFilterWidget(parent) {
    setup();
    connect(this, &SimpleSliderFilterWidget::valueChanged, this, [this](F32 value) {
        valueLabel->setText(formatValueLabel(value));
        if (filter_) {
            filter_->multiplier = value;
            emit filterUpdated();
        }
    });
}

void SaturationFilterWidget::setFilter(image::AbstractFilterSpec *filter) noexcept {
    filter_ = static_cast<SaturationFilterSpec *>(filter);
    if (filter_) { setValue(filter_->multiplier); }
}

ContrastFilterWidget::ContrastFilterWidget(QWidget *parent) noexcept : SimpleSliderFilterWidget(parent) {
    setup();
    connect(this, &SimpleSliderFilterWidget::valueChanged, this, [this](F32 value) {
        valueLabel->setText(formatValueLabel(value));
        if (filter_) {
            filter_->factor = value;
            emit filterUpdated();
        }
    });
}

void ContrastFilterWidget::setFilter(image::AbstractFilterSpec *filter) noexcept {
    filter_ = static_cast<ContrastFilterSpec *>(filter);
    if (filter_) { setValue(filter_->factor); }
}

ChannelMixerFilterWidget::ChannelMixerFilterWidget(QWidget *parent) noexcept : FilterWidget(parent) {
    auto layout = new QVBoxLayout();
    setLayout(layout);

    float minVal = -2.0f;
    float maxVal = 2.0f;

    // Red.
    {
        auto channelLabel = new QLabel("Red channel", this);
        layout->addWidget(channelLabel);
        auto formLayout = new QFormLayout();
        layout->addLayout(formLayout);

        redOutRedIn = new FloatSlider(this);
        redOutRedIn->setRange(minVal, maxVal);
        redOutRedIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        redOutRedIn->setFixedWidth(300);
        formLayout->addRow("Red", redOutRedIn);

        redOutGreenIn = new FloatSlider(this);
        redOutGreenIn->setRange(minVal, maxVal);
        redOutGreenIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        redOutGreenIn->setFixedWidth(300);
        formLayout->addRow("Green", redOutGreenIn);

        redOutBlueIn = new FloatSlider(this);
        redOutBlueIn->setRange(minVal, maxVal);
        redOutBlueIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        redOutBlueIn->setFixedWidth(300);
        formLayout->addRow("Blue", redOutBlueIn);

        redOutRedIn->setValue(1.0f, true);
        redOutGreenIn->setValue(0.0f, true);
        redOutBlueIn->setValue(0.0f, true);
    }

    // Green.
    {
        auto channelLabel = new QLabel("Green channel", this);
        layout->addWidget(channelLabel);
        auto formLayout = new QFormLayout();
        layout->addLayout(formLayout);

        greenOutRedIn = new FloatSlider(this);
        greenOutRedIn->setRange(minVal, maxVal);
        greenOutRedIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        greenOutRedIn->setFixedWidth(300);
        formLayout->addRow("Red", greenOutRedIn);

        greenOutGreenIn = new FloatSlider(this);
        greenOutGreenIn->setRange(minVal, maxVal);
        greenOutGreenIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        greenOutGreenIn->setFixedWidth(300);
        formLayout->addRow("Green", greenOutGreenIn);

        greenOutBlueIn = new FloatSlider(this);
        greenOutBlueIn->setRange(minVal, maxVal);
        greenOutBlueIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        greenOutBlueIn->setFixedWidth(300);
        formLayout->addRow("Blue", greenOutBlueIn);

        greenOutRedIn->setValue(0.0f, true);
        greenOutGreenIn->setValue(1.0f, true);
        greenOutBlueIn->setValue(0.0f, true);
    }

    // Blue.
    {
        auto channelLabel = new QLabel("Blue channel", this);
        layout->addWidget(channelLabel);
        auto formLayout = new QFormLayout();
        layout->addLayout(formLayout);

        blueOutRedIn = new FloatSlider(this);
        blueOutRedIn->setRange(minVal, maxVal);
        blueOutRedIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        blueOutRedIn->setFixedWidth(300);
        formLayout->addRow("Red", blueOutRedIn);

        blueOutGreenIn = new FloatSlider(this);
        blueOutGreenIn->setRange(minVal, maxVal);
        blueOutGreenIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        blueOutGreenIn->setFixedWidth(300);
        formLayout->addRow("Green", blueOutGreenIn);

        blueOutBlueIn = new FloatSlider(this);
        blueOutBlueIn->setRange(minVal, maxVal);
        blueOutBlueIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        blueOutBlueIn->setFixedWidth(300);
        formLayout->addRow("Blue", blueOutBlueIn);

        blueOutRedIn->setValue(0.0f, true);
        blueOutGreenIn->setValue(0.0f, true);
        blueOutBlueIn->setValue(1.0f, true);
    }

    connect(redOutRedIn, &FloatSlider::valueChanged, this, [this](float value) { handleValueChanged(0, 0, value); });
    connect(redOutGreenIn, &FloatSlider::valueChanged, this, [this](float value) { handleValueChanged(0, 1, value); });
    connect(redOutBlueIn, &FloatSlider::valueChanged, this, [this](float value) { handleValueChanged(0, 2, value); });

    connect(greenOutRedIn, &FloatSlider::valueChanged, this, [this](float value) { handleValueChanged(1, 0, value); });
    connect(greenOutGreenIn, &FloatSlider::valueChanged, this, [this](float value) { handleValueChanged(1, 1, value); });
    connect(greenOutBlueIn, &FloatSlider::valueChanged, this, [this](float value) { handleValueChanged(1, 2, value); });

    connect(blueOutRedIn, &FloatSlider::valueChanged, this, [this](float value) { handleValueChanged(2, 0, value); });
    connect(blueOutGreenIn, &FloatSlider::valueChanged, this, [this](float value) { handleValueChanged(2, 1, value); });
    connect(blueOutBlueIn, &FloatSlider::valueChanged, this, [this](float value) { handleValueChanged(2, 2, value); });

    // Preserve luminosity.
    preserveLuminosity = new QCheckBox(tr("Preserve luminosity"), this);
    layout->addWidget(preserveLuminosity);
    connect(preserveLuminosity, &QCheckBox::toggled, this, [this](bool checked) {
        filter_->preserveLuminosity = checked;
        emit filterUpdated();
    });
}

void ChannelMixerFilterWidget::setFilter(image::AbstractFilterSpec *filter) noexcept {
    filter_ = static_cast<ChannelMixerFilterSpec *>(filter);
    if (filter_) {
        auto &mat = filter_->matrix;
        redOutRedIn->setValue(mat[0][0]);
        redOutGreenIn->setValue(mat[1][0]);
        redOutBlueIn->setValue(mat[2][0]);
        greenOutRedIn->setValue(mat[0][1]);
        greenOutGreenIn->setValue(mat[1][1]);
        greenOutBlueIn->setValue(mat[2][1]);
        blueOutRedIn->setValue(mat[0][2]);
        blueOutGreenIn->setValue(mat[1][2]);
        blueOutBlueIn->setValue(mat[2][2]);
    }
}

void ChannelMixerFilterWidget::handleValueChanged(int row, int column, float value) noexcept {
    if (!filter_) { return; }
    filter_->matrix[column][row] = value;
    emit filterUpdated();
}
