
/*
 * 1. 阴影和光照 https://examples.vtk.org/site/Cxx/Visualization/ShadowsLightsDemo/
 * 2. 关闭默认的光照，让颜色的对比度看起来更明显
 * 3. 光线投射raycast
 */

#define TEST3

#ifdef TEST1

// The scene consists of:
// 1) four actors: a rectangle, a box, a cone and a sphere.
//    The box, the cone and the sphere are above the rectangle.
// 2) Two spotlights, one in the direction of the box, another one in the
//    direction of the sphere.
//    Both lights are above the box, the cone and  the sphere.

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCameraPass.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkLight.h>
#include <vtkLightActor.h>
#include <vtkLightCollection.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOpaquePass.h>
#include <vtkOpenGLRenderer.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderPassCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSequencePass.h>
#include <vtkShadowMapBakerPass.h>
#include <vtkShadowMapPass.h>
#include <vtkSphereSource.h>

namespace {
// For each spotlight, add a light frustum wireframe representation and a cone
// wireframe representation, colored with the light color.
void AddLightActors(vtkRenderer* r);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkRenderWindowInteractor> interactor;

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(400, 400);
    renderWindow->SetMultiSamples(0);

    renderWindow->SetAlphaBitPlanes(1);
    interactor->SetRenderWindow(renderWindow);

    vtkNew<vtkOpenGLRenderer> renderer;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(640, 480);

    vtkNew<vtkSequencePass> seq;

    vtkNew<vtkRenderPassCollection> passes;

    vtkNew<vtkShadowMapPass> shadows;
    passes->AddItem(shadows->GetShadowMapBakerPass());
    passes->AddItem(shadows);

    vtkNew<vtkOpaquePass> opaque;
    passes->AddItem(opaque);

    seq->SetPasses(passes);

    vtkNew<vtkCameraPass> cameraP;
    cameraP->SetDelegatePass(seq);

    // Tell the renderer to use our render pass pipeline.
    vtkOpenGLRenderer* glrenderer = dynamic_cast<vtkOpenGLRenderer*>(renderer.GetPointer());
    glrenderer->SetPass(cameraP);

    vtkNew<vtkNamedColors> colors;
    vtkColor3d boxColor       = colors->GetColor3d("Tomato");
    vtkColor3d rectangleColor = colors->GetColor3d("Beige");
    vtkColor3d coneColor      = colors->GetColor3d("Peacock");
    vtkColor3d sphereColor    = colors->GetColor3d("Banana");

    vtkNew<vtkPlaneSource> rectangleSource;
    rectangleSource->SetOrigin(-5.0, 0.0, 5.0);
    rectangleSource->SetPoint1(5.0, 0.0, 5.0);
    rectangleSource->SetPoint2(-5.0, 0.0, -5.0);
    rectangleSource->SetResolution(100, 100);

    vtkNew<vtkPolyDataMapper> rectangleMapper;
    rectangleMapper->SetInputConnection(rectangleSource->GetOutputPort());

    rectangleMapper->SetScalarVisibility(0);

    vtkNew<vtkActor> rectangleActor;
    rectangleActor->SetMapper(rectangleMapper);
    rectangleActor->SetVisibility(1);
    rectangleActor->GetProperty()->SetColor(rectangleColor.GetData());

    vtkNew<vtkCubeSource> boxSource;
    boxSource->SetXLength(2.0);

    vtkNew<vtkPolyDataNormals> boxNormals;
    boxNormals->SetInputConnection(boxSource->GetOutputPort());
    boxNormals->ComputePointNormalsOff();
    boxNormals->ComputeCellNormalsOn();
    boxNormals->Update();
    boxNormals->GetOutput()->GetPointData()->SetNormals(nullptr);

    vtkNew<vtkPolyDataMapper> boxMapper;
    boxMapper->SetInputConnection(boxNormals->GetOutputPort());
    boxMapper->SetScalarVisibility(0);

    vtkNew<vtkActor> boxActor;
    boxActor->SetMapper(boxMapper);
    boxActor->SetVisibility(1);
    boxActor->SetPosition(-2.0, 2.0, 0.0);
    boxActor->GetProperty()->SetColor(boxColor.GetData());

