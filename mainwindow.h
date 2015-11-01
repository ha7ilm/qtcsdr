/*
This software is part of qtcsdr.

Copyright (c) 2015, Andras Retzler <randras@sdr.hu>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ANDRAS RETZLER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProcess>
#include <QTimer>
#include <QTextStream>

#define RTLTCP_SET_FREQ 0x1
#define RTLTCP_SET_DIRECT_SAMPLING 0x9

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
    void setShift();
    void on_shiftChanged(int newOffset);

    void on_spinOffset_valueChanged(int arg1);

    void on_spinCenter_valueChanged(int arg1);

    void on_comboDirectSamp_currentIndexChanged(int index);

    void on_toggleTransmit_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QList<QPushButton*> modsButtons;
    void untoggleOtherModButtonsThan(QPushButton* pb);
    void sendCommand(unsigned char cmd_num, unsigned value);
    QString getDemodulatorCommand();
    void redirectProcessOutput(QProcess &proc, bool onlyStdErr = false);
    void updateFilterBw();
    QString getNextArgAfter(QString what);
    QString getModulatorCommand();
    QProcess procDemod;
    QProcess procDistrib;
    QProcess procIQServer;
    QProcess procFFT;
    QProcess procTX;
    QProcess procKillTX;
    QString fifoPipePath;
    int fifoPipe;
    QTimer tmrRead;
    QTextStream qStdOut;
    QByteArray FFTDataBuffer;
    QString audioPlayerCommand;
    QString audioRecordCommand;
    QString alsaDevice;
};

#endif // MAINWINDOW_H
