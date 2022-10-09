
#include "SideBySideRenderWindowsQt.h"
#include "ui_SideBySideRenderWindowsQt.h"

#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkVersion.h>

// https://kitware.github.io/vtk-examples/site/Cxx/Qt/SideBySideRenderWindowsQt/

// Constructor
SideBySideRenderWindowsQt::SideBySideRenderWindowsQt(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::SideBySideRenderWindowsQt)
{
    this->ui->setupUi(this);

    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindowLeft;
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindowRight;
    //renderWindowLeft->SetSharedRenderWindow(renderWindowRight);

    this->ui->qvtkWidgetLeft->setRenderWindow(renderWindowLeft);
    this->ui->qvtkWidgetRight->setRenderWindow(renderWindowRight);

    // Sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetPhiResolution(30);
    sphereSource->SetThetaResolution(30);
    sphereSource->Update();
    vtkNew<vtkElevationFilter> sphereElev;
    sphereElev->SetInputConnection(sphereSource->GetOutputPort());
    sphereElev->SetLowPoint(0, -1.0, 0);
    sphereElev->SetHighPoint(0, 1.0, 0);
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereElev->GetOutputPort());
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);

    // Cube
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();
    vtkNew<vtkElevationFilter> cubeElev;
    cubeElev->SetInputConnection(cubeSource->GetOutputPort());
    cubeElev->SetLowPoint(0, -1.0, 0);
    cubeElev->SetHighPoint(0, 1.0, 0);
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cubeElev->GetOutputPort());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);

    // VTK Renderer
    vtkNew<vtkRenderer> leftRenderer;
    leftRenderer->AddActor(cubeActor);
    leftRenderer->SetBackground(colors->GetColor3d("LightSteelBlue").GetData());

    vtkNew<vtkRenderer> rightRenderer;

    // Add Actor to renderer
    rightRenderer->AddActor(sphereActor);
    rightRenderer->GetActiveCamera()->SetPosition(1.0, 0.8, 1.0);
    rightRenderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    rightRenderer->SetBackground(colors->GetColor3d("LightSteelBlue").GetData());
    rightRenderer->ResetCamera();
    rightRenderer->GetActiveCamera()->Zoom(0.8);

    // VTK/Qt wedded
    this->ui->qvtkWidgetLeft->renderWindow()->AddRenderer(leftRenderer);
    this->ui->qvtkWidgetRight->renderWindow()->AddRenderer(rightRenderer);


    // Set up action signals and slots
    connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}

void SideBySideRenderWindowsQt::slotExit()
{
    qApp->exit();
}
