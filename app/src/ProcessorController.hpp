#pragma once

#include <QObject>
#include <QString>
#include <QThread>

#include "Processor.hpp"

class ProcessorController : public QObject {
    Q_OBJECT

public:
    ProcessorController(Processor &processor, QObject *parent = nullptr);
    virtual ~ProcessorController();

signals:
    void openImage(const QString &path);
    void imageOpened(const QString &path);
    void exportImage(const QString &path);
    void openLut(const QString &path);
    void lutOpened(const QString &path);

    void imageChanged();

private:
    QThread thread;
};
