#include "LookFiltersModel.hpp"

#include <QByteArray>
#include <QDebug>
#include <QHash>

using namespace image;

void LookFiltersModel::setLook(std::shared_ptr<Look> look) noexcept {
    beginResetModel();
    look_ = look;
    endResetModel();
}

void LookFiltersModel::addFilter(FilterSpec &&filter) noexcept {
    if (!look_) { return; }
    beginInsertRows(QModelIndex(), 0, 1);
    look_->filterSpecs.emplace_back(std::move(filter));
    endInsertRows();
}

FilterSpec &LookFiltersModel::filterAtIndex(const QModelIndex &idx) const noexcept {
    return look_->filterSpecs.at(idx.row());
}

FilterManager *LookFiltersModel::filterManagerAtIndex(const QModelIndex &idx) noexcept {
    auto &filter = filterAtIndex(idx);
    if (filterManagers_.contains(filter.ptr())) {
        return filterManagers_.value(filter.ptr());
    } else {
        auto manager = new FilterManager(filter.ptr(), this);
        filterManagers_.insert(filter.ptr(), manager);
        connect(manager, &FilterManager::filterUpdated, this, &LookFiltersModel::lookUpdated);
        return manager;
    }
}

int LookFiltersModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid() || !look_) { return 0; }
    return look_->filterSpecs.size();
}

QVariant LookFiltersModel::data(const QModelIndex &idx, int role) const {
    if (!idx.isValid() || !look_) { return QVariant(); }

    auto &filterSpec = filterAtIndex(idx);
    switch (role) {
    case Qt::DisplayRole:
    case Roles::NameRole:
        return QString::fromStdString(filterSpec->getMeta().name);
    default:
        return QVariant();
    }
}

Qt::ItemFlags LookFiltersModel::flags(const QModelIndex &index) const {
    if (!index.isValid() || !look_) { return Qt::NoItemFlags; }

    return Qt::ItemFlag::ItemIsEnabled;
}

QHash<int, QByteArray> LookFiltersModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    return roles;
}

LookFiltersModel::LookFiltersModel(std::shared_ptr<Look> look, QObject *parent) noexcept
  : QAbstractListModel(parent)
  , look_(look) {}

LookFiltersModel::LookFiltersModel(const LookFiltersModel &other) noexcept
  : QAbstractListModel(other.parent())
  , look_(other.look_) {}
