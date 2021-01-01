#include "ProcessorController.hpp"

#include "ProcessorWorker.hpp"

ProcessorController::ProcessorController(Processor &processor, QObject *parent) : QObject(parent) {
    auto worker = new ProcessorWorker(processor);
    worker->moveToThread(&thread);
    thread.start();

    connect(this, &ProcessorController::openImage, worker, &ProcessorWorker::openImage, Qt::ConnectionType::QueuedConnection);
    connect(worker, &ProcessorWorker::imageOpened, this, &ProcessorController::imageOpened, Qt::ConnectionType::QueuedConnection);
    connect(worker, &ProcessorWorker::failedToOpenImage, this, &ProcessorController::failedToOpenImage, Qt::ConnectionType::QueuedConnection);

    connect(this, &ProcessorController::openLut, worker, &ProcessorWorker::openLut, Qt::ConnectionType::QueuedConnection);
    connect(worker, &ProcessorWorker::lutOpened, this, &ProcessorController::lutOpened, Qt::ConnectionType::QueuedConnection);

    connect(this, &ProcessorController::exportImage, worker, &ProcessorWorker::exportImage, Qt::ConnectionType::QueuedConnection);

    connect(worker, &ProcessorWorker::imageChanged, this, &ProcessorController::imageChanged, Qt::ConnectionType::QueuedConnection);
}

ProcessorController::~ProcessorController() {
    thread.quit();
    thread.wait();
}
