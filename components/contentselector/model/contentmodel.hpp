#ifndef CONTENTMODEL_HPP
#define CONTENTMODEL_HPP

#include <QAbstractTableModel>
#include <QStringList>
#include <QSet>
#include <QIcon>
#include "loadordererror.hpp"

namespace ContentSelectorModel
{
    class EsmFile;

    typedef QList<EsmFile *> ContentFileList;

    enum ContentType
    {
        ContentType_GameFile,
        ContentType_Addon
    };

    class ContentModel : public QAbstractTableModel
    {
        Q_OBJECT
    public:
        explicit ContentModel(QObject *parent, QIcon warningIcon);
        ~ContentModel();

        void setEncoding(const QString &encoding);

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;

        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

        bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
        bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

        void addFile(EsmFile *file);

        Qt::DropActions supportedDropActions() const;
        QStringList mimeTypes() const;
        QMimeData *mimeData(const QModelIndexList &indexes) const;

        QModelIndex indexFromItem(const EsmFile *item) const;
        const EsmFile *item(const QString &name) const;

        bool isEnabled (QModelIndex index) const;
        bool isChecked(const QString &filepath) const;
        bool setCheckState(const QString &filepath, bool isChecked);
        ContentFileList checkedItems() const;
        void clearFiles();

        void refreshModel();

    protected:

        const EsmFile *item(int row) const;
        EsmFile *item(int row);

        void sortFiles();

        /// Icon to decorate plug-in with in view.
        virtual QVariant getDecoration(const EsmFile *file) const;

        virtual QString toolTip(const EsmFile *file) const;

        ContentFileList mFiles;
        QHash<QString, Qt::CheckState> mCheckStates;
        QTextCodec *mCodec;
        QString mEncoding;
        QIcon mWarningIcon;

    public:

        QString mMimeType;
        QStringList mMimeTypes;
        int mColumnCount;
        Qt::ItemFlags mDragDropFlags;
        Qt::DropActions mDropActions;
    };
}
#endif // CONTENTMODEL_HPP
