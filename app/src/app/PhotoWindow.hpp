#pragma once

#include <QFileDialog>
#include <QMainWindow>
#include <QThread>

#include <app/composition/CompositionManager.hpp>
#include <app/composition/CompositionOutline.hpp>
#include <app/widgets/ImageView.hpp>
#include <app/widgets/ProcessingIndicator.hpp>

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