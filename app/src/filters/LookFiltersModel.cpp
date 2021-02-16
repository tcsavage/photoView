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
    look_->addFilter(std::move(filter));
    endInsertRows();
    emit lookUpdated();
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
    case Qt::CheckStateRole:
        return filterSpec->isEnabled ? Qt::Checked : Qt::Unchecked;
    default:
        return QVariant();
    }
}

bool LookFiltersModel::setData(const QModelIndex &idx, const QVariant &value, int role) {
    (void)value;
    if (!idx.isValid() || !look_) { return false; }

    auto &filterSpec = filterAtIndex(idx);
    (void)filterSpec;
    switch (role) {
    case Qt::CheckStateRole:
        filterSpec->isEnabled = value.value<Qt::CheckState>() == Qt::Checked;
        emit lookUpdated();
        return true;
    default:
        return false;
    }
}

bool LookFiltersModel::removeRows(int row, int count, const QModelIndex &parent) {
    assert(look_);
    beginRemoveRows(parent, row, row + count);
    look_->removeFilters(row, count);
    endRemoveRows();
    emit lookUpdated();
    return true;
}

bool LookFiltersModel::moveRows(const QModelIndex &srcParent,
                                int srcRow,
                                int count,
                                const QModelIndex &destParent,
                                int destRow) {
    assert(srcParent == destParent);
    assert(!srcParent.isValid());
    beginMoveRows(srcParent, srcRow, srcRow + count, destParent, destRow);
    look_->rotateFilters(srcRow, count, destRow);
    endMoveRows();
    emit lookUpdated();
    return true;
}

Qt::ItemFlags LookFiltersModel::flags(const QModelIndex &index) const {
    if (!index.isValid() || !look_) { return Qt::NoItemFlags; }

    return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsUserCheckable;
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
