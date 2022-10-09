/*
1. 写ffmpeg
2. vtkOpenGLFramebufferObject
3. 图片满屏显示，适配窗口
4. 写ffmpeg
5. vtkFFMPEGVideoSource
6. vtk自带按钮动态设置窗口大小
7. 切片 vtkCutter  从多边形提取线条
8. 给图元添加标签文本 https://kitware.github.io/vtk-examples/site/Cxx/Visualization/LabelPlacementMapper/
9. 命令模式
10.2D直线 vtkGlyph3D 矢量图箭头方向，箭头起始段末端翻转
11.获取OpenGL版本  https://gitlab.kitware.com/vtk/vtk/-/blob/v9.1.0/Rendering/OpenGL2/Testing/Cxx/TestWindowBlits.cxx
12.网格中心vtkCenterOfMass
13.vtkMultiThreader  dem文件  多线程渲染
14.从输入的多个polydata获取交集并集差集vtkBooleanOperationPolyDataFilter
15.vtkOrientationMarkerWidget左键事件屏蔽对父窗口的响应
16.网格模型的特征边与封闭性检测 https://www.cnblogs.com/ybqjymy/p/14241831.html
17.按键盘p键显示物体的包围盒，边框
18.vtkLODActor 加载大型网格
19.拾取 cellpick
20.按钮 vtkButtonWidget
21.vtkFeatureEdges 封闭性检测 TEST16 标注边界的类型  https://kitware.github.io/vtk-examples/site/Cxx/Meshes/BoundaryEdges/
22.给一个没有封闭的图元加一个盖子，让它封闭 https://kitware.github.io/vtk-examples/site/Cxx/Meshes/CapClip/
23.线条生成三角面  vtkStripper https://kitware.github.io/vtk-examples/site/Cxx/Meshes/CapClip/
24.多边形转三角形 vtkTriangleFilter https://kitware.github.io/vtk-examples/site/Cxx/PolyData/PolygonalSurfaceContourLineInterpolator/
25.让一个模型始终在其他模型的上层 重叠的面旋转时会闪烁 https://www.weiy.city/2020/03/make-model-always-on-top/
26.在曲面上画线 TEST24绘制最短路径就是用的该方法 https://kitware.github.io/vtk-examples/site/Cxx/PolyData/PolygonalSurfacePointPlacer/
27 vtkContourTriangulator https://kitware.github.io/vtk-examples/site/Cxx/Modelling/ContourTriangulator/
28 vtkStripper https://kitware.github.io/vtk-examples/site/Cxx/Visualization/LabelContours/
29.自定义方法填充线框生成面，闭合线框生成三角面(一个三角面只能由三条线构成，即生成最小的所有三角面）
30.三角剖分 https://zhuanlan.zhihu.com/p/459884570
31.
32.
33. vtkCaptionWidget
34. vtkElevationFilter 沿指定方向生成Scalars https://kitware.github.io/vtk-examples/site/Cxx/Visualization/ProjectSphere/
35  拾取标记
36. 矢量图箭头大小
*/

#define TEST36

#ifdef TEST1

#include <vtkFFMPEGWriter.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkNew.h>

int main(int argc, char* argv[])
{
    vtkNew<vtkImageCanvasSource2D> source;
    source->SetScalarTypeToUnsignedChar();
    source->SetNumberOfScalarComponents(3);
    source->SetExtent(0, 100, 0, 100, 0, 0);

    vtkNew<vtkFFMPEGWriter> writer;

    //writer->SetInputData();
    writer->SetInputConnection(source->GetOutputPort());
    writer->SetFileName("test.avi");
    writer->Start();

    for (unsigned int i = 0; i < 100; i++)
    {
        std::cout << i << std::endl;

        source->SetDrawColor(0, 0, 0, 1);   // black
        source->FillBox(0, 100, 0, 100);    // clear image
        source->SetDrawColor(255, 0, 0, 1); // red
        source->FillBox(i, 20, 10, 20);
        source->Update();

        writer->Write();
    }
    writer->End();

    return EXIT_SUCCESS;
}


#endif // TEST1

#ifdef TEST2

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkNew.h"
#include "vtkOpenGLFramebufferObject.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLState.h"
#include "vtkPLYReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"

#include "vtkRegressionTestImage.h"
#include "vtkTestUtilities.h"

#include "vtkRenderWindowInteractor.h"

#include "vtkOpenGLRenderWindow.h"

#include "vtk_glew.h"

class vtkStartRenderObserver : public vtkCommand
{
public:
    static vtkStartRenderObserver* New() { return new vtkStartRenderObserver; }

    void Execute(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(event),
        void* vtkNotUsed(calldata)) override
    {
        // put something into a framebuffer
        int const* size = this->RenderWindow->GetSize();
        this->Framebuffer->Resize(size[0], size[1]);

        auto ostate = this->RenderWindow->GetState();
        ostate->PushFramebufferBindings();
        this->Framebuffer->Bind();
        this->Framebuffer->ActivateDrawBuffer(0);

        // make the left half green and initialize the
        // depth buffer to 0.7 so that some geometry gets clipped
        ostate->vtkglScissor(0, 0, size[0] / 2, size[1]);
        ostate->vtkglClearColor(0.1, 0.3, 0.2, 1.0);
        ostate->vtkglClearDepth(0.7);
        ostate->vtkglDepthMask(GL_TRUE);
        ostate->vtkglColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        ostate->vtkglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ostate->vtkglScissor(0, 0, size[0], size[1]);

        this->Framebuffer->ActivateReadBuffer(0);
        this->Framebuffer->DeactivateDrawBuffers();

        this->RenderWindow->BlitToRenderFramebuffer(0, 0, size[0] / 2, size[1], 0, 0, size[0] / 2,
            size[1], GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        this->Framebuffer->ActivateDrawBuffer(0);

        // make the right half blue and initialize the
        // depth buffer to 1.0
        ostate->vtkglScissor(size[0] / 2, 0, size[0] / 2, size[1]);
        ostate->vtkglClearColor(0.1, 0.2, 0.4, 1.0);
        ostate->vtkglClearDepth(1.0);
        ostate->vtkglDepthMask(GL_TRUE);
        ostate->vtkglColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        ostate->vtkglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ostate->vtkglScissor(0, 0, size[0], size[1]);

        this->Framebuffer->ActivateReadBuffer(0);
        this->Framebuffer->DeactivateDrawBuffers();

        this->RenderWindow->BlitToRenderFramebuffer(size[0] / 2, 0, size[0] / 2, size[1], size[0] / 2,
            0, size[0] / 2, size[1], GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        ostate->PopFramebufferBindings();
    }

    vtkOpenGLRenderWindow* RenderWindow;
    vtkOpenGLFramebufferObject* Framebuffer;

    void SetRenderWindow(vtkRenderWindow* rw)
    {
        this->RenderWindow = static_cast<vtkOpenGLRenderWindow*>(rw);
        this->Framebuffer->SetContext(this->RenderWindow);
        this->RenderWindow->GetState()->PushFramebufferBindings();
        int const* size = this->RenderWindow->GetSize();
        this->Framebuffer->PopulateFramebuffer(size[0], size[1],
            true,                 // textures
            1, VTK_UNSIGNED_CHAR, // 1 color buffer uchar
            true, 32,             // depth buffer
            0, this->RenderWindow->GetStencilCapable() ? true : false);
        this->RenderWindow->GetState()->PopFramebufferBindings();
    }

protected:
    vtkStartRenderObserver()
    {
        this->RenderWindow = nullptr;
        this->Framebuffer = vtkOpenGLFramebufferObject::New();
    }
    ~vtkStartRenderObserver() override { this->Framebuffer->Delete(); }
};

class vtkEndRenderObserver : public vtkCommand
{
public:
    static vtkEndRenderObserver* New() { return new vtkEndRenderObserver; }

    void Execute(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(event),
        void* vtkNotUsed(calldata)) override
    {
        // put something into a framebuffer
        int const* size = this->RenderWindow->GetSize();
        int qsize[2] = { size[0] / 4, size[1] / 4 };
        this->Framebuffer->Resize(qsize[0], qsize[1]);

        auto ostate = this->RenderWindow->GetState();
        ostate->PushFramebufferBindings();
        this->Framebuffer->Bind();
        this->Framebuffer->ActivateDrawBuffer(0);

        // copy the middle of the last frame rendered
        this->RenderWindow->BlitDisplayFramebuffer(0, qsize[0], qsize[1], size[0] / 2, size[1] / 2, 0,
            0, qsize[0], qsize[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);

        this->Framebuffer->DeactivateDrawBuffers();
        this->Framebuffer->ActivateReadBuffer(0);

        // draw it in lower left corner at half size
        this->RenderWindow->BlitToRenderFramebuffer(
            0, 0, qsize[0], qsize[1], 0, 0, qsize[0], qsize[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);
        ostate->PopFramebufferBindings();
    }

    vtkOpenGLRenderWindow* RenderWindow;
    vtkOpenGLFramebufferObject* Framebuffer;

    void SetRenderWindow(vtkRenderWindow* rw)
    {
        this->RenderWindow = static_cast<vtkOpenGLRenderWindow*>(rw);
        this->Framebuffer->SetContext(this->RenderWindow);
        this->RenderWindow->GetState()->PushFramebufferBindings();
        int const* size = this->RenderWindow->GetSize();
        this->Framebuffer->PopulateFramebuffer(size[0] / 4, size[1] / 4,
            true,                 // textures
            1, VTK_UNSIGNED_CHAR, // 1 color buffer uchar
            true, 32,             // depth buffer
            0, this->RenderWindow->GetStencilCapable() ? true : false);
        this->RenderWindow->GetState()->PopFramebufferBindings();
    }

protected:
    vtkEndRenderObserver()
    {
        this->RenderWindow = nullptr;
        this->Framebuffer = vtkOpenGLFramebufferObject::New();
    }
    ~vtkEndRenderObserver() override { this->Framebuffer->Delete(); }
};

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    vtkNew<vtkActor> actor;
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkPolyDataMapper> mapper;
    renderer->SetBackground(0.0, 0.0, 0.0);
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(400, 400);
    renderWindow->AddRenderer(renderer);
    renderer->AddActor(actor);
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renderWindow);

    if (!renderWindow->SupportsOpenGL())
    {
        cerr << "The platform does not support OpenGL as required\n";
        cerr << vtkOpenGLRenderWindow::SafeDownCast(renderWindow)->GetOpenGLSupportMessage();
        cerr << renderWindow->ReportCapabilities();
        return 1;
    }

    std::string fileName = "C:\\Users\\yangpan\\source\\repos\\SolutionCMake\\target\\04vtk\\resource\\Bearded guy.ply";
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();

    mapper->SetInputConnection(reader->GetOutputPort());
    actor->SetMapper(mapper);
    actor->GetProperty()->SetAmbientColor(0.2, 0.2, 1.0);
    actor->GetProperty()->SetDiffuseColor(1.0, 0.65, 0.7);
    actor->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
    actor->GetProperty()->SetSpecular(0.5);
    actor->GetProperty()->SetDiffuse(0.7);
    actor->GetProperty()->SetAmbient(0.5);
    actor->GetProperty()->SetSpecularPower(20.0);
    actor->GetProperty()->SetOpacity(1.0);

    renderWindow->SetMultiSamples(8);

    renderer->GetActiveCamera()->SetPosition(0, 0, 1);
    renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
    renderer->ResetCamera();

    // must render once to create context etc
    renderWindow->Render();

    int major, minor;
    vtkOpenGLRenderWindow::SafeDownCast(renderWindow)->GetOpenGLVersion(major, minor);
    cerr << "opengl version " << major << "." << minor << "\n";

    vtkNew<vtkStartRenderObserver> startObserver;
    startObserver->SetRenderWindow(renderWindow);
    renderWindow->AddObserver(vtkCommand::StartEvent, startObserver);
    vtkNew<vtkEndRenderObserver> endObserver;
    endObserver->SetRenderWindow(renderWindow);
    renderWindow->AddObserver(vtkCommand::RenderEvent, endObserver);

    renderer->GetActiveCamera()->Azimuth(80);
    renderer->ResetCameraClippingRange();
    renderWindow->Render();
    renderer->PreserveColorBufferOn();
    renderer->PreserveDepthBufferOn();
    renderer->GetActiveCamera()->Azimuth(-20);
    renderer->ResetCameraClippingRange();
    renderWindow->Render();
    renderer->GetActiveCamera()->Azimuth(-20);
    renderer->ResetCameraClippingRange();
    renderWindow->Render();

    int retVal = vtkRegressionTestImage(renderWindow);
    if (retVal == vtkRegressionTester::DO_INTERACTOR)
    {
        iren->Start();
    }

    renderWindow->GetInteractor()->Start();

    return !retVal;
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

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkFFMPEGWriter.h>

namespace {
    // Define interaction style
    class InteractorStyle : public vtkInteractorStyleRubberBandPick
    {
    public:
        static InteractorStyle* New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

        virtual void OnLeftButtonUp() override
        {
            SelectedActor->GetProperty()->SetColor(1, 0, 0);

            Superclass::OnLeftButtonUp();
        }

        void SetActor(vtkSmartPointer<vtkActor> actor)
        {
            SelectedActor = actor;
        }

    private:
        vtkSmartPointer<vtkActor> SelectedActor;
    };

    vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char* [])
{
    vtkNew<vtkFFMPEGWriter> writer;

    vtkNew<vtkCubeSource> cube;

    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    //cubeActor->SetTexture()

    //camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);//设置相机位置
    camera->SetFocalPoint(0, 0, 0);//设置相机焦点

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);//设置window大小
    vtkNew<vtkWindowToImageFilter> windowToImage;
    windowToImage->SetInput(renWin);

    //writer->SetInputData();
    writer->SetInputConnection(windowToImage->GetOutputPort());
    writer->SetFileName("test.avi");
    writer->Start();

    auto style = InteractorStyle::New();
    style->SetActor(cubeActor);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    //数据交互
    renWin->Render();
    writer->Write();
    iren->Start();
    writer->End();

    return 0;

}

#endif // TEST4

#ifdef TEST5

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestFFMPEGVideoSource.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkNew.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTexture.h"

#include "vtkRegressionTestImage.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTestUtilities.h"

#include "vtkLookupTable.h"

#include "vtkFFMPEGVideoSource.h"

int main(int argc, char* argv[])
{
    vtkNew<vtkActor> actor;
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkPolyDataMapper> mapper;
    renderer->SetBackground(0.2, 0.3, 0.4);
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(300, 300);
    renderWindow->AddRenderer(renderer);
    renderer->AddActor(actor);
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renderWindow);

    const char* fileName = "C:\\Users\\yangpan\\source\\repos\\SolutionCMake\\target\\Qt_Window_FFMPEG.avi";

    vtkNew<vtkFFMPEGVideoSource> video;
    video->SetFileName(fileName);
    delete[] fileName;

    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(video->GetOutputPort());
    actor->SetTexture(texture);

    vtkNew<vtkPlaneSource> plane;
    mapper->SetInputConnection(plane->GetOutputPort());
    actor->SetMapper(mapper);

    video->Initialize();
    int fsize[3];
    video->GetFrameSize(fsize);
    plane->SetOrigin(0, 0, 0);
    plane->SetPoint1(fsize[0], 0, 0);
    plane->SetPoint2(0, fsize[1], 0);
    renderWindow->Render();

    for (int i = 0; i < 10; ++i)
    {
        video->Grab();
        renderWindow->Render();
    }

    int retVal = vtkRegressionTestImage(renderWindow);
    if (retVal == vtkRegressionTester::DO_INTERACTOR)
    {
        iren->Start();
    }

    return !retVal;
}


#endif // TEST5

#ifdef TEST6

#include <vtkActor.h>
#include <vtkButtonWidget.h>
#include <vtkCommand.h>
#include <vtkCoordinate.h>
#include <vtkEllipticalButtonSource.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DButtonRepresentation.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSuperquadricSource.h>
#include <vtkTexture.h>

// Callback for the interaction
namespace {

    class vtkButtonCallback : public vtkCommand
    {
    public:
        static vtkButtonCallback* New()
        {
            return new vtkButtonCallback;
        }

        virtual void Execute(vtkObject* caller, unsigned long, void*)
        {
            auto buttonWidget = reinterpret_cast<vtkButtonWidget*>(caller);
            auto rep = reinterpret_cast<vtkProp3DButtonRepresentation*>(
                buttonWidget->GetRepresentation());
            int state = rep->GetState();
            this->Actor->GetProperty()->SetColor(
                reinterpret_cast<vtkActor*>(rep->GetButtonProp(state))
                ->GetProperty()
                ->GetColor());
            std::cout << "State: " << state << "\n";

            if (m_window)
            {
                m_window->SetSize(800, 600);
            }

        }

        vtkButtonCallback() : Actor(0)
        {
        }

        void SetWindow(vtkSmartPointer<vtkRenderWindow> window)
        {
            m_window = window;
        }

        vtkActor* Actor;
        vtkSmartPointer<vtkRenderWindow> m_window{ nullptr };
    };

    vtkSmartPointer<vtkActor> CreateButtonActor(const char* textureFile);

} // namespace

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> color;

    std::string fileName = R"(resource/p1.jpg)";

