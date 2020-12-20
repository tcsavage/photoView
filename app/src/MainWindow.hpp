#pragma once

#include <QFileDialog>
#include <QLineEdit>
#include <QMainWindow>

#include "Processor.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

protected:
    void setupDialogs();
    void setupActions();
    void setupMenus();
    void setupToolBars();

    void openImage(const QUrl &url);
    void openLut(const QUrl &url);

private:
    Processor processor;
    
    QLineEdit *openLutFileText;

    QAction *openImageAction;
    QAction *exportImageAction;
    QAction *openLutAction;
    QAction *quitAction;

    QFileDialog *openImageDialog;
    QFileDialog *exportImageDialog;
    QFileDialog *openLutDialog;
};
