/*
* 1.打印vtkCamera的信息
* 2.窗口缩放，图元不缩放，即图元始终和显示器比例一致
* 3.图片满屏显示，适配窗口
* 4.部分图元大小不变，位置变化，即类似广告牌，只响应旋转平移，不响应缩放
* 5.照相机矩阵变换，相机旋转，actor不旋转
* 6.前后平面裁剪，如果摄像机处于图元内部，设置裁剪平面图元仍会被裁剪
* 7.相机变换原理
* 8.Actor变换（旋转，缩放，平移）相机的模型变换矩阵
* 9.相机旋转Actor也跟着旋转，即Actor的某一部分始终朝向观察者
* 10.TEST9备份
* 11.vtkBillboardTextActor3D 鼠标旋转，文本始终朝向观察者，广告板
* 12 vtkFollower 始终面向镜头 https://kitware.github.io/vtk-examples/site/Cxx/Visualization/Follower/
* 13.vtkFollower 的原理
* 14.vtkFollower vtkDistanceToCamera 指定图元大小不变，方向不变，始终在最上层
* 15.vtkActor2D 不响应缩放，平移，旋转
* 
* 88.照相机属性，获取网格的最大最小边界，无限放大缩小
*/

// https://blog.csdn.net/liushao1031177/article/details/116903698
// https://www.cnblogs.com/ybqjymy/p/13925462.html

#define TEST15

#ifdef TEST1

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLegendScaleActor.h>
#include <vtkProperty.h>
#include <vtkObjectFactory.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <iostream>

class CustomStyle :
    public vtkInteractorStyleRubberBand3D
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleRubberBand3D);

protected:
    void OnLeftButtonDown() override
    {
        if (this->Interactor && this->GetCurrentRenderer())
        {
            auto renderer = this->GetCurrentRenderer();
            auto camera = renderer->GetActiveCamera();
            std::cout << "-----------------------------------------------\n";
            camera->Print(std::cout);
        }

        Superclass::OnLeftButtonDown();
    }

    void OnMouseMove() override
    {

        Superclass::OnMouseMove();
    }
};

vtkStandardNewMacro(CustomStyle);


int main(int, char* [])
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->SetCenter(0, 0, 0);
    cubeSource->SetXLength(10);
    cubeSource->SetYLength(20);
    cubeSource->SetZLength(30);
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cubeSource->GetOutput());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkLegendScaleActor> scale;
    scale->BottomAxisVisibilityOff();//下面的比例尺不显示
    scale->LeftAxisVisibilityOff();  //左边比例尺不显示
    scale->RightAxisVisibilityOff();
    scale->TopAxisVisibilityOff();

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor(actor);
    renderer->AddActor(scale);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->SetParallelProjection(true); // 平行投影

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<CustomStyle> style;
    iren->SetInteractorStyle(style);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLegendScaleActor.h>
#include <vtkProperty.h>
#include <vtkButtonWidget.h>
#include <vtkObjectFactory.h>
#include <vtkCallbackCommand.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkTextProperty.h>
#include <vtkCoordinate.h>

#include <vector>
#include <iostream>

class WindowResizeCB :
    public vtkCommand
{
public:
    static WindowResizeCB* New();
    vtkTypeMacro(WindowResizeCB, vtkCommand);
protected:
    virtual void Execute(vtkObject* caller, unsigned long, void* callData)
    {
        // 这里获取到的大小已经是窗口缩放后的

        if (auto window = vtkRenderWindow::SafeDownCast(caller))
        {

            auto size = window->GetSize();
            std::cout << size[0] << '\t' << size[1] << '\n';
        }

        if (m_renderer && m_renderer->GetActiveCamera())
        {
            // vtk窗口只有高度变化时，才会缩放图元，即修改相机
            // 因此在正交投影下，当窗口高度变化时，调用SetParallelScale即可让图元大小不变
            auto size = m_renderer->GetSize();
            auto factor = size[1] / m_height;
            auto scale = m_renderer->GetActiveCamera()->GetParallelScale();
            m_renderer->GetActiveCamera()->SetParallelScale(scale * factor);
            m_height = size[1];

            auto window = vtkRenderWindow::SafeDownCast(caller);
            window->Render();

        }

        if (m_scaleActor)
        {
            //std::cout << "--------------------------------------------------\n";
            ////m_scaleActor->Print(std::cout);

            //m_scaleActor->GetLegendLabelProperty()->Print(std::cout);
            //std::cout << "--------------------------------------------------\n";

            //m_scaleActor->GetLegendTitleProperty()->Print(std::cout);
        }
    }
public:
    void SetRenderer(const vtkSmartPointer<vtkRenderer>& renderer)
    {
        m_renderer = renderer;
    }
    void SetScaleActor(const vtkSmartPointer<vtkLegendScaleActor>& actor)
    {
        m_scaleActor = actor;
    }
    void SetParallelScale(double scale)
    {
        m_scale = scale;
    }
private:
    vtkSmartPointer<vtkRenderer> m_renderer{ nullptr };
    vtkSmartPointer<vtkLegendScaleActor> m_scaleActor{ nullptr };
    double m_scale{ 0. };
    double m_height{ 600.0 };
};

vtkStandardNewMacro(WindowResizeCB);

int main(int, char* [])
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->SetCenter(0, 0, 0);
    cubeSource->SetXLength(10);
    cubeSource->SetYLength(20);
    cubeSource->SetZLength(30);
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cubeSource->GetOutput());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    // 比例尺
    vtkNew<vtkLegendScaleActor> scaleActor;
    scaleActor->BottomAxisVisibilityOff();//下面的比例尺不显示
    scaleActor->LeftAxisVisibilityOff();  //左边比例尺不显示
    scaleActor->RightAxisVisibilityOff();
    scaleActor->TopAxisVisibilityOff();
    //scaleActor->GetBounds();
    //scaleActor->GetCornerOffsetFactorMaxValue();
    //scaleActor->GetCornerOffsetFactorMinValue();
    //scaleActor->GetCornerOffsetFactor();
    //scaleActor->GetLabelModeMaxValue();
    //scaleActor->GetLabelModeMinValue();
    scaleActor->GetLegendLabelProperty();
    scaleActor->GetLegendTitleProperty()->SetColor(1, 0, 0);
    scaleActor->SetLegendVisibility(true);


    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(scaleActor);
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->SetParallelProjection(true); // 平行投影
    auto parallelScale = renderer->GetActiveCamera()->GetParallelScale();


    // window
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<WindowResizeCB> resizeCB;
    resizeCB->SetRenderer(renderer);
    resizeCB->SetScaleActor(scaleActor);
    resizeCB->SetParallelScale(parallelScale);
    renWin->AddObserver(vtkCommand::WindowResizeEvent, resizeCB);

    // interactor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkCallbackCommand> cb;
    cb->SetCallback([](vtkObject* caller, unsigned long eventId, void* clientData, void* callData) {
        if (auto interactor = vtkRenderWindowInteractor::SafeDownCast(caller))
        {
            if (auto renderer = vtkRenderer::SafeDownCast(static_cast<vtkObject*>(clientData)))
            {

            }

            if (auto window = vtkRenderWindow::SafeDownCast(static_cast<vtkObject*>(clientData)))
            {
                window->SetSize(400, 400);
            }
        }
        });
    cb->SetClientData(reinterpret_cast<void*>(renderer.GetPointer()));
    //cb->SetClientData(reinterpret_cast<void*>(renWin.GetPointer()));
    iren->AddObserver(vtkCommand::LeftButtonReleaseEvent, cb);

    vtkNew< vtkInteractorStyleRubberBand3D> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <vtkCamera.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkJPEGReader.h>
#include <vtkImageActor.h>
#include <vtkCommand.h>

#include <filesystem>

class Callback :public vtkCommand
{
public:
    static Callback* New()
    {
        return new Callback;
    }
    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {


        auto window = static_cast<vtkWindow*>(caller);
        auto w = window->GetSize()[0];
        auto h = window->GetSize()[1];
        double w_h = static_cast<double>(w) / static_cast<double>(h);
        double x_y = m_xDistance / m_yDistance;

        std::cout << "resize window:\t" << m_camera->GetParallelScale() << '\t' << w << ',' << h << '\t' <<
            m_xDistance << ',' << m_yDistance << "       \t" << w_h << "      \t" << x_y << '\n';

        if (w_h > x_y)
        {
            m_camera->SetParallelScale(0.5 * m_yDistance);
        }
        else
        {
            m_camera->SetParallelScale(0.5 * m_xDistance / w_h);
        }

        //if (w > h)
        //{
        //    m_camera->SetParallelScale(0.5 * m_yDistance);
        //}
        //else
        //{
        //    m_camera->SetParallelScale(0.5 * m_yDistance / w_h);
        //}

    }
    void SetData(vtkSmartPointer<vtkCamera> camera, double x, double y)
    {
        m_camera = camera;
        m_xDistance = x;
        m_yDistance = y;
    }
private:
    vtkSmartPointer<vtkCamera> m_camera;
    double m_xDistance{ 0.0 };
    double m_yDistance{ 0.0 };
};

int main(int, char* [])
{
    auto path = std::filesystem::current_path();
    std::string file = path.string() + "\\file\\57_test1.jpg";
    //std::string file = "57_test2.jpg";

    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName(file.c_str());
    jPEGReader->Update();

    int extent[6];
    jPEGReader->GetOutput()->GetExtent(extent);
    double origin[3];
    jPEGReader->GetOutput()->GetOrigin(origin);
    double spacing[3];
    jPEGReader->GetOutput()->GetSpacing(spacing);

    vtkNew<vtkImageActor> imageActor;
    imageActor->SetInputData(jPEGReader->GetOutput());

    // Setup renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(imageActor);
    renderer->ResetCamera();

    // Setup render window
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("FillWindow");

    vtkCamera* camera = renderer->GetActiveCamera();
    camera->ParallelProjectionOn();

    double xc = origin[0] + 0.5 * (extent[0] + extent[1]) * spacing[0];
    double yc = origin[1] + 0.5 * (extent[2] + extent[3]) * spacing[1];
    double xd = (extent[1] - extent[0] + 1) * spacing[0]; // not used
    double yd = (extent[3] - extent[2] + 1) * spacing[1];

    //float d = camera->GetDistance();
    //camera->SetParallelScale(0.5f * static_cast<float>(yd));
    //camera->SetParallelScale(0.5f * static_cast<float>(xd));
    //camera->SetFocalPoint(xc, yc, 0.0);
    //camera->SetPosition(xc, yc, +d);

    //if (renderWindow->GetSize()[0] < renderWindow->GetSize()[1])
    //{
    //    camera->Zoom(double(renderWindow->GetSize()[0]) / renderWindow->GetSize()[1]);
    //}
    renderWindow->SetSize(400, 800);
    auto w = renderWindow->GetSize()[0];
    auto h = renderWindow->GetSize()[1];
    //w>h x>y
    //w>h x<y
    //camera->SetParallelScale(0.5 * yd);
    //w<h x<y
    //w<h x>y
    camera->SetParallelScale(0.5 * xd / ((double)w / h));

    auto cb = Callback::New();
    cb->SetData(camera, xd, yd);
    renderWindow->AddObserver(vtkCommand::WindowResizeEvent, cb);


    // 1和2效果一样
    // *** 1
    //camera->SetParallelScale(0.5 * yd);
    //camera->Zoom(w/h);
    // *** 2
    //camera->SetParallelScale(0.5 * yd/((double)w/h));

    /*zoom的源码：*/
    //this->SetParallelScale(this->ParallelScale / amount);

    // Setup render window interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkInteractorStyleImage> style;
    renderWindowInteractor->SetInteractorStyle(style);
    //renderWindowInteractor->SetInteractorStyle(nullptr);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST3

#ifdef TEST4

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkDistanceToCamera.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkCamera.h>

#include <vtkPointSet.h>
#include <array>
#include <iostream>

class InteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static InteractorStyle* New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonUp() override
    {
        if (this->Interactor)
        {
            auto camera = this->CurrentRenderer->GetActiveCamera();
            std::cout << camera->GetParallelScale() << '\n';
        }


        Superclass::OnLeftButtonUp();
    }
};

vtkStandardNewMacro(InteractorStyle);