    vtkNew<vtkSuperquadricSource> source;
    source->SetPhiResolution(64);
    source->SetThetaResolution(64);
    source->SetThetaRoundness(1.5);
    source->SetThickness(1.5);
    source->SetSize(2);
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(source->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(color->GetColor3d("Burlywood").GetData());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(1000, 800);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("EllipticalButton");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Create the widget and its representation
    vtkSmartPointer<vtkActor> buttonActor1 = CreateButtonActor(fileName.c_str());
    buttonActor1->GetProperty()->SetColor(color->GetColor3d("Tomato").GetData());
    vtkSmartPointer<vtkActor> buttonActor2 = CreateButtonActor(fileName.c_str());
    buttonActor2->GetProperty()->SetColor(color->GetColor3d("Banana").GetData());

    vtkNew<vtkProp3DButtonRepresentation> buttonRepresentation;
    buttonRepresentation->SetNumberOfStates(2);
    buttonRepresentation->SetButtonProp(0, buttonActor1);
    buttonRepresentation->SetButtonProp(1, buttonActor2);
    buttonRepresentation->FollowCameraOn();

    vtkNew<vtkButtonCallback> callback;
    callback->SetWindow(renderWindow);
    callback->Actor = actor;
    actor->GetProperty()->SetColor(
        reinterpret_cast<vtkActor*>(buttonRepresentation->GetButtonProp(0))
        ->GetProperty()
        ->GetColor());

    renderWindowInteractor->Initialize();

    vtkNew<vtkButtonWidget> buttonWidget;
    buttonWidget->SetInteractor(renderWindowInteractor);
    buttonWidget->SetRepresentation(buttonRepresentation);
    buttonWidget->AddObserver(vtkCommand::StateChangedEvent, callback);
    buttonWidget->SetEnabled(1);

    renderer->ResetCamera();
    renderer->Render();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace {

    vtkSmartPointer<vtkActor> CreateButtonActor(const char* textureFile)
    {
        vtkNew<vtkImageReader2Factory> readerFactory;
        vtkSmartPointer<vtkImageReader2> imageReader;
        imageReader.TakeReference(readerFactory->CreateImageReader2(textureFile));
        imageReader->SetFileName(textureFile);
        imageReader->Update();

        // Aspect ratio of image
        int dims[3];
        imageReader->GetOutput()->GetDimensions(dims);
        double aspect = static_cast<double>(dims[0]) / static_cast<double>(dims[1]);

        vtkNew<vtkTexture> texture;
        texture->SetInputConnection(imageReader->GetOutputPort());

        vtkNew<vtkEllipticalButtonSource> ellipticalButtonSource;
        ellipticalButtonSource->SetCircumferentialResolution(50);
        ellipticalButtonSource->SetShoulderResolution(10);
        ellipticalButtonSource->SetTextureResolution(10);
        ellipticalButtonSource->SetRadialRatio(1.05);
        ellipticalButtonSource->SetShoulderTextureCoordinate(0.0, 0.0);
        ellipticalButtonSource->SetTextureDimensions(dims[0], dims[1]);
        ellipticalButtonSource->SetTextureStyleToProportional();
        ellipticalButtonSource->TwoSidedOn();
        ellipticalButtonSource->SetWidth(aspect);
        ellipticalButtonSource->SetHeight(1.0);
        ellipticalButtonSource->SetDepth(.15);
        ellipticalButtonSource->SetCenter(2, 2, 0);

        ellipticalButtonSource->SetOutputPointsPrecision(
            vtkAlgorithm::SINGLE_PRECISION);

        vtkNew<vtkPolyDataMapper> buttonMapper;
        buttonMapper->SetInputConnection(ellipticalButtonSource->GetOutputPort());

        vtkNew<vtkActor> buttonActor;
        buttonActor->SetMapper(buttonMapper);
        buttonActor->SetTexture(texture);

        return buttonActor;
    }

} // namespace

#endif // TEST6

#ifdef TEST7

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkColor.h>
#include <vtkCutter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkStripper.h>

#ifdef VTK_CELL_ARRAY_V2
#include <vtkCellArrayIterator.h>
#endif // VTK_CELL_ARRAY_V2

int main(int, char* [])
{
    // Define colors for example
    vtkNew<vtkNamedColors> colors;
    vtkColor3d lineColor = colors->GetColor3d("peacock");
    vtkColor3d modelColor = colors->GetColor3d("silver");
    vtkColor3d backgroundColor = colors->GetColor3d("wheat");

    vtkNew<vtkSphereSource> modelSource;

    vtkNew<vtkPlane> plane;

    vtkNew<vtkCutter> cutter;
    cutter->SetInputConnection(modelSource->GetOutputPort());
    cutter->SetCutFunction(plane);
    cutter->GenerateValues(10, -0.5, 0.5);

    vtkNew<vtkPolyDataMapper> modelMapper;
    modelMapper->SetInputConnection(modelSource->GetOutputPort());

    vtkNew<vtkActor> model;
    model->SetMapper(modelMapper);
    model->GetProperty()->SetDiffuseColor(modelColor.GetData());
    model->GetProperty()->SetInterpolationToFlat();

    vtkNew<vtkStripper> stripper;
    stripper->SetInputConnection(cutter->GetOutputPort());
    stripper->JoinContiguousSegmentsOn(); // 连接线条

    vtkNew<vtkPolyDataMapper> linesMapper;
    linesMapper->SetInputConnection(stripper->GetOutputPort());

    vtkNew<vtkActor> lines;
    lines->SetMapper(linesMapper);
    lines->GetProperty()->SetDiffuseColor(lineColor.GetData());
    lines->GetProperty()->SetLineWidth(3.0);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;

    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(640, 480);
    renderWindow->SetWindowName("ExtractPolyLinesFromPolyData");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    // Add the actors to the renderer
    renderer->AddActor(model);
    renderer->AddActor(lines);
    renderer->SetBackground(backgroundColor.GetData());
    renderer->GetActiveCamera()->Azimuth(-45);
    renderer->GetActiveCamera()->Elevation(-22.5);
    renderer->ResetCamera();

    // This starts the event loop and as a side effect causes an initial
    // render.
    renderWindow->Render();

    interactor->Start();

    // Extract the lines from the polydata
    vtkIdType numberOfLines = cutter->GetOutput()->GetNumberOfLines();

    std::cout << "-----------Lines without using vtkStripper" << std::endl;
    std::cout << "There are " << numberOfLines << " lines in the polydata"
        << std::endl;

    numberOfLines = stripper->GetOutput()->GetNumberOfLines();
    vtkPoints* points = stripper->GetOutput()->GetPoints();
    vtkCellArray* cells = stripper->GetOutput()->GetLines();

    std::cout << "-----------Lines using vtkStripper" << std::endl;
    std::cout << "There are " << numberOfLines << " lines in the polydata"
        << std::endl;

#ifdef VTK_CELL_ARRAY_V2

    // Newer versions of vtkCellArray prefer local iterators:
    auto cellIter = vtk::TakeSmartPointer(cells->NewIterator());
    for (cellIter->GoToFirstCell(); !cellIter->IsDoneWithTraversal();
        cellIter->GoToNextCell())
    {
        std::cout << "Line " << cellIter->GetCurrentCellId() << ":\n";

        vtkIdList* cell = cellIter->GetCurrentCell();
        for (vtkIdType i = 0; i < cell->GetNumberOfIds(); ++i)
        {
            double point[3];
            points->GetPoint(cell->GetId(i), point);
            std::cout << "\t(" << point[0] << ", " << point[1] << ", " << point[2]
                << ")" << std::endl;
        }
    }

#else // VTK_CELL_ARRAY_V2

    // Older implementations of vtkCellArray use internal iterator APIs (not
    // thread safe):
    vtkIdType* indices;
    vtkIdType numberOfPoints;
    unsigned int lineCount = 0;
    for (cells->InitTraversal(); cells->GetNextCell(numberOfPoints, indices);
        lineCount++)
    {
        std::cout << "Line " << lineCount << ": " << std::endl;
        for (vtkIdType i = 0; i < numberOfPoints; i++)
        {
            double point[3];
            points->GetPoint(indices[i], point);
            std::cout << "\t(" << point[0] << ", " << point[1] << ", " << point[2]
                << ")" << std::endl;
        }
    }

#endif // VTK_CELL_ARRAY_V2

    return EXIT_SUCCESS;
}


#endif // TEST7

#ifdef TEST8

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkIntArray.h>
#include <vtkLabelPlacementMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkStringArray.h>
#include <vtkTextProperty.h>

namespace {
    /**
     * Convert points to glyphs.
     *
     * @param points - The points to glyph
     * @param scale - The scale, used to determine the size of the glyph
     * representing the point, expressed as a fraction of the largest side of the
     * bounding box surrounding the points. e.g. 0.05
     *
     * @return The actor.
     */
    vtkSmartPointer<vtkActor> PointToGlyph(vtkPoints* points, double const& scale);

} // namespace
int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    // Create a point set.
    vtkNew<vtkPointSource> pointSource;
    pointSource->SetNumberOfPoints(6);
    pointSource->Update();

    // Add label array.
    vtkNew<vtkStringArray> labels;
    labels->SetNumberOfValues(6);
    labels->SetName("labels");
    labels->SetValue(0, "Priority 10");
    labels->SetValue(1, "Priority 7");
    labels->SetValue(2, "Priority 6");
    labels->SetValue(3, "Priority 4");
    labels->SetValue(4, "Priority 4");
    labels->SetValue(5, "Priority 4");
    pointSource->GetOutput()->GetPointData()->AddArray(labels);

    // Add priority array.
    vtkNew<vtkIntArray> sizes;
    sizes->SetNumberOfValues(6);
    sizes->SetName("sizes");
    sizes->SetValue(0, 10);
    sizes->SetValue(1, 7);
    sizes->SetValue(2, 6);
    sizes->SetValue(3, 4);
    sizes->SetValue(4, 4);
    sizes->SetValue(5, 4);
    pointSource->GetOutput()->GetPointData()->AddArray(sizes);

    // Create a mapper and actor for the points.
    vtkNew<vtkPolyDataMapper> pointMapper;
    pointMapper->SetInputConnection(pointSource->GetOutputPort());

    vtkNew<vtkActor> pointActor;
    pointActor->SetMapper(pointMapper);

    // Map the points to spheres
    auto sphereActor = PointToGlyph(pointSource->GetOutput()->GetPoints(), 0.05);
    sphereActor->GetProperty()->SetColor(
        colors->GetColor3d("MistyRose").GetData());

    // Generate the label hierarchy.
    vtkNew<vtkPointSetToLabelHierarchy> pointSetToLabelHierarchyFilter;
    pointSetToLabelHierarchyFilter->SetInputConnection(
        pointSource->GetOutputPort());
    pointSetToLabelHierarchyFilter->SetLabelArrayName("labels");
    pointSetToLabelHierarchyFilter->SetPriorityArrayName("sizes");
    pointSetToLabelHierarchyFilter->Update();

    // Create a mapper and actor for the labels.
    vtkNew<vtkLabelPlacementMapper> labelMapper;
    labelMapper->SetInputConnection(
        pointSetToLabelHierarchyFilter->GetOutputPort());
    vtkNew<vtkActor2D> labelActor;
    labelActor->SetMapper(labelMapper);
    // labelActor->GetProperty()->SetColor(
    //    colors->GetColor3d("Yellow").GetData());

    // Create a renderer, render window, and interactor.
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("LabelPlacementMapper");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actors to the scene.
    renderer->AddActor(pointActor);
    renderer->AddActor(sphereActor);
    renderer->AddActor(labelActor);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // Render and interact.
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace {

    vtkSmartPointer<vtkActor> PointToGlyph(vtkPoints* points, double const& scale)
    {
        auto bounds = points->GetBounds();
        double maxLen = 0;
        for (int i = 1; i < 3; ++i)
        {
            maxLen = std::max(bounds[i + 1] - bounds[i], maxLen);
        }

        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->SetRadius(scale * maxLen);

        vtkNew<vtkPolyData> pd;
        pd->SetPoints(points);

        vtkNew<vtkGlyph3DMapper> mapper;
        mapper->SetInputData(pd);
        mapper->SetSourceConnection(sphereSource->GetOutputPort());
        mapper->ScalarVisibilityOff();
        mapper->ScalingOff();

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        return actor;
    }

} // namespace

#endif // TEST8

#ifdef TEST9

#include <iostream>
#include <string>
#include <queue>
#include <thread>

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>

class Viewer
{
public:
    Viewer()
    {
        vtkNew<vtkCubeSource> cube;

        // 如果要建立管道连接，请使用SetInputConnection
        // 如果要处理独立数据集，请使用SetInputData
        //mapper
        vtkNew<vtkPolyDataMapper> cubeMapper;
        cubeMapper->SetInputConnection(cube->GetOutputPort());

        //actor
        vtkNew<vtkActor> cubeActor;
        cubeActor->SetMapper(cubeMapper);
        //cubeActor->SetTexture()

        //camera
        vtkNew<vtkCamera> camera;
        camera->SetPosition(1, 1, 1);//设置相机位置
        camera->SetFocalPoint(0, 0, 0);//设置相机焦点

        //renderer
        m_renderer = vtkRenderer::New();
        m_renderer->AddActor(cubeActor);
        m_renderer->SetActiveCamera(camera);
        m_renderer->ResetCamera();

        //RenderWindow
        m_renWin = vtkRenderWindow::New();
        m_renWin->AddRenderer(m_renderer);
        m_renWin->SetSize(600, 600);//设置window大小

        //RenderWindowInteractor
        vtkNew<vtkRenderWindowInteractor> iren;
        iren->SetRenderWindow(m_renWin);

        //数据交互
        m_renWin->Render();
    }

private:
    vtkRenderWindow* m_renWin;
    vtkRenderer* m_renderer;
};

class Command
{
public:
    virtual ~Command() = default;
    virtual void Execute() const = 0;
};

class SetIntCommand : public Command {
private:
    int m_int;

public:
    explicit SetIntCommand(int a) :
        m_int(a)
    {
    }

    void Execute() const override
    {
        std::cout << "第一个命令\n";
    }
};

class SetVoidCommand : public Command
{
public:
    SetVoidCommand()
    {
    }

    void Execute() const override
    {
        std::cout << "第二个命令\n";
    }
};

class Invoker {
public:
    Invoker()
    {
        std::thread t(&Invoker::run, this);
        t.detach();
    }

    void run()
    {
        while (true)
        {
            //std::cout << m_command.size() << " === \n";

            if (m_command.empty())
                continue;

            m_command.front()->Execute();
            m_command.pop();
        }
    }

    void AddCommand(Command* command)
    {
        if (dynamic_cast<SetIntCommand*>(command))
            std::cout << "success\n";

        m_command.emplace(command);
    }

    template<typename ...Args>
    void AddCommand(int token, Args...args)
    {
        //Command* command = nullptr;
        //if (token == 1)
        //{
        //    command = new SetIntCommand(args...);
        //}
        //else if (token == 2)
        //{
        //    command = new SetVoidCommand(args...);
        //}

        //if (command)
        //{
        //    m_command.emplace(command);
        //}
    }

    // 起一个线程，一直循环遍历 command
    std::queue<Command*> m_command;

};

class Receiver {
public:
    void SetInt(int n)
    {

    }
};

// 客户端代码可以用任何命令参数化调用程序。
int main()
{
    //Invoker* invoker = new Invoker();
    //invoker->AddCommand(new SetIntCommand(111));
    //invoker->AddCommand(new SetVoidCommand());

    ////invoker->AddCommand(1, 111);
    ////invoker->AddCommand(2);

    Viewer* viewer = new Viewer();


    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "   *** end ***\n";
    return 0;
}
#endif // TEST9

#ifdef TEST10

// 01_02 TEST17

#include <vtkArrowSource.h>

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkVector.h>
#include <vtkCleanPolyData.h>
#include <vtkAlgorithmOutput.h>
#include <vtkPolyDataNormals.h>
#include <vtkCellData.h>
#include <vtkCellCenters.h>
#include <vtkTransform.h>

#include <array>

int main(int, char* [])
{
    // Points
    vtkNew<vtkPoints> points1;
    points1->InsertNextPoint(0.0, 0.0, 0.0);
    points1->InsertNextPoint(0.0, 1.0, 0.0);
    points1->InsertNextPoint(0.0, 2.0, 0.0);
    points1->InsertNextPoint(0.0, 3.0, 0.0);

    vtkNew<vtkPoints> points2;
    points2->InsertNextPoint(2.0, 0.0, 0.0);
    points2->InsertNextPoint(2.0, 1.0, 0.0);
    points2->InsertNextPoint(2.0, 2.0, 0.0);
    points2->InsertNextPoint(2.0, 3.0, 0.0);

    vtkNew<vtkPoints> points3;
    points3->InsertNextPoint(4.0, 3.0, 0.0);
    points3->InsertNextPoint(4.0, 2.0, 0.0);
    points3->InsertNextPoint(4.0, 1.0, 0.0);
    points3->InsertNextPoint(4.0, 0.0, 0.0);

    //vtkNew<vtkPoints> points4;
    //points4->InsertNextPoint(0.0, 4.0, 0.0);
    //points4->InsertNextPoint(1.0, 5.0, 0.0);
    //points4->InsertNextPoint(2.0, 6.0, 0.0);
    //points4->InsertNextPoint(3.0, 7.0, 0.0);

    vtkNew<vtkPoints> points4;
    points4->InsertNextPoint(6.0, 3.0, 0.0);
    points4->InsertNextPoint(6.0, 2.0, 0.0);
    points4->InsertNextPoint(6.0, 1.0, 0.0);
    points4->InsertNextPoint(6.0, 0.0, 0.0);

    // Cell : shared index data
    vtkNew<vtkCellArray> lines;
    lines->InsertNextCell({ 0,1 });
    lines->InsertNextCell({ 1,2 });
    lines->InsertNextCell({ 2,3 });

    // PolyData Mapper Actor
    vtkNew<vtkPolyData> poly1;
    poly1->SetPoints(points1);
    poly1->SetLines(lines);
    vtkNew<vtkPolyDataMapper> mapper1;
    mapper1->SetInputData(poly1);
    vtkNew<vtkActor> actor1;
    actor1->SetMapper(mapper1);

    vtkNew<vtkPolyData> poly2;
    poly2->SetPoints(points2);
    poly2->SetLines(lines);
    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(poly2);
    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);

    vtkNew<vtkPolyData> poly3;
    poly3->SetPoints(points3);
    poly3->SetLines(lines);
    vtkNew<vtkPolyDataMapper> mapper3;
    mapper3->SetInputData(poly3);
    vtkNew<vtkActor> actor3;
    actor3->SetMapper(mapper3);

    vtkNew<vtkPolyData> poly4;
    poly4->SetPoints(points4);
    poly4->SetLines(lines);
    vtkNew<vtkPolyDataMapper> mapper4;
    mapper4->SetInputData(poly4);
    vtkNew<vtkActor> actor4;
    actor4->SetMapper(mapper4);

    // Arrow
    vtkNew<vtkActor> glyphActor1;
    {
        vtkNew<vtkFloatArray> centerVectors;
        centerVectors->SetNumberOfComponents(3);

        double dir = -1.0;

        for (size_t i = 1; i < static_cast<size_t>(poly1->GetNumberOfPoints()); ++i)
        {
            std::array<double, 2>  startPoint = { poly1->GetPoint(i - 1)[0],poly1->GetPoint(i - 1)[1] };
            std::array<double, 2>  endPoint = { poly1->GetPoint(i)[0],poly1->GetPoint(i)[1] };

            //线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0],endPoint[1] - startPoint[1] };

            // 如果平行于y轴
            if (std::abs(lineVector[0] - 0.0) < 1e-6 && std::abs(lineVector[1] - 0.0) > 1e-6)
            {
                centerVectors->InsertNextTuple3(dir, 0.0, 0.0);
                continue;
            }

            // 如果平行于x轴
            if (std::abs(lineVector[0] - 0.0) > 1e-6 && std::abs(lineVector[1] - 0.0) < 1e-6)
            {
                centerVectors->InsertNextTuple3(0.0, dir, 0.0);
                continue;
            }

            centerVectors->InsertNextTuple3(dir, -dir * (lineVector[0] / lineVector[1]), 0.0);
        }

        vtkNew<vtkCellCenters> cellCenters;
        cellCenters->SetInputData(poly1);
        cellCenters->Update(); // 必须在调用SetVectors之前Update

        cellCenters->GetOutput()->GetPointData()->SetVectors(centerVectors);
        cellCenters->Update();

        vtkNew<vtkArrowSource> arrow;
        arrow->Update();

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetInputData(cellCenters->GetOutput());
        glyph->SetSourceConnection(arrow->GetOutputPort());
        glyph->SetVectorModeToUseVector();
        glyph->Update();

        vtkNew<vtkPolyDataMapper> glyphMapper;
        glyphMapper->SetInputConnection(glyph->GetOutputPort());
        glyphActor1->SetMapper(glyphMapper);
    }

    vtkNew<vtkActor> glyphActor2;
    {
        vtkNew<vtkFloatArray> centerVectors;
        centerVectors->SetNumberOfComponents(3);

        double dir = 1.0;

        for (size_t i = 1; i < static_cast<size_t>(poly2->GetNumberOfPoints()); ++i)
        {
            std::array<double, 2>  startPoint = { poly2->GetPoint(i - 1)[0],poly2->GetPoint(i - 1)[1] };
            std::array<double, 2>  endPoint = { poly2->GetPoint(i)[0],poly2->GetPoint(i)[1] };

            //线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0],endPoint[1] - startPoint[1] };

            // 如果平行于y轴
            if (std::abs(lineVector[0] - 0.0) < 1e-6 && std::abs(lineVector[1] - 0.0) > 1e-6)
            {
                centerVectors->InsertNextTuple3(dir, 0.0, 0.0);
                continue;
            }

            // 如果平行于x轴
            if (std::abs(lineVector[0] - 0.0) > 1e-6 && std::abs(lineVector[1] - 0.0) < 1e-6)
            {
                centerVectors->InsertNextTuple3(0.0, dir, 0.0);
                continue;
            }

            centerVectors->InsertNextTuple3(dir, -dir * (lineVector[0] / lineVector[1]), 0.0);
        }

        vtkNew<vtkCellCenters> cellCenters;
        cellCenters->SetInputData(poly2);
        cellCenters->Update(); // 必须在调用SetVectors之前Update
        cellCenters->GetOutput()->GetPointData()->SetVectors(centerVectors);

        vtkNew<vtkArrowSource> arrow;
        arrow->SetInvert(true);  // 将箭头的起始端和末端翻转
        arrow->Update();

        vtkNew<vtkTransform> transform;
        transform->RotateZ(180);
        transform->Update();

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetInputConnection(cellCenters->GetOutputPort());
        glyph->SetSourceData(arrow->GetOutput());
        glyph->SetVectorModeToUseVector();
        //glyph->SetSourceTransform(transform);
        glyph->Update();

        vtkNew<vtkPolyDataMapper> glyphMapper;
        glyphMapper->SetInputConnection(glyph->GetOutputPort());
        glyphActor2->SetMapper(glyphMapper);
    }

    vtkNew<vtkActor> glyphActor3;
    {
        vtkNew<vtkFloatArray> centerVectors;
        centerVectors->SetNumberOfComponents(3);

        double dir = -1.0;

        for (size_t i = 1; i < static_cast<size_t>(poly3->GetNumberOfPoints()); ++i)
        {
            std::array<double, 2>  startPoint = { poly3->GetPoint(i - 1)[0],poly3->GetPoint(i - 1)[1] };
            std::array<double, 2>  endPoint = { poly3->GetPoint(i)[0],poly3->GetPoint(i)[1] };

            //线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0],endPoint[1] - startPoint[1] };

            // 如果平行于y轴
            if (std::abs(lineVector[0] - 0.0) < 1e-6 && std::abs(lineVector[1] - 0.0) > 1e-6)
            {
                centerVectors->InsertNextTuple3(dir, 0.0, 0.0);
                continue;
            }

            // 如果平行于x轴
            if (std::abs(lineVector[0] - 0.0) > 1e-6 && std::abs(lineVector[1] - 0.0) < 1e-6)
            {
                centerVectors->InsertNextTuple3(0.0, dir, 0.0);
                continue;
            }

            centerVectors->InsertNextTuple3(dir, -dir * (lineVector[0] / lineVector[1]), 0.0);
        }

        vtkNew<vtkCellCenters> cellCenters;
        cellCenters->SetInputData(poly3);
        cellCenters->Update(); // 必须在调用SetVectors之前Update
        cellCenters->GetOutput()->GetPointData()->SetVectors(centerVectors);

        vtkNew<vtkArrowSource> arrow;
        arrow->Update();

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetInputConnection(cellCenters->GetOutputPort());
        glyph->SetSourceConnection(arrow->GetOutputPort());
        glyph->SetVectorModeToUseVector();
        //glyph->OrientOff();  关闭箭头和输入的向量方向相同
        glyph->Update();

        vtkNew<vtkPolyDataMapper> glyphMapper;
        glyphMapper->SetInputConnection(glyph->GetOutputPort());
        glyphActor3->SetMapper(glyphMapper);
    }

    vtkNew<vtkActor> glyphActor4;
    {
        vtkNew<vtkFloatArray> centerVectors;
        centerVectors->SetNumberOfComponents(3);

        double dir = -1.0;

        for (size_t i = 1; i < static_cast<size_t>(poly4->GetNumberOfPoints()); ++i)
        {
            std::array<double, 2>  startPoint = { poly4->GetPoint(i - 1)[0],poly4->GetPoint(i - 1)[1] };
            std::array<double, 2>  endPoint = { poly4->GetPoint(i)[0],poly4->GetPoint(i)[1] };

            //线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0],endPoint[1] - startPoint[1] };

            // 如果平行于y轴
            if (std::abs(lineVector[0] - 0.0) < 1e-6 && std::abs(lineVector[1] - 0.0) > 1e-6)
            {
                centerVectors->InsertNextTuple3(dir, 0.0, 0.0);
                continue;
            }

            // 如果平行于x轴
            if (std::abs(lineVector[0] - 0.0) > 1e-6 && std::abs(lineVector[1] - 0.0) < 1e-6)
            {
                centerVectors->InsertNextTuple3(0.0, dir, 0.0);
                continue;
            }

            centerVectors->InsertNextTuple3(dir, -dir * (lineVector[0] / lineVector[1]), 0.0);
        }

        vtkNew<vtkCellCenters> cellCenters;
        cellCenters->SetInputData(poly4);
        cellCenters->Update(); // 必须在调用SetVectors之前Update
        cellCenters->GetOutput()->GetPointData()->SetVectors(centerVectors);

        vtkNew<vtkArrowSource> arrow;
        arrow->Update();

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetInputConnection(cellCenters->GetOutputPort());
        glyph->SetSourceConnection(arrow->GetOutputPort());
        glyph->SetVectorModeToUseVector();
        //glyph->OrientOff();  关闭箭头和输入的向量方向相同
        glyph->Update();

        vtkNew<vtkPolyDataMapper> glyphMapper;
        glyphMapper->SetInputConnection(glyph->GetOutputPort());
        glyphActor4->SetMapper(glyphMapper);
    }

    // Renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor1);
    renderer->AddActor(actor2);
    renderer->AddActor(actor3);
    renderer->AddActor(actor4);

    renderer->AddActor(glyphActor1);
    renderer->AddActor(glyphActor2);
    renderer->AddActor(glyphActor3);
    renderer->AddActor(glyphActor4);

    // Window
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->SetBackground(.1, .2, .3);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST10

#ifdef TEST11

/*=========================================================================

  Program:   Visualization Toolkit

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkNew.h"
#include "vtkOpenGLFramebufferObject.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLState.h"
#include "vtkPLYReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"

#include "vtkRegressionTestImage.h"
#include "vtkTestUtilities.h"

#include "vtkRenderWindowInteractor.h"

#include "vtkOpenGLRenderWindow.h"

#include "vtk_glew.h"



//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    vtkNew<vtkRenderWindow> renderWindow;

    if (!renderWindow->SupportsOpenGL())
    {
        cerr << "The platform does not support OpenGL as required\n";
        cerr << vtkOpenGLRenderWindow::SafeDownCast(renderWindow)->GetOpenGLSupportMessage();
        cerr << renderWindow->ReportCapabilities();
        return 1;
    }

    //renderWindow->SetMultiSamples(8);

    // must render once to create context etc
    renderWindow->Render();

    int major, minor;
    vtkOpenGLRenderWindow::SafeDownCast(renderWindow)->GetOpenGLVersion(major, minor);
    cerr << "opengl version " << major << "." << minor << "\n";


    return 0;
}


#endif // TEST11

#ifdef TEST12

#include <vtkCenterOfMass.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <cmath>
#include <limits>

int main(int, char* [])
{
    // Create a point set of a square
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 0, 0);
    points->InsertNextPoint(0, 1, 0);
    points->InsertNextPoint(1, 1, 0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    // Compute the center of mass
    vtkNew<vtkCenterOfMass> centerOfMassFilter;
    centerOfMassFilter->SetInputData(polydata);
    centerOfMassFilter->SetUseScalarsAsWeights(false);
    centerOfMassFilter->Update();

    double center[3];
    centerOfMassFilter->GetCenter(center);

    std::cout << "Center of mass is " << center[0] << " " << center[1] << " "
        << center[2] << std::endl;

    return EXIT_SUCCESS;
}

#endif // TEST12

#ifdef TEST13


#include <iostream>
using namespace std;
#include "vtkSmartPointer.h"

#include "vtkDEMReader.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkWarpScalar.h"
#include "vtkPolyDataNormals.h"
#include "vtkLODActor.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyDataCollection.h"
#include "vtkTriangleFilter.h"
#include "vtkImageResample.h"
#include "vtkHandleWidget.h"
#include "vtkSphereHandleRepresentation.h"
#include "vtkTestUtilities.h"
#include "vtkTesting.h"
#include "vtkAbstractWidget.h"
#include "vtkDataSetWriter.h"
#include "vtkMultiThreader.h"
#include "vtkWarpScalar.h"

int main(int argc, char* argv[])
{

    //读入海拔高度图.SainteHelens.dem
    std::string fname = "D:/Qt/VTK6.3.0/vtkExampleModelFiles/SainteHelens.dem";

    vtkMultiThreader::SetGlobalMaximumNumberOfThreads(2); // 多线程

    // Read height field.
    //
    vtkSmartPointer<vtkDEMReader> demReader = vtkSmartPointer<vtkDEMReader>::New();
    demReader->SetFileName(fname.c_str());

    vtkSmartPointer<vtkImageResample>  resample = vtkSmartPointer<vtkImageResample>::New();
    resample->SetInputConnection(demReader->GetOutputPort());
    resample->SetDimensionality(2);
    resample->SetAxisMagnificationFactor(0, 1);
    resample->SetAxisMagnificationFactor(1, 1);

    // Extract geometry
    vtkSmartPointer<vtkImageDataGeometryFilter> surface = vtkSmartPointer<vtkImageDataGeometryFilter>::New();
    surface->SetInputConnection(resample->GetOutputPort());

    // The Dijkistra interpolator will not accept cells that aren't triangles
    vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
    triangleFilter->SetInputConnection(surface->GetOutputPort());
    triangleFilter->Update();

    vtkSmartPointer<vtkWarpScalar> warp = vtkSmartPointer<vtkWarpScalar>::New();
    warp->SetInputConnection(triangleFilter->GetOutputPort());
    warp->SetScaleFactor(1);
    warp->UseNormalOn();
    warp->SetNormal(0, 0, 1);
    warp->Update();

    // cout << warp->GetOutput()->GetNumberOfCells() << endl;

    // vtkSmartPointer<vtkDataSetWriter> writer = vtkSmartPointer<vtkDataSetWriter>::New();
    // writer->SetInputConnection(resample->GetOutputPort());
    // writer->SetFileName("foo.vtk");
    // writer->Write();

    // Define a LUT mapping for the height field

    double lo = demReader->GetOutput()->GetScalarRange()[0];
    double hi = demReader->GetOutput()->GetScalarRange()[1];

    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetHueRange(0.6, 0);
    lut->SetSaturationRange(1.0, 0);
    lut->SetValueRange(0.5, 1.0);

    vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();

    vtkSmartPointer<vtkPolyDataMapper> demMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    demMapper->SetInputConnection(warp->GetOutputPort());
    demMapper->SetScalarRange(lo, hi);
    demMapper->SetLookupTable(lut);

    vtkSmartPointer<vtkActor> demActor = vtkSmartPointer<vtkActor>::New();
    demActor->SetMapper(demMapper);

    // Create the RenderWindow, Renderer and the DEM + path actors.
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(ren1);
    vtkSmartPointer<vtkRenderWindowInteractor> iren =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    // Add the actors to the renderer, set the background and size
    ren1->AddActor(demActor);
    ren1->GetActiveCamera()->SetViewUp(0, 0, 1);
    ren1->GetActiveCamera()->SetPosition(-99900, -21354, 131801);
    ren1->GetActiveCamera()->SetFocalPoint(41461, 41461, 2815);
    ren1->ResetCamera();
    ren1->ResetCameraClippingRange();

    // Here comes the surface constrained handle widget stuff.....
    vtkSmartPointer<vtkHandleWidget> widget = vtkSmartPointer<vtkHandleWidget>::New();
    widget->SetInteractor(iren);
    vtkSmartPointer<vtkSphereHandleRepresentation> rep = vtkSmartPointer<vtkSphereHandleRepresentation>::New();
    widget->SetRepresentation(rep);

    // Let the surface constrained point-placer be the sole constraint dictating
    // the placement of handles. Lets not over-constrain it allowing axis
    // constrained interactions.
    widget->EnableAxisConstraintOff();

    // Set some defaults on the handle widget
    double d[3] = { 562532, 5.11396e+06, 2618.62 };
    rep->SetWorldPosition(d);
    rep->GetProperty()->SetColor(1.0, 0.0, 0.0);
    rep->GetProperty()->SetLineWidth(1.0);
    rep->GetSelectedProperty()->SetColor(0.2, 0.0, 1.0);
    renWin->Render();
    iren->Initialize();
    widget->EnabledOn();
    renWin->Render();
    ren1->ResetCamera();
    ren1->ResetCameraClippingRange();
    return vtkTesting::InteractorEventLoop(argc, argv, iren);
    return 0;
}

#endif // TEST13

#ifdef TEST14

#include <vtkActor.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

#include <vtkSphereSource.h>
#include <vtksys/SystemTools.hxx>

#include <vtkCamera.h>


int main(int argc, char* argv[])
{
    vtkSmartPointer<vtkPolyData> input1;
    vtkSmartPointer<vtkPolyData> input2;

    vtkNew<vtkSphereSource> sphereSource1;
    sphereSource1->SetCenter(0.5, 0, 0);
    sphereSource1->Update();
    input1 = sphereSource1->GetOutput();

    vtkNew<vtkSphereSource> sphereSource2;
    sphereSource1->SetCenter(-0.5, 0, 0);
    sphereSource2->Update();
    input2 = sphereSource2->GetOutput();

    vtkNew<vtkNamedColors> colors;

    //vtkNew<vtkPolyDataMapper> input1Mapper;
    //input1Mapper->SetInputData(input1);
    //input1Mapper->ScalarVisibilityOff();
    //vtkNew<vtkActor> input1Actor;
    //input1Actor->SetMapper(input1Mapper);
    //input1Actor->GetProperty()->SetDiffuseColor(
    //    colors->GetColor3d("Tomato").GetData());
    //input1Actor->GetProperty()->SetSpecular(0.6);
    //input1Actor->GetProperty()->SetSpecularPower(20);
    //input1Actor->SetPosition(input1->GetBounds()[1] - input1->GetBounds()[0], 0, 0);
    //vtkNew<vtkPolyDataMapper> input2Mapper;
    //input2Mapper->SetInputData(input2);
    //input2Mapper->ScalarVisibilityOff();
    //vtkNew<vtkActor> input2Actor;
    //input2Actor->SetMapper(input2Mapper);
    //input2Actor->GetProperty()->SetDiffuseColor(
    //    colors->GetColor3d("Mint").GetData());
    //input2Actor->GetProperty()->SetSpecular(0.6);
    //input2Actor->GetProperty()->SetSpecularPower(20);
    //input2Actor->SetPosition(-(input1->GetBounds()[1] - input1->GetBounds()[0]),0, 0);
    vtkNew<vtkBooleanOperationPolyDataFilter> booleanOperation;


    // 设置bool操作的类型（并集，交集，差集）
    booleanOperation->SetOperationToUnion();
    //booleanOperation->SetOperationToIntersection();
    //booleanOperation->SetOperationToDifference();

    booleanOperation->SetInputData(0, input1);
    booleanOperation->SetInputData(1, input2);

    vtkNew<vtkPolyDataMapper> booleanOperationMapper;
    booleanOperationMapper->SetInputConnection(booleanOperation->GetOutputPort());
    booleanOperationMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> booleanOperationActor;
    booleanOperationActor->SetMapper(booleanOperationMapper);
    booleanOperationActor->GetProperty()->SetDiffuseColor(1, 0, 0);
    booleanOperationActor->GetProperty()->SetSpecular(.6);
    booleanOperationActor->GetProperty()->SetSpecularPower(20);

    vtkNew<vtkRenderer> renderer;
    //renderer->AddViewProp(input1Actor);
    //renderer->AddViewProp(input2Actor);
    renderer->AddViewProp(booleanOperationActor);
    renderer->SetBackground(colors->GetColor3d("Silver").GetData());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(640, 480);
    renderWindow->SetWindowName("BooleanOperationPolyDataFilter");

    vtkNew<vtkRenderWindowInteractor> renWinInteractor;
    renWinInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renWinInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST14

#ifdef TEST15

#include <vtkAnnotatedCubeActor.h>
#include <vtkAxesActor.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkCellData.h>
#include <vtkColorSeries.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkElevationFilter.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkCallbackCommand.h>

#include <iostream>

static int index = 0;

class CustomOrientationMarkerWidget :
    public vtkOrientationMarkerWidget
{
public:
    static CustomOrientationMarkerWidget* New();
    vtkTypeMacro(CustomOrientationMarkerWidget, vtkOrientationMarkerWidget);
protected:
    CustomOrientationMarkerWidget() = default;
    ~CustomOrientationMarkerWidget() override = default;
private:
    CustomOrientationMarkerWidget(const CustomOrientationMarkerWidget&) = delete;
    CustomOrientationMarkerWidget& operator = (const CustomOrientationMarkerWidget&) = delete;
public:
    virtual void OnLeftButtonDown() override
    {
        // We're only here if we are enabled
        int X = this->Interactor->GetEventPosition()[0];
        int Y = this->Interactor->GetEventPosition()[1];

        // are we over the widget?
        double vp[4];
        this->Renderer->GetViewport(vp);

        this->Renderer->NormalizedDisplayToDisplay(vp[0], vp[1]);
        this->Renderer->NormalizedDisplayToDisplay(vp[2], vp[3]);

        int pos1[2] = { static_cast<int>(vp[0]), static_cast<int>(vp[1]) };
        int pos2[2] = { static_cast<int>(vp[2]), static_cast<int>(vp[3]) };

        this->StartPosition[0] = X;
        this->StartPosition[1] = Y;

        // flag that we are attempting to adjust or move the outline
        this->Moving = 1;
        this->State = this->ComputeStateBasedOnPosition(X, Y, pos1, pos2);
        this->SetCursor(this->State);

        if (this->State == vtkOrientationMarkerWidget::Outside)
        {
            this->Moving = 0;
            return;
        }

        this->EventCallbackCommand->SetAbortFlag(1);
        this->StartInteraction();
        this->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
    }
    virtual void OnLeftButtonUp() override
    {
        std::cout << index++ << '\t' << this->State << '\t' << this->Moving << '\n';

        if (this->State == vtkOrientationMarkerWidget::Outside)
        {
            return;
        }

        // finalize any corner adjustments
        this->SquareRenderer();
        this->UpdateOutline();

        switch (this->State)
        {
        case vtkOrientationMarkerWidget::AdjustingP1:
        case vtkOrientationMarkerWidget::AdjustingP2:
        case vtkOrientationMarkerWidget::AdjustingP3:
        case vtkOrientationMarkerWidget::AdjustingP4:
        case vtkOrientationMarkerWidget::Translating:
            this->EventCallbackCommand->SetAbortFlag(1);
            break;
        }

        // stop adjusting
        this->State = vtkOrientationMarkerWidget::Outside;
        this->Moving = 0;

        this->RequestCursorShape(VTK_CURSOR_DEFAULT);
        this->EndInteraction();
        this->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
        this->Interactor->Render();
    }
    virtual void OnMouseMove() override
    {
        // compute some info we need for all cases
        int X = this->Interactor->GetEventPosition()[0];
        int Y = this->Interactor->GetEventPosition()[1];

        double vp[4];
        this->Renderer->GetViewport(vp);

        // compute display bounds of the widget to see if we are inside or outside
        this->Renderer->NormalizedDisplayToDisplay(vp[0], vp[1]);
        this->Renderer->NormalizedDisplayToDisplay(vp[2], vp[3]);

        int pos1[2] = { static_cast<int>(vp[0]), static_cast<int>(vp[1]) };
        int pos2[2] = { static_cast<int>(vp[2]), static_cast<int>(vp[3]) };

        int state = this->ComputeStateBasedOnPosition(X, Y, pos1, pos2);
        this->State = this->Moving ? this->State : state;
        this->SetCursor(this->State);
        this->OutlineActor->SetVisibility(this->State);

        if (this->State == vtkOrientationMarkerWidget::Outside || !this->Moving)
        {
            this->Interactor->Render();
            return;
        }

        // based on the state set when the left mouse button is clicked,
        // adjust the renderer's viewport
        switch (this->State)
        {
        case vtkOrientationMarkerWidget::AdjustingP1:
            this->ResizeBottomLeft(X, Y);
            break;
        case vtkOrientationMarkerWidget::AdjustingP2:
            this->ResizeBottomRight(X, Y);
            break;
        case vtkOrientationMarkerWidget::AdjustingP3:
            this->ResizeTopRight(X, Y);
            break;
        case vtkOrientationMarkerWidget::AdjustingP4:
            this->ResizeTopLeft(X, Y);
            break;
        case vtkOrientationMarkerWidget::Translating:
            this->MoveWidget(X, Y);
            break;
        }

        this->UpdateOutline();
        this->EventCallbackCommand->SetAbortFlag(1);
        this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
        this->Interactor->Render();
    }
};

vtkStandardNewMacro(CustomOrientationMarkerWidget);

class CustomInteractorStyle :
    public vtkInteractorStyleRubberBand3D
{
public:
    static CustomInteractorStyle* New();
    vtkTypeMacro(CustomInteractorStyle, vtkInteractorStyleRubberBand3D);
protected:
    CustomInteractorStyle() = default;
    ~CustomInteractorStyle() override = default;
private:
    CustomInteractorStyle(const CustomInteractorStyle&) = delete;
    CustomInteractorStyle& operator = (const CustomInteractorStyle&) = delete;
public:
    virtual void OnLeftButtonDown() override
    {
        std::cout << __FUNCTION__ << std::endl;
        Superclass::OnLeftButtonDown();
    }
    virtual void OnLeftButtonUp() override
    {
        std::cout << __FUNCTION__ << std::endl;
        Superclass::OnLeftButtonUp();
    }
    virtual void OnMouseMove() override
    {
        //std::cout << __FUNCTION__ << std::endl;
        Superclass::OnMouseMove();
    }
};

vtkStandardNewMacro(CustomInteractorStyle);

int main(int, char* [])
{
    vtkNew<vtkRenderer> ren;
    vtkNew<vtkRenderWindow> renWin;
    vtkNew<vtkRenderWindowInteractor> iRen;
    iRen->SetRenderWindow(renWin);
    renWin->AddRenderer(ren);

    //vtkNew<vtkInteractorStyleRubberBand3D> style;
    vtkNew<CustomInteractorStyle> style;
    iRen->SetInteractorStyle(style);

    vtkNew<vtkAxesActor> axes;

    // 2D的窗口小部件，可以用鼠标拖来拖去，小部件上可以绘制其他图元
    //vtkNew<vtkOrientationMarkerWidget> om2;
    vtkNew<CustomOrientationMarkerWidget> om2;
    om2->SetOrientationMarker(axes);
    om2->SetViewport(0.5, 0, 1.0, 0.5);
    //om2->SetShouldConstrainSize(true);       // 开启最大最小尺寸限制
    //om2->SetSizeConstraintDimensionSizes();  // 设置最大和最小尺寸
    om2->SetInteractor(iRen);
    om2->EnabledOn();
    om2->InteractiveOn();

    ren->SetBackground(.1, .2, .3);
    ren->ResetCamera();

    renWin->SetSize(800, 600);
    renWin->Render();
    iRen->Start();

    return EXIT_SUCCESS;
}

#endif // TEST15

#ifdef TEST16

// 官方示例 https://vtk.org/doc/nightly/html/classvtkFillHolesFilter.html

#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkIdTypeArray.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkInformation.h>
#include <vtkProperty.h>

//生成带孔洞的网格球
void GenerateData(vtkSmartPointer<vtkPolyData> input)
{
    vtkSmartPointer<vtkSphereSource> sphereSource =
        vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->Update();

    //提供了插入和检索值的方法，并会自动调整大小以保存新数据
    vtkSmartPointer<vtkIdTypeArray> ids =
        vtkSmartPointer<vtkIdTypeArray>::New();
    ids->SetNumberOfComponents(1);
    ids->InsertNextValue(2);
    ids->InsertNextValue(10);

    //选择树中的节,用于存储选择结果
    vtkSmartPointer<vtkSelectionNode> selectionNode =
        vtkSmartPointer<vtkSelectionNode>::New();
    selectionNode->SetFieldType(vtkSelectionNode::CELL);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(ids);
    selectionNode->GetProperties()->Set(vtkSelectionNode::INVERSE(), 1);

    vtkSmartPointer<vtkSelection> selection =
        vtkSmartPointer<vtkSelection>::New();
    selection->AddNode(selectionNode);

    //从vtkdataset提取子集,删除操作
    vtkSmartPointer<vtkExtractSelection> extractSelection =
        vtkSmartPointer<vtkExtractSelection>::New();
    extractSelection->SetInputData(0, sphereSource->GetOutput());
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();

    //vtkDataSetSurfaceFilter是更快版本的vtkgeometry滤波器
    //但它没有一个选择范围。比vtkGeometryFilter使用更多的内存
    //只有一个选择：输入结构类型时是否使用三角形条
    vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter =
        vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    surfaceFilter->SetInputConnection(extractSelection->GetOutputPort());
    surfaceFilter->Update();

    input->ShallowCopy(surfaceFilter->GetOutput());
}
#include <vtkPolyData.h>
#include <vtkFeatureEdges.h>
#include <vtkPolyDataMapper.h>
#include <vtkFillHolesFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

int main()
{
    vtkSmartPointer<vtkPolyData> input =
        vtkSmartPointer<vtkPolyData>::New();
    GenerateData(input);

    vtkSmartPointer<vtkFeatureEdges> featureEdges =
        vtkSmartPointer<vtkFeatureEdges>::New();
    featureEdges->SetInputData(input);
    featureEdges->BoundaryEdgesOn();
    featureEdges->FeatureEdgesOff();
    featureEdges->ManifoldEdgesOff();
    featureEdges->NonManifoldEdgesOff();
    featureEdges->Update();

    int numberOfOpenEdges = featureEdges->GetOutput()->GetNumberOfCells();
    if (numberOfOpenEdges)
    {
        std::cout << "the mesh is open\n" << std::endl;
    }
    else
    {
        std::cout << "the mesh does not open" << std::endl;
        return EXIT_SUCCESS;
    }
    vtkSmartPointer<vtkFillHolesFilter> fillHolesFilter =
        vtkSmartPointer<vtkFillHolesFilter>::New();
    fillHolesFilter->SetInputData(input);
    fillHolesFilter->Update();

    vtkSmartPointer<vtkPolyDataNormals> normals =
        vtkSmartPointer<vtkPolyDataNormals>::New();
    normals->SetInputConnection(fillHolesFilter->GetOutputPort());
    normals->ConsistencyOn(); //很重要，根据其他单元点的顺序调整补充点的顺序
    normals->SplittingOff();
    normals->Update();
    double leftViewport[4] = { 0.0, 0.0, 0.5, 1.0 };
    double rightViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

    vtkSmartPointer<vtkPolyDataMapper> originalMapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    originalMapper->SetInputData(input);
    vtkSmartPointer<vtkProperty> backfaceProp =
        vtkSmartPointer<vtkProperty>::New();
    backfaceProp->SetDiffuseColor(1.0, 0.0, 0.0);
    vtkSmartPointer<vtkActor> originalActor =
        vtkSmartPointer<vtkActor>::New();
    originalActor->SetMapper(originalMapper);
    originalActor->SetBackfaceProperty(backfaceProp);
    originalActor->GetProperty()->SetDiffuseColor(0.0, 1.0, 0.0);

    vtkSmartPointer<vtkPolyDataMapper> edgeMapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    edgeMapper->SetInputData(featureEdges->GetOutput());
    vtkSmartPointer<vtkActor> edgeActor =
        vtkSmartPointer<vtkActor>::New();
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->SetEdgeColor(0., 0., 1.0);
    edgeActor->GetProperty()->SetEdgeVisibility(1);
    edgeActor->GetProperty()->SetLineWidth(5);

    vtkSmartPointer<vtkPolyDataMapper> filledMapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    filledMapper->SetInputData(normals->GetOutput());
    vtkSmartPointer<vtkActor> filledActor =
        vtkSmartPointer<vtkActor>::New();
    filledActor->SetMapper(filledMapper);
    filledActor->GetProperty()->SetDiffuseColor(1.0, 1.0, 0.0);
    ///
    vtkSmartPointer<vtkRenderer> leftRenderer =
        vtkSmartPointer<vtkRenderer>::New();
    leftRenderer->SetViewport(leftViewport);
    leftRenderer->AddActor(originalActor);
    leftRenderer->AddActor(edgeActor);
    leftRenderer->SetBackground(1.0, 1.0, 1.0);

    vtkSmartPointer<vtkRenderer> rightRenderer =
        vtkSmartPointer<vtkRenderer>::New();
    rightRenderer->SetViewport(rightViewport);
    rightRenderer->AddActor(filledActor);
    rightRenderer->SetBackground(0, 0, 0);

    vtkSmartPointer<vtkRenderWindow> renderWindow =
        vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(leftRenderer);
    renderWindow->AddRenderer(rightRenderer);
    renderWindow->SetSize(1200, 600);
    renderWindow->Render();
    renderWindow->SetWindowName("Poly Data Closed");

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    leftRenderer->GetActiveCamera()->SetPosition(0, -1, 0);
    leftRenderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    leftRenderer->GetActiveCamera()->SetViewUp(0, 0, 1);
    leftRenderer->GetActiveCamera()->Azimuth(30);
    leftRenderer->GetActiveCamera()->Elevation(30);
    leftRenderer->ResetCamera();
    rightRenderer->SetActiveCamera(leftRenderer->GetActiveCamera()); //同步响应
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST16

#ifdef TEST17

#include <vtkConeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorObserver.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkObjectFactory.h>

class CustomStyle :
    public vtkInteractorStyleRubberBand3D
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleRubberBand3D);

protected:
    void OnChar() override
    {
        if (!this->Interactor)
        {
            return;
        }

        switch (this->Interactor->GetKeyCode())
        {
        case 'x':
        case 'X':
        {
            // press x to do something
        }
        case 'y':
        case 'Y':
        {
            // press y to do something
        }
        case 'p':
        case 'P':
        {
            return Superclass::OnChar();
        }
        default:
            break;
        }

        return Superclass::OnChar();
    }
};

vtkStandardNewMacro(CustomStyle);

int main(int, char* [])
{
    vtkNew<vtkConeSource> cone;

    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cone->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    //cubeActor->SetTexture()

    //camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);//设置相机位置
    camera->SetFocalPoint(0, 0, 0);//设置相机焦点

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);//设置window大小

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<CustomStyle> style;
    iren->SetInteractorStyle(style);
    iren->SetRenderWindow(renWin);

    std::cout << iren->GetInteractorStyle()->GetClassName() << std::endl;

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST17

#ifdef TEST18

#include <vtkClipPolyData.h>
#include <vtkLODActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelectPolyData.h>
#include <vtkSphereSource.h>

int main(int, char* [])
{

    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetPhiResolution(50);
    sphereSource->SetThetaResolution(100);
    sphereSource->Update();

    vtkNew<vtkPoints> selectionPoints;

    selectionPoints->InsertPoint(0, -0.16553, 0.135971, 0.451972);
    selectionPoints->InsertPoint(1, -0.0880123, -0.134952, 0.4747);
    selectionPoints->InsertPoint(2, 0.00292618, -0.134604, 0.482459);
    selectionPoints->InsertPoint(3, 0.0641941, 0.067112, 0.490947);
    selectionPoints->InsertPoint(4, 0.15577, 0.0734765, 0.469245);
    selectionPoints->InsertPoint(5, 0.166667, -0.129217, 0.454622);
    selectionPoints->InsertPoint(6, 0.241259, -0.123363, 0.420581);
    selectionPoints->InsertPoint(7, 0.240334, 0.0727106, 0.432555);
    selectionPoints->InsertPoint(8, 0.308529, 0.0844311, 0.384357);
    selectionPoints->InsertPoint(9, 0.32672, -0.121674, 0.359187);
    selectionPoints->InsertPoint(10, 0.380721, -0.117342, 0.302527);
    selectionPoints->InsertPoint(11, 0.387804, 0.0455074, 0.312375);
    selectionPoints->InsertPoint(12, 0.43943, -0.111673, 0.211707);
    selectionPoints->InsertPoint(13, 0.470984, -0.0801913, 0.147919);
    selectionPoints->InsertPoint(14, 0.436777, 0.0688872, 0.233021);
    selectionPoints->InsertPoint(15, 0.44874, 0.188852, 0.109882);
    selectionPoints->InsertPoint(16, 0.391352, 0.254285, 0.176943);
    selectionPoints->InsertPoint(17, 0.373274, 0.154162, 0.294296);
    selectionPoints->InsertPoint(18, 0.274659, 0.311654, 0.276609);
    selectionPoints->InsertPoint(19, 0.206068, 0.31396, 0.329702);
    selectionPoints->InsertPoint(20, 0.263789, 0.174982, 0.387308);
    selectionPoints->InsertPoint(21, 0.213034, 0.175485, 0.417142);
    selectionPoints->InsertPoint(22, 0.169113, 0.261974, 0.390286);
    selectionPoints->InsertPoint(23, 0.102552, 0.25997, 0.414814);
    selectionPoints->InsertPoint(24, 0.131512, 0.161254, 0.454705);
    selectionPoints->InsertPoint(25, 0.000192443, 0.156264, 0.475307);
    selectionPoints->InsertPoint(26, -0.0392091, 0.000251724, 0.499943);
    selectionPoints->InsertPoint(27, -0.096161, 0.159646, 0.46438);

    vtkNew<vtkSelectPolyData> loop;
    loop->SetInputConnection(sphereSource->GetOutputPort());
    loop->SetLoop(selectionPoints);
    loop->GenerateSelectionScalarsOn();
    loop->SetSelectionModeToSmallestRegion(); // negative scalars inside

    vtkNew<vtkClipPolyData> clip;
    clip->SetInputConnection(loop->GetOutputPort());

    vtkNew<vtkPolyDataMapper> clipMapper;
    clipMapper->SetInputConnection(clip->GetOutputPort());
    clipMapper->ScalarVisibilityOff();

    vtkNew<vtkProperty> backProp;
    backProp->SetColor(colors->GetColor3d("Tomato").GetData());

    vtkNew<vtkLODActor> clipActor;
    clipActor->SetMapper(clipMapper);
    clipActor->SetBackfaceProperty(backProp);
    clipActor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());

    vtkNew<vtkRenderer> renderer;

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("SelectPolyData");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    // Add the actors to the renderer, set the background and size
    renderer->AddActor(clipActor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderWindow->SetSize(500, 500);

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST18

#ifdef TEST19

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractSelection.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>
#include <vtkUnstructuredGrid.h>

namespace {

    // Catch mouse events
    class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
    {
    public:
        static MouseInteractorStyle* New();

        MouseInteractorStyle()
        {
            selectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            selectedActor = vtkSmartPointer<vtkActor>::New();
        }

        virtual void OnLeftButtonDown() override
        {
            vtkNew<vtkNamedColors> colors;

            // Get the location of the click (in window coordinates)
            int* pos = this->GetInteractor()->GetEventPosition();

            vtkNew<vtkCellPicker> picker;
            picker->SetTolerance(0.0005);

            // Pick from this location.
            picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());

            double* worldPosition = picker->GetPickPosition();
            std::cout << "Cell id is: " << picker->GetCellId() << std::endl;

            if (picker->GetCellId() != -1)
            {

                std::cout << "Pick position is: (" << worldPosition[0] << ", "
                    << worldPosition[1] << ", " << worldPosition[2] << ")" << endl;

                vtkNew<vtkIdTypeArray> ids;
                ids->SetNumberOfComponents(1);
                ids->InsertNextValue(picker->GetCellId());

                vtkNew<vtkSelectionNode> selectionNode;
                selectionNode->SetFieldType(vtkSelectionNode::CELL);
                selectionNode->SetContentType(vtkSelectionNode::INDICES);
                selectionNode->SetSelectionList(ids);

                vtkNew<vtkSelection> selection;
                selection->AddNode(selectionNode);

                vtkNew<vtkExtractSelection> extractSelection;
                extractSelection->SetInputData(0, this->Data);
                extractSelection->SetInputData(1, selection);
                extractSelection->Update();

                // In selection
                vtkNew<vtkUnstructuredGrid> selected;
                selected->ShallowCopy(extractSelection->GetOutput());

                std::cout << "Number of points in the selection: "
                    << selected->GetNumberOfPoints() << std::endl;
                std::cout << "Number of cells in the selection : "
                    << selected->GetNumberOfCells() << std::endl;
                selectedMapper->SetInputData(selected);
                selectedActor->SetMapper(selectedMapper);
                selectedActor->GetProperty()->EdgeVisibilityOn();
                selectedActor->GetProperty()->SetColor(0., 1., 0.);

                selectedActor->GetProperty()->SetLineWidth(3);

                this->Interactor->GetRenderWindow()
                    ->GetRenderers()
                    ->GetFirstRenderer()
                    ->AddActor(selectedActor);
            }
            // Forward events
            vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
        }

        vtkSmartPointer<vtkPolyData> Data;
        vtkSmartPointer<vtkDataSetMapper> selectedMapper;
        vtkSmartPointer<vtkActor> selectedActor;
    };

    vtkStandardNewMacro(MouseInteractorStyle);

} // namespace

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPlaneSource> planeSource;
    planeSource->Update();

    vtkNew<vtkTriangleFilter> triangleFilter;
    triangleFilter->SetInputConnection(planeSource->GetOutputPort());
    triangleFilter->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(triangleFilter->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->GetProperty()->SetColor(colors->GetColor3d("SeaGreen").GetData());
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("CellPicking");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    // Set the custom stype to use for interaction.
    vtkNew<MouseInteractorStyle> style;
    style->SetDefaultRenderer(renderer);
    style->Data = triangleFilter->GetOutput();

    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(actor);
    renderer->ResetCamera();

    renderer->SetBackground(colors->GetColor3d("PaleTurquoise").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST19

#ifdef TEST20

#include <vtkActor.h>
#include <vtkButtonWidget.h>
#include <vtkCoordinate.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTexturedButtonRepresentation2D.h>
#include <vtkProperty2D.h>

#include <array>

namespace {
    void CreateImage(vtkImageData* image, std::string const& color1,
        std::string const& color2);
}

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    // Create two images for texture
    vtkNew<vtkImageData> image1;
    vtkNew<vtkImageData> image2;
    unsigned char banana[3] = { 227, 207, 87 };
    unsigned char tomato[3] = { 255, 99, 71 };
    CreateImage(image1, "Banana", "Tomato");
    CreateImage(image2, "Tomato", "Banana");

    // Create some geometry
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("TexturedButtonWidget");

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Create the widget and its representation
    vtkNew<vtkTexturedButtonRepresentation2D> buttonRepresentation;
    buttonRepresentation->SetNumberOfStates(2);
    buttonRepresentation->SetButtonTexture(0, image1);
    buttonRepresentation->SetButtonTexture(1, image2);
    //buttonRepresentation->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkButtonWidget> buttonWidget;
    buttonWidget->SetInteractor(renderWindowInteractor);
    buttonWidget->SetRepresentation(buttonRepresentation);

    // Add the actors to the scene
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("MidnightBLue").GetData());

    renderWindow->SetSize(640, 480);
    renderWindow->Render();

    // Place the widget. Must be done after a render so that the
    // viewport is defined.
    // Here the widget placement is in normalized display coordinates
    vtkNew<vtkCoordinate> upperRight;
    upperRight->SetCoordinateSystemToNormalizedDisplay();
    upperRight->SetValue(1.0, 1.0);

    double bds[6];
    double sz = 50.0;
    bds[0] = upperRight->GetComputedDisplayValue(renderer)[0] - sz;
    bds[1] = bds[0] + sz;
    bds[2] = upperRight->GetComputedDisplayValue(renderer)[1] - sz;
    bds[3] = bds[2] + sz;
    bds[4] = bds[5] = 0.0;

    // Scale to 1, default is .5
    buttonRepresentation->SetPlaceFactor(1);
    buttonRepresentation->PlaceWidget(bds);

    buttonWidget->On();

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace {
    void CreateImage(vtkImageData* image, std::string const& color1,
        std::string const& color2)
    {
        vtkNew<vtkNamedColors> colors;

        std::array<unsigned char, 3> dc1{ 0, 0, 0 };
        std::array<unsigned char, 3> dc2{ 0, 0, 0 };
        auto c1 = colors->GetColor3ub(color1).GetData();
        auto c2 = colors->GetColor3ub(color2).GetData();
        for (auto i = 0; i < 3; ++i)
        {
            dc1[i] = c1[i];
            dc2[i] = c2[i];
        }

        // Specify the size of the image data
        image->SetDimensions(10, 10, 1);
        image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

        int* dims = image->GetDimensions();

        // Fill the image with
        for (int y = 0; y < dims[1]; y++)
        {
            for (int x = 0; x < dims[0]; x++)
            {
                unsigned char* pixel =
                    static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
                for (int i = 0; i < 3; ++i)
                {
                    if (x < 5)
                    {
                        pixel[i] = dc1[i];
                    }
                    else
                    {
                        pixel[i] = dc2[i];
                    }
                }
            }
        }
    }
} // namespace

#endif // TEST20

#ifdef TEST21

#include <vtkActor.h>
#include <vtkDiskSource.h>
#include <vtkFeatureEdges.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>

namespace
{
    // 顶点数据
    std::vector<double> vertices1{
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
    std::vector<long long> indices1{
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

int main(int, char* [])
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    for (size_t i = 0; i < vertices1.size(); i += 3)
    {
        points->InsertNextPoint(vertices1[i], vertices1[i + 1], vertices1[i + 2]);
    }

    for (size_t i = 0; i < indices1.size(); i += 3)
    {
        cells->InsertNextCell({ indices1[i], indices1[i + 1], indices1[i + 2] });
    }

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    //vtkNew<vtkDiskSource> diskSource;
    //diskSource->Update();


    vtkNew<vtkFeatureEdges> featureEdges;
    //featureEdges->SetInputConnection(diskSource->GetOutputPort());
    featureEdges->SetInputData(polyData);

    // 如果以下值大于0，则对应的图元不是封闭的 https://kitware.github.io/vtk-examples/site/Cxx/PolyData/ClosedSurface/
    int numberOfOpenEdges = featureEdges->GetOutput()->GetNumberOfCells();

    /* 具体看 TEST16 中的链接
    * 边界边：只被一个多边形或者一条边包围的边。
    * 非流形边：被三个或者三个以上多边形包围的边；
    * 特征边：需要设置一个特征角的阈值，当包含同一条边的两个三角形的法向量的夹角大于该阈值时，即为一个特征边。
    * 流行边：只被两个多边形包含的边。
    */

    //featureEdges->BoundaryEdgesOff();
    featureEdges->BoundaryEdgesOn();
    featureEdges->FeatureEdgesOff();
    //featureEdges->FeatureEdgesOn();
    featureEdges->ManifoldEdgesOff();
    //featureEdges->ManifoldEdgesOn();
    featureEdges->NonManifoldEdgesOff();
    //featureEdges->NonManifoldEdgesOn();
    //featureEdges->ColoringOn();
    featureEdges->Update();

    // Visualize
    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputConnection(featureEdges->GetOutputPort());
    edgeMapper->SetScalarModeToUseCellData();
    //edgeMapper->SetScalarModeToUsePointData();
    //edgeMapper->SetScalarModeToUseCellFieldData();
    vtkNew<vtkActor> edgeActor;
    //edgeActor->GetProperty()->SetColor(1., 0., 0.);
    edgeActor->SetMapper(edgeMapper);

    vtkNew<vtkPolyDataMapper> diskMapper;
    //diskMapper->SetInputConnection(diskSource->GetOutputPort());
    diskMapper->SetInputData(polyData);
    vtkNew<vtkActor> diskActor;
    diskActor->SetMapper(diskMapper);
    diskActor->GetProperty()->SetColor(0.,1.,0.);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("BoundaryEdges");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(edgeActor);  // 边界，红色
    renderer->AddActor(diskActor); // 原始的环状图元
    renderer->SetBackground(.1,.2,.3);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST21

#ifdef TEST22

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkClipPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkFeatureEdges.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkStripper.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
    // Define colors
    vtkNew<vtkNamedColors> colors;
    auto backgroundColor = colors->GetColor3d("steel_blue");
    auto boundaryColor = colors->GetColor3d("banana");
    auto clipColor = colors->GetColor3d("tomato");

    // PolyData to process
    vtkNew<vtkSphereSource> source;
    source->SetThetaResolution(20);
    source->SetPhiResolution(11);
    source->Update();
    auto polyData = source->GetOutput();

    vtkNew<vtkPlane> plane;
    plane->SetOrigin(polyData->GetCenter());
    plane->SetNormal(1.0, -1.0, -1.0);

    vtkNew<vtkClipPolyData> clipper;
    clipper->SetInputData(polyData);
    clipper->SetClipFunction(plane);
    clipper->SetValue(0);
    clipper->Update();

    polyData = clipper->GetOutput();

    vtkNew<vtkDataSetMapper> clipMapper;
    clipMapper->SetInputData(polyData);

    vtkNew<vtkActor> clipActor;
    clipActor->SetMapper(clipMapper);
    clipActor->GetProperty()->SetDiffuseColor(clipColor.GetData());
    clipActor->GetProperty()->SetInterpolationToFlat();
    clipActor->GetProperty()->EdgeVisibilityOn();

    // Now extract feature edges
    vtkNew<vtkFeatureEdges> boundaryEdges;
    boundaryEdges->SetInputData(polyData);
    boundaryEdges->BoundaryEdgesOn();
    boundaryEdges->FeatureEdgesOff();
    boundaryEdges->NonManifoldEdgesOff();
    boundaryEdges->ManifoldEdgesOff();

    vtkNew<vtkStripper> boundaryStrips;
    boundaryStrips->SetInputConnection(boundaryEdges->GetOutputPort());
    boundaryStrips->Update();

    // Change the polylines into polygons
    vtkNew<vtkPolyData> boundaryPoly;
    boundaryPoly->SetPoints(boundaryStrips->GetOutput()->GetPoints());
    boundaryPoly->SetPolys(boundaryStrips->GetOutput()->GetLines());

    vtkNew<vtkPolyDataMapper> boundaryMapper;
    boundaryMapper->SetInputData(boundaryPoly);

    vtkNew<vtkActor> boundaryActor;
    boundaryActor->SetMapper(boundaryMapper);
    boundaryActor->GetProperty()->SetDiffuseColor(boundaryColor.GetData());

    // Create graphics stuff
    //
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(backgroundColor.GetData());
    renderer->UseHiddenLineRemovalOn();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(640, 480);

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    // Add the actors to the renderer, set the background and size
    //
    renderer->AddActor(clipActor);
    renderer->AddActor(boundaryActor);

    // Generate an interesting view
    //
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->GetActiveCamera()->Dolly(1.2);
    renderer->ResetCameraClippingRange();

    renderWindow->Render();
    renderWindow->SetWindowName("CapClip");
    renderWindow->Render();

    interactor->Start();

    return EXIT_SUCCESS;
}


#endif // TEST22

#ifdef TEST23

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkClipPolyData.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkDataSetMapper.h>
#include <vtkFeatureEdges.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkStripper.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkLookupTable.h>

#include <vector>

namespace
{
    std::vector<float> vertices{
        0.,0.,
        0.,1.,
        0.,2.,
        0.,3.,

        1.,3.,
        1.,2.,
        1.,1.,
        1.,0.,

        2.,0.,
        2.,1.,
        2.,2.,
        2.,3.,

        3.,3.,
        3.,2.,
        3.,1.,
        3.,0.
    };

