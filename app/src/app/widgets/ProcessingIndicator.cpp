#include <app/widgets/ProcessingIndicator.hpp>

#include <QMargins>
#include <QVBoxLayout>

ProcessingIndicator::ProcessingIndicator(QWidget *parent) : QWidget(parent) {
    setAutoFillBackground(true);

    auto layout = new QVBoxLayout();
    setLayout(layout);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->setContentsMargins(20, 20, 20, 20);

    label = new QLabel(tr("Processing image..."));
    layout->addWidget(label);
}
