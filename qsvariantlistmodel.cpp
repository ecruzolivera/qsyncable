/* QSyncable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/benlau/qsyncable
*/
#include <QtCore>
#include "qsvariantlistmodel.h"

QSVariantListModel::QSVariantListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int QSVariantListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return count();
}

QVariant QSVariantListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_data.size())
        return QVariant();

    QVariant v;

    if (m_roles.contains(role)) {
        QVariantMap item = m_data.at(index.row()).toMap();
        v = item[m_roles[role]];
    }

    return v;
}

void QSVariantListModel::append(const QVariantMap &value)
{
    beginInsertRows(QModelIndex(),m_data.size(),m_data.size());
    m_data.append(value);
    endInsertRows();
    emit countChanged();
}

void QSVariantListModel::insert(int index, const QVariantMap &value)
{
    beginInsertRows(QModelIndex(), index, index);
    m_data.insert(index, value);
    endInsertRows();
    emit countChanged();
}

void QSVariantListModel::move(int from, int to, int count)
{

    if (from > to) {
        int f = from;
        int t = to;
        int c = count;
        count = f - t;
        from = t;
        to = t + c;
    }

    if (count <= 0 ||
        from == to ||
        from + count > m_data.count() ||
        to + count > m_data.count() ||
        from < 0 ||
        to < 0) {
        return;
    }

    beginMoveRows(QModelIndex(), from, from + count - 1,
                  QModelIndex(), to > from ? to + count : to);

    QVariantList tmp;
    tmp.reserve(to - from + count);

    // Move "to" block
    for (int i=0 ; i < (to - from) ; i++) {
        tmp.append(m_data[from + count + i]);
    }

    // Move "from" block
    for (int i = 0 ; i < count ; i++) {
        tmp.append(m_data[from + i]);
    }

    // Replace
    for (int i=0 ; i < tmp.count() ; ++i) {
        m_data[from + i] = tmp[i];
    }

    endMoveRows();
}

void QSVariantListModel::clear()
{
    if (m_data.isEmpty())
        return;

    beginRemoveRows(QModelIndex(), 0, m_data.count() - 1);
    m_data.clear();
    endRemoveRows();
    emit countChanged();

}

void QSVariantListModel::remove(int i, int count)
{
    beginRemoveRows(QModelIndex(), i, i + count - 1);
    for (int j = 0; j < count; ++j)
        m_data.removeAt(i);
    endRemoveRows();
    emit countChanged();
}

int QSVariantListModel::count() const
{
    return m_data.size();
}

QVariantMap QSVariantListModel::get(int i) const
{
    QVariantMap map;
    if (i >=0 && i < m_data.size()) {
        map = m_data.at(i).toMap();
    }
    return map;

}

void QSVariantListModel::setProperty(int idx, QString property, QVariant value)
{
    if (idx < 0 || idx >= m_data.size())
        return;
    QVector<int> roles;
    QVariantMap item = get(idx);

    QHashIterator<int, QByteArray> i(m_roles);
    while (i.hasNext()) {
        i.next();
        if (i.value() == property) {
            roles << i.key();
            break;
        }
    }

    item[property] = value;

    m_data[idx] = item;

    emit dataChanged(index(idx,0),
                     index(idx,0),
                     roles);
}

void QSVariantListModel::set(int idx, QVariantMap data)
{
    if (idx < 0 || idx >= m_data.size())
        return;

    QVector<int> roles;
    QHashIterator<int, QByteArray> i(m_roles);
    while (i.hasNext()) {
        i.next();
        roles << i.key();
    }
    m_data[idx] = data;

    emit dataChanged(index(idx,0),
                     index(idx,0),
                     roles);
}

void QSVariantListModel::setProperties(int index, QVariantMap changes)
{
    QVariantMap original = get(index);

    QMapIterator<QString, QVariant> iter(changes);

    while (iter.hasNext()) {
        iter.next();
        if (!original.contains(iter.key()) ||
             original[iter.key()] != iter.value()) {
            setProperty(index,iter.key(),iter.value());
        }
    }
}

QHash<int, QByteArray> QSVariantListModel::roleNames() const
{
    return m_roles;
}

void QSVariantListModel::setRoleNames(const QVariantMap &value)
{
    m_roles.clear();
    QMapIterator<QString,QVariant> iter(value);

    int role = Qt::UserRole;

    while (iter.hasNext()) {
        iter.next();
        m_roles[role++] = iter.key().toLocal8Bit();
    }

}

void QSVariantListModel::setRoleNames(const QStringList& list)
{
    m_roles.clear();
    int role = Qt::UserRole;

    for (int i = 0 ; i < list.size();i++) {
        m_roles[role++] = list.at(i).toLocal8Bit();
    }
}

void QSVariantListModel::setList(const QVariantList &value)
{
    int oldCount = m_data.count();
    beginResetModel();
    m_data = value;
    endResetModel();
    if (oldCount != m_data.size()) {
        emit countChanged();
    }
}

QVariantList QSVariantListModel::list() const
{
    return m_data;
}

int QSVariantListModel::indexOf(QString field, QVariant value) const
{
    int res = -1;
    for (int i = 0 ; i < m_data.count();i++) {
        QVariantMap item = m_data.at(i).toMap();
        if (item.contains(field) && item[field] == value) {
            res = i;
            break;
        }
    }
    return res;
}

