#pragma once

#include <QStyledItemDelegate>
#include <QTreeView>
#include <QWidget>

#include <app/composition/CompositionModel.hpp>

class CompositionOutline final : public QWidget {
    Q_OBJECT
public:
    explicit CompositionOutline(QWidget *parent = nullptr) noexcept;

    virtual ~CompositionOutline() {}

    void setCompositionModel(CompositionModel *model) noexcept;

    void setupContextMenu() noexcept;

signals:
    void filtersEnabledChanged(bool isEnabled);

private:
    void syncWidgetEnabled() noexcept;

    CompositionModel *model_ { nullptr };
    QTreeView *treeView { nullptr };
    QAction *addFilterAction { nullptr };
    QMenu *addFilterMenu { nullptr };
};
