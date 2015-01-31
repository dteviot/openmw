#include "contentselector.hpp"

#include "../model/esmfile.hpp"

#include <QSortFilterProxyModel>

#include <QMenu>
#include <QContextMenuEvent>

#include <QGridLayout>
#include <QMessageBox>
#include <QModelIndex>
#include <assert.h>

ContentSelectorView::ContentSelector::ContentSelector(QWidget *parent) :
    QObject(parent)
{
    ui.setupUi (parent);

    buildContentModel();
    buildGameFileView();
    buildAddonView();
}

void ContentSelectorView::ContentSelector::buildContentModel()
{
    QIcon warningIcon(ui.addonView->style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(QSize(16, 15)));
    mAllPluginsContentModel = new ContentSelectorModel::AllPluginsContentModel(this, warningIcon);
    mLoadPluginsContentModel = new ContentSelectorModel::LoadPluginsContentModel(this, warningIcon);
}

void ContentSelectorView::ContentSelector::buildGameFileView()
{
    ui.gameFileView->setVisible (true);

    mGameFileProxyModel = new QSortFilterProxyModel(this);
    mGameFileProxyModel->setFilterRegExp(QString::number((int)ContentSelectorModel::ContentType_GameFile));
    mGameFileProxyModel->setFilterRole (Qt::UserRole);
    mGameFileProxyModel->setSourceModel (mAllPluginsContentModel);

    ui.gameFileView->setPlaceholderText(QString("Select a game file..."));
    ui.gameFileView->setModel(mGameFileProxyModel);

    connect (ui.gameFileView, SIGNAL (currentIndexChanged(int)),
             this, SLOT (slotCurrentGameFileIndexChanged(int)));

    ui.gameFileView->setCurrentIndex(-1);
    ui.gameFileView->setCurrentIndex(0);
}

void ContentSelectorView::ContentSelector::buildAddonView()
{
    ui.addonView->setVisible (true);

    mAllPluginsProxyModel = CreateProxy(ui.addonView, mAllPluginsContentModel);
    mLoadPluginsProxyModel = CreateProxy(ui.addonView2, mLoadPluginsContentModel);

    connect(ui.addonView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotAddonTableItemActivated(const QModelIndex&)));
    connect(mAllPluginsContentModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SIGNAL(signalAddonDataChanged(QModelIndex,QModelIndex)));
}

QSortFilterProxyModel* ContentSelectorView::ContentSelector::CreateProxy(QTableView* tableView, QAbstractTableModel* contentModel)
{
    tableView->setVisible(true);

    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
    proxy->setFilterRegExp(QString::number((int)ContentSelectorModel::ContentType_Addon));
    proxy->setFilterRole(Qt::UserRole);
    proxy->setDynamicSortFilter(true);
    proxy->setSourceModel(contentModel);

    tableView->setModel(proxy);
    
    return proxy;
}

void ContentSelectorView::ContentSelector::setProfileContent(const QStringList &fileList)
{
    clearCheckStates();

    foreach (const QString &filepath, fileList)
    {
        const ContentSelectorModel::EsmFile *file = mAllPluginsContentModel->item(filepath);
        if (file && file->isGameFile())
        {
            setGameFile (filepath);
            break;
        }
    }

    setContentList(fileList);
}

void ContentSelectorView::ContentSelector::setGameFile(const QString &filename)
{
    int index = -1;

    if (!filename.isEmpty())
    {
        const ContentSelectorModel::EsmFile *file = mAllPluginsContentModel->item (filename);
        index = ui.gameFileView->findText (file->fileName());

        //verify that the current index is also checked in the model
        if (!mAllPluginsContentModel->setCheckState(filename, true))
        {
            //throw error in case file not found?
            return;
        }
    }

    ui.gameFileView->setCurrentIndex(index);
}

void ContentSelectorView::ContentSelector::clearCheckStates()
{
    mAllPluginsContentModel->uncheckAll();
}

void ContentSelectorView::ContentSelector::setContentList(const QStringList &list)
{
    if (list.isEmpty())
    {
        slotCurrentGameFileIndexChanged (ui.gameFileView->currentIndex());
    }
    else
        mAllPluginsContentModel->setContentList(list, true);
}

ContentSelectorModel::ContentFileList
        ContentSelectorView::ContentSelector::selectedFiles() const
{
    if (!mAllPluginsContentModel)
        return ContentSelectorModel::ContentFileList();

    return mAllPluginsContentModel->checkedItems();
}

void ContentSelectorView::ContentSelector::addFiles(const QString &path)
{
    mAllPluginsContentModel->addFiles(path);

    if (ui.gameFileView->currentIndex() != -1)
        ui.gameFileView->setCurrentIndex(-1);

    mAllPluginsContentModel->uncheckAll();
}

QString ContentSelectorView::ContentSelector::currentFile() const
{
    QModelIndex currentIdx = ui.addonView->currentIndex();

    if (!currentIdx.isValid())
        return ui.gameFileView->currentText();

    QModelIndex idx = mAllPluginsContentModel->index(mAllPluginsProxyModel->mapToSource(currentIdx).row(), 0, QModelIndex());
    return mAllPluginsContentModel->data(idx, Qt::DisplayRole).toString();
}

void ContentSelectorView::ContentSelector::slotCurrentGameFileIndexChanged(int index)
{
    static int oldIndex = -1;

    QAbstractItemModel *const model = ui.gameFileView->model();
    QSortFilterProxyModel *proxy = dynamic_cast<QSortFilterProxyModel *>(model);

    if (proxy)
        proxy->setDynamicSortFilter(false);

    if (index != oldIndex)
    {
        if (oldIndex > -1)
            model->setData(model->index(oldIndex, 0), false, Qt::UserRole + 1);

        oldIndex = index;

        model->setData(model->index(index, 0), true, Qt::UserRole + 1);
        mAllPluginsContentModel->checkForLoadOrderErrors();
    }

    if (proxy)
        proxy->setDynamicSortFilter(true);

    emit signalCurrentGamefileIndexChanged (index);
}

void ContentSelectorView::ContentSelector::slotAddonTableItemActivated(const QModelIndex &index)
{
    QModelIndex sourceIndex = mAllPluginsProxyModel->mapToSource (index);

    if (!mAllPluginsContentModel->isEnabled (sourceIndex))
        return;

    Qt::CheckState checkState = Qt::Unchecked;

    if (mAllPluginsContentModel->data(sourceIndex, Qt::CheckStateRole).toInt() == Qt::Unchecked)
        checkState = Qt::Checked;

    mAllPluginsContentModel->setData(sourceIndex, checkState, Qt::CheckStateRole);
}
