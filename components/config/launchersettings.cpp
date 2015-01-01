#include "launchersettings.hpp"

#include <QTextStream>
#include <QString>
#include <QRegExp>
#include <QMap>

#include <QDebug>

const char Config::LauncherSettings::sCurrentProfileKey[] = "Profiles/currentprofile";
const char Config::LauncherSettings::sLauncherConfigFileName[] = "launcher.cfg";

Config::LauncherSettings::LauncherSettings()
{
}

Config::LauncherSettings::~LauncherSettings()
{
}

QStringList Config::LauncherSettings::subKeys(const QString &key)
{
    QMap<QString, QString> settings = SettingsBase::getSettings();
    QStringList keys = settings.uniqueKeys();

    qDebug() << keys;

    QRegExp keyRe("(.+)/");

    QStringList result;

    foreach (const QString &currentKey, keys) {

        if (keyRe.indexIn(currentKey) != -1)
        {
            QString prefixedKey = keyRe.cap(1);

            if(prefixedKey.startsWith(key))
            {
                QString subKey = prefixedKey.remove(key);
                if (!subKey.isEmpty())
                    result.append(subKey);
            }
        }
    }

    result.removeDuplicates();
    return result;
}


bool Config::LauncherSettings::writeFile(QTextStream &stream)
{
    QString sectionPrefix;
    QRegExp sectionRe("([^/]+)/(.+)$");
    QMap<QString, QString> settings = SettingsBase::getSettings();

    QMapIterator<QString, QString> i(settings);
    i.toBack();

    while (i.hasPrevious()) {
        i.previous();

        QString prefix;
        QString key;

        if (sectionRe.exactMatch(i.key())) {
             prefix = sectionRe.cap(1);
             key = sectionRe.cap(2);
        }

        // Get rid of legacy settings
        if (key.contains(QChar('\\')))
            continue;

        if (key == QLatin1String("CurrentProfile"))
            continue;

        if (sectionPrefix != prefix) {
            sectionPrefix = prefix;
            stream << "\n[" << prefix << "]\n";
        }

        stream << key << "=" << i.value() << "\n";
    }

    return true;

}

/// \return names of all Content Lists in the launcher's .cfg file. 
QStringList Config::LauncherSettings::getContentLists()
{
    return subKeys(QString("Profiles/"));
}

/// \return key to use to get the files in the specified Content List
QString Config::LauncherSettings::makeContentListKey(const QString& contentListName)
{
    return QString("Profiles/") + contentListName + QString("/content");
}

void Config::LauncherSettings::setContentList(const GameSettings& gameSettings)
{
    // obtain content list from game settings (if present)
    const QStringList files(gameSettings.values(QLatin1String(Config::GameSettings::sContentKey)));

    // if any existing profile in launcher matches the content list, make that profile the default
    foreach(const QString &listName, getContentLists())
    {
        QStringList contentList = getContentListFiles(listName);
        if (contentList.count() == files.count())
        {
            bool matchFound = true;
            for (int i = 0; i < contentList.count(); ++i)
            {
                if (contentList.at(i) != files.at(i))
                {
                    matchFound = false;
                    break;
                }
            }
            if (matchFound)
            {
                setCurrentContentListName(listName);
                return;
            }
        }
    }

    // otherwise, add content list
    QString newContentListName("Autocreated");
    setCurrentContentListName(newContentListName);
    setContentList(newContentListName, reverse(files));
}

void Config::LauncherSettings::removeContentList(const QString &contentListName)
{
    remove(makeContentListKey(contentListName));
}

void Config::LauncherSettings::setCurrentContentListName(const QString &contentListName)
{
    remove(QString(sCurrentProfileKey));
    setValue(QString(sCurrentProfileKey), contentListName);
}

void Config::LauncherSettings::setContentList(const QString& contentListName, const QStringList& fileNames)
{
    removeContentList(contentListName);
    QString key = Config::LauncherSettings::makeContentListKey(contentListName);
    foreach(const QString& fileName, fileNames)
    {
        setMultiValue(key, fileName);
    }
}

QString Config::LauncherSettings::currentContentListName() const
{
    return value(QString(sCurrentProfileKey));
}

QStringList Config::LauncherSettings::getContentListFiles(const QString& contentListName)
{
    return getSettings().values(Config::LauncherSettings::makeContentListKey(contentListName));
}

QStringList Config::LauncherSettings::reverse(const QStringList& toReverse)
{
    QStringList result;
    result.reserve(toReverse.size());
    std::reverse_copy(toReverse.begin(), toReverse.end(), std::back_inserter(result));
    return result;
}

