#include "allpluginscontentmodel.hpp"
#include "esmfile.hpp"

#include <stdexcept>

#include <QDir>
#include <QTextCodec>
#include <QDebug>

#include "components/esm/esmreader.hpp"

ContentSelectorModel::AllPluginsContentModel::AllPluginsContentModel(QObject *parent, QIcon warningIcon,
    ContentModel* loadPluginsContentModel) :
    ContentModel(parent, warningIcon),
    mLoadPluginsContentModel(loadPluginsContentModel)
{
}

void ContentSelectorModel::AllPluginsContentModel::addFiles(const QString &path)
{
    QDir dir(path);
    QStringList filters;
    filters << "*.esp" << "*.esm" << "*.omwgame" << "*.omwaddon";
    dir.setNameFilters(filters);

    QTextCodec *codec = QTextCodec::codecForName("UTF8");

    // Create a decoder for non-latin characters in esx metadata
    QTextDecoder *decoder = codec->makeDecoder();

    foreach (const QString &path, dir.entryList())
    {
        QFileInfo info(dir.absoluteFilePath(path));

        if (item(info.absoluteFilePath()) != 0)
            continue;

        try {
            ESM::ESMReader fileReader;
            ToUTF8::Utf8Encoder encoder =
            ToUTF8::calculateEncoding(mEncoding.toStdString());
            fileReader.setEncoder(&encoder);
            fileReader.open(dir.absoluteFilePath(path).toStdString());

            EsmFile *file = new EsmFile(path);

            foreach (const ESM::Header::MasterData &item, fileReader.getGameFiles())
                file->addGameFile(QString::fromStdString(item.name));

            file->setAuthor     (decoder->toUnicode(fileReader.getAuthor().c_str()));
            file->setDate       (info.lastModified());
            file->setFormat     (fileReader.getFormat());
            file->setFilePath       (info.absoluteFilePath());
            file->setDescription(decoder->toUnicode(fileReader.getDesc().c_str()));

            // Put the file in the table
            addFile(file);

        } catch(std::runtime_error &e) {
            // An error occurred while reading the .esp
            qWarning() << "Error reading addon file: " << e.what();
            continue;
        }

    }

    delete decoder;

    sortFiles();
}

bool ContentSelectorModel::AllPluginsContentModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    // nothing to do, will just be someone trying drag & drop an in-use plug-in.
    // no state change required.
    return true;
}

bool ContentSelectorModel::AllPluginsContentModel::removeRows(int position, int rows, const QModelIndex &index)
{
    // nothing to do, will just be someone trying drag & drop an in-use plug-in.
    // no state change required.
    return true;
}

Qt::ItemFlags ContentSelectorModel::AllPluginsContentModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags | Qt::ItemIsDropEnabled;

    const EsmFile *file = itemAsConst(index.row());

    if (!file)
        return Qt::NoItemFlags;

    //game files can always be checked
    if (file->isGameFile())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    // if item is in the plug-ins to be loaded list, disable it in this list.
    Qt::ItemFlags returnFlags = 0;
    if (mLoadPluginsContentModel->item(file->filePath()) == NULL)
    {
        returnFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    return returnFlags | mDragDropFlags;
}
