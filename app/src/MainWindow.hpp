#pragma once

#include <QFileDialog>
#include <QLineEdit>
#include <QMainWindow>

#include "ImageView.hpp"
#include "Processor.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

protected:
    void setupMainWidget();
    void setupDialogs();
    void setupActions();
    void setupMenus();
    void setupToolBars();
    void setupProcessor();

    void openImage(const QString &path);
    void openLut(const QString &path);
    void exportImage(const QString &path);

    void updateImageView(bool showOriginal = false);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

private:
    Processor *processor { nullptr };
    
    ImageView *imageView;
    QLineEdit *openLutFileText;

    QAction *openImageAction;
    QAction *exportImageAction;
    QAction *openLutAction;
    QAction *toggleShowOriginalAction;
    QAction *quitAction;

    QFileDialog *openImageDialog;
    QFileDialog *exportImageDialog;
    QFileDialog *openLutDialog;
};
