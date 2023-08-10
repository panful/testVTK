#include "window_qt.h"
#include "window_vtk.h"

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
#include "vtkOrientationMarkerWidget.h"
#include "vtkAxesActor.h"
#include "vtkTextProperty.h"
#include "vtkCaptionActor2D.h"
#include "vtkNamedColors.h"
#include "vtkSphereSource.h"
#include "vtkFFMPEGWriter.h"
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkUnsignedCharArray.h"

#include <QPushButton>
#include <QDebug>

WindowQt::WindowQt(QWidget* parent)
    :QVTKRenderWidget(parent)
{
    QPushButton* btn = new QPushButton("Start", this);
    connect(btn, SIGNAL(clicked()), this, SLOT(testEvent()));

    qDebug() << "=== Start ===\n";

    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    this->setRenderWindow(m_renderWindow);

    vtkNew<vtkCubeSource> cube;

    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);

    // VTK Renderer
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->AddActor(cubeActor);
    m_renderer->SetBackground(.1, .2, .3);
    m_renderer->ResetCamera();

    // Connect VTK with Qt
    this->renderWindow()->AddRenderer(m_renderer);
}

bool WindowQt::event(QEvent* evt)
{
    if (evt == nullptr)
    {
        return false;
    }

    if (m_VTKWindow)
    {
        m_VTKWindow->ProcessEvent(evt);
    }

    return QVTKOpenGLNativeWidget::event(evt);
}

void WindowQt::testEvent()
{
    this->m_VTKWindow = new WindowVTK();
    this->m_VTKWindow->resize(800, 600);
    this->m_VTKWindow->show();
}
