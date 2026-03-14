/**
 * @brief   :硬件信息显示主窗口
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include "hardwaremanager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent=nullptr);

private:

    void initUI();
    void loadHardware();
    void buildCards();

private:

    QWidget *mContainer;
    QGridLayout *mGrid;

    HardwareManager mHardware;

};

#endif
