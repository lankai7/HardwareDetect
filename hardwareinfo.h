/**
 * @brief   :硬件信息结构
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/

#ifndef HARDWAREINFO_H
#define HARDWAREINFO_H

#include <QString>
#include <QList>

struct CPUInfo
{
    QString name;
    QString frequency;
};

struct GPUInfo
{
    QString name;
    QString memory;
};

struct MotherboardInfo
{
    QString brand;
    QString model;
};

struct MemorySlot
{
    QString brand;      // 品牌
    QString capacity;   // 容量
    QString speed;      // 频率
    QString model;      // 型号
};

struct DiskInfo
{
    QString model;
    QString size;
    QString type;   // NVMe / SSD / HDD
};

struct NetworkInfo
{
    QString name;
    QString mac;
    bool wireless;
};

struct MonitorInfo
{
    QString brand;     // 品牌
    QString model;     // 型号
    QString resolution;  // 分辨率
};

struct AudioInfo
{
    QString name;
};

struct SystemInfo
{
    QString os;
};

#endif