    std::vector<int> indices{
        // 四条外部线
        0,1,
        1,2,
        2,3,

        12,13,
        13,14,
        14,15,

        0,7,
        7,8,
        8,15,

        3,4,
        4,11,
        11,12,

        // 内部线
        1,6,
        6,9,
        9,14,

        2,5,
        5,10,
        10,13,

        4,5,
        5,6,
        6,7,

        8,9,
        9,10,
        10,11,
    };

    std::vector<int> indicesPoly{
        0,1,6,7,
        1,2,5,6,
        2,3,4,5,

        4,5,10,11,
        5,6,9,10,
        6,7,8,9,

        8,9,14,15,
        9,10,13,14,
        10,11,12,13,
    };

    std::vector<float> fields{
        1.0f,
        1.0f,
        1.0f,
        1.0f,

        2.0f,
        2.0f,
        2.0f,
        2.0f,

        3.0f,
        3.0f,
        3.0f,
        3.0f,

        4.0f,
        4.0f,
        4.0f,
        4.0f,
    };

    vtkSmartPointer<vtkPolyData> GenPolyData()
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cellsLine;
        vtkNew<vtkCellArray> cellsPoly;

        for (size_t i = 0; i < vertices.size(); i += 2)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], 0.0);
        }
        for (size_t i = 0; i < indices.size(); i += 2)
        {
            cellsLine->InsertNextCell({ indices[i], indices[i + 1] });
        }
        for (size_t i = 0; i < indicesPoly.size(); i += 4)
        {
            cellsPoly->InsertNextCell({ indicesPoly[i],indicesPoly[i + 1], indicesPoly[i + 2],indicesPoly[i + 3] });
        }
        polyData->SetPoints(points);
        polyData->SetLines(cellsLine);
        //polyData->SetPolys(cellsPoly);

        vtkNew<vtkFloatArray> scalars;
        for (size_t i = 0; i < 16; ++i)
        {
            scalars->InsertNextValue(fields[i]);
        }

        polyData->GetPointData()->SetScalars(scalars);

        return polyData;
    }
}

