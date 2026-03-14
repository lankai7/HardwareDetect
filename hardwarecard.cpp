#include "hardwarecard.h"

HardwareCard::HardwareCard(const QString &title,
                           const QString &content,
                           QWidget *parent)
    :QFrame(parent)
{
    setObjectName("card");

    QVBoxLayout *layout = new QVBoxLayout(this);

    mTitle = new QLabel(title);
    mContent = new QLabel(content);

    QFont font;
    font.setBold(true);
    font.setPointSize(11);

    mTitle->setFont(font);

    mContent->setWordWrap(true);
    mContent->setTextInteractionFlags(Qt::TextSelectableByMouse);

    layout->addWidget(mTitle);
    layout->addWidget(mContent);

    layout->setSpacing(10);
    layout->setContentsMargins(12,12,12,12);
    mContent->setContentsMargins(12,0,0,0);
    mContent->setObjectName("content");
    mContent->setWordWrap(true);

    setStyleSheet(
                "#card{"
                "background:#ffffff;"
                "border:1px solid #e6e9ef;"
                "border-radius:8px;"
                "padding:10px;"
                "}"
                "QLabel{"
                "color:#2c3e50;"
                "font-size:14px;"
                "}"
                "#card:hover{"
                " border:1px solid #409eff;"
                "}"
                "#content{"
                "line-height:50px;"
                "}"
    );
}
