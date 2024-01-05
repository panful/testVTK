/*
1. 写ffmpeg
2. vtkOpenGLFramebufferObject
3.
4. 写ffmpeg
5. vtkFFMPEGVideoSource
6. 
7. 切片 vtkCutter  从多边形提取线条
8. 
9. 命令模式
10.
11.获取OpenGL版本  https://gitlab.kitware.com/vtk/vtk/-/blob/v9.1.0/Rendering/OpenGL2/Testing/Cxx/TestWindowBlits.cxx
12.网格中心vtkCenterOfMass
13.
14.从输入的多个polydata获取交集并集差集vtkBooleanOperationPolyDataFilter
15.
16.网格模型的特征边与封闭性检测 https://www.cnblogs.com/ybqjymy/p/14241831.html
17.按键盘p键显示物体的包围盒，边框
18.
19.

21.vtkFeatureEdges 封闭性检测 TEST16 标注边界的类型  https://kitware.github.io/vtk-examples/site/Cxx/Meshes/BoundaryEdges/
22.
23.线条生成三角面  vtkStripper https://kitware.github.io/vtk-examples/site/Cxx/Meshes/CapClip/
24.多边形转三角形 vtkTriangleFilter https://kitware.github.io/vtk-examples/site/Cxx/PolyData/PolygonalSurfaceContourLineInterpolator/
25.让一个模型始终在其他模型的上层 重叠的面旋转时会闪烁 https://www.weiy.city/2020/03/make-model-always-on-top/
26.在曲面上画线 TEST24绘制最短路径就是用的该方法 https://kitware.github.io/vtk-examples/site/Cxx/PolyData/PolygonalSurfacePointPlacer/
27 
28 vtkStripper https://kitware.github.io/vtk-examples/site/Cxx/Visualization/LabelContours/
29.自定义方法填充线框生成面，闭合线框生成三角面(一个三角面只能由三条线构成，即生成最小的所有三角面）
30.
31.vtkCellType 获取actor的单元类型
32.
33.

42.
43 直线与图元的交点 vtkOBBTree  线与线的交点 IntersectWithLine
44 通过设置mapper让某个图元始终在最下面或者最上面显示
45.图层设置，让某个mapper始终在最上层或最下层，或中间某一层

47.
*/

