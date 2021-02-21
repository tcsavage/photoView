#pragma once

#include <QMenu>

#include <image/AllFilters.hpp>

inline image::FilterRegistry filterRegistry { image::makeFilterRegistry() };

inline QMenu *makeFilterMenu() noexcept {
    auto menu = new QMenu();
    for (auto &&id : filterRegistry.registeredIds()) {
        auto r = filterRegistry.getMeta(id);
        auto meta = *r;  // We're using IDs from the registry. This should never be an error.
        auto action = new QAction(QString::fromStdString(meta->name));
        action->setData(QString::fromStdString(id));
        menu->addAction(action);
    }
    return menu;
}
