#include "EventQtSlotConnect.h"
#include "ui_EventQtSlotConnect.h"

#include <vtkEventQtSlotConnect.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlatonicSolidSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVersion.h>

// https://kitware.github.io/vtk-examples/site/Cxx/Qt/EventQtSlotConnect/

EventQtSlotConnect::EventQtSlotConnect(QWidget* parent) : QMainWindow(parent), ui(new Ui::EventQtSlotConnect)
{
    this->ui->setupUi(this);

    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    this->ui->qvtkWidget->setRenderWindow(renderWindow);

    vtkNew<vtkEventQtSlotConnect> slotConnector;
    this->Connections = slotConnector;

    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(20);
    lut->SetTableRange(0.0, 19.0);
    lut->SetHueRange(0.0, 0.67);
    lut->Build();

    vtkNew<vtkPlatonicSolidSource> source;
    source->SetSolidTypeToOctahedron();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(source->GetOutputPort());
    mapper->SetLookupTable(lut);
    mapper->SetScalarRange(0, 19);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

    this->ui->qvtkWidget->renderWindow()->AddRenderer(renderer);

    // 连接信号和槽函数
    this->Connections->Connect(this->ui->qvtkWidget->renderWindow()->GetInteractor(), vtkCommand::LeftButtonPressEvent, this,
        SLOT(slot_clicked(vtkObject*, unsigned long, void*, void*)));
};

EventQtSlotConnect::~EventQtSlotConnect()
{
    delete this->ui;
}

// 槽函数
void EventQtSlotConnect::slot_clicked(vtkObject*, unsigned long, void*, void*)
{
    static int count { 0 };
    std::cout << "Clicked " << count++ << std::endl;
}