#define TEST30

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

    std::string fileName = "../resources/Bearded guy.ply";
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

    const char* fileName = "../resources/Qt_Window_FFMPEG.avi";

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
    diskActor->GetProperty()->SetColor(0., 1., 0.);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("BoundaryEdges");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(edgeActor);  // 边界，红色
    renderer->AddActor(diskActor); // 原始的环状图元
    renderer->SetBackground(.1, .2, .3);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST21


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
        for (size_t i = 0; i < indicesPoly.size(); i += 4)
        {
            cellsPoly->InsertNextCell({ indicesPoly[i],indicesPoly[i + 1], indicesPoly[i + 2],indicesPoly[i + 3] });
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
    actor->GetProperty()->SetColor(1, 0, 0);

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

        std::map<int, std::vector<int>> otherPoints;

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
        std::vector<std::array<int, 3>> triangles;
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

    renderer->SetBackground(.1, .2, .3);
    renWin->SetSize(800, 600);

    renWin->Render();
    iren->Initialize();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST29



#ifdef TEST31

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

#include <vtkCellTypes.h>

#include <array>
#include <iostream>
#include "magic_enum.hpp"

#if(0)
typedef enum
{
    // Linear cells
    VTK_EMPTY_CELL = 0,
    VTK_VERTEX = 1,
    VTK_POLY_VERTEX = 2,
    VTK_LINE = 3,
    VTK_POLY_LINE = 4,
    VTK_TRIANGLE = 5,
    VTK_TRIANGLE_STRIP = 6,
    VTK_POLYGON = 7,
    VTK_PIXEL = 8,
    VTK_QUAD = 9,
    VTK_TETRA = 10,
    VTK_VOXEL = 11,
    VTK_HEXAHEDRON = 12,
    VTK_WEDGE = 13,
    VTK_PYRAMID = 14,
    VTK_PENTAGONAL_PRISM = 15,
    VTK_HEXAGONAL_PRISM = 16,

    // Quadratic, isoparametric cells
    VTK_QUADRATIC_EDGE = 21,
    VTK_QUADRATIC_TRIANGLE = 22,
    VTK_QUADRATIC_QUAD = 23,
    VTK_QUADRATIC_POLYGON = 36,
    VTK_QUADRATIC_TETRA = 24,
    VTK_QUADRATIC_HEXAHEDRON = 25,
    VTK_QUADRATIC_WEDGE = 26,
    VTK_QUADRATIC_PYRAMID = 27,
    VTK_BIQUADRATIC_QUAD = 28,
    VTK_TRIQUADRATIC_HEXAHEDRON = 29,
    VTK_TRIQUADRATIC_PYRAMID = 37,
    VTK_QUADRATIC_LINEAR_QUAD = 30,
    VTK_QUADRATIC_LINEAR_WEDGE = 31,
    VTK_BIQUADRATIC_QUADRATIC_WEDGE = 32,
    VTK_BIQUADRATIC_QUADRATIC_HEXAHEDRON = 33,
    VTK_BIQUADRATIC_TRIANGLE = 34,

    // Cubic, isoparametric cell
    VTK_CUBIC_LINE = 35,

    // Special class of cells formed by convex group of points
    VTK_CONVEX_POINT_SET = 41,

    // Polyhedron cell (consisting of polygonal faces)
    VTK_POLYHEDRON = 42,

    // Higher order cells in parametric form
    VTK_PARAMETRIC_CURVE = 51,
    VTK_PARAMETRIC_SURFACE = 52,
    VTK_PARAMETRIC_TRI_SURFACE = 53,
    VTK_PARAMETRIC_QUAD_SURFACE = 54,
    VTK_PARAMETRIC_TETRA_REGION = 55,
    VTK_PARAMETRIC_HEX_REGION = 56,

    // Higher order cells
    VTK_HIGHER_ORDER_EDGE = 60,
    VTK_HIGHER_ORDER_TRIANGLE = 61,
    VTK_HIGHER_ORDER_QUAD = 62,
    VTK_HIGHER_ORDER_POLYGON = 63,
    VTK_HIGHER_ORDER_TETRAHEDRON = 64,
    VTK_HIGHER_ORDER_WEDGE = 65,
    VTK_HIGHER_ORDER_PYRAMID = 66,
    VTK_HIGHER_ORDER_HEXAHEDRON = 67,

    // Arbitrary order Lagrange elements (formulated separated from generic higher order cells)
    VTK_LAGRANGE_CURVE = 68,
    VTK_LAGRANGE_TRIANGLE = 69,
    VTK_LAGRANGE_QUADRILATERAL = 70,
    VTK_LAGRANGE_TETRAHEDRON = 71,
    VTK_LAGRANGE_HEXAHEDRON = 72,
    VTK_LAGRANGE_WEDGE = 73,
    VTK_LAGRANGE_PYRAMID = 74,

    // Arbitrary order Bezier elements (formulated separated from generic higher order cells)
    VTK_BEZIER_CURVE = 75,
    VTK_BEZIER_TRIANGLE = 76,
    VTK_BEZIER_QUADRILATERAL = 77,
    VTK_BEZIER_TETRAHEDRON = 78,
    VTK_BEZIER_HEXAHEDRON = 79,
    VTK_BEZIER_WEDGE = 80,
    VTK_BEZIER_PYRAMID = 81,

    VTK_NUMBER_OF_CELL_TYPES
} VTKCellType;
#endif

namespace
{
    std::array<float, 9 * 3> vertices{
        0,2,0,
        1,2,0,
        2,2,0,

        0,1,0,
        1,1,0,
        2,1,0,

        0,0,0,
        1,0,0,
        2,0,0
    };

    std::array<long long, 3 * 2> topoLine{
        0,6,
        1,7,
        2,8
    };

    std::array<long long, 4 * 3> topoTriangle{
        0,3,1,
        1,5,2,
        7,8,5,
        3,6,7
    };

    std::array<long long, 4 * 2> topoQuad{
        0,3,4,1,
        4,7,8,5,
    };

    std::array<long long, 6> topoPolygon{
        1,3,6,7,8,5
    };
}

// index 可以为0,1,2,3 分别测试不同单元类型
const int index = 0;

int main()
{
    vtkNew<vtkActor> actor;

    vtkNew<vtkPoints> points;
    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }

    if (index == 0)
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        for (size_t i = 0; i < topoLine.size(); i += 2)
        {
            cells->InsertNextCell({ topoLine[i + 0],topoLine[i + 1] });
        }
        polyData->SetPoints(points);
        polyData->SetLines(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        actor->SetMapper(mapper);
    }

    if (index == 1)
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        for (size_t i = 0; i < topoTriangle.size(); i += 3)
        {
            cells->InsertNextCell({ topoTriangle[i + 0], topoTriangle[i + 1], topoTriangle[i + 2] });
        }
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        actor->SetMapper(mapper);
    }

    if (index == 2)
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        for (size_t i = 0; i < topoQuad.size(); i += 4)
        {
            cells->InsertNextCell({ topoQuad[i + 0], topoQuad[i + 1], topoQuad[i + 2], topoQuad[i + 3] });
        }
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        actor->SetMapper(mapper);
    }

    if (index == 3)
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        for (size_t i = 0; i < topoPolygon.size(); i += 6)
        {
            cells->InsertNextCell({ topoPolygon[i + 0], topoPolygon[i + 1], topoPolygon[i + 2],
                topoPolygon[i + 3],topoPolygon[i + 4], topoPolygon[i + 5] });
        }
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        actor->SetMapper(mapper);
    }

    vtkNew<vtkCellTypes> types;
    actor->GetMapper()->GetInput()->GetCellTypes(types);
    auto numCellTypes = types->GetNumberOfTypes();
    for (size_t i = 0; i < numCellTypes; ++i)
    {
        auto id = static_cast<int>(types->GetCellType(i));
        auto name = magic_enum::enum_name(static_cast<VTKCellType>(id));
        std::cout << "type id : " << id << "\t name : " << name << '\n';
    }

    actor->GetProperty()->SetColor(1, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor(actor);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST31










#ifdef TEST43

#define TEST_LINE2LINE // 测试线与线的交点
#define TEST_LINE2POLY // 测试线与多边形网格的交点

#ifdef TEST_LINE2POLY

#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkLine.h>
#include <vtkOBBTree.h>
#include <vtkExtractCells.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkNamedColors.h>
#include <vtkPolyData.h>
#include <vtkVertexGlyphFilter.h>

#define vsp	vtkSmartPointer

int main(int, char* [])
{
    auto colors = vsp<vtkNamedColors>::New();

    auto sphereSource = vsp<vtkSphereSource>::New();
    sphereSource->Update();

    // 注意 {0,0,100}和{0,0,-100}两个点的连线并不一定和屏幕垂直，它会根据照相机旋转
    // 这是一个世界坐标
    double lineP0[3] = { -0.6, -0.8, -0.6 };
    double lineP1[3] = { .6, .6, .6 };
    auto lineSource = vsp<vtkLineSource>::New();
    lineSource->SetPoint1(lineP0);
    lineSource->SetPoint2(lineP1);
    lineSource->Update();

    auto intersectionPoints = vsp<vtkPoints>::New();
    auto intersectioncells = vsp<vtkIdList>::New();
    double tol = 1e-8;
    auto obbTree = vsp<vtkOBBTree>::New();
    obbTree->SetTolerance(tol);
    obbTree->SetDataSet(sphereSource->GetOutput());
    obbTree->BuildLocator();
    obbTree->IntersectWithLine(lineP0, lineP1, intersectionPoints, intersectioncells);

    intersectioncells->GetNumberOfIds();
    intersectionPoints->GetNumberOfPoints();

    auto pointsPolydata = vsp<vtkPolyData>::New();
    pointsPolydata->SetPoints(intersectionPoints);
    auto vertexFilter = vsp<vtkVertexGlyphFilter>::New();
    vertexFilter->SetInputData(pointsPolydata);
    vertexFilter->Update();
    auto polydata = vsp<vtkPolyData>::New();
    polydata->ShallowCopy(vertexFilter->GetOutput());

    auto cellSource = vsp<vtkExtractCells>::New();
    cellSource->SetInputConnection(sphereSource->GetOutputPort());
    cellSource->SetCellList(intersectioncells);

    auto sphereMapper = vsp<vtkPolyDataMapper>::New();
    sphereMapper->SetInputData(sphereSource->GetOutput());
    auto sphereActor = vsp<vtkActor>::New();
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetRepresentationToWireframe();

    auto lineMapper = vsp<vtkPolyDataMapper>::New();
    lineMapper->SetInputData(lineSource->GetOutput());
    auto lineActor = vsp<vtkActor>::New();
    lineActor->SetMapper(lineMapper);
    lineActor->GetProperty()->SetColor(colors->GetColor3d("red").GetData());

    auto pointMapper = vsp<vtkPolyDataMapper>::New();
    pointMapper->SetInputData(polydata);
    auto pointActor = vsp<vtkActor>::New();
    pointActor->SetMapper(pointMapper);
    pointActor->GetProperty()->SetColor(colors->GetColor3d("blue").GetData());
    pointActor->GetProperty()->SetPointSize(8);

    auto cellMapper = vsp<vtkDataSetMapper>::New();
    cellMapper->SetInputConnection(cellSource->GetOutputPort());
    auto cellActor = vsp<vtkActor>::New();
    cellActor->SetMapper(cellMapper);
    cellActor->GetProperty()->SetColor(colors->GetColor3d("yellow").GetData());

    auto renderer = vsp<vtkRenderer>::New();
    auto renWinddow = vsp<vtkRenderWindow>::New();
    auto interactor = vsp<vtkRenderWindowInteractor>::New();
    renWinddow->SetSize(600, 600);
    renWinddow->AddRenderer(renderer);
    interactor->SetRenderWindow(renWinddow);
    renderer->SetBackground(colors->GetColor3d("DarkOliveGreen").GetData());
    renderer->AddActor(sphereActor);
    renderer->AddActor(lineActor);
    renderer->AddActor(pointActor);
    renderer->AddActor(cellActor);

    interactor->Initialize();
    interactor->Start();

    return EXIT_SUCCESS;
}
#endif // TEST_LINE2POLY

#ifdef TEST_LINE2LINE

#include "vtkSmartPointer.h"
#include "vtkProperty.h"
#include "vtkCamera.h"

#include "vtkCylinderSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolygon.h>

int main(int, char* [])
{

    // Create a square in the XY plane
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    // Create the polygon
    vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
    polygon->GetPoints()->DeepCopy(points);
    polygon->GetPointIds()->SetNumberOfIds(4); // 4 corners of the square
    polygon->GetPointIds()->SetId(0, 0);
    polygon->GetPointIds()->SetId(1, 1);
    polygon->GetPointIds()->SetId(2, 2);
    polygon->GetPointIds()->SetId(3, 3);

    // Inputs
    double p1[3] = { 1, 0, -1 };
    double p2[3] = { 1, 0, 1 };
    double tolerance = 0.001;   // 公差
    // Outputs
    double t; // Parametric coordinate of intersection (0 (corresponding to p1) to 1 (corresponding to p2))
    double x[3]; // The coordinate of the intersection
    double pcoords[3];
    int subId;

    vtkIdType iD = polygon->IntersectWithLine(p1, p2, tolerance, t, x, pcoords, subId);

    std::cout << "t:  " << t << std::endl;
    std::cout << "pcoords:  " << pcoords[0] << " " << pcoords[1] << " " << pcoords[2] << std::endl;
    std::cout << "Intersect " << iD << std::endl;
    std::cout << "subid: " << subId << std::endl;
    std::cout << "x : " << x[0] << " " << x[1] << " " << x[2] << std::endl;


    vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
    double p3[3] = { 1, -1, 0 };
    double p4[3] = { 1, 1, 0 };
    double u, v;
    vtkIdType iD2 = line->Intersection(p1, p2, p3, p4, u, v);

    std::cout << "u :" << u << endl;
    std::cout << "v :" << v << endl;
    std::cout << "Intersect :" << iD2 << endl;

    return 0;
}
#endif // TEST_LINT2LINE

#endif // TEST43

#ifdef TEST44

#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCoordinate.h>
#include <vtkPicker.h>
#include <vtkPropPicker.h>
#include <vtkRegularPolygonSource.h>

#include <iostream>
#include <array>

namespace
{
    vtkNew<vtkRenderer> renderer1;

    class InteractorStyle : public vtkInteractorStyleTrackballCamera
    {
    public:
        static InteractorStyle* New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleTrackballCamera);

        void OnLeftButtonUp() override
        {
            Superclass::OnLeftButtonUp();

            if (this->Interactor && this->CurrentRenderer)
            {
                auto eventPos = this->Interactor->GetEventPosition();

                vtkNew<vtkPropPicker> picker;
                this->Interactor->SetPicker(picker);

                if (picker->Pick(eventPos[0], eventPos[1], 0., renderer1) != 0)
                {
                    auto pickModelCoordPos = picker->GetPickPosition();

                    // 将鼠标所在的位置转化为世界坐标
                    vtkNew<vtkCoordinate> coord1;
                    coord1->SetCoordinateSystemToDisplay();
                    coord1->SetValue(static_cast<double>(eventPos[0]), static_cast<double>(eventPos[1]));
                    auto eventWorldPos = coord1->GetComputedWorldValue(renderer1);

                    // 将拾取到的世界坐标转化为屏幕坐标
                    vtkNew<vtkCoordinate> coord2;
                    coord2->SetCoordinateSystemToWorld();
                    coord2->SetValue(pickModelCoordPos);
                    auto pickDisplayPos = coord2->GetComputedDisplayValue(renderer1);

                    std::cout << "-----------------------------------------\n";
                    std::cout << "display coordinate\tmouse pos:\t" << eventPos[0] << ',' << eventPos[1] << '\n';
                    std::cout << "world coordinate\tmouse pos:\t" << eventWorldPos[0] << ',' << eventWorldPos[1] << ',' << eventWorldPos[2] << '\n';
                    std::cout << "display coordinate\tpick pos:\t" << pickDisplayPos[0] << ',' << pickDisplayPos[1] << '\n';
                    std::cout << "world coordinate\tpick pos:\t" << pickModelCoordPos[0] << ',' << pickModelCoordPos[1] << ',' << pickModelCoordPos[2] << '\n';
                }
            }
        }

        void OnRightButtonUp()override
        {

            Superclass::OnRightButtonUp();
        }
    };

    vtkStandardNewMacro(InteractorStyle);
}