// vtkStripper可以从输入的多边形、三角形带、线 生成 三角形带、多边形线

int main(int argc, char* argv[])
{
    auto polyData = GenPolyData();

    std::cout << "Number of cells before stripping: "
        << polyData->GetNumberOfCells() << std::endl;

    vtkNew<vtkLookupTable> surfaceLUT;
    surfaceLUT->SetRange(polyData->GetPointData()->GetScalars()->GetRange());
    surfaceLUT->Build();

    vtkNew<vtkPolyDataMapper> mapperLine;
    vtkNew<vtkActor> actorLine;
    mapperLine->SetInputData(polyData);
    mapperLine->ScalarVisibilityOn();
    mapperLine->SetScalarRange(polyData->GetPointData()->GetScalars()->GetRange());
    mapperLine->SetLookupTable(surfaceLUT);
    actorLine->SetMapper(mapperLine);
    actorLine->GetProperty()->SetColor(1., 0., 0.);

    // ----------------------------------------

    vtkNew<vtkStripper> stripper;
    stripper->SetInputData(polyData);
    //stripper->PassCellDataAsFieldDataOn();
    //stripper->PassThroughPointIdsOn();
    //stripper->PassThroughCellIdsOn();
    stripper->SetMaximumLength(4); //指定一个三角形带中的最大三角形数量，或一个多边形线中的最大线数。
    stripper->JoinContiguousSegmentsOn();
    stripper->Update();

    std::cout << "Number of cells after stripping: "
        << stripper->GetOutput()->GetNumberOfCells() << std::endl;

    // 
    // Change the polylines into polygons
    vtkNew<vtkPolyData> boundaryPoly;
    boundaryPoly->SetPoints(stripper->GetOutput()->GetPoints());
    boundaryPoly->SetPolys(stripper->GetOutput()->GetLines());
    boundaryPoly->GetPointData()->SetScalars(polyData->GetPointData()->GetScalars());
    //boundaryPoly->GetPointData()->SetScalars(stripper->GetOutput()->GetPointData()->GetScalars());

    for (size_t i = 0; i < boundaryPoly->GetPolys()->GetNumberOfCells(); ++i)
    {
        vtkNew<vtkIdList> pts;
        boundaryPoly->GetPolys()->GetCellAtId(i, pts);
    }

    std::cout << "valueSize\t" << boundaryPoly->GetPointData()->GetScalars()->GetNumberOfValues() << '\n';
    std::cout << "arraySize\t" << boundaryPoly->GetPointData()->GetNumberOfArrays() << '\n';
    std::cout << "pointSize\t" << boundaryPoly->GetNumberOfPoints() << '\n';
    std::cout << "cellSize \t" << boundaryPoly->GetNumberOfCells() << '\n';
    std::cout << "polySize \t" << boundaryPoly->GetNumberOfPolys() << '\n';

    vtkNew<vtkPolyDataMapper> mapperPoly;
    mapperPoly->SetInputData(boundaryPoly);
    mapperPoly->ScalarVisibilityOn();
    mapperPoly->SetScalarRange(polyData->GetPointData()->GetScalars()->GetRange());
    mapperPoly->SetLookupTable(surfaceLUT);

    vtkNew<vtkActor> actorPoly;
    actorPoly->SetMapper(mapperPoly);
    //actorPoly->GetProperty()->SetColor(0., 1., 0.);

    // Create graphics stuff
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actorPoly);
    //renderer->AddActor(actorLine);

    renderer->SetBackground(.1, .2, .3);
    renderer->UseHiddenLineRemovalOn();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(640, 480);

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    renderer->ResetCamera();

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}


