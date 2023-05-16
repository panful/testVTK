
/*
 * 1. vbo ibo DrawCall
 * 2.
 * 3. 调试vtk源码
 * 6. 交叉图形的混合透明度
 */

#define TEST2

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

创建VBO IBO
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
 vtkOpenGLQuadHelper.cxx                [109]   glDrawArrays() 将渲染的结果（FBO）绘制到屏幕上

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
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1, 0, 0);

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

#endif // TEST2

#ifdef TEST3

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>

// 加载顶点拓扑数据等
// vtkOpenGLBufferObject.cxx bool vtkOpenGLBufferObject::UploadInternal()

// 设置状态，点的大小，线的粗细，深度测试等等
// vtkOpenGLState.cxx

// 设置uniform等
// vtkShaderProgram.cxx

// 编译shader
// vtkShader.cxx

// FBO
// vtkOpenGLFrameBufferObject.cxx

int main()
{
    vtkNew<vtkNamedColors> colors;

    std::array<std::array<float, 3>, 8> pts = { { { { 0, 0, 0 } }, { { 1, 0, 0 } }, { { 1, 1, 0 } }, { { 0, 1, 0 } }, { { 0, 0, 1 } },
        { { 1, 0, 1 } }, { { 1, 1, 1 } }, { { 0, 1, 1 } } } };
    // The ordering of the corner points on each face.
    std::array<std::array<vtkIdType, 4>, 6> ordering
        = { { { { 0, 1, 2, 3 } }, { { 4, 5, 6, 7 } }, { { 0, 1, 5, 4 } }, { { 1, 2, 6, 5 } }, { { 2, 3, 7, 6 } }, { { 3, 0, 4, 7 } } } };

    // We'll create the building blocks of polydata including data attributes.
    vtkNew<vtkPolyData> cube;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> polys;
    vtkNew<vtkFloatArray> scalars;

    // Load the point, cell, and data attributes.
    for (auto i = 0ul; i < pts.size(); ++i)
    {
        points->InsertPoint(i, pts[i].data());
        scalars->InsertTuple1(i, i);
    }
    for (auto&& i : ordering)
    {
        polys->InsertNextCell(vtkIdType(i.size()), i.data());
    }

    // We now assign the pieces to the vtkPolyData.
    cube->SetPoints(points);
    cube->SetPolys(polys);
    cube->GetPointData()->SetScalars(scalars);

    // Now we'll look at it.
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(cube);
    cubeMapper->SetScalarRange(cube->GetScalarRange());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);

    // The usual rendering stuff.
    // vtkNew<vtkCamera> camera;
    // camera->SetPosition(1, 1, 1);
    // camera->SetFocalPoint(0, 0, 0);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renderer->AddActor(cubeActor);
    // renderer->SetActiveCamera(camera);
    renderer->ResetCamera();
    renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());

    renWin->SetSize(600, 600);

    // interact with data
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST3

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
