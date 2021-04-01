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

    int minVal = scaleToInt(this->minValue());
    int maxVal = scaleToInt(this->maxValue());

    slider = new Slider(Qt::Horizontal);
    slider->setRange(minVal, maxVal);
    slider->setTickInterval(this->tickInterval());
    slider->setTickPosition(QSlider::TickPosition::TicksBelow);
    slider->setFixedWidth(300);
    layout->addWidget(slider);

    connect(slider, &QSlider::valueChanged, this, [this](int value) { emit valueChanged(scaleToFloat(value)); });

    setValue(this->defaultValue());

    connect(this, &SimpleSliderFilterWidget::valueChanged, this, [this](F32 value) {
        valueLabel->setText(formatValueLabel(value));
    });

    connect(slider, &Slider::doubleClicked, this, [this](QMouseEvent *) { setValue(this->defaultValue()); });
}

void SimpleSliderFilterWidget::setValue(image::F32 value) noexcept { slider->setValue(scaleToInt(value)); }

int SimpleSliderFilterWidget::scaleToInt(image::F32 value) const noexcept {
    auto scaledRange = static_cast<F32>(numSubdivisions()) / (maxValue() - minValue());
    return static_cast<int>(scaledRange * value);
}

image::F32 SimpleSliderFilterWidget::scaleToFloat(int value) const noexcept {
    auto scaledRange = static_cast<F32>(numSubdivisions()) / (maxValue() - minValue());
    return static_cast<F32>(value) / scaledRange;
}

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

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setTickInterval(1);
    layout->addRow("Strength", slider);

    connect(fileChooser, &FileChooser::fileChosen, this, [this](const QString &path) {
        qDebug() << "Loading LUT into filter:" << path;
        filter_->lut.setPath(path.toStdString());
        filter_->lut.load();
        // TODO; Should check for success.
        filter_->update();
        emit filterUpdated();
    });

    connect(slider, &QSlider::valueChanged, this, [this](int value) {
        auto strength = static_cast<F32>(value) / 100.0f;
        filter_->strength = strength;
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

    int minVal = scaleToInt(this->minValue());
    int maxVal = scaleToInt(this->maxValue());

    // Red.
    {
        auto channelLabel = new QLabel("Red channel", this);
        layout->addWidget(channelLabel);
        auto formLayout = new QFormLayout();
        layout->addLayout(formLayout);

        redOutRedIn = new QSlider(Qt::Orientation::Horizontal, this);
        redOutRedIn->setRange(minVal, maxVal);
        redOutRedIn->setTickInterval(this->tickInterval());
        redOutRedIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        redOutRedIn->setFixedWidth(300);
        formLayout->addRow("Red", redOutRedIn);

        redOutGreenIn = new QSlider(Qt::Orientation::Horizontal, this);
        redOutGreenIn->setRange(minVal, maxVal);
        redOutGreenIn->setTickInterval(this->tickInterval());
        redOutGreenIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        redOutGreenIn->setFixedWidth(300);
        formLayout->addRow("Green", redOutGreenIn);

        redOutBlueIn = new QSlider(Qt::Orientation::Horizontal, this);
        redOutBlueIn->setRange(minVal, maxVal);
        redOutBlueIn->setTickInterval(this->tickInterval());
        redOutBlueIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        redOutBlueIn->setFixedWidth(300);
        formLayout->addRow("Blue", redOutBlueIn);

        redOutRedIn->setValue(100);
        redOutGreenIn->setValue(0);
        redOutBlueIn->setValue(0);
    }

    // Green.
    {
        auto channelLabel = new QLabel("Green channel", this);
        layout->addWidget(channelLabel);
        auto formLayout = new QFormLayout();
        layout->addLayout(formLayout);

        greenOutRedIn = new QSlider(Qt::Orientation::Horizontal, this);
        greenOutRedIn->setRange(minVal, maxVal);
        greenOutRedIn->setTickInterval(this->tickInterval());
        greenOutRedIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        greenOutRedIn->setFixedWidth(300);
        formLayout->addRow("Red", greenOutRedIn);

        greenOutGreenIn = new QSlider(Qt::Orientation::Horizontal, this);
        greenOutGreenIn->setRange(minVal, maxVal);
        greenOutGreenIn->setTickInterval(this->tickInterval());
        greenOutGreenIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        greenOutGreenIn->setFixedWidth(300);
        formLayout->addRow("Green", greenOutGreenIn);

        greenOutBlueIn = new QSlider(Qt::Orientation::Horizontal, this);
        greenOutBlueIn->setRange(minVal, maxVal);
        greenOutBlueIn->setTickInterval(this->tickInterval());
        greenOutBlueIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        greenOutBlueIn->setFixedWidth(300);
        formLayout->addRow("Blue", greenOutBlueIn);

        greenOutRedIn->setValue(0);
        greenOutGreenIn->setValue(100);
        greenOutBlueIn->setValue(0);
    }

    // Blue.
    {
        auto channelLabel = new QLabel("Blue channel", this);
        layout->addWidget(channelLabel);
        auto formLayout = new QFormLayout();
        layout->addLayout(formLayout);

        blueOutRedIn = new QSlider(Qt::Orientation::Horizontal, this);
        blueOutRedIn->setRange(minVal, maxVal);
        blueOutRedIn->setTickInterval(this->tickInterval());
        blueOutRedIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        blueOutRedIn->setFixedWidth(300);
        formLayout->addRow("Red", blueOutRedIn);

        blueOutGreenIn = new QSlider(Qt::Orientation::Horizontal, this);
        blueOutGreenIn->setRange(minVal, maxVal);
        blueOutGreenIn->setTickInterval(this->tickInterval());
        blueOutGreenIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        blueOutGreenIn->setFixedWidth(300);
        formLayout->addRow("Green", blueOutGreenIn);

        blueOutBlueIn = new QSlider(Qt::Orientation::Horizontal, this);
        blueOutBlueIn->setRange(minVal, maxVal);
        blueOutBlueIn->setTickInterval(this->tickInterval());
        blueOutBlueIn->setTickPosition(QSlider::TickPosition::TicksBelow);
        blueOutBlueIn->setFixedWidth(300);
        formLayout->addRow("Blue", blueOutBlueIn);

        blueOutRedIn->setValue(0);
        blueOutGreenIn->setValue(0);
        blueOutBlueIn->setValue(100);
    }

    connect(redOutRedIn, &QSlider::valueChanged, this, [this](int value) { handleValueChanged(0, 0, value); });
    connect(redOutGreenIn, &QSlider::valueChanged, this, [this](int value) { handleValueChanged(0, 1, value); });
    connect(redOutBlueIn, &QSlider::valueChanged, this, [this](int value) { handleValueChanged(0, 2, value); });

    connect(greenOutRedIn, &QSlider::valueChanged, this, [this](int value) { handleValueChanged(1, 0, value); });
    connect(greenOutGreenIn, &QSlider::valueChanged, this, [this](int value) { handleValueChanged(1, 1, value); });
    connect(greenOutBlueIn, &QSlider::valueChanged, this, [this](int value) { handleValueChanged(1, 2, value); });

    connect(blueOutRedIn, &QSlider::valueChanged, this, [this](int value) { handleValueChanged(2, 0, value); });
    connect(blueOutGreenIn, &QSlider::valueChanged, this, [this](int value) { handleValueChanged(2, 1, value); });
    connect(blueOutBlueIn, &QSlider::valueChanged, this, [this](int value) { handleValueChanged(2, 2, value); });

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
        redOutRedIn->setValue(scaleToInt(mat[0][0]));
        redOutGreenIn->setValue(scaleToInt(mat[1][0]));
        redOutBlueIn->setValue(scaleToInt(mat[2][0]));
        greenOutRedIn->setValue(scaleToInt(mat[0][1]));
        greenOutGreenIn->setValue(scaleToInt(mat[1][1]));
        greenOutBlueIn->setValue(scaleToInt(mat[2][1]));
        blueOutRedIn->setValue(scaleToInt(mat[0][2]));
        blueOutGreenIn->setValue(scaleToInt(mat[1][2]));
        blueOutBlueIn->setValue(scaleToInt(mat[2][2]));
    }
}

void ChannelMixerFilterWidget::handleValueChanged(int row, int column, int value) noexcept {
    if (!filter_) { return; }
    F32 valuef = scaleToFloat(value);
    filter_->matrix[column][row] = valuef;
    emit filterUpdated();
}