int main(int, char* [])
{
    vtkNew<vtkPolyData> polyPoint;
    vtkNew<vtkPolyData> polyLine;
    vtkNew<vtkPoints> points_;

    {
        std::array<float, 4 * 3> vertices{
            10,10,0,
            20,10,0,
            20,20,0,
            10,20,0
        };


        vtkNew<vtkCellArray> cells;

        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            points_->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
        }
        for (long long i = 0; i < vertices.size() / 3; ++i)
        {
            cells->InsertNextCell({ i });
        }

        polyPoint->SetPoints(points_);
        polyPoint->SetVerts(cells);
    }

    {
        std::array<float, 4 * 3> vertices
        {
            30,30,0,
            40,30,0,
            40,40,0,
            30,40,0
        };

        std::array<long long, 4 * 2> indices
        {
            0,1,
            1,2,
            2,3,
            3,0
        };

        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
        }
        for (size_t i = 0; i < indices.size(); i += 2)
        {
            cells->InsertNextCell({ indices[i],indices[i + 1] });
        }

        polyLine->SetPoints(points);
        polyLine->SetLines(cells);
    }

    vtkNew<vtkPolyDataMapper> mapperLine;
    mapperLine->SetInputData(polyLine);

    vtkNew<vtkActor> actorLine;
    actorLine->SetMapper(mapperLine);
    actorLine->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkPointSet> pointSet;
    pointSet->SetPoints(points_);

    vtkNew<vtkDistanceToCamera> distToCamera;
    distToCamera->SetInputData(pointSet);
    distToCamera->SetScreenSize(50);

    vtkNew<vtkSphereSource> sphere;
    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputConnection(distToCamera->GetOutputPort());
    glyph->SetSourceConnection(sphere->GetOutputPort());

    //glyph->SetScaleModeToScaleByScalar();
    glyph->SetInputArrayToProcess(
        0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera");

    vtkNew<vtkPolyDataMapper> mapperGlyph;
    mapperGlyph->SetInputConnection(glyph->GetOutputPort());
    mapperGlyph->SetScalarVisibility(false); // 不关闭时，鼠标缩放会改变球的颜色

    vtkNew<vtkActor> actorGlyph;
    actorGlyph->SetMapper(mapperGlyph);



    //---------------------------------------------------------------------------

    // A renderer and render window.
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    distToCamera->SetRenderer(renderer);
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("DistanceToCamera");

    // Give DistanceToCamera a pointer to the renderer.


    //renderer->ResetCamera();
    //std::cout << renderer->GetActiveCamera()->GetParallelProjection() << '\n';
    //renderer->GetActiveCamera()->ParallelProjectionOn();

    // Add the actors to the scene.
    renderer->AddActor(actorGlyph);
    renderer->AddActor(actorLine);

    // An interactor.
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<InteractorStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Render an image (lights and cameras are created automatically).
    renderWindow->Render();
    renderer->ResetCamera();
    // Begin mouse interaction.
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST4

#ifdef TEST5

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkBillboardTextActor3D.h>
#include <vtkMatrix4x4.h>
#include <vtkTextProperty.h>
#include <vtkPerspectiveTransform.h>

#include <array>
#include <iostream>

namespace
{
    std::array<float, 3 * 3> vertices1{
        -5,-1,0,
        -4,1,0,
        -3,-1,0
    };

    std::array<float, 3 * 3> vertices2{
        3,-1,0,
        4,1,0,
        5,-1,0
    };

    std::array<float, 3 * 3> vertices3{
        -1,-1,0,
        0,1,0,
        1,-1,0
    };

    vtkNew<vtkActor> actor1;
    vtkNew<vtkActor> actor2;
    vtkNew<vtkActor> actor3;

    class InteractorStyle : public vtkInteractorStyleTrackballCamera
    {
    public:
        static InteractorStyle* New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleTrackballCamera);

        void OnLeftButtonUp() override
        {
            std::cout << "#############################################################\n";
            if (this->Interactor)
            {
                // 打印照相机信息
                if (0)
                {
                    double eyePos[3]{ 0. };
                    this->CurrentRenderer->GetActiveCamera()->GetEyePosition(eyePos);
                    auto camPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();
                    auto focalPos = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();

                    std::cout << "focal pos: " << focalPos[0] << ',' << focalPos[1] << ',' << focalPos[2] << '\n';
                    std::cout << "camera pos: " << camPos[0] << ',' << camPos[1] << ',' << camPos[2] << '\n';
                    std::cout << "eye pos: " << eyePos[0] << ',' << eyePos[1] << ',' << eyePos[2] << '\n';
                    std::cout << "------------------------------------\n";
                    // modelView和view是同一个矩阵
                    auto model = this->CurrentRenderer->GetActiveCamera()->GetModelTransformMatrix();
                    auto projection = this->CurrentRenderer->GetActiveCamera()->GetProjectionTransformMatrix(this->CurrentRenderer);
                    auto modelView = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();
                    auto eye = this->CurrentRenderer->GetActiveCamera()->GetEyeTransformMatrix();
                    auto view = this->CurrentRenderer->GetActiveCamera()->GetViewTransformMatrix();
                    //std::cout << "model\n";
                    //model->Print(std::cout);
                    //std::cout << "eye\n";
                    //eye->Print(std::cout);
                    std::cout << "projection\n";
                    projection->Print(std::cout);
                    std::cout << "modelView\n";
                    modelView->Print(std::cout);
                    //std::cout << "view\n";
                    //view->Print(std::cout);

                    // 照相机所有信息
                    //this->CurrentRenderer->GetActiveCamera()->Print(std::cout);
                }



                if (0)
                {
                    static std::array<double, 3> lastCameraPos;
                    static vtkSmartPointer<vtkMatrix4x4> lastMV{ nullptr };

                    double eyePos[3]{ 0. };
                    this->CurrentRenderer->GetActiveCamera()->GetEyePosition(eyePos);
                    auto camPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();
                    auto focalPos = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();

                    std::cout << "focal pos: " << focalPos[0] << ',' << focalPos[1] << ',' << focalPos[2] << '\n';
                    std::cout << "camera pos: " << camPos[0] << ',' << camPos[1] << ',' << camPos[2] << '\n';
                    std::cout << "eye pos: " << eyePos[0] << ',' << eyePos[1] << ',' << eyePos[2] << '\n';
                    std::cout << "------------------------------------\n";
                    auto model = this->CurrentRenderer->GetActiveCamera()->GetModelTransformMatrix();
                    auto projection = this->CurrentRenderer->GetActiveCamera()->GetProjectionTransformMatrix(this->CurrentRenderer);
                    auto modelView = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();
                    auto eye = this->CurrentRenderer->GetActiveCamera()->GetEyeTransformMatrix();
                    auto view = this->CurrentRenderer->GetActiveCamera()->GetViewTransformMatrix();
                    //std::cout << "model\n";
                    //model->Print(std::cout);
                    //std::cout << "eye\n";
                    //eye->Print(std::cout);
                    std::cout << "view\n";
                    view->Print(std::cout);
                    //std::cout << "projection\n";
                    //projection->Print(std::cout);
                    //std::cout << "modelView\n";
                    //modelView->Print(std::cout);

                    if (lastMV == nullptr)
                    {
                        lastMV = view;
                        lastCameraPos = { camPos[0], camPos[1], camPos[2] };
                    }
                    else
                    {
                        double inMat[4]{ lastCameraPos[0],lastCameraPos[1],lastCameraPos[2],0. };
                        double outMat[4]{ 0. };
                        lastMV->MultiplyPoint(inMat, outMat);
                        std::cout << "compute result: " << outMat[0] << ',' << outMat[1] << ',' << outMat[2] << '\n';

                        lastMV = modelView;
                        //lastCameraPos = { camPos[0], camPos[1], camPos[2] };
                    }

                    //vtkNew<vtkMatrix4x4> modelPro;
                    //vtkMatrix4x4::Multiply4x4(modelView, projection, modelPro);
                    //modelPro->Print(std::cout);
                    // 照相机所有信息
                    //this->CurrentRenderer->GetActiveCamera()->Print(std::cout);
                }

                if (0)
                {
                    auto camPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();
                    auto focalPos = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();

                    if (camPos[2] - focalPos[2] > 0. || camPos[2] - focalPos[2] < 0.)
                    {
                        auto tanY = (camPos[0] - focalPos[0]) / (camPos[2] - focalPos[2]);
                        auto tanX = -(camPos[1] - focalPos[1]) / (camPos[2] - focalPos[2]);
                        auto radianX = std::atan(tanX);
                        auto radianY = std::atan(tanY);
                        auto angleX = radianX / 3.1415926 * 180;
                        auto angleY = radianY / 3.1415926 * 180;

                        std::cout << "X: " << angleX << "\tY: " << angleY << '\n';
                        //std::cout << "X: " << m_rotateX << "\tY: " << m_rotateY << '\n';

                        actor1->RotateX(-m_lastRotateX);
                        actor1->RotateY(-m_lastRotateY);

                        m_lastRotateX = angleX;
                        m_lastRotateY = angleY;

                        actor1->RotateX(angleX);
                        actor1->RotateY(angleY);
                    }
                }
                this->Interactor->Render();
            }


            Superclass::OnLeftButtonUp();
        }

        void OnMouseMove()override
        {
            Superclass::OnMouseMove();

            if (this->Interactor)
            {
                if (this->CurrentRenderer && this->CurrentRenderer->GetActiveCamera())
                {
                    // 相机旋转原理
                    if (0)
                    {
                        if (this->State != VTKIS_ROTATE)
                        {
                            return;
                        }

                        vtkRenderWindowInteractor* rwi = this->Interactor;

                        int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
                        int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

                        const int* size = this->CurrentRenderer->GetRenderWindow()->GetSize();

                        double delta_elevation = -20.0 / size[1];
                        double delta_azimuth = -20.0 / size[0];

                        double rxf = dx * delta_azimuth * this->MotionFactor;
                        double ryf = dy * delta_elevation * this->MotionFactor;

                        vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
                        camera->Azimuth(rxf);           // 绕x旋转
                        camera->Elevation(ryf);         // 绕y旋转
                        camera->OrthogonalizeViewUp();  // 绕z旋转，其实是修改viewup的值

                        if (this->AutoAdjustCameraClippingRange)
                        {
                            this->CurrentRenderer->ResetCameraClippingRange();
                        }

                        if (rwi->GetLightFollowCamera())
                        {
                            this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
                        }

                        rwi->Render();
                    }
                    // 旋转不影响actor
                    if (0)
                    {
                        double eyePos[3]{ 0. };
                        this->CurrentRenderer->GetActiveCamera()->GetEyePosition(eyePos);
                        auto camPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();
                        auto focalPos = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();

                        std::cout << "focal pos: " << focalPos[0] << ',' << focalPos[1] << ',' << focalPos[2] << '\n';
                        std::cout << "camera pos: " << camPos[0] << ',' << camPos[1] << ',' << camPos[2] << '\n';
                        std::cout << "eye pos: " << eyePos[0] << ',' << eyePos[1] << ',' << eyePos[2] << '\n';
                        std::cout << "------------------------------------\n";
                        auto model = this->CurrentRenderer->GetActiveCamera()->GetModelTransformMatrix();
                        auto projection = this->CurrentRenderer->GetActiveCamera()->GetProjectionTransformMatrix(this->CurrentRenderer);
                        auto modelView = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();
                        auto eye = this->CurrentRenderer->GetActiveCamera()->GetEyeTransformMatrix();
                        auto view = this->CurrentRenderer->GetActiveCamera()->GetViewTransformMatrix();
                        //std::cout << "model\n";
                        //model->Print(std::cout);
                        //std::cout << "eye\n";
                        //eye->Print(std::cout);
                        //std::cout << "projection\n";
                        //projection->Print(std::cout);
                        //std::cout << "modelView\n";
                        //modelView->Print(std::cout);
                        //std::cout << "view\n";
                        //view->Print(std::cout);


                        vtkNew<vtkMatrix4x4> invertModelViewMat;
                        vtkMatrix4x4::Invert(view, invertModelViewMat);
                        vtkNew<vtkMatrix4x4> transposeModelViewMat;
                        vtkMatrix4x4::Transpose(view, transposeModelViewMat);

                        // actor1\actor2始终都是最开始的状态
                        // 前提条件是照相机的位置为{0,0,Z}
                        actor1->SetUserMatrix(transposeModelViewMat);
                        actor2->SetUserMatrix(transposeModelViewMat);

                        //this->CurrentRenderer->ResetCamera();
                        //this->Interactor->Render();

                    }
                    // 指定actor始终某一个面朝前，即不会被旋转
                    if (0)
                    {
                        if (this->State != VTKIS_ROTATE)
                        {
                            return;
                        }

                        vtkRenderWindowInteractor* rwi = this->Interactor;

                        int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
                        int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

                        const int* size = this->CurrentRenderer->GetRenderWindow()->GetSize();

                        double delta_elevation = -20.0 / size[1];
                        double delta_azimuth = -20.0 / size[0];

                        double rxf = dx * delta_azimuth * this->MotionFactor;
                        double ryf = dy * delta_elevation * this->MotionFactor;

                        //std::cout << "X: " << -ryf << "\tY: " << rxf << '\n';

                        m_rotateY += rxf;
                        m_rotateX += (-ryf);

                        //actor1->RotateY(rxf);
                        //actor1->RotateX(-ryf);

                        rwi->Render();
                    }

                    if (1)
                    {
                        auto camPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();
                        auto focalPos = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();
                        actor1->SetOrientation(camPos[0] - focalPos[0], camPos[1] - focalPos[1], camPos[2] - focalPos[2]);
                    }
                }
            }


        }

    private:
        double m_rotateX{ 0.0 };
        double m_rotateY{ 0.0 };
        double m_lastRotateX{ 0.0 };
        double m_lastRotateY{ 0.0 };
    };

    vtkStandardNewMacro(InteractorStyle);
}

