#include "widget.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCubeSource.h>
#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>

#include <QDebug>
#include <QPushButton>
#include <QPixmap>
#include <QImage>

#include <thread>

namespace
{
    void GenImage(int w, int h, unsigned char* pixels, QImage* image)
    {
        for (size_t i = 0; i < h; i++)
        {
            for (size_t j = 0; j < w; j++)
            {
                auto r = static_cast<int>(pixels[(i * w + j) * 4 + 0]);
                auto g = static_cast<int>(pixels[(i * w + j) * 4 + 1]);
                auto b = static_cast<int>(pixels[(i * w + j) * 4 + 2]);

                // vtk左下角开始，qt左上角开始
                image->setPixelColor(j, h - i - 1, QColor(r, g, b));
            }
        }
    }
}

class RenderEventCB :
    public vtkCommand
{
public:
    static RenderEventCB* New()
    {
        return new RenderEventCB();
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        if (auto interactor = dynamic_cast<vtkRenderWindowInteractor*>(caller))
        {
            auto window = interactor->GetRenderWindow();
            auto winSize = window->GetSize();

            const auto pixelsSize = winSize[0] * winSize[1] * 4;
            auto pixels = window->GetRGBACharPixelData(0, 0, winSize[0] - 1, winSize[1] - 1, 1);

            std::thread t([winSize, pixels, this]() 
                {
                    static int index = 0;
                    QImage* image = new QImage(winSize[0], winSize[1], QImage::Format::Format_RGB32);
                    GenImage(winSize[0], winSize[1], pixels, image);
                    delete[] pixels;
                    image->save("Image_" + QString::number(index++) + ".jpg");
                    if (m_widget)
                    {
                        m_widget->SetPixmap(QPixmap::fromImage(*image));
                    }
                });

            t.detach();
        }
    }

    void SetCopyWidget(CopyWidget* widget)
    {
        m_widget = widget;
    }

private:
    CopyWidget* m_widget{ nullptr };
};

VTKWidget::VTKWidget(QWidget* parent)
    :QVTKRenderWidget(parent)
{
    QPushButton* btn = new QPushButton("ShowCopyWindow", this);
    connect(btn, SIGNAL(clicked()), this, SLOT(OnBtnDisplayCopyWindow()));

    m_copyWidget = new CopyWidget();
    m_copyWidget->setMinimumSize(800, 600);
    m_copyWidget->show();

    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    this->setRenderWindow(m_renderWindow);

    // Cube
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);

    // VTK Renderer
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->AddActor(cubeActor);
    m_renderer->SetBackground(.1, .2, .3);
    m_renderer->ResetCamera();

    vtkNew<RenderEventCB> renderEventCB;
    renderEventCB->SetCopyWidget(m_copyWidget);
    this->renderWindow()->GetInteractor()->AddObserver(vtkCommand::RenderEvent, renderEventCB);

    // Connect VTK with Qt
    this->renderWindow()->AddRenderer(m_renderer);
}

void VTKWidget::OnBtnDisplayCopyWindow()
{
    //if (!m_copyWidget)
    //{
    //    m_copyWidget = new CopyWidget();
    //    m_copyWidget->setMinimumSize(800, 600);
    //    m_copyWidget->show();
    //}
}
