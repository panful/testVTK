/**
 * 101. 将窗口保存为图片（位图）
 * 102. 矢量图保存 vtkSVGExporter
 *
 * 201. vtkImageViewer2 显示2D图片
 * 202. vtkImageSlice 在3D场景中显示图像
 *
 * 301. vtkImageClip 裁剪图片
 * 302. vtkImageBlend 合成图片，利用多个图片的z值，将多张图片放在一起，vtkImageCanvasSource2D 画布，可以在上面绘制任意形状
 * 303. vtkImageBlend vtkImageCanvasSource2D 截图状态：选中部分正常显示，未选中部分覆盖一层阴影
 *
 * 401. 图片铺满整个窗口，图片的宽高比不变
 *
 * 501. vtkImageLuminance 将彩色图像转换为灰度图
 * 502. vtkImageMagnitude 将彩色图像转换为灰度图
 *
 * 601. 2D图像3维化，高程图
 *
 */

#define TEST601

#ifdef TEST101

#include <vtkActor.h>
#include <vtkBMPWriter.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkResizingWindowToImageFilter.h>
#include <vtkTIFFWriter.h>
#include <vtkWindowToImageFilter.h>

namespace {
class Style : public vtkInteractorStyleTrackballCamera
{
public:
    static Style* New();
    vtkTypeMacro(Style, vtkInteractorStyleTrackballCamera);

