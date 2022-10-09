#include "copyWidget.h"

CopyWidget::CopyWidget() :
    m_label(new QLabel(this))
{
    QImage image(800, 600, QImage::Format::Format_RGB32);
    m_label->setPixmap(QPixmap::fromImage(image));
}

void CopyWidget::SetPixmap(const QPixmap& pixmap)
{
    if (!pixmap.isNull())
    {
        m_label->setPixmap(pixmap);
    }
}