int main()
{
    {
        vtkNew<vtkPoints> points;
        for (size_t i = 0; i < vertices1.size(); i += 3)
        {
            points->InsertNextPoint(vertices1[i], vertices1[i + 1], vertices1[i + 2]);
        }

        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0,1,2 });
        polyData->SetPoints(points);
        polyData->SetPolys(cells);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        actor1->SetMapper(mapper);
    }

    {
        vtkNew<vtkPoints> points;
        for (size_t i = 0; i < vertices2.size(); i += 3)
        {
            points->InsertNextPoint(vertices2[i], vertices2[i + 1], vertices2[i + 2]);
        }

        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0,1,2 });
        polyData->SetPoints(points);
        polyData->SetPolys(cells);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        actor2->SetMapper(mapper);
    }

    {
        vtkNew<vtkPoints> points;
        for (size_t i = 0; i < vertices3.size(); i += 3)
        {
            points->InsertNextPoint(vertices3[i], vertices3[i + 1], vertices3[i + 2]);
        }

        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0,1,2 });
        polyData->SetPoints(points);
        polyData->SetPolys(cells);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        actor3->SetMapper(mapper);
    }


    actor1->GetProperty()->SetColor(1, 0, 0);
    actor2->GetProperty()->SetColor(0, 1, 0);
    actor3->GetProperty()->SetColor(0, 0, 1);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor1);
    renderer->AddActor(actor2);
    renderer->AddActor(actor3);

    renderer->SetBackground(.1, .2, .3);
    //renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 800);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<InteractorStyle> style;
    iren->SetInteractorStyle(style);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLegendScaleActor.h>
#include <vtkProperty.h>
#include <vtkObjectFactory.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkVectorOperators.h>

#include <iostream>

/*
* 先对顶点进行变换，当相机处于某一个位置时，添加网格（不适配窗口），此时的网格就有可能不能完整显示，即被裁剪
*/

namespace
{
    // 一个网格，绿色
    vtkSmartPointer<vtkActor> GenPolyActor()
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        for (size_t i = 0; i < 100; i++)
        {
            for (size_t j = 0; j < 100; j++)
            {
                points->InsertNextPoint(static_cast<double>(i), static_cast<double>(j), 0.0);

                //static int index = 0;
                //std::cout << index++ << '\t' << i << '\t' << j << '\n';
            }
        }

        for (long long i = 0; i < 99; i++)
        {
            for (long long j = 0; j < 99; j++)
            {
                cells->InsertNextCell({ i * 100 + 0 + j,i * 100 + 1 + j,i * 100 + 101 + j, i * 100 + 100 + j });

                //static int index = 0;
                //std::cout << index++ << '\t' << i * 100 + 0 + j << '\t' << i * 100 + 1 + j << '\t' << i * 100 + 100 + j << '\t' << i * 100 + 101 + j << '\n';
            }
        }

        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        actor->GetProperty()->SetRepresentationToWireframe();
        actor->GetProperty()->SetColor(0, 1, 0);

        return actor;
    }

    // 一个顶点，红色
    vtkSmartPointer<vtkActor> GenVertexActor()
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        points->InsertNextPoint(0, 0, 0);
        vtkIdType pts = 0;
        cells->InsertNextCell(1, &pts);

        polyData->SetPoints(points);
        polyData->SetVerts(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        actor->GetProperty()->SetPointSize(3);
        actor->GetProperty()->SetColor(1, 0, 0);

        return actor;
    }
}

class CustomStyle :
    public vtkInteractorStyleRubberBand3D
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleRubberBand3D);

protected:
    void OnMouseWheelForward() override
    {
        this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

        if (this->CurrentRenderer == nullptr)
        {
            return;
        }

        auto mousePos = vtkVector2d(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
        ZoomByFocalPoint(mousePos, true);
        this->Interactor->Render();
    }

    void OnMouseWheelBackward() override
    {
        this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

        if (this->CurrentRenderer == nullptr)
        {
            return;
        }

        auto mousePos = vtkVector2d(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
        ZoomByFocalPoint(mousePos, false);
        this->Interactor->Render();
    }

    void OnLeftButtonDown() override
    {
        if (this->Interactor && this->GetCurrentRenderer())
        {
            // 显示网格
            auto renderer = this->GetCurrentRenderer();
            renderer->AddActor(GenPolyActor());
            this->Interactor->Render();
        }

        //Superclass::OnLeftButtonDown();
    }

    void OnMiddleButtonUp() override
    {
        if (this->Interactor && this->GetCurrentRenderer())
        {
            // 打印相机信息
            auto renderer = this->GetCurrentRenderer();
            auto camera = renderer->GetActiveCamera();
            std::cout << "-----------------------------------------------\n";
            camera->Print(std::cout);
            //renderer->ResetCameraClippingRange();
            //renderer->ResetCameraClippingRange(0, 1000, 0, 1000, 0, 1000);
            camera->SetClippingRange(-10, 1000);
        }

        //Superclass::OnMiddleButtonUp();
    }


private:
    void ZoomByFocalPoint(const vtkVector2d& focalPoint, bool zoomIn)
    {
        TranslateCameraToFocalPoint(focalPoint, true);
        if (zoomIn)
        {
            Superclass::OnMouseWheelForward();
        }
        else
        {
            Superclass::OnMouseWheelBackward();
        }
        TranslateCameraToFocalPoint(focalPoint, false);
    }

    void TranslateCameraToFocalPoint(const vtkVector2d& point, bool forwardTo)
    {
        this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

        if (this->CurrentRenderer == nullptr)
        {
            return;
        }

        if (this->CurrentRenderer->GetActiveCamera()->GetParallelProjection())
        {
            auto camera = this->CurrentRenderer->GetActiveCamera();
            const int* size = this->CurrentRenderer->GetSize();
            const int* origin = this->CurrentRenderer->GetOrigin();
            const auto& focalPoint = point;

            const vtkVector2d rbCenter2d = vtkVector2d(focalPoint.GetX(), focalPoint.GetY());
            const vtkVector3d rbCenter3d = vtkVector3d(rbCenter2d.GetX(), rbCenter2d.GetY(), 0.0);
            const vtkVector3d worldRBCenter = this->GetCurrentRenderer()->DisplayToWorld(rbCenter3d);

            const vtkVector2d winCenter2d = vtkRecti(origin[0], origin[1], size[0], size[1]).GetCenter();
            const vtkVector3d winCenter3d = vtkVector3d(winCenter2d.GetX(), winCenter2d.GetY(), 0.0);
            const vtkVector3d worldWinCenter = this->GetCurrentRenderer()->DisplayToWorld(winCenter3d);
            const vtkVector3d translation = worldRBCenter - worldWinCenter;

            vtkVector3d pos, fp;
            camera->GetPosition(pos.GetData());
            camera->GetFocalPoint(fp.GetData());

            if (forwardTo)
            {
                pos = pos + translation;
                fp = fp + translation;
            }
            else
            {
                pos = pos - translation;
                fp = fp - translation;
            }

            camera->SetPosition(pos.GetData());
            camera->SetFocalPoint(fp.GetData());
        }
    }
};

vtkStandardNewMacro(CustomStyle);

int main(int, char* [])
{
    vtkNew<vtkLegendScaleActor> scale;
    scale->BottomAxisVisibilityOff();//下面的比例尺不显示
    scale->LeftAxisVisibilityOff();  //左边比例尺不显示
    scale->RightAxisVisibilityOff();
    scale->TopAxisVisibilityOff();

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor(GenPolyActor());
    renderer->AddActor(GenVertexActor());
    renderer->AddActor(scale);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->SetParallelProjection(true); // 平行投影
    //renderer->SetNearClippingPlaneTolerance(0.0001);
    //renderer->SetClippingRangeExpansion(1000);
    renderer->GetActiveCamera()->SetPosition(50, 50, 0);
    renderer->GetActiveCamera()->SetFocalPoint(0, 20, 0);
    renderer->GetActiveCamera()->SetViewUp(1, 0.1, 0);
    //renderer->GetActiveCamera()->SetParallelScale(1.56921);
    //renderer->GetActiveCamera()->SetDistance(1.93185);

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<CustomStyle> style;
    iren->SetInteractorStyle(style);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkMatrix4x4.h>
#include <vtkPerspectiveTransform.h>

#include <array>
#include <iostream>

namespace
{
    class MyInteractorStyle : public vtkInteractorStyleRubberBand3D
    {
    public:
        static MyInteractorStyle* New();
        vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleRubberBand3D);

        void OnLeftButtonUp() override
        {
            Superclass::OnLeftButtonUp();

            static int index = 0;
            std::cout << "----------\t" << index++ << "\t-----------\n";

            if (this->Interactor &&
                this->CurrentRenderer &&
                this->CurrentRenderer->GetActiveCamera())
            {
                auto camera = this->CurrentRenderer->GetActiveCamera();
                camera->Print(std::cout);
            }
        }

        void OnMouseMove()override
        {
            if (this->Interactor &&
                this->CurrentRenderer &&
                this->CurrentRenderer->GetActiveCamera())
            {
                //-----------------------------------------------------------------------
                // 相机旋转原理
                if (this->Interaction == ROTATING)
                {
                    vtkRenderWindowInteractor* rwi = this->Interactor;

                    int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
                    int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

                    const int* size = this->CurrentRenderer->GetRenderWindow()->GetSize();

                    double delta_elevation = -20.0 / size[1];
                    double delta_azimuth = -20.0 / size[0];

                    double rxf = dx * delta_azimuth * this->MotionFactor;
                    double ryf = dy * delta_elevation * this->MotionFactor;

                    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
                    camera->Azimuth(rxf);           // 围绕ViewUp向量旋转
                    camera->Elevation(ryf);         // 围绕ViewUp和viewPlaneNormal（焦点到相机位置的向量）构成面的法线旋转
                    camera->OrthogonalizeViewUp();  // 围绕viewPlaneNormal旋转，即修改viewup的值

                    // 如果相机的焦点为(0,0,0)，ViewUp为(0,1,0)，Position为(0,0,1)
                    // Azimuth就是绕y轴旋转
                    // Elevation就是绕x轴旋转
                    // OrthogonalizeViewUp就是绕z轴旋转

                    if (this->AutoAdjustCameraClippingRange)
                    {
                        this->CurrentRenderer->ResetCameraClippingRange();
                    }

                    if (rwi->GetLightFollowCamera())
                    {
                        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
                    }

                    rwi->Render();
                    return;
                }

                //-----------------------------------------------------------------------
                //相机平移原理
                if (this->Interaction == PANNING)
                {
                    vtkRenderWindowInteractor* rwi = this->Interactor;

                    double viewFocus[4], focalDepth, viewPoint[3];
                    double newPickPoint[4], oldPickPoint[4], motionVector[3];

                    // Calculate the focal depth since we'll be using it a lot

                    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
                    camera->GetFocalPoint(viewFocus);
                    this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
                    focalDepth = viewFocus[2];

                    this->ComputeDisplayToWorld(
                        rwi->GetEventPosition()[0], rwi->GetEventPosition()[1], focalDepth, newPickPoint);

                    // Has to recalc old mouse point since the viewport has moved,
                    // so can't move it outside the loop

                    this->ComputeDisplayToWorld(
                        rwi->GetLastEventPosition()[0], rwi->GetLastEventPosition()[1], focalDepth, oldPickPoint);

                    // Camera motion is reversed

                    motionVector[0] = oldPickPoint[0] - newPickPoint[0];
                    motionVector[1] = oldPickPoint[1] - newPickPoint[1];
                    motionVector[2] = oldPickPoint[2] - newPickPoint[2];

                    camera->GetFocalPoint(viewFocus);
                    camera->GetPosition(viewPoint);
                    camera->SetFocalPoint(
                        motionVector[0] + viewFocus[0], motionVector[1] + viewFocus[1], motionVector[2] + viewFocus[2]);

                    camera->SetPosition(
                        motionVector[0] + viewPoint[0], motionVector[1] + viewPoint[1], motionVector[2] + viewPoint[2]);

                    if (rwi->GetLightFollowCamera())
                    {
                        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
                    }

                    rwi->Render();
                    return;
                }
            }

            Superclass::OnMouseMove();
        }

    public:
        void SetActor(const vtkSmartPointer<vtkActor>& _actor)
        {
            m_actor = _actor;
        }
    private:
        vtkSmartPointer<vtkActor> m_actor{ nullptr };

    }; // class

    vtkStandardNewMacro(MyInteractorStyle);
}

int main()
{
    // 构造actor
    std::array<float, 4 * 3> vertices{
    -1,-1,0,
     1,-1,0,
     1, 1,0,
    -1, 1,0
    };

    vtkNew<vtkActor> actor;
    vtkNew<vtkPoints> points;
    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }

    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0,1,2,3 });
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    renderer->SetBackground(.1, .2, .3);
    //renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    //renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 800);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //InteractorStyle
    vtkNew<MyInteractorStyle> style;
    iren->SetInteractorStyle(style);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST7

