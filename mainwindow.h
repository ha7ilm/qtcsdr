#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProcess>
#include <QTimer>
#include <QTextStream>

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
    void on_spinFreq_valueChanged(int val);
    void tmrRead_timeout();

private:
    Ui::MainWindow *ui;
    QList<QPushButton*> modsButtons;
    void untoggleOtherModButtonsThan(QPushButton* pb);
    void sendCommand(unsigned char cmd_num, unsigned value);
    QString getDemodulatorCommand();
    void redirectProcessOutput(QProcess &proc, bool onlyStdErr = false);
    void updateFilterBw();
    QProcess procDemod;
    QProcess procDistrib;
    QProcess procIQServer;
    QProcess procFFT;
    QString fifoPipe;
    QTimer tmrRead;
    QTextStream qStdOut;
    QByteArray FFTDataBuffer;


};

#endif // MAINWINDOW_H