    virtual void OnRightButtonUp() override
    {
        Superclass::OnRightButtonUp();

        if (this->Interactor)
        {
            auto renWin = this->Interactor->GetRenderWindow();

            //----------------------------------------------------
            // 将窗口像素转换为图像，如果设置了Scale，Update()时窗口会被缩放
            vtkNew<vtkWindowToImageFilter> windowToImage;
            windowToImage->SetInput(renWin);
            windowToImage->SetScale(3, 3); // 当前窗口缩放之后就是图片的尺寸
            windowToImage->Update();

            //----------------------------------------------------
            // 设置的Size和窗口的大小不一致时，并不会重绘，只是将图片缩放，所以图片放大时会失真
            vtkNew<vtkResizingWindowToImageFilter> img;
            img->SetInput(renWin);
            img->SetSize(3000, 2500); // 图片的尺寸
            img->Update();

            //----------------------------------------------------
            // jpg 如果文件名没有后缀.jpg，不会补全.jpg，但是保存的图片名加上.jpg以后也可以显示
            vtkNew<vtkJPEGWriter> jpg;
            jpg->SetInputData(img->GetOutput());
            jpg->SetFileName("test_jpg.jpg");
            jpg->Write();

            //----------------------------------------------------
            // tiff 中文路径会保存失败 vtk-9.1 底层使用CreateFileA创建文件并不是CreateFileW
            vtkNew<vtkTIFFWriter> tiff;
            tiff->SetInputData(img->GetOutput());
            tiff->SetFileName("test_tiff.tiff");
            tiff->Write();

            //----------------------------------------------------
            // png
            vtkNew<vtkPNGWriter> png;
            png->SetInputData(windowToImage->GetOutput());
            png->SetFileName("test_png.png");
            png->Write();

            //----------------------------------------------------
            // bmp
            vtkNew<vtkBMPWriter> bmp;
            bmp->SetInputData(windowToImage->GetOutput());
            bmp->SetFileName("test_bmp.bmp");
            bmp->Write();
        }
    }
};

vtkStandardNewMacro(Style);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cube;

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(0.2, 0.3, 0.4);
    renderer->AddActor(cubeActor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<Style> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TSET101

#ifdef TEST102

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkChartXYZ.h>
#include <vtkContext2D.h>
#include <vtkContext3D.h>
#include <vtkContextActor.h>
#include <vtkContextItem.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkCubeSource.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlotLine3D.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererSource.h>
#include <vtkSVGExporter.h>
#include <vtkTable.h>

//----------------------------------------------------------------------------
int main(int, char*[])
{
    vtkNew<vtkCubeSource> cube;

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    // actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    // renderer->Render();

    // Set up a 3D scene and add an XYZ chart to it.
    vtkNew<vtkContextView> view;
    view->GetRenderWindow()->SetSize(640, 480);
    view->GetRenderWindow()->SetWindowName("PlotLine3D");
    view->GetRenderer()->SetBackground(1.0, 1.0, 0.5);
    view->GetRenderer()->SetBackground2(1.0, 0.5, 1.0);
    view->GetRenderer()->SetBackgroundAlpha(0.5);
    view->GetRenderer()->GradientBackgroundOn();

    // view->GetRenderer()->AddActor(cubeActor);
    // view->GetRenderWindow()->SetMultiSamples(10);

    // vtkNew<vtkCamera> camera;
    // camera->SetPosition(1, 1, 1);//设置相机位置
    // camera->SetFocalPoint(0, 0, 0);//设置相机焦点

    // view->GetRenderer()->SetActiveCamera(camera);
    // view->GetRenderer()->ResetCamera();

    // vtkNew<vtkContext2D> context2D;
    // vtkNew<vtkContextItem> contextItem;
    // vtkNew<vtkContextActor> contextActor;

    vtkNew<vtkRendererSource> renderSource;
    renderSource->SetInput(renderer);
    auto imageData1 = renderSource->GetOutput();

    // view->GetScene()->AddItem(contextItem);
    view->GetContext()->DrawImage(0, 0, imageData1);

    view->GetRenderWindow()->Render();
    view->GetRenderer()->Render();
    vtkNew<vtkSVGExporter> svg;
    svg->SetRenderWindow(view->GetRenderWindow());
    // svg->SetActiveRenderer(view->GetRenderer());
    svg->SetFileName("contextViewSvg.svg");
    svg->Write();

    // view->GetInteractor()->Initialize();
    // view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

#endif // TEST102

#ifdef TEST201

#include <vtkCallbackCommand.h>
#include <vtkImageViewer2.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

int main(int argc, char* argv[])
{
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName("../resources/test1.jpg");
    jPEGReader->Update();

    vtkNew<vtkImageViewer2> imageViewer;
    imageViewer->SetInputData(jPEGReader->GetOutput());
    // imageViewer->SetColorLevel(0);          // 设置窗位，相当于起始灰度值
    // imageViewer->SetColorWindow(255);       // 设置窗宽，灰度显示范围：[-窗位,窗宽-窗位]
    // imageViewer->SetSlice(50);              // 当前显示切片的索引（适用于医学图像）
    // imageViewer->SetSliceOrientationToXY(); // 设置当前切片显示方向（垂直于XY，Z轴方向）

    vtkNew<vtkRenderWindowInteractor> iren;
    imageViewer->SetupInteractor(iren);
    imageViewer->GetRenderWindow()->SetWindowName("vtkImageViewer2");
    imageViewer->Render();

    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST201

#ifdef TEST202

#include <vtkImageData.h>
#include <vtkImageResliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

// vtkImageSlice 已经取代了 vtkImageActor

int main(int, char*[])
{
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName("../resources/test1.jpg");
    jPEGReader->Update();

    vtkNew<vtkImageResliceMapper> imageResliceMapper;
    imageResliceMapper->SetInputData(jPEGReader->GetOutput());

    vtkNew<vtkImageSlice> imageSlice;
    imageSlice->SetMapper(imageResliceMapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddViewProp(imageSlice);
    renderer->ResetCamera();
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("ImageSlice");

    vtkNew<vtkInteractorStyleImage> style;

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST202

#ifdef TEST301

#include <vtkActor.h>
#include <vtkImageActor.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName("../resources/test1.jpg");
    jPEGReader->Update();

    // 输入图片大小
    int extent[6] {};
    jPEGReader->GetOutput()->GetExtent(extent);
    std::cout << "extent: " << extent[0] << " " << extent[1] << " " << extent[2] << " " << extent[3] << " " << extent[4] << " " << extent[5] << '\n';

    // 裁剪图片
    vtkNew<vtkImageClip> imageClip;
    imageClip->SetInputData(jPEGReader->GetOutput());
    imageClip->SetOutputWholeExtent(0, 100, 0, 100, 0, 0); // 裁剪范围
    imageClip->ClipDataOn();
    imageClip->Update();

    //---------------------------------------------------------------------
    vtkNew<vtkImageActor> imageActor;
    imageActor->GetMapper()->SetInputData(imageClip->GetOutput());

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(imageActor);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("ImageClip");

    // 鼠标左键修改WindowLevel
    // https://vtk.org/doc/nightly/html/classvtkInteractorStyleImage.html
    vtkNew<vtkInteractorStyleImage> style;
    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetInteractorStyle(style);
    interactor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST301

#ifdef TEST302

#include <vtkImageBlend.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageViewer2.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName("../resources/test1.jpg");
    jPEGReader->Update();

    // 输入图片的大小
    int extent[6] {};
    jPEGReader->GetOutput()->GetExtent(extent);
    std::cout << "extent: " << extent[0] << " " << extent[1] << " " << extent[2] << " " << extent[3] << " " << extent[4] << " " << extent[5] << '\n';

    // 创建一个vtkImageData
    vtkNew<vtkImageCanvasSource2D> drawing;
    drawing->SetScalarTypeToUnsignedChar();
    drawing->SetNumberOfScalarComponents(4); // 设置颜色分量（r,g,b,a)共4个，设置为3则只有(r,g,b)
    drawing->SetExtent(extent);              // 大小
    drawing->SetDrawColor(255, 0, 0, 255);   // 注意此处的颜色值为[0,255]，第4个参数是alpha，可以理解为透明度
    drawing->FillBox(0, 200, 0, 200);        // 画一个矩形（实心）
    drawing->SetDrawColor(255, 255, 0, 255); // 重新设置颜色
    drawing->FillBox(300, 500, 300, 350);    // FillBox的参数一定要注意：0:lowerX,1:upperX,2:lowerY,3:upperY
    drawing->Update();

    // 混合图片
    vtkNew<vtkImageBlend> blend;
    blend->AddInputData(jPEGReader->GetOutput());
    blend->AddInputData(drawing->GetOutput());
    blend->SetOpacity(0, 0.5); // 设置第一个输入的图片不透明度
    blend->SetOpacity(1, 0.5); // 设置第二个输入的图片不透明度

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkImageViewer2> imageViewer;
    imageViewer->SetInputConnection(blend->GetOutputPort());
    imageViewer->SetupInteractor(renderWindowInteractor);
    imageViewer->GetRenderer()->ResetCamera();

    imageViewer->GetRenderWindow()->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST302

#ifdef TEST303

#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageBlend.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {
class Style : public vtkInteractorStyleRubberBand2D
{
public:
    static Style* New();
    vtkTypeMacro(Style, vtkInteractorStyleRubberBand2D);

    void OnLeftButtonUp() override
    {
        Superclass::OnLeftButtonUp();
        if (this->Interactor)
        {
            auto width  = this->Interactor->GetSize()[0];
            auto height = this->Interactor->GetSize()[1];

            auto leftBottom_X = std::min(this->StartPosition[0], this->EndPosition[0]);
            auto leftBottom_Y = std::min(this->StartPosition[1], this->EndPosition[1]);
            auto rightTop_X   = std::max(this->StartPosition[0], this->EndPosition[0]);
            auto rightTop_Y   = std::max(this->StartPosition[1], this->EndPosition[1]);

            vtkNew<vtkImageCanvasSource2D> drawing;
            drawing->SetScalarTypeToUnsignedChar();
            drawing->SetNumberOfScalarComponents(4); // 设置颜色为4维，即rgba
            drawing->SetExtent(0, width - 1, 0, height - 1, 0, 0);
            drawing->SetDrawColor(0, 0, 0, 100);                                  // 黑色，alpha为100
            drawing->FillBox(0, width - 1, 0, height - 1);                        // 给整个窗口添加一层阴影
            drawing->SetDrawColor(255, 255, 255, 0);                              // 白色，alpha为0，即完全透明
            drawing->FillBox(leftBottom_X, rightTop_X, leftBottom_Y, rightTop_Y); // 注意参数顺序
            drawing->Update();

            vtkNew<vtkImageBlend> blend;
            blend->AddInputData(m_image);
            blend->AddInputData(drawing->GetOutput());
            blend->SetOpacity(0, 1.);
            blend->SetOpacity(1, 1.);
            blend->Update();

            m_mapper->SetInputData(blend->GetOutput());
            this->Interactor->Render();
        }
    }

    void SetMapperImage(vtkImageMapper3D* mapper, vtkImageData* image)
    {
        m_mapper = mapper;
        m_image  = image;
    }

private:
    vtkImageData* m_image;
    vtkImageMapper3D* m_mapper;
};

vtkStandardNewMacro(Style);
} // namespace

int main(int argc, char* argv[])
{
    vtkNew<vtkJPEGReader> reader;
    reader->SetFileName("../resources/test1.jpg");
    reader->Update();

    vtkNew<vtkImageActor> actor;
    actor->GetMapper()->SetInputData(reader->GetOutput());

    auto extent = reader->GetOutput()->GetExtent();
    auto width  = extent[1] - extent[0] + 1;
    auto height = extent[3] - extent[2] + 1;

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->GetActiveCamera()->SetParallelScale(0.5 * width / (static_cast<double>(width) / height)); // 图片铺满窗口

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);
    window->SetSize(width, height);

    vtkNew<Style> style;
    style->SetMapperImage(actor->GetMapper(), reader->GetOutput());

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(window);
    renderWindowInteractor->SetInteractorStyle(style);

    window->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST56

#ifdef TEST401

#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

class Callback : public vtkCommand
{
public:
    static Callback* New()
    {
        return new Callback;
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {

        auto window = static_cast<vtkWindow*>(caller);
        auto w      = window->GetSize()[0];
        auto h      = window->GetSize()[1];
        double w_h  = static_cast<double>(w) / static_cast<double>(h);
        double x_y  = m_xDistance / m_yDistance;

        std::cout << "Window size: " << w << ',' << h << "\tCamera paralle scale: " << m_camera->GetParallelScale() << '\n';

        // vtk窗口只有高度变化时，才会缩放图元，即修改相机
        // 01_04_TEST2
        if (w_h > x_y)
        {
            m_camera->SetParallelScale(0.5 * m_yDistance);
        }
        else
        {
            m_camera->SetParallelScale(0.5 * m_xDistance / w_h);
        }
    }

    /// @brief 设置窗口的相机和图片的大小
    /// @param camera
    /// @param x
    /// @param y
    void SetData(vtkSmartPointer<vtkCamera> camera, double x, double y)
    {
        m_camera    = camera;
        m_xDistance = x;
        m_yDistance = y;
    }

private:
    vtkSmartPointer<vtkCamera> m_camera;
    double m_xDistance { 0.0 };
    double m_yDistance { 0.0 };
};

// https://kitware.github.io/vtk-examples/site/Cxx/Images/FillWindow/

int main(int, char*[])
{
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName("../resources/test1.jpg");
    jPEGReader->Update();

    int extent[6];
    jPEGReader->GetOutput()->GetExtent(extent);
    double origin[3];
    jPEGReader->GetOutput()->GetOrigin(origin);
    double spacing[3];
    jPEGReader->GetOutput()->GetSpacing(spacing);

    std::cout << "extent: " << extent[0] << ' ' << extent[1] << ' ' << extent[2] << ' ' << extent[3] << ' ' << extent[4] << ' ' << extent[5] << '\n';
    std::cout << "origin: " << origin[0] << ' ' << origin[1] << ' ' << origin[2] << '\n';
    std::cout << "spacing: " << spacing[0] << ' ' << spacing[1] << ' ' << spacing[2] << '\n';

    // 图片的宽度和高度
    double xd = (extent[1] - extent[0] + 1) * spacing[0];
    double yd = (extent[3] - extent[2] + 1) * spacing[1];

    vtkNew<vtkImageActor> imageActor;
    imageActor->SetInputData(jPEGReader->GetOutput());

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(imageActor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("FillWindow");
    renderWindow->SetSize(800, 600);

    vtkCamera* camera = renderer->GetActiveCamera();
    camera->ParallelProjectionOn();
    camera->SetParallelScale(0.5 * xd / (800. / 600.));

    auto resizeCB = Callback::New();
    resizeCB->SetData(camera, xd, yd);
    renderWindow->AddObserver(vtkCommand::WindowResizeEvent, resizeCB);

    vtkNew<vtkInteractorStyleRubberBand2D> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST401

#ifdef TEST501

#include <vtkImageData.h>
#include <vtkImageLuminance.h>
#include <vtkImageResliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

// vtkImageLuminance使用亮度方程对R、G和B分量进行适当加权
// https://examples.vtk.org/site/Cxx/Images/ImageLuminance/

int main(int, char*[])
{
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName("../resources/test1.jpg");
    jPEGReader->Update();

    // 将彩色图像转换为灰度图像
    vtkNew<vtkImageLuminance> luminance;
    luminance->SetInputData(jPEGReader->GetOutput());
    luminance->Update();

    vtkNew<vtkImageResliceMapper> imageResliceMapper;
    imageResliceMapper->SetInputData(luminance->GetOutput());

    vtkNew<vtkImageSlice> imageSlice;
    imageSlice->SetMapper(imageResliceMapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddViewProp(imageSlice);
    renderer->ResetCamera();
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkInteractorStyleImage> style;

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST501

#ifdef TEST502

#include <vtkImageData.h>
#include <vtkImageMagnitude.h>
#include <vtkImageResliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

// vtkImageMagnitude对分量进行简单的平方和计算（即欧几里得范数，而不是二次均值）
// https://examples.vtk.org/site/Cxx/Images/ImageLuminance/

int main(int, char*[])
{
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName("../resources/test1.jpg");
    jPEGReader->Update();

    // 将彩色图像转换为灰度图像
    vtkNew<vtkImageMagnitude> luminance;
    luminance->SetInputData(jPEGReader->GetOutput());
    luminance->Update();

    vtkNew<vtkImageResliceMapper> imageResliceMapper;
    imageResliceMapper->SetInputData(luminance->GetOutput());

    vtkNew<vtkImageSlice> imageSlice;
    imageSlice->SetMapper(imageResliceMapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddViewProp(imageSlice);
    renderer->ResetCamera();
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkInteractorStyleImage> style;

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST502

#ifdef TEST601

#include <vtkActor.h>
#include <vtkBMPReader.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageLuminance.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMergeFilter.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWarpScalar.h>

// https://examples.vtk.org/site/Cxx/Images/ImageWarp/

int main(int argc, char* argv[])
{
    vtkNew<vtkBMPReader> reader;
    reader->SetFileName("../resources/masonry.bmp");
    reader->Update();

    vtkNew<vtkImageLuminance> luminance;
    luminance->SetInputConnection(reader->GetOutputPort());
    luminance->Update();

    vtkNew<vtkImageDataGeometryFilter> geometry;
    geometry->SetInputConnection(luminance->GetOutputPort());
    geometry->Update();

    vtkNew<vtkWarpScalar> warp;
    warp->SetInputConnection(geometry->GetOutputPort());
    warp->SetScaleFactor(-0.1);
    warp->Update();

    vtkNew<vtkMergeFilter> merge;
    merge->SetGeometryInputData(warp->GetOutput());
    merge->SetScalarsData(vtkDataSet::SafeDownCast(reader->GetOutput()));
    merge->GlobalWarningDisplayOff();
    merge->Update();

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputConnection(merge->GetOutputPort());
    mapper->SetScalarRange(0, 255);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> ren;
    ren->AddActor(actor);
    ren->ResetCamera();
    ren->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren);
    renWin->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST601