/**
 * 101. vtkCamera
 * 102. 可视化相机的旋转操作 Elevation Azimuth Roll
 * 103. vtkCameraActor 可视化相机的Frustum
 *
 * 201. 相机旋转原理
 * 202. 相机平移原理
 * 203. 缩放
 * 204. 透视投影 正交投影
 * 205. 以鼠标所在点缩放（默认窗口中心）
 *
 * 301. 相机的裁剪 相机的重置
 * 302. 相机处于物体内部，即使重置裁剪平面，物体仍然会被裁剪
 * 303. 物体被裁剪时，调整相机位置，重置裁剪范围，让物体能完整显示
 *
 * 401. vtkBillboardTextActor3D 广告牌，旋转相机时广告牌可以始终朝向相机
 * 402. vtkFollower 始终面向镜头
 * 403. vtkFollower 的原理，提取旋转矩阵

 * 501. vtkDistanceToCamera 图形在屏幕上始终以指定像素大小显示，即图元不能被缩放
 * 502. vtkFollower vtkDistanceToCamera 指定图元大小不变，朝向不变，始终在最上层
 * 503. vtkActor2D vtkPolyDataMapper2D 屏幕指定位置、指定大小、始终朝向相机
 * 504. 窗口缩放，图元不缩放，即图元始终和显示器比例一致
 *
 * 601. vtkCoordinate 坐标转换
 * 602. 世界坐标转换为屏幕坐标
 * 603. 将屏幕坐标转换为世界坐标，然后将这些顶点连接为一条线
 * 604. 屏幕坐标转换为世界坐标原理
 * 605. vtkPerspectiveTransform 3种投影矩阵的生成
 */

// vtk是行矩阵
// https://blog.csdn.net/liushao1031177/article/details/116903698
// https://www.cnblogs.com/ybqjymy/p/13925462.html

#define TEST605

#ifdef TEST101

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLegendScaleActor.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {

class CustomStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleTrackballCamera);

protected:
    void OnLeftButtonUp() override
    {
        Superclass::OnLeftButtonUp();

        if (this->Interactor && this->GetCurrentRenderer())
        {
            auto camera = this->CurrentRenderer->GetActiveCamera();
            std::cout << "--------------------------------------------------\n";

            auto cameraPos           = camera->GetPosition();
            auto cameraFocalPos      = camera->GetFocalPoint();
            auto cameraFocalDistance = camera->GetFocalDistance();
            auto cameraFocalDisk     = camera->GetFocalDisk();
            auto cameraFocalScale    = camera->GetFocalPointScale();
            auto cameraFocalShift    = camera->GetFocalPointShift();
            auto cameraDistance      = camera->GetDistance();
            auto cameraClippingRange = camera->GetClippingRange();
            auto cameraParallelScale = camera->GetParallelScale();
            auto cameraViewAngle     = camera->GetViewAngle();

            camera->GetViewPlaneNormal();
            camera->GetDirectionOfProjection();
            camera->GetViewUp();
            camera->GetViewShear();
            camera->GetUserViewTransform();
            camera->GetThickness();
            camera->GetStereo();
            camera->GetUserTransform();
            camera->GetUseHorizontalViewAngle();
            camera->UseHorizontalViewAngleOn();

            double vec3[3] { 0.0 };
            camera->GetEyePosition(vec3);
            camera->GetEyePlaneNormal(vec3);

            // std::cout << "Camera Pos:\t\t" << cameraPos[0] << '\t' << cameraPos[1] << '\t' << cameraPos[2] << '\n';
            // std::cout << "Camera Focal Pos:\t" << cameraFocalPos[0] << '\t' << cameraFocalPos[1] << '\t' << cameraFocalPos[2] << '\n';
            // std::cout << "Camera Focal Distance:\t" << cameraFocalDistance << '\n';
            // std::cout << "Camera Focal Disk:\t" << cameraFocalDisk << '\n';
            // std::cout << "Camera Focal Scale:\t" << cameraFocalScale << '\n';
            // std::cout << "Camera Focal Shift:\t" << cameraFocalShift[0] << '\t' << cameraFocalShift[1] << '\t' << cameraFocalShift[2] << '\n';
            // std::cout << "Distance:\t" << distance << '\n';
            // std::cout << "Camera Eye Pos:\t" << eyePos[0] << '\t' << eyePos[1] << '\t' << eyePos[2] << '\n';
            // std::cout << "Camera Clipping Range:\t" << cameraClippingRange[0] << '\t' << cameraClippingRange[1] << '\n';
            // std::cout << "Camera Parallel Scale:\t" << cameraParallelScale << '\n';
            // std::cout << "Camera View Angle:\t" << cameraViewAngle << '\n';

            // modelView和view是同一个矩阵
            auto model     = camera->GetModelTransformMatrix();
            auto proj      = camera->GetProjectionTransformMatrix(this->CurrentRenderer);
            auto modelview = camera->GetModelViewTransformMatrix();
            auto view      = camera->GetViewTransformMatrix();
            auto eye       = camera->GetEyeTransformMatrix();
            auto light     = camera->GetCameraLightTransformMatrix();

            std::cout << "\n----------------Model:\n";
            model->Print(std::cout);

            std::cout << "\n----------------View:\n";
            view->Print(std::cout);

            std::cout << "\n----------------ModelView:\n";
            modelview->Print(std::cout);

            std::cout << "\n----------------Projection:\n";
            proj->Print(std::cout);

            std::cout << "\n----------------Eye:\n";
            eye->Print(std::cout);

            std::cout << "\n----------------CameraLight:\n";
            light->Print(std::cout);
        }
    }
};

