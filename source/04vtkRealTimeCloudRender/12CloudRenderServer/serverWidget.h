#include <QVTKRenderWidget.h>
#include <vtkSmartPointer.h>

class vtkGenericOpenGLRenderWindow;
class vtkRenderer;

class QTcpServer;
class QTcpSocket;

class VTKWidget :public QVTKRenderWidget
{
    Q_OBJECT
public:
    explicit VTKWidget(QWidget* parent = nullptr);
    ~VTKWidget();
protected:
    virtual void paintGL()override;
    virtual bool event(QEvent* evt) override;

private slots:
    void OnNewConnection();
private:
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;

    QTcpServer* m_server;
    QTcpSocket* m_socket;

    bool m_bHasConnect{ false };
};