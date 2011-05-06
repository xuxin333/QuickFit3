#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H

#include <QtGui>

/*! \brief this class manages the overall program options (and may also display an options Dialog
    \ingroup qf3lib_settings
 */
class ProgramOptions: public QObject {
        Q_OBJECT
    public:
        /** \brief Default constructor
         *
         * \param ini filename of the INI file of the application
         * \param parent pointer to the parent object
         * \param app the application this object manages the properties for
         */
        ProgramOptions( QString ini=QString(""), QObject * parent = 0, QApplication* app=NULL);

        /** \brief Default destructor */
        virtual ~ProgramOptions();

        /** \brief Access languageID (which language to use for display) */
        inline QString getLanguageID() { return languageID; }

        /** \brief Access languageID (which language to use for display) */
        inline QString getIniFilename() { return iniFilename; }

        /** \brief return the currently used stylesheet */
        inline QString getStylesheet() { return stylesheet; }

        /** \brief return the currently used style */
        inline QString getStyle() { return style; }

        /** \brief return a pointer to a QSettings object that may be used to save config data throughout the application */
        inline QSettings* getQSettings() { return settings; }

        /** \brief return the maximum number of threds */
        int getMaxThreads() { return maxThreads; };

        inline QString getCurrentRawDataDir() { return currentRawDataDir; }
        inline void setCurrentRawDataDir(QString d) { currentRawDataDir=d; }


        /** \brief set languageID (which language to use for display) */
        void setLanguageID(QString id);

        /** \brief set the currently used stylesheet */
        void setStylesheet(QString st);

        /** \brief set the currently used style */
        void setStyle(QString st);

        /** \brief set the maximum number of threds */
        void setMaxThreads(int threads) { maxThreads=threads; };

        /** \brief autosave project every X minutes */
        int getAutosave() { return autosave; }
        /** \brief autosave project every X minutes (0 == off) */
        void setAutosave(int interval) { autosave=interval; }

        /** \brief the directory in which to save configuration data, see \ref qf3whereiswhat */
        QString getConfigFileDirectory() const;
        /** \brief points to a directory containing the QuickFit plugins, see \ref qf3whereiswhat */
        QString getPluginDirectory() const;
        /** \brief points to a directory containing the QuickFit plugins, see \ref qf3whereiswhat */
        QString getAssetsDirectory() const;
    public slots:



    signals:
        /** \brief emitted when any of the data is changed by the user */
        void configDataChanged();

        /** \brief emitted when the language has been changed */
        void languageChanged(QString languageID);

        /** \brief emitted when the stylesheet changed */
        void stylesheetChanged(QString stylesheet);

        /** \brief emitted when the style changed */
        void styleChanged(QString style);

    protected:
        /** \brief this function reads the current settings from the internal QSettings object settings */
        void readSettings();
        /** \brief this function writes the current settings to the internal QSettings object settings */
        void writeSettings();


        /** \brief QSettings object used for config options I/O */
        QSettings* settings;
        /** \brief the application this object manages the properties for */
        QApplication* app;



        /** \brief style to use in the application */
        QString style;
        /** \brief stylesheet to use in the application */
        QString stylesheet;
        /** \brief filename of INI file */
        QString iniFilename;
        /** \brief language of the user interface */
        QString languageID;
        /** \brief last dir to load a raw data from */
        QString currentRawDataDir;
        /** \brief maximum number of threads */
        int maxThreads;
        /** \brief autosave interval in minutes (0=off) */
        int autosave;
        /** \brief the directory in which to save configuration data, see \ref qf3whereiswhat */
        QString configDir;
        /** \brief assetsDir is a directory that contains QuickFit assets, like Help-Files, additional needed files, lik
         *         data files ... that are NOT changed by QuickFit, see \ref qf3whereiswhat */
        QString assetsDir;
        /** \brief points to a directory containing the QuickFit plugins, see \ref qf3whereiswhat */
        QString pluginsDir;

};

#endif // PROGRAMOPTIONS_H