#ifdef TEST8

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkMatrix4x4.h>
#include <vtkPerspectiveTransform.h>

#include <array>
#include <iostream>

namespace
{
    class MyInteractorStyle : public vtkInteractorStyleRubberBand3D
    {
    public:
        static MyInteractorStyle* New();
        vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleRubberBand3D);

        void OnLeftButtonUp() override
        {
            Superclass::OnLeftButtonUp();

            if (this->Interactor &&
                this->CurrentRenderer &&
                this->CurrentRenderer->GetActiveCamera())
            {
                auto camera = this->CurrentRenderer->GetActiveCamera();
                auto camPos = camera->GetPosition();
                auto focalPos = camera->GetFocalPoint();
                auto viewUp = camera->GetViewUp();

                std::cout << "------------------------------------------\n";
                std::cout << "focal pos: " << focalPos[0] << ',' << focalPos[1] << ',' << focalPos[2] << '\n';
                std::cout << "camera pos: " << camPos[0] << ',' << camPos[1] << ',' << camPos[2] << '\n';
                std::cout << "viewUp: " << viewUp[0] << ',' << viewUp[1] << ',' << viewUp[2] << '\n';
                std::cout << "------------------------------------------\n";

                double data[3]{ 0. };
                m_actor->GetOrientation(data);
                std::cout << "orientation: " << data[0] << ',' << data[1] << ',' << data[2] << '\n';
                m_actor->GetOrigin(data);
                std::cout << "origin: " << data[0] << ',' << data[1] << ',' << data[2] << '\n';
                m_actor->GetPosition(data);
                std::cout << "position: " << data[0] << ',' << data[1] << ',' << data[2] << '\n';

                //---------------------------------------------------------------------------------------------
                // 对单个actor进行模型变换
                if (true && m_actor)
                {
                    // 缩放矩阵，xyz都缩放为原来的0.5
                    double scale[4][4]{
                        0.5, 0.0, 0.0, 0.0,
                        0.0, 0.5, 0.0, 0.0,
                        0.0, 0.0, 0.5, 0.0,
                        0.0, 0.0, 0.0, 1.0
                    };
                    // 平移矩阵，xy都平移0.5个单位
                    double translation[4][4]{
                        1.0, 0.0, 0.0, 0.5,
                        0.0, 1.0, 0.0, 0.5,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0
                    };
                    // 旋转矩阵，绕y轴旋转30°
                    double alpha = 30 / 180.0 * 3.1415926;
                    double rotation[4][4]{
                        std::cos(alpha), 0.0, std::sin(alpha), 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        -sin(alpha), 0.0, std::cos(alpha), 0.0,
                        0.0, 0.0, 0.0, 1.0
                    };

                    vtkNew<vtkMatrix4x4> mat;
                    for (size_t i = 0; i < 4; i++)
                    {
                        for (size_t j = 0; j < 4; j++)
                        {
                            //mat->SetElement(i, j, rotation[i][j]);
                            mat->SetElement(i, j, translation[i][j]);
                            //mat->SetElement(i, j, scale[i][j]);
                        }
                    }

                    // 矩阵都只执行一次，不会修改网格的顶点
                    //m_actor->SetUserMatrix(mat); // 使用矩阵对模型进行变换

                    // 旋转每调用一次执行一次
                    //m_actor->RotateWXYZ(30, 0, 1, 0); // 绕着向量(0,1,0)旋转30°
                    //m_actor->RotateY(180);       // 绕y轴旋转180
                    //m_actor->RotateX(30);        // 绕x轴旋转30°


                    // 缩放比率只要不变，无论调用多少次都只缩放一次
                    // 网格的顶点并不会被改变
                    //m_actor->SetScale(0.5);      // xyz都缩放为原来的0.5

                    // 设置Actor的方向
                    // 绕x,y,z分别旋转多少30°，0°，0°
                    // 先执行RotateZ再RotateX最后RotateY
                    m_actor->SetOrientation(30, 0, 0);
                }

                //---------------------------------------------------------------------------------------------
                // 给相机设置模型变换矩阵
                // 会将所有显示的actor都设置该变换矩阵
                // 设置模型变换矩阵后，模型的顶点坐标并不会改变
                if (false && m_actor)
                {
                    // 缩放矩阵，xyz都缩放为原来的0.5
                    double scale[16]{
                        0.5, 0.0, 0.0, 0.0,
                        0.0, 0.5, 0.0, 0.0,
                        0.0, 0.0, 0.5, 0.0,
                        0.0, 0.0, 0.0, 1.0
                    };
                    // 平移矩阵，xy都平移0.5个单位
                    double translation[16]{
                        0.5, 0.0, 0.0, 0.5,
                        0.0, 0.5, 0.0, 0.5,
                        0.0, 0.0, 0.5, 0.0,
                        0.0, 0.0, 0.0, 1.0
                    };
                    // 旋转矩阵
                    double rotation[16]{
                        0.5, 0.0, 0.0, 0.0,
                        0.0, 0.5, 0.0, 0.0,
                        0.0, 0.0, 0.5, 0.0,
                        0.0, 0.0, 0.0, 1.0
                    };

                    this->CurrentRenderer->GetActiveCamera()->SetModelTransformMatrix(scale);
                }

                //---------------------------------------------------------------------------------------------
                this->CurrentRenderer->ResetCameraClippingRange();
                this->Interactor->Render();
            }
        }

        void OnMiddleButtonUp() override
        {
            Superclass::OnMiddleButtonUp();

            if (m_actor)
            {
                if (auto mapper = m_actor->GetMapper())
                {
                    if (auto dataSet = mapper->GetInput())
                    {
                        for (size_t i = 0; i < 4; i++)
                        {
                            auto pt = dataSet->GetPoint(i);
                            std::cout << "point " << i << ": " << pt[0] << '\t' << pt[1] << '\t' << pt[2] << '\n';
                        }
                    }
                }
            }
        }

    public:
        void SetActor(const vtkSmartPointer<vtkActor>& _actor)
        {
            m_actor = _actor;
        }
    private:
        vtkSmartPointer<vtkActor> m_actor{ nullptr };
    }; // class

    vtkStandardNewMacro(MyInteractorStyle);
}

int main()
{
    // 构造actor
    std::array<float, 4 * 3> vertices{
    -1,-1,0,
     1,-1,0,
     1, 1,0,
    -1, 1,0
    };

    vtkNew<vtkActor> actor;
    vtkNew<vtkPoints> points;
    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }

    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0,1,2,3 });
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    renderer->SetBackground(.1, .2, .3);
    //renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 800);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //InteractorStyle
    vtkNew<MyInteractorStyle> style;
    iren->SetInteractorStyle(style);
    style->SetActor(actor);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST8

#ifdef TEST9

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkMatrix4x4.h>
#include <vtkPerspectiveTransform.h>
#include <vtkTransform.h>
#include <vtkRegularPolygonSource.h>
#include <vtkCoordinate.h>

#include <array>
#include <iostream>

namespace
{
    class MyInteractorStyle : public vtkInteractorStyleRubberBand3D
    {
    public:
        static MyInteractorStyle* New();
        vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleRubberBand3D);

        void OnLeftButtonUp() override
        {
            Superclass::OnLeftButtonUp();

            if (this->Interactor &&
                this->CurrentRenderer &&
                this->CurrentRenderer->GetActiveCamera())
            {
                auto camera = this->CurrentRenderer->GetActiveCamera();
                //camera->Print(std::cout);

                auto camPos = camera->GetPosition();
                auto focalPos = camera->GetFocalPoint();
                auto viewUp = camera->GetViewUp();
                std::cout << "------------------------------------------\n";
                std::cout << "focal pos: " << focalPos[0] << ',' << focalPos[1] << ',' << focalPos[2] << '\n';
                std::cout << "camera pos: " << camPos[0] << ',' << camPos[1] << ',' << camPos[2] << '\n';
                std::cout << "viewUp: " << viewUp[0] << ',' << viewUp[1] << ',' << viewUp[2] << '\n';
                std::cout << "--------------------------------------------\n";

                //auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();
                //mvMat->Print(std::cout);

                auto vMat = this->CurrentRenderer->GetActiveCamera()->GetCameraLightTransformMatrix();
                vMat->Print(std::cout);

                if (m_actor->GetUserMatrix())
                {
                    m_actor->GetUserMatrix()->Print(std::cout);
                }
            }
        }

