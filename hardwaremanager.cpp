#include "hardwaremanager.h"
#include "gpureader.h"
#include <windows.h>
#include <QDebug>

HardwareManager::HardwareManager()
{
    if(!mWMI.init())
        qDebug()<<"WMI init failed";
}

void HardwareManager::loadAll()
{
    // 清空旧数据
    memories.clear();
    disks.clear();
    networks.clear();
    monitors.clear();
    audios.clear();
    gpus.clear();

    readCPU();
    readGPU();
    readMotherboard();
//    readMemory();
    readMemorySMBIOS();
    readDisk();
    readNetwork();
    readMonitor();
    readAudio();
    readSystem();
}

/**
 * @brief   :SMBIOS内存类型转字符串
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/
QString HardwareManager::memoryTypeToString(int type)
{
    switch(type)
    {
        case 20: return "DDR";
        case 21: return "DDR2";
        case 24: return "DDR3";
        case 26: return "DDR4";
        case 34: return "DDR5";
        default: return "";
    }
}

QByteArray getSMBIOS()
{
    DWORD size = GetSystemFirmwareTable('RSMB',0,nullptr,0);

    if(size == 0)
        return QByteArray();

    QByteArray buffer;
    buffer.resize(size);

    GetSystemFirmwareTable('RSMB',0,buffer.data(),size);

    return buffer;
}

QString getSMBIOSString(uchar* structStart,int index)
{
    if(index == 0)
        return "";

    char* str = (char*)structStart;

    str += ((SMBIOSHeader*)structStart)->length;

    int current = 1;

    while(current < index)
    {
        while(*str != 0) str++;
        str++;
        current++;
    }

    return QString::fromLocal8Bit(str);
}

/**
 * @brief   :通过SMBIOS读取内存信息
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/
void HardwareManager::readMemorySMBIOS()
{
    QByteArray data = getSMBIOS();

    if(data.isEmpty())
        return;

    uchar* ptr = (uchar*)data.data() + 8;
    uchar* end = (uchar*)data.data() + data.size();

    while(ptr < end)
    {
        SMBIOSHeader* header = (SMBIOSHeader*)ptr;

        if(header->type == 17)
        {
            SMBIOSType17* mem = (SMBIOSType17*)ptr;

            if(mem->size != 0 && mem->size != 0xFFFF)
            {
                MemorySlot slot;

                // 品牌
                slot.brand = getSMBIOSString(ptr,mem->manufacturer).trimmed();

                // 内存类型
                QString type = memoryTypeToString(mem->memoryType);

                // 容量
                int gb = mem->size / 1024;

                // 频率
                int speed = mem->speed;

                // 生成展示字符串
                slot.model = QString("%1 %2GB %3MHz")
                                .arg(type)
                                .arg(gb)
                                .arg(speed);

                memories.append(slot);
            }
        }

        // 找到下一个结构
        uchar* next = ptr + header->length;

        while(next < end && !(next[0]==0 && next[1]==0))
            next++;

        next += 2;

        ptr = next;
    }
}


void HardwareManager::readCPU()
{
    cpu.name = mWMI.queryFirst("Win32_Processor","Name");

    QString freq = mWMI.queryFirst("Win32_Processor","MaxClockSpeed");

    if(!freq.isEmpty())
        cpu.frequency = freq + " MHz";
}

void HardwareManager::readGPU()
{
    gpus = GPUReader::readGPU();
}

void HardwareManager::readMotherboard()
{
    motherboard.brand = mWMI.queryFirst("Win32_BaseBoard","Manufacturer");
    motherboard.model = mWMI.queryFirst("Win32_BaseBoard","Product");
}

//没用
void HardwareManager::readMemory()
{
    QStringList brands  = mWMI.queryAll("Win32_PhysicalMemory","Manufacturer");
    QStringList sizes   = mWMI.queryAll("Win32_PhysicalMemory","Capacity");
    QStringList speeds  = mWMI.queryAll("Win32_PhysicalMemory","ConfiguredClockSpeed");
    QStringList models  = mWMI.queryAll("Win32_PhysicalMemory","PartNumber");

    int count = sizes.size();

    for(int i=0;i<count;i++)
    {
        MemorySlot slot;

        // 品牌
        if(i < brands.size() && !brands[i].isEmpty())
            slot.brand = brands[i].trimmed();
        else
            slot.brand = "Unknown";

        // 容量
        double gb = sizes[i].toDouble()/(1024.0*1024*1024);
        slot.capacity = QString("%1 GB").arg(qRound(gb));

        // 实际运行频率
        if(i < speeds.size() && !speeds[i].isEmpty())
            slot.speed = speeds[i] + " MHz";
        else
            slot.speed = "Unknown";

        // 型号
        if(i < models.size())
            slot.model = models[i].trimmed();

        memories.append(slot);
    }
}

void HardwareManager::readDisk()
{
    QStringList models = mWMI.queryAll("Win32_DiskDrive","Model");
    QStringList sizes  = mWMI.queryAll("Win32_DiskDrive","Size");
    QStringList iface  = mWMI.queryAll("Win32_DiskDrive","InterfaceType");

    int count = qMin(models.size(), qMin(sizes.size(), iface.size()));

    QSet<QString> exist; // 去重

    for(int i=0;i<count;i++)
    {
        QString model = models[i];

        if(exist.contains(model))
            continue;

        exist.insert(model);

        DiskInfo disk;

        disk.model = model;

        // 容量
        double gb = sizes[i].toDouble()/(1024.0*1024*1024);
        int display = qRound(gb);

        disk.size = QString("%1 GB").arg(display);

        QString lower = model.toLower();

        // 类型识别
        if(lower.contains("nvme"))
        {
            disk.type = "NVMe";
        }
        else if(lower.contains("ssd"))
        {
            disk.type = "SSD";
        }
        else if(iface[i].contains("SCSI",Qt::CaseInsensitive))
        {
            // 很多NVMe在WMI里会显示SCSI
            disk.type = "NVMe";
        }
        else
        {
            disk.type = "HDD";
        }

        disks.append(disk);
    }
}

void HardwareManager::readNetwork()
{
    networks.clear();

    QStringList names = mWMI.queryAll("Win32_NetworkAdapter","Name");
    QStringList macs  = mWMI.queryAll("Win32_NetworkAdapter","MACAddress");

    for(int i=0;i<names.size();i++)
    {
        QString name = names[i];

        if(name.contains("WAN Miniport",Qt::CaseInsensitive)) continue;
        if(name.contains("Adapter",Qt::CaseInsensitive)) continue;
        if(name.contains("Kernel Debug",Qt::CaseInsensitive)) continue;
        if(name.contains("Virtual",Qt::CaseInsensitive)) continue;
        if(name.contains("VMware",Qt::CaseInsensitive)) continue;
        if(name.contains("Hyper-V",Qt::CaseInsensitive)) continue;
        if(name.contains("Bluetooth",Qt::CaseInsensitive)) continue;

        NetworkInfo net;

        net.name = name;

        networks.append(net);
    }
}
/**
 * @brief   :读取显示器信息
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/

void HardwareManager::readMonitor()
{
    monitors.clear();

    QStringList brands =
        mWMI.queryAll("ROOT\\WMI","WmiMonitorID","ManufacturerName");

    QStringList models =
        mWMI.queryAll("ROOT\\WMI","WmiMonitorID","UserFriendlyName");

    int count = qMin(brands.size(), models.size());

    for(int i=0;i<count;i++)
    {
        MonitorInfo mon;

        mon.brand = brands[i];
        mon.model = models[i];

        // 过滤无效设备
        if(mon.brand.isEmpty() && mon.model.isEmpty())
            continue;

        monitors.append(mon);
    }
}

void HardwareManager::readAudio()
{
    QStringList auds = mWMI.queryAll("Win32_SoundDevice","Name");

    QSet<QString> existNames;   // 用于去重

    for(const QString &a : auds)
    {
        // 过滤虚拟设备
        if(a.contains("Steam",Qt::CaseInsensitive)) continue;
        if(a.contains("Virtual",Qt::CaseInsensitive)) continue;
        if(a.contains("Sonar",Qt::CaseInsensitive)) continue;
        if(a.contains("Streaming",Qt::CaseInsensitive)) continue;
        if(a.contains("VB-Audio",Qt::CaseInsensitive)) continue;
        if(a.contains("Voicemeeter",Qt::CaseInsensitive)) continue;
        if(a.contains("CABLE",Qt::CaseInsensitive)) continue;

        // 去重
        if(existNames.contains(a))
            continue;

        existNames.insert(a);

        AudioInfo au;
        au.name = a;

        audios.append(au);
    }
}

void HardwareManager::readSystem()
{
    system.os = mWMI.queryFirst("Win32_OperatingSystem","Caption");
}
