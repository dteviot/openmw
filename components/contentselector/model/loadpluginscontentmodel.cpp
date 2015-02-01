#include "loadpluginscontentmodel.hpp"
#include "esmfile.hpp"

#include <stdexcept>

#include <QDir>
#include <QTextCodec>
#include <QDebug>

#include "components/esm/esmreader.hpp"

ContentSelectorModel::LoadPluginsContentModel::LoadPluginsContentModel(QObject *parent, QIcon warningIcon) :
    ContentModel(parent, warningIcon)
{
}

bool ContentSelectorModel::LoadPluginsContentModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    return dropMimeData(data, row, parent);
}

bool ContentSelectorModel::LoadPluginsContentModel::dropMimeData(const QMimeData *data, int row, const QModelIndex &parent)
{
    if (!data->hasFormat(mMimeType))
        return false;

    int beginRow = rowCount();

    if (row != -1)
        beginRow = row;

    else if (parent.isValid())
        beginRow = parent.row();

    QByteArray encodedData = data->data(mMimeType);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd())
    {

        QString value;
        QStringList values;
        QStringList gamefiles;

        for (int i = 0; i < EsmFile::FileProperty_GameFile; ++i)
        {
            stream >> value;
            values << value;
        }

        stream >> gamefiles;

        insertRows(beginRow, 1);

        QModelIndex idx = index(beginRow++, 0, QModelIndex());
        setData(idx, QStringList() << values << gamefiles, Qt::EditRole);
    }

    return true;
}
