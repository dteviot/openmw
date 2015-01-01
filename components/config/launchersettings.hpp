#ifndef LAUNCHERSETTINGS_HPP
#define LAUNCHERSETTINGS_HPP

#include "settingsbase.hpp"
#include "gamesettings.hpp"

namespace Config
{
    class LauncherSettings : public SettingsBase<QMap<QString, QString> >
    {
    public:
        LauncherSettings();
        ~LauncherSettings();

        bool writeFile(QTextStream &stream);

        /// \return names of all Content Lists in the launcher's .cfg file. 
        QStringList getContentLists();

        /// Set initally selected content list to match values from openmw.cfg, creating if necessary
        void setContentList(const GameSettings& gameSettings);

        /// Create a Content List (or replace if it already exists)
        void setContentList(const QString& contentListName, const QStringList& fileNames);

        void removeContentList(const QString &contentListName);

        void setCurrentContentListName(const QString &contentListName);

        QString currentContentListName() const;

        QStringList getContentListFiles(const QString& contentListName);

        /// \return new list that is reversed order of input
        static QStringList reverse(const QStringList& toReverse);

        static const char sCurrentProfileKey[];
        static const char sLauncherConfigFileName[];
    
    private:

        /// \return key to use to get the files in the specified Content List
        static QString makeContentListKey(const QString& contentListName);

        QStringList subKeys(const QString &key);
    };
}
#endif // LAUNCHERSETTINGS_HPP