    vtkNew<vtkConeSource> coneSource;
    coneSource->SetResolution(24);
    coneSource->SetDirection(1.0, 1.0, 1.0);

    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(coneSource->GetOutputPort());
    coneMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper(coneMapper);
    coneActor->VisibilityOn();
    coneActor->SetPosition(0.0, 1.0, 1.0);
    coneActor->GetProperty()->SetColor(coneColor.GetData());

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetThetaResolution(32);
    sphereSource->SetPhiResolution(32);

    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    sphereMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);

    sphereActor->VisibilityOn();
    sphereActor->SetPosition(2.0, 2.0, -1.0);
    sphereActor->GetProperty()->SetColor(sphereColor.GetData());

    renderer->AddViewProp(rectangleActor);
    renderer->AddViewProp(boxActor);
    renderer->AddViewProp(coneActor);
    renderer->AddViewProp(sphereActor);

    // Spotlights.

    // Lighting the box.
    vtkNew<vtkLight> l1;
    l1->SetPosition(-4.0, 4.0, -1.0);
    l1->SetFocalPoint(boxActor->GetPosition());
    l1->SetColor(colors->GetColor3d("White").GetData());
    l1->PositionalOn();
    renderer->AddLight(l1);
    l1->SwitchOn();

    // Lighting the sphere.
    vtkNew<vtkLight> l2;
    l2->SetPosition(4.0, 5.0, 1.0);
    l2->SetFocalPoint(sphereActor->GetPosition());
    l2->SetColor(colors->GetColor3d("Magenta").GetData());
    l2->PositionalOn();
    renderer->AddLight(l2);
    l2->SwitchOn();

    AddLightActors(renderer);

    renderer->SetBackground2(colors->GetColor3d("Black").GetData());
    renderer->SetBackground(colors->GetColor3d("Silver").GetData());
    renderer->SetGradientBackground(true);

    renderWindow->Render();
    renderWindow->SetWindowName("ShadowsLightsDemo");

    renderer->ResetCamera();

    auto camera = renderer->GetActiveCamera();
    camera->Azimuth(40.0);
    camera->Elevation(10.0);

    renderWindow->Render();

    interactor->Start();

    return EXIT_SUCCESS;
}

namespace {
// For each spotlight, add a light frustum wireframe representation and a cone
// wireframe representation, colored with the light color.
void AddLightActors(vtkRenderer* r)
{
    vtkLightCollection* lights = r->GetLights();
    lights->InitTraversal();

    vtkLight* light = lights->GetNextItem();
    while (light != 0)
    {
        double angle = light->GetConeAngle();
        if (light->LightTypeIsSceneLight() && light->GetPositional() && angle < 180.0) // spotlight
        {
            vtkNew<vtkLightActor> light_actor;
            light_actor->SetLight(light);
            r->AddViewProp(light_actor);
        }
        light = lights->GetNextItem();
    }
}
} // namespace

#endif // TEST1

#ifdef TEST2

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkShaderProperty.h>
#include <vtkSmartPointer.h>

auto GenPolyData()
{
    // clang-format off
    std::vector<float> vertices {
        0.f, 0.f,
        1.f, 0.f,
        1.f, 1.f,
        0.f, 1.f
    };

    std::vector<int> indicesPoly {
         0, 1, 2, 3
    };

    std::vector<float> fields {
        -10.f, 10.f, 10.f, -10.f
    };
    // clang-format on

    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cellsPoly;

    for (size_t i = 0; i < vertices.size(); i += 2)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], 0.0);
    }

    for (size_t i = 0; i < indicesPoly.size(); i += 4)
    {
        cellsPoly->InsertNextCell({ indicesPoly[i], indicesPoly[i + 1], indicesPoly[i + 2], indicesPoly[i + 3] });
    }
    polyData->SetPoints(points);
    polyData->SetPolys(cellsPoly);

    vtkNew<vtkFloatArray> scalars;
    for (size_t i = 0; i < fields.size(); ++i)
    {
        scalars->InsertNextValue(fields[i]);
    }

    polyData->GetPointData()->SetScalars(scalars);

    return polyData;
}

