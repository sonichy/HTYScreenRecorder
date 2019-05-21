#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QStandardPaths>
#include <QDateTime>
#include <QTimer>
#include <QWindow>
#include <QScreen>
#include <QDebug>
#include <QShortcut>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_stop->hide();
    invert = false;
    setAttribute(Qt::WA_TranslucentBackground, true);
    //setAttribute(Qt::WA_TransparentForMouseEvents);
    winFlags = windowFlags();
    width_screen = QApplication::desktop()->width();
    height_screen = QApplication::desktop()->height();
    move((width_screen - width())/2, (height_screen - height())/2);
    ui->spinBox_width->setRange(10, width_screen);
    ui->spinBox_height->setRange(10, height_screen);
    connect(ui->spinBox_width, SIGNAL(valueChanged(int)), this, SLOT(spinBoxWidthChanged(int)));
    connect(ui->spinBox_height, SIGNAL(valueChanged(int)), this, SLOT(spinBoxHeightChanged(int)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(insertImageToGIF()));

    timer_invert = new QTimer(this);
    connect(timer_invert, SIGNAL(timeout()), this, SLOT(invertIcon()));

    systray = new QSystemTrayIcon;
    systray->setToolTip("海天鹰GIF录屏");
    systray->setIcon(QIcon(":/icon.svg"));
    connect(systray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    connect(new QShortcut(QKeySequence(Qt::Key_Escape),this), SIGNAL(activated()), this, SLOT(showNormal()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::paintEvent(QPaintEvent *event)
//{

//}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->spinBox_width->setValue(width());
    if(!isFullScreen())
        ui->spinBox_height->setValue(height() - ui->control->height());
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() != QEvent::WindowStateChange) return;
    if (windowState() == Qt::WindowNoState){
        ui->control->show();
    }
}

//void MainWindow::moveEvent(QMoveEvent *event)
//{

//}

void MainWindow::on_pushButton_fullscreen_clicked()
{
    showFullScreen();
    ui->spinBox_width->setValue(QApplication::desktop()->width());
    ui->spinBox_height->setValue(QApplication::desktop()->height());
}

void MainWindow::on_pushButton_start_clicked()
{
    count = 0;
    ui->pushButton_start->hide();
    ui->pushButton_stop->show();
    systray->show();
    QEventLoop eventloop;
    QTimer::singleShot(500, &eventloop, SLOT(quit()));
    eventloop.exec();
    if(ui->checkBox->checkState() == Qt::Checked){
        QTimer::singleShot(10000, &eventloop, SLOT(quit()));
        eventloop.exec();
    }
    QDateTime time = QDateTime::currentDateTime();
    QString filename = time.toString("yyyyMMddhhmmss") + ".gif";
    filepath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + filename;
    GifBegin(&GW, filepath.toUtf8().data(), ui->spinBox_width->value(), ui->spinBox_height->value(), ui->spinBox_delay->value());
    timer->start(ui->spinBox_delay->value());
    if(ui->spinBox_delay->value() < 1000){
        timer_invert->start(1000);
    }else{
        timer_invert->start(ui->spinBox_delay->value());
    };
    //setWindowFlags(Qt::X11BypassWindowManagerHint);
    if (isFullScreen()) { showMinimized(); }
    QApplication::beep();
}

void MainWindow::on_pushButton_stop_clicked()
{
    ui->pushButton_start->show();
    ui->pushButton_stop->hide();
    timer->stop();
    timer_invert->stop();
    systray->hide();
    GifEnd(&GW);
    setWindowFlags(winFlags);
}

void MainWindow::spinBoxWidthChanged(int w)
{
    resize(w, height());
}

void MainWindow::spinBoxHeightChanged(int h)
{
    resize(width(), h + ui->control->height());
}

void MainWindow::insertImageToGIF()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;
    QPixmap pixmap = screen->grabWindow(qApp->desktop()->winId(), geometry().x(), geometry().y(), ui->spinBox_width->value(), ui->spinBox_height->value());
    QPixmap pixmap_mouse(":/mouse.png");
    QPainter painter(&pixmap);
    //qDebug() << cursor().pos();
    painter.drawPixmap(QPoint(cursor().pos().x() - geometry().x(), cursor().pos().y() - geometry().y()), pixmap_mouse);
    GifWriteFrame(&GW, pixmap.toImage().bits(), ui->spinBox_width->value(), ui->spinBox_height->value(), ui->spinBox_delay->value());
    filesize = BS(QFileInfo(filepath).size());
    ui->label_filesize->setText(filesize);
    count++;
    ui->label_count->setText(QString::number(count) + "帧");
    systray->setToolTip(QString::number(count) + "帧\n" + filesize);
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    qDebug() << reason;
    switch(reason) {
    case QSystemTrayIcon::Trigger:
        timer->stop();
        timer_invert->stop();
        GifEnd(&GW);
        //setWindowFlags(winFlags);
        ui->pushButton_start->show();
        ui->pushButton_stop->hide();
        showNormal();
        systray->hide();
        break;
    default:
        break;
    }
}

void MainWindow::invertIcon()
{
    if(invert){
        systray->setIcon(QIcon(":/icon.svg"));
    }else{
        systray->setIcon(QIcon(":/iconi.svg"));
    }
    invert = !invert;
}

QString MainWindow::BS(long b)
{
    QString s = "";
    if(b > 999999999){
        s = QString::number(b/(1024*1024*1024.0), 'f', 2) + "GB";
    }else{
        if(b > 999999){
            s = QString::number(b/(1024*1024.0), 'f', 2) + "MB";
        }else{
            if(b>999){
                s = QString::number(b/(1024.0), 'f',2) + "KB";
            }else{
                s = b + "B";
            }
        }
    }
    return s;
}