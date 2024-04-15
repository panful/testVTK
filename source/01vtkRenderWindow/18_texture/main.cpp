/**
 * 1. 纹理的基础使用
 * 2. 天空盒
 */

#define TEST2

#ifdef TEST1

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGReader.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>

int main()
{
    vtkNew<vtkJPEGReader> jpgReader;
    jpgReader->SetFileName("../resources/test3.jpg");
    jpgReader->Update();

    // 生成纹理
    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(jpgReader->GetOutputPort());
    texture->InterpolateOn();             // 启用线性插值
    texture->SetWrap(vtkTexture::Repeat); // 环绕模式
    texture->RepeatOn();
    texture->EdgeClampOn();
    texture->PremultipliedAlphaOff();           // 纹理颜色是否与 alpha 预乘，默认 off
    texture->RestrictPowerOf2ImageSmallerOff(); // 纹理强制为2的幂时，强制“新”图像的尺寸不小于原始图像，on:不大于
    texture->UseSRGBColorSpaceOff();            // sRGB 颜色空间
    texture->CubeMapOff();

    texture->SetColorModeToMapScalars();
    texture->SetColorModeToDirectScalars();

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 0, 0);
    points->InsertNextPoint(1, 1, 0);
    points->InsertNextPoint(0, 1, 0);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1, 2, 3 });

    // 纹理坐标
    vtkNew<vtkDoubleArray> texCoords;
    texCoords->SetNumberOfComponents(2);
    texCoords->InsertNextTuple2(0., 0.);
    texCoords->InsertNextTuple2(1., 0.);
    texCoords->InsertNextTuple2(1., 1.);
    texCoords->InsertNextTuple2(0., 1.);

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetPointData()->SetTCoords(texCoords); // 给每个顶点设置纹理坐标

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetTexture(texture); // 使用纹理

    //-------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);
    window->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> rWinInt;
    rWinInt->SetRenderWindow(window);
    rWinInt->SetInteractorStyle(style);

    window->Render();
    rWinInt->Start();

    return EXIT_SUCCESS;
}

#endif // TEST1

#ifdef TEST2

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkImageFlip.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSkybox.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>

int main()
{
    vtkNew<vtkTexture> textureCubemap;
    textureCubemap->CubeMapOn(); // 开启立方体贴图
    textureCubemap->UseSRGBColorSpaceOn();

    // clang-format off
    std::string pathSkybox[6] = { 
        "../resources/skybox/posx.jpg", 
        "../resources/skybox/negx.jpg", 
        "../resources/skybox/posy.jpg",
        "../resources/skybox/negy.jpg", 
        "../resources/skybox/posz.jpg", 
        "../resources/skybox/negz.jpg",
    };
    // clang-format on

    for (int i = 0; i < 6; i++)
    {
        vtkNew<vtkJPEGReader> jpg;
        jpg->SetFileName(pathSkybox[i].c_str());

        vtkNew<vtkImageFlip> flip;
        flip->SetInputConnection(jpg->GetOutputPort());
        flip->SetFilteredAxis(1);                                     // flip y axis
        textureCubemap->SetInputConnection(i, flip->GetOutputPort()); // 立方体贴图的6个纹理图片
    }

    vtkNew<vtkSkybox> skybox;
    skybox->SetTexture(textureCubemap);
    skybox->SetProjectionToCube();

    //-------------------------------------------------------------
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> cube;
    cube->SetMapper(mapper);
    cube->GetProperty()->SetColor(0, 1, 0);

    //-------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(skybox);
    renderer->AddActor(cube);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);
    window->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> rWinInt;
    rWinInt->SetRenderWindow(window);
    rWinInt->SetInteractorStyle(style);

    window->Render();
    rWinInt->Start();

    return EXIT_SUCCESS;
}

#endif // TEST2