int main()
{
    //（底下的图层）添加一个红色的矩形
    {
        std::array<float, 4 * 3> vertices{
            -1.f,-1.f, 0.f,
             1.f,-1.f, 0.f,
             1.f, 1.f, 0.f,
            -1.f, 1.f, 0.f
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
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);
        renderer1->AddActor(actor);
    }

    //（上层的图层）添加一个绿色的三角形
    {
        std::array<float, 3 * 3> vertices{
            -1.f, -1.f, 0.1f,
             1.f, -1.f, 0.1f,
             0.f,  1.f, 0.1f,
        };

        vtkNew<vtkPoints> points;
        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
        }

        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0,1,2 });
        polyData->SetPoints(points);
        polyData->SetPolys(cells);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        // 设置图元始终在最上面还是最下面
        mapper->SetResolveCoincidentTopologyToPolygonOffset();
        mapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, -1.e6);     // 面类型 负值则在上面，正值则在下面
        //sphereMapper->SetRelativeCoincidentTopologyLineOffsetParameters(0, units0); // 线类型
        //sphereMapper->SetRelativeCoincidentTopologyPointOffsetParameter(units0);    // 点类型

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0, 1, 0);
        renderer1->AddActor(actor);
    }

    renderer1->SetBackground(.1, .2, .3);
    renderer1->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(600, 600);
    renderWindow->AddRenderer(renderer1);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<InteractorStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}


