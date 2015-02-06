#include "contentmodel.hpp"
#include "esmfile.hpp"

#include <stdexcept>

#include <QDir>
#include <QTextCodec>
#include <QDebug>

#include "components/esm/esmreader.hpp"

ContentSelectorModel::ContentModel::ContentModel(QObject *parent, QIcon warningIcon) :
    QAbstractTableModel(parent),
    mWarningIcon(warningIcon),
    mMimeType ("application/omwcontent"),
    mMimeTypes (QStringList() << mMimeType),
    mColumnCount (1),
    mDragDropFlags (Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled),
    mDropActions (Qt::CopyAction | Qt::MoveAction)
{
    setEncoding ("win1252");
}

ContentSelectorModel::ContentModel::~ContentModel()
{
    qDeleteAll(mFiles);
    mFiles.clear();
}

void ContentSelectorModel::ContentModel::setEncoding(const QString &encoding)
{
    mEncoding = encoding;
    if (encoding == QLatin1String("win1252"))
        mCodec = QTextCodec::codecForName("windows-1252");

    else if (encoding == QLatin1String("win1251"))
        mCodec = QTextCodec::codecForName("windows-1251");

    else if (encoding == QLatin1String("win1250"))
        mCodec = QTextCodec::codecForName("windows-1250");

    else
        return; // This should never happen;
}

int ContentSelectorModel::ContentModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return mColumnCount;
}

int ContentSelectorModel::ContentModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return mFiles.size();
}

const ContentSelectorModel::EsmFile *ContentSelectorModel::ContentModel::item(int row) const
{
    if (row >= 0 && row < mFiles.size())
        return mFiles.at(row);

    return 0;
}

ContentSelectorModel::EsmFile *ContentSelectorModel::ContentModel::item(int row)
{
    if (row >= 0 && row < mFiles.count())
        return mFiles.at(row);

    return 0;
}
const ContentSelectorModel::EsmFile *ContentSelectorModel::ContentModel::item(const QString &name) const
{
    EsmFile::FileProperty fp = EsmFile::FileProperty_FileName;

    if (name.contains ('/'))
        fp = EsmFile::FileProperty_FilePath;

    foreach (const EsmFile *file, mFiles)
    {
        if (name.compare(file->fileProperty (fp).toString(), Qt::CaseInsensitive) == 0)
            return file;
    }
    return 0;
}

QModelIndex ContentSelectorModel::ContentModel::indexFromItem(const EsmFile *item) const
{
    //workaround: non-const pointer cast for calls from outside contentmodel/contentselector
    EsmFile *non_const_file_ptr = const_cast<EsmFile *>(item);

    if (item)
        return index(mFiles.indexOf(non_const_file_ptr),0);

    return QModelIndex();
}

Qt::ItemFlags ContentSelectorModel::ContentModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags | Qt::ItemIsDropEnabled;

    const EsmFile *file = item(index.row());

    if (!file)
        return Qt::NoItemFlags;

    //game files can always be checked
    if (file->isGameFile())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    Qt::ItemFlags returnFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | mDragDropFlags;
}

QVariant ContentSelectorModel::ContentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mFiles.size())
        return QVariant();

    const EsmFile *file = item(index.row());

    if (!file)
        return QVariant();

    const int column = index.column();

    switch (role)
    {
    case Qt::DecorationRole:
    {
        return getDecoration(file);
    }

    case Qt::EditRole:
    case Qt::DisplayRole:
    {
        if (column >=0 && column <=EsmFile::FileProperty_GameFile)
            return file->fileProperty(static_cast<const EsmFile::FileProperty>(column));

        return QVariant();
    }

    case Qt::TextAlignmentRole:
    {
        switch (column)
        {
        case 0:
        case 1:
            return Qt::AlignLeft + Qt::AlignVCenter;
        case 2:
        case 3:
            return Qt::AlignRight + Qt::AlignVCenter;
        default:
            return Qt::AlignLeft + Qt::AlignVCenter;
        }
    }

    case Qt::ToolTipRole:
    {
        if (column != 0)
            return QVariant();

        return toolTip(file);
    }

    case Qt::UserRole:
    {
        if (file->isGameFile())
            return ContentType_GameFile;
        else
            if (flags(index))
                return ContentType_Addon;

        break;
    }
    }
    return QVariant();
}

bool ContentSelectorModel::ContentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    EsmFile *file = item(index.row());
    QString fileName = file->fileName();
    bool success = false;

    switch(role)
    {
        case Qt::EditRole:
        {
            QStringList list = value.toStringList();

            for (int i = 0; i < EsmFile::FileProperty_GameFile; i++)
                file->setFileProperty(static_cast<EsmFile::FileProperty>(i), list.at(i));

            for (int i = EsmFile::FileProperty_GameFile; i < list.size(); i++)
                file->setFileProperty (EsmFile::FileProperty_GameFile, list.at(i));

            emit dataChanged(index, index);

            success = true;
        }
        break;

        case Qt::UserRole+1:
        {
            success = (flags (index) & Qt::ItemIsEnabled);

            if (success)
            {
                success = setCheckState(file->filePath(), value.toBool());
                emit dataChanged(index, index);
            }
        }
        break;

    }

    return success;
}

bool ContentSelectorModel::ContentModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    beginInsertRows(parent, position, position+rows-1);
    {
        for (int row = 0; row < rows; ++row)
            mFiles.insert(position, new EsmFile);

    } endInsertRows();

    return true;
}

