#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "photobrowser.h"

#include <cstdio>
#include <ctime>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>
#include <QProcess>
#include <QPixmap>
#include <QActionGroup>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qApp->setAttribute(Qt::AA_DontShowIconsInMenus, false);

    _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    set_values();
    init_MainContextMenu();
    init_SystemTrayIcon();
    check_dir();
    load_wallpaper();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_values()
{
    QString _iniFileDir = QDir::homePath()+"/.DailyDesktopWallpaperPlus";

    QDir settings_dir(_iniFileDir);
    if (!settings_dir.exists())
    {
        settings_dir.mkpath(_iniFileDir);
    }

    // If settings file does not exist then create it with default settings
    QFile iniFile(_iniFilePath);
    if(!iniFile.exists(_iniFilePath))
    {
        QString iniDefaultData = "[BING-SETTINGS]\n"
                "binglocation=en-US\n"
                "resolution=1920x1080\n"
                "\n"
                "[SETTINGS]\n"
                "WallpaperDir="+QDir::homePath()+"/.DailyDesktopWallpaperPlus/background_wallpaper\n"
                "OldWallpaperDir="+QDir::homePath()+"/.DailyDesktopWallpaperPlus/old_Wallpapers\n"
                "Autostart=true\n"
                "SaveOldWallpaper=true\n"
                "Provider=Bing\n"
                "current_photo_dl_url=\n"
                "current_description=\n"
                "current_title=\n"
                "create_menu_item=true\n"
                "\n"
                "[SETWALLPAPER]\n"
                "AutoChange=true\n"
                "Parameter=0\n"
                "\n"
                "[VERSION]\n"
                "Version="+_appVersion+"\n";

        if (iniFile.open(QIODevice::Append))
        {
            QTextStream stream(&iniFile);
            stream <<iniDefaultData<<endl;
        }
    }

    // read settings from INI file
    QSettings settings(_iniFilePath, QSettings::IniFormat);

    settings.beginGroup("BING-SETTINGS");
    _resolution = settings.value("resolution","").toString();
    _binglocation = settings.value("binglocation","").toString();
    settings.endGroup();

    settings.beginGroup("SETTINGS");
    _Autostart = settings.value("Autostart","").toBool();
    _WallpaperDir = settings.value("WallpaperDir","").toString();
    _OldWallpaperDir = settings.value("OldWallpaperDir","").toString();
    _SaveOldWallpaper = settings.value("SaveOldWallpaper","").toBool();
    _create_menu_item = settings.value("create_menu_item","").toBool();
    settings.endGroup();

    settings.beginGroup("SETWALLPAPER");
    _AutoChange = settings.value("AutoChange","").toBool();
    _Parameter = settings.value("Parameter","").toInt();
    settings.endGroup();

    settings.beginGroup("SETTINGS");
    _Provider = settings.value("Provider","").toString();
    settings.endGroup();

    _appVersion = "1.0";
    _write_AppVersion();

    if (_Autostart == true)
    {
        set_autostart();
    }
    else
    {
        no_autostart();
    }

    if (_create_menu_item == true)
    {
        set_menu_item();
    }
    else
    {
        no_menu_item();
    }
}

void MainWindow::check_dir()
{
    QDir _wallDir(_WallpaperDir);
    QDir _oldwallDir(_OldWallpaperDir);

    if(!_wallDir.exists()) {
        _wallDir.mkpath(_WallpaperDir);
    }

    if(_SaveOldWallpaper == true) {
        if(!_oldwallDir.exists()) {
            _oldwallDir.mkpath(_OldWallpaperDir);
        }
    }
}

void MainWindow::load_wallpaper()
{
    if(_Provider =="Bing") {
        _setBingWallpaper();
    }
    if(_Provider =="WindowsSpotlight") {
        _setWinSpotWallpaper();
    }
}

void MainWindow::no_autostart()
{
    autostart_and_menuitem.no_autostart();
}

void MainWindow::set_autostart()
{
    autostart_and_menuitem.set_autostart();
}

void MainWindow::no_menu_item()
{
    autostart_and_menuitem.no_menuitem();
}

void MainWindow::set_menu_item()
{
    autostart_and_menuitem.set_menuitem();
}

