#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gif.h"

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QFileInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);

private:
    Ui::MainWindow *ui;
    int width_screen, height_screen, count;
    GifWriter GW;
    QTimer *timer, *timer_invert;
    Qt::WindowFlags winFlags;
    QSystemTrayIcon *systray;
    QString filepath;
    bool invert;
    QString filesize;
    QString BS(long b);

private slots:
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void on_pushButton_fullscreen_clicked();
    void spinBoxWidthChanged(int w);
    void spinBoxHeightChanged(int w);
    void insertImageToGIF();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void invertIcon();

};

#endif // MAINWINDOW_H