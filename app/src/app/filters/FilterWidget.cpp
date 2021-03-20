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

ExposureFilterWidget::ExposureFilterWidget(image::AbstractFilterSpec *filter, QWidget *parent) noexcept
  : FilterWidget(filter, parent)
  , filter_(reinterpret_cast<ExposureFilterSpec *>(filter)) {
    auto layout = new QVBoxLayout();
    setLayout(layout);

    auto topLayout = new QHBoxLayout();
    layout->addLayout(topLayout);

    auto textLabel = new QLabel(tr("Exposure"));
    topLayout->addWidget(textLabel);

    topLayout->addStretch();

    auto valueLabel = new QLabel(formatEvsLabel(filter_->exposureEvs));
    topLayout->addWidget(valueLabel);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setRange(-(rangeEvs * nDivisions), rangeEvs * nDivisions);
    slider->setTickInterval(nDivisions);
    slider->setTickPosition(QSlider::TickPosition::TicksBelow);
    slider->setValue(static_cast<int>(filter_->exposureEvs * static_cast<F32>(nDivisions)));
    slider->setFixedWidth(300);
    layout->addWidget(slider);

    connect(slider, &QSlider::valueChanged, this, [this, valueLabel](int value) {
        auto exposureEvs = static_cast<F32>(value) / static_cast<F32>(nDivisions);
        valueLabel->setText(formatEvsLabel(exposureEvs));
        filter_->exposureEvs = exposureEvs;
        filter_->update();
        emit filterUpdated();
    });
}

LutFilterWidget::LutFilterWidget(image::AbstractFilterSpec *filter, QWidget *parent) noexcept
  : FilterWidget(filter, parent)
  , filter_(reinterpret_cast<LutFilterSpec *>(filter)) {
    auto layout = new QFormLayout();
    setLayout(layout);

    auto fileDialog = new QFileDialog(this, "Open LUT");
    fileDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    fileDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    fileDialog->setNameFilter("3D LUTs (*.cube)");
    fileDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    auto fileChooser = new FileChooser(fileDialog, this);
    fileChooser->setPath(QString::fromStdString(filter_->lut.filePath.value_or("")));
    layout->addRow("3D Lut file", fileChooser);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setTickInterval(1);
    slider->setValue(static_cast<int>(filter_->strength * 100.0f));
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

SaturationFilterWidget::SaturationFilterWidget(image::AbstractFilterSpec *filter, QWidget *parent) noexcept
  : FilterWidget(filter, parent)
  , filter_(reinterpret_cast<SaturationFilterSpec *>(filter)) {
    auto layout = new QVBoxLayout();
    setLayout(layout);

    auto topLayout = new QHBoxLayout();
    layout->addLayout(topLayout);

    auto textLabel = new QLabel(tr("Saturation"));
    topLayout->addWidget(textLabel);

    topLayout->addStretch();

    auto valueLabel = new QLabel(formatFloat(filter_->multiplier));
    topLayout->addWidget(valueLabel);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 200);
    slider->setTickInterval(10);
    slider->setTickPosition(QSlider::TickPosition::TicksBelow);
    slider->setValue(100);
    slider->setFixedWidth(300);
    layout->addWidget(slider);

    connect(slider, &QSlider::valueChanged, this, [this, valueLabel](int value) {
        auto multiplier = static_cast<F32>(value) / 100;
        valueLabel->setText(formatFloat(multiplier));
        filter_->multiplier = multiplier;
        // filter_->update(); // Not necessary
        emit filterUpdated();
    });
}

ContrastFilterWidget::ContrastFilterWidget(image::AbstractFilterSpec *filter, QWidget *parent) noexcept
  : FilterWidget(filter, parent)
  , filter_(reinterpret_cast<ContrastFilterSpec *>(filter)) {
    auto layout = new QVBoxLayout();
    setLayout(layout);

    auto topLayout = new QHBoxLayout();
    layout->addLayout(topLayout);

    auto textLabel = new QLabel(tr("Contrast"));
    topLayout->addWidget(textLabel);

    topLayout->addStretch();

    auto valueLabel = new QLabel(formatFloat(filter_->factor, 4));
    topLayout->addWidget(valueLabel);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setRange(-63, 64);
    slider->setTickInterval(8);
    slider->setTickPosition(QSlider::TickPosition::TicksBelow);
    slider->setValue(0);
    slider->setFixedWidth(300);
    layout->addWidget(slider);

    connect(slider, &QSlider::valueChanged, this, [this, valueLabel](int value) {
        auto valuef = static_cast<F32>(value) / 64;
        auto factor = std::pow(valuef, 3.0) / 2.0 + 1.0;
        valueLabel->setText(formatFloat(factor, 4));
        filter_->factor = factor;
        // filter_->update(); // Not necessary
        emit filterUpdated();
    });
}
