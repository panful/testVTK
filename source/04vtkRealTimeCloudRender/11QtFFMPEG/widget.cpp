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

#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPixmap>
#include <QImage>
#include <QLabel>

#include <thread>
#include <string>
#include <chrono>

VTKWidget::VTKWidget(QWidget* parent)
    :QVTKRenderWidget(parent),
    m_imageWidget(new QWidget()),
    m_label(new QLabel(m_imageWidget))
{
    m_imageWidget->setMinimumSize(800, 600);
    m_imageWidget->setWindowTitle("display");
    m_imageWidget->show();

    QPushButton* btn = new QPushButton("test", this);
    connect(btn, SIGNAL(clicked()), this, SLOT(testEvent()));

    qDebug() << "=== Start ===\n";

    vtkNew<vtkNamedColors> colors;
    //m_image = vtkSmartPointer<vtkWindowToImageFilter>::New();
    //m_ffmpeg = vtkSmartPointer<vtkFFMPEGWriter>::New();
    //m_ffmpeg->SetInputConnection(m_image->GetOutputPort());
    //m_ffmpeg->SetFileName("out\\Qt_Window_FFMPEG.avi");
    //m_ffmpeg->Start();

    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    this->setRenderWindow(m_renderWindow);
    //m_image->SetInput(m_renderWindow);

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
    m_renderer->GradientBackgroundOn();
    //m_renderer->SetBackground(.1, .2, .3);
    m_renderer->SetBackground(1., 0., 0.);
    m_renderer->SetBackground2(0., 1., 0.);

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
}

VTKWidget::~VTKWidget()
{
    qDebug() << "===End===\n";
    //m_ffmpeg->End();
}

void VTKWidget::GenImage(int w, int h, unsigned char* pixels, QImage* image)
{
    //QImage image (w, h, QImage::Format::Format_RGB32);
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

void VTKWidget::paintGL()
{
    qDebug() << clock() / 1000.0 << "=== Render ===\n";
    std::string fileName = "outImage\\" + std::to_string(clock()) + ".jpg";
    QVTKOpenGLNativeWidget::paintGL();


    auto w = this->width();
    auto h = this->height();
    auto pixels = this->renderWindow()->GetRGBACharPixelData(0, 0, w - 1, h - 1, 1);


    QImage* image = new QImage(w, h, QImage::Format::Format_RGB32);
    std::thread t(&VTKWidget::GenImage, this, w, h, pixels, image);
    t.join();
    delete[] pixels;

    m_label->setPixmap(QPixmap::fromImage(*image));






    /*
    auto w = this->width();
    auto h = this->height();
    auto size = this->renderWindow()->GetSize();

    //auto pixel0 = this->renderWindow()->GetRGBACharPixelData(0, 0, size[0] - 1, size[1] - 1, 0); // 第五个参数指示应该使用前缓冲区还是后缓冲区
    auto pixel1 = this->renderWindow()->GetRGBACharPixelData(0, 0, size[0] - 1, size[1] - 1, 1); // 第五个参数指示应该使用前缓冲区还是后缓冲区
    auto pixel2 = this->renderWindow()->GetRGBAPixelData(0, 0, size[0] - 1, size[1] - 1, 1);  // float*
    auto pixelSize = size[0] * size[1] * 4;  // uchar
    auto pix0 = static_cast<int>(pixel1[0]); // 第一个像素的红色分量
    auto pix1 = static_cast<int>(pixel1[pixelSize - 1 - 3]); // 最后一个像素的红色分量
    //delete[] pixel0;
    delete[] pixel1;
    //auto len1 = strlen((char*)pixel1);
    //auto len0 = strlen((char*)pixel0);
    //auto size1 = sizeof(pixel1);
    //auto size0 = sizeof(pixel0);

    //vtkNew<vtkUnsignedCharArray> ucArr;
    //ucArr->Initialize();
    //ucArr->SetNumberOfComponents(4);
    //ucArr->SetNumberOfTuples(size[0] * size[1]);
    //this->renderWindow()->GetRGBACharPixelData(0, 0, size[0] - 1, size[1] - 1, 1, ucArr);
    //ucArr->GetValue(1);

    // vtkWindowToImageFilter不在paintGL中new时 不会一直调用paintGL，但是这样图片内容就不会更新，一直都是初始的图片
    // vtkWindowToImageFilter在paintGL中new时，不论有没有事件响应，即刷新渲染窗口，都会一直调用paintGL

    //m_image = vtkSmartPointer<vtkWindowToImageFilter>::New();
    //m_image->SetInput(this->renderWindow());
    //m_image->Update();

    //vtkNew<vtkJPEGWriter> jpeg;
    //jpeg->SetFileName(fileName.c_str());
    //jpeg->SetInputConnection(m_image->GetOutputPort());
    //jpeg->Write();

    //m_ffmpeg->SetInputConnection(m_image->GetOutputPort());
    //m_ffmpeg->Write();
    */
}

bool VTKWidget::event(QEvent* evt)
{
    std::cout << clock() << "\tEvent\n";
    if (evt == nullptr)
        return false;

    QVTKOpenGLNativeWidget::event(evt);

    return false;
}

void VTKWidget::testEvent()
{
    QEvent* e = nullptr;
    this->event(e);
}