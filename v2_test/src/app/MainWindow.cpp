#include <app/MainWindow.hpp>

#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>

#include <app/DataAPI.hpp>
#include <app/QtSupport.hpp>

namespace app {

    MainWindow::MainWindow(QWidget *parent) noexcept : QMainWindow(parent) {
        program = std::make_shared<Program>();
        program->addFunction<AddFn>()->operand = 123;
        program->addFunction<AddFn>()->operand = 321;

        model.setProgram(program);

        auto widget = new QWidget(this);

        auto layout = new QVBoxLayout();
        widget->setLayout(layout);

        auto button = new QToolButton();
        button->setPopupMode(QToolButton::InstantPopup);
        auto addFunctionDefaultAction = new QAction("Add function");
        button->setDefaultAction(addFunctionDefaultAction);
        auto menu = new QMenu();
        menu->addActions(functionsActionsList());
        button->setMenu(menu);
        layout->addWidget(button);

        auto label = new QLabel("Test label");
        layout->addWidget(label);

        treeView = new QTreeView();
        treeView->setHeaderHidden(true);
        layout->addWidget(treeView);

        treeView->setModel(&model);

        setCentralWidget(widget);

        connect(button, &QToolButton::triggered, this, [this](QAction *action) {
            qDebug() << action->data();
        });
    }

}
