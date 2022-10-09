#include <QVTKRenderWidget.h>
#include <vtkSmartPointer.h>

#include "copyWidget.h"

class vtkGenericOpenGLRenderWindow;
class vtkRenderer;

class VTKWidget :
    public QVTKRenderWidget
{
    Q_OBJECT
public:
    explicit VTKWidget(QWidget* parent = nullptr);
    ~VTKWidget() = default;

private slots:
    void OnBtnDisplayCopyWindow();

private:
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;

    CopyWidget* m_copyWidget{ nullptr };
};
