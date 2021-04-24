#include <app/QtSupport.hpp>

#include <QString>

#include <app/DataAPI.hpp>

using namespace image;

QList<QAction *> functionsActionsList() noexcept {
    QList<QAction *> lst;
    for (auto &&id : FunctionRegistry::registeredIds()) {
        std::string idStr { id };
        auto action = new QAction(QString::fromStdString(String(dynInfo(id).name)));
        action->setData(QString::fromStdString(String(id)));
        lst.push_back(action);
    }
    return lst;
}