#endif // TEST23

#ifdef TEST24

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkContourWidget.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygonalSurfaceContourLineInterpolator.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTriangleFilter.h>
#include <vtkXMLPolyDataReader.h>

namespace
{
    std::vector<float> vertices{
        0.,0.,
        0.,1.,
        0.,2.,
        0.,3.,

        1.,3.,
        1.,2.,
        1.,1.,
        1.,0.,

        2.,0.,
        2.,1.,
        2.,2.,
        2.,3.,

        3.,3.,
        3.,2.,
        3.,1.,
        3.,0.
    };

    std::vector<int> indicesLine{
        // 竖线
        0,1,
        1,2,
        2,3,

        4,5,
        5,6,
        6,7,

        8,9,
        9,10,
        10,11,

        12,13,
        13,14,
        14,15,

        // 横线
        0,7,
        7,8,
        8,15,

        1,6,
        6,9,
        9,14,

        2,5,
        5,10,
        10,13,

        3,4,
        4,11,
        11,12,
    };

    std::vector<int> indicesPoly{
        0,1,6,7,
        1,2,5,6,
        2,3,4,5,

        4,5,10,11,
        5,6,9,10,
        6,7,8,9,

        8,9,14,15,
        9,10,13,14,
        10,11,12,13,
    };

