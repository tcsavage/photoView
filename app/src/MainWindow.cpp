#include "MainWindow.hpp"

#include <QAction>
#include <QApplication>
#include <QVBoxLayout>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMenuBar>
#include <QMimeData>
#include <QSlider>
#include <QStandardPaths>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>
#include <QUrl>

#include "HaldImageDialog.hpp"

MainWindow::MainWindow() {
    qDebug() << "Constructing MainWindow";
    setupProcessor();
    setupMainWidget();
    setupDialogs();
    setupActions();
    setupMenus();
    setupToolBars();
    clear();
    setAcceptDrops(true);
}

void MainWindow::clear() {
    imageView->clear();
    setWindowTitle("Photo View");
}

void MainWindow::setupMainWidget() {
    imageView = new ImageView();
    setCentralWidget(imageView);
    imageView->setLayout(new QVBoxLayout());

    processingIndicator = new ProcessingIndicator();
    imageView->layout()->addWidget(processingIndicator);
    imageView->layout()->setAlignment(Qt::AlignCenter);
    processingIndicator->setVisible(false);
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
    exportImageDialog->setNameFilter("Images (*.jpg *.jpeg *.png)");
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

    openLutAction = new QAction("Open &LUT", this);
    openLutAction->setShortcuts(QKeySequence::Open);
    connect(openLutAction, &QAction::triggered, this, [this] { openLutDialog->show(); });

    toggleShowOriginalAction = new QAction("Show original", this);
    toggleShowOriginalAction->setCheckable(true);
    toggleShowOriginalAction->setChecked(false);
    connect(toggleShowOriginalAction, &QAction::toggled, this, [this](bool showOriginal) {
        processor.setProcessingEnabled(!showOriginal);
        updateImageView();
    });

    nextImageAction = new QAction("&Next", this);
    nextImageAction->setEnabled(false);
    nextImageAction->setShortcut(QKeySequence::MoveToNextChar);
    connect(nextImageAction, &QAction::triggered, this, [this] {
        if (auto next = openFileState.next()) {
            openImage(QString::fromStdString(next->string()));
        }
    });

    prevImageAction = new QAction("&Previous", this);
    prevImageAction->setEnabled(false);
    prevImageAction->setShortcut(QKeySequence::MoveToPreviousChar);
    connect(prevImageAction, &QAction::triggered, this, [this] {
        if (auto prev = openFileState.prev()) {
            openImage(QString::fromStdString(prev->string()));
        }
    });

    loadHaldImageAction = new QAction("Load Hald image", this);
    connect(loadHaldImageAction, &QAction::triggered, this, [this]{
        HaldImageDialog dialog(this);
        if (auto r = dialog.exec()) {
            processorController->loadHaldImage(r);
        }
    });

    quitAction = new QAction("&Quit", this);
    quitAction->setShortcuts(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QApplication::quit);
}

void MainWindow::setupMenus() {
    // File
    auto fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openImageAction);
    fileMenu->addAction(loadHaldImageAction);
    fileMenu->addAction(quitAction);
}

void MainWindow::setupToolBars() {
    {
        auto toolBar = this->addToolBar("Image");
        toolBar->addAction(openImageAction);
        toolBar->addAction(exportImageAction);
        toolBar->addAction(prevImageAction);
        toolBar->addAction(nextImageAction);
    }
    
    {
        auto toolBar = this->addToolBar("LUT");
        toolBar->addAction(openLutAction);
        openLutFileText = new QLineEdit();
        openLutFileText->setEnabled(false);
        openLutFileText->setMaximumWidth(200);
        toolBar->addWidget(openLutFileText);

        auto lutMixSlider = new QSlider(Qt::Orientation::Horizontal);
        lutMixSlider->setRange(0, 100);
        lutMixSlider->setValue(100);
        toolBar->addWidget(lutMixSlider);
        connect(lutMixSlider, &QSlider::valueChanged, this, [this](int value) {
            processor.setLutStrengthFactor(static_cast<image::F32>(value) / 100.0f);
            updateImageView();
        });
    }

    {
        auto toolBar = this->addToolBar("Processing");
        toolBar->addAction(toggleShowOriginalAction);
    }

    {
        auto toolBar = this->addToolBar("Exposure");
        auto exposureSlider = new QSlider(Qt::Orientation::Horizontal);
        exposureSlider->setRange(-30, 30);
        exposureSlider->setValue(0);
        toolBar->addWidget(exposureSlider);
        connect(exposureSlider, &QSlider::valueChanged, this, [this](int value) {
            processor.setExposure(static_cast<image::F32>(value) / 30.0f);
            updateImageView();
        });
    }
}

void MainWindow::setupProcessor() {
    processorController = new ProcessorController(processor);
    connect(processorController, &ProcessorController::imageChanged, this, [this] { updateImageView(); });
    connect(processorController, &ProcessorController::imageOpened, this, &MainWindow::imageOpened);
    connect(processorController, &ProcessorController::failedToOpenImage, this, &MainWindow::failedToOpenImage);
    connect(processorController, &ProcessorController::lutOpened, this, &MainWindow::lutOpened);
}

void MainWindow::openImage(const QString &pathStr) {
    processorController->openImage(pathStr);
    processingIndicator->setVisible(true);
    openFileState.setOpenFile(image::Path { pathStr.toStdString() });
}

void MainWindow::openLut(const QString &pathStr) {
    processorController->openLut(pathStr);
    processingIndicator->setVisible(true);
}

void MainWindow::exportImage(const QString &pathStr) {
    processorController->exportImage(pathStr);
}

void MainWindow::imageOpened(const QString &pathStr) {
    image::Path path = pathStr.toStdString();
    setWindowTitle(QString::fromStdString(path.filename()));
    processingIndicator->setVisible(false);
    prevImageAction->setEnabled(true);
    nextImageAction->setEnabled(true);
}

void MainWindow::failedToOpenImage(const QString &pathStr) {
    image::Path path = pathStr.toStdString();
    setWindowTitle(QString::fromStdString(path.filename()));
    processingIndicator->setVisible(false);
    imageView->clear();
}

void MainWindow::lutOpened(const QString &pathStr) {
    image::Path path = pathStr.toStdString();
    openLutFileText->setText(QString::fromStdString(path.filename()));
    processingIndicator->setVisible(false);
}

void MainWindow::updateImageView(bool) {
    auto &img = processor.proc.output;
    QSize size { static_cast<int>(img.width()), static_cast<int>(img.height()) };
    imageView->load(size, img.data());
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
