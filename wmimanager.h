/**
 * @brief   :WMI查询管理类
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/

#ifndef WMIMANAGER_H
#define WMIMANAGER_H

#include <QString>
#include <QStringList>

#include <windows.h>
#include <Wbemidl.h>

class WMIManager
{
public:

    WMIManager();
    ~WMIManager();

    bool init();

    QString queryFirst(const QString &wmiClass,const QString &property);

    QStringList queryAll(const QString &wmiClass,const QString &property);
    QStringList queryAll(const QString &nameSpace,
                         const QString &wmiClass,
                         const QString &property);

private:

    IWbemLocator *pLocator;
    IWbemServices *pServices;
};

#endif