        void OnMouseMove()override
        {
            if (this->Interactor &&
                this->CurrentRenderer &&
                this->CurrentRenderer->GetActiveCamera())
            {
                // [0] -----------------------------------------------------------------------
                // actor随着相机一起旋转
                // 还没完整实现
                if (false && this->Interaction == ROTATING)
                {
                    vtkRenderWindowInteractor* rwi = this->Interactor;

                    int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
                    int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

                    const int* size = this->CurrentRenderer->GetRenderWindow()->GetSize();

                    double delta_elevation = -20.0 / size[1];
                    double delta_azimuth = -20.0 / size[0];

                    double rxf = dx * delta_azimuth * this->MotionFactor;
                    double ryf = dy * delta_elevation * this->MotionFactor;

                    std::cout << ryf << ',' << rxf << '\n';

                    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
                    camera->Azimuth(rxf);
                    camera->Elevation(ryf);
                    camera->OrthogonalizeViewUp();

                    if (this->AutoAdjustCameraClippingRange)
                    {
                        this->CurrentRenderer->ResetCameraClippingRange();
                    }

                    if (rwi->GetLightFollowCamera())
                    {
                        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
                    }

                    if (m_actor)
                    {
                        m_actor->RotateX(-ryf);
                        m_actor->RotateY(rxf);
                    }
                }

                Superclass::OnMouseMove();

                // [1] -----------------------------------------------------------------------
                // actor随着相机一起旋转
                // 当相机焦点是(0,0,0),viewup是(0,1,0)时，actor随着相机一起旋转
                if (false && m_actor)
                {
                    // 这两个矩阵是一样的
                    //auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();
                    auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetViewTransformMatrix();

                    vtkNew<vtkMatrix4x4> invertModelViewMat;
                    vtkMatrix4x4::Invert(mvMat, invertModelViewMat);
                    vtkNew<vtkMatrix4x4> transposeModelViewMat;
                    vtkMatrix4x4::Transpose(mvMat, transposeModelViewMat);

                    // 看到的actor始终都是最开始的状态
                    m_actor->SetUserMatrix(transposeModelViewMat);

                    this->Interactor->Render();
                }

                // [2] -----------------------------------------------------------------------
                // actor随着相机一起旋转
                // 相机焦点可以是任意值，但是viewup必须是(0,1,0)
                if (false && m_actor)
                {
                    auto fp = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();
                    auto viewUp = this->CurrentRenderer->GetActiveCamera()->GetViewUp();
                    auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();

                    vtkNew<vtkMatrix4x4> mat;
                    for (size_t i = 0; i < 4; i++)
                    {
                        for (size_t j = 0; j < 4; j++)
                        {
                            mat->SetElement(i, j, mvMat->GetElement(i, j));
                        }
                    }

                    // 直接对mvMat调用SetElement会报错
                    // 将焦点移动到原点(0,0,0)
                    mat->SetElement(0, 3, 0);
                    mat->SetElement(1, 3, 0);
                    mat->SetElement(2, 3, 0);

                    // 转置矩阵
                    vtkNew<vtkMatrix4x4> transposeModelViewMat;
                    vtkMatrix4x4::Transpose(mat, transposeModelViewMat);

                    m_actor->SetUserMatrix(transposeModelViewMat);

                    this->Interactor->Render();
                }

                // [3] -----------------------------------------------------------------------
                // actor随着相机一起旋转
                // 相机焦点可以是任意值
                if (false && m_actor)
                {
                    auto fp = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();
                    auto viewUp = this->CurrentRenderer->GetActiveCamera()->GetViewUp();
                    auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();

                    vtkNew<vtkMatrix4x4> mat;
                    for (size_t i = 0; i < 4; i++)
                    {
                        for (size_t j = 0; j < 4; j++)
                        {
                            mat->SetElement(i, j, mvMat->GetElement(i, j));
                        }
                    }

                    // 直接对mvMat调用SetElement会报错
                    // 将焦点移动到原点(0,0,0)
                    mat->SetElement(0, 3, 0);
                    mat->SetElement(1, 3, 0);
                    mat->SetElement(2, 3, 0);

                    //mat->SetElement(1, 0, viewUp[0]);
                    //mat->SetElement(1, 1, viewUp[1]);
                    //mat->SetElement(1, 2, viewUp[2]);

                    // 转置矩阵
                    vtkNew<vtkMatrix4x4> transposeModelViewMat;
                    vtkMatrix4x4::Transpose(mat, transposeModelViewMat);

                    m_actor->SetUserMatrix(transposeModelViewMat);
                    //m_actor->RotateZ(45); // viewup为(1,1,0)

                    this->Interactor->Render();
                }

                // [4] -----------------------------------------------------------------------
                // actor随着相机一起旋转
                // 相机焦点可以是任意值
                if (false && m_actor)
                {
                    auto fp = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();
                    auto viewUp = this->CurrentRenderer->GetActiveCamera()->GetViewUp();
                    auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();
                    auto pMat = this->CurrentRenderer->GetActiveCamera()->GetProjectionTransformMatrix(this->CurrentRenderer);

                    vtkNew<vtkMatrix4x4> mvMat1;
                    mvMat1->DeepCopy(mvMat);

                    // 直接对mvMat调用SetElement会报错
                    // 将焦点移动到原点(0,0,0)
                    mvMat1->SetElement(0, 3, 0);
                    mvMat1->SetElement(1, 3, 0);
                    mvMat1->SetElement(2, 3, 0);

                    //mvMat1->SetElement(1, 0, viewUp[0]);
                    //mvMat1->SetElement(1, 1, viewUp[1]);
                    //mvMat1->SetElement(1, 2, viewUp[2]);

                    vtkNew<vtkMatrix4x4> pMat1;
                    pMat1->DeepCopy(pMat);

                    // 转置矩阵
                    mvMat1->Transpose();
                    //pMat1->Transpose();

                    vtkNew<vtkMatrix4x4> outMat;
                    vtkMatrix4x4::Multiply4x4(pMat1, mvMat1, outMat);

                    m_actor->SetUserMatrix(outMat);
                    //m_actor->RotateZ(45); // viewup为(1,1,0)

                    this->Interactor->Render();
                }

                // [5] -----------------------------------------------------------------------
                // actor随着相机一起旋转
                // 相机焦点可以是任意值
                if (true && m_actor)
                {
                    auto fp = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();
                    auto cMat = this->CurrentRenderer->GetActiveCamera()->GetCameraLightTransformMatrix();

                    auto m00 = cMat->GetElement(0, 0);
                    auto m10 = cMat->GetElement(0, 1);
                    auto m20 = cMat->GetElement(0, 2);
                    auto m30 = cMat->GetElement(0, 3);

                    auto m01 = cMat->GetElement(1, 0);
                    auto m11 = cMat->GetElement(1, 1);
                    auto m21 = cMat->GetElement(1, 2);
                    auto m31 = cMat->GetElement(1, 3);

                    auto m02 = cMat->GetElement(2, 0);
                    auto m12 = cMat->GetElement(2, 1);
                    auto m22 = cMat->GetElement(2, 2);
                    auto m32 = cMat->GetElement(2, 3);

                    auto l0 = std::hypot(m00, m01, m02);
                    auto l1 = std::hypot(m10, m11, m12);
                    auto l2 = std::hypot(m20, m21, m22);

                    // 旋转矩阵
                    vtkNew<vtkMatrix4x4> rotateMat;
                    rotateMat->SetElement(0, 0, m00 / l0);
                    rotateMat->SetElement(1, 0, m01 / l0);
                    rotateMat->SetElement(2, 0, m02 / l0);

                    rotateMat->SetElement(0, 1, m10 / l1);
                    rotateMat->SetElement(1, 1, m11 / l1);
                    rotateMat->SetElement(2, 1, m12 / l1);

                    rotateMat->SetElement(0, 2, m20 / l2);
                    rotateMat->SetElement(1, 2, m21 / l2);
                    rotateMat->SetElement(2, 2, m22 / l2);

                    // 缩放矩阵
                    vtkNew<vtkMatrix4x4> scaleMat;
                    //scaleMat->SetElement(0, 0, l0);
                    //scaleMat->SetElement(1, 1, l1);
                    //scaleMat->SetElement(2, 2, l2);

                    vtkNew<vtkMatrix4x4> translation1;
                    translation1->SetElement(0, 3, .5);
                    translation1->SetElement(1, 3, .5);
                    translation1->SetElement(2, 3, 0.);

                    vtkNew<vtkMatrix4x4> translation2;
                    translation2->SetElement(0, 3, -.5);
                    translation2->SetElement(1, 3, -.5);
                    translation2->SetElement(2, 3, 0.);

                    vtkNew<vtkMatrix4x4> outMat1;
                    vtkMatrix4x4::Multiply4x4(translation1, rotateMat, outMat1);

                    vtkNew<vtkMatrix4x4> outMat;
                    vtkMatrix4x4::Multiply4x4(outMat1, translation2, outMat);

                    m_actor->SetUserMatrix(outMat);

                    this->Interactor->Render();
                }
            }
        }

    public:
        void SetActor(const vtkSmartPointer<vtkActor>& _actor)
        {
            m_actor = _actor;
        }
        void SetRegularPoly(const vtkSmartPointer<vtkRegularPolygonSource>& _poly)
        {
            m_poly = _poly;
        }
    private:
        vtkSmartPointer<vtkActor> m_actor{ nullptr };
        vtkSmartPointer<vtkRegularPolygonSource> m_poly{ nullptr };
    }; // class

    vtkStandardNewMacro(MyInteractorStyle);
}

int main()
{


    vtkNew<vtkActor> actor;
    vtkNew<vtkActor> actor2;

    {
        // 构造actor
        std::array<float, 4 * 3> vertices{
        -1,-1,0,
         1,-1,0,
         1, 1,0,
        -1, 1,0
        };
        vtkNew<vtkPoints> points;
        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
        }

        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0,1,2,3 });
        polyData->SetPoints(points);
        polyData->SetPolys(cells);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);
    }

    vtkNew<vtkRegularPolygonSource> poly;
    {
        poly->GeneratePolygonOff();
        poly->SetRadius(0.1);
        poly->SetCenter(-.5, -.5, 0);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(poly->GetOutputPort());
        actor2->SetMapper(mapper);
        actor2->SetOrigin(-.5, -.5, 0.);
        actor2->GetProperty()->SetColor(1, 1, 1);
    }

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);

    renderer->SetBackground(.1, .2, .3);
    //renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->ParallelProjectionOn();
    //renderer->GetActiveCamera()->SetViewUp(1, 1, 0);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 800);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //InteractorStyle
    vtkNew<MyInteractorStyle> style;
    iren->SetInteractorStyle(style);
    style->SetActor(actor2);
    style->SetRegularPoly(poly);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST9

#ifdef TEST10

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkMatrix4x4.h>
#include <vtkPerspectiveTransform.h>
#include <vtkTransform.h>
#include <vtkRegularPolygonSource.h>

#include <array>
#include <iostream>

namespace
{
    class MyInteractorStyle : public vtkInteractorStyleRubberBand3D
    {
    public:
        static MyInteractorStyle* New();
        vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleRubberBand3D);

        void OnLeftButtonUp() override
        {
            Superclass::OnLeftButtonUp();

            if (this->Interactor &&
                this->CurrentRenderer &&
                this->CurrentRenderer->GetActiveCamera())
            {
                auto camera = this->CurrentRenderer->GetActiveCamera();
                //camera->Print(std::cout);

                auto camPos = camera->GetPosition();
                auto focalPos = camera->GetFocalPoint();
                auto viewUp = camera->GetViewUp();
                std::cout << "------------------------------------------\n";
                std::cout << "focal pos: " << focalPos[0] << ',' << focalPos[1] << ',' << focalPos[2] << '\n';
                std::cout << "camera pos: " << camPos[0] << ',' << camPos[1] << ',' << camPos[2] << '\n';
                std::cout << "viewUp: " << viewUp[0] << ',' << viewUp[1] << ',' << viewUp[2] << '\n';
                std::cout << "--------------------------------------------\n";
                auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();
                mvMat->Print(std::cout);
            }
        }

        void OnMouseMove()override
        {
            if (this->Interactor &&
                this->CurrentRenderer &&
                this->CurrentRenderer->GetActiveCamera())
            {
                // [0] -----------------------------------------------------------------------
                // actor随着相机一起旋转
                // 还没完整实现
                if (false && this->Interaction == ROTATING)
                {
                    vtkRenderWindowInteractor* rwi = this->Interactor;

                    int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
                    int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

                    const int* size = this->CurrentRenderer->GetRenderWindow()->GetSize();

                    double delta_elevation = -20.0 / size[1];
                    double delta_azimuth = -20.0 / size[0];

                    double rxf = dx * delta_azimuth * this->MotionFactor;
                    double ryf = dy * delta_elevation * this->MotionFactor;

                    std::cout << ryf << ',' << rxf << '\n';

                    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
                    camera->Azimuth(rxf);
                    camera->Elevation(ryf);
                    camera->OrthogonalizeViewUp();

                    if (this->AutoAdjustCameraClippingRange)
                    {
                        this->CurrentRenderer->ResetCameraClippingRange();
                    }

                    if (rwi->GetLightFollowCamera())
                    {
                        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
                    }

                    if (m_actor)
                    {
                        m_actor->RotateX(-ryf);
                        m_actor->RotateY(rxf);
                    }
                }

                Superclass::OnMouseMove();

                // [1] -----------------------------------------------------------------------
                // actor随着相机一起旋转
                // 当相机焦点是(0,0,0),viewup是(0,1,0)时，actor随着相机一起旋转
                if (false && m_actor)
                {
                    // 这两个矩阵是一样的
                    //auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();
                    auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetViewTransformMatrix();

                    vtkNew<vtkMatrix4x4> invertModelViewMat;
                    vtkMatrix4x4::Invert(mvMat, invertModelViewMat);
                    vtkNew<vtkMatrix4x4> transposeModelViewMat;
                    vtkMatrix4x4::Transpose(mvMat, transposeModelViewMat);

                    // 看到的actor始终都是最开始的状态
                    m_actor->SetUserMatrix(transposeModelViewMat);

                    this->Interactor->Render();
                }

                // [2] -----------------------------------------------------------------------
                // actor随着相机一起旋转
                // 相机焦点可以是任意值，但是viewup必须是(0,1,0)
                if (false && m_actor)
                {
                    auto fp = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();
                    auto viewUp = this->CurrentRenderer->GetActiveCamera()->GetViewUp();
                    auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();

                    vtkNew<vtkMatrix4x4> mat;
                    for (size_t i = 0; i < 4; i++)
                    {
                        for (size_t j = 0; j < 4; j++)
                        {
                            mat->SetElement(i, j, mvMat->GetElement(i, j));
                        }
                    }

                    // 直接对mvMat调用SetElement会报错
                    // 将焦点移动到原点(0,0,0)
                    mat->SetElement(0, 3, 0);
                    mat->SetElement(1, 3, 0);
                    mat->SetElement(2, 3, 0);

                    // 转置矩阵
                    vtkNew<vtkMatrix4x4> transposeModelViewMat;
                    vtkMatrix4x4::Transpose(mat, transposeModelViewMat);

                    m_actor->SetUserMatrix(transposeModelViewMat);

                    this->Interactor->Render();
                }

                // [3] -----------------------------------------------------------------------
                // actor随着相机一起旋转
                // 相机焦点可以是任意值
                if (true && m_actor)
                {
                    auto fp = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();
                    auto viewUp = this->CurrentRenderer->GetActiveCamera()->GetViewUp();
                    auto mvMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();

                    vtkNew<vtkMatrix4x4> mat;
                    for (size_t i = 0; i < 4; i++)
                    {
                        for (size_t j = 0; j < 4; j++)
                        {
                            mat->SetElement(i, j, mvMat->GetElement(i, j));
                        }
                    }

                    // 直接对mvMat调用SetElement会报错
                    // 将焦点移动到原点(0,0,0)
                    mat->SetElement(0, 3, 0);
                    mat->SetElement(1, 3, 0);
                    mat->SetElement(2, 3, 0);

                    //mat->SetElement(1, 0, -viewUp[0]);
                    //mat->SetElement(1, 1, -viewUp[1]);
                    //mat->SetElement(1, 2, -viewUp[2]);

                    // 转置矩阵
                    vtkNew<vtkMatrix4x4> transposeModelViewMat;
                    vtkMatrix4x4::Transpose(mat, transposeModelViewMat);

                    m_actor->SetUserMatrix(transposeModelViewMat);
                    //m_actor->RotateZ(45); // viewup为(1,1,0)

                    this->Interactor->Render();
                }
            }
        }

    public:
        void SetActor(const vtkSmartPointer<vtkActor>& _actor)
        {
            m_actor = _actor;
        }
    private:
        vtkSmartPointer<vtkActor> m_actor{ nullptr };

    }; // class

    vtkStandardNewMacro(MyInteractorStyle);
}

