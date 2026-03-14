#include "wmimanager.h"

#include <QChar>
#include <QDebug>
#include <windows.h>
#include <Wbemidl.h>

WMIManager::WMIManager()
{
    pLocator = nullptr;
    pServices = nullptr;
}

WMIManager::~WMIManager()
{
    if(pServices)
        pServices->Release();

    if(pLocator)
        pLocator->Release();

    CoUninitialize();
}

bool WMIManager::init()
{
    HRESULT hr;

    // 初始化COM
    hr = CoInitializeEx(0, COINIT_MULTITHREADED);

    if(FAILED(hr) && hr != RPC_E_CHANGED_MODE)
        return false;

    // 初始化安全
    hr = CoInitializeSecurity(
                NULL,
                -1,
                NULL,
                NULL,
                RPC_C_AUTHN_LEVEL_DEFAULT,
                RPC_C_IMP_LEVEL_IMPERSONATE,
                NULL,
                EOAC_NONE,
                NULL);

    if(FAILED(hr) && hr != RPC_E_TOO_LATE)
        return false;

    // 创建WMI locator
    hr = CoCreateInstance(
                CLSID_WbemLocator,
                0,
                CLSCTX_INPROC_SERVER,
                IID_IWbemLocator,
                (LPVOID *)&pLocator);

    if(FAILED(hr))
        return false;

    BSTR namespaceStr = SysAllocString(L"ROOT\\CIMV2");

    if(!namespaceStr)
        return false;

    // 连接WMI
    hr = pLocator->ConnectServer(
                namespaceStr,
                NULL,
                NULL,
                0,
                NULL,
                0,
                0,
                &pServices);

    SysFreeString(namespaceStr);

    if(FAILED(hr))
        return false;

    // 设置安全代理
    hr = CoSetProxyBlanket(
                pServices,
                RPC_C_AUTHN_WINNT,
                RPC_C_AUTHZ_NONE,
                NULL,
                RPC_C_AUTHN_LEVEL_CALL,
                RPC_C_IMP_LEVEL_IMPERSONATE,
                NULL,
                EOAC_NONE);

    if(FAILED(hr))
        return false;

    return true;
}

QString WMIManager::queryFirst(const QString &wmiClass,const QString &property)
{
    QString result;

    QString sql = QString("SELECT %1 FROM %2")
            .arg(property)
            .arg(wmiClass);

    IEnumWbemClassObject *pEnumerator = NULL;

    BSTR lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString((LPCWSTR)sql.utf16());

    HRESULT hr = pServices->ExecQuery(
                lang,
                query,
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL,
                &pEnumerator);

    SysFreeString(lang);
    SysFreeString(query);

    if(FAILED(hr))
        return result;

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;

    if(pEnumerator)
    {
        hr = pEnumerator->Next(WBEM_INFINITE,1,&pclsObj,&uReturn);

        if(uReturn)
        {
            VARIANT vtProp;
            VariantInit(&vtProp);

            hr = pclsObj->Get((LPCWSTR)property.utf16(),0,&vtProp,0,0);

            if(SUCCEEDED(hr) && vtProp.vt == VT_BSTR)
                result = QString::fromWCharArray(vtProp.bstrVal);

            VariantClear(&vtProp);

            pclsObj->Release();
        }

        pEnumerator->Release();
    }

    return result;
}

QStringList WMIManager::queryAll(const QString &wmiClass,const QString &property)
{
    QStringList list;

    QString sql = QString("SELECT %1 FROM %2")
            .arg(property)
            .arg(wmiClass);

    IEnumWbemClassObject *pEnumerator = NULL;

    BSTR lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString((LPCWSTR)sql.utf16());

    HRESULT hr = pServices->ExecQuery(
                lang,
                query,
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL,
                &pEnumerator);

    SysFreeString(lang);
    SysFreeString(query);

    if(FAILED(hr))
        return list;

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;

    while(pEnumerator)
    {
        hr = pEnumerator->Next(WBEM_INFINITE,1,&pclsObj,&uReturn);

        if(!uReturn)
            break;

        VARIANT vtProp;
        VariantInit(&vtProp);

        hr = pclsObj->Get((LPCWSTR)property.utf16(),0,&vtProp,0,0);

        if(SUCCEEDED(hr) && vtProp.vt == VT_BSTR)
            list.append(QString::fromWCharArray(vtProp.bstrVal));

        VariantClear(&vtProp);

        pclsObj->Release();
    }

    pEnumerator->Release();

    return list;
}
/**
 * @brief   :WMI查询函数（支持EDID数组解析）
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/

QStringList WMIManager::queryAll(const QString &nameSpace,
                                 const QString &wmiClass,
                                 const QString &property)
{
    QStringList list;

    IWbemServices *pSvc = nullptr;

    BSTR ns = SysAllocString((LPCWSTR)nameSpace.utf16());

    HRESULT hr = pLocator->ConnectServer(
                ns,
                NULL,
                NULL,
                0,
                0,
                0,
                0,
                &pSvc);

    SysFreeString(ns);

    if(FAILED(hr))
        return list;

    QString sql;

    if(wmiClass == "WmiMonitorID")
    {
        sql = QString("SELECT %1 FROM %2 WHERE Active = TRUE")
                .arg(property)
                .arg(wmiClass);
    }
    else
    {
        sql = QString("SELECT %1 FROM %2")
                .arg(property)
                .arg(wmiClass);
    }

    IEnumWbemClassObject *pEnumerator = NULL;

    BSTR lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString((LPCWSTR)sql.utf16());

    hr = pSvc->ExecQuery(
                lang,
                query,
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL,
                &pEnumerator);

    SysFreeString(lang);
    SysFreeString(query);

    if(FAILED(hr))
    {
        pSvc->Release();
        return list;
    }

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;

    while(pEnumerator)
    {
        hr = pEnumerator->Next(WBEM_INFINITE,1,&pclsObj,&uReturn);

        if(!uReturn)
            break;

        VARIANT vtProp;
        VariantInit(&vtProp);

        hr = pclsObj->Get((LPCWSTR)property.utf16(),0,&vtProp,0,0);

        if(SUCCEEDED(hr))
        {
            // 普通字符串
            if(vtProp.vt == VT_BSTR)
            {
                list.append(QString::fromWCharArray(vtProp.bstrVal));
            }
            // EDID数组（显示器信息）
            else if(vtProp.vt & VT_ARRAY)
            {
                SAFEARRAY *sa = vtProp.parray;

                LONG lBound = 0;
                LONG uBound = 0;

                if(SafeArrayGetLBound(sa,1,&lBound) == S_OK &&
                   SafeArrayGetUBound(sa,1,&uBound) == S_OK)
                {
                    // 防止异常长度
                    if(uBound - lBound > 256)
                        uBound = lBound + 256;

                    QString str;

                    for(LONG i=lBound;i<=uBound;i++)
                    {
                        LONG val = 0;

                        if(SafeArrayGetElement(sa,&i,&val) != S_OK)
                            continue;

                        if(val == 0)
                            break;

                        str.append(QChar(static_cast<ushort>(val)));
                    }

                    if(!str.isEmpty())
                        list.append(str);
                }
            }
        }

        VariantClear(&vtProp);

        pclsObj->Release();
    }

    if(pEnumerator)
        pEnumerator->Release();

    if(pSvc)
        pSvc->Release();

    return list;
}
