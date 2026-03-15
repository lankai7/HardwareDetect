#include "mainwindow.h"
#include "hardwarecard.h"

#include <QScrollArea>
#include <QVBoxLayout>

QString joinLines(const QStringList &list)
{
    return list.join("\n");
}

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent)
{
    initUI();

    loadHardware();

    buildCards();
}

void MainWindow::initUI()
{
    QScrollArea *scroll = new QScrollArea;

    scroll->setWidgetResizable(true);

    setCentralWidget(scroll);

    mContainer = new QWidget;

    scroll->setWidget(mContainer);

    mGrid = new QGridLayout(mContainer);

    mGrid->setSpacing(15);
    mGrid->setContentsMargins(20,20,20,20);

    setMinimumSize(900,700);
}

void MainWindow::loadHardware()
{
    mHardware.loadAll();
}

/**
 * @brief   :构建硬件信息卡片
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/
void MainWindow::buildCards()
{
    int row = 0;

    /* ================= CPU ================= */

    QString cpuText = mHardware.cpu.name;

    if(!mHardware.cpu.frequency.isEmpty())
        cpuText += "\n" + mHardware.cpu.frequency;

    HardwareCard *cpu =
            new HardwareCard(" ▌CPU", cpuText);


    /* ================= GPU ================= */

    QStringList gpuList;

    for(const auto &gpu : mHardware.gpus)
    {
        QString text = gpu.name;

        if(!gpu.memory.isEmpty())
            text += "  (" + gpu.memory + ")";

        gpuList << text;
    }

    HardwareCard *gpu =
            new HardwareCard(" ▌显卡", gpuList.join("\n"));


    /* ================= 主板 ================= */

    QString boardText =
            mHardware.motherboard.brand + " "
            + mHardware.motherboard.model;

    HardwareCard *board =
            new HardwareCard(" ▌主板", boardText);


    /* ================= 内存 ================= */

    QStringList memList;

    for(const auto &mem : mHardware.memories)
    {
        QString text =
                mem.brand + " "
                + mem.model + " "
                + mem.capacity + " "
                + mem.speed;

        memList << text;
    }

    HardwareCard *mem =
            new HardwareCard(" ▌内存", memList.join("\n"));


    /* ================= 硬盘 ================= */

    QStringList diskList;

    for(const auto &disk : mHardware.disks)
    {
        QString text =
                disk.model + " ("
                + disk.size + " "
                + disk.type + ")";

        diskList << text;
    }

    HardwareCard *disk =
            new HardwareCard(" ▌硬盘", diskList.join("\n"));


    /* ================= 显示器 ================= */

    QStringList monList;

    for(const auto &mon : mHardware.monitors)
    {
        QString text =
                mon.brand + " "
                + mon.model;

        if(!mon.resolution.isEmpty())
            text += "  " + mon.resolution;

        monList << text;
    }

    HardwareCard *monitor =
            new HardwareCard(" ▌显示器", monList.join("\n"));


    /* ================= 网卡 ================= */

    QStringList netList;

    for(const auto &net : mHardware.networks)
    {
        QString text = net.name;

        if(!net.mac.isEmpty())
            text += "\nMAC: " + net.mac;

        netList << text;
    }

    HardwareCard *net =
            new HardwareCard(" ▌网卡", netList.join("\n"));


    /* ================= 声卡 ================= */

    QStringList audioList;

    for(const auto &au : mHardware.audios)
    {
        audioList << au.name;
    }

    HardwareCard *audio =
            new HardwareCard(" ▌声卡", audioList.join("\n"));


    /* ================= 系统 ================= */

    HardwareCard *system =
            new HardwareCard(" ▌系统", mHardware.system.os);


    /* ================= 布局 ================= */

    mGrid->addWidget(cpu,row,0);
    mGrid->addWidget(gpu,row,1);
    row++;

    mGrid->addWidget(board,row,0);
    mGrid->addWidget(mem,row,1);
    row++;

    mGrid->addWidget(disk,row,0);
    mGrid->addWidget(monitor,row,1);
    row++;

    mGrid->addWidget(net,row,0);
    mGrid->addWidget(audio,row,1);
    row++;

    mGrid->addWidget(system,row,0);
}
