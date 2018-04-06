#ifndef PROVIDERBUTTON_HPP
#define PROVIDERBUTTON_HPP

#include <QAbstractItemModel>
#include <QPushButton>

#include <Core/StreamingProviderStore.hpp>

namespace ProviderButton
{
    QPushButton *create(const Provider &provider);
};

class ProviderListModel : public QAbstractItemModel //QAbstractTableModel
{
    // note: show icon + name instead?
    //  currently only shows name

    Q_OBJECT

public:
    ProviderListModel(QObject *parent = nullptr);
    ~ProviderListModel() override;

    inline void reload()
    {
        this->beginResetModel();
        this->endResetModel();
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
    {
        return this->createIndex(row, column, parent.internalId());
    }
    QModelIndex parent(const QModelIndex &child) const override
    {
        return QModelIndex();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // PROVIDERBUTTON_HPP
