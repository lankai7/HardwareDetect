#include "gpureader.h"
#include <dxgi.h>
#include <QSet>

QList<GPUInfo> GPUReader::readGPU()
{
    QList<GPUInfo> list;

        IDXGIFactory *factory = nullptr;

        if(CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)&factory) != S_OK)
            return list;

        IDXGIAdapter *adapter = nullptr;
        UINT i = 0;

        QSet<QString> existNames;   // 用于去重

        while(factory->EnumAdapters(i,&adapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC desc;
            adapter->GetDesc(&desc);

            QString name = QString::fromWCharArray(desc.Description);

            // ========================
            // 1. 过滤虚拟显卡
            // ========================
            bool isVirtual = false;

            if(desc.VendorId == 0x1414)   // Microsoft虚拟GPU
                isVirtual = true;

            if(name.contains("Microsoft",Qt::CaseInsensitive) ||
               name.contains("Virtual",Qt::CaseInsensitive) ||
               name.contains("Basic Render",Qt::CaseInsensitive) ||
               name.contains("VMware",Qt::CaseInsensitive) ||
               name.contains("Hyper-V",Qt::CaseInsensitive))
            {
                isVirtual = true;
            }

            if(isVirtual)
            {
                adapter->Release();
                i++;
                continue;
            }

            // ========================
            // 2. 去重
            // ========================
            if(existNames.contains(name))
            {
                adapter->Release();
                i++;
                continue;
            }

            existNames.insert(name);

            // ========================
            // 3. 读取显存
            // ========================
            GPUInfo info;
            info.name = name;

            double memGiB = desc.DedicatedVideoMemory / (1024.0 * 1024 * 1024);

            // 四舍五入
            int mem = qRound(memGiB);

            info.memory = QString::number(mem,'f',1) + " GB";

            list.append(info);

            adapter->Release();
            i++;
        }

        factory->Release();

        return list;
}
