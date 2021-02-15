#pragma once

#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class FileChooser : public QWidget {
    Q_OBJECT
public:
    explicit FileChooser(QFileDialog *fileDialog = nullptr, QWidget *parent = nullptr) noexcept;

    void setPath(const QString &path) noexcept;

    inline QFileDialog *dialog() noexcept { return fileDialog; }

signals:
    void fileChosen(const QString &path);

private:
    QFileDialog *fileDialog;
    QPushButton *openButton;
    QLineEdit *openFileText;
};
