#include "MainWindow.hpp"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QErrorMessage>
#include <QMenuBar>
#include <QStandardPaths>
#include <QStyle>
#include <QToolBar>
#include <QUrl>

#include <image/NDArray.hpp>

MainWindow::MainWindow() {
    qDebug() << "Constructing MainWindow";
    setWindowTitle("Photo LUTs");
    setupMainWidget();
    setupDialogs();
    setupActions();
    setupMenus();
    setupToolBars();
}

void MainWindow::setupMainWidget() {
    imageView = new ImageView();
    setCentralWidget(imageView);
}

void MainWindow::setupDialogs() {
    openImageDialog = new QFileDialog(this, "Open image");
    openImageDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    openImageDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    openImageDialog->setNameFilters({"Images (*.nef *.jpg *.jpeg)", "All files (*)"});
    openImageDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    connect(openImageDialog, &QFileDialog::fileSelected, this, [this] (const QString &path) { openImage(path); });

    exportImageDialog = new QFileDialog(this, "Export image");
    exportImageDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    exportImageDialog->setFileMode(QFileDialog::FileMode::AnyFile);
    exportImageDialog->setNameFilter("Images (*.jpg, *.jpeg)");
    exportImageDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    openLutDialog = new QFileDialog(this, "Open LUT");
    openLutDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    openLutDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    openLutDialog->setNameFilter("3D LUTs (*.cube)");
    openLutDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    connect(openLutDialog, &QFileDialog::fileSelected, this, [this] (const QString &path) { openLut(path); });
}

void MainWindow::setupActions() {
    openImageAction = new QAction("&Open image", this);
    openImageAction->setShortcuts(QKeySequence::Open);
    openImageAction->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DialogOpenButton));
    connect(openImageAction, &QAction::triggered, this, [this] { openImageDialog->show(); });

    exportImageAction = new QAction("&Export image", this);
    exportImageAction->setShortcuts(QKeySequence::SaveAs);
    exportImageAction->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DialogSaveButton));
    connect(exportImageAction, &QAction::triggered, this, [this] { exportImageDialog->show(); });

    openLutAction = new QAction("&Open LUT", this);
    openLutAction->setShortcuts(QKeySequence::Open);
    connect(openLutAction, &QAction::triggered, this, [this] { openLutDialog->show(); });

    quitAction = new QAction("&Quit", this);
    quitAction->setShortcuts(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QApplication::quit);
}

void MainWindow::setupMenus() {
    // File
    auto fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openImageAction);
    fileMenu->addAction(quitAction);
}

void MainWindow::setupToolBars() {
    {
        auto toolBar = this->addToolBar("Image");
        toolBar->addAction(openImageAction);
        toolBar->addAction(exportImageAction);
    }
    
    {
        auto toolBar = this->addToolBar("LUT");
        toolBar->addAction(openLutAction);
        openLutFileText = new QLineEdit();
        openLutFileText->setEnabled(false);
        openLutFileText->setMaximumWidth(200);
        toolBar->addWidget(openLutFileText);
    }
}

void MainWindow::openImage(const QString &pathStr) {
    qDebug() << "Opening requested image:" << pathStr;
    auto loaded = processor.loadImageFromFile(pathStr.toStdString());
    if (!loaded) {
        qDebug() << "Failed to open image:" << pathStr;
        QErrorMessage errMsg { this };
        errMsg.showMessage(tr("Failed to load image"));
        return;
    }
    updateImageView();
}

void MainWindow::openLut(const QString &pathStr) {
    qDebug() << "Opening requested LUT:" << pathStr;
    image::Path path = pathStr.toStdString();
    auto loaded = processor.loadLutFromFile(path);
    if (!loaded) {
        qDebug() << "Failed to open LUT:" << pathStr;
        QErrorMessage errMsg { this };
        errMsg.showMessage(tr("Failed to load LUT"));
        return;
    }
    openLutFileText->setText(QString::fromStdString(path.filename()));
}

void MainWindow::updateImageView() {
    qDebug() << "Updating image view";
    const auto &spec = processor.image.spec();
    image::NDArray<image::U8> arr { image::Shape { 3, static_cast<std::size_t>(spec.width), static_cast<std::size_t>(spec.height) } };
    processor.image.get_pixels(processor.image.roi(), OIIO::TypeDesc::UINT8, arr.data());
    imageView->load(QSize { spec.width, spec.height }, std::span(arr.data(), arr.size()));
    qDebug() << "Finished updating image view";
}
