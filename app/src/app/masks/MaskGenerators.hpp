#pragma once

#include <QMenu>

#include <image/AllMaskGenerators.hpp>

inline image::MaskGeneratorRegistry maskGeneratorRegistry { image::makeMaskGeneratorRegistry() };

inline QMenu *makeMaskGeneratorMenu() noexcept {
    auto menu = new QMenu();
    for (auto &&id : maskGeneratorRegistry.registeredIds()) {
        auto r = maskGeneratorRegistry.getMeta(id);
        auto meta = *r;  // We're using IDs from the registry. This should never be an error.
        auto action = new QAction(QString::fromStdString(meta->name));
        action->setData(QString::fromStdString(id));
        menu->addAction(action);
    }
    return menu;
}
