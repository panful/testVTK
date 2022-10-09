#include "screenCapture.h"

#include <vtkNew.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkQImageToImageSource.h>
#include <vtkImageData.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleImage.h>

#include <QPixmap>

LeoScreenCapture::LeoScreenCapture(QWidget* parent) :
    QVTKRenderWidget(parent)
{
    m_window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();

    this->setRenderWindow(m_window);
    m_window->AddRenderer(m_renderer);
    m_renderer->SetBackground(1, 1, 1);
}

void LeoScreenCapture::SetWidget(QWidget* widget)
{
    m_width = widget->width();
    m_height = widget->height();

    auto pixmap = widget->grab();
    vtkNew<vtkQImageToImageSource> qimageToImageSource;
    QImage qimage = pixmap.toImage();
    qimageToImageSource->SetQImage(&qimage);
    qimageToImageSource->Update();
    m_windowImage = qimageToImageSource->GetOutput();
}

LeoScreenCapture* LeoScreenCapture::GetWidget()
{
    if (m_windowImage == nullptr || m_width == 0 || m_height == 0)
    {
        return nullptr;
    }

    int extent[6];
    m_windowImage->GetExtent(extent);
    double origin[3];
    m_windowImage->GetOrigin(origin);
    double spacing[3];
    m_windowImage->GetSpacing(spacing);

    double xc = origin[0] + 0.5 * (extent[0] + extent[1]) * spacing[0];
    double yc = origin[1] + 0.5 * (extent[2] + extent[3]) * spacing[1];
    double xd = (extent[1] - extent[0] + 1) * spacing[0]; // not used
    double yd = (extent[3] - extent[2] + 1) * spacing[1];

    vtkNew<vtkImageActor> imageActor;
    imageActor->SetInputData(m_windowImage);

    m_renderer->AddActor(imageActor);
    m_renderer->ResetCamera();

    vtkCamera* camera = m_renderer->GetActiveCamera();
    camera->ParallelProjectionOn();
    camera->SetParallelScale(0.5 * xd / ((double)m_width / (double)m_height));

    vtkNew<vtkInteractorStyleImage> style;
    this->renderWindow()->GetInteractor()->SetInteractorStyle(style);

    this->renderWindow()->Render();
    return this;
}
