#include "ShareCameraQt.h"
#include "ui_ShareCameraQt.h"

#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkQtTableView.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

// https://kitware.github.io/vtk-examples/site/Cxx/Qt/ShareCameraQt/

// Constructor
ShareCameraQt::ShareCameraQt(QWidget* parent) : QMainWindow(parent), ui(new Ui::ShareCameraQt)
{
    this->ui->setupUi(this);

    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindowLeft;
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindowRight;

    this->ui->qvtkWidgetLeft->setRenderWindow(renderWindowLeft);
    this->ui->qvtkWidgetRight->setRenderWindow(renderWindowRight);

    // Sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(colors->GetColor4d("Tomato").GetData());

    // Cube
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(colors->GetColor4d("MediumSeaGreen").GetData());

    // VTK Renderer
    vtkNew<vtkRenderer> leftRenderer;
    leftRenderer->AddActor(sphereActor);
    leftRenderer->SetBackground(colors->GetColor3d("LightSteelBlue").GetData());

    vtkNew<vtkRenderer> rightRenderer;

    // Add Actor to renderer
    rightRenderer->AddActor(cubeActor);
    rightRenderer->SetBackground(colors->GetColor3d("LightSteelBlue").GetData());

    // VTK/Qt wedded
    this->ui->qvtkWidgetLeft->renderWindow()->AddRenderer(leftRenderer);
    this->ui->qvtkWidgetRight->renderWindow()->AddRenderer(rightRenderer);

    rightRenderer->ResetCamera();
    leftRenderer->ResetCamera();

    // Here we share the camera
    rightRenderer->SetActiveCamera(leftRenderer->GetActiveCamera());

    // Position the cube using the left renderer active camera
    leftRenderer->GetActiveCamera()->SetPosition(1.0, 0.8, 1.0);
    leftRenderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    leftRenderer->ResetCamera();
    leftRenderer->GetActiveCamera()->Zoom(0.8);

    // Set up action signals and slots
    connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));

    this->ui->qvtkWidgetLeft->renderWindow()->AddObserver(vtkCommand::ModifiedEvent, this, &ShareCameraQt::ModifiedHandler);
}

void ShareCameraQt::ModifiedHandler()
{
    this->ui->qvtkWidgetRight->renderWindow()->Render();
}

void ShareCameraQt::slotExit()
{
    qApp->exit();
}
