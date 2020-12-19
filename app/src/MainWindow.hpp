#pragma once

#include <QFileDialog>
#include <QLineEdit>
#include <QMainWindow>

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

private:
    QLineEdit *openLutFileText;

    QAction *openImageAction;
    QAction *exportImageAction;
    QAction *openLutAction;
    QAction *quitAction;

    QFileDialog *openImageDialog;
    QFileDialog *exportImageDialog;
    QFileDialog *openLutDialog;
};
