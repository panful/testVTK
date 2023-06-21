
/*
 * 1. 整个渲染的流程 生成vbo和ibo DrawCall
 * 2. shader源码的生成、修改、编译、uniform的设置
 * 3.
 * 4.
 * 5.
 * 6. 交叉图形的混合透明度
 */

#define TEST3

#ifdef TEST1

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <iostream>

/*
 vtkRenderWindow::Render()->vtkRenderer::Render()->vtkActor::Render()->vtkMapper::Render()

 vtkOpenGLRenderer.cxx                  [298]   UpdateGeometry()
 vtkOpenGLPolyDataMapper.cxx            [3845]  BuildBufferObjects()

创建VBO IBO，加载顶点、拓扑数据
 vtkOpenGLVertexBufferObjectGroup.cxx   [322]   BuildAllVBOs()
 vtkOpenGLBufferObject.cxx              [137]   UploadInternal()  glBufferData

根据vtkPolyData的Cell类型创建IBO
 vtkOpenGLPolyDataMapper.cxx            [4002]  BuildIBO()
 vtkOpenGLIndexBufferObject.cxx                 CreateTriangleIndexBuffer()
                                                CreatePointIndexBuffer()
                                                CreateTriangleLineIndexBuffer
                                                CreateLineIndexBuffer CreateStripIndexBuffer
                                                CreateVertexIndexBuffer
                                                CreateEdgeFlagIndexBuffer
 vtkOpenGLBufferObject.cxx              [137]   UploadInternal()  glBufferData

调用DrawCall
 vtkOpenGLPolyDataMapper.cxx            [3444]  RenderPieceDraw()   glDrawRangeElements() 绘制点、线、三角面

离屏渲染
 vtkRenderWindow.cxx                    [547]   CopyResultFrame()
 vtkOpenGLRenderWindow.cxx              [1110]  Frame() 将渲染的结果（FBO）提交给窗口FBO对应的着色器
 vtkOpenGLQuadHelper.cxx                [109]   glDrawArrays() 将渲染的结果（铺满屏幕的quad）绘制到屏幕上

设置状态，点的大小，线的粗细，开启关闭深度测试、开启关闭混合等
 vtkOpenGLState.cxx

shader文件
 .../Rendering/OpenGL2/glsl
*/

int main()
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(10., 0., 0.);
    points->InsertNextPoint(5., 10., 0.);

    cells->InsertNextCell({ 0, 1, 2 });

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // interactor syle
    vtkNew<vtkInteractorStyleRubberBand3D> style;
    iren->SetInteractorStyle(style);

    // render
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkOpenGLFramebufferObject.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkPixelBufferObject.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShaderProperty.h>

#include <iostream>

namespace {

class MyStyle : public vtkInteractorStyleRubberBand3D
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleRubberBand3D);

    void OnLeftButtonUp() override
    {
        Superclass::OnLeftButtonUp();
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

/*
 * 生成shader vtkOpenGLPolyDataMapper.cxx [350] vtkOpenGLPolyDataMapper::BuildShaders
 * 根据vtkActor的属性生成指定的shader vtkOpenGLPolyDataMapper.cxx [2461] vtkOpenGLPolyDataMapper::ReplaceShaderValues
 * 编译shader vtkShader.cxx [43] vtkShader::Compile()
 * 设置uniform vtkShaderProgram.cxx
 */

int main()
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(10., 0., 0.);
    points->InsertNextPoint(5., 10., 0.);

    cells->InsertNextCell({ 0, 1, 2 });

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0., 1., 0.);

    // 将片段着色器的输出设置为指定值，只是在光照后边将默认的输出覆盖掉，具体怎么设置输出还不太清楚
    // 将shader模板文件的"//VTK::Light::Impl"替换为 "//VTK::Light::Impl\nfragOutput0 = vec4(1.0, 0.0, 0.0, 1.0);\n"
    auto sp = actor->GetShaderProperty();
    sp->AddFragmentShaderReplacement("//VTK::Light::Impl", true,
        "//VTK::Light::Impl\n"
        "  fragOutput0 = vec4(1.0, 0.0, 0.0, 1.0);\n",
        false);

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->LightFollowCameraOff();
    renderer->RemoveAllLights();
    renderer->AddActor(actor);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // interactor syle
    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    // render
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST2



#ifdef TEST6

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <iostream>

/*
 * 调用glDepthMask(GL_TRUE)先渲染不透明的vtkActor，再使用顺序无关透明渲染透明的vtkActor
 * uniform opacityUniform
 * gl_FragDepth
 *
 */

int main()
{
    vtkNew<vtkRenderer> renderer;

    // 红色
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        points->InsertNextPoint(.9f, .6f, -.5f);
        points->InsertNextPoint(.9f, .5f, -.5f);
        points->InsertNextPoint(-.9f, .6f, .5f);
        points->InsertNextPoint(-.9f, .5f, .5f);
        cells->InsertNextCell({ 0, 1, 3, 2 });

        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        // mapper
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        // actor
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);
        actor->GetProperty()->SetOpacity(.5);

        renderer->AddActor(actor);
    }
    // 绿色
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        points->InsertNextPoint(-.9f, .9f, -.5f);
        points->InsertNextPoint(-.9f, .7f, -.5f);
        points->InsertNextPoint(.5f, -.7f, .5f);
        points->InsertNextPoint(.5f, -.9f, .5f);
        cells->InsertNextCell({ 0, 1, 3, 2 });

        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        // mapper
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        // actor
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0, 1, 0);
        actor->GetProperty()->SetOpacity(.5);

        renderer->AddActor(actor);
    }
    // 蓝色
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        points->InsertNextPoint(-.5f, -.7f, -.5f);
        points->InsertNextPoint(-.5f, -.9f, -.5f);
        points->InsertNextPoint(.9f, .9f, .5f);
        points->InsertNextPoint(.9f, .7f, .5f);
        cells->InsertNextCell({ 0, 1, 3, 2 });

        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        // mapper
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        // actor
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0, 0, 1);
        actor->GetProperty()->SetOpacity(.5);

        renderer->AddActor(actor);
    }
    // 上面的不透明红色
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        points->InsertNextPoint(.9f, .2f, .5f);
        points->InsertNextPoint(.9f, .1f, .5f);
        points->InsertNextPoint(-.9f, .2f, .5f);
        points->InsertNextPoint(-.9f, .1f, .5f);
        cells->InsertNextCell({ 0, 1, 3, 2 });

        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        // mapper
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        // actor
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);
        // actor->GetProperty()->SetOpacity(1.);

        renderer->AddActor(actor);
    }
    // 下面的不透明红色
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        points->InsertNextPoint(.9f, -.5f, -.5f);
        points->InsertNextPoint(.9f, -.6f, -.5f);
        points->InsertNextPoint(-.9f, -.5f, -.5f);
        points->InsertNextPoint(-.9f, -.6f, -.5f);
        cells->InsertNextCell({ 0, 1, 3, 2 });

        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        // mapper
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        // actor
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);
        // actor->GetProperty()->SetOpacity(1.0);

        renderer->AddActor(actor);
    }

    renderer->SetBackground(1, 1, 1);
    renderer->GetActiveCamera()->SetPosition(0, 0, 5);
    renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
    renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->ParallelProjectionOn();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // interactor syle
    vtkNew<vtkInteractorStyleRubberBand3D> style;
    iren->SetInteractorStyle(style);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST6
