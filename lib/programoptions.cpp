#include "programoptions.h"
#include <iostream>
#include <QtCore>
#include <QDir>

#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#ifndef __WINDOWS__
# ifndef __LINUX__
#  warning("these methods are ment to be used under windows or linux ... no other system were tested")
# endif
#endif

ProgramOptions* ProgramOptions::inst=NULL;

ProgramOptions::ProgramOptions( QString ini, QObject * parent, QApplication* app  ):
    QObject(parent)
{
    if (inst==NULL) inst=this;
    userSaveAfterFirstEdit=true;
    childWindowsStayOnTop=true;
    helpWindowsStayOnTop=true;
    projectWindowStayOnTop=false;
    m_debugLogVisible=false;

    QFileInfo fi(QApplication::applicationFilePath());
    appDir=QApplication::applicationDirPath();
    #if defined(Q_OS_MAC)
        // get out of /MyApp.app/Contents/MacOS in MacOSX App-Bundle
        QDir aappDir(appDir);
        if (aappDir.dirName() == "MacOS") {
            aappDir.cdUp();
            aappDir.cdUp();
            aappDir.cdUp();
        }
        appDir=	aappDir.absolutePath();
    #endif
    globalConfigDir=appDir+"/globalconfig/";
    configDir=QDir::homePath()+"/."+fi.completeBaseName()+"/";
    assetsDir=appDir+"/assets/";
    pluginsDir=appDir+"/plugins/";

    #ifdef __WINDOWS__
    #endif

    #if defined(__LINUX__) || defined(Q_OS_MAC)
    globalConfigDir=QString("%1/quickfit3/").arg(GLOBALCONFIGDIR);
    #endif




    QDir d(appDir);
    d.mkpath(configDir);
    d.mkpath(globalConfigDir);

    this->app=app;
    iniFilename=ini;
    if (iniFilename.isEmpty()) {
        iniFilename= configDir+"/"+fi.completeBaseName()+".ini";
    }
    currentRawDataDir=fi.absolutePath();
    //std::cout<<"config file is: "<<iniFilename.toStdString()<<std::endl;
    settings=NULL;

    // default values
    style= QApplication::style()->metaObject()->className();
    stylesheet="default";
    languageID="en";
    maxThreads=2;
    autosave=5;

    readSettings();
}

ProgramOptions::~ProgramOptions()
{
    writeSettings();
    if (inst==this) inst=NULL;
}


void ProgramOptions::writeSettings() {
    settings->setValue("quickfit/language", languageID);
    settings->setValue("quickfit/stylesheet", stylesheet);
    settings->setValue("quickfit/style", style);
    settings->setValue("quickfit/max_threads", maxThreads);
    settings->setValue("quickfit/autosave", autosave);
    settings->setValue("quickfit/current_raw_data_dir", currentRawDataDir);
    settings->setValue("quickfit/userSaveAfterFirstEdit", userSaveAfterFirstEdit);
    settings->setValue("quickfit/childWindowsStayOnTop", childWindowsStayOnTop);
    settings->setValue("quickfit/helpWindowsStayOnTop", helpWindowsStayOnTop);
    settings->setValue("quickfit/projectWindowStayOnTop", projectWindowStayOnTop);
    settings->setValue("quickfit/debugLogVisible", m_debugLogVisible);
}


