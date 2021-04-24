#pragma once

#include <QMainWindow>
#include <QTreeView>

#include <app/Model.hpp>
#include <app/ProgramModel.hpp>

using namespace image;

namespace app {

    class MainWindow : public QMainWindow {
        Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent = nullptr) noexcept; 

    private:
        QTreeView *treeView { nullptr };

        std::shared_ptr<Program> program;
        ProgramModel model;
    };

}