#endif // TEST44

#ifdef TEST45

#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCoordinate.h>
#include <vtkPicker.h>
#include <vtkPropPicker.h>
#include <vtkRegularPolygonSource.h>

#include <iostream>
#include <array>

namespace
{
    vtkNew<vtkRenderer> renderer1;
    vtkNew<vtkRenderer> renderer2;

    class InteractorStyle : public vtkInteractorStyleTrackballCamera
    {
    public:
        static InteractorStyle* New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleTrackballCamera);

        void OnLeftButtonUp() override
        {
            Superclass::OnLeftButtonUp();

            if (this->Interactor && this->CurrentRenderer)
            {
                // 多个图层拾取
                auto eventPos = this->Interactor->GetEventPosition();

                vtkNew<vtkPropPicker> picker;
                this->Interactor->SetPicker(picker);

                if (picker->Pick(eventPos[0], eventPos[1], 0., renderer1) != 0)
                {
                    auto pickModelCoordPos = picker->GetPickPosition();

                    // 将鼠标所在的位置转化为世界坐标
                    vtkNew<vtkCoordinate> coord1;
                    coord1->SetCoordinateSystemToDisplay();
                    coord1->SetValue(static_cast<double>(eventPos[0]), static_cast<double>(eventPos[1]));
                    auto eventWorldPos = coord1->GetComputedWorldValue(renderer1);

                    // 将拾取到的世界坐标转化为屏幕坐标
                    vtkNew<vtkCoordinate> coord2;
                    coord2->SetCoordinateSystemToWorld();
                    coord2->SetValue(pickModelCoordPos);
                    auto pickDisplayPos = coord2->GetComputedDisplayValue(renderer1);

                    std::cout << "-----------------------------------------\n";
                    std::cout << "display coordinate\tmouse pos:\t" << eventPos[0] << ',' << eventPos[1] << '\n';
                    std::cout << "world coordinate\tmouse pos:\t" << eventWorldPos[0] << ',' << eventWorldPos[1] << ',' << eventWorldPos[2] << '\n';
                    std::cout << "display coordinate\tpick pos:\t" << pickDisplayPos[0] << ',' << pickDisplayPos[1] << '\n';
                    std::cout << "world coordinate\tpick pos:\t" << pickModelCoordPos[0] << ',' << pickModelCoordPos[1] << ',' << pickModelCoordPos[2] << '\n';
                }
            }
        }

