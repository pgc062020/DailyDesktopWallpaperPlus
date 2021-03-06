#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <autostart_and_menuitem.h>
#include <QDialog>
#include <QString>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

    bool _set_reset;

private slots:
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_checkBox_3_clicked();
    void on_checkBox_4_clicked();
    void on_pushButton_5_clicked();

private:
    Ui::SettingsWindow *ui;
    Autostart_and_MenuItem autostart_and_menuitem;
    QString _iniFilePath;
    QString _OldWallpaperDir;
    QString select_dir;
    QString select_dir_2;
    QString _WallpaperDir;

    bool _Autostart;
    bool _AutoChange;
    bool _SaveOldWallpaper;
    bool _create_menu_item;
    bool _delete_automatically;

    int _delete_older_than;
    int _time_hours;
    int _time_minutes;

    void init_settings();
    void write_settings();
    void selectOldWallpaperDir();
    void selectWallpaperDir();
    void closeEvent(QCloseEvent * event);
    void reject();
};

#endif // SETTINGSWINDOW_H
