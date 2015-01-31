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