        void OnRightButtonUp()override
        {
            if (this->CurrentRenderer == renderer1)
            {
                std::cout << "current renderer is renderer 1\n";
            }
            else if (this->CurrentRenderer == renderer2)
            {
                std::cout << "current renderer is  renderer 2\n";
            }

            Superclass::OnRightButtonUp();
        }
    };

    vtkStandardNewMacro(InteractorStyle);
}

int main()
{
    // renderer1（底下的图层）添加一个红色的矩形
    {
        std::array<float, 4 * 3> vertices{
            -1.f,-1.f, 0.f,
             1.f,-1.f, 0.f,
             1.f, 1.f, 0.f,
            -1.f, 1.f, 0.f
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
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);
        renderer1->AddActor(actor);
    }

    // renderer2（上层的图层）添加一个绿色的三角形
    {
        std::array<float, 3 * 3> vertices{
            -1.f, -1.f, 0.1f,
             1.f, -1.f, 0.1f,
             0.f,  1.f, 0.1f,
        };

        vtkNew<vtkPoints> points;
        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
        }

        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0,1,2 });
        polyData->SetPoints(points);
        polyData->SetPolys(cells);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0, 1, 0);
        renderer2->AddActor(actor);
    }

    renderer1->SetBackground(.1, .2, .3);
    renderer1->ResetCamera();
    renderer1->SetLayer(0);

    // renderer2和renderer1使用同一个camera
    renderer2->SetBackground(1, 1, 0);
    renderer2->SetActiveCamera(renderer1->GetActiveCamera());
    // renderer2始终在上面
    renderer2->SetLayer(1);

    // 开启关闭交互
    renderer1->InteractiveOn();
    renderer2->InteractiveOff();

    std::cout << "renderer1 interactive: " << renderer1->GetInteractive() << '\n';
    std::cout << "renderer2 interactive: " << renderer2->GetInteractive() << '\n';

    // renderWindow可以添加多个vtkRenderer
    // 每个vtkRenderer可以设置图层
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(600, 600);
    renderWindow->SetNumberOfLayers(2);

    // 如果当前有多个renderer都开起了交互
    // 则鼠标交互响应的最后添加的renderer
    // 如果当前只有一个renderer开启了交互
    // 则鼠标交互响应的是开启交互的renderer
    renderWindow->AddRenderer(renderer2);
    renderWindow->AddRenderer(renderer1);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<InteractorStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}


#endif // TEST45





