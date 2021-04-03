#pragma once

#include <QStyledItemDelegate>
#include <QTreeView>
#include <QWidget>

#include <app/composition/CompositionModel.hpp>

class CompositionTreeView : public QTreeView {
    Q_OBJECT
public:
    explicit CompositionTreeView(QWidget *parent = nullptr);

    virtual ~CompositionTreeView() {}

    virtual void setModel(QAbstractItemModel *model) override;

public slots:
    void handleRowsInserted(const QModelIndex &parent, int, int);

signals:
    void currentIndexChanged(const QModelIndex &current);

protected slots:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
};

class CompositionOutline final : public QWidget {
    Q_OBJECT
public:
    explicit CompositionOutline(QWidget *parent = nullptr) noexcept;

    virtual ~CompositionOutline() {}

    void setCompositionModel(CompositionModel *model) noexcept;

    void setupContextMenu() noexcept;

signals:
    void filtersEnabledChanged(bool isEnabled);

    void activeMaskChanged(image::AbstractMaskGenerator *activeMaskGen);

private:
    void syncWidgetEnabled() noexcept;

    CompositionModel *model_ { nullptr };
    CompositionTreeView *treeView { nullptr };
    QAction *addFilterAction { nullptr };
    QMenu *addFilterMenu { nullptr };

    image::AbstractMaskGenerator *lastActiveMaskGen_ { nullptr };
};
