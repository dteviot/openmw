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

        bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

        /// Checks all plug-ins for load order errors and updates mPluginsWithLoadOrderError with plug-ins with issues
        void checkForLoadOrderErrors();

        /// Provides list of all available files
        void setAllPluginsContentModel(ContentModel *allPluginsContentModel);

        ContentFileList filesToLoad() const;

    protected:
        /// Icon to decorate plug-in with in view.
        QVariant getDecoration(const EsmFile *file) const;

        QString toolTip(const EsmFile *file) const;

    private:
        /// Checks a specific plug-in for load order errors
        /// \return all errors found for specific plug-in
        QList<LoadOrderError> checkForLoadOrderErrors(const EsmFile *file, int row) const;

        ///  \return true if plug-in has a Load Order error
        bool isLoadOrderError(const EsmFile *file) const;

        QSet<QString> mPluginsWithLoadOrderError;

        ContentModel *mAllPluginsContentModel;
    };
}
#endif // LOADPLUGINSCONTENTMODEL_HPP