int main()
{


    vtkNew<vtkActor> actor;
    vtkNew<vtkActor> actor2;

    {
        // 构造actor
        std::array<float, 4 * 3> vertices{
        -1,-1,0,
         1,-1,0,
         1, 1,0,
        -1, 1,0
        };
        vtkNew<vtkPoints> points;
        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
        }

        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0,1,2,3 });
        polyData->SetPoints(points);
        polyData->SetPolys(cells);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);
    }
    {
        vtkNew<vtkRegularPolygonSource> poly;
        poly->GeneratePolygonOff();
        poly->SetRadius(0.1);
        poly->SetCenter(-1, -1, 0);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(poly->GetOutputPort());
        actor2->SetMapper(mapper);
        actor2->GetProperty()->SetColor(1, 1, 1);
    }

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);

    renderer->SetBackground(.1, .2, .3);
    //renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->ParallelProjectionOff();
    //renderer->GetActiveCamera()->SetViewUp(1, 1, 0);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 800);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //InteractorStyle
    vtkNew<MyInteractorStyle> style;
    iren->SetInteractorStyle(style);
    style->SetActor(actor2);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST10

#ifdef TEST11

#include <vtkActor.h>
#include <vtkBillboardTextActor3D.h>
#include <vtkCallbackCommand.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>
#include <vtkInteractorStyleRubberBand3D.h>

#include <sstream>

// 底层原理：
// 使用Actor的世界坐标Position转化为屏幕坐标
// 然后使用四个屏幕坐标转化为世界坐标
// 这四个坐标就是广告文本的纹理坐标

namespace
{
    void ActorCallback(vtkObject* caller, long unsigned int vtkNotUsed(eventId),
        void* clientData, void* vtkNotUsed(callData))
    {
        auto textActor = static_cast<vtkBillboardTextActor3D*>(clientData);
        auto actor = static_cast<vtkActor*>(caller);
        std::ostringstream label;
        label << std::setprecision(3) << actor->GetPosition()[0] << ", "
            << actor->GetPosition()[1] << ", " << actor->GetPosition()[2]
            << std::endl;
        textActor->SetPosition(actor->GetPosition());
        textActor->SetInput(label.str().c_str());
    }

    class MyInteractorStyle : public vtkInteractorStyleRubberBand3D
    {
    public:
        static MyInteractorStyle* New();
        vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleRubberBand3D);

        void OnLeftButtonUp() override
        {
            Superclass::OnLeftButtonUp();

            if (auto billboardActor = vtkBillboardTextActor3D::SafeDownCast(m_actor))
            {
                std::cout << "----------------------------------------------------\n";
                double data[3]{ 0. };
                billboardActor->GetPosition(data);
                std::cout << "position: " << data[0] << '\t' << data[1] << '\t' << data[2] << '\n';

                vtkNew<vtkPropCollection> props;
                billboardActor->GetActors(props);
                auto numProps = props->GetNumberOfItems();
                std::cout << "num of props: " << numProps << '\n';

                if (auto prop = props->GetLastProp())
                {
                    if (auto actor = vtkActor::SafeDownCast(prop))
                    {
                        if (auto mapper = actor->GetMapper())
                        {
                            if (auto poly = mapper->GetInput())
                            {
                                auto numPoints = poly->GetNumberOfPoints();
                                std::cout << "num points: " << numPoints << '\n';
                                for (size_t i = 0; i < numPoints; i++)
                                {
                                    // 四个顶点会一直变化
                                    double pt[3]{ 0. };
                                    poly->GetPoint(i, pt);
                                    std::cout << "point " << i << " : " << pt[0] << '\t' << pt[0] << '\t' << pt[0] << '\n';
                                }
                            }
                        }
                    }
                }
            }
        }
    public:
        void SetActor(const vtkSmartPointer<vtkProp3D>& _actor)
        {
            m_actor = _actor;
        }
    private:
        vtkSmartPointer<vtkProp3D> m_actor{ nullptr };
    }; // class

    vtkStandardNewMacro(MyInteractorStyle);
}

int main(int, char* [])
{
    // Create a renderer
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);

    // Create a render window
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("BillboardTextActor3D");
    renderWindow->SetSize(800, 600);

    // Create an interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Create a sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(1.0);

    // Create an actor representing the origin
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetPosition(0, 0, 0);
    actor->GetProperty()->SetColor(0, 1, 0);

    // Setup the text and add it to the renderer
    vtkNew<vtkBillboardTextActor3D> textActor;
    textActor->SetInput("text");
    textActor->SetPosition(2, 2, 0);
    textActor->GetTextProperty()->SetFontSize(12);
    textActor->GetTextProperty()->SetColor(1, 0, 0);
    textActor->GetTextProperty()->SetJustificationToCentered();

    renderer->AddActor(actor);
    renderer->AddActor(textActor);

    //If you want to use a callback, do this:
    //vtkNew<vtkCallbackCommand> actorCallback;
    //actorCallback->SetCallback(ActorCallback);
    //actorCallback->SetClientData(textActor);
    //actor->AddObserver(vtkCommand::ModifiedEvent, actorCallback);

    vtkNew<MyInteractorStyle> style;
    style->SetActor(textActor);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindow->SetWindowName("BillboardTextActor3D");
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST11

#ifdef TEST12

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkFollower.h>
#include <vtkInteractorStyle.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVectorText.h>
#include <vtkRegularPolygonSource.h>
#include <vtkInteractorStyleRubberBand3D.h>

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    // Create marker
    vtkNew<vtkRegularPolygonSource> textSource;
    textSource->SetCenter(0, 0, 0);  // 标记的位置始终设置为(0,0,0)
    textSource->SetRadius(.1);
    textSource->GeneratePolygonOn();

    // Create a mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(textSource->GetOutputPort());

    // Create a subclass of vtkActor: a vtkFollower that remains facing the camera
    vtkNew<vtkFollower> follower;
    follower->SetPosition(.5, .5, .5); // vtkFollower的位置设置为actor应该在的位置，这样相机旋转时，vtkFollower的位置就可以跟着变化
    //follower->SetScale(1);
    follower->SetMapper(mapper);
    follower->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    // Create a transparent cube that does not move around to face the camera
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->SetCenter(0, 0, 0);
    cubeSource->SetXLength(1);
    cubeSource->SetYLength(1);
    cubeSource->SetZLength(1);
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(
        colors->GetColor3d("MidnightBlue").GetData());
    cubeActor->GetProperty()->SetOpacity(0.3);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("Follower");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew< vtkInteractorStyleRubberBand3D> style;
    renderWindowInteractor->SetInteractorStyle(style);
    dynamic_cast<vtkInteractorStyle*>(
        renderWindowInteractor->GetInteractorStyle())
        ->AutoAdjustCameraClippingRangeOn();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actor to the scene
    renderer->AddActor(follower);
    renderer->AddActor(cubeActor);
    renderer->SetBackground(colors->GetColor3d("LightSlateGray").GetData());
    follower->SetCamera(renderer->GetActiveCamera());

    // Render and interact
    renderWindow->Render();
    renderer->ResetCamera();
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST12

#ifdef TEST13

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkFollower.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRegularPolygonSource.h>
#include <vtkCamera.h>
#include <vtkTransform.h>

#include <iostream>

class MyFollower :
    public vtkFollower
{
public:
    static MyFollower* New();
    vtkTypeMacro(MyFollower, vtkFollower);
protected:
    void ComputeMatrix() override
    {
        // check whether or not need to rebuild the matrix
        if (this->GetMTime() > this->MatrixMTime ||
            (this->Camera && this->Camera->GetMTime() > this->MatrixMTime))
        {
            std::cout << "-------------------------------------------------\n";
            this->GetOrientation();
            this->Transform->Push();
            this->Transform->Identity();
            this->Transform->PostMultiply();

            //std::cout << "######### 111 ##########\n";
            //Transform->Print(std::cout);

            this->Transform->Translate(-this->Origin[0], -this->Origin[1], -this->Origin[2]);
            // scale
            this->Transform->Scale(this->Scale[0], this->Scale[1], this->Scale[2]);

            //std::cout << "######### 222 ##########\n";
            //Transform->Print(std::cout);

            // rotate
            this->Transform->RotateY(this->Orientation[1]);
            this->Transform->RotateX(this->Orientation[0]);
            this->Transform->RotateZ(this->Orientation[2]);

            //std::cout << "######### 333 ##########\n";
            //Transform->Print(std::cout);

            if (this->Camera)
            {
                double pos[3], vup[3], distance;
                double Rx[3], Ry[3], Rz[3];

                vtkMatrix4x4* matrix = this->InternalMatrix;
                matrix->Identity();

                // do the rotation
                // first rotate y
                this->Camera->GetPosition(pos);
                this->Camera->GetViewUp(vup);

                if (this->Camera->GetParallelProjection())
                {
                    this->Camera->GetDirectionOfProjection(Rz);
                    Rz[0] = -Rz[0];
                    Rz[1] = -Rz[1];
                    Rz[2] = -Rz[2];
                }
                else
                {
                    distance = sqrt((pos[0] - this->Position[0]) * (pos[0] - this->Position[0]) +
                        (pos[1] - this->Position[1]) * (pos[1] - this->Position[1]) +
                        (pos[2] - this->Position[2]) * (pos[2] - this->Position[2]));
                    for (int i = 0; i < 3; i++)
                    {
                        Rz[i] = (pos[i] - this->Position[i]) / distance;
                    }
                }

                // We cannot directly use the vup angle since it can be aligned with Rz:
                // vtkMath::Cross(vup,Rz,Rx);
                // vtkMath::Normalize(Rx);
                // vtkMath::Cross(Rz,Rx,Ry);

                // instead use the view right angle:
                double dop[3], vur[3];
                this->Camera->GetDirectionOfProjection(dop);

                vtkMath::Cross(dop, vup, vur);
                vtkMath::Normalize(vur);

                vtkMath::Cross(Rz, vur, Ry);
                vtkMath::Normalize(Ry);
                vtkMath::Cross(Ry, Rz, Rx);

                matrix->Element[0][0] = Rx[0];
                matrix->Element[1][0] = Rx[1];
                matrix->Element[2][0] = Rx[2];
                matrix->Element[0][1] = Ry[0];
                matrix->Element[1][1] = Ry[1];
                matrix->Element[2][1] = Ry[2];
                matrix->Element[0][2] = Rz[0];
                matrix->Element[1][2] = Rz[1];
                matrix->Element[2][2] = Rz[2];

                //--------------------------------------------------------------------
                auto cMat = this->Camera->GetCameraLightTransformMatrix();

                auto m00 = cMat->GetElement(0, 0);
                auto m10 = cMat->GetElement(0, 1);
                auto m20 = cMat->GetElement(0, 2);
                auto m30 = cMat->GetElement(0, 3);

                auto m01 = cMat->GetElement(1, 0);
                auto m11 = cMat->GetElement(1, 1);
                auto m21 = cMat->GetElement(1, 2);
                auto m31 = cMat->GetElement(1, 3);

                auto m02 = cMat->GetElement(2, 0);
                auto m12 = cMat->GetElement(2, 1);
                auto m22 = cMat->GetElement(2, 2);
                auto m32 = cMat->GetElement(2, 3);

                auto l0 = std::hypot(m00, m01, m02);
                auto l1 = std::hypot(m10, m11, m12);
                auto l2 = std::hypot(m20, m21, m22);

                // 旋转矩阵
                vtkNew<vtkMatrix4x4> rotateMat;
                rotateMat->SetElement(0, 0, m00 / l0);
                rotateMat->SetElement(1, 0, m01 / l0);
                rotateMat->SetElement(2, 0, m02 / l0);

                rotateMat->SetElement(0, 1, m10 / l1);
                rotateMat->SetElement(1, 1, m11 / l1);
                rotateMat->SetElement(2, 1, m12 / l1);

                rotateMat->SetElement(0, 2, m20 / l2);
                rotateMat->SetElement(1, 2, m21 / l2);
                rotateMat->SetElement(2, 2, m22 / l2);
                //--------------------------------------------------------------------

                matrix->Print(std::cout);
                rotateMat->Print(std::cout);

                this->Transform->Concatenate(matrix);      // vtk官方
                //this->Transform->Concatenate(rotateMat); // 自己计算得到

                //std::cout << "######### 444 ##########\n";
                //Transform->Print(std::cout);
            }

            // translate to projection reference point PRP
            // this is the camera's position blasted through
            // the current matrix
            this->Transform->Translate(this->Origin[0] + this->Position[0],
                this->Origin[1] + this->Position[1], this->Origin[2] + this->Position[2]);

            //std::cout << "######### 555 ##########\n";
            //Transform->Print(std::cout);


            // apply user defined matrix last if there is one
            if (this->UserMatrix)
            {
                this->Transform->Concatenate(this->UserMatrix);
            }

            //std::cout << "######### 666 ##########\n";
            //Transform->Print(std::cout);

            this->Transform->PreMultiply();
            this->Transform->GetMatrix(this->Matrix);
            this->MatrixMTime.Modified();
            this->Transform->Pop();

            //std::cout << "######### 777 ##########\n";
            //Transform->Print(std::cout);
        }
    }
};

vtkStandardNewMacro(MyFollower);

