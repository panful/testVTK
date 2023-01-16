/*
* 1.打印vtkCamera的信息
* 2.窗口缩放，图元不缩放
* 3.图片满屏显示，适配窗口
* 4.部分图元大小不变，位置变化
* 5.照相机矩阵变换，相机旋转，actor不旋转
* 6.前后平面裁剪，如果摄像机处于图元内部，设置裁剪平面图元仍会被裁剪，
* 88.照相机属性，获取网格的最大最小边界，无限放大缩小
*/

#define TEST7

#ifdef TEST1

// https://blog.csdn.net/liushao1031177/article/details/116903698
// https://www.cnblogs.com/ybqjymy/p/13925462.html

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

        }
        break;
        default:
            break;
        }

        return Superclass::OnChar();
    }

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
                // 相机旋转，actor不旋转
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
                    std::cout << "projection\n";
                    projection->Print(std::cout);
                    std::cout << "modelView\n";
                    modelView->Print(std::cout);
                    //std::cout << "view\n";
                    //view->Print(std::cout);


                    vtkNew<vtkMatrix4x4> invertModelViewMat;
                    vtkMatrix4x4::Invert(modelView, invertModelViewMat);
                    vtkNew<vtkMatrix4x4> transposeModelViewMat;
                    vtkMatrix4x4::Transpose(modelView, transposeModelViewMat);

                    // actor1始终都是最开始的状态，特定条件下才可以
                    actor1->SetUserMatrix(transposeModelViewMat);
                    actor2->SetUserMatrix(transposeModelViewMat);

                    this->CurrentRenderer->ResetCamera();
                    this->Interactor->Render();
                }
                // 给相机设置模型变换矩阵
                if (0)
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

                    // 设置模型变换矩阵后，模型的顶点坐标并不会改变
                    auto dataset = actor1->GetMapper()->GetInput();
                    for (size_t i = 0; i < dataset->GetNumberOfPoints(); ++i)
                    {
                        double point[3]{ 0. };
                        dataset->GetPoint(i, point);
                        std::cout << "point " << i << " : " << point[0] << ',' << point[1] << ',' << point[2] << '\n';
                    }
                }
                // 通过相机位置，求旋转矩阵
                if (0)
                {
                    static int n = 0;
                    auto originPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();

                    auto camPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();
                    double eyePos[3]{ 0.0 };
                    this->CurrentRenderer->GetActiveCamera()->GetEyePosition(eyePos);
                    std::cout << "camera pos: " << camPos[0] << ',' << camPos[1] << ',' << camPos[2] << '\n';
                    std::cout << "eye pos: " << eyePos[0] << ',' << eyePos[1] << ',' << eyePos[2] << '\n';
                    std::cout << "------------------------------------\n";
                }
                // 对单个actor进行模型变换
                if (0)
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
                        0.5, 0.0, 0.0, 0.5,
                        0.0, 0.5, 0.0, 0.5,
                        0.0, 0.0, 0.5, 0.0,
                        0.0, 0.0, 0.0, 1.0
                    };
                    // 旋转矩阵，绕y轴旋转180°
                    double alpha = 180 / 180.0 * 3.1415926;
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
                            mat->SetElement(i, j, rotation[i][j]);
                        }
                    }

                    actor1->SetUserMatrix(mat); // 使用矩阵对模型进行变换
                    actor2->RotateY(180);       // 绕y轴旋转180
                    actor1->RotateX(30);        // 绕x轴旋转30°
                    actor2->SetScale(0.5);      // xyz都缩放为原来的0.5

                    this->Interactor->Render();
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

namespace
{
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

        }
        break;
        default:
            break;
        }

        return Superclass::OnChar();
    }

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

#endif // TEST7

#ifdef TEST88

// https://blog.csdn.net/liushao1031177/article/details/116903698
// https://www.cnblogs.com/ybqjymy/p/13925462.html

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

// https://blog.csdn.net/liushao1031177/article/details/116903698
// https://www.cnblogs.com/ybqjymy/p/13925462.html

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