void MainWindow::init_MainContextMenu()
{
    menu = new QMenu(this);

    if (_Provider == "Bing")
    {
        QPixmap refresh(":icons/Download.png");
        QPixmap _loc(":/icons/Computer.png");
        QPixmap _display(":/icons/monitor.png");

        QAction * bingRefresh = menu->addAction(refresh, trUtf8("Refresh Wallpaper"));
        menu->addSeparator();
        QMenu * bingLoc = menu->addMenu(_loc, trUtf8("Bing Location"));
        QMenu * bingRes = menu->addMenu(_display, trUtf8("Resolution of the Wallpaper"));
        menu->addSeparator();

        connect(bingRefresh, SIGNAL(triggered()), this, SLOT(_menu_bingRefresh_click()));

        //create submenus to select bing locations
        QPixmap usa(":countries/us.png");
        QPixmap japan(":countries/japan.png");
        QPixmap china(":countries/china.png");
        QPixmap australia(":countries/australia.png");
        QPixmap gb(":countries/gb.png");
        QPixmap germany(":countries/germany.png");
        QPixmap canada(":countries/canada.png");
        QPixmap nz(":countries/nz.png");
        QPixmap brazil(":countries/brazil.png");
        QPixmap france(":countries/france.png");

        QAction * _usa_ = bingLoc->addAction(usa, trUtf8("USA"));
        QAction * _japan_ = bingLoc->addAction(japan, trUtf8("Japan"));
        QAction * _china_ = bingLoc->addAction(china, trUtf8("China"));
        QAction * _australia_ = bingLoc->addAction(australia, trUtf8("Australia"));
        QAction * _gb_ = bingLoc->addAction(gb, trUtf8("Great Britain"));
        QAction * _germany_ = bingLoc->addAction(germany, trUtf8("Germany"));
        QAction * _canada_ = bingLoc->addAction(canada, trUtf8("Canada"));
        QAction * _nz_ = bingLoc->addAction(nz, trUtf8("New Zealand"));
        QAction * _brazil_ = bingLoc->addAction(brazil, trUtf8("Brazil"));
        QAction * _france_ = bingLoc->addAction(france, trUtf8("France"));

        connect(_usa_, SIGNAL(triggered()), this, SLOT(_menu_usa_click()));
        connect(_japan_, SIGNAL(triggered()), this, SLOT(_menu_japan_click()));
        connect(_china_, SIGNAL(triggered()), this, SLOT(_menu_china_click()));
        connect(_australia_, SIGNAL(triggered()), this, SLOT(_menu_australia_click()));
        connect(_gb_, SIGNAL(triggered()), this, SLOT(_menu_gb_click()));
        connect(_germany_, SIGNAL(triggered()), this, SLOT(_menu_germany_click()));
        connect(_canada_, SIGNAL(triggered()), this, SLOT(_menu_canada_click()));
        connect(_nz_, SIGNAL(triggered()), this, SLOT(_menu_nz_click()));
        connect(_brazil_, SIGNAL(triggered()), this, SLOT(_menu_brazil_click()));
        connect(_france_, SIGNAL(triggered()), this, SLOT(_menu_france_click()));

        // create submenus to select the resolution of the photo
        QAction * _1920x1200_ = bingRes->addAction(trUtf8("1920x1200"));
        QAction * _1920x1080_ = bingRes->addAction(trUtf8("1920x1080"));
        QAction * _1366x768_ = bingRes->addAction(trUtf8("1366x768"));
        QAction * _1280x720_ = bingRes->addAction(trUtf8("1280x720"));
        QAction * _1024x768_ = bingRes->addAction(trUtf8("1024x768"));

        QActionGroup* _bingphoto_resolution_group = new QActionGroup(this);

        _1920x1200_->setCheckable(true);
        _1920x1080_->setCheckable(true);
        _1366x768_->setCheckable(true);
        _1280x720_->setCheckable(true);
        _1024x768_->setCheckable(true);

        _1920x1200_->setActionGroup(_bingphoto_resolution_group);
        _1920x1080_->setActionGroup(_bingphoto_resolution_group);
        _1366x768_->setActionGroup(_bingphoto_resolution_group);
        _1280x720_->setActionGroup(_bingphoto_resolution_group);
        _1024x768_->setActionGroup(_bingphoto_resolution_group);

        connect(_1920x1200_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_1920x1200_click()));
        connect(_1920x1080_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_1920x1080_click()));
        connect(_1366x768_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_1366x768_click()));
        connect(_1280x720_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_1280x720_click()));
        connect(_1024x768_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_1024x768_click()));

        // set item
        if(_resolution=="1920x1200") {
            _1920x1200_->setChecked(true);
        } else if(_resolution=="1920x1080") {
            _1920x1080_->setChecked(true);
        } else if(_resolution=="1366x768") {
            _1366x768_->setChecked(true);
        } else if(_resolution=="1280x720") {
            _1280x720_->setChecked(true);
        } else if(_resolution=="1024x768") {
            _1024x768_->setChecked(true);
        }
    }

    if (_Provider == "WindowsSpotlight")
    {
        QPixmap refresh(":icons/Download.png");
        QAction * wspotRef = menu->addAction(refresh, trUtf8("Refresh Wallpaper"));

        menu->addSeparator();

        connect(wspotRef, SIGNAL(triggered()), this, SLOT(_wspotRef_click()));
    }

    QPixmap _providers(":/icons/Earth.png");
    QPixmap _loadexistwallpaper(":/icons/Upload.png");
    QPixmap _wrench(":/icons/Wrench.png");
    QPixmap _about(":/icons/About.png");
    QPixmap _exit(":/icons/Exit.png");

    QMenu * providers = menu->addMenu(_providers, trUtf8("Providers"));
    QAction * ShowPhotoBrowser = menu->addAction(_loadexistwallpaper, trUtf8("Load existing Wallpaper"));
    menu->addSeparator();
    QAction * settings = menu->addAction(_wrench, trUtf8("Settings"));
    QAction * about = menu->addAction(_about, trUtf8("About"));
    menu->addSeparator();
    QAction * exitApp = menu->addAction(_exit, trUtf8("Exit"));

    connect(ShowPhotoBrowser, SIGNAL(triggered()), this, SLOT(_show_photobrowser_click()));
    connect(settings, SIGNAL(triggered()), this, SLOT(_menu_settings_click()));
    connect(about, SIGNAL(triggered()), this, SLOT(_menu_about_click()));
    connect(exitApp, SIGNAL(triggered()), this, SLOT(close()));

    // Create submenus of providers
    QAction * _winspot_wall_option = providers->addAction(trUtf8("Photo of Windows Spotlight"));
    QAction * _bing_wall_option = providers->addAction(trUtf8("Bing's picture of the day"));

    QActionGroup* _providers_group = new QActionGroup(this);

    _winspot_wall_option->setCheckable(true);
    _bing_wall_option->setCheckable(true);

    _winspot_wall_option->setActionGroup(_providers_group);
    _bing_wall_option->setActionGroup(_providers_group);

    connect(_winspot_wall_option, SIGNAL(triggered()), this, SLOT(_menu_winspot_wall_option_click()));
    connect(_bing_wall_option, SIGNAL(triggered()), this, SLOT(_menu_bing_wall_option_click()));

    if(_SaveOldWallpaper == false) {
        ShowPhotoBrowser->setEnabled(false);
    } else {
        ShowPhotoBrowser->setEnabled(true);
    }

    // set item
    if(_Provider=="Bing") {
        _bing_wall_option->setChecked(true);
    } else {
        _winspot_wall_option->setChecked(true);
    }
}

