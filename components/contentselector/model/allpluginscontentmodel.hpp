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
        explicit AllPluginsContentModel(QObject *parent, QIcon warningIcon);

        void addFiles(const QString &path);

    };
}
#endif // ALLPLUGINSCONTENTMODEL_HPP
