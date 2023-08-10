#include <QWidget>
#include <QLabel>
#include <QPixmap>


class CopyWidget :
    public QWidget
{
public:
    CopyWidget();
    void SetPixmap(const QPixmap& pixmap);

private:
    QLabel* m_label;
};