void MainWindow::init_SystemTrayIcon()
{
    mSystemTrayIcon = new QSystemTrayIcon(this);
    mSystemTrayIcon->setIcon(QIcon(":/128.png"));

    mSystemTrayIcon->setContextMenu(menu);
    mSystemTrayIcon->show();
    mSystemTrayIcon->setVisible(true);
}

void MainWindow::_show_photobrowser_click()
{
   PhotoBrowser _photobrowser;
   _photobrowser.setModal(true);
   _photobrowser.exec();
   mSystemTrayIcon->setToolTip("");
}

void MainWindow::_menu_settings_click()
{
    SettingsWindow _sett_win;
    _sett_win.setFixedSize(591,532);
    _sett_win.setModal(true);
    _sett_win.exec();
    set_values();
    init_MainContextMenu();
    mSystemTrayIcon->setContextMenu(menu);
}

void MainWindow::_menu_about_click()
{
    About _about_win;
    _about_win.setFixedSize(433,353);
    _about_win.setModal(true);
    _about_win.exec();

}

void MainWindow::_menu_winspot_wall_option_click()
{
    _Provider = "WindowsSpotlight";
    _write_provider_settings();
    menu->clear();
    init_MainContextMenu();
    mSystemTrayIcon->setContextMenu(menu);
    _setWinSpotWallpaper();
}

void MainWindow::_menu_bing_wall_option_click()
{
    _Provider = "Bing";
    _write_provider_settings();
    menu->clear();
    init_MainContextMenu();
    mSystemTrayIcon->setContextMenu(menu);
    _setBingWallpaper();
}

void MainWindow::_menu_bingRefresh_click()
{
    _setBingWallpaper();
}

void MainWindow::_wspotRef_click()
{
    _setWinSpotWallpaper();
}

