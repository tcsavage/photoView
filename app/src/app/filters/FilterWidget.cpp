#include <app/filters/FilterWidget.hpp>

#include <QDebug>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>
#include <QVBoxLayout>

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

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(minVal, maxVal);
    slider->setTickInterval(this->tickInterval());
    slider->setTickPosition(QSlider::TickPosition::TicksBelow);
    slider->setFixedWidth(300);
    layout->addWidget(slider);

    connect(slider, &QSlider::valueChanged, this, [this](int value) {
        emit valueChanged(scaleToFloat(value));
    });

    setValue(this->defaultValue());

    connect(this, &SimpleSliderFilterWidget::valueChanged, this, [this](F32 value) {
        valueLabel->setText(formatValueLabel(value));
    });
}

void SimpleSliderFilterWidget::setValue(image::F32 value) noexcept {
    slider->setValue(scaleToInt(value));
}

int SimpleSliderFilterWidget::scaleToInt(image::F32 value) const noexcept {
    auto scaledRange = static_cast<F32>(numSubdivisions()) / (maxValue() - minValue());
    return static_cast<int>(scaledRange * value);
}

image::F32 SimpleSliderFilterWidget::scaleToFloat(int value) const noexcept {
    auto scaledRange = static_cast<F32>(numSubdivisions()) / (maxValue() - minValue());
    return static_cast<F32>(value) / scaledRange;
}

QString SimpleSliderFilterWidget::formatValueLabel(F32 value) const noexcept { return formatFloat(value); }

ExposureFilterWidget::ExposureFilterWidget(QWidget *parent) noexcept
  : SimpleSliderFilterWidget(parent) {
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
    if (filter_) {
        setValue(filter_->exposureEvs);
    }
}

LutFilterWidget::LutFilterWidget(QWidget *parent) noexcept
  : FilterWidget(parent) {
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

SaturationFilterWidget::SaturationFilterWidget(QWidget *parent) noexcept
  : SimpleSliderFilterWidget(parent) {
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
    if (filter_) {
        setValue(filter_->multiplier);
    }
}

ContrastFilterWidget::ContrastFilterWidget(QWidget *parent) noexcept
  : SimpleSliderFilterWidget(parent) {
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
    if (filter_) {
        setValue(filter_->factor);
    }
}