void ProgramOptions::readSettings() {
    if (!settings) {
        //qDebug()<<"iniFilename="<<iniFilename;
        if (iniFilename=="native") { // on windows: registry, on Linux/MacOS: default
            settings= new QSettings(this);
        } else if (iniFilename=="native_inifile") { // ensures a INI file at the default location, even on windows
            settings= new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName(), this);
        } else {
            settings= new QSettings(iniFilename, QSettings::IniFormat, this);
        }
    }
    maxThreads=settings->value("quickfit/max_threads", maxThreads).toInt();
    autosave=settings->value("quickfit/autosave", autosave).toInt();
    currentRawDataDir=settings->value("quickfit/current_raw_data_dir", currentRawDataDir).toString();
    userSaveAfterFirstEdit=settings->value("quickfit/userSaveAfterFirstEdit", userSaveAfterFirstEdit).toBool();
    childWindowsStayOnTop=settings->value("quickfit/childWindowsStayOnTop", childWindowsStayOnTop).toBool();
    helpWindowsStayOnTop=settings->value("quickfit/helpWindowsStayOnTop", helpWindowsStayOnTop).toBool();
    projectWindowStayOnTop=settings->value("quickfit/projectWindowStayOnTop", projectWindowStayOnTop).toBool();
    m_debugLogVisible=settings->value("quickfit/debugLogVisible", m_debugLogVisible).toBool();

    languageID=settings->value("quickfit/language", languageID).toString();
    if (languageID != "en") { // english is default
        QDir d(assetsDir+"/translations");
        QStringList filters;
        filters << "*.qm";
        QStringList sl=d.entryList(filters, QDir::Files);
        for (int i=0; i<sl.size(); i++) {
            QString s=sl[i];
            if (s.startsWith(languageID+".")) {
                QString fn=d.absoluteFilePath(s);
                //std::cout<<"loading translation '"<<fn.toStdString()<<"' ... \n";
                QTranslator* translator=new QTranslator(this);
                if (app && translator->load(fn)) {
                    app->installTranslator(translator);
                    //std::cout<<"OK\n";
                } else {
                    //std::cout<<"ERROR\n";
                }
            }
        }
        emit languageChanged(languageID);

    }
    style=settings->value("quickfit/style", style).toString();
    if (app) {
        app->setStyle(style);
    }
    emit styleChanged(style);
    stylesheet=settings->value("quickfit/stylesheet", stylesheet).toString();
    if (app) {
        QString fn=QString(appDir+"/stylesheets/%1.qss").arg(stylesheet);
        //std::cout<<"loading stylesheet '"<<fn.toStdString()<<"' ... ";
        QFile f(fn);
        f.open(QFile::ReadOnly);
        QTextStream s(&f);
        QString qss=s.readAll();
        //std::cout<<qss.toStdString()<<std::endl;
        app->setStyleSheet(qss);
        //std::cout<<"OK\n";
    }
    emit stylesheetChanged(stylesheet);

}

QString ProgramOptions::getConfigFileDirectory() const {
    return configDir;
}

QString ProgramOptions::getGlobalConfigFileDirectory() const {
    return globalConfigDir;
}

QString ProgramOptions::getPluginDirectory() const {
    return pluginsDir;
}

QString ProgramOptions::getAssetsDirectory() const {
    return assetsDir;
}

QString ProgramOptions::getApplicationDirectory() const {
    return appDir;
}

bool ProgramOptions::getUserSaveAfterFirstEdit() const
{
    return userSaveAfterFirstEdit;
}

void ProgramOptions::setUserSaveAfterFirstEdit(bool set)
{
    userSaveAfterFirstEdit=set;
}

bool ProgramOptions::getChildWindowsStayOnTop() const
{
    return childWindowsStayOnTop;
}

void ProgramOptions::setChildWindowsStayOnTop(bool set)
{
    childWindowsStayOnTop=set;
}

bool ProgramOptions::getHelpWindowsStayOnTop() const
{
    return helpWindowsStayOnTop;
}

void ProgramOptions::setHelpWindowsStayOnTop(bool set)
{
    helpWindowsStayOnTop=set;
}

bool ProgramOptions::getProjectWindowsStayOnTop() const
{
    return projectWindowStayOnTop;
}

void ProgramOptions::setProjectWindowsStayOnTop(bool set)
{
    projectWindowStayOnTop=set;
}

bool ProgramOptions::debugLogVisible() const
{
    return m_debugLogVisible;
}

void ProgramOptions::setDebugLogVisible(bool visible)
{
    m_debugLogVisible=visible;
}

void ProgramOptions::setLanguageID(QString id) {
    languageID=id;
}

void ProgramOptions::setStylesheet(QString st) {
    stylesheet=st;
}

void ProgramOptions::setStyle(QString st) {
    style=st;
}
