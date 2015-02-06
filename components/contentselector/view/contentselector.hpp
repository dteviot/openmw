#ifndef CONTENTSELECTOR_HPP
#define CONTENTSELECTOR_HPP

#include <QDialog>

#include "ui_contentselector.h"
#include "../model/allpluginscontentmodel.hpp"
#include "../model/loadpluginscontentmodel.hpp"

class QSortFilterProxyModel;

namespace ContentSelectorView
{
    class ContentSelector : public QObject
    {
        Q_OBJECT

        QStringList mFilePaths;

    protected:

        ContentSelectorModel::AllPluginsContentModel *mAllPluginsContentModel;
        ContentSelectorModel::LoadPluginsContentModel *mLoadPluginsContentModel;
        QSortFilterProxyModel *mGameFileProxyModel;
        QSortFilterProxyModel *mAllPluginsProxyModel;
        QSortFilterProxyModel *mLoadPluginsProxyModel;

    public:

        explicit ContentSelector(QWidget *parent = 0);

        QString currentFile() const;

        void addFiles(const QString &path);
        void setProfileContent (const QStringList &fileList);

        void clearFiles();
        void setContentList(const QStringList &list);
        void cloneToFilesToLoad(const QString &fileName);
        void cloneToFilesToLoad(const QModelIndex &index);
        void removeFromFilesToLoad(const QString &fileName);

        ContentSelectorModel::ContentFileList selectedFiles() const;

        void setGameFile (const QString &filename = QString(""));

        bool isGamefileSelected() const
            { return ui.gameFileView->currentIndex() != -1; }

        QWidget *uiWidget() const
            { return ui.contentGroupBox; }


   private:

        Ui::ContentSelector ui;

        void buildContentModel();
        void buildGameFileView();
        void buildAddonView();

        QSortFilterProxyModel* CreateProxy(QTableView* tableView, QAbstractTableModel* contentModel);

    signals:
        void signalCurrentGamefileIndexChanged (int);

        void signalAddonDataChanged (const QModelIndex& topleft, const QModelIndex& bottomright);

    private slots:

        void slotCurrentGameFileIndexChanged(int index);
        void slotAddonTableItemActivated(const QModelIndex& index);
    };
}

#endif // CONTENTSELECTOR_HPP
