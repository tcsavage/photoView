#pragma once

#include <memory>

#include <QAbstractListModel>
#include <QMap>

#include <image/Look.hpp>

#include "FilterManager.hpp"

class LookFiltersModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
    };

    inline std::shared_ptr<image::Look> look() noexcept { return look_; }
    void setLook(std::shared_ptr<image::Look> look) noexcept;

    void addFilter(image::FilterSpec &&filter) noexcept;

    image::FilterSpec &filterAtIndex(const QModelIndex &idx) const noexcept;
    FilterManager *filterManagerAtIndex(const QModelIndex &idx) noexcept;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    LookFiltersModel() noexcept {}
    explicit LookFiltersModel(std::shared_ptr<image::Look> look, QObject *parent = nullptr) noexcept;
    explicit LookFiltersModel(const LookFiltersModel &other) noexcept;

    virtual ~LookFiltersModel() {}

signals:
    void lookUpdated();

private:
    std::shared_ptr<image::Look> look_;
    QMap<image::AbstractFilterSpec *, FilterManager *> filterManagers_;
};