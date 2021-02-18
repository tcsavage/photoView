#pragma once

#include <QFileDialog>
#include <QMainWindow>
#include <QThread>

#include "ImageView.hpp"
#include "ProcessingIndicator.hpp"
#include "composition/CompositionManager.hpp"
#include "composition/CompositionOutline.hpp"

class PhotoWindow : public QMainWindow {
    Q_OBJECT
public:
    PhotoWindow();
    virtual ~PhotoWindow();

    void clear();

    void setupMainWidget();
    void setupDialogs();
    void setupActions();
    void setupMenus();
    void setupToolBars();
    void setupDockWidgets();
    void setupProcessor();

    void openImage(const QString &path);

    void imageOpened(const QString &path);
    void failedToOpenImage(const QString &path);

    void updateImageView();

private:
    QThread processorThread;
    CompositionManager *compositionManager;

    ImageView *imageView;
    ProcessingIndicator *processingIndicator;
    CompositionOutline *compositionOutline;

    QMenu *viewMenu;

    QAction *openImageAction;
    QAction *exportImageAction;
    QAction *quitAction;

    QFileDialog *openImageDialog;
    QFileDialog *exportImageDialog;
};
