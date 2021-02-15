#include "FileChooser.hpp"

#include <QHBoxLayout>

FileChooser::FileChooser(QFileDialog *fd, QWidget *parent) noexcept : QWidget(parent) {
    fileDialog = fd ? fd : new QFileDialog();
    fileDialog->setParent(this);
    openButton = new QPushButton("Open", this);
    openFileText = new QLineEdit(this);
    openFileText->setEnabled(false);

    auto layout = new QHBoxLayout();
    setLayout(layout);
    layout->addWidget(openButton);
    layout->addWidget(openFileText);

    connect(openButton, &QAbstractButton::clicked, this, [this](bool) { fileDialog->show(); });

    connect(fileDialog, &QFileDialog::fileSelected, this, [this](const QString &path) {
        setPath(path);
        emit fileChosen(path);
    });
}

void FileChooser::setPath(const QString &path) noexcept { openFileText->setText(path); }
