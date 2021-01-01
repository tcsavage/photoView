#pragma once

#include <QObject>
#include <QString>

#include "Processor.hpp"

class ProcessorWorker : public QObject {
    Q_OBJECT

public:
    ProcessorWorker(Processor &processor, QObject *parent = nullptr);

    void openImage(const QString &path);
    void exportImage(const QString &path);
    void openLut(const QString &path);

signals:
    void imageOpened(const QString &path);
    void lutOpened(const QString &path);
    void imageChanged();
    void failedToOpenImage(const QString &path);

private:
    Processor &processor;
};