int main(int, char* [])
{
    // Create marker
    vtkNew<vtkRegularPolygonSource> regularSource;
    regularSource->SetCenter(0., 0., 0.);
    regularSource->SetRadius(.1);
    regularSource->GeneratePolygonOn();

    // Create a mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(regularSource->GetOutputPort());

    // Create a subclass of vtkActor: a vtkFollower that remains facing the camera
    //vtkNew<vtkFollower> follower;
    vtkNew<MyFollower> follower;
    follower->SetMapper(mapper);
    follower->SetPosition(.5, .5, .5);
    follower->GetProperty()->SetColor(1, 0, 0);

    // Create a transparent cube that does not move around to face the camera
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->SetCenter(0, 0, 0);
    cubeSource->SetXLength(1);
    cubeSource->SetYLength(1);
    cubeSource->SetZLength(1);
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);
    cubeActor->GetProperty()->SetOpacity(0.3);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("Follower");
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew< vtkInteractorStyleRubberBand3D> style;
    renderWindowInteractor->SetInteractorStyle(style);
    dynamic_cast<vtkInteractorStyle*>(
        renderWindowInteractor->GetInteractorStyle())
        ->AutoAdjustCameraClippingRangeOn();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actor to the scene
    renderer->AddActor(follower);
    renderer->AddActor(cubeActor);
    renderer->SetBackground(.1, .2, .3);
    follower->SetCamera(renderer->GetActiveCamera());

    // Render and interact
    renderer->ResetCamera();
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST13

#ifdef TEST14

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkDistanceToCamera.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkCamera.h>
#include <vtkRegularPolygonSource.h>
#include <vtkAlgorithmOutput.h>
#include <vtkPointSet.h>
#include <vtkPropPicker.h>
#include <vtkFollower.h>

#include <array>
#include <iostream>

namespace {

vtkNew<vtkFollower> actorGlyph;
vtkNew<vtkRenderer> renderer;

class MyInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyInteractorStyle* New();
    vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonUp() override
    {
        if (this->Interactor)
        {
            auto camera = this->CurrentRenderer->GetActiveCamera();
            std::cout << camera->GetParallelScale() << '\n';

            auto mousePos = this->Interactor->GetEventPosition();
            std::cout << "mouse pos: " << mousePos[0] << '\t' << mousePos[1] << '\n';

            vtkNew<vtkPropPicker> picker;
            this->Interactor->SetPicker(picker);
            if (picker->Pick(mousePos[0], mousePos[1], 0, this->CurrentRenderer) != 0)
            {
                auto actorCenter = actorGlyph->GetCenter();
                auto actorPos = actorGlyph->GetPosition();
                std::cout << "actor center: " << actorCenter[0] << '\t' << actorCenter[1] << '\t' << actorCenter[2] << '\n';
                std::cout << "actor pos: " << actorPos[0] << '\t' << actorPos[1] << '\t' << actorPos[2] << '\n';

                auto pickPos = picker->GetPickPosition();

                // 从大小不变，方向不变，始终在最上面的actor获取原始数据
                if (auto reference = vtkGlyph3D::SafeDownCast(actorGlyph->GetMapper()->GetInputConnection(0, 0)->GetProducer()))
                {
                    if (auto disToCamera = vtkDistanceToCamera::SafeDownCast(reference->GetInputConnection(0, 0)->GetProducer()))
                    {
                        auto screenSize = disToCamera->GetScreenSize();
                        std::cout << "screen Size: " << screenSize << '\n';

                        if (auto input = vtkPointSet::SafeDownCast(disToCamera->GetInput()))
                        {
                            for (size_t i = 0; i < input->GetNumberOfPoints(); i++)
                            {
                                // 多边形的原点（中心）
                                auto pt = input->GetPoint(i);
                                std::cout << "point " << i << " : " << pt[0] << '\t' << pt[1] << '\t' << pt[2] << '\n';

                                vtkNew<vtkPoints> points;
                                //points->InsertNextPoint(pickPos[0], pickPos[1], pickPos[2]);
                                points->InsertNextPoint(0, 0, 0);
                                input->SetPoints(points);

                                actorGlyph->SetPosition(pickPos[0], pickPos[1], pickPos[2]);
                            }
                        }

                    }
                }
            }
        }


        Superclass::OnLeftButtonUp();
    }
};

vtkStandardNewMacro(MyInteractorStyle);
}

int main(int, char* [])
{
    // 左下角为{30,30,0}右上角为{40,40,0}的正方形
    vtkNew<vtkActor> actorRect;
    {
        std::array<float, 4 * 3> vertices
        {
            30,30,0,
            40,30,0,
            40,40,0,
            30,40,0
        };

        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
        }

        cells->InsertNextCell({ 0,1,2,3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        actorRect->SetMapper(mapper);
        actorRect->GetProperty()->SetColor(0, 1, 0);
    }

    // 多边形的原点
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(35, 35, 0);
    vtkNew<vtkPointSet> originPoint;
    originPoint->SetPoints(points);

    vtkNew<vtkDistanceToCamera> distToCamera;
    distToCamera->SetInputData(originPoint);
    distToCamera->SetScreenSize(50);

    // 需要大小不变的多边形
    vtkNew<vtkRegularPolygonSource> source;
    source->GeneratePolygonOn();

    // 实际添加到mapper中的polyData
    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputConnection(distToCamera->GetOutputPort());
    glyph->SetSourceConnection(source->GetOutputPort());
    // 大小不变
    glyph->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera");

    vtkNew<vtkPolyDataMapper> mapperGlyph;
    mapperGlyph->SetInputConnection(glyph->GetOutputPort());
    mapperGlyph->SetScalarVisibility(false); // 不关闭时，鼠标缩放会改变球的颜色

    // 始终在最上面
    const double units0 = -1.e6;
    mapperGlyph->SetResolveCoincidentTopologyToPolygonOffset();
    mapperGlyph->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, units0);

    actorGlyph->SetMapper(mapperGlyph);
    actorGlyph->GetProperty()->SetColor(1, 0, 0);
    actorGlyph->SetCamera(renderer->GetActiveCamera());

    //---------------------------------------------------------------------------

    renderer->AddActor(actorGlyph);
    renderer->AddActor(actorRect);
    renderer->SetBackground(.1, .2, .3);
    distToCamera->SetRenderer(renderer);
    renderer->GetActiveCamera()->ParallelProjectionOff();
    //renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("DistanceToCamera");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<MyInteractorStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST14

#ifdef TEST15

#include <vtkActor2D.h>
#include <vtkCoordinate.h>
#include <vtkLineSource.h>
#include <vtkRegularPolygonSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkVectorText.h>
#include <vtkProperty2D.h>
#include <vtkProperty.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

int main()
{
    // 创建一个渲染器和窗口
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    // 创建一个立方体
    vtkNew<vtkActor> cubeActor;
    {
        vtkNew<vtkCubeSource> cube;
        vtkNew<vtkPolyDataMapper> cubeMapper;
        cubeMapper->SetInputConnection(cube->GetOutputPort());
        cubeActor->SetMapper(cubeMapper);
        cubeActor->GetProperty()->SetColor(0, 1, 0);
    }

    // 创建一个多边形
    vtkNew<vtkActor2D> regularActor;
    {
        vtkNew<vtkRegularPolygonSource> regular;
        regular->SetCenter(500, 500, 0);
        regular->SetRadius(50);
        regular->Update();
        vtkNew<vtkPolyDataMapper2D> mapper;
        mapper->SetInputConnection(regular->GetOutputPort());
        regularActor->SetMapper(mapper);
        regularActor->GetProperty()->SetColor(1, 1, 0);
    }

    // 创建一些线条
    vtkNew<vtkActor2D> lineActor;
    {
        vtkNew<vtkLineSource> lineSource;
        lineSource->SetPoint1(100.0, 100.0, 0.0);
        lineSource->SetPoint2(300.0, 300.0, 0.0);
        vtkNew<vtkPolyDataMapper2D> lineMapper;
        lineMapper->SetInputConnection(lineSource->GetOutputPort());
        lineActor->SetMapper(lineMapper);
        lineActor->GetProperty()->SetColor(0, 1, 0);
    }

    renderer->AddActor(cubeActor);

    renderer->AddActor(lineActor);
    //renderer->AddActor2D(lineActor);
    renderer->AddActor(regularActor);
    //renderer->AddActor2D(regularActor);

    // 创建交互器并启动渲染窗口
    vtkNew<vtkRenderWindowInteractor> interactor;
    vtkNew< vtkInteractorStyleRubberBand3D> style;
    interactor->SetInteractorStyle(style);
    renderWindow->SetInteractor(interactor);

    renderWindow->Render();
    interactor->Start();

    return 0;
}

#endif // TEST15

#ifdef TEST89

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkMatrix4x4.h>
#include <vtkPerspectiveTransform.h>

#include <array>
#include <iostream>

namespace
{
    class MyInteractorStyle : public vtkInteractorStyleRubberBand3D
    {
    public:
        static MyInteractorStyle* New();
        vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleRubberBand3D);

        void OnLeftButtonUp() override
        {
            Superclass::OnLeftButtonUp();

            static int index = 0;
            std::cout << "----------\t" << index++ << "\t-----------\n";

            if (this->Interactor &&
                this->CurrentRenderer &&
                this->CurrentRenderer->GetActiveCamera())
            {
                auto camera = this->CurrentRenderer->GetActiveCamera();
                //camera->Print(std::cout);

                //this->CurrentRenderer->ResetCamera();

                auto camPos = camera->GetPosition();
                auto focalPos = camera->GetFocalPoint();
                auto viewUp = camera->GetViewUp();
                auto parallelScale = camera->GetParallelScale();

                auto transformMatrix = camera->GetCameraLightTransformMatrix();
                //transformMatrix->Print(std::cout);

                //double inFocalPos[4]{ 0,0,0,1 };
                //double inCameraPos[4]{ 0,0,1,1 };
                //double inViewUp[4]{ 0,1,0,1 };

                //double outFocalPos[4]{ 0 };
                //double outCameraPos[4]{ 0 };
                //double outViewUp[4]{ 0 };

                //transformMatrix->MultiplyPoint(inFocalPos, outFocalPos);
                //transformMatrix->MultiplyPoint(inCameraPos, outCameraPos);
                //transformMatrix->MultiplyPoint(inViewUp, outViewUp);

                //std::cout << "focal pos: " << outFocalPos[0] << ',' << outFocalPos[1] << ',' << outFocalPos[2] << '\n';
                //std::cout << "camera pos: " << outCameraPos[0] << ',' << outCameraPos[1] << ',' << outCameraPos[2] << '\n';
                //std::cout << "viewUp: " << outViewUp[0] << ',' << outViewUp[1] << ',' << outViewUp[2] << '\n';
                ////std::cout << "parallel Scale: " << parallelScale << '\n';

                //std::cout << "---------------\n";

                //std::cout << "focal pos: " << focalPos[0] << ',' << focalPos[1] << ',' << focalPos[2] << '\n';
                //std::cout << "camera pos: " << camPos[0] << ',' << camPos[1] << ',' << camPos[2] << '\n';
                //std::cout << "viewUp: " << viewUp[0] << ',' << viewUp[1] << ',' << viewUp[2] << '\n';
                ////std::cout << "parallel Scale: " << parallelScale << '\n';

                //camera->SetModelTransformMatrix(transformMatrix);

                //this->Interactor->Render();


                auto viewPlaneNormal = camera->GetViewPlaneNormal();
                auto module = std::hypot(viewPlaneNormal[0], viewPlaneNormal[1], viewPlaneNormal[2]);
                std::cout << module << '\t' << viewPlaneNormal[0] << '\t' << viewPlaneNormal[1] << '\t' << viewPlaneNormal[2] << '\n';

                auto x = viewPlaneNormal[0];
                auto y = viewPlaneNormal[1];

                if (m_actor)
                {
                    m_actor->RotateX(-m_lastX);
                    m_actor->RotateY(-m_lastY);
                    m_lastX = std::asin(y) * 180 / 3.1415926;
                    m_lastY = std::asin(x) * 180 / 3.1415926;
                    m_actor->RotateY(m_lastY);
                    m_actor->RotateX(m_lastX);
                    std::cout << "angle: " << m_lastX << '\t' << m_lastY << '\n';
                    //m_actor->RotateWXYZ(viewPlaneNormal[0], viewPlaneNormal[1], viewPlaneNormal[2], 0);
                }

                this->Interactor->Render();
            }
        }

    public:
        void SetActor(const vtkSmartPointer<vtkActor>& _actor)
        {
            m_actor = _actor;
        }
    private:
        vtkSmartPointer<vtkActor> m_actor{ nullptr };
        double m_lastX{ 0. };
        double m_lastY{ 0. };

    }; // class

    vtkStandardNewMacro(MyInteractorStyle);
}

int main()
{
    // 构造actor
    std::array<float, 4 * 3> vertices{
    -1,-1,0,
     1,-1,0,
     1, 1,0,
    -1, 1,0
    };

    vtkNew<vtkActor> actor;
    vtkNew<vtkPoints> points;
    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }

    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0,1,2,3 });
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    renderer->SetBackground(.1, .2, .3);
    //renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 800);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //InteractorStyle
    vtkNew<MyInteractorStyle> style;
    iren->SetInteractorStyle(style);
    style->SetActor(actor);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST8

