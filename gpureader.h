/**
 * @brief   :GPU读取类
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/

#ifndef GPUREADER_H
#define GPUREADER_H

#include "hardwareinfo.h"

class GPUReader
{
public:

    static QList<GPUInfo> readGPU();
};

#endif
