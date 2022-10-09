#include <QVTKRenderWidget.h>
#include <vtkSmartPointer.h>

class vtkGenericOpenGLRenderWindow;
class vtkRenderer;
class WindowVTK;

class WindowQt :
    public QVTKRenderWidget
{
    Q_OBJECT
public:
    explicit WindowQt(QWidget* parent = nullptr);
    ~WindowQt() = default;

protected:
    bool event(QEvent* evt) override;

private slots:
    void testEvent();

private:
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;
    WindowVTK* m_VTKWindow{ nullptr };
};