    vtkSmartPointer<vtkPolyData> GenPolyData()
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cellsLine;
        vtkNew<vtkCellArray> cellsPoly;

        for (size_t i = 0; i < vertices.size(); i += 2)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], 0.0);
        }
        for (size_t i = 0; i < indicesLine.size(); i += 2)
        {
            cellsLine->InsertNextCell({ indicesLine[i], indicesLine[i + 1] });
        }
        for (size_t i = 0; i < indicesPoly.size(); i+=4)
        {
            cellsPoly->InsertNextCell({indicesPoly[i],indicesPoly[i + 1], indicesPoly[i + 2],indicesPoly[i+3]});
        }
        polyData->SetPoints(points);
        //polyData->SetLines(cellsLine);
        polyData->SetPolys(cellsPoly);

        return polyData;
    }
}

// vtkTriangleFilter 从多边形(polygons)或三角形带(triangle strips)转换为三角形(triangles)，也可以从多线段生成线段

int main(int argc, char* argv[])
{
    auto polyData = GenPolyData();

#if(1)
    // 按p可以绘制线框
    vtkNew<vtkTriangleFilter> triangleFilter;
    //triangleFilter->PassLinesOff();
    //triangleFilter->PassVertsOff();
    triangleFilter->SetInputData(polyData);
    triangleFilter->Update();

    // mapper actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(triangleFilter->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1,0,0);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(1000, 800);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("PolygonalSurfaceContourLineInterpolator");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    renderWindow->Render();
    interactor->Start();

#else
    // 求最短路径，可以用鼠标选取图像中的点，然后会自动标出到该点的最短路径
    // The Dijkistra interpolator will not accept cells that aren't triangles
    vtkNew<vtkTriangleFilter> triangleFilter;
    triangleFilter->SetInputData(polyData);
    triangleFilter->Update();

    auto pd = triangleFilter->GetOutput();

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(triangleFilter->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToFlat();
    actor->GetProperty()->SetColor(1, 0, 0);

    // Create the render window, renderer and interactor.

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("PolygonalSurfaceContourLineInterpolator");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    // Add the actors to the renderer, set the background and size

    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    // Here comes the contour widget stuff...

    vtkNew<vtkContourWidget> contourWidget;
    contourWidget->SetInteractor(interactor);
    vtkSmartPointer<vtkOrientedGlyphContourRepresentation> rep =
        dynamic_cast<vtkOrientedGlyphContourRepresentation*>(
            contourWidget->GetRepresentation());
    rep->GetLinesProperty()->SetColor(0, 1, 0);
    rep->GetLinesProperty()->SetLineWidth(3.0);

    vtkNew<vtkPolygonalSurfacePointPlacer> pointPlacer;
    pointPlacer->AddProp(actor);
    pointPlacer->GetPolys()->AddItem(pd);
    rep->SetPointPlacer(pointPlacer);

    vtkNew<vtkPolygonalSurfaceContourLineInterpolator> interpolator;
    interpolator->GetPolys()->AddItem(pd);
    rep->SetLineInterpolator(interpolator);

    renderWindow->Render();
    interactor->Initialize();

    contourWidget->EnabledOn();

    interactor->Start();

#endif

    return EXIT_SUCCESS;
}

#endif // TEST24

#ifdef TEST25

#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>

#define vtkSPtr vtkSmartPointer
#define vtkSPtrNew(Var, Type) vtkSPtr<Type> Var = vtkSPtr<Type>::New();

using namespace std;

int main()
{
    vtkSPtrNew(sphere, vtkSphereSource);
    sphere->SetCenter(0, 0, 0);
    sphere->SetRadius(1);
    sphere->Update();

    vtkSPtrNew(sphereMapper, vtkPolyDataMapper);

    // set mapper to make sphere always on top
    const double units0 = -66000;
    sphereMapper->SetInputData(sphere->GetOutput());
    sphereMapper->SetResolveCoincidentTopologyToPolygonOffset();
    sphereMapper->SetRelativeCoincidentTopologyLineOffsetParameters(0, units0);
    sphereMapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, units0);
    sphereMapper->SetRelativeCoincidentTopologyPointOffsetParameter(units0);

    vtkSPtrNew(sphereActor, vtkActor);
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(1, 0, 0);

    vtkSPtrNew(cone, vtkConeSource);
    cone->SetRadius(2);
    cone->SetHeight(4);
    cone->Update();
    vtkSPtrNew(coneMapper, vtkPolyDataMapper);
    coneMapper->SetInputData(cone->GetOutput());
    vtkSPtrNew(coneActor, vtkActor);
    coneActor->SetMapper(coneMapper);

    vtkSPtrNew(renderer, vtkRenderer);
    renderer->AddActor(coneActor);
    renderer->AddActor(sphereActor);
    renderer->SetBackground(0, 0, 0);

    vtkSPtrNew(renderWindow, vtkRenderWindow);
    renderWindow->AddRenderer(renderer);

    vtkSPtrNew(renderWindowInteractor, vtkRenderWindowInteractor);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->ResetCamera();
    renderWindow->Render();
    renderWindowInteractor->Start();
    return 0;
}

#endif // TEST25

#ifdef TEST26

#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkContourWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkPolyDataCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {

    class MyCallback : public vtkCommand
    {
    public:
        static MyCallback* New()
        {
            return new MyCallback;
        }
        MyCallback()
        {
        }

        virtual void Execute(vtkObject* caller, unsigned long, void*)
        {
            vtkContourWidget* contourWidget =
                reinterpret_cast<vtkContourWidget*>(caller);
            vtkContourRepresentation* rep = static_cast<vtkContourRepresentation*>(
                contourWidget->GetRepresentation());
            std::cout << "There are " << rep->GetNumberOfNodes() << " nodes."
                << std::endl;
        }

        void SetSphereSource(vtkSmartPointer<vtkSphereSource> sphere)
        {
            this->SphereSource = sphere;
        }

    private:
        vtkSmartPointer<vtkSphereSource> SphereSource;
    };

} // namespace

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(5);
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // Create the RenderWindow, Renderer
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("PolygonalSurfacePointPlacer");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("CadetBlue").GetData());

    vtkNew<vtkContourWidget> contourWidget;
    contourWidget->SetInteractor(interactor);

    vtkNew<MyCallback> callback;
    callback->SetSphereSource(sphereSource);
    contourWidget->AddObserver(vtkCommand::InteractionEvent, callback);

    vtkOrientedGlyphContourRepresentation* rep =
        dynamic_cast<vtkOrientedGlyphContourRepresentation*>(
            contourWidget->GetRepresentation());

    vtkNew<vtkPolygonalSurfacePointPlacer> pointPlacer;
    pointPlacer->AddProp(actor);
    pointPlacer->GetPolys()->AddItem(sphereSource->GetOutput());

    rep->GetLinesProperty()->SetColor(colors->GetColor3d("Crimson").GetData());
    rep->GetLinesProperty()->SetLineWidth(3.0);
    rep->SetPointPlacer(pointPlacer);

    contourWidget->EnabledOn();
    renderer->ResetCamera();
    renderWindow->Render();
    interactor->Initialize();

    interactor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST26

#ifdef TEST27

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourTriangulator.h>
#include <vtkDataSetMapper.h>
#include <vtkMarchingSquares.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
    std::string inputFileName;
    if (argc > 1)
    {
        inputFileName = argv[1];
    }
    else
    {
        cout << "Usage: " << argv[0]
            << " png_file [iso value] e.g. fullhead15.png 500" << endl;
        return EXIT_FAILURE;
    }
    int isoValue = 500;
    if (argc > 2)
    {
        isoValue = atoi(argv[2]);
    }

    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPNGReader> reader;
    if (!reader->CanReadFile(inputFileName.c_str()))
    {
        std::cerr << "Error: Could not read " << inputFileName << ".\n";
        return EXIT_FAILURE;
    }
    reader->SetFileName(inputFileName.c_str());
    reader->Update();

    vtkNew<vtkMarchingSquares> iso;
    iso->SetInputConnection(reader->GetOutputPort());
    iso->SetValue(0, isoValue);

    vtkNew<vtkDataSetMapper> isoMapper;
    isoMapper->SetInputConnection(iso->GetOutputPort());
    isoMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> isoActor;
    isoActor->SetMapper(isoMapper);
    isoActor->GetProperty()->SetColor(
        colors->GetColor3d("MediumOrchid").GetData());

    vtkNew<vtkContourTriangulator> poly;
    poly->SetInputConnection(iso->GetOutputPort());

    vtkNew<vtkDataSetMapper> polyMapper;
    polyMapper->SetInputConnection(poly->GetOutputPort());
    polyMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> polyActor;
    polyActor->SetMapper(polyMapper);
    polyActor->GetProperty()->SetColor(colors->GetColor3d("Gray").GetData());

    // Standard rendering classes
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renWin;
    renWin->SetMultiSamples(0);
    renWin->AddRenderer(renderer);
    renWin->SetWindowName("ContourTriangulator");

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renderer->AddActor(polyActor);
    renderer->AddActor(isoActor);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
    renWin->SetSize(300, 300);

    vtkCamera* camera = renderer->GetActiveCamera();
    renderer->ResetCamera();
    camera->Azimuth(180);

    renWin->Render();
    iren->Initialize();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST27

#ifdef TEST28

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkContourFilter.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStripper.h>
#include <vtkTextProperty.h>
#include <vtkXMLPolyDataReader.h>

#include <iomanip>
#include <iostream>

#ifdef VTK_CELL_ARRAY_V2
#include <vtkCellArrayIterator.h>
#endif // VTK_CELL_ARRAY_V2

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> colors;

    int pointThreshold = 10;

    vtkSmartPointer<vtkPolyData> polyData;
    vtkNew<vtkContourFilter> contours;

    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(1);

    // If a file is present, read it, otherwise generate some random
    // scalars on a plane
    if (argc > 1)
    {
        vtkNew<vtkXMLPolyDataReader> reader;
        reader->SetFileName(argv[1]);
        reader->Update();

        double range[2];
        reader->GetOutput()->GetScalarRange(range);
        polyData = reader->GetOutput();

        std::cout << "range: " << range[0] << ", " << range[1] << std::endl;
        contours->SetValue(0, (range[1] + range[0]) / 2.0);

        contours->SetInputConnection(reader->GetOutputPort());
        if (argc == 3)
        {
            contours->SetValue(0, atof(argv[2]));
        }
        else if (argc == 4)
        {
            contours->SetValue(0, atof(argv[2]));
            contours->SetValue(1, atof(argv[3]));
        }
        else if (argc == 5)
        {
            contours->GenerateValues(atoi(argv[2]), atof(argv[3]), atof(argv[4]));
        }
    }
    else
    {
        vtkNew<vtkPlaneSource> plane;
        plane->SetXResolution(10);
        plane->SetYResolution(10);
        plane->Update();

        vtkNew<vtkDoubleArray> randomScalars;
        randomScalars->SetNumberOfComponents(1);
        randomScalars->SetName("Isovalues");
        for (int i = 0; i < plane->GetOutput()->GetNumberOfPoints(); i++)
        {
            randomScalars->InsertNextTuple1(
                randomSequence->GetRangeValue(-100.0, 100.0));
            randomSequence->Next();
        }
        plane->GetOutput()->GetPointData()->SetScalars(randomScalars);
        polyData = plane->GetOutput();
        contours->SetInputConnection(plane->GetOutputPort());
        contours->GenerateValues(5, -100, 100);
        pointThreshold = 0;
    }

    // Connect the segments of the contours into polylines
    vtkNew<vtkStripper> contourStripper;
    contourStripper->SetInputConnection(contours->GetOutputPort());
    contourStripper->Update();

    auto numberOfContourLines = contourStripper->GetOutput()->GetNumberOfLines();

    std::cout << "There are " << numberOfContourLines << " contour lines."
        << std::endl;

    vtkPoints* points = contourStripper->GetOutput()->GetPoints();
    vtkCellArray* cells = contourStripper->GetOutput()->GetLines();
    vtkDataArray* scalars =
        contourStripper->GetOutput()->GetPointData()->GetScalars();

    // Create a polydata that contains point locations for the contour
    // line labels
    vtkNew<vtkPolyData> labelPolyData;
    vtkNew<vtkPoints> labelPoints;
    vtkNew<vtkDoubleArray> labelScalars;
    labelScalars->SetNumberOfComponents(1);
    labelScalars->SetName("Isovalues");

#ifdef VTK_CELL_ARRAY_V2

    // Newer versions of vtkCellArray prefer local iterators:
    auto cellIter = vtk::TakeSmartPointer(cells->NewIterator());
    for (cellIter->GoToFirstCell(); !cellIter->IsDoneWithTraversal();
        cellIter->GoToNextCell())
    {
        vtkIdList* cell = cellIter->GetCurrentCell();
        if (cell->GetNumberOfIds() < pointThreshold)
        {
            continue;
        }
        std::cout << "Line " << cellIter->GetCurrentCellId() << ": " << std::endl;

        // Compute the point id to hold the label
        // Mid point or a random point
        // const vtkIdType samplePtIdx = cell->GetNumberOfIds() / 2;
        const vtkIdType samplePtIdx = static_cast<vtkIdType>(
            randomSequence->GetRangeValue(0, cell->GetNumberOfIds()));
        randomSequence->Next();

        auto midPointId = cell->GetId(samplePtIdx);

        double midPoint[3];
        points->GetPoint(midPointId, midPoint);
        std::cout << "\tmidPoint is " << midPointId << " with coordinate "
            << "(" << std::fixed << std::setprecision(2) << midPoint[0]
            << ", " << midPoint[1] << ", " << midPoint[2] << ")"
            << " and value " << scalars->GetTuple1(midPointId) << std::endl;
        labelPoints->InsertNextPoint(midPoint);
        labelScalars->InsertNextTuple1(scalars->GetTuple1(midPointId));
    }

#else // VTK_CELL_ARRAY_V2

    // Older implementations of vtkCellArray use internal iterator APIs (not
    // thread safe):
    vtkIdType* indices;
    vtkIdType numberOfPoints;
    unsigned int lineCount = 0;
    for (cells->InitTraversal(); cells->GetNextCell(numberOfPoints, indices);
        lineCount++)
    {
        if (numberOfPoints < pointThreshold)
        {
            continue;
        }
        std::cout << "Line " << lineCount << ": " << std::endl;

        // Compute the point id to hold the label
        // Mid point or a random point
        // auto midPointId = indices[numberOfPoints / 2];
        // auto midPointId =
        //    indices[static_cast<vtkIdType>(vtkMath::Random(0, numberOfPoints))];
        const vtkIdType samplePtIdx = static_cast<vtkIdType>(
            randomSequence->GetRangeValue(0, cell->GetNumberOfIds()));
        randomSequence->Next();

        double midPoint[3];
        points->GetPoint(midPointId, midPoint);
        std::cout << "\tmidPoint is " << midPointId << " with coordinate "
            << "(" << std::fixed << std::setprecision(2) << midPoint[0]
            << ", " << midPoint[1] << ", " << midPoint[2] << ")"
            << " and value " << scalars->GetTuple1(midPointId) << std::endl;
        labelPoints->InsertNextPoint(midPoint);
        labelScalars->InsertNextTuple1(scalars->GetTuple1(midPointId));
    }

#endif // VTK_CELL_ARRAY_V2

    labelPolyData->SetPoints(labelPoints);
    labelPolyData->GetPointData()->SetScalars(labelScalars);

    vtkNew<vtkPolyDataMapper> contourMapper;
    contourMapper->SetInputConnection(contourStripper->GetOutputPort());
    contourMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> isolines;
    isolines->SetMapper(contourMapper);
    isolines->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
    isolines->GetProperty()->SetLineWidth(2);

    vtkNew<vtkLookupTable> surfaceLUT;
    surfaceLUT->SetRange(polyData->GetPointData()->GetScalars()->GetRange());
    surfaceLUT->Build();

    vtkNew<vtkPolyDataMapper> surfaceMapper;
    surfaceMapper->SetInputData(polyData);
    surfaceMapper->ScalarVisibilityOn();
    surfaceMapper->SetScalarRange(
        polyData->GetPointData()->GetScalars()->GetRange());
    surfaceMapper->SetLookupTable(surfaceLUT);

    vtkNew<vtkActor> surface;
    surface->SetMapper(surfaceMapper);

    // The labeled data mapper will place labels at the points
    vtkNew<vtkLabeledDataMapper> labelMapper;
    labelMapper->SetFieldDataName("Isovalues");
    labelMapper->SetInputData(labelPolyData);
    labelMapper->SetLabelModeToLabelScalars();
    labelMapper->SetLabelFormat("%6.2f");
    labelMapper->GetLabelTextProperty()->SetColor(
        colors->GetColor3d("Gold").GetData());

    vtkNew<vtkActor2D> isolabels;
    isolabels->SetMapper(labelMapper);

    // Create a renderer and render window
    vtkNew<vtkRenderer> renderer;

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(600, 600);
    renderWindow->SetWindowName("LabelContours");

    // Create an interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actors to the scene
    renderer->AddActor(surface);
    renderer->AddActor(isolines);
    renderer->AddActor(isolabels);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // Render the scene (lights and cameras are created automatically)
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST28

#ifdef TEST29

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourTriangulator.h>
#include <vtkDataSetMapper.h>
#include <vtkMarchingSquares.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>

#include <set>

namespace
{
    std::vector<float> vertices{
        0.,0.,
        0.,1.,
        0.,2.,
        0.,3.,

        1.,3.,
        1.,2.,
        1.,1.,
        1.,0.,

        2.,0.,
        2.,1.,
        2.,2.,
        2.,3.,

        3.,3.,
        3.,2.,
        3.,1.,
        3.,0.
    };

    std::vector<int> indices{
        // 四条外部线
        0,1,
        1,2,
        2,3,

        12,13,
        13,14,
        14,15,

        0,7,
        7,8,
        8,15,

        3,4,
        4,11,
        11,12,

        // 内部线
        1,6,
        6,9,
        9,14,

        2,5,
        5,10,
        10,13,

        4,5,
        5,6,
        6,7,

        8,9,
        9,10,
        10,11,
    };

