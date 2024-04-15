/**
 * 1. 纹理的基础使用
 * 2. 天空盒
 * 3. PBR IBL
 */

#define TEST3

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

#ifdef TEST3

#include <vtkActor.h>
#include <vtkImageFlip.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkOpenGLRenderer.h>
#include <vtkPBRIrradianceTexture.h>
#include <vtkPBRLUTTexture.h>
#include <vtkPBRPrefilterTexture.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSkybox.h>
#include <vtkSphereSource.h>
#include <vtkTexture.h>

void AddActor(vtkOpenGLRenderer* ren)
{
    vtkNew<vtkSphereSource> sphere;
    sphere->SetThetaResolution(100);
    sphere->SetPhiResolution(100);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphere->GetOutputPort());

    for (size_t x = 0; x < 5; ++x)
    {
        for (size_t y = 0; y < 5; ++y)
        {
            vtkNew<vtkActor> actorSphere;
            actorSphere->SetPosition(static_cast<double>(x), static_cast<double>(y), 0.);
            actorSphere->SetMapper(mapper);
            actorSphere->GetProperty()->SetColor(1., 1., 1.);
            actorSphere->GetProperty()->SetInterpolationToPBR();                   // PBR着色，还有 GOURAUD PHONG FLAT
            actorSphere->GetProperty()->SetMetallic(static_cast<double>(x) / 4.);  // 设置金属系数[0.,1.]，默认为 0
            actorSphere->GetProperty()->SetRoughness(static_cast<double>(y) / 4.); // 设置粗糙度系数[0.,1.]，光泽0 粗糙1 默认为 0.5
            ren->AddActor(actorSphere);
        }
    }
}

void AddSkybox(vtkRenderer* ren)
{
    vtkNew<vtkTexture> textureCubemap;
    textureCubemap->CubeMapOn(); // 开启立方体贴图
    textureCubemap->UseSRGBColorSpaceOn();
    textureCubemap->InterpolateOn();
    textureCubemap->MipmapOn();

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

    ren->SetEnvironmentTexture(textureCubemap, true); // 设置环境贴图（天空盒）
    ren->AddActor(skybox);
}

int main(int argc, char* argv[])
{
    vtkNew<vtkOpenGLRenderer> ren;
    ren->SetBackground(.1, .2, .3);
    ren->UseImageBasedLightingOn();  // 使用IBL，Renderer 必须设置 EnvironmentTexture
    ren->UseSphericalHarmonicsOff(); // 不使用球面谐波，使用辐照度纹理
    AddActor(ren);
    AddSkybox(ren);

    // IBL(image based lighting)
    auto pbrIrradiance = ren->GetEnvMapIrradiance();
    auto pbrBRDFLUT    = ren->GetEnvMapLookupTable();
    auto pbrPrefilter  = ren->GetEnvMapPrefiltered();

    pbrIrradiance->SetIrradianceStep(0.3); // 对半球采样的步长，默认为 pi/64

    vtkNew<vtkRenderWindow> renWin;
    renWin->SetSize(800, 600);
    renWin->AddRenderer(ren);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST3
