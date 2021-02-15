#pragma once

#include <QListView>
#include <QStyledItemDelegate>
#include <QWidget>

#include "LookFiltersModel.hpp"

class FilterItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    FilterItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    virtual ~FilterItemDelegate() {}

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class LookFilters final : public QWidget {
    Q_OBJECT
public:
    explicit LookFilters(QWidget *parent = nullptr) noexcept;

    virtual ~LookFilters() {}

    void setLookFiltersModel(LookFiltersModel *model) noexcept;

    void setupContextMenu() noexcept;

signals:
    void lookEnabledChanged(bool isEnabled);

private:
    void syncWidgetEnabled() noexcept;

    LookFiltersModel *model_ { nullptr };
    QListView *listView { nullptr };
    FilterItemDelegate *delegate { nullptr };
    QAction *addFilterAction { nullptr };
    QMenu *addFilterMenu { nullptr };
};
