#include "serverWidget.h"

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

#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <sstream>

#include <QTcpServer>
#include <QTcpSocket>
#include <QBuffer>

VTKWidget::VTKWidget(QWidget* parent)
    :QVTKRenderWidget(parent)
{
    std::cout << "=== Start ===\n";

    vtkNew<vtkNamedColors> colors;
    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    this->setRenderWindow(m_renderWindow);

    // Sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(colors->GetColor4d("Tomato").GetData());

    // VTK Renderer
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->AddActor(sphereActor);
    m_renderer->SetBackground(.1, .2, .3);

    m_renderer->ResetCamera();

    // Connect VTK with Qt
    this->renderWindow()->AddRenderer(m_renderer);

    std::array<double, 3> scale{ {1.0, 1.0, 1.0} };

    vtkAxesActor* axes = vtkAxesActor::New();
    axes->SetScale(0, 1, 1);
    axes->SetShaftTypeToCylinder();

    axes->SetCylinderRadius(0.5 * axes->GetCylinderRadius());  //轴的粗细
    axes->SetConeRadius(1.025 * axes->GetConeRadius());        //圆锥的大小
    axes->SetSphereRadius(1.5 * axes->GetSphereRadius());

    vtkOrientationMarkerWidget* omw = vtkOrientationMarkerWidget::New();
    omw->SetOrientationMarker(axes);
    omw->SetViewport(0., 0., 0.2, 0.2);
    omw->SetOutlineColor(1, 0, 0);
    omw->SetInteractor(this->interactor());
    omw->EnabledOn();
    omw->InteractiveOn();

    // socket
    m_server = new QTcpServer(this);
    m_server->listen(QHostAddress::LocalHost, 7788);

    connect(m_server, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));
}

void VTKWidget::OnNewConnection()
{
    m_socket = m_server->nextPendingConnection();
    //connect(m_socket, SIGNAL(readyRead()), this, SLOT(OnRead())); // 当客户端有数据发送过来就会响应该信号

    m_bHasConnect = true;

    std::cout << "   *** client connected ***\n";

    //char buf[] = "abcd";
    //m_socket->write(buf, sizeof(buf));
}

VTKWidget::~VTKWidget()
{
    std::cout << "===End===\n";
}

void VTKWidget::paintGL()
{
    //std::cout << "=== Render ===\t" << clock() << std::endl;
    //std::string fileName = "outImage\\" + std::to_string(clock()) + ".jpg";
    QVTKOpenGLNativeWidget::paintGL();


    auto w = this->width();
    auto h = this->height();
    auto pixels = this->renderWindow()->GetRGBACharPixelData(0, 0, w - 1, h - 1, 1);

    auto pixesSize = w * h * 4;

    std::stringstream ss;
    ss.write((char*)pixels, pixesSize);

    if (m_bHasConnect)
    {
        while (pixesSize > 0)
        {
            if (pixesSize > 1024)
            {
                auto buffer = new char[1025]();
                ss.read(buffer, 1024);
                m_socket->write(buffer, 1024);

                std::cout << 1920000- pixesSize<<'\t'<<(int)((unsigned char)buffer[0]) << '\t' << (int)((unsigned char)buffer[1]) << '\t' << (int)((unsigned char)buffer[2]) << '\t' << '\n';

                pixesSize -= 1024;
            }
            else
            {
                auto buffer = new char[pixesSize + 1]();
                ss.read(buffer, pixesSize);
                m_socket->write(buffer, pixesSize);
                pixesSize = 0;
            }


            //auto r = static_cast<int>(pixels[1920000 - pixesSize]);
            //auto g = static_cast<int>(pixels[1920000 - pixesSize + 1]);
            //auto b = static_cast<int>(pixels[1920000 - pixesSize + 2]);

            //std::cout << 1920000 - pixesSize<<'\t'<< r << '\t' << g << '\t' << b << '\n';
                
        }
    }



}

bool VTKWidget::event(QEvent* evt)
{
    //std::cout << "Event\t" << clock() << std::endl;

    //if (evt == nullptr)
    //    return false;

    QVTKOpenGLNativeWidget::event(evt);

    return false;
}