void MainWindow::_menu_usa_click()
{
    _selected_binglocation = "en-US";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_japan_click()
{
    _selected_binglocation = "ja-JP";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_china_click()
{
    _selected_binglocation = "zh-CN";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_australia_click()
{
    _selected_binglocation = "en-AU";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_gb_click()
{
    _selected_binglocation = "en-GB";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_germany_click()
{
    _selected_binglocation = "de-DE";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_canada_click()
{
    _selected_binglocation = "en-CA";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_nz_click()
{
    _selected_binglocation = "en-NZ";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_brazil_click()
{
    _selected_binglocation = "pt-BR";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_france_click()
{
    _selected_binglocation = "fr-FR";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_bingRes_1920x1200_click()
{
    _selected_bing_photo_resolution = "1920x1200";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_bingRes_1920x1080_click()
{
    _selected_bing_photo_resolution = "1920x1080";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_bingRes_1366x768_click()
{
    _selected_bing_photo_resolution = "1366x768";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_bingRes_1280x720_click()
{
    _selected_bing_photo_resolution = "1280x720";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_bingRes_1024x768_click()
{
    _selected_bing_photo_resolution = "1024x768";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
}

void MainWindow::_write_provider_settings()
{
    QSettings _select_provider_settings(_iniFilePath, QSettings::IniFormat);

    _select_provider_settings.beginGroup("SETTINGS");
    _select_provider_settings.setValue("Provider", _Provider);
    _select_provider_settings.endGroup();
    _select_provider_settings.sync();
}

void MainWindow::_write_binglocation_settings()
{
    QSettings _bingLocation_settings(_iniFilePath, QSettings::IniFormat);

    _bingLocation_settings.beginGroup("BING-SETTINGS");
    _bingLocation_settings.setValue("binglocation", _selected_binglocation);
    _bingLocation_settings.endGroup();
    _bingLocation_settings.sync();
}

void MainWindow::_write_bing_photo_resolution_settings()
{
    QSettings _bing_photo_resolution_settings(_iniFilePath, QSettings::IniFormat);

    _bing_photo_resolution_settings.beginGroup("BING-SETTINGS");
    _bing_photo_resolution_settings.setValue("resolution", _selected_bing_photo_resolution);
    _bing_photo_resolution_settings.endGroup();
    _bing_photo_resolution_settings.sync();
}

void MainWindow::_write_AppVersion()
{
    QSettings _version_settings(_iniFilePath, QSettings::IniFormat);

    _version_settings.beginGroup("VERSION");
    _version_settings.setValue("Version", _appVersion);
    _version_settings.endGroup();
    _version_settings.sync();
}

void MainWindow::_check_internet_connection()
{
    //check internet connection as fist

    QString _check_url = "http://www.bing.com";

    QEventLoop loop;
    QObject::connect(&dl_manager,&QNetworkAccessManager::finished,&loop,&QEventLoop::quit);
    reply = dl_manager.get(QNetworkRequest(_check_url));
    loop.exec();

    if (reply->bytesAvailable())
    {
        _keeporremove_old_wallpaper();
        if(_Provider =="Bing") {
            getbingwallpaper.get_bing_wallpaper();
            set_tooltip_string();
        }
        if(_Provider =="WindowsSpotlight") {
            getwinspotwallpaper.get_wallpaper();
            set_tooltip_string();
        }
    }
    else
    {
        _tooltip_title ="Error";
        _tooltip_message = "Connection to the internet failed!";
        _show_tooltip();
    }
}

void MainWindow::_keeporremove_old_wallpaper()
{
    if (_SaveOldWallpaper == true)
    {
        QString filename = "background.jpg";
        QString filename_new = "background_old.jpg";

        QFile old_bgfile(_OldWallpaperDir+"/"+filename_new);

        if(old_bgfile.exists()) {
            //create timestamp
            std::time_t rawtime;
            std::tm* timeinfo;
            char timestamp [80];
            std::time(&rawtime);
            timeinfo = std::localtime(&rawtime);
            std::strftime(timestamp,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
            std::puts(timestamp);

            QString tmstmp = timestamp;

            QString filename_new2 = "background"+tmstmp+".jpg";
            QFile::copy(_OldWallpaperDir+"/"+filename_new, _OldWallpaperDir+"/"+filename_new2);

            old_bgfile.remove();
    }

        QFile::copy(_WallpaperDir+"/"+filename, _OldWallpaperDir+"/"+filename_new);

        QFile _current_backgroundfile(_WallpaperDir+"/"+filename);
        _current_backgroundfile.remove();
    }
    else
    {
        QFile background_file(_WallpaperDir+"/background.jpg");
        background_file.remove();
    }
}

void MainWindow::set_tooltip_string()
{
    if(_Provider == "Bing")
    {
        _tooltip_title = getbingwallpaper._headline_bing_desc;
        _tooltip_message = getbingwallpaper._copyright_bing_photo;
    } else
    {
        _tooltip_title = getwinspotwallpaper._wspot_title_text;
        _tooltip_message = getwinspotwallpaper._wspot_photo_description;
    }
}

void MainWindow::_show_tooltip()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
    mSystemTrayIcon->showMessage(_tooltip_title, _tooltip_message, icon, 5000);
    mSystemTrayIcon->setToolTip("Description: \n"+_tooltip_message);
}

void MainWindow::_setBingWallpaper()
{
    _check_internet_connection();
    _show_tooltip();
    _setwall._set_wallpaper();
}

void MainWindow::_setWinSpotWallpaper()
{
    _check_internet_connection();
    _show_tooltip();
    _setwall._set_wallpaper();
}