vtkStandardNewMacro(CustomStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->SetBackground(.1, .2, .3);
    renderer->GetActiveCamera()->ParallelProjectionOn(); // 平行投影

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<CustomStyle> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST101

#ifdef TEST102

#include <vtkAppendFilter.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkConeSource.h>
#include <vtkContourFilter.h>
#include <vtkCubeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkImplicitModeller.h>
#include <vtkLODActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRotationalExtrusionFilter.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkWarpTo.h>

#include <algorithm>
#include <array>

// https://examples.vtk.org/site/Cxx/Visualization/CameraModel1/
// https://examples.vtk.org/site/Cxx/Visualization/CameraModel2/

int main()
{
    vtkNew<vtkNamedColors> colors;

    // Set the colors.
    std::array<unsigned char, 4> azArrowColor { { 255, 77, 77 } };
    colors->SetColor("AzimuthArrowColor", azArrowColor.data());
    std::array<unsigned char, 4> elevArrowColor { { 77, 255, 77 } };
    colors->SetColor("ElevationArrowColor", elevArrowColor.data());
    std::array<unsigned char, 4> rollArrowColor { { 255, 255, 77 } };
    colors->SetColor("RollArrowColor", rollArrowColor.data());
    std::array<unsigned char, 4> spikeColor { { 255, 77, 255 } };
    colors->SetColor("SpikeColor", spikeColor.data());
    // Set the background color.
    std::array<unsigned char, 4> bkg { { 26, 51, 102 } };
    colors->SetColor("BkgColor", bkg.data());

    // Create the RenderWindow, Renderer and both Actors
    //
    vtkNew<vtkRenderer> ren1;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // create a camera model
    vtkNew<vtkConeSource> camCS;
    camCS->SetHeight(1.5);
    camCS->SetResolution(12);
    camCS->SetRadius(0.4);

    vtkNew<vtkCubeSource> camCBS;
    camCBS->SetXLength(1.5);
    camCBS->SetZLength(0.8);
    camCBS->SetCenter(0.4, 0, 0);

    vtkNew<vtkAppendFilter> camAPD;
    camAPD->AddInputConnection(camCS->GetOutputPort());
    camAPD->AddInputConnection(camCBS->GetOutputPort());

    vtkNew<vtkDataSetMapper> camMapper;
    camMapper->SetInputConnection(camAPD->GetOutputPort());

    vtkNew<vtkLODActor> camActor;
    camActor->SetMapper(camMapper);
    camActor->SetScale(2, 2, 2);

    // draw the arrows
    vtkNew<vtkPolyData> pd;
    vtkNew<vtkCellArray> ca;
    vtkNew<vtkPoints> fp;
    fp->InsertNextPoint(0, 1, 0);
    fp->InsertNextPoint(8, 1, 0);
    fp->InsertNextPoint(8, 2, 0);
    fp->InsertNextPoint(10, 0.01, 0);
    fp->InsertNextPoint(8, -2, 0);
    fp->InsertNextPoint(8, -1, 0);
    fp->InsertNextPoint(0, -1, 0);
    ca->InsertNextCell(7);
    ca->InsertCellPoint(0);
    ca->InsertCellPoint(1);
    ca->InsertCellPoint(2);
    ca->InsertCellPoint(3);
    ca->InsertCellPoint(4);
    ca->InsertCellPoint(5);
    ca->InsertCellPoint(6);
    pd->SetPoints(fp);
    pd->SetPolys(ca);

    vtkNew<vtkPolyData> pd2;
    vtkNew<vtkCellArray> ca2;
    vtkNew<vtkPoints> fp2;
    fp2->InsertNextPoint(0, 1, 0);
    fp2->InsertNextPoint(8, 1, 0);
    fp2->InsertNextPoint(8, 2, 0);
    fp2->InsertNextPoint(10, 0.01, 0);
    ca2->InsertNextCell(4);
    ca2->InsertCellPoint(0);
    ca2->InsertCellPoint(1);
    ca2->InsertCellPoint(2);
    ca2->InsertCellPoint(3);
    pd2->SetPoints(fp2);
    pd2->SetLines(ca2);

    vtkNew<vtkImplicitModeller> arrowIM;
    arrowIM->SetInputData(pd);
    arrowIM->SetSampleDimensions(50, 20, 8);

    vtkNew<vtkContourFilter> arrowCF;
    arrowCF->SetInputConnection(arrowIM->GetOutputPort());
    arrowCF->SetValue(0, 0.2);

    vtkNew<vtkWarpTo> arrowWT;
    arrowWT->SetInputConnection(arrowCF->GetOutputPort());
    arrowWT->SetPosition(5, 0, 5);
    arrowWT->SetScaleFactor(0.85);
    arrowWT->AbsoluteOn();

    vtkNew<vtkTransform> arrowT;
    arrowT->RotateY(60);
    arrowT->Translate(-1.33198, 0, -1.479);
    arrowT->Scale(1, 0.5, 1);

    vtkNew<vtkTransformFilter> arrowTF;
    arrowTF->SetInputConnection(arrowWT->GetOutputPort());
    arrowTF->SetTransform(arrowT);

    vtkNew<vtkDataSetMapper> arrowMapper;
    arrowMapper->SetInputConnection(arrowTF->GetOutputPort());
    arrowMapper->ScalarVisibilityOff();

    // draw the azimuth arrows
    vtkNew<vtkLODActor> a1Actor;
    a1Actor->SetMapper(arrowMapper);
    a1Actor->RotateZ(180);
    a1Actor->SetPosition(1, 0, -1);
    a1Actor->GetProperty()->SetColor(colors->GetColor3d("AzimuthArrowColor").GetData());
    a1Actor->GetProperty()->SetSpecularColor(colors->GetColor3d("White").GetData());
    a1Actor->GetProperty()->SetSpecular(0.3);
    a1Actor->GetProperty()->SetSpecularPower(20);
    a1Actor->GetProperty()->SetAmbient(0.2);
    a1Actor->GetProperty()->SetDiffuse(0.8);

    vtkNew<vtkLODActor> a2Actor;
    a2Actor->SetMapper(arrowMapper);
    a2Actor->RotateZ(180);
    a2Actor->RotateX(180);
    a2Actor->SetPosition(1, 0, 1);
    a2Actor->GetProperty()->SetColor(colors->GetColor3d("AzimuthArrowColor").GetData());
    a2Actor->GetProperty()->SetSpecularColor(colors->GetColor3d("White").GetData());
    a2Actor->GetProperty()->SetSpecular(0.3);
    a2Actor->GetProperty()->SetSpecularPower(20);
    a2Actor->GetProperty()->SetAmbient(0.2);
    a2Actor->GetProperty()->SetDiffuse(0.8);

    // draw the elevation arrows
    vtkNew<vtkLODActor> a3Actor;
    a3Actor->SetMapper(arrowMapper);
    a3Actor->RotateZ(180);
    a3Actor->RotateX(90);
    a3Actor->SetPosition(1, -1, 0);
    a3Actor->GetProperty()->SetColor(colors->GetColor3d("ElevationArrowColor").GetData());
    a3Actor->GetProperty()->SetSpecularColor(colors->GetColor3d("White").GetData());
    a3Actor->GetProperty()->SetSpecular(0.3);
    a3Actor->GetProperty()->SetSpecularPower(20);
    a3Actor->GetProperty()->SetAmbient(0.2);
    a3Actor->GetProperty()->SetDiffuse(0.8);

    vtkNew<vtkLODActor> a4Actor;
    a4Actor->SetMapper(arrowMapper);
    a4Actor->RotateZ(180);
    a4Actor->RotateX(-90);
    a4Actor->SetPosition(1, 1, 0);
    a4Actor->GetProperty()->SetColor(colors->GetColor3d("ElevationArrowColor").GetData());
    a4Actor->GetProperty()->SetSpecularColor(colors->GetColor3d("White").GetData());
    a4Actor->GetProperty()->SetSpecular(0.3);
    a4Actor->GetProperty()->SetSpecularPower(20);
    a4Actor->GetProperty()->SetAmbient(0.2);
    a4Actor->GetProperty()->SetDiffuse(0.8);

    // draw the DOP
    vtkNew<vtkTransform> arrowT2;
    arrowT2->Scale(1, 0.6, 1);
    arrowT2->RotateY(90);

    vtkNew<vtkTransformPolyDataFilter> arrowTF2;
    arrowTF2->SetInputData(pd2);
    arrowTF2->SetTransform(arrowT2);

    vtkNew<vtkRotationalExtrusionFilter> arrowREF;
    arrowREF->SetInputConnection(arrowTF2->GetOutputPort());
    arrowREF->CappingOff();
    arrowREF->SetResolution(30);

    vtkNew<vtkPolyDataMapper> spikeMapper;
    spikeMapper->SetInputConnection(arrowREF->GetOutputPort());

    vtkNew<vtkLODActor> a5Actor;
    a5Actor->SetMapper(spikeMapper);
    a5Actor->SetScale(.3, .3, .6);
    a5Actor->RotateY(90);
    a5Actor->SetPosition(-2, 0, 0);
    a5Actor->GetProperty()->SetColor(colors->GetColor3d("SpikeColor").GetData());
    a5Actor->GetProperty()->SetAmbient(0.2);
    a5Actor->GetProperty()->SetDiffuse(0.8);

    // focal point
    vtkNew<vtkSphereSource> fps;
    fps->SetRadius(0.5);
    vtkNew<vtkPolyDataMapper> fpMapper;
    fpMapper->SetInputConnection(fps->GetOutputPort());
    vtkNew<vtkLODActor> fpActor;
    fpActor->SetMapper(fpMapper);
    fpActor->SetPosition(-9, 0, 0);
    fpActor->GetProperty()->SetSpecularColor(colors->GetColor3d("White").GetData());
    fpActor->GetProperty()->SetSpecular(0.3);
    fpActor->GetProperty()->SetAmbient(0.2);
    fpActor->GetProperty()->SetDiffuse(0.8);
    fpActor->GetProperty()->SetSpecularPower(20);

    // create the roll arrows
    vtkNew<vtkWarpTo> arrowWT2;
    arrowWT2->SetInputConnection(arrowCF->GetOutputPort());
    arrowWT2->SetPosition(5, 0, 2.5);
    arrowWT2->SetScaleFactor(0.95);
    arrowWT2->AbsoluteOn();

    vtkNew<vtkTransform> arrowT3;
    arrowT3->Translate(-2.50358, 0, -1.70408);
    arrowT3->Scale(0.5, 0.3, 1);

    vtkNew<vtkTransformFilter> arrowTF3;
    arrowTF3->SetInputConnection(arrowWT2->GetOutputPort());
    arrowTF3->SetTransform(arrowT3);

    vtkNew<vtkDataSetMapper> arrowMapper2;
    arrowMapper2->SetInputConnection(arrowTF3->GetOutputPort());
    arrowMapper2->ScalarVisibilityOff();

    // draw the roll arrows
    vtkNew<vtkLODActor> a6Actor;
    a6Actor->SetMapper(arrowMapper2);
    a6Actor->RotateZ(90);
    a6Actor->SetPosition(-4, 0, 0);
    a6Actor->SetScale(1.5, 1.5, 1.5);
    a6Actor->GetProperty()->SetColor(colors->GetColor3d("RollArrowColor").GetData());
    a6Actor->GetProperty()->SetSpecularColor(colors->GetColor3d("White").GetData());
    a6Actor->GetProperty()->SetSpecular(0.3);
    a6Actor->GetProperty()->SetSpecularPower(20);
    a6Actor->GetProperty()->SetAmbient(0.2);
    a6Actor->GetProperty()->SetDiffuse(0.8);

    // Add the actors to the renderer, set the background and size
    ren1->AddActor(camActor);
    ren1->AddActor(a1Actor);
    ren1->AddActor(a2Actor);
    ren1->AddActor(a3Actor);
    ren1->AddActor(a4Actor);
    ren1->AddActor(a5Actor);
    ren1->AddActor(a6Actor);
    ren1->AddActor(fpActor);
    ren1->SetBackground(colors->GetColor3d("BkgColor").GetData());
    ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
    renWin->SetSize(640, 480);
    renWin->SetWindowName("CameraModel1");

    // render the image

    vtkCamera* cam1 = (ren1->GetActiveCamera());
    ren1->ResetCamera();
    cam1->Azimuth(150);
    cam1->Elevation(30);
    cam1->Dolly(1.5);
    ren1->ResetCameraClippingRange();

    // Create a TextActor for azimuth  (a1 and a2 actor's color)
    vtkNew<vtkTextActor> text;
    text->SetInput("Azimuth");
    vtkTextProperty* tprop = text->GetTextProperty();
    tprop->SetFontFamilyToArial();
    tprop->ShadowOff();
    tprop->SetLineSpacing(1.0);
    tprop->SetFontSize(36);
    tprop->SetColor(a1Actor->GetProperty()->GetColor());
    text->SetDisplayPosition(20, 50);
    ren1->AddActor2D(text);

    // Create a TextActor for elevation  (a3 and a4 actor's color)
    vtkNew<vtkTextActor> text2;
    text2->SetInput("Elevation");
    tprop = text2->GetTextProperty();
    tprop->SetFontFamilyToArial();
    tprop->ShadowOff();
    tprop->SetLineSpacing(1.0);
    tprop->SetFontSize(36);
    tprop->SetColor(a3Actor->GetProperty()->GetColor());
    text2->SetDisplayPosition(20, 100);
    ren1->AddActor2D(text2);

    // Create a TextActor for roll (a6 actor's color)
    vtkNew<vtkTextActor> text3;
    text3->SetInput("Roll");
    tprop = text3->GetTextProperty();
    tprop->SetFontFamilyToArial();
    tprop->ShadowOff();
    tprop->SetLineSpacing(1.0);
    tprop->SetFontSize(36);
    tprop->SetColor(a6Actor->GetProperty()->GetColor());
    text3->SetDisplayPosition(20, 150);
    ren1->AddActor2D(text3);

    renWin->Render();
    iren->Initialize();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST102

#ifdef TEST103

#include <vtkCamera.h>
#include <vtkCameraActor.h>
#include <vtkMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlanes.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
    vtkNew<vtkNamedColors> namedColors;

    // Sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(400);
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetDiffuseColor(namedColors->GetColor3d("Tomato").GetData());

    // Camera
    vtkNew<vtkCamera> camera;

    vtkNew<vtkCameraActor> cameraActor;
    cameraActor->SetCamera(camera);
    cameraActor->GetProperty()->SetColor(namedColors->GetColor3d("Black").GetData());

    // (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
    auto bounds = cameraActor->GetBounds();
    std::cout << "bounds: " << bounds[0] << " " << bounds[1] << " " << bounds[2] << " " << bounds[3] << " " << bounds[4] << " " << bounds[5]
              << std::endl;

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("CameraActor");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(sphereActor);
    // Compute the active camera parameters
    renderer->ResetCamera();

    // Set the camera parameters for the camera actor
    camera->DeepCopy(renderer->GetActiveCamera());
    renderer->AddActor(cameraActor);

    // Position the camera so that we can see the camera actor
    renderer->GetActiveCamera()->SetPosition(1, 0, 0);
    renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);

    renderer->ResetCamera();
    renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST103

#ifdef TEST201

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnMouseMove() override
    {
        // 相机旋转原理
        if (this->State != VTKIS_ROTATE)
        {
            Superclass::OnMouseMove();
            return;
        }

        if (this->Interactor && this->CurrentRenderer && this->CurrentRenderer->GetActiveCamera())
        {
            vtkRenderWindowInteractor* rwi = this->Interactor;

            int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
            int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

            const int* size = this->CurrentRenderer->GetRenderWindow()->GetSize();

            double delta_elevation = -20.0 / size[1];
            double delta_azimuth   = -20.0 / size[0];

            double rxf = dx * delta_azimuth * this->MotionFactor;
            double ryf = dy * delta_elevation * this->MotionFactor;

            vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
            camera->Azimuth(rxf);          // 围绕ViewUp向量旋转
            camera->Elevation(ryf);        // 围绕ViewUp和viewPlaneNormal（焦点到相机位置的向量）构成面的法线旋转
            camera->OrthogonalizeViewUp(); // 围绕viewPlaneNormal旋转，其实是修改viewup的值

            // 如果相机的焦点为(0,0,0)，ViewUp为(0,1,0)，Position为(0,0,1)
            // Azimuth 就是绕y轴旋转
            // Elevation 就是绕x轴旋转
            // OrthogonalizeViewUp 就是绕z轴旋转

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
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main()
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST201

#ifdef TEST202

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnMouseMove() override
    {
        // 相机平移原理
        if (this->State != VTKIS_PAN)
        {
            Superclass::OnMouseMove();
            return;
        }

        if (this->Interactor && this->CurrentRenderer && this->CurrentRenderer->GetActiveCamera())
        {
            vtkRenderWindowInteractor* rwi = this->Interactor;

            double viewFocus[4], focalDepth, viewPoint[3];
            double newPickPoint[4], oldPickPoint[4], motionVector[3];

            // 计算焦点的深度值，其实这里不用计算也行，屏幕坐标转世界坐标不需要Z分量
            vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
            camera->GetFocalPoint(viewFocus);
            this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
            focalDepth = viewFocus[2];

            // 分别计算移动之前和移动之后的屏幕坐标
            this->ComputeDisplayToWorld(rwi->GetEventPosition()[0], rwi->GetEventPosition()[1], focalDepth, newPickPoint);
            this->ComputeDisplayToWorld(rwi->GetLastEventPosition()[0], rwi->GetLastEventPosition()[1], focalDepth, oldPickPoint);

            // 通过屏幕坐标的差值，反向运动相机
            motionVector[0] = oldPickPoint[0] - newPickPoint[0];
            motionVector[1] = oldPickPoint[1] - newPickPoint[1];
            motionVector[2] = oldPickPoint[2] - newPickPoint[2];

            camera->GetFocalPoint(viewFocus);
            camera->GetPosition(viewPoint);

            // 重新设置相机焦点和位置
            camera->SetFocalPoint(motionVector[0] + viewFocus[0], motionVector[1] + viewFocus[1], motionVector[2] + viewFocus[2]);
            camera->SetPosition(motionVector[0] + viewPoint[0], motionVector[1] + viewPoint[1], motionVector[2] + viewPoint[2]);

            if (rwi->GetLightFollowCamera())
            {
                this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
            }

            rwi->Render();
        }
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main()
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST202

#ifdef TEST203

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnMouseWheelForward() override
    {
        if (this->CurrentRenderer == nullptr)
        {
            return;
        }

        MyDolly(1.1);
    }

    void OnMouseWheelBackward() override
    {
        if (this->CurrentRenderer == nullptr)
        {
            return;
        }

        MyDolly(0.9);
    }

private:
    void MyDolly(double factor)
    {
        vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
        if (camera->GetParallelProjection())
        {
            // 如果是平行投影，设置ParallelScale即可实现缩放
            camera->SetParallelScale(camera->GetParallelScale() / factor);
        }
        else
        {
            // 如果是透视投影，沿着焦点到相机的方向移动相机即可实现缩放
            double d = camera->GetDistance() / factor;

            camera->SetPosition(camera->GetFocalPoint()[0] - d * camera->GetDirectionOfProjection()[0],
                camera->GetFocalPoint()[1] - d * camera->GetDirectionOfProjection()[1],
                camera->GetFocalPoint()[2] - d * camera->GetDirectionOfProjection()[2]);

            if (this->AutoAdjustCameraClippingRange)
            {
                this->CurrentRenderer->ResetCameraClippingRange();
            }
        }

        if (this->Interactor->GetLightFollowCamera())
        {
            this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
        }

        this->Interactor->Render();
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main()
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST203

#ifdef TEST204

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnMiddleButtonUp() override
    {
        Superclass::OnMiddleButtonUp();

        if (this->CurrentRenderer)
        {
            auto parallel = this->CurrentRenderer->GetActiveCamera()->GetParallelProjection();
            this->CurrentRenderer->GetActiveCamera()->SetParallelProjection(!parallel);
            this->Interactor->Render();
        }
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

// 在以下函数内计算投影矩阵
// void vtkCamera::ComputeProjectionTransform(double aspect, double nearz, double farz)

int main()
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 800);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST204

#ifdef TEST205

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVectorOperators.h>

namespace {

class CustomStyle : public vtkInteractorStyleRubberBand3D
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleRubberBand3D);

protected:
    void OnMouseWheelForward() override
    {
        this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

        if (this->CurrentRenderer)
        {
            auto mousePos = vtkVector2d(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
            ZoomByFocalPoint(mousePos, true);
        }
    }

    void OnMouseWheelBackward() override
    {
        this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

        if (this->CurrentRenderer)
        {
            auto mousePos = vtkVector2d(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
            ZoomByFocalPoint(mousePos, false);
        }
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
        // this->CurrentRenderer->ResetCameraClippingRange();
    }

    void TranslateCameraToFocalPoint(const vtkVector2d& point, bool forwardTo)
    {
        if (this->CurrentRenderer->GetActiveCamera()->GetParallelProjection())
        {
            auto camera     = this->CurrentRenderer->GetActiveCamera();
            auto size       = this->CurrentRenderer->GetSize();
            auto origin     = this->CurrentRenderer->GetOrigin();
            auto focalPoint = point;

            const vtkVector2d rbCenter2d    = vtkVector2d(focalPoint.GetX(), focalPoint.GetY());
            const vtkVector3d rbCenter3d    = vtkVector3d(rbCenter2d.GetX(), rbCenter2d.GetY(), 0.0);
            const vtkVector3d worldRBCenter = this->GetCurrentRenderer()->DisplayToWorld(rbCenter3d);

            const vtkVector2d winCenter2d    = vtkRecti(origin[0], origin[1], size[0], size[1]).GetCenter();
            const vtkVector3d winCenter3d    = vtkVector3d(winCenter2d.GetX(), winCenter2d.GetY(), 0.0);
            const vtkVector3d worldWinCenter = this->GetCurrentRenderer()->DisplayToWorld(winCenter3d);
            const vtkVector3d translation    = worldRBCenter - worldWinCenter;

            vtkVector3d pos, fp;
            camera->GetPosition(pos.GetData());
            camera->GetFocalPoint(fp.GetData());

            if (forwardTo)
            {
                pos = pos - translation;
                fp  = fp - translation;
            }
            else
            {
                pos = pos + translation;
                fp  = fp + translation;
            }

            camera->SetPosition(pos.GetData());
            camera->SetFocalPoint(fp.GetData());
        }
    }
};

vtkStandardNewMacro(CustomStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<CustomStyle> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST205

#ifdef TEST301

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {

class CustomStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleTrackballCamera);

protected:
    void OnMiddleButtonUp() override
    {
        if (this->Interactor && this->GetCurrentRenderer())
        {
            double range[2] {};
            this->GetCurrentRenderer()->GetActiveCamera()->GetClippingRange(range);
            std::cout << "Range: " << range[0] << ' ' << range[1] << '\n';
        }
        Superclass::OnMiddleButtonUp();
    }

    void OnLeftButtonDown() override
    {
        if (this->Interactor && this->GetCurrentRenderer())
        {
            // 根据可见的物体自动设置相机
            // 使用物体的包围盒边框调整相机，物体会显得小一些，窗口空洞一些
            this->GetCurrentRenderer()->ResetCamera();
        }
        Superclass::OnLeftButtonDown();
    }

    void OnRightButtonDown() override
    {
        if (this->Interactor && this->GetCurrentRenderer())
        {
            // 根据可见的物体自动设置相机
            // 使用屏幕空间边界框可以让物体更大一些，即窗口更饱满一些
            this->GetCurrentRenderer()->ResetCameraScreenSpace();
        }
        Superclass::OnRightButtonDown();
    }
};

vtkStandardNewMacro(CustomStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor(actor);
    renderer->GetActiveCamera()->ParallelProjectionOn(); // 平行投影
    // 将相机的焦点设置到所有物体构成的包围盒的中心，并移动到可以完整看到所有物体的位置
    renderer->ResetCamera();
    // 近裁剪平面与远裁剪平面的百分比，默认会根据深度值的字节数设置为0.01或0.001
    // 例如远裁剪平面是100，NearClippingPlaneTolerance是0.01，那近裁剪平面就是1
    // 当近裁剪平面小于1时会自动修正，大于1时不会修正
    renderer->SetNearClippingPlaneTolerance(0.001);
    // 重置裁剪范围时，近远平面分别向两边扩大 (range[1] - range[0]) * ClippingRangeExpansion，默认0.5
    renderer->SetClippingRangeExpansion(0.5);
    renderer->ResetCameraClippingRange(); // 手动更新裁剪范围

    // ResetCamera()之后，只有重绘才会更新range（前提是开启自动调整裁剪范围），否则即使设置相机位置等属性也不会修改裁剪范围
    double range[2] {};
    renderer->GetActiveCamera()->GetClippingRange(range);
    std::cout << "Range: " << range[0] << ' ' << range[1] << '\n';

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    // 开启每次渲染之前自动调整相机的裁剪范围以适应整个场景。仅仅调整裁剪范围，如果物体在相机的后面仍然会被裁剪
    vtkNew<CustomStyle> style;
    style->AutoAdjustCameraClippingRangeOn(); // 默认开启

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST301

#ifdef TEST302

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {

class CustomStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleTrackballCamera);

protected:
    void OnMiddleButtonUp() override
    {
        if (this->Interactor && this->GetCurrentRenderer())
        {
            double range[2] {};
            this->GetCurrentRenderer()->GetActiveCamera()->GetClippingRange(range);
            std::cout << "Range: " << range[0] << ' ' << range[1] << '\n';
        }
        Superclass::OnMiddleButtonUp();
    }
};

vtkStandardNewMacro(CustomStyle);
} // namespace

// 先添加一个vtkActor，通过鼠标进行各种变换后，相机处于一个特殊的位置，此时再添加一个vtkActor
// 如果相机的位置处于这个新添加的vtkActor的内部，则这个vtkActor就一直会被裁剪（除非重置相机适配窗口）
// 为了方便演示，示例代码直接将相机指定到四边形的内部，通过鼠标左键旋转即可看到被裁剪的效果

int main(int, char*[])
{
    vtkNew<vtkPlaneSource> source;
    source->SetResolution(99, 99);
    source->SetPoint1(100., 0., 0.);
    source->SetPoint2(0., 100., 0.);
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->ParallelProjectionOn();     // 平行投影
    renderer->GetActiveCamera()->SetPosition(50., 50., 0.);  // 将相机挪到指定位置，四边形的一部分在相机前面一部分在后面
    renderer->GetActiveCamera()->SetFocalPoint(0., 20., 0.); // 即使开启自动调整裁剪范围，物体的一部分将始终被裁剪
    renderer->GetActiveCamera()->SetViewUp(1., 0., 0.);

    double range[2] {};
    renderer->GetActiveCamera()->GetClippingRange(range);
    std::cout << "Range: " << range[0] << ' ' << range[1] << '\n';

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<CustomStyle> style;
    style->AutoAdjustCameraClippingRangeOn();

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST302

#ifdef TEST303

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVectorOperators.h>

namespace {

class CustomStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleTrackballCamera);

protected:
    void OnMiddleButtonUp() override
    {
        if (this->Interactor && this->GetCurrentRenderer())
        {
            auto camera = this->GetCurrentRenderer()->GetActiveCamera();

            double range[2] {};
            camera->GetClippingRange(range);
            std::cout << "Range: " << range[0] << ' ' << range[1] << '\n';

            double bounds[6] {};
            this->CurrentRenderer->ComputeVisiblePropBounds(bounds);

            // 立方体包围盒的中心点
            double centerX = (bounds[0] + bounds[1]) / 2.0;
            double centerY = (bounds[2] + bounds[3]) / 2.0;
            double centerZ = (bounds[4] + bounds[5]) / 2.0;

            std::cout << centerX << ' ' << centerY << ' ' << centerZ;

            // 立方体包围盒的半径
            double cubeRadiusX = (bounds[1] - bounds[0]) / 2.0;
            double cubeRadiusY = (bounds[3] - bounds[2]) / 2.0;
            double cubeRadiusZ = (bounds[5] - bounds[4]) / 2.0;

            // 计算球体的半径
            double sphereRadius = sqrt(cubeRadiusX * cubeRadiusX + cubeRadiusY * cubeRadiusY + cubeRadiusZ * cubeRadiusZ);

            vtkVector3d focalPoint {}, position {};
            camera->GetFocalPoint(focalPoint.GetData());
            camera->GetPosition(position.GetData());
            std::cout << "focal: " << focalPoint[0] << ' ' << focalPoint[1] << ' ' << focalPoint[2] << '\n';
            std::cout << "position: " << position[0] << ' ' << position[1] << ' ' << position[2] << '\n';

            auto angle    = vtkMath::RadiansFromDegrees(camera->GetViewAngle());
            auto distance = sphereRadius / std::sin(angle * 0.5); // 当焦点是立方体包围盒的中心时，相机到焦点的最小距离

            vtkVector3d p2f = position - focalPoint;
            vtkVector3d f2c = { focalPoint[0] - centerX, focalPoint[1] - centerY, focalPoint[2] - centerZ };

            // 如果相机到焦点的距离小于(distance + 焦点到中心)的距离，则将相机拉远，保证可以完整观察到整个场景
            // 加上f2c是为了保证：焦点恰好在p2f方向上，这时焦点比中心距离相机的位置更远
            if (p2f.Norm() < distance + f2c.Norm())
            {
                p2f.Normalize();
                vtkVector3d newPos = focalPoint + (distance + f2c.Norm()) * p2f;
                camera->SetPosition(newPos.GetData());
                this->CurrentRenderer->ResetCameraClippingRange();
            }
        }

        Superclass::OnMiddleButtonUp();
    }
};

vtkStandardNewMacro(CustomStyle);
} // namespace

// 先添加一个vtkActor，通过鼠标进行各种变换后，相机处于一个特殊的位置，此时再添加一个vtkActor
// 如果相机的位置处于这个新添加的vtkActor的内部，则这个vtkActor就一直会被裁剪（除非重置相机适配窗口）
// 为了方便演示，示例代码直接将相机指定到四边形的内部，通过鼠标左键旋转即可看到被裁剪的效果，中键调整相机位置重新设置裁剪范围
// 暂时仅支持平行投影

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkPlaneSource> source;
    source->SetResolution(99, 99);
    source->SetPoint1(100., 0., 0.);
    source->SetPoint2(0., 100., 0.);
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor(actor);
    renderer->AddActor(cubeActor);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->ParallelProjectionOn();    // 平行投影
    renderer->GetActiveCamera()->SetPosition(50., 50., 0.); // 将相机挪到指定位置，四边形的一部分在相机前面一部分在后面
    renderer->GetActiveCamera()->SetFocalPoint(0., 0., 0.); // 即使开启自动调整裁剪范围，物体的一部分将始终被裁剪
    renderer->GetActiveCamera()->SetViewUp(-1., 1., 0.);

    double range[2] {};
    renderer->GetActiveCamera()->GetClippingRange(range);
    std::cout << "Range: " << range[0] << ' ' << range[1] << '\n';

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<CustomStyle> style;
    style->AutoAdjustCameraClippingRangeOn();

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST303

#ifdef TEST401

#include <vtkActor.h>
#include <vtkBillboardTextActor3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>

// 底层原理：vtkBillboardTextActor3D::GenerateQuad
// 使用Actor的世界坐标Position先转化为屏幕坐标，再通过这个屏幕坐标生成四边形的四个屏幕坐标
// 再使用四个屏幕坐标转化为世界坐标，这四个坐标就是广告文本所在四边形坐标
// 将文本的纹理贴到这个四边形上就是最后的显示结果
// 相机每次变换后都会更新这个四边形的坐标

int main(int, char*[])
{
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(sphereSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetPosition(0, 0, 0);
    actor->GetProperty()->SetColor(0, 1, 0);

    //-----------------------------------------------------------------------
    // 广告牌
    vtkNew<vtkBillboardTextActor3D> textActor;
    textActor->SetInput("text");           // 显示的文本
    textActor->SetPosition(2, 2, 0);       // 广告牌位置
    textActor->SetDisplayOffset(0, -1e-6); // 设置偏移，可以让广告牌始终在最上面
    textActor->GetTextProperty()->SetFontSize(18);
    textActor->GetTextProperty()->SetColor(1, 0, 0);
    textActor->GetTextProperty()->SetJustificationToCentered(); // 文本左对齐

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor(actor);
    renderer->AddActor(textActor);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST401

#ifdef TEST402

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkFollower.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

// https://kitware.github.io/vtk-examples/site/Cxx/Visualization/Follower/

int main(int, char*[])
{
    vtkNew<vtkRenderer> renderer;

    //-----------------------------------------------------------
    // 始终朝向相机的标记
    vtkNew<vtkRegularPolygonSource> markerSource;
    markerSource->SetCenter(0, 0, 0); // 标记的位置始终设置为(0,0,0)
    markerSource->SetRadius(.1);
    markerSource->GeneratePolygonOn();
    markerSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(markerSource->GetOutput());

    vtkNew<vtkFollower> follower;
    follower->SetPosition(.5, .5, .5); // vtkFollower的位置设置为actor应该在的位置，这样相机旋转时，vtkFollower的位置就可以跟着变化
    follower->SetMapper(mapper);
    follower->GetProperty()->SetColor(1., 0., 0.);

    //-----------------------------------------------------------
    // 用来对比的立方体
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0., 1., 0.);
    cubeActor->GetProperty()->SetOpacity(0.3);

    //------------------------------------------------------------
    follower->SetCamera(renderer->GetActiveCamera());
    renderer->AddActor(follower);
    renderer->AddActor(cubeActor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST402

#ifdef TEST403

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkFollower.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

#include <iostream>

namespace {
class MyFollower : public vtkFollower
{
public:
    static MyFollower* New();
    vtkTypeMacro(MyFollower, vtkFollower);

protected:
    void ComputeMatrix() override
    {
        // check whether or not need to rebuild the matrix
        if (this->GetMTime() > this->MatrixMTime || (this->Camera && this->Camera->GetMTime() > this->MatrixMTime))
        {
            this->GetOrientation();
            this->Transform->Push();
            this->Transform->Identity();
            this->Transform->PostMultiply();

            this->Transform->Translate(-this->Origin[0], -this->Origin[1], -this->Origin[2]);
            // scale
            this->Transform->Scale(this->Scale[0], this->Scale[1], this->Scale[2]);

            // rotate
            this->Transform->RotateY(this->Orientation[1]);
            this->Transform->RotateX(this->Orientation[0]);
            this->Transform->RotateZ(this->Orientation[2]);

            if (this->Camera)
            {
                double *pos, *vup, distance;
                double Rx[3], Ry[3], Rz[3];

                vtkMatrix4x4* matrix = this->InternalMatrix;
                matrix->Identity();

                // do the rotation
                // first rotate y
                pos = this->Camera->GetPosition();
                vup = this->Camera->GetViewUp();

                if (this->Camera->GetParallelProjection())
                {
                    this->Camera->GetDirectionOfProjection(Rz);
                    Rz[0] = -Rz[0];
                    Rz[1] = -Rz[1];
                    Rz[2] = -Rz[2];
                }
                else
                {
                    distance = sqrt((pos[0] - this->Position[0]) * (pos[0] - this->Position[0])
                        + (pos[1] - this->Position[1]) * (pos[1] - this->Position[1]) + (pos[2] - this->Position[2]) * (pos[2] - this->Position[2]));
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
                // 自定义旋转矩阵
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

                //--------------------------------------------------------------
                // vtk源码的矩阵和自定义矩阵效果一致
                this->Transform->Concatenate(rotateMat);
                // this->Transform->Concatenate(matrix);
            }

            // translate to projection reference point PRP
            // this is the camera's position blasted through
            // the current matrix
            this->Transform->Translate(this->Origin[0] + this->Position[0], this->Origin[1] + this->Position[1], this->Origin[2] + this->Position[2]);

            // apply user defined matrix last if there is one
            if (this->UserMatrix)
            {
                this->Transform->Concatenate(this->UserMatrix);
            }

            this->Transform->PreMultiply();
            this->Transform->GetMatrix(this->Matrix);
            this->MatrixMTime.Modified();
            this->Transform->Pop();
        }
    }
};

vtkStandardNewMacro(MyFollower);

}; // namespace

// vtkFollower始终朝向相机的原理 vtkFollower::ComputeMatrix()

int main(int, char*[])
{
    //-----------------------------------------------------------
    // 始终朝向相机的标记
    vtkNew<vtkRegularPolygonSource> markerSource;
    markerSource->SetCenter(0, 0, 0);
    markerSource->SetRadius(.1);
    markerSource->GeneratePolygonOn();
    markerSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(markerSource->GetOutput());

    vtkNew<MyFollower> follower;
    follower->SetPosition(.5, .5, .5);
    follower->SetMapper(mapper);
    follower->GetProperty()->SetColor(1., 0., 0.);

    //-----------------------------------------------------------
    // 用来对比的立方体
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0., 1., 0.);
    cubeActor->GetProperty()->SetOpacity(0.3);

    //------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    follower->SetCamera(renderer->GetActiveCamera());
    renderer->AddActor(follower);
    renderer->AddActor(cubeActor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST13

#ifdef TEST501

#include <vtkActor.h>
#include <vtkDistanceToCamera.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// 每次执行缩放操作时，都会重新计算几何数据的顶点，然后提交一份新的顶点数据给GPU
// vtkDistanceToCamera会生成PointData的Array数据，vtkGlyph3D利用该Array数据重新计算新的顶点
// 让几何数据和相机的缩放相反，屏幕上显示的图形就好像没有被缩放一样

int main()
{
    vtkNew<vtkRenderer> renderer;

    //--------------------------------------------------------------
    // 用来对比的四边形
    vtkNew<vtkPlaneSource> plane;
    plane->Update();

    vtkNew<vtkPolyDataMapper> mapper_plane;
    mapper_plane->SetInputData(plane->GetOutput());

    vtkNew<vtkActor> actor_plane;
    actor_plane->SetMapper(mapper_plane);
    actor_plane->GetProperty()->SetColor(1, 0, 0);
    renderer->AddActor(actor_plane);

    //--------------------------------------------------------------
    // 不能缩放的绿色圆圈
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);

    vtkNew<vtkPointSet> pointset;
    pointset->SetPoints(points);

    vtkNew<vtkDistanceToCamera> disToCamera;
    disToCamera->ScalingOff();
    disToCamera->SetInputData(pointset);
    disToCamera->SetScreenSize(20.0); // 设置图元在屏幕上所占的像素大小，即图元始终以该像素大小显示，不会被缩放
    disToCamera->SetRenderer(renderer);
    disToCamera->Update();

    vtkNew<vtkRegularPolygonSource> source;
    source->SetNumberOfSides(10);
    source->GeneratePolygonOff();
    source->Update();

    std::cout << "Number of points: " << source->GetOutput()->GetNumberOfPoints() << std::endl
              << "Number of cells: " << source->GetOutput()->GetNumberOfCells() << std::endl;

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputConnection(disToCamera->GetOutputPort()); // 必须使用管道方式设置Input
    glyph->SetSourceData(source->GetOutput());               // 管道和数据集都可以
    glyph->ScalingOn();                                      // 开启缩放
    glyph->SetScaleModeToScaleByScalar();                    // 使用Scalar数据缩放，DistanceToCamera就是Scalar数据
    glyph->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera");
    glyph->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(glyph->GetOutputPort()); // 必须使用管道方式设置
    mapper->ScalarVisibilityOff();
    mapper->SetResolveCoincidentTopologyToPolygonOffset();
    mapper->SetRelativeCoincidentTopologyLineOffsetParameters(0, -1.e6); // 始终在上面

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    renderer->AddActor(actor);

    //-----------------------------------------------------------------------------
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST501

#ifdef TEST502

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDistanceToCamera.h>
#include <vtkFollower.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPlaneSource.h>
#include <vtkPointSet.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnChar() override
    {
        if (this->Interactor)
        {
            switch (this->Interactor->GetKeyCode())
            {
                case 'r':
                case 'R':
                {
                    auto key = this->Interactor->GetKeyCode();
                }
                break;
                default:
                    break;
            }
        }
        Superclass::OnChar();
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main()
{
    vtkNew<vtkRenderer> renderer;

    //--------------------------------------------------------------
    // （底下的图层）添加一个红色的矩形
    vtkNew<vtkPlaneSource> plane;
    plane->Update();

    vtkNew<vtkPolyDataMapper> mapper_plane;
    mapper_plane->SetInputData(plane->GetOutput());

    vtkNew<vtkActor> actor_plane;
    actor_plane->SetMapper(mapper_plane);
    actor_plane->GetProperty()->SetColor(1, 0, 0);
    renderer->AddActor(actor_plane);

    //--------------------------------------------------------------
    // （上层的图层）始终朝向相机的绿色圆圈
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    vtkNew<vtkPointSet> pointset;
    pointset->SetPoints(points);

    vtkNew<vtkDistanceToCamera> disToCamera;
    disToCamera->SetInputData(pointset);
    disToCamera->SetScreenSize(20.0);
    disToCamera->SetRenderer(renderer);

    vtkNew<vtkRegularPolygonSource> source;
    source->GeneratePolygonOff();
    source->SetNumberOfSides(30);
    source->Update();

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputConnection(disToCamera->GetOutputPort());
    glyph->SetSourceConnection(source->GetOutputPort());
    glyph->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "DistanceToCamera");
    glyph->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(glyph->GetOutputPort());
    mapper->ScalarVisibilityOff();
    mapper->SetResolveCoincidentTopologyToPolygonOffset();
    mapper->SetRelativeCoincidentTopologyLineOffsetParameters(0, -1.e6); // 始终在上面

    vtkNew<vtkFollower> actor;
    actor->SetCamera(renderer->GetActiveCamera());
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->UseBoundsOff(); // 将绿色的圈从边界范围中移除，防止绿色圈比实际的平面还大，导致ResetCamera时，无法一次适配窗口
    renderer->AddActor(actor);

    //-----------------------------------------------------------------------------
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<MyStyle> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST502

#ifdef TEST503

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCoordinate.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main()
{
    vtkNew<vtkRenderer> renderer;

    // 创建一个立方体
    {
        vtkNew<vtkCubeSource> source;
        source->SetCenter(0, 0, 0);
        source->Update();

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(source->GetOutput());

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);

        renderer->AddActor(actor);
    }

    //--------------------------------------------------------------------------
    // 绿色多边形，始终在屏幕上的指定位置，且在其他vtkActor3D的上面，不会被缩放、旋转、平移
    {
        vtkNew<vtkRegularPolygonSource> source;
        source->SetCenter(100., 0., 0.); // 屏幕坐标，Z坐标没有作用
        source->SetRadius(50.);          // 屏幕距离
        source->Update();

        vtkNew<vtkPolyDataMapper2D> mapper;
        mapper->SetInputData(source->GetOutput());

        vtkNew<vtkActor2D> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0, 1, 0);
        actor->SetPosition(200., 200.); // 屏幕坐标，加上面设置的Center就是最终屏幕坐标

        renderer->AddActor2D(actor);    // AddActor 和 AddActor2D 没有区别
    }

    //--------------------------------------------------------------------------
    // 蓝色多边形，始终在vtkActor3D之上，在这个vtkActor2D之前添加的vtkActor2D之上
    {
        vtkNew<vtkRegularPolygonSource> source;
        source->SetRadius(5.);
        source->SetCenter(1., 1., 0.);
        source->Update();

        // 坐标系设置为世界坐标系，就可以被旋转、缩放、平移
        // 还可以设置为其他坐标系：相机坐标系、归一化的屏幕坐标系等
        vtkNew<vtkCoordinate> coordinate;
        coordinate->SetCoordinateSystemToWorld();

        vtkNew<vtkPolyDataMapper2D> mapper;
        mapper->SetInputData(source->GetOutput());
        mapper->SetTransformCoordinate(coordinate);

        vtkNew<vtkActor2D> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0, 0, 1);

        renderer->AddActor(actor);
    }

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    // 创建交互器并启动渲染窗口
    vtkNew<vtkRenderWindowInteractor> interactor;
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    interactor->SetInteractorStyle(style);
    renderWindow->SetInteractor(interactor);

    renderWindow->Render();
    interactor->Start();

    return 0;
}

#endif // TEST503

#ifdef TEST504

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLegendScaleActor.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {

class WindowResizeCB : public vtkCommand
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
            auto size   = m_renderer->GetSize();
            auto factor = size[1] / m_height;
            auto scale  = m_renderer->GetActiveCamera()->GetParallelScale();
            m_height    = size[1];

            m_renderer->GetActiveCamera()->SetParallelScale(scale * factor);
            vtkRenderWindow::SafeDownCast(caller)->Render();
        }
    }

public:
    void SetRenderer(const vtkSmartPointer<vtkRenderer>& renderer)
    {
        m_renderer = renderer;
    }

    void SetBeginHeight(double h)
    {
        m_height = h;
    }

private:
    vtkSmartPointer<vtkRenderer> m_renderer { nullptr };
    double m_height { 600.0 };
};

vtkStandardNewMacro(WindowResizeCB);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    // 比例尺
    vtkNew<vtkLegendScaleActor> scaleActor;
    scaleActor->BottomAxisVisibilityOff();
    scaleActor->LeftAxisVisibilityOff();
    scaleActor->RightAxisVisibilityOff();
    scaleActor->TopAxisVisibilityOff();
    scaleActor->GetLegendLabelProperty();
    scaleActor->LegendVisibilityOn();

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(scaleActor);
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->ParallelProjectionOn(); // 平行投影

    vtkNew<WindowResizeCB> resizeCB;
    resizeCB->SetRenderer(renderer);
    resizeCB->SetBeginHeight(600.);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);
    renWin->AddObserver(vtkCommand::WindowResizeEvent, resizeCB);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST504

#ifdef TEST601

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCoordinate.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnLeftButtonDown() override
    {
        Superclass::OnLeftButtonDown();

        if (!this->Interactor || !this->CurrentRenderer)
        {
            return;
        }

        int displayPos[2] {};
        this->Interactor->GetEventPosition(displayPos); // 整个窗口的屏幕坐标，左下角(0,0)

        vtkNew<vtkCoordinate> coordinate;
        coordinate->SetValue(displayPos[0], displayPos[1], 0.);
        coordinate->SetCoordinateSystemToDisplay();

        auto worldPos = coordinate->GetComputedWorldValue(this->CurrentRenderer);       // 世界坐标
        auto viewportPos = coordinate->GetComputedViewportValue(this->CurrentRenderer); // viewport屏幕坐标，超出当前vtkRenderer时会有负值
        auto localDisplayPos = coordinate->GetComputedLocalDisplayValue(this->CurrentRenderer); // 左上角(0,0)屏幕坐标

        std::cout << "-------------------------------------------------\n";
        std::cout << "Display Position:\t" << displayPos[0] << ' ' << displayPos[1] << '\n';
        std::cout << "World Position:\t\t" << worldPos[0] << ' ' << worldPos[1] << ' ' << worldPos[2] << '\n';
        std::cout << "Viewport Position:\t" << viewportPos[0] << ' ' << viewportPos[1] << '\n';
        std::cout << "LocalDisplay Position:\t" << localDisplayPos[0] << ' ' << localDisplayPos[1] << '\n';
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->SetViewport(.2, .2, .8, .8); // 窗口是(1000,1000) viewport是(600,600)
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(1000, 1000);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST601

#ifdef TEST602

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCoordinate.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVector.h>
#include <vtkVectorOperators.h>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnLeftButtonDown() override
    {
        Superclass::OnLeftButtonDown();

        if (!this->Interactor || !this->CurrentRenderer)
        {
            return;
        }

        int eventPos[2] {};
        this->Interactor->GetEventPosition(eventPos);

        auto size   = this->CurrentRenderer->GetSize();
        auto camera = this->CurrentRenderer->GetActiveCamera();

        auto aspect  = this->CurrentRenderer->GetTiledAspectRatio();
        auto viewMat = camera->GetViewTransformMatrix();
        auto projMat = camera->GetProjectionTransformMatrix(aspect, 0., 1.);

        // 第一次显示时，立方体的前右上角顶点
        double point[] { .5, .5, .5, 1. };

        vtkNew<vtkMatrix4x4> vpMat;
        viewMat->Multiply4x4(projMat, viewMat, vpMat);

        // 世界坐标转NDC坐标
        auto displayPos = vpMat->MultiplyPoint(point);
        displayPos[0]   = displayPos[0] / displayPos[3];
        displayPos[1]   = displayPos[1] / displayPos[3];
        displayPos[2]   = displayPos[2] / displayPos[3];

        // NDC转屏幕坐标
        auto x = static_cast<int>((displayPos[0] + 1.) / 2. * size[0]);
        auto y = static_cast<int>((displayPos[1] + 1.) / 2. * size[1]);

        std::cout << "-------------------------------------------------\n";
        std::cout << "NDC position:\t\t" << displayPos[0] << ' ' << displayPos[1] << ' ' << displayPos[2] << '\n';
        std::cout << "event position:\t\t" << eventPos[0] << ' ' << eventPos[1] << '\n';
        std::cout << "display position:\t" << x << ' ' << y << '\n';
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    // for (vtkIdType i = 0; i < source->GetOutput()->GetNumberOfPoints(); ++i)
    // {
    //     auto pt = source->GetOutput()->GetPoint(i);
    //     std::cout << pt[0] << ' ' << pt[1] << ' ' << pt[2] << '\n';
    // }

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<MyStyle> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST602

#ifdef TEST603

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCoordinate.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnRightButtonDown() override
    {
        Superclass::OnRightButtonDown();

        if (!this->Interactor || !this->CurrentRenderer)
        {
            return;
        }

        int displayPos[2] {};
        this->Interactor->GetEventPosition(displayPos);

        vtkNew<vtkCoordinate> coordinate;
        coordinate->SetValue(displayPos[0], displayPos[1], 0.);
        coordinate->SetCoordinateSystemToDisplay();

        auto worldPos = coordinate->GetComputedWorldValue(this->CurrentRenderer);

        m_points->InsertNextPoint(worldPos);
        std::cout << "point: " << worldPos[0] << '\t' << worldPos[1] << '\t' << worldPos[2] << '\n';
    }

    void OnMiddleButtonUp() override
    {
        Superclass::OnMiddleButtonUp();

        if (!this->Interactor || !this->CurrentRenderer)
        {
            return;
        }

        vtkNew<vtkLineSource> line;
        line->SetPoints(m_points);
        line->Update();

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(line->GetOutput());

        static vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1., 0., 0.);

        this->CurrentRenderer->AddActor(actor);
        this->Interactor->Render();
    }

private:
    vtkSmartPointer<vtkPoints> m_points { vtkSmartPointer<vtkPoints>::New() };
};

vtkStandardNewMacro(MyStyle);
} // namespace

// 无论怎样变换相机，在相机状态相同时，拾取到的所有点都在一个平面上（相机近裁剪平面）

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST603

#ifdef TEST604

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCoordinate.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVector.h>
#include <vtkVectorOperators.h>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnLeftButtonDown() override
    {
        Superclass::OnLeftButtonDown();

        if (!this->Interactor || !this->CurrentRenderer)
        {
            return;
        }

        int displayPos[2] {};
        this->Interactor->GetEventPosition(displayPos); // 整个窗口的屏幕坐标，左下角(0,0)

        auto size   = this->CurrentRenderer->GetSize();
        auto camera = this->CurrentRenderer->GetActiveCamera();

        vtkVector3d cameraPos, cameraFocalPos;
        camera->GetPosition(cameraPos.GetData());
        camera->GetFocalPoint(cameraFocalPos.GetData());

        double clipRange[2] {};
        camera->GetClippingRange(clipRange);

        auto cameraDir     = -(cameraPos - cameraFocalPos).Normalized();
        auto nearClipPlane = cameraPos + cameraDir * clipRange[0];

        vtkNew<vtkCoordinate> coordinate;
        coordinate->SetValue(displayPos[0], displayPos[1], 0.);
        coordinate->SetCoordinateSystemToDisplay();

        // 世界坐标的z分量就是相机的近裁剪平面对应的z值
        auto worldPos = coordinate->GetComputedWorldValue(this->CurrentRenderer); // 世界坐标

        //--------------------------------------------------------------------------------------
        // 自己实现屏幕坐标转世界坐标（暂时不支持透视投影，vtkRenderer不填满整个窗口
        // 屏幕坐标在vtkRenderer中的标准坐标[-1,1]
        double NViewPos[4] = { (displayPos[0] * 2. - size[0]) / size[0], (displayPos[1] * 2. - size[1]) / size[1], 0., 1. };

        auto aspect  = this->CurrentRenderer->GetTiledAspectRatio();
        auto viewMat = camera->GetViewTransformMatrix();
        auto projMat = camera->GetProjectionTransformMatrix(aspect, 0., 1.);

        vtkNew<vtkMatrix4x4> invertViewMat;
        vtkNew<vtkMatrix4x4> invertProjMat;
        vtkMatrix4x4::Invert(viewMat, invertViewMat);
        vtkMatrix4x4::Invert(projMat, invertProjMat);

        auto projPos = invertProjMat->MultiplyPoint(NViewPos);
        projPos[0]   = projPos[0] / projPos[3];
        projPos[1]   = projPos[1] / projPos[3];
        projPos[2]   = projPos[2] / projPos[3];

        // 屏幕坐标转世界坐标
        auto worldPos2 = invertViewMat->MultiplyPoint(projPos);
        worldPos2[0]   = worldPos2[0] / worldPos2[3];
        worldPos2[1]   = worldPos2[1] / worldPos2[3];
        worldPos2[2]   = worldPos2[2] / worldPos2[3];

        std::cout << "-------------------------------------------------\n";
        std::cout << "Display Position:\t" << displayPos[0] << ' ' << displayPos[1] << '\n';
        std::cout << "World Position:\t\t" << worldPos[0] << ' ' << worldPos[1] << ' ' << worldPos[2] << '\n';
        std::cout << "World Position2:\t" << worldPos2[0] << ' ' << worldPos2[1] << ' ' << worldPos2[2] << '\n';
        std::cout << "Near clip plane:\t" << nearClipPlane[2] << '\n';
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();
    // renderer->GetActiveCamera()->SetClippingRange(.2, 100.);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<MyStyle> style;
    // style->AutoAdjustCameraClippingRangeOff();

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST604

#ifdef TEST605

#include <iostream>
#include <vtkMatrix4x4.h>
#include <vtkPerspectiveTransform.h>
#include <vtkSmartPointer.h>

int main()
{
    auto print = [](vtkMatrix4x4* matrix)
    {
        for (auto i = 0u; i < 4; ++i)
        {
            for (auto j = 0u; j < 4; ++j)
            {
                std::cout << matrix->GetElement(j, i) << '\t';
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    };

    vtkNew<vtkPerspectiveTransform> transform;

    transform->Identity();
    transform->Ortho(-100, 100, -100, 100, -1, 1);
    print(transform->GetMatrix());

    transform->Identity();
    transform->Perspective(45., 8. / 6., 0.1, 100.);
    print(transform->GetMatrix());

    transform->Identity();
    transform->Frustum(-100, 100, -100, 100, -1, 1);
    print(transform->GetMatrix());
}

#endif // TEST605
