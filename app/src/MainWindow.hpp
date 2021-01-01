#pragma once

#include <QFileDialog>
#include <QLineEdit>
#include <QMainWindow>

#include "ImageView.hpp"
#include "OpenFileState.hpp"
#include "ProcessingIndicator.hpp"
#include "Processor.hpp"
#include "ProcessorController.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

    void clear();

    void setupMainWidget();
    void setupDialogs();
    void setupActions();
    void setupMenus();
    void setupToolBars();
    void setupProcessor();

    void openImage(const QString &path);
    void openLut(const QString &path);
    void exportImage(const QString &path);

    void imageOpened(const QString &path);
    void failedToOpenImage(const QString &path);
    void lutOpened(const QString &path);

    void updateImageView(bool showOriginal = false);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

private:
    Processor processor;
    ProcessorController *processorController { nullptr };
    OpenFileState openFileState;
    
    ImageView *imageView;
    ProcessingIndicator *processingIndicator;
    QLineEdit *openLutFileText;

    QAction *openImageAction;
    QAction *exportImageAction;
    QAction *openLutAction;
    QAction *toggleShowOriginalAction;
    QAction *prevImageAction;
    QAction *nextImageAction;
    QAction *quitAction;

    QFileDialog *openImageDialog;
    QFileDialog *exportImageDialog;
    QFileDialog *openLutDialog;
};