bool ContentSelectorModel::ContentModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    beginRemoveRows(parent, position, position+rows-1);
    {
        for (int row = 0; row < rows; ++row)
            delete mFiles.takeAt(position);

    } endRemoveRows();

    return true;
}

Qt::DropActions ContentSelectorModel::ContentModel::supportedDropActions() const
{
    return mDropActions;
}

QStringList ContentSelectorModel::ContentModel::mimeTypes() const
{
    return mMimeTypes;
}

QMimeData *ContentSelectorModel::ContentModel::mimeData(const QModelIndexList &indexes) const
{
    QByteArray encodedData;

    foreach (const QModelIndex &index, indexes)
    {
        if (!index.isValid())
            continue;

        encodedData.append(item(index.row())->encodedData());
    }

    QMimeData *mimeData = new QMimeData();
    mimeData->setData(mMimeType, encodedData);

    return mimeData;
}

void ContentSelectorModel::ContentModel::addFile(EsmFile *file)
{
    beginInsertRows(QModelIndex(), mFiles.count(), mFiles.count());
        mFiles.append(file);
    endInsertRows();

    QModelIndex idx = index (mFiles.size() - 2, 0, QModelIndex());

    emit dataChanged (idx, idx);
}

void ContentSelectorModel::ContentModel::sortFiles()
{
    //first, sort the model such that all dependencies are ordered upstream (gamefile) first.
    bool movedFiles = true;
    int fileCount = mFiles.size();

    //Dependency sort
    //iterate until no sorting of files occurs
    while (movedFiles)
    {
        movedFiles = false;
        //iterate each file, obtaining a reference to it's gamefiles list
        for (int i = 0; i < fileCount; i++)
        {
            QModelIndex idx1 = index (i, 0, QModelIndex());
            const QStringList &gamefiles = mFiles.at(i)->gameFiles();
            //iterate each file after the current file, verifying that none of it's
            //dependencies appear.
            for (int j = i + 1; j < fileCount; j++)
            {
                if (gamefiles.contains(mFiles.at(j)->fileName(), Qt::CaseInsensitive))
                {
                        mFiles.move(j, i);

                        QModelIndex idx2 = index (j, 0, QModelIndex());

                        emit dataChanged (idx1, idx2);

                        movedFiles = true;
                }
            }
            if (movedFiles)
                break;
        }
    }
}

bool ContentSelectorModel::ContentModel::isChecked(const QString& filepath) const
{
    if (mCheckStates.contains(filepath))
        return (mCheckStates[filepath] == Qt::Checked);

    return false;
}

bool ContentSelectorModel::ContentModel::isEnabled (QModelIndex index) const
{
    return (flags(index) & Qt::ItemIsEnabled);
}

QString ContentSelectorModel::ContentModel::toolTip(const EsmFile *file) const
{
    return file->toolTip();
}

void ContentSelectorModel::ContentModel::refreshModel()
{
    emit dataChanged (index(0,0), index(rowCount()-1,0));
}

bool ContentSelectorModel::ContentModel::setCheckState(const QString &filepath, bool checkState)
{
    if (filepath.isEmpty())
        return false;

    const EsmFile *file = item(filepath);

    if (!file)
        return false;

    Qt::CheckState state = Qt::Unchecked;

    if (checkState)
        state = Qt::Checked;

    mCheckStates[filepath] = state;
    emit dataChanged(indexFromItem(item(filepath)), indexFromItem(item(filepath)));

    if (file->isGameFile())
        refreshModel();

    //if we're checking an item, ensure all "upstream" files (dependencies) are checked as well.
    if (state == Qt::Checked)
    {
        foreach (QString upstreamName, file->gameFiles())
        {
            const EsmFile *upstreamFile = item(upstreamName);

            if (!upstreamFile)
                continue;

            if (!isChecked(upstreamFile->filePath()))
                mCheckStates[upstreamFile->filePath()] = Qt::Checked;

            emit dataChanged(indexFromItem(upstreamFile), indexFromItem(upstreamFile));

        }
    }
    //otherwise, if we're unchecking an item (or the file is a game file) ensure all downstream files are unchecked.
    if (state == Qt::Unchecked)
    {
        foreach (const EsmFile *downstreamFile, mFiles)
        {
            QFileInfo fileInfo(filepath);
            QString filename = fileInfo.fileName();

            if (downstreamFile->gameFiles().contains(filename, Qt::CaseInsensitive))
            {
                if (mCheckStates.contains(downstreamFile->filePath()))
                    mCheckStates[downstreamFile->filePath()] = Qt::Unchecked;

                emit dataChanged(indexFromItem(downstreamFile), indexFromItem(downstreamFile));
            }
        }
    }

    return true;
}

ContentSelectorModel::ContentFileList ContentSelectorModel::ContentModel::checkedItems() const
{
    ContentFileList list;

    // TODO:
    // First search for game files and next addons,
    // so we get more or less correct game files vs addons order.
    foreach (EsmFile *file, mFiles)
        if (isChecked(file->filePath()))
            list << file;

    return list;
}

void ContentSelectorModel::ContentModel::clearFiles()
{
    emit layoutAboutToBeChanged();
    qDeleteAll(mFiles);
    mFiles.clear();
    emit layoutChanged();
}

QVariant ContentSelectorModel::ContentModel::getDecoration(const EsmFile *file) const
{
    return QVariant();
}
