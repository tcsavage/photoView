#include <app/PhotoWindow.hpp>

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDockWidget>
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
#include <QVBoxLayout>

PhotoWindow::PhotoWindow() {
    qDebug() << "Constructing PhotoWindow";
    setupProcessor();
    setupMainWidget();
    setupDialogs();
    setupActions();
    setupMenus();
    setupToolBars();
    setupDockWidgets();
    clear();
}

PhotoWindow::~PhotoWindow() {
    processorThread.terminate();
    processorThread.wait();
}

void PhotoWindow::clear() {
    canvasScene->clearImage();
    setWindowTitle("Photo View");
}

void PhotoWindow::setupMainWidget() {
    canvasView = new CanvasView(this);
    canvasScene = new CanvasScene(this);
    canvasView->setScene(canvasScene);
    canvasView->fitInView(canvasScene->itemsBoundingRect(), Qt::AspectRatioMode::KeepAspectRatio);
    setCentralWidget(canvasView);

    processingIndicator = new ProcessingIndicator();
    processingIndicator->setVisible(false);
    canvasView->setLayout(new QVBoxLayout());
    canvasView->layout()->addWidget(processingIndicator);
    canvasView->layout()->setAlignment(Qt::AlignCenter);

    activeMaskManager = std::make_unique<MaskManager>(canvasScene, nullptr, nullptr);
    connect(activeMaskManager.get(), &MaskManager::maskUpdated, this, [this] {
        if (auto maskGen = activeMaskManager->maskGen()) {
            compositionManager->notifyMaskChanged(maskGen);
        }
    });
    connect(compositionManager, &CompositionManager::maskGenerated, activeMaskManager.get(), &MaskManager::handleMaskGenerated);
}

void PhotoWindow::setupDialogs() {
    // "Open-image" dialog.
    openImageDialog = new QFileDialog(this, "Open image");
    openImageDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    openImageDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    openImageDialog->setNameFilters({ "Images (*.nef *.jpg *.jpeg)", "All files (*)" });
    openImageDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    connect(openImageDialog, &QFileDialog::fileSelected, compositionManager, &CompositionManager::openImage);

    exportImageDialog = new QFileDialog(this, "Export image");
    exportImageDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    exportImageDialog->setFileMode(QFileDialog::FileMode::AnyFile);
    exportImageDialog->setNameFilter("Images (*.jpg *.jpeg *.png)");
    exportImageDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    connect(exportImageDialog, &QFileDialog::fileSelected, compositionManager, &CompositionManager::exportImage);

    openCompositionDialog = new QFileDialog(this, "Open composition");
    openCompositionDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    openCompositionDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
    openCompositionDialog->setNameFilter("Compositions (*.comp)");
    openCompositionDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    connect(openCompositionDialog, &QFileDialog::fileSelected, compositionManager, &CompositionManager::openComposition);

    saveCompositionDialog = new QFileDialog(this, "Save composition");
    saveCompositionDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    saveCompositionDialog->setFileMode(QFileDialog::FileMode::AnyFile);
    saveCompositionDialog->setNameFilter("Compositions (*.comp)");
    saveCompositionDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    connect(saveCompositionDialog, &QFileDialog::fileSelected, compositionManager, &CompositionManager::saveComposition);
}

void PhotoWindow::setupActions() {
    newWindowAction = new QAction("&New Window", this);
    connect(newWindowAction, &QAction::triggered, this, [] {
        auto wnd = new PhotoWindow();
        wnd->show();
    });

    openImageAction = new QAction("&Open Image...", this);
    openImageAction->setShortcuts(QKeySequence::Open);
    openImageAction->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DialogOpenButton));
    connect(openImageAction, &QAction::triggered, this, [this] { openImageDialog->show(); });

    exportImageAction = new QAction("&Export Image...", this);
    connect(exportImageAction, &QAction::triggered, this, [this] { exportImageDialog->show(); });

    openCompositionAction = new QAction("&Open Composition...", this);
    openCompositionAction->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DialogOpenButton));
    connect(openCompositionAction, &QAction::triggered, this, [this] { openCompositionDialog->show(); });

    saveCompositionAction = new QAction("&Save Composition", this);
    saveCompositionAction->setEnabled(false);
    saveCompositionAction->setShortcuts(QKeySequence::Save);
    saveCompositionAction->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DialogSaveButton));
    connect(saveCompositionAction, &QAction::triggered, this, [this] {
        if (isCompositionFromFile) {
            saveComposition();
        } else {
            saveCompositionDialog->show();
        }
    });

    saveCompositionAsAction = new QAction("Save Composition &As...", this);
    saveCompositionAsAction->setEnabled(false);
    saveCompositionAsAction->setShortcuts(QKeySequence::SaveAs);
    connect(saveCompositionAsAction, &QAction::triggered, this, [this] { saveCompositionDialog->show(); });

    quitAction = new QAction("&Quit", this);
    quitAction->setShortcuts(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QApplication::quit);

    toggleShowMaskOverlayAction = new QAction("&Show Mask Overlay", this);
    toggleShowMaskOverlayAction->setCheckable(true);
    connect(toggleShowMaskOverlayAction, &QAction::toggled, activeMaskManager.get(), &MaskManager::setOverlayEnabled);
}

