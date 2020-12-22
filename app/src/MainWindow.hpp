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

    void openImage(const QString &path);
    void openLut(const QString &path);

    void updateImageView();

private:
    Processor processor;
    
    ImageView *imageView;
    QLineEdit *openLutFileText;

    QAction *openImageAction;
    QAction *exportImageAction;
    QAction *openLutAction;
    QAction *quitAction;

    QFileDialog *openImageDialog;
    QFileDialog *exportImageDialog;
    QFileDialog *openLutDialog;
};
