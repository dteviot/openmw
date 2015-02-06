#ifndef LOADPLUGINSCONTENTMODEL_HPP
#define LOADPLUGINSCONTENTMODEL_HPP

#include <QAbstractTableModel>
#include <QStringList>
#include <QSet>
#include <QIcon>
#include "loadordererror.hpp"
#include "contentmodel.hpp"

namespace ContentSelectorModel
{
    class LoadPluginsContentModel : public ContentModel
    {
        Q_OBJECT
    public:
        explicit LoadPluginsContentModel(QObject *parent, QIcon warningIcon);

        bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
        bool dropMimeData(const QMimeData *data, int row, const QModelIndex &parent);

    protected:
        virtual QString toolTip(const EsmFile *file) const;
    };
}
#endif // LOADPLUGINSCONTENTMODEL_HPP