void PhotoWindow::setupMenus() {
    // File
    auto fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(newWindowAction);
    fileMenu->addSeparator();
    fileMenu->addAction(openImageAction);
    fileMenu->addAction(exportImageAction);
    fileMenu->addSeparator();
    fileMenu->addAction(openCompositionAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveCompositionAction);
    fileMenu->addAction(saveCompositionAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    // View
    viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(toggleShowMaskOverlayAction);
    viewMenu->addSeparator();
    // Dock widgets will add themselves here.
}

void PhotoWindow::setupToolBars() {
    {
        auto toolBar = this->addToolBar("Image");
        toolBar->addAction(openImageAction);
        toolBar->addAction(exportImageAction);
    }
}

void PhotoWindow::setupDockWidgets() {
    {
        auto dock = new QDockWidget(tr("Histogram"), this);
        dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        histogram = new Histogram();
        // QDockWidget doesn't respect the size policy. So lets just set a max height for now.
        histogram->setMaximumHeight(150);
        dock->setWidget(histogram);
        addDockWidget(Qt::RightDockWidgetArea, dock);
        viewMenu->addAction(dock->toggleViewAction());
    }
    
    {
        auto dock = new QDockWidget(tr("Composition"), this);
        dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        compositionOutline = new CompositionOutline();
        compositionOutline->setCompositionModel(compositionManager->compositionModel());
        dock->setWidget(compositionOutline);
        addDockWidget(Qt::RightDockWidgetArea, dock);
        viewMenu->addAction(dock->toggleViewAction());

        connect(compositionOutline,
                &CompositionOutline::filtersEnabledChanged,
                compositionManager,
                &CompositionManager::setFiltersEnabled);

        connect(compositionOutline, &CompositionOutline::activeMaskChanged, this, [this](image::AbstractMaskGenerator *maskGen) {
            activeMaskManager->setMask(maskGen);
            // This is a little bit hacky. Basically we need to get the generated mask buffer for the new active mask
            // generator (if any) and give it to the mask manager to display in the overlay.
            if (maskGen) {
                activeMaskManager->handleMaskGenerated(maskGen, &compositionManager->processor()->state.mask(maskGen));
            }
        });
    }
}

void PhotoWindow::setupProcessor() {
    // Initialise empty composition manager and move to own thread.
    compositionManager = new CompositionManager();
    compositionManager->moveToThread(&processorThread);
    processorThread.start();

    // Connect to composition manager signals.
    connect(compositionManager, &CompositionManager::imageStartedLoading, this, [this](const QString &) {
        processingIndicator->setVisible(true);
        activeMaskManager->setMask(nullptr);
    });
    connect(compositionManager, &CompositionManager::imageLoaded, this, &PhotoWindow::imageOpened);
    connect(compositionManager, &CompositionManager::imageChanged, this, &PhotoWindow::updateImageView);
    connect(compositionManager, &CompositionManager::compositionChanged, this, [this] {
        saveCompositionAction->setEnabled(true);
        saveCompositionAsAction->setEnabled(true);
    });
    connect(compositionManager, &CompositionManager::compositionPathChanged, this, [this](const QString &qPath) {
        image::Path path = qPath.toStdString();
        setWindowTitle(QString::fromStdString(path.filename()));
        isCompositionFromFile = true;
        compositionPath = qPath;
    });
}

void PhotoWindow::openImage(const QString &pathStr) {
    qDebug() << "[PhotoWindow] openImage called";
    compositionManager->openImage(pathStr);
    processingIndicator->setVisible(true);
}

void PhotoWindow::imageOpened(const QString &pathStr) {
    qDebug() << "[PhotoWindow] imageOpened called";
    image::Path path = pathStr.toStdString();
    setWindowTitle(QString::fromStdString(path.filename()));
    processingIndicator->setVisible(false);
}

void PhotoWindow::failedToOpenImage(const QString &pathStr) {
    qDebug() << "[PhotoWindow] failedToOpenImage called";
    image::Path path = pathStr.toStdString();
    setWindowTitle(QString::fromStdString(path.filename()));
    processingIndicator->setVisible(false);
    canvasScene->clearImage();
}

void PhotoWindow::saveComposition() {
    assert(!compositionPath.isEmpty());
    compositionManager->saveComposition(compositionPath);
}

void PhotoWindow::updateImageView() {
    auto &img = compositionManager->output();
    QSize size { static_cast<int>(img.width()), static_cast<int>(img.height()) };
    canvasScene->setImage(size, img.data());
    histogram->generate(img);
}
