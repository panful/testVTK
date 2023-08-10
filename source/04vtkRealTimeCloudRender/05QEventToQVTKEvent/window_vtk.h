#include <QVTKRenderWidget.h>
#include <vtkSmartPointer.h>

class vtkGenericOpenGLRenderWindow;
class vtkRenderer;

class WindowVTK :
    public QVTKRenderWidget
{
    Q_OBJECT
public:
    explicit WindowVTK(QWidget* parent = nullptr);
    ~WindowVTK() = default;

    bool ProcessEvent(QEvent* e);

protected:
    bool event(QEvent* evt) override;

private:
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;
};
