#ifndef ALLPLUGINSCONTENTMODEL_HPP
#define ALLPLUGINSCONTENTMODEL_HPP

#include <QAbstractTableModel>
#include <QStringList>
#include <QSet>
#include <QIcon>
#include "loadordererror.hpp"
#include "contentmodel.hpp"

namespace ContentSelectorModel
{
    class AllPluginsContentModel : public ContentModel
    {
        Q_OBJECT
    public:
        explicit AllPluginsContentModel(QObject *parent, QIcon warningIcon, ContentModel* loadPluginsContentModel);

        void addFiles(const QString &path);

        bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

        bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

        Qt::ItemFlags flags(const QModelIndex &index) const;

    private:
        ContentModel  *mLoadPluginsContentModel;
    };
}
#endif // ALLPLUGINSCONTENTMODEL_HPP
