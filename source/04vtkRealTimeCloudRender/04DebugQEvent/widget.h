#include <QVTKRenderWidget.h>
#include <vtkSmartPointer.h>

class vtkGenericOpenGLRenderWindow;
class vtkRenderer;

class VTKWidget :
    public QVTKRenderWidget
{
    Q_OBJECT
public:
    explicit VTKWidget(QWidget* parent = nullptr);
    ~VTKWidget() = default;
protected:
    void paintGL()override;
    bool event(QEvent* evt) override;
private slots:
    void testEvent();
private:
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;
};
