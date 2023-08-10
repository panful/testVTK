#include "widget.h"

#include "vtkGenericOpenGLRenderWindow.h"
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCubeSource.h>
#include <vtkCamera.h>
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkInteractorStyleRubberBand3D.h"
#include "vtkNamedColors.h"
#include "vtkSphereSource.h"
#include "vtkObjectFactory.h"

#include <QPushButton>

class CustomStyle :
    public vtkInteractorStyleRubberBand3D
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleRubberBand3D);

protected:
    void OnChar() override
    {
        if (!this->Interactor || !this->CurrentRenderer)
        {
            return;
        }

        switch (this->Interactor->GetKeyCode())
        {
        case 'x':
        case 'X':
        {
            std::cout << "press x\n";
            break;
        }
        case 'Z':
        case 'z':
        {
            std::cout << "press z\n";
            break;
        }
        default:
            break;
        }

        return Superclass::OnChar();
    }

    void OnMouseMove() override
    {
        std::cout << "mouse move\n";
        Superclass::OnMouseMove();
    }

    void OnMouseWheelForward() override
    {
        std::cout << "mouse wheel forward\n";
        Superclass::OnMouseWheelForward();
    }
    void OnMouseWheelBackward() override
    {
        std::cout << "mouse wheel backward\n";
        Superclass::OnMouseWheelBackward();
    }
};

vtkStandardNewMacro(CustomStyle);


VTKWidget::VTKWidget(QWidget* parent)
    :QVTKRenderWidget(parent)
{
    QPushButton* btn = new QPushButton("test", this);
    connect(btn, SIGNAL(clicked()), this, SLOT(testEvent()));

    std::cout << "=== Start ===\n";

    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    this->setRenderWindow(m_renderWindow);

    // Sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(0, 1, 0);

    // VTK Renderer
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->AddActor(sphereActor);
    m_renderer->SetBackground(.1, .2, .3);
    m_renderer->ResetCamera();

    // Interactor style
    vtkNew<CustomStyle> style;
    this->renderWindow()->GetInteractor()->SetInteractorStyle(style);

    // Connect VTK with Qt
    this->renderWindow()->AddRenderer(m_renderer);
}

void VTKWidget::paintGL()
{
    //std::cout << "===Paint GL\t" << clock() << '\n';

    return QVTKOpenGLNativeWidget::paintGL();
}

bool VTKWidget::event(QEvent* evt)
{
    if (evt == nullptr)
    {
        return false;
    }

    //std::cout << "***Event\t" << clock() << '\n';
    return QVTKOpenGLNativeWidget::event(evt);
}

void VTKWidget::testEvent()
{
    QEvent* e = nullptr;
    this->event(e);
}
