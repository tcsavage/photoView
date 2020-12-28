#include "ProcessorWorker.hpp"

ProcessorWorker::ProcessorWorker(Processor &processor, QObject *parent)
    : QObject(parent)
    , processor(processor) {}

void ProcessorWorker::openImage(const QString &path) {
    processor.loadImageFromFile(path.toStdString());
    emit imageOpened(path);
    emit imageChanged();
}

void ProcessorWorker::exportImage(const QString &path) {
    processor.exportImageToFile(path.toStdString());
}

void ProcessorWorker::openLut(const QString &path) {
    processor.loadLutFromFile(path.toStdString());
    emit lutOpened(path);
    emit imageChanged();
}
