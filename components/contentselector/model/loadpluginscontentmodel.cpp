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

    // may have droped from "All plug-ins" view, so check for errors now.
    checkForLoadOrderErrors();

    return true;
}

QString ContentSelectorModel::LoadPluginsContentModel::toolTip(const EsmFile *file) const
{
    if (isLoadOrderError(file))
    {
        QString text("<b>");
        int index = indexFromItem(item(file->filePath())).row();
        foreach(const LoadOrderError& error, checkForLoadOrderErrors(file, index))
        {
            text += "<p>";
            text += error.toolTip();
            text += "</p>";
        }
        text += ("</b>");
        text += file->toolTip();
        return text;
    }
    else
    {
        return file->toolTip();
    }
}

bool ContentSelectorModel::LoadPluginsContentModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    bool retVal = ContentModel::removeRows(position, rows, parent);
    if (retVal)
    {
        // at this point we know that drag and drop has finished.
        checkForLoadOrderErrors();
    }
    return retVal;
}

bool ContentSelectorModel::LoadPluginsContentModel::isLoadOrderError(const EsmFile *file) const
{
    return mPluginsWithLoadOrderError.contains(file->filePath());
}

QVariant ContentSelectorModel::LoadPluginsContentModel::getDecoration(const EsmFile *file) const
{
    return isLoadOrderError(file) ? mWarningIcon : QVariant();
}

void ContentSelectorModel::LoadPluginsContentModel::checkForLoadOrderErrors()
{
    for (int row = 0; row < mFiles.count(); ++row)
    {
        EsmFile* file = item(row);
        bool isRowInError = checkForLoadOrderErrors(file, row).count() != 0;
        if (isRowInError)
        {
            mPluginsWithLoadOrderError.insert(file->filePath());
        }
        else
        {
            mPluginsWithLoadOrderError.remove(file->filePath());
        }
    }
}

QList<ContentSelectorModel::LoadOrderError> ContentSelectorModel::LoadPluginsContentModel::checkForLoadOrderErrors(const EsmFile *file, int row) const
{
    QList<LoadOrderError> errors = QList<LoadOrderError>();
    foreach(QString dependentfileName, file->gameFiles())
    {
        const EsmFile* dependentFile = item(dependentfileName);

        if (!dependentFile)
        {
            if (mAllPluginsContentModel->item(dependentfileName))
            {
                errors.append(LoadOrderError(LoadOrderError::ErrorCode_InactiveDependency, dependentfileName));
            }
            else
            {
                errors.append(LoadOrderError(LoadOrderError::ErrorCode_MissingDependency, dependentfileName));
            }
        }
        else
        {
            if (row < indexFromItem(dependentFile).row())
            {
                errors.append(LoadOrderError(LoadOrderError::ErrorCode_LoadOrder, dependentfileName));
            }
        }
    }
    return errors;
}

void ContentSelectorModel::LoadPluginsContentModel::setAllPluginsContentModel(ContentModel *allPluginsContentModel)
{
    mAllPluginsContentModel = allPluginsContentModel;
}

