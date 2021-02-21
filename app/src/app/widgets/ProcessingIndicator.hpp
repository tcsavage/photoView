#pragma once

#include <QLabel>
#include <QWidget>

class ProcessingIndicator : public QWidget {
    Q_OBJECT
public:
    ProcessingIndicator(QWidget *parent = nullptr);

private:
    QLabel *label;
};
