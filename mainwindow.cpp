#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QProcess>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

//#define CMD_RTLTCP "rtl_tcp -s 2400000 -p 4950 -f 89500000"
//#define CMD_DISTRIB "bash -c \"ncat localhost 4950 | ncat -4l 4951 -k --send-only --allow 127.0.0.1\""
//#define CMD_WFM "bash -c \"ncat localhost 4951 | csdr convert_u8_f | csdr shift_addition_cc -0.085 | csdr fir_decimate_cc 10 0.05 HAMMING | csdr fmdemod_quadri_cf | csdr fractional_decimator_ff 5 | csdr deemphasis_wfm_ff 48000 50e-6 | csdr convert_f_i16 | mplayer -cache 1024 -quiet -rawaudio samplesize=2:channels=1:rate=48000 -demuxer rawaudio -\""
//#define CMD_NFM "bash -c \"ncat localhost 4951 | csdr convert_u8_f | csdr shift_addition_cc -0.085 | csdr fir_decimate_cc 50 0.005 HAMMING | csdr fmdemod_quadri_cf | csdr limit_ff | csdr deemphasis_nfm_ff 48000 | csdr fastagc_ff | csdr convert_f_i16 | mplayer -cache 1024 -quiet -rawaudio samplesize=2:channels=1:rate=48000 -demuxer rawaudio -\""

#define CMD_WFM "pgroup -9 bash -c \"rtl_tcp -s 2400000 -p 4951 -f 89500000 & (sleep 1; nc localhost 4951 | csdr convert_u8_f | csdr shift_addition_cc -0.085 | csdr fir_decimate_cc 10 0.05 HAMMING | csdr fmdemod_quadri_cf | csdr fractional_decimator_ff 5 | csdr deemphasis_wfm_ff 48000 50e-6 | csdr convert_f_i16 | mplayer -cache 768 -quiet -rawaudio samplesize=2:channels=1:rate=48000 -demuxer rawaudio -)\""


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    modsButtons.append(ui->toggleAM);
    modsButtons.append(ui->toggleNFM);
    modsButtons.append(ui->toggleWFM);
    modsButtons.append(ui->toggleLSB);
    modsButtons.append(ui->toggleUSB);
    connect(&tmrRead, SIGNAL(timeout()), this, SLOT(tmrRead_timeout()));
    tmrRead.start(100);
}

void MainWindow::untoggleOtherModButtonsThan(QPushButton* pb)
{
    static bool protect;
    if(protect) return;
    protect = true;
    foreach(QPushButton* ipb, modsButtons) if(ipb!=pb) ipb->setChecked(false); else pb->setChecked(true);
    protect = false;
}

void MainWindow::tmrRead_timeout()
{
    QString temp = procDemod.readAll();
    if(temp.length()) qDebug() << temp;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_toggleWFM_toggled(bool checked) { untoggleOtherModButtonsThan(ui->toggleWFM); }
void MainWindow::on_toggleNFM_toggled(bool checked) { untoggleOtherModButtonsThan(ui->toggleNFM); }
void MainWindow::on_toggleAM_toggled(bool checked)  { untoggleOtherModButtonsThan(ui->toggleAM); }
void MainWindow::on_toggleUSB_toggled(bool checked) { untoggleOtherModButtonsThan(ui->toggleUSB); }
void MainWindow::on_toggleLSB_toggled(bool checked) { untoggleOtherModButtonsThan(ui->toggleLSB); }

void MainWindow::on_toggleRun_toggled(bool checked)
{
    if(checked)
    {
        fifoPipe = QString("/tmp/qtcsdr_shift_pipe_")+QString::number(rand());
        mkfifo(fifoPipe.toStdString().c_str(), 0600);
        procDemod.start(CMD_WFM);
        procDemod.waitForStarted(1000);
        on_spinFreq_valueChanged();

    }
    else
    {
        kill(procDemod.pid(), SIGTERM);
    }
}

void MainWindow::sendCommand(unsigned char cmd_num, unsigned value)
{
    //docs: https://github.com/pinkavaj/rtl-sdr/blob/master/src/rtl_tcp.c#L324
    unsigned char cmd[5];
    cmd[0] = cmd_num; //set_freq
    cmd[1] = (value>>24)&0xff;
    cmd[2] = (value>>16)&0xff;
    cmd[3] = (value>>8)&0xff;
    cmd[4] = value&0xff;
    procDemod.write((char*)cmd, 5);
}




void MainWindow::on_spinFreq_valueChanged()
{
    sendCommand(RTLTCP_SET_FREQ, ui->spinFreq->value()-ui->spinOffset->value());
    //procDemod.write("\x01\x05\x55\xa9\x60");
    //procDemod.write("macska\n");
}
