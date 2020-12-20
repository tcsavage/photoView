#include "MainWindow.hpp"

#include <QAction>
#include <QApplication>
#include <QMenuBar>
#include <QStandardPaths>
#include <QStyle>
#include <QToolBar>
#include <QUrl>

MainWindow::MainWindow() {
    setWindowTitle("Photo LUTs");
    setupDialogs();
    setupActions();
    setupMenus();
    setupToolBars();
}

void MainWindow::setupDialogs() {
    openImageDialog = new QFileDialog(this, "Open image");
    openImageDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    openImageDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    openImageDialog->setNameFilter("Images (*.nef, *.jpg, *.jpeg)");
    openImageDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    connect(openImageDialog, &QFileDialog::urlSelected, this, [this] (const QUrl &url) { openImage(url); });

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

void MainWindow::openImage(const QUrl &) {
    
}

void MainWindow::openLut(const QUrl &url) {
    processor.loadLutFromFile(url.toLocalFile().toStdString());
}
