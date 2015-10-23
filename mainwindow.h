#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProcess>
#include <QTimer>

#define RTLTCP_SET_FREQ 0x1

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_toggleWFM_toggled(bool checked);
    void on_toggleNFM_toggled(bool checked);
    void on_toggleAM_toggled(bool checked);
    void on_toggleUSB_toggled(bool checked);
    void on_toggleLSB_toggled(bool checked);
    void on_toggleRun_toggled(bool checked);
    void on_spinFreq_valueChanged();
    void tmrRead_timeout();

private:
    Ui::MainWindow *ui;
    QList<QPushButton*> modsButtons;
    void untoggleOtherModButtonsThan(QPushButton* pb);
    void sendCommand(unsigned char cmd_num, unsigned value);
    QProcess procDemod;
    QString fifoPipe;
    QTimer tmrRead;

};

#endif // MAINWINDOW_H
