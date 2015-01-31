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
    };
}
#endif // LOADPLUGINSCONTENTMODEL_HPP
