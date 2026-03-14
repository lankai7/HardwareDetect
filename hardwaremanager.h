/**
 * @brief   :硬件信息管理类
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/

#ifndef HARDWAREMANAGER_H
#define HARDWAREMANAGER_H

#include "hardwareinfo.h"
#include "wmimanager.h"

#pragma pack(push,1)

struct SMBIOSHeader
{
    quint8 type;
    quint8 length;
    quint16 handle;
};

struct SMBIOSType17
{
    SMBIOSHeader header;

    quint16 physicalMemoryArrayHandle;
    quint16 memoryErrorInfoHandle;
    quint16 totalWidth;
    quint16 dataWidth;
    quint16 size;

    quint8 formFactor;
    quint8 deviceSet;
    quint8 deviceLocator;
    quint8 bankLocator;

    quint8 memoryType;
    quint16 typeDetail;

    quint16 speed;

    quint8 manufacturer;
    quint8 serialNumber;
    quint8 assetTag;
    quint8 partNumber;
};

#pragma pack(pop)

class HardwareManager
{
public:

    HardwareManager();

    // 读取全部硬件信息
    void loadAll();

    CPUInfo cpu;
    QList<GPUInfo> gpus;
    MotherboardInfo motherboard;
    QList<MemorySlot> memories;
    QList<DiskInfo> disks;
    QList<NetworkInfo> networks;
    QList<MonitorInfo> monitors;
    QList<AudioInfo> audios;
    SystemInfo system;

private:

    void readCPU();
    void readGPU();
    void readMotherboard();
    void readMemory();
    void readDisk();
    void readNetwork();
    void readMonitor();
    void readAudio();
    void readSystem();
    void readMemorySMBIOS();

private:

    WMIManager mWMI;
    QString memoryTypeToString(int type);
    QString decodeEDID(const QString &data);
};

#endif
