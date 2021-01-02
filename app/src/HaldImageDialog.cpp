#include "HaldImageDialog.hpp"

#include <cmath>

#include <QComboBox>
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include <image/CoreTypes.hpp>

HaldImageDialog::HaldImageDialog(QWidget *parent) : QDialog(parent) {
    auto outerLayout = new QVBoxLayout();
    auto innerLayout = new QFormLayout();
    outerLayout->addLayout(innerLayout);
    setLayout(outerLayout);

    auto sizeSelector = new QComboBox();
    using T = image::U8;
    int sizeBits = sizeof(T) * 8 * 2; // The *2 is required to reach LUT size of 256. Unclear how this scales to other types.
    for (int i = 2; i <= sizeBits; i *= 2) {
        int lutSize = i * i;
        sizeSelector->addItem(QString::number(lutSize), i);
    }
    innerLayout->addRow(tr("LUT size"), sizeSelector);

    auto button = new QPushButton(tr("Accept"));
    outerLayout->addWidget(button);
    connect(button, &QPushButton::clicked, this, [this, sizeSelector] { done(sizeSelector->currentData().toInt()); });
}
