/**
 * @brief   :硬件信息卡片组件
 * @author  :樊晓亮
 * @date    :2026.03.14
 **/

#ifndef HARDWARECARD_H
#define HARDWARECARD_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

class HardwareCard : public QFrame
{
    Q_OBJECT

public:

    explicit HardwareCard(const QString &title,
                          const QString &content,
                          QWidget *parent=nullptr);

private:

    QLabel *mTitle;
    QLabel *mContent;

};

#endif