    std::vector<int> indicesPoly{
        0,1,6,7,
        1,2,5,6,
        2,3,4,5,

        4,5,10,11,
        5,6,9,10,
        6,7,8,9,

        8,9,14,15,
        9,10,13,14,
        10,11,12,13,
    };

    std::vector<float> fields{
        1.0f,
        2.0f,
        3.0f,
        4.0f,

        1.0f,
        2.0f,
        3.0f,
        4.0f,

        1.0f,
        2.0f,
        3.0f,
        4.0f,

        1.0f,
        2.0f,
        3.0f,
        4.0f,
    };

    vtkSmartPointer<vtkPolyData> GenPolyData()
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cellsLine;
        vtkNew<vtkCellArray> cellsPoly;

        for (size_t i = 0; i < vertices.size(); i += 2)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], 0.0);
        }
        for (size_t i = 0; i < indices.size(); i += 2)
        {
            cellsLine->InsertNextCell({ indices[i], indices[i + 1] });
        }
        for (size_t i = 0; i < indicesPoly.size(); i += 4)
        {
            cellsPoly->InsertNextCell({ indicesPoly[i],indicesPoly[i + 1], indicesPoly[i + 2],indicesPoly[i + 3] });
        }
        polyData->SetPoints(points);
        polyData->SetLines(cellsLine);
        //polyData->SetPolys(cellsPoly);

        vtkNew<vtkFloatArray> scalars;
        for (size_t i = 0; i < 16; ++i)
        {
            scalars->InsertNextValue(fields[i]);
        }

        polyData->GetPointData()->SetScalars(scalars);

        // ---------------------------------------
        // 使用vtkContourTriangulator生成面
        auto cellSize = cellsLine->GetNumberOfCells();
        vtkNew<vtkIdList> pts1;
        vtkNew<vtkIdList> pts4;
        cellsPoly->GetCellAtId(0, pts1);
        cellsPoly->GetCellAtId(4, pts4);

        vtkSmartPointer<vtkCellArray> outputPoly = vtkSmartPointer<vtkCellArray>::New();
        double normals[]{ 0.,0.,1. };
        vtkContourTriangulator::TriangulateContours(polyData, 0, 24, outputPoly, normals);
        auto outCellSize = outputPoly->GetNumberOfCells();
        vtkNew<vtkIdList> pts2;
        vtkNew<vtkIdList> pts3;
        outputPoly->GetCellAtId(0, pts2);
        outputPoly->GetCellAtId(1, pts3);

        vtkNew<vtkPolyData> outPolyData;
        outPolyData->SetPoints(points);
        outPolyData->SetPolys(outputPoly);
        // ------------------------------
        auto s1 = cellsLine->GetSize(); // 94
        auto s2 = cellsPoly->GetSize(); // 78
        auto s3 = points->GetNumberOfPoints();
        auto s4 = cellsLine->GetNumberOfCells(); // 24
        auto s5 = cellsPoly->GetNumberOfCells(); // 9

        // --------------------------------------
        // 自定义方法闭合线框生成最小的三角面
        // map.key 第几个点，map.value 点都被那些单元使用
        std::map<int, std::vector<int>> pointWithCell;
        // i表示第几个点
        for (size_t i = 0; i < points->GetNumberOfPoints(); ++i)
        {
            std::vector<int> theCells;
            // j表示第几个单元（此处表示第几条线段）
            for (size_t j = 0; j < cellsLine->GetNumberOfCells(); ++j)
            {
                vtkNew<vtkIdList> pts5;
                cellsLine->GetCellAtId(j, pts5);
                auto r0 = pts5->GetNumberOfIds();
                auto r3 = pts5->GetPointer(0);

                // m表示该线段中第几个点（线段只有两个点）
                for (size_t m = 0; m < r0; ++m)
                {
                    if (r3[m] == i)
                    {
                        theCells.emplace_back(j);
                        break;
                    }
                }
            }
            pointWithCell.emplace(i, theCells);
        }

        std::map<int,std::vector<int>> otherPoints;

        // 找到第i个点可以和那些点组成线段
        for (size_t i = 0; i < pointWithCell.size(); ++i)
        {
            // 该点被几个单元共用
            auto numCells = pointWithCell[i].size();

            std::vector<int> temp;

            vtkNew<vtkIdList> pts6;
            for (size_t j = 0; j < numCells; ++j)
            {
                cellsLine->GetCellAtId(pointWithCell[i][j], pts6);

                for (size_t m = 0; m < pts6->GetNumberOfIds(); ++m)
                {
                    // 找到共享这个点的线段的另一个点
                    if (pts6->GetPointer(0)[m] != i)
                    {
                        temp.emplace_back(pts6->GetPointer(0)[m]);
                        break;
                    }
                }
            }

            otherPoints.emplace(i, temp);
        }

        // 保存所有的三角形，可能含有直线，三角面的法线可能朝内也可能朝外
        std::vector<std::array<int,3>> triangles;
        for (size_t i = 0; i < otherPoints.size(); ++i)
        {
            if (otherPoints[i].size() > 2)
            {
                for (size_t j = 0; j < otherPoints[i].size() - 1; ++j)
                {
                    for (size_t m = j + 1; m < otherPoints[i].size(); ++m)
                    {
                        std::array<int, 3> triangle{ i,otherPoints[i][j],otherPoints[i][m] };
                        triangles.emplace_back(triangle);
                    }
                }
            }
            else if (otherPoints[i].size() == 2)
            {
                // 只有两个点和该点可以组成线段，那么最多只能有一个三角形
                triangles.emplace_back(std::array<int, 3>{ (int)i, otherPoints[i][0], otherPoints[i][1] });
            }
        }

        vtkNew<vtkCellArray> triangleCells;
        for (size_t i = 0; i < triangles.size(); ++i)
        {
            std::cout << triangles[i][0] << '\t' << triangles[i][1] << '\t' << triangles[i][2] << std::endl;
            triangleCells->InsertNextCell({ triangles[i][0],triangles[i][1],triangles[i][2] });
        }
        
        vtkNew<vtkPolyData> trianglePoly;
        trianglePoly->SetPoints(points);
        trianglePoly->SetPolys(triangleCells);
        trianglePoly->GetPointData()->SetScalars(scalars);

        return trianglePoly;
        //return outPolyData;
        //return polyData;
    }
}

int main(int argc, char* argv[])
{
    auto polyData = GenPolyData();

    vtkNew<vtkLookupTable> surfaceLUT;
    surfaceLUT->SetRange(polyData->GetPointData()->GetScalars()->GetRange());
    surfaceLUT->Build();

    //vtkNew<vtkContourTriangulator> poly;
    //poly->SetInputData(polyData);
    //vtkCellArray* outputPoly = nullptr;
    //vtkPolyData* polydata = nullptr;

    //vtkContourTriangulator::TriangulateContours(polydata, 0, 24, outputPoly, nullptr);
    ////poly->TriangulatePolygon();

    //poly->Update();

    vtkNew<vtkPolyDataMapper> polyMapper;
    //polyMapper->SetInputConnection(poly->GetOutputPort());
    polyMapper->SetInputData(polyData);
    polyMapper->ScalarVisibilityOn();
    polyMapper->SetScalarRange(polyData->GetPointData()->GetScalars()->GetRange());
    polyMapper->SetLookupTable(surfaceLUT);

    vtkNew<vtkActor> polyActor;
    polyActor->SetMapper(polyMapper);
    //polyActor->GetProperty()->SetColor(1,0,0);

    // Standard rendering classes
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetWindowName("Fill all minimum triangles");

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renderer->AddActor(polyActor);

    renderer->SetBackground(.1,.2,.3);
    renWin->SetSize(800, 600);

    renWin->Render();
    iren->Initialize();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST29

#ifdef TEST30

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDelaunay2D.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>

namespace
{
    std::vector<float> vertices{
        0.,0.,
        0.,1.,
        0.,2.,
        0.,3.,

        1.,3.,
        1.,2.,
        1.,1.,
        1.,0.,

        2.,0.,
        2.,1.,
        2.,2.,
        2.,3.,

        3.,3.,
        3.,2.,
        3.,1.,
        3.,0.
    };

    std::vector<float> fields{
        4.0f,
        4.0f,
        4.0f,
        4.0f,

        4.0f,
        1.0f,
        1.0f,
        4.0f,

        4.0f,
        1.0f,
        1.0f,
        4.0f,

        4.0f,
        4.0f,
        4.0f,
        4.0f,
    };

    vtkSmartPointer<vtkPolyData> GenPolyData()
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;

        for (size_t i = 0; i < vertices.size(); i += 2)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], 0.0);
        }
        polyData->SetPoints(points);
        //polyData->SetLines(cellsLine);

        vtkNew<vtkFloatArray> scalars;
        for (size_t i = 0; i < 16; ++i)
        {
            scalars->InsertNextValue(fields[i]);
        }

        polyData->GetPointData()->SetScalars(scalars);

        return polyData;
    }
}

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    auto aPolyData = GenPolyData();

    vtkNew<vtkLookupTable> surfaceLUT;
    surfaceLUT->SetRange(aPolyData->GetPointData()->GetScalars()->GetRange());
    surfaceLUT->Build();

    // Triangulate the grid points
    vtkNew<vtkDelaunay2D> delaunay;
    delaunay->SetInputData(aPolyData);
    delaunay->Update();

    auto polyData = delaunay->GetOutput();
    auto pointsNum = polyData->GetNumberOfPoints();
    auto cellNum = polyData->GetNumberOfCells();

    // Visualize
    vtkNew<vtkPolyDataMapper> meshMapper;
    meshMapper->InterpolateScalarsBeforeMappingOn();
    meshMapper->ScalarVisibilityOn();
    meshMapper->SetInputConnection(delaunay->GetOutputPort());
    meshMapper->SetScalarRange(aPolyData->GetPointData()->GetScalars()->GetRange());
    meshMapper->SetLookupTable(surfaceLUT);

    vtkNew<vtkActor> meshActor;
    meshActor->SetMapper(meshMapper);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actor to the scene
    renderer->AddActor(meshActor);

    renderer->SetBackground(colors->GetColor3d("Mint").GetData());

    // Render and interact
    renderWindow->SetSize(640, 480);
    renderWindow->SetWindowName("ConstrainedDelaunay2D");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST30

#ifdef TEST31

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDelaunay2D.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkCellArray.h>

namespace
{
    std::vector<float> vertices{
        0.,0.,
        0.,1.,
        0.,2.,
        0.,3.,

        1.,3.,
        1.,2.,
        1.,1.,
        1.,0.,

        2.,0.,
        2.,1.,
        2.,2.,
        2.,3.,

        3.,3.,
        3.,2.,
        3.,1.,
        3.,0.
    };

    std::vector<int> triangle{
        0,1,7,
        7,1,6,
        1,2,5,
        1,5,6,
        2,3,4,
        2,4,5,

        5,4,10,
        10,4,11,
        6,5,9,
        9,5,10,
        7,6,8,
        8,6,9,

        8,9,14,
        8,14,15,
        9,10,13,
        9,13,14,
        10,11,13,
        13,11,12
    };

    std::vector<int> quad{
        0,1,6,7,
        1,2,5,6,
        2,3,4,5,

        5,4,11,10,
        6,5,10,9,
        7,6,9,8,

        8,9,14,15,
        9,10,13,14,
        10,11,12,13
    };

    std::vector<float> fields{
        4.0f,
        4.0f,
        4.0f,
        4.0f,

        4.0f,
        1.0f,
        1.0f,
        4.0f,

        4.0f,
        1.0f,
        1.0f,
        4.0f,

        4.0f,
        4.0f,
        4.0f,
        4.0f,
    };

    vtkSmartPointer<vtkPolyData> GenPolyData()
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cellTriangle;
        vtkNew<vtkCellArray> cellPoly;

        for (size_t i = 0; i < vertices.size(); i += 2)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], 0.0);
        }
        for (size_t i = 0; i < triangle.size(); i += 3)
        {
            cellTriangle->InsertNextCell({ triangle[i],triangle[i + 1], triangle[i + 2] });
        }
        for (size_t i = 0; i < quad.size(); i += 4)
        {
            cellPoly->InsertNextCell({ quad[i],quad[i + 1],quad[i + 2],quad[i + 3] });
        }

        polyData->SetPoints(points);
        polyData->SetPolys(cellPoly);
        //polyData->SetPolys(cellTriangle);

        vtkNew<vtkFloatArray> scalars;
        for (size_t i = 0; i < 16; ++i)
        {
            scalars->InsertNextValue(fields[i]);
        }

        polyData->GetPointData()->SetScalars(scalars);

        return polyData;
    }
}

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    auto aPolyData = GenPolyData();

    vtkNew<vtkLookupTable> surfaceLUT;
    surfaceLUT->SetRange(aPolyData->GetPointData()->GetScalars()->GetRange());
    surfaceLUT->Build();

    // Triangulate the grid points
    //vtkNew<vtkDelaunay2D> delaunay;
    //delaunay->SetInputData(aPolyData);
    //delaunay->Update();

    //auto polyData = delaunay->GetOutput();
    //auto pointsNum = polyData->GetNumberOfPoints();
    //auto cellNum = polyData->GetNumberOfCells();

    // Visualize
    vtkNew<vtkPolyDataMapper> meshMapper;
    meshMapper->InterpolateScalarsBeforeMappingOn();
    meshMapper->ScalarVisibilityOn();
    //meshMapper->SetInputConnection(delaunay->GetOutputPort());
    meshMapper->SetInputData(aPolyData);
    meshMapper->SetScalarRange(aPolyData->GetPointData()->GetScalars()->GetRange());
    meshMapper->SetLookupTable(surfaceLUT);

    vtkNew<vtkActor> meshActor;
    meshActor->SetMapper(meshMapper);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actor to the scene
    renderer->AddActor(meshActor);

    renderer->SetBackground(colors->GetColor3d("Mint").GetData());

    // Render and interact
    renderWindow->SetSize(640, 480);
    renderWindow->SetWindowName("ConstrainedDelaunay2D");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST31

#ifdef TEST32

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDelaunay2D.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    // Generate a 10 x 10 grid of points
    vtkNew<vtkPoints> points;
    unsigned int gridSize = 10;
    unsigned int seed = 0;
    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->Initialize(seed);
    for (unsigned int x = 0; x < gridSize; x++)
    {
        for (unsigned int y = 0; y < gridSize; y++)
        {
            auto d1 = randomSequence->GetValue() / 2.0 - 0.25;
            randomSequence->Next();
            auto d2 = randomSequence->GetValue() / 2.0 - 0.25;
            randomSequence->Next();
            points->InsertNextPoint(x + d1, y + d2, 0);
        }
    }

    vtkNew<vtkPolyData> aPolyData;
    aPolyData->SetPoints(points);

    // Create a cell array to store the polygon in
    vtkNew<vtkCellArray> aCellArray;

    // Define a polygonal hole with a clockwise polygon
    vtkNew<vtkPolygon> aPolygon;

    aPolygon->GetPointIds()->InsertNextId(22);
    aPolygon->GetPointIds()->InsertNextId(23);
    aPolygon->GetPointIds()->InsertNextId(24);
    aPolygon->GetPointIds()->InsertNextId(25);
    aPolygon->GetPointIds()->InsertNextId(35);
    aPolygon->GetPointIds()->InsertNextId(45);
    aPolygon->GetPointIds()->InsertNextId(44);
    aPolygon->GetPointIds()->InsertNextId(43);
    aPolygon->GetPointIds()->InsertNextId(42);
    aPolygon->GetPointIds()->InsertNextId(32);

    aCellArray->InsertNextCell(aPolygon);

    // Create a polydata to store the boundary. The points must be the
    // same as the points we will triangulate.
    vtkNew<vtkPolyData> boundary;
    boundary->SetPoints(aPolyData->GetPoints());
    boundary->SetPolys(aCellArray);

    // Triangulate the grid points
    vtkNew<vtkDelaunay2D> delaunay;
    delaunay->SetAlpha(1.0);
    delaunay->SetInputData(aPolyData);
    delaunay->SetSourceData(boundary);

    // Visualize
    vtkNew<vtkPolyDataMapper> meshMapper;
    meshMapper->SetInputConnection(delaunay->GetOutputPort());

    vtkNew<vtkActor> meshActor;
    meshActor->SetMapper(meshMapper);
    //meshActor->GetProperty()->EdgeVisibilityOn();
    meshActor->GetProperty()->SetEdgeColor(1, 0, 0);
    //meshActor->GetProperty()->SetInterpolationToFlat();

    vtkNew<vtkPolyDataMapper> boundaryMapper;
    boundaryMapper->SetInputData(boundary);

    vtkNew<vtkActor> boundaryActor;
    boundaryActor->SetMapper(boundaryMapper);
    boundaryActor->GetProperty()->SetColor(0, 1, 0);
    //boundaryActor->GetProperty()->SetLineWidth(3);
    //boundaryActor->GetProperty()->EdgeVisibilityOn();
    //boundaryActor->GetProperty()->SetEdgeColor(1, 1, 0);
    //boundaryActor->GetProperty()->SetRepresentationToWireframe();

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actor to the scene
    renderer->AddActor(meshActor);
    //renderer->AddActor(boundaryActor);
    renderer->SetBackground(.1, .2, .3);

    // Render and interact
    renderWindow->SetSize(640, 480);
    renderWindow->SetWindowName("ConstrainedDelaunay2D");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST32

#ifdef TEST33

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkCaptionRepresentation.h>
#include <vtkCaptionWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    // Sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(
        colors->GetColor3d("DarkOliveGreen").GetData());

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("CaptionWidget");

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Create the widget and its representation
    vtkNew<vtkCaptionRepresentation> captionRepresentation;
    captionRepresentation->GetCaptionActor2D()->SetCaption("Test caption");
    captionRepresentation->GetCaptionActor2D()
        ->GetTextActor()
        ->GetTextProperty()
        ->SetFontSize(100);

    double pos[3] = { .5, 0, 0 };
    captionRepresentation->SetAnchorPosition(pos);

    vtkNew<vtkCaptionWidget> captionWidget;
    captionWidget->SetInteractor(renderWindowInteractor);
    captionWidget->SetRepresentation(captionRepresentation);

    // Add the actors to the scene
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("Blue").GetData());

    renderWindow->Render();

    // Rotate the camera to bring the point the caption is pointing to into view.
    renderer->GetActiveCamera()->Azimuth(90);

    captionWidget->On();

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST33

