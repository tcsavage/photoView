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
    imageView->clear();
    setWindowTitle("Photo View");
}

void PhotoWindow::setupMainWidget() {
    imageView = new ImageView();
    setCentralWidget(imageView);
    imageView->setLayout(new QVBoxLayout());

    processingIndicator = new ProcessingIndicator();
    imageView->layout()->addWidget(processingIndicator);
    imageView->layout()->setAlignment(Qt::AlignCenter);
    processingIndicator->setVisible(false);
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
}

void PhotoWindow::setupActions() {
    openImageAction = new QAction("&Open image", this);
    openImageAction->setShortcuts(QKeySequence::Open);
    openImageAction->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DialogOpenButton));
    connect(openImageAction, &QAction::triggered, this, [this] { openImageDialog->show(); });

    exportImageAction = new QAction("&Export image", this);
    exportImageAction->setShortcuts(QKeySequence::SaveAs);
    exportImageAction->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DialogSaveButton));
    connect(exportImageAction, &QAction::triggered, this, [this] { exportImageDialog->show(); });

    quitAction = new QAction("&Quit", this);
    quitAction->setShortcuts(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QApplication::quit);
}

void PhotoWindow::setupMenus() {
    // File
    auto fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openImageAction);
    fileMenu->addAction(exportImageAction);
    fileMenu->addAction(quitAction);

    // View
    viewMenu = menuBar()->addMenu("&View");
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
    auto dock = new QDockWidget(tr("Composition"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    compositionOutline = new CompositionOutline();
    compositionOutline->setCompositionModel(compositionManager->compositionModel());
    dock->setWidget(compositionOutline);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());

    connect(compositionOutline, &CompositionOutline::filtersEnabledChanged, compositionManager, &CompositionManager::setFiltersEnabled);
}

void PhotoWindow::setupProcessor() {
    // Initialise empty composition manager and move to own thread.
    compositionManager = new CompositionManager();
    compositionManager->moveToThread(&processorThread);
    processorThread.start();

    // Connect to composition manager signals.
    connect(compositionManager, &CompositionManager::imageStartedLoading, this, [this](const QString &) {
        processingIndicator->setVisible(true);
    });
    connect(compositionManager, &CompositionManager::imageLoaded, this, &PhotoWindow::imageOpened);
    connect(compositionManager, &CompositionManager::imageChanged, this, &PhotoWindow::updateImageView);
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
    imageView->clear();
}

void PhotoWindow::updateImageView() {
    auto &img = compositionManager->output();
    QSize size { static_cast<int>(img.width()), static_cast<int>(img.height()) };
    imageView->load(size, img.data());
}
