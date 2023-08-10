#include <QWidget>

class QTcpServer;
class QTcpSocket;

class VTKWidget :public QWidget
{
    Q_OBJECT
public:
    explicit VTKWidget(QWidget* parent = nullptr);
    ~VTKWidget();

private slots:
    void OnSend();
    void OnWrite();
private:
    QTcpSocket* m_socket{ nullptr };
};