int main(int, char*[])
{
    // 颜色查找表的颜色个数为20个
    vtkIdType numOfColors { 20 };

    // 创建颜色查找表
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetNumberOfColors(numOfColors);
    hueLut->SetNumberOfTableValues(numOfColors);

    // 指定颜色查找表的每一个颜色值
    double color = 255.0;
    hueLut->SetTableValue(0, 255.0 / color, 0.0 / color, 0.0 / color);
    hueLut->SetTableValue(1, 255.0 / color, 42. / color, 0.0 / color);
    hueLut->SetTableValue(2, 255.0 / color, 85. / color, 0.0 / color);
    hueLut->SetTableValue(3, 255.0 / color, 127. / color, 0.0 / color);
    hueLut->SetTableValue(4, 255.0 / color, 170. / color, 0.0 / color);
    hueLut->SetTableValue(5, 255.0 / color, 213. / color, 0.0 / color);
    hueLut->SetTableValue(6, 255.0 / color, 255. / color, 0.0 / color);
    hueLut->SetTableValue(7, 191.0 / color, 255. / color, 0.0 / color);
    hueLut->SetTableValue(8, 127.0 / color, 255. / color, 0.0 / color);
    hueLut->SetTableValue(9, 63.0 / color, 255. / color, 0.0 / color);
    hueLut->SetTableValue(10, 255.0 / color, 255. / color, 255.0 / color);
    hueLut->SetTableValue(11, 0.0 / color, 255. / color, 63.0 / color);
    hueLut->SetTableValue(12, 0.0 / color, 255. / color, 127.0 / color);
    hueLut->SetTableValue(13, 0.0 / color, 255. / color, 191.0 / color);
    hueLut->SetTableValue(14, 0.0 / color, 255. / color, 255.0 / color);
    hueLut->SetTableValue(15, 0.0 / color, 204. / color, 255.0 / color);
    hueLut->SetTableValue(16, 0.0 / color, 153. / color, 255.0 / color);
    hueLut->SetTableValue(17, 0.0 / color, 102. / color, 255.0 / color);
    hueLut->SetTableValue(18, 0.0 / color, 51. / color, 255.0 / color);
    hueLut->SetTableValue(19, 0.0 / color, 0. / color, 255.0 / color);
    hueLut->BuildSpecialColors();

    //---------------------------------------------------------
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(GenPolyData());
    mapper->SetScalarRange(mapper->GetInput()->GetPointData()->GetScalars()->GetRange());
    mapper->ScalarVisibilityOn();
    mapper->SetLookupTable(hueLut);
    mapper->InterpolateScalarsBeforeMappingOn();

    //---------------------------------------------------------
    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLookupTable(mapper->GetLookupTable());
    scalarBar->SetMaximumNumberOfColors(numOfColors);

    //---------------------------------------------------------
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    // 关闭默认的光照，这样颜色映射表是什么颜色，图元显示的就是什么颜色
    // 不会因为视角的原因导致颜色的RGB分量减小
    // [2510]vtkOpenGLPolyDataMapper::GetNeedToRebuildShaders 会在该函数内判断是否需要生成光照的shader
    // [888] vtkOpenGLPolyDataMapper::ReplaceShaderLight 在该函数内设置着色器的最终输出
    actor->GetProperty()->LightingOff();

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor2D(scalarBar);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLight.h>
#include <vtkNIFTIImageReader.h>
#include <vtkNew.h>
#include <vtkOpenGLGPUVolumeRayCastMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

int main()
{
    vtkNew<vtkNIFTIImageReader> reader;
    // reader->SetFileName("../resources/filtered_func_data.nii.gz");
    reader->SetFileName("../resources/avg152T1_RL_nifti.nii.gz");
    reader->Update();

    vtkNew<vtkOpenGLGPUVolumeRayCastMapper> mapper;
    mapper->SetAutoAdjustSampleDistances(0);
    mapper->SetSampleDistance(0.5);
    mapper->UseJitteringOn();
    mapper->SetInputData(reader->GetOutput());
    // mapper->SetGlobalIlluminationReach(0.75);
    // mapper->SetVolumetricScatteringBlending(1.0);

    vtkNew<vtkVolume> volume;
    volume->SetMapper(mapper);

    vtkNew<vtkVolumeProperty> volumeProperty;
    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->ShadeOn(); // 打开或者关闭阴影测试
    volumeProperty->SetAmbient(0.55);
    volumeProperty->SetDiffuse(0.85);     // 漫反射
    volumeProperty->SetSpecular(0.54);    // 镜面反射
    volumeProperty->SetSpecularPower(69); // 镜面反射

    // 设置不透明度
    vtkNew<vtkPiecewiseFunction> compositeOpacity;
    compositeOpacity->AddPoint(-1000, 0.00);
    compositeOpacity->AddPoint(100, 0.00);
    compositeOpacity->AddPoint(300, 0.879);
    compositeOpacity->AddPoint(478, 0.879);
    compositeOpacity->AddPoint(1500, 0.901);
    volumeProperty->SetScalarOpacity(compositeOpacity); // 设置不透明度传输函数

    vtkNew<vtkColorTransferFunction> color;
    color->AddRGBPoint(-1000, 0.00, 0.00, 0.00);
    color->AddRGBPoint(100, 220 / 255., 21 / 255., 3 / 255.);
    color->AddRGBPoint(300, 250 / 255., 1.00, 189 / 255.);
    color->AddRGBPoint(478, 224 / 255., 1, 249 / 255.);
    color->AddRGBPoint(1500, 1, 1, 1);
    volumeProperty->SetColor(color);

    volume->SetProperty(volumeProperty);

    vtkNew<vtkRenderer> ren;
    ren->AddActor(volume);
    ren->SetBackground(0, 0, 0);
    ren->ResetCamera();
    vtkCamera* camera = ren->GetActiveCamera();
    camera->Elevation(-70);

    ren->ClearLights();
    ren->RemoveAllLights();

    double* lightPosition    = camera->GetPosition();
    double lightPositions[3] = { lightPosition[0], lightPosition[1], lightPosition[2] - 100 };
    vtkNew<vtkLight> light;
    light->SetLightTypeToSceneLight();
    light->SetPosition(lightPositions);
    light->SetPositional(true);
    light->SetAmbientColor(0.3, 0.2, 0.1);
    light->SetConeAngle(60);
    light->SetFocalPoint(camera->GetFocalPoint());
    light->SetIntensity(1.2);
    ren->AddLight(light);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren);

    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);
    renWin->SetSize(1024, 768);
    renWin->Render();

    iren->Start();

    return 0;
}

#endif // TEST3
