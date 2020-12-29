#include "MainWindow.hpp"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QIcon>
#include <QMenuBar>
#include <QMimeData>
#include <QStandardPaths>
#include <QStatusBar>
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
    setupStatusBar();
    setupProcessor();
    setAcceptDrops(true);
    setWindowIcon(QIcon(":/icons/appIcon"));
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
    exportImageDialog->setNameFilter("Images (*.jpg *.jpeg)");
    exportImageDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    connect(exportImageDialog, &QFileDialog::fileSelected, this, [this] (const QString &path) { exportImage(path); });

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

    toggleShowOriginalAction = new QAction("Show original", this);
    toggleShowOriginalAction->setCheckable(true);
    toggleShowOriginalAction->setChecked(false);
    connect(toggleShowOriginalAction, &QAction::toggled, this, [this](bool showOriginal) { updateImageView(showOriginal); });

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

    {
        auto toolBar = this->addToolBar("Processing");
        toolBar->addAction(toggleShowOriginalAction);
    }
}

void MainWindow::setupStatusBar() {
    statusBar();
}

void MainWindow::setupProcessor() {
    processorController = new ProcessorController(processor);
    connect(processorController, &ProcessorController::imageChanged, this, [this] { updateImageView(); });
    connect(processorController, &ProcessorController::imageOpened, this, &MainWindow::imageOpened);
    connect(processorController, &ProcessorController::lutOpened, this, &MainWindow::lutOpened);
}

void MainWindow::openImage(const QString &pathStr) {
    processorController->openImage(pathStr);
    statusBar()->showMessage("Loading image...");
}

void MainWindow::openLut(const QString &pathStr) {
    processorController->openLut(pathStr);
    statusBar()->showMessage("Loading LUT...");
}

void MainWindow::exportImage(const QString &pathStr) {
    processorController->exportImage(pathStr);
}

void MainWindow::imageOpened(const QString &pathStr) {
    image::Path path = pathStr.toStdString();
    setWindowTitle(QString::fromStdString(path.filename()));
    statusBar()->clearMessage();
}

void MainWindow::lutOpened(const QString &pathStr) {
    image::Path path = pathStr.toStdString();
    openLutFileText->setText(QString::fromStdString(path.filename()));
    statusBar()->clearMessage();
}

void MainWindow::updateImageView(bool showOriginal) {
    qDebug() << "Updating image view";
    auto &img = showOriginal ? processor.originalImage : processor.image;
    auto rawImg = img.reinterpret<image::U8>();
    QSize size { processor.imageWidth, processor.imageHeight };
    imageView->load(size, rawImg.data());
    qDebug() << "Finished updating image view";
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    qDebug() << "Drag enter event: " << event->mimeData()->formats();
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        centralWidget()->setForegroundRole(QPalette::Highlight);
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    qDebug() << "Drop event: " << event->mimeData()->formats();
    auto url = event->mimeData()->urls().first();
    centralWidget()->setForegroundRole(QPalette::Dark);
    openImage(url.toLocalFile());
}