#ifdef TEST34

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkElevationFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSuperEllipsoid.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProjectSphereFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkParametricSuperEllipsoid> surface;
    surface->SetN1(2.0);
    surface->SetN2(0.5);

    vtkNew<vtkParametricFunctionSource> source;
    source->SetParametricFunction(surface);

    vtkNew<vtkElevationFilter> elevationFilter;
    elevationFilter->SetInputConnection(source->GetOutputPort());
    elevationFilter->SetLowPoint(0.0, 0.0, -4.0);
    elevationFilter->SetHighPoint(0.0, 0.0, 4.0);
    elevationFilter->Update();

    // Deep copy the point data since in some versions of VTK,
    // the ProjectSphereFilter modifies the input point data
    vtkNew<vtkPolyData> pd1;
    pd1->DeepCopy(elevationFilter->GetOutput());

    vtkNew<vtkProjectSphereFilter> sphereProject1;
    sphereProject1->SetInputConnection(elevationFilter->GetOutputPort());
    sphereProject1->Update();

    vtkNew<vtkPolyDataMapper> mapper1;
    mapper1->SetInputConnection(sphereProject1->GetOutputPort());
    mapper1->SetScalarRange(
        sphereProject1->GetOutput()->GetPointData()->GetScalars()->GetRange());
    vtkNew<vtkActor> actor1;
    actor1->SetMapper(mapper1);

    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(pd1);
    mapper2->SetScalarRange(pd1->GetPointData()->GetScalars()->GetRange());
    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);

    // A render window
    vtkNew<vtkRenderWindow> renderWindow;

    // Define viewport ranges
    // (xmin, ymin, xmax, ymax)
    double leftViewport[4] = { 0.0, 0.0, 0.5, 1.0 };
    double rightViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

    // Setup both renderers
    vtkNew<vtkRenderer> leftRenderer;
    renderWindow->AddRenderer(leftRenderer);
    leftRenderer->SetViewport(leftViewport);
    leftRenderer->SetBackground(colors->GetColor3d("RosyBrown").GetData());

    vtkNew<vtkRenderer> rightRenderer;
    renderWindow->AddRenderer(rightRenderer);
    rightRenderer->SetViewport(rightViewport);
    rightRenderer->SetBackground(colors->GetColor3d("CadetBlue").GetData());

    leftRenderer->AddActor(actor2);
    rightRenderer->AddActor(actor1);

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    leftRenderer->GetActiveCamera()->Azimuth(30);
    leftRenderer->GetActiveCamera()->Elevation(-30);
    leftRenderer->ResetCamera();

    // Render an image (lights and cameras are created automatically)
    renderWindow->SetSize(640, 480);
    renderWindow->SetWindowName("ProjectSphere");

    renderWindow->Render();

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST34

#ifdef TEST35

#include <vtkSmartPointer.h>
#include <vtkFeatureEdges.h>
#include <vtkRenderWindow.h>
#include <vtkSTLReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkLookupTable.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCellPicker.h>
#include <vtkSphereSource.h>

#include <string>



class DesignInteractorStyle : 
    public vtkInteractorStyleTrackballCamera
{
public:
    static DesignInteractorStyle* New()
    {
        return new DesignInteractorStyle;
    }
    vtkTypeMacro(DesignInteractorStyle, vtkInteractorStyleTrackballCamera);

    DesignInteractorStyle()
    {
        sphereActor = vtkSmartPointer<vtkActor>::New();
    }

    virtual void OnLeftButtonDown() {
        if (!this->Interactor)
        {
            return;
        }

        Press = 1;
        int PressFlag = 1;

        int* pEvtPos = this->Interactor->GetEventPosition();
        this->Interactor->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);

        vtkNew<vtkCellPicker> vtkCurPicker;
        this->Interactor->SetPicker(vtkCurPicker);
        this->Interactor->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, this->CurrentRenderer);
        double Position[3];
        vtkCurPicker->GetPickPosition(Position);
        int TriID = vtkCurPicker->GetCellId();

        m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, PressFlag);
        m_polyData->GetCellData()->Modified();
        m_polyData->GetCellData()->GetScalars()->Modified();

        this->Interactor->Render();
    }
    virtual void OnLeftButtonUp() {}
    virtual void OnRightButtonDown() { this->StartRotate(); } // 避免vtk的GrabFocus接口占用交互命令
    virtual void OnRightButtonUp() { this->vtkInteractorStyleTrackballCamera::OnLeftButtonUp(); }

    virtual void OnMouseMove() {
        if (!this->Interactor)
        {
            return;
        }
        if (EditMode == 0) {

            int* pEvtPos = this->Interactor->GetEventPosition();
            this->Interactor->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);

            vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
            this->Interactor->SetPicker(vtkCurPicker);
            this->Interactor->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, this->CurrentRenderer);
            double Position[3];
            vtkCurPicker->GetPickPosition(Position);
            int TriID = vtkCurPicker->GetCellId();
            if (TriID == -1) return;

            vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
            sphere->SetCenter(Position);
            sphere->SetRadius(Radius);
            sphere->SetPhiResolution(36);
            sphere->SetThetaResolution(36);
            sphere->Update();

            vtkSmartPointer<vtkPolyDataMapper> spheremapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            spheremapper->SetInputConnection(sphere->GetOutputPort());
            sphereActor->SetMapper(spheremapper);
            sphereActor->GetProperty()->SetOpacity(0.2);
            sphereActor->GetProperty()->SetColor(lut->GetTableValue(PressFlag));
            sphereActor->PickableOff();
            this->CurrentRenderer->AddActor(sphereActor);
            this->Interactor->Render();

            if (Press == 0)return;

            BFS(Position, TriID);

            m_polyData->GetCellData()->Modified();
            m_polyData->GetCellData()->GetScalars()->Modified();
            this->Interactor->Render();
        }
        if (EditMode == 1)
        {
            int* pEvtPos = this->Interactor->GetEventPosition();
            this->Interactor->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);

            vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
            this->Interactor->SetPicker(vtkCurPicker);
            this->Interactor->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, this->CurrentRenderer);
            int TriID = vtkCurPicker->GetCellId();
            if (TriID == -1) return;
            if (Press == 0)return;
            m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, PressFlag);
            m_polyData->GetCellData()->GetScalars()->Modified();
            this->Interactor->Render();
        }
    }
    virtual void OnMouseWheelForward() { if (!this->Interactor->GetControlKey() && !this->Interactor->GetShiftKey()) this->vtkInteractorStyleTrackballCamera::OnMouseWheelForward(); }
    virtual void OnMouseWheelBackward() { if (!this->Interactor->GetControlKey() && !this->Interactor->GetShiftKey()) this->vtkInteractorStyleTrackballCamera::OnMouseWheelBackward(); }

    void SetPolyData(vtkSmartPointer<vtkPolyData> _polyData)
    {
        m_polyData = _polyData;
    }
    void SetLookUpTable(vtkSmartPointer<vtkLookupTable> _lut)
    {
        lut = _lut;
    }
private:
    void BFS(double* Position, int TriID)
    {
        if (CellInSphere(Position, TriID) == 0) return;
        if (static_cast<int>(m_polyData->GetCellData()->GetScalars()->GetTuple1(TriID)) == PressFlag) return;
        m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, PressFlag);
        auto id0 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(0);
        auto id1 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(1);
        auto id2 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(2);
        vtkNew<vtkIdList> idlist0;
        m_polyData->GetPointCells(id0, idlist0);
        vtkNew<vtkIdList> idlist1;
        m_polyData->GetPointCells(id1, idlist1);
        vtkNew<vtkIdList> idlist2;
        m_polyData->GetPointCells(id2, idlist2);
        for (int i = 0; i < idlist0->GetNumberOfIds(); i++)
            BFS(Position, idlist0->GetId(i));
        for (int i = 0; i < idlist1->GetNumberOfIds(); i++)
            BFS(Position, idlist1->GetId(i));
        for (int i = 0; i < idlist2->GetNumberOfIds(); i++)
            BFS(Position, idlist2->GetId(i));
        return;
    }

    bool CellInSphere(double* Position, int TriID)
    {
        auto pt0 = m_polyData->GetCell(TriID)->GetPoints()->GetPoint(0);
        auto pt1 = m_polyData->GetCell(TriID)->GetPoints()->GetPoint(1);
        auto pt2 = m_polyData->GetCell(TriID)->GetPoints()->GetPoint(2);
        //auto f0 = vtkMath::Distance2BetweenPoints(Position, pt0) < Radius ? 1 : 0;
        if (vtkMath::Distance2BetweenPoints(Position, pt0) < Radius)
            return 1;
        if (vtkMath::Distance2BetweenPoints(Position, pt1) < Radius)
            return 1;
        if (vtkMath::Distance2BetweenPoints(Position, pt2) < Radius)
            return 1;
        //auto f1 = vtkMath::Distance2BetweenPoints(Position, pt1) > Radius ? 0 : 1;
        //auto f2 = vtkMath::Distance2BetweenPoints(Position, pt2) > Radius ? 0 : 1;
        return 0;
    }
private:
    vtkSmartPointer<vtkPolyData> m_polyData{ nullptr };
    vtkSmartPointer<vtkActor> sphereActor{ nullptr };
    vtkSmartPointer<vtkLookupTable> lut;
    int Press{0};
    int EditMode{0};
    int PressFlag{0};
    double Radius = 2.5;
};

int main()
{
    std::string inputFileName = "resource/test2.stl";

    vtkNew<vtkSTLReader> STLReader;
    STLReader->SetFileName(inputFileName.c_str());
    STLReader->Update();

    auto polydata = STLReader->GetOutput();

    vtkNew<vtkFeatureEdges> featureEdges;
    featureEdges->SetInputData(polydata);
    featureEdges->BoundaryEdgesOff();
    featureEdges->FeatureEdgesOn();
    featureEdges->SetFeatureAngle(20);
    featureEdges->ManifoldEdgesOff();
    featureEdges->NonManifoldEdgesOff();
    featureEdges->ColoringOff();
    featureEdges->Update();

    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputConnection(featureEdges->GetOutputPort());

    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->SetColor(1, 0, 0);
    edgeActor->GetProperty()->SetLineWidth(3.0);
    edgeActor->GetProperty()->SetRenderLinesAsTubes(0.5);
    edgeActor->PickableOff();

    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(20);
    lut->Build();

    double white[4] = { 1.0,1.0,1.0,1.0 };
    lut->SetTableValue(0, white);
    vtkNew<vtkFloatArray> cellData;
    for (int i = 0; i < polydata->GetNumberOfCells(); i++)
        cellData->InsertTuple1(i, 0);
    polydata->GetCellData()->SetScalars(cellData);
    polydata->GetCellData()->GetScalars()->Print(std::cout);
    polydata->BuildLinks();

    vtkNew<vtkPolyDataMapper> polydataMapper;
    polydataMapper->SetInputData(polydata);
    polydataMapper->SetScalarRange(0, 19);
    polydataMapper->SetLookupTable(lut);
    polydataMapper->Update();

    vtkNew<vtkActor> polydataActor;
    polydataActor->GetProperty()->EdgeVisibilityOff();
    polydataActor->SetMapper(polydataMapper);
    polydataActor->GetProperty()->SetOpacity(1);

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor(edgeActor);
    renderer->AddActor(polydataActor);

    vtkNew<vtkRenderWindow> window;
    window->SetSize(800, 600);
    window->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> interactor;
    vtkNew<DesignInteractorStyle> vtkStyle;
    interactor->SetInteractorStyle(vtkStyle);
    interactor->SetRenderWindow(window);

    //zxk::initCallbackCommand();


    //vtkInter->AddObserver(vtkCommand::LeftButtonPressEvent, LeftButtonPressCallback);
    //vtkInter->AddObserver(vtkCommand::LeftButtonReleaseEvent, LeftButtonReleaseCallback);
    //vtkInter->AddObserver(vtkCommand::MiddleButtonPressEvent, MiddleButtonPressCallback);
    //vtkInter->AddObserver(vtkCommand::MiddleButtonReleaseEvent, MiddleButtonReleaseCallback);
    //vtkInter->AddObserver(vtkCommand::RightButtonPressEvent, RightButtonPressCallback);
    //vtkInter->AddObserver(vtkCommand::RightButtonReleaseEvent, RightButtonReleaseCallback);
    //vtkInter->AddObserver(vtkCommand::KeyPressEvent, KeyPressCallback);
    //vtkInter->AddObserver(vtkCommand::KeyReleaseEvent, KeyReleaseCallback);
    //vtkInter->AddObserver(vtkCommand::MouseMoveEvent, MouseMoveCallback);
    //vtkInter->AddObserver(vtkCommand::MouseWheelForwardEvent, MouseWheelForwardCallback);
    //vtkInter->AddObserver(vtkCommand::MouseWheelBackwardEvent, MouseWheelBackwardCallback);

    //vtkInter->AddObserver(vtkCommand::KeyPressEvent, );


    window->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}
#endif // TEST35

#ifdef TEST36

// 01_02_TEST17

#include <vtkArrowSource.h>

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkVector.h>
#include <vtkCleanPolyData.h>
#include <vtkAlgorithmOutput.h>

std::vector<float> vecPoints{
    // ===1===
    //0.0, 0.0, 0.0,
    //1.0, 1.0, 0.0,
    //1.0, 0.0, 0.0,
    //0.0, 1.0, 0.0,

    // ===2===
    100,200,100,
    500,200,100,
    0,0,0,
    0.1,0.001,100,
};

std::vector<float> vecScalars{
    1.0f,
    2.0f,
    3.0f,
    4.0f,
};
std::vector<float> vecNormals{
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    1.0, 1.0, 1.0,
    0.0, 0.0, 0.0,
};
std::vector<float> vecVectors{
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    1.0, 1.0, 1.0,
    0.0, 0.0, 1.0,
};

int main(int, char* [])
{
    vtkNew<vtkPoints> points;
    for (size_t i = 0; i < vecPoints.size(); i += 3)
    {
        points->InsertNextPoint(vecPoints[i], vecPoints[i + 1], vecPoints[i + 2]);
    }

    // Add the points to a polydata
    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);
    auto bounds = polydata->GetBounds(); // 返回polyData在xyz上的范围
    auto length = polydata->GetLength(); // 返回包围盒对角线的长度
    std::cout << "bounds:\t" <<bounds[0] << '\t' << bounds[1] << '\t' << bounds[2] << '\t' << bounds[3]
        << '\t' << bounds[4] << '\t' << bounds[5] << "\tlength:\t" << length << '\n';

    vtkNew<vtkFloatArray> scalars;
    vtkNew<vtkFloatArray> vectors;
    vtkNew<vtkFloatArray> normals;
    vectors->SetNumberOfComponents(3);
    normals->SetNumberOfComponents(3);

    for (size_t i = 0; i < vecScalars.size(); i++)
    {
        scalars->InsertNextValue(vecScalars[i]);
    }
    for (size_t i = 0; i < vecVectors.size(); i += 3)
    {
        vectors->InsertNextTuple3(vecVectors[i], vecVectors[i + 1], vecVectors[i + 2]);
    }
    for (size_t i = 0; i < vecNormals.size(); i += 3)
    {
        normals->InsertNextTuple3(vecNormals[i], vecNormals[i + 1], vecNormals[i + 2]);
    }

    polydata->GetPointData()->SetScalars(scalars); // 标量可以控制箭头的大小以及颜色
    polydata->GetPointData()->SetNormals(normals); // 法线可以控制方向
    polydata->GetPointData()->SetVectors(vectors); // 向量可以控制箭头的颜色，方向，大小

    vtkNew<vtkArrowSource> arrowSource;
    arrowSource->SetShaftRadius(0.01);
    arrowSource->SetTipRadius(0.05);
    arrowSource->SetTipLength(0.2);

    vtkNew<vtkGlyph2D> glyph3D;
    glyph3D->SetSourceConnection(arrowSource->GetOutputPort());
    auto colorMode = glyph3D->GetColorModeAsString();    // "ColorByScale"
    auto scaleMode = glyph3D->GetScaleModeAsString();    // "ScaleByScalar"
    auto vectorMode = glyph3D->GetVectorModeAsString();  // "UseVector"
    auto scaleFactor = glyph3D->GetScaleFactor();        // 1.0
    std::cout << "Befor mode:\t" << colorMode << '\t' << scaleMode << '\t' << vectorMode << '\t' << scaleFactor << '\n';

    // 比例（大小）
    //glyph3D->SetScaleModeToDataScalingOff(); // 关闭由于数据（标量或向量）导致的缩放
    //glyph3D->SetScaleModeToScaleByScalar();  // 使用标量数据设置箭头的大小
    //glyph3D->SetScaleModeToScaleByVector();   // 使用向量的长度设置箭头的大小
    //glyph3D->SetScaleModeToScaleByVectorComponents();

    // 颜色
    //glyph3D->SetColorModeToColorByScalar();
    //glyph3D->SetColorModeToColorByVector(); // 使用向量映射颜色（不再需要设置scalerRange）
    //glyph3D->SetColorModeToColorByScale();  // 使用标量控制颜色（类似云图颜色映射）

    // 方向
    //glyph3D->SetVectorModeToVectorRotationOff();     // 关闭法线和向量导致的方向更改
    //glyph3D->SetVectorModeToUseNormal();             // 使用法线设置方向
    //glyph3D->SetVectorModeToUseVector();             // 使用向量设置方向
    //glyph3D->SetVectorModeToFollowCameraDirection();

    // 比例因子（控制大小）,包围盒长度会影响箭头大小，所以比例因子应该根据包围盒长度动态设置
    // 上面的比例，颜色，方向全部注释，顶点使用第二组数据，比例因子过小就不能显示
    //glyph3D->SetScaleFactor(.5); // 设置缩放比例，默认为1
    glyph3D->SetScaleFactor(length);

    glyph3D->SetInputData(polydata);
    glyph3D->Update();

    auto colorMode1 = glyph3D->GetColorModeAsString();    // "ColorByScale"
    auto scaleMode1 = glyph3D->GetScaleModeAsString();    // "ScaleByScalar"
    auto vectorMode1 = glyph3D->GetVectorModeAsString();  // "UseVector"
    auto scaleFactor1 = glyph3D->GetScaleFactor();        // 1.0
    std::cout << "After mode:\t" << colorMode1 << '\t' << scaleMode1 << '\t' << vectorMode1 << '\t' << scaleFactor1 << '\n';

    // color map
    vtkNew<vtkLookupTable> pColorTable;
    pColorTable->SetNumberOfColors(10);
    pColorTable->SetHueRange(.0, .67);
    pColorTable->Build();

    // Visualize
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(glyph3D->GetOutputPort());
    mapper->SetLookupTable(pColorTable);
    // 如果添加的是向量，获取的范围就是向量的【模长】最大最小值
    auto range = mapper->GetInput()->GetScalarRange();
    mapper->SetScalarRange(range);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->ResetCamera();
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    renderWindow->SetWindowName("Glyph3D");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST36