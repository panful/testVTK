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

#include <QDataStream>
#include <QPushButton>
#include <QWheelEvent>
#include <QEvent>
#include <QDebug>

#include <sstream>

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

    //vtkNew<vtkRenderWindowInteractor> iren;
    //iren->SetRenderWindow(m_renderWindow);

    if (auto interactor = m_renderWindow->GetInteractor())
    {
        vtkNew<vtkInteractorStyleRubberBand3D> style;
        interactor->SetInteractorStyle(style);
    }

    // Connect VTK with Qt
    this->renderWindow()->AddRenderer(m_renderer);
}

bool WindowQt::event(QEvent* evt)
{
    if (evt == nullptr)
    {
        return false;
    }

    std::map<Qt::MouseButton, long> pressEventMap{
        { Qt::LeftButton, 1 },
        { Qt::RightButton, 5 },
        { Qt::MiddleButton, 3 },
    };

    std::map<Qt::MouseButton, long> releaseEventMap{
        { Qt::LeftButton, 2 },
        { Qt::RightButton, 6 },
        { Qt::MiddleButton, 4 },
    };

    std::stringstream ss;

    if (m_VTKWindow)
    {
        switch (evt->type())
        {
        case QEvent::Type::MouseButtonPress:
        {
            qDebug() << "mouse press";

            auto me = reinterpret_cast<QMouseEvent*>(evt);
            switch (me->button())
            {
            case Qt::LeftButton:
            case Qt::RightButton:
            case Qt::MiddleButton:
            {
                long type = pressEventMap[me->button()];
                int x_ = me->position().x();
                int y_ = me->position().y();
                ss.write(reinterpret_cast<char*>(&type), 4);
                ss.write(reinterpret_cast<char*>(&x_),4);
                ss.write(reinterpret_cast<char*>(&y_),4);

                m_VTKWindow->ProcessEvent((EventType)type, x_, y_, nullptr);

                break;
            }
            default:
                break;
            }
            break;
        }
        case QEvent::Type::MouseButtonRelease:
        {
            qDebug() << "mouse release";

            auto me = reinterpret_cast<QMouseEvent*>(evt);
            switch (me->button())
            {
            case Qt::LeftButton:
            case Qt::RightButton:
            case Qt::MiddleButton:
            {
                long type = releaseEventMap[me->button()];
                int x_ = me->position().x();
                int y_ = me->position().y();
                ss.write(reinterpret_cast<char*>(&type), 4);
                ss.write(reinterpret_cast<char*>(&x_), 4);
                ss.write(reinterpret_cast<char*>(&y_), 4);

                m_VTKWindow->ProcessEvent((EventType)type, x_, y_, nullptr);

                break;
            }
            default:
                break;
            }

            break;
        }
        case QEvent::Type::Wheel:
        {
            qDebug() << "mouse wheel";

            QWheelEvent* e2 = reinterpret_cast<QWheelEvent*>(evt);

            double horizontalDelta = e2->angleDelta().x();
            double verticalDelta = e2->angleDelta().y();
            const int threshold = 120;
            auto  accumulatedDelta = verticalDelta + horizontalDelta;

            if (accumulatedDelta >= threshold && verticalDelta != 0.0)
            {
                long type = 9;
                int x_ = 0;
                int y_ = 0;
                ss.write(reinterpret_cast<char*>(&type), 4);
                ss.write(reinterpret_cast<char*>(&x_), 4);
                ss.write(reinterpret_cast<char*>(&y_), 4);

                m_VTKWindow->ProcessEvent((EventType)type, x_, y_, nullptr);

                qDebug() << "mouse wheel forward event";
            }
            else if (accumulatedDelta <= -threshold && verticalDelta != 0.0)
            {
                long type = 10;
                int x_ = 0;
                int y_ = 0;
                ss.write(reinterpret_cast<char*>(&type), 4);
                ss.write(reinterpret_cast<char*>(&x_), 4);
                ss.write(reinterpret_cast<char*>(&y_), 4);

                m_VTKWindow->ProcessEvent((EventType)type, x_, y_, nullptr);

                qDebug() << "mouse wheel backward event";
            }

            break;
        }
        case QEvent::Type::MouseMove:
        {
            auto e2 = reinterpret_cast<QMouseEvent*>(evt);

            long type = 11;
            int x_ = e2->position().x();
            int y_ = e2->position().y();
            ss.write(reinterpret_cast<char*>(&type), 4);
            ss.write(reinterpret_cast<char*>(&x_), 4);
            ss.write(reinterpret_cast<char*>(&y_), 4);

            m_VTKWindow->ProcessEvent((EventType)type, x_, y_, nullptr);

            break;
        }
        case QEvent::Type::KeyPress:
        {
            qDebug() << "key press";
            break;
        }
        case QEvent::Type::KeyRelease:
        {
            qDebug() << "key release";
            break;
        }
        default:
            break;
        }
    }



    return QVTKOpenGLNativeWidget::event(evt);
}

void WindowQt::testEvent()
{
    this->m_VTKWindow = new WindowVTK();
}
