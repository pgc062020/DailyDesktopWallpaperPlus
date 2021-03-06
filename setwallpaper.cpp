#include "setwallpaper.h"

#include <QProcess>
#include <QDebug>
#include <QPixmap>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QDir>
#include <QTimer>
#include <QThread>

void setWallpaper::_read_settings()
{
    QString _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    QSettings settings(_iniFilePath, QSettings::IniFormat);

    settings.beginGroup("SETTINGS");
    _WallpaperDir = settings.value("WallpaperDir","").toString();
    _OldWallpaperDir = settings.value("OldWallpaperDir","").toString();
    settings.endGroup();

    settings.beginGroup("SETWALLPAPER");
    _AutoChange = settings.value("AutoChange","").toBool();
    _Parameter = settings.value("Parameter","").toInt();
    settings.endGroup();
}

void setWallpaper::_set_wallpaper()
{
    _read_settings();
    if(_AutoChange==true)
    {
        QDir wallDir(_WallpaperDir);
        QFileInfoList WallpaperList = wallDir.entryInfoList(QStringList() << "*.jpg", QDir::Files);
        int totalfiles = WallpaperList.size();
        int minFile = 1;

        //check if a file is in the wallpaper directory to avoid a crash
        if (!(minFile > totalfiles))
        {
            _wallpaperfile = _wallpaperfilename;

            //check if a file is a wallpaperfile of DailyDesktopWallpaperPlus
            if(_wallpaperfile.contains("-background.jpg"))
            {
                QProcess setWallpaper;

                if(_Parameter==0)
                    //Budgie
                    setWallpaper.execute("gsettings set org.gnome.desktop.background picture-uri  \"file://"+_wallpaperfile+"\"");
                else if(_Parameter==1)
                    //Cinnamon
                    setWallpaper.execute("gsettings set org.cinnamon.desktop.background picture-uri  \"file://"+_wallpaperfile+"\"");
                else if(_Parameter==2)
                    //DDE
                    setWallpaper.execute("gsettings set com.deepin.wrap.gnome.desktop.background picture-uri \"file://"+_wallpaperfile+"\"");
                else if(_Parameter==3)
                    //GNOME
                    setWallpaper.execute("gsettings set org.gnome.desktop.background picture-uri \"file://"+_wallpaperfile+"\"");
                else if(_Parameter==4)
                    //MATE
                    setWallpaper.execute("gsettings set org.mate.background picture-filename \""+_wallpaperfile+"\"");
                else if(_Parameter==5)
                    //Unity (Ubuntu)
                    setWallpaper.execute("gsettings set org.gnome.desktop.background picture-uri  \"file://"+_wallpaperfile+"\"");
                else if(_Parameter==6) {
                    //KDE Plasma 5.x
                    _create_bashfile();
                    setWallpaper.execute("/bin/bash "+_scriptfile);
                    _remove_bashfile();
                    }
                else if(_Parameter==7)
                    //KDE3.x or TDE
                    setWallpaper.execute("dcop kdesktop KBackgroundIface setWallpaper \""+_wallpaperfile+"\" 8");
                else if(_Parameter==8)
                    //LXDE
                    setWallpaper.execute("pcmanfm --set-wallpaper=\""+_wallpaperfile+"\"");
                else if(_Parameter==9) {
                    // XFCE
                    // detect all monitors (_xfce4_detect_monitors()); set wallpaper for all monitors in a second part
                    _xfce4_detect_monitors();
                    for (int i = 0; i < _detected_monitors.size(); i++)
                    {
                        QString _selected_monitor = _detected_monitors.at(i);
                        setWallpaper.execute("xfconf-query --channel xfce4-desktop --property "+_selected_monitor+" --set \""+_wallpaperfile+"\"");
                        QThread::msleep(100);
                    }
                }
            }
        }
    }
}

void setWallpaper::_xfce4_detect_monitors()
{
    QProcess _list_monitors;

    _list_monitors.start("xfconf-query -c xfce4-desktop -l | grep \"last-image\" ");
    _list_monitors.waitForStarted();
    if(_list_monitors.state()==QProcess::NotRunning)
            qDebug() << "Monitor detection failed!";
    _list_monitors.waitForFinished();

    out = _list_monitors.readAllStandardOutput();
    _detected_monitors = out.split("\n");
}

void setWallpaper::_create_bashfile()
{
    // Create a scriptfile

    _scriptfile = QDir::homePath()+"/.DailyDesktopWallpaperPlus/spw.sh";

    QFile set_wallpaper_plasma(_scriptfile);
    if(!set_wallpaper_plasma.exists(_scriptfile))
    {
        QString content = "#!/bin/bash\n"
                "wallpaperDir=\""+_WallpaperDir+"\"\n"
                "\n"
                "WallpaperDir="+QApplication::applicationDirPath()+"/"+"DailyDesktopWallpaperPlus_wallpapers\n"
                " \n"
                "dbus-send --session --dest=org.kde.plasmashell --type=method_call /PlasmaShell org.kde.PlasmaShell.evaluateScript 'string: \n"
                "var Desktops = desktops(); \n"
                "print (Desktops); \n"
                "for (i=0;i<Desktops.length;i++) {\n"
                "        d = Desktops[i];\n"
                "        d.wallpaperPlugin = \"org.kde.image\";\n"
                "        d.currentConfigGroup = Array(\"Wallpaper\",\n"
                "                                    \"org.kde.image\",\n"
                "                                    \"General\");\n"
                "        d.writeConfig(\"Image\", \"file://"+_wallpaperfile+"\");\n"
                "}'";

        if (set_wallpaper_plasma.open(QIODevice::Append))
        {
            QTextStream stream(&set_wallpaper_plasma);
            stream <<content<<endl;
        }
    }
}

void setWallpaper::_remove_bashfile()
{
    QFile sr(_scriptfile);
    sr.remove();
}