#ifdef TEST88

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLegendScaleActor.h>
#include <vtkProperty.h>
#include <vtkButtonWidget.h>
#include <vtkObjectFactory.h>

#include <vtkInteractorStyleRubberBand3D.h>

#include <vector>
#include <iostream>

namespace
{
    // 顶点数据
    std::vector<double> vertices{
        -1.,0.,0.,
        -.5,.5,.5,
        0.,1.,1.5,

        .5,.5,.5,
        1.,0.,0.,
        .5,-.5,.5,

        0.,-1.,-1.5,
        -.5,-.5,-.5,
        0.,0.,0.
    };

    // 拓扑数据
    std::vector<long long> indices{
        0,1,8,
        1,2,3,
        8,3,4,
        8,4,5,
        7,5,6,
        7,8,5,
        0,8,7,
        1,3,8
    };
}

class CustomStyle :
    public vtkInteractorStyleRubberBand3D
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleRubberBand3D);

protected:
    void OnChar() override
    {
        if (!this->Interactor || !this->CurrentRenderer)
        {
            return;
        }

        auto camera = this->CurrentRenderer->GetActiveCamera();

        switch (this->Interactor->GetKeyCode())
        {
        case 'x':
        case 'X':
        {
            camera->Print(std::cout);
            break;
        }
        case 'Z':
        case 'z':
        {
            camera->SetPosition(0, 0, 0);
            this->Interactor->Render();

            std::cout << "set camera position\n";
            break;
        }
        case 'C':
        case 'c':
        {

            break;
        }
        default:
            break;
        }

        return Superclass::OnChar();
    }

    void OnMouseWheelForward() override
    {
        Superclass::OnMouseWheelForward();
    }
    void OnMouseWheelBackward() override
    {
        Superclass::OnMouseWheelBackward();
    }

};

vtkStandardNewMacro(CustomStyle);


vtkSmartPointer<vtkActor> GenActor1()
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i]/*/100.0*/, vertices[i + 1] /*/ 100.0*/, vertices[i + 2] /*/ 100.0*/);
    }

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        cells->InsertNextCell({ indices[i], indices[i + 1], indices[i + 2] });
    }

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    return actor;
}

vtkSmartPointer<vtkActor> GenActorPoint(double x, double y, double z)
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    points->InsertNextPoint(x, y, z);
    const vtkIdType pts = 0;
    cells->InsertNextCell(1, &pts);

    polyData->SetPoints(points);
    polyData->SetVerts(cells);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    return actor;
}

int main(int, char* [])
{
    // 比例尺
    vtkNew<vtkLegendScaleActor> scaleActor;
    scaleActor->BottomAxisVisibilityOff();//下面的比例尺不显示
    scaleActor->LeftAxisVisibilityOff();  //左边比例尺不显示
    scaleActor->RightAxisVisibilityOff();
    scaleActor->TopAxisVisibilityOff();

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(scaleActor);
    renderer->AddActor(GenActor1());
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->SetParallelProjection(true); // 平行投影

    // window
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    // interactor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // style
    vtkNew<CustomStyle> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST8

#ifdef TEST888

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLegendScaleActor.h>
#include <vtkProperty.h>
#include <vtkButtonWidget.h>
#include <vtkObjectFactory.h>

#include <vtkInteractorStyleRubberBand3D.h>

#include <vector>
#include <iostream>

namespace
{
    // 顶点数据
    std::vector<double> vertices{
        -1.,0.,0.,
        -.5,.5,.5,
        0.,1.,1.5,

        .5,.5,.5,
        1.,0.,0.,
        .5,-.5,.5,

        0.,-1.,-1.5,
        -.5,-.5,-.5,
        0.,0.,0.
    };

    // 拓扑数据
    std::vector<long long> indices{
        0,1,8,
        1,2,3,
        8,3,4,
        8,4,5,
        7,5,6,
        7,8,5,
        0,8,7,
        1,3,8
    };
}

class CustomStyle :
    public vtkInteractorStyleRubberBand3D
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleRubberBand3D);

protected:
    void OnChar() override
    {
        if (!this->Interactor || !this->CurrentRenderer)
        {
            return;
        }

        switch (this->Interactor->GetKeyCode())
        {
        case 'x':
        case 'X':
        {
            std::cout << "--------------------------------------------------\n";
            auto camera = this->CurrentRenderer->GetActiveCamera();
            auto cameraPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();
            auto cameraFocalPos = this->CurrentRenderer->GetActiveCamera()->GetFocalPoint();
            auto cameraFocalDistance = this->CurrentRenderer->GetActiveCamera()->GetFocalDistance();
            auto cameraFocalDisk = this->CurrentRenderer->GetActiveCamera()->GetFocalDisk();
            auto cameraFocalScale = this->CurrentRenderer->GetActiveCamera()->GetFocalPointScale();
            auto cameraFocalShift = this->CurrentRenderer->GetActiveCamera()->GetFocalPointShift();
            auto distance = this->CurrentRenderer->GetActiveCamera()->GetDistance();
            double eyePos[3]{ 0.0 };
            this->CurrentRenderer->GetActiveCamera()->GetEyePosition(eyePos);
            auto cameraClippingRange = this->CurrentRenderer->GetActiveCamera()->GetClippingRange();
            auto cameraParallelScale = this->CurrentRenderer->GetActiveCamera()->GetParallelScale();
            auto cameraViewAngle = this->CurrentRenderer->GetActiveCamera()->GetViewAngle();

            std::cout << "Camera Pos:\t\t" << cameraPos[0] << '\t' << cameraPos[1] << '\t' << cameraPos[2] << '\n';
            std::cout << "Camera Focal Pos:\t" << cameraFocalPos[0] << '\t' << cameraFocalPos[1] << '\t' << cameraFocalPos[2] << '\n';
            //std::cout << "Camera Focal Distance:\t" << cameraFocalDistance << '\n';
            //std::cout << "Camera Focal Disk:\t" << cameraFocalDisk << '\n';
            //std::cout << "Camera Focal Scale:\t" << cameraFocalScale << '\n';
            //std::cout << "Camera Focal Shift:\t" << cameraFocalShift[0] << '\t' << cameraFocalShift[1] << '\t' << cameraFocalShift[2] << '\n';
            std::cout << "Distance:\t" << distance << '\n';
            //std::cout << "Camera Eye Pos:\t" << eyePos[0] << '\t' << eyePos[1] << '\t' << eyePos[2] << '\n';
            //std::cout << "Camera Clipping Range:\t" << cameraClippingRange[0] << '\t' << cameraClippingRange[1] << '\n';
            std::cout << "Camera Parallel Scale:\t" << cameraParallelScale << '\n';
            std::cout << "Camera View Angle:\t" << cameraViewAngle << '\n';

            auto cameraClippingRangeEx = this->CurrentRenderer->GetClippingRangeExpansion();
            auto cameraClippingRangeExMax = this->CurrentRenderer->GetClippingRangeExpansionMaxValue();
            auto cameraClippingRangeExMin = this->CurrentRenderer->GetClippingRangeExpansionMinValue();
            //std::cout << "Clipping Range Ex\t" << cameraClippingRangeEx << '\t' << cameraClippingRangeExMax << '\t' << cameraClippingRangeExMin << '\n';

            break;
        }
        case 'Z':
        case 'z':
        {
            this->CurrentRenderer->GetActiveCamera()->SetPosition(0, 0, 0.1);
            this->Interactor->Render();
            std::cout << "set camera position\n";
            break;
        }
        case 'C':
        case 'c':
        {
            if (this->CurrentRenderer && this->Interactor)
            {
                this->CurrentRenderer->ResetCamera();
                this->Interactor->Render();
            }
            break;
        }
        default:
            break;
        }

        return Superclass::OnChar();
    }

    void OnMouseWheelForward() override
    {
        if (this->CurrentRenderer && this->CurrentRenderer->GetActiveCamera()->GetParallelScale() < 1e-3)
            return;

        /*return*/ Superclass::OnMouseWheelForward();
        //this->CurrentRenderer->ResetCameraClippingRange();

        //std::cout << this->CurrentRenderer->GetActiveCamera()->GetParallelScale() << std::endl;
    }
    void OnMouseWheelBackward() override
    {
        if (this->CurrentRenderer && this->CurrentRenderer->GetActiveCamera()->GetParallelScale() > 10)
            return;

        /*return*/ Superclass::OnMouseWheelBackward();
        //this->CurrentRenderer->ResetCameraClippingRange();

        //std::cout << this->CurrentRenderer->GetActiveCamera()->GetParallelScale() << std::endl;
    }
};

vtkStandardNewMacro(CustomStyle);

class ActiveCameraCB :
    public vtkCommand
{
public:
    static ActiveCameraCB* New();

    vtkTypeMacro(ActiveCameraCB, vtkCommand);
protected:
    virtual void Execute(vtkObject* caller, unsigned long, void* callData)
    {
        if (callData)
        {
            std::cout << "------------------------\n";
        }
        static int index = 0;
        std::cout << index++ << "\tactive camera CB\n";

        this->SetAbortFlag(2);
    }
};

vtkStandardNewMacro(ActiveCameraCB);

int main(int, char* [])
{
    // polyData 1
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i]/*/100.0*/, vertices[i + 1] /*/ 100.0*/, vertices[i + 2] /*/ 100.0*/);
    }

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        cells->InsertNextCell({ indices[i], indices[i + 1], indices[i + 2] });
    }

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    // 计算网格三个方向上的范围，即xyz的最大最小值
    auto bounds1 = polyData->GetBounds();
    auto bounds2 = points->GetBounds();

    // polyData 2
    vtkNew<vtkPolyData> polyData2;
    vtkNew<vtkPoints> points2;
    vtkNew<vtkCellArray> cells2;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points2->InsertNextPoint(vertices[i] + 3., vertices[i + 1] + 3., vertices[i + 2] + 3.);
    }

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        cells2->InsertNextCell({ indices[i], indices[i + 1], indices[i + 2] });
    }

    polyData2->SetPoints(points2);
    polyData2->SetPolys(cells2);

    // 计算网格三个方向上的范围，即xyz的最大最小值
    auto bounds1_2 = polyData2->GetBounds();
    auto bounds2_2 = points2->GetBounds();

    // actor 1
    vtkNew<vtkActor> actor;
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    auto bounds3 = mapper->GetBounds();
    auto bounds4 = actor->GetBounds();

    // actor 2
    vtkNew<vtkActor> actor2;
    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(polyData2);
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(1, 0, 0);

    auto bounds3_2 = mapper2->GetBounds();
    auto bounds4_2 = actor2->GetBounds();

    vtkNew<vtkLegendScaleActor> scale;
    scale->BottomAxisVisibilityOff();//下面的比例尺不显示
    scale->LeftAxisVisibilityOff();  //左边比例尺不显示
    scale->RightAxisVisibilityOff();
    scale->TopAxisVisibilityOff();

    //camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);    //设置相机位置
    camera->SetFocalPoint(0, 0, 0);  //设置相机焦点

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);

    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->AddActor(scale);

    // vtkRenderer获取的范围是render中所有actor的最大范围
    auto bounds5 = renderer->ComputeVisiblePropBounds();

    renderer->ResetCamera();

    //renderer->ResetCameraClippingRange(-1e30, 1e30, -1e30, 1e30, -1e30, 1e30);
    renderer->GetActiveCamera()->SetParallelProjection(true); // 平行投影

    //renderer->GetActiveCamera()->SetParallelScale(.001);

    //renderer->GetActiveCamera()->SetPosition(0, 0, 0.001);
    //renderer->ResetCameraClippingRange();

    vtkNew<ActiveCameraCB> cameraCB;
    //renderer->AddObserver(vtkCommand::ResetCameraEvent, cameraCB);
    //renderer->AddObserver(vtkCommand::ResetCameraClippingRangeEvent, cameraCB);

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);//设置window大小

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //iren->AddObserver(vtkCommand::RenderEvent, cameraCB);
    renderer->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, cameraCB);
    //renderer->GetActiveCamera()-

    vtkNew< CustomStyle> style;
    iren->SetInteractorStyle(style);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

//vtkInteractorStyleTrackballCamera* style = vtkInteractorStyleTrackballCamera::New();  //左键旋转，滚轮缩放，滚轮拖动，右键缩放
//vtkInteractorStyleTrackballActor *style = vtkInteractorStyleTrackballActor::New();    //滚轮不能缩放，其他和vtkInteractorStyleTrackballCamera一样
//vtkInteractorStyleUnicam *style = vtkInteractorStyleUnicam::New();                    //只能左键缩放平移
//vtkInteractorStyleUser* style = vtkInteractorStyleUser::New();                        //没有鼠标响应 主要用于用户自定义的操作

#endif // TEST8