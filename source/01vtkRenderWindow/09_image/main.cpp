/*
 * 47.保存图片 (位图，矢量图）  qt框选截图 https://blog.csdn.net/GoForwardToStep/article/details/54237689
 * 48.矢量图svg保存
 * 49.vtkImageView  https://blog.csdn.net/liushao1031177/article/details/115799500
 * 50.保存矢量图svg文件
 * 52.左右两个窗口，一个拖动选择框，一个显示框选的内容（官方例子）vtkBorderWidget   坐标转换  截图
 * 53.vtkImageClip 裁剪图片
 * 54.vtkImageBlend 合成图片，利用多个图片的z值，将多张图片放在一起
 * 55.logowidget  vtkImageCanvasSource2D
 * 56.vtkImageBlend vtkImageCanvasSource2D 将多张图片叠加在一起，可以实现截图框选中为原图颜色，没选中的加阴影
 * 57.图片铺满整个窗口 图片占满(充满整个屏幕) https://kitware.github.io/vtk-examples/site/Cxx/Images/FillWindow/
 * 63.vtkImageData 转 QImage
 */

#define TEST47

#ifdef TEST47

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vtkResizingWindowToImageFilter.h>
#include <vtkWindowToImageFilter.h>

#include <vtkBMPWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkVRMLExporter.h>

#include <vtkImageLuminance.h>

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cube;

    // mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    // actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1, 0, 0);

    // camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);   // 设置相机位置
    camera->SetFocalPoint(0, 0, 0); // 设置相机焦点

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(0.2, 0.3, 0.4);
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600); // 设置window大小

    vtkNew<vtkWindowToImageFilter> windowToImage;
    windowToImage->SetInput(renWin);
    windowToImage->SetScale(2, 3);
    windowToImage->Update();

    vtkNew<vtkResizingWindowToImageFilter> img;
    img->SetInput(renWin);
    img->SetSize(1200, 1200);
    img->Update();
    // img->SetProgressShiftScale();

    // vtkNew<vtkWindowToImageFilter> img;
    // img->SetInput(renWin);
    // img->SetScale(100, 100);
    // img->Update();

    vtkNew<vtkImageLuminance> luminance;
    luminance->SetInputData(img->GetOutput());
    luminance->Update();

    // jpg
    // vtkNew<vtkJPEGWriter> jpg;
    // jpg->SetInputData(img->GetOutput());
    ////jpg->SetFileName("test");  //不会补全.jpg，但是保存的图片名加上.jpg以后也可以显示
    // jpg->SetFileName("test.jpg");
    // jpg->Write();

    // jpg->SetInputData(luminance->GetOutput());
    // jpg->SetFileName("test1.jpg");
    // jpg->Write();

    //// tiff
    // vtkNew<vtkTIFFWriter> tiff;
    // tiff->SetInputData(img->GetOutput());
    // tiff->SetFileName("test.tiff");
    // tiff->Write();

    // vtkNew<vtkTIFFWriter> tiff1;
    // tiff1->SetInputData(luminance->GetOutput());
    // tiff1->SetFileName("test1.tiff");
    // tiff1->Write();

    //// png
    // vtkNew<vtkPNGWriter> png;
    // png->SetInputData(img->GetOutput());
    // png->SetFileName("test.png");
    // png->Write();

    // vtkNew<vtkPNGWriter> png1;
    // png1->SetInputData(luminance->GetOutput());
    // png1->SetFileName("test1.png");
    // png1->Write();

    //// bmp
    // vtkNew<vtkBMPWriter> bmp;
    // bmp->SetInputData(img->GetOutput());
    // bmp->SetFileName("test.bmp");
    // bmp->Write();

    // vtkNew<vtkBMPWriter> bmp1;
    // bmp1->SetInputData(luminance->GetOutput());
    // bmp1->SetFileName("test1.bmp");
    // bmp1->Write();

    // *.ps 矢量图
    vtkNew<vtkPostScriptWriter> ps;
    ps->SetInputData(img->GetOutput());
    ps->SetFileName("test.ps");
    ps->Write();

    std::string exportFileName = "test.wrl";
    vtkNew<vtkVRMLExporter> exporter;
    exporter->SetFileName(exportFileName.c_str());
    exporter->SetActiveRenderer(renderer);
    exporter->SetRenderWindow(renWin);
    exporter->Update();

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TSET47

#ifdef TEST48

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkChartXYZ.h>
#include <vtkContext2D.h>
#include <vtkContext3D.h>
#include <vtkContextActor.h>
#include <vtkContextItem.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkCubeSource.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlotLine3D.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererSource.h>
#include <vtkSVGExporter.h>
#include <vtkTable.h>

//----------------------------------------------------------------------------
int main(int, char*[])
{
    vtkNew<vtkCubeSource> cube;

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    // actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    // renderer->Render();

    // Set up a 3D scene and add an XYZ chart to it.
    vtkNew<vtkContextView> view;
    view->GetRenderWindow()->SetSize(640, 480);
    view->GetRenderWindow()->SetWindowName("PlotLine3D");
    view->GetRenderer()->SetBackground(1.0, 1.0, 0.5);
    view->GetRenderer()->SetBackground2(1.0, 0.5, 1.0);
    view->GetRenderer()->SetBackgroundAlpha(0.5);
    view->GetRenderer()->GradientBackgroundOn();

    // view->GetRenderer()->AddActor(cubeActor);
    // view->GetRenderWindow()->SetMultiSamples(10);

    // vtkNew<vtkCamera> camera;
    // camera->SetPosition(1, 1, 1);//设置相机位置
    // camera->SetFocalPoint(0, 0, 0);//设置相机焦点

    // view->GetRenderer()->SetActiveCamera(camera);
    // view->GetRenderer()->ResetCamera();

    // vtkNew<vtkContext2D> context2D;
    // vtkNew<vtkContextItem> contextItem;
    // vtkNew<vtkContextActor> contextActor;

    vtkNew<vtkRendererSource> renderSource;
    renderSource->SetInput(renderer);
    auto imageData1 = renderSource->GetOutput();

    // view->GetScene()->AddItem(contextItem);
    view->GetContext()->DrawImage(0, 0, imageData1);

    view->GetRenderWindow()->Render();
    view->GetRenderer()->Render();
    vtkNew<vtkSVGExporter> svg;
    svg->SetRenderWindow(view->GetRenderWindow());
    // svg->SetActiveRenderer(view->GetRenderer());
    svg->SetFileName("contextViewSvg.svg");
    svg->Write();

    // view->GetInteractor()->Initialize();
    // view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

#endif // TEST48

#ifdef TEST49

#include "vtkHDRReader.h"
#include "vtkImageData.h"
#include "vtkImageViewer.h"
#include "vtkNew.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include <vtkCallbackCommand.h>
#include <vtkImageViewer2.h>
#include <vtkJPEGReader.h>

#include <iostream>

int main(int argc, char* argv[])
{
    std::string file = "test.jpg";
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName(file.c_str());
    jPEGReader->Update();

    // Visualize
    vtkNew<vtkImageViewer2> imageViewer;
    imageViewer->SetInputData(jPEGReader->GetOutput());

    imageViewer->SetColorWindow(1620);
    imageViewer->SetColorLevel(1080);
    // imageViewer->SetSlice(50);
    // imageViewer->SetSliceOrientationToXY();
    // imageViewer->SetPosition(0, 100);

    // vtkNew<ColorCallback> getColorWindow;
    // getColorWindow->imageViewer = imageViewer;

    vtkNew<vtkRenderWindowInteractor> iren;
    imageViewer->SetupInteractor(iren);
    imageViewer->GetRenderWindow()->SetWindowName("HDRReader");
    imageViewer->Render();

    // iren->AddObserver(vtkCommand::EndInteractionEvent, getColorWindow);

    iren->Start();

    auto color1 = imageViewer->GetColorLevel();
    auto color2 = imageViewer->GetColorWindow();

    std::cout << color1 << "," << color2 << "\n";

    std::cout << imageViewer->GetRenderWindow()->GetSize()[0] << ",";
    std::cout << imageViewer->GetRenderWindow()->GetSize()[1];

    // system("pause");
    return EXIT_SUCCESS;
}

#endif // TEST49

#ifdef TEST50

#include "vtkSVGExporter.h"

#include "vtkAbstractMapper.h"
#include "vtkBrush.h"
#include "vtkCellArray.h"
#include "vtkContext2D.h"
#include "vtkContextItem.h"
#include "vtkContextScene.h"
#include "vtkContextView.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLContextDevice2D.h"
#include "vtkPen.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPoints2D.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkRTAnalyticSource.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkTestingInteractor.h"
#include "vtkTextProperty.h"
#include "vtkTransform2D.h"
#include "vtkTriangle.h"
#include "vtkUnsignedCharArray.h"

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <string>

//------------------------------------------------------------------------------
class ContextSVGTest : public vtkContextItem
{
    void SetSpritePoint(int x, int y, vtkImageData* sprite);

public:
    static ContextSVGTest* New();
    vtkTypeMacro(ContextSVGTest, vtkContextItem);
    // Paint event for the chart, called whenever the chart needs to be drawn
    bool Paint(vtkContext2D* painter) override;
};

void drawPolyLinePolyData(vtkContext2D* painter)
{
    // Setup points
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(50.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 50.0, 0.0);
    points->InsertNextPoint(50.0, 0.0, 0.0);

    // Define some colors
    unsigned char black[4] = { 0, 0, 0, 255 };

    // Setup the colors array
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(4);
    colors->SetName("Colors");

    // Add the three colors we have created to the array
    colors->InsertNextTypedTuple(black);
    colors->InsertNextTypedTuple(black);
    colors->InsertNextTypedTuple(black);
    colors->InsertNextTypedTuple(black);

    vtkNew<vtkCellArray> polylines;
    vtkNew<vtkPolyLine> polyline;
    polyline->GetPointIds()->SetNumberOfIds(4);
    polyline->GetPointIds()->SetId(0, 0);
    polyline->GetPointIds()->SetId(1, 1);
    polyline->GetPointIds()->SetId(2, 2);
    polyline->GetPointIds()->SetId(3, 3);
    polylines->InsertNextCell(polyline);

    // Create a polydata object and add everything to it
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetLines(polylines);
    painter->GetPen()->SetWidth(2.0);
    painter->DrawPolyData(475, 200, polydata, colors, VTK_SCALAR_MODE_USE_POINT_DATA);
}

int main(int, char*[])
{

    vtkNew<vtkCubeSource> cube;

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    // actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1, 0, 0);

    // Set up a 2D context view, context test object and add it to the scene
    vtkNew<vtkContextView> view;
    view->GetRenderer()->SetBackground(1.0, 1.0, 0.5);
    view->GetRenderer()->SetBackground2(1.0, 0.5, 1.0);
    view->GetRenderer()->SetBackgroundAlpha(0.5);
    view->GetRenderer()->GradientBackgroundOn();
    view->GetRenderWindow()->SetSize(800, 600);
    // view->GetRenderWindow()->GetGenericContext();

    vtkNew<ContextSVGTest> test;
    view->GetScene()->AddItem(test);
    view->GetRenderer()->AddActor(cubeActor);
    // Force the use of the freetype based rendering strategy
    vtkOpenGLContextDevice2D::SafeDownCast(view->GetContext()->GetDevice())->SetStringRendererToFreeType();

    view->GetRenderWindow()->SetMultiSamples(0);
    view->GetRenderWindow()->Render();

    vtkNew<vtkSVGExporter> exp;
    exp->SetRenderWindow(view->GetRenderWindow());
    exp->SetFileName("test111.svg");
    exp->Write();

#if 0
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(view->GetRenderWindow());
    view->GetRenderWindow()->GetInteractor()->Initialize();
    view->GetRenderWindow()->GetInteractor()->Start();
#endif

    return EXIT_SUCCESS;
}

// Make our new derived class to draw a diagram
vtkStandardNewMacro(ContextSVGTest);

// This function aims to test the primitives provided by the 2D API.
bool ContextSVGTest::Paint(vtkContext2D* painter)
{

    // Reset painter state that we care about:
    painter->GetBrush()->SetTexture(nullptr);
    painter->GetBrush()->SetColor(0, 0, 0, 255);
    painter->GetPen()->SetColor(0, 0, 0, 255);
    painter->GetPen()->SetWidth(1.f);
    painter->GetTextProp()->SetUseTightBoundingBox(1);
    painter->GetTextProp()->SetOrientation(0.);
    painter->GetTextProp()->SetVerticalJustificationToCentered();
    painter->GetTextProp()->SetJustificationToCentered();
    painter->GetTextProp()->SetColor(0.0, 0.0, 0.0);
    painter->GetTextProp()->SetOpacity(1.);
    painter->GetTextProp()->SetFontSize(24);
    painter->GetTextProp()->SetBold(0);
    painter->GetTextProp()->SetItalic(0);
    painter->GetTextProp()->SetFontFamilyToArial();

    // Now to draw some points
    painter->GetPen()->SetColor(0, 0, 255);
    painter->GetPen()->SetWidth(5.0);
    painter->DrawPoint(10, 10);
    painter->DrawPoint(790, 10);
    painter->DrawPoint(10, 590);
    painter->DrawPoint(790, 590);

    // Test the string drawing functionality of the context
    painter->DrawString(400, 25, "SVG is used as a backend to the context.");
    painter->GetTextProp()->SetFontFamilyToTimes();
    painter->GetTextProp()->SetColor(1, 0.2, 0.1);
    painter->GetTextProp()->SetOpacity(0.5);
    painter->GetTextProp()->SetOrientation(-38.);
    painter->GetTextProp()->SetJustificationToRight();
    painter->GetTextProp()->SetVerticalJustificationToCentered();
    painter->DrawString(475, 250, "Testing multi-\nline justified\nand rotated text.");

    drawPolyLinePolyData(painter);

    // Draw some individual lines of different thicknesses.
    for (int i = 0; i < 10; ++i)
    {
        painter->GetPen()->SetColor(255, i * 25, 0);
        painter->GetPen()->SetWidth(1.f + static_cast<float>(i));
        painter->DrawLine(10, 50 + float(i) * 10, 60, 50 + float(i) * 10);
    }

    // Draw some individual lines of different thicknesses.
    painter->GetPen()->SetWidth(10);
    for (int i = 0; i < 10; ++i)
    {
        painter->GetPen()->SetLineType(i % (vtkPen::DENSE_DOT_LINE + 1));
        painter->GetPen()->SetColor(static_cast<unsigned char>(float(i) * 25.0), 255, 128);
        painter->DrawLine(10, 250 + float(i) * 10, 60, 250 + float(i) * 10);
    }
    painter->GetPen()->SetLineType(vtkPen::SOLID_LINE);

    // Use the draw lines function now to draw a shape.
    vtkSmartPointer<vtkPoints2D> points = vtkSmartPointer<vtkPoints2D>::New();
    points->SetNumberOfPoints(30);
    for (int i = 0; i < 30; ++i)
    {
        double point[2] = { float(i) * 25.0 + 10.0, sin(float(i) / 5.0) * 100.0 + 200.0 };
        points->SetPoint(i, point);
    }
    painter->GetPen()->SetColor(0, 255, 0);
    painter->GetPen()->SetWidth(5.0);
    painter->DrawPoly(points);

    // Test the markers
    float markerPoints[10 * 2];
    unsigned char markerColors[10 * 4];
    for (int i = 0; i < 10; ++i)
    {
        markerPoints[2 * i]     = 500.0 + i * 30.0;
        markerPoints[2 * i + 1] = 20 * sin(markerPoints[2 * i]) + 375.0;

        markerColors[4 * i]     = static_cast<unsigned char>(255 * i / 10.0);
        markerColors[4 * i + 1] = static_cast<unsigned char>(255 * (1.0 - i / 10.0));
        markerColors[4 * i + 2] = static_cast<unsigned char>(255 * (0.3));
        markerColors[4 * i + 3] = static_cast<unsigned char>(255 * (1.0 - ((i / 10.0) * 0.25)));
    }

    for (int style = VTK_MARKER_NONE + 1; style < VTK_MARKER_UNKNOWN; ++style)
    {
        // Increment the y values:
        for (int i = 0; i < 10; ++i)
        {
            markerPoints[2 * i + 1] += 35.0;
        }
        painter->GetPen()->SetWidth(style * 5 + 5);
        // Not highlighted:
        painter->DrawMarkers(style, false, markerPoints, 10, markerColors, 4);
        // Highlight the middle 4 points.
        painter->GetPen()->SetColorF(0.9, 0.8, 0.1, 0.5);
        painter->DrawMarkers(style, true, markerPoints + 3 * 2, 4);
    }

    // Draw some points of different widths.
    for (int i = 0; i < 10; ++i)
    {
        painter->GetPen()->SetColor(0, static_cast<unsigned char>(float(i) * 25.0), 255, 255);
        painter->GetPen()->SetWidth(1.0 + float(i));
        painter->DrawPoint(75, 50 + float(i) * 10);
    }

    painter->GetPen()->SetColor(0, 0, 255);
    painter->GetPen()->SetWidth(3.0);
    painter->DrawPoints(points);

    painter->GetPen()->SetColor(100, 200, 255);
    painter->GetPen()->SetWidth(3.0);
    painter->GetBrush()->SetColor(100, 255, 100);
    painter->DrawRect(100, 50, 200, 100);

    // Add in an arbitrary quad
    painter->GetPen()->SetColor(159, 0, 255);
    painter->GetPen()->SetWidth(1.0);
    painter->GetBrush()->SetColor(100, 55, 0, 200);
    painter->DrawQuad(350, 50, 375, 150, 525, 199, 666, 45);

    // Now to test out the transform...
    vtkNew<vtkTransform2D> transform;
    transform->Translate(145, 385);
    transform->Scale(0.25, 0.25);
    transform->Rotate(-45.);
    painter->PushMatrix();
    painter->SetTransform(transform);
    painter->GetPen()->SetColor(255, 0, 0);
    painter->GetPen()->SetWidth(6.0);
    painter->DrawPoly(points);

    transform->Identity();
    transform->Translate(0, 10);
    painter->AppendTransform(transform);
    painter->GetPen()->SetColor(0, 0, 200);
    painter->GetPen()->SetWidth(2.0);
    painter->DrawPoints(points);

    transform->Identity();
    transform->Translate(0, -20);
    painter->AppendTransform(transform);
    painter->GetPen()->SetColor(200, 0, 100);
    painter->GetPen()->SetWidth(5.0);
    painter->DrawPoints(points);

    transform->Identity();
    transform->Translate(20, 200);
    painter->SetTransform(transform);
    painter->GetPen()->SetColor(0, 0, 0);
    painter->GetPen()->SetWidth(1.0);
    painter->GetBrush()->SetColor(0, 0, 100, 69);
    // Draws smooth path (Full circle, testing oddball angles):
    painter->DrawEllipseWedge(100.0, 89.0, 20, 100, 15, 75, -26.23, 333.77);
    painter->DrawEllipseWedge(100.0, 89.0, 15, 15, 0, 0, -26.23, 333.77);
    painter->DrawEllipseWedge(125.0, 89.0, 20, 100, 0, 0, -26.23, 333.77);
    // Parital circle, more odd angles:
    painter->DrawEllipseWedge(150.0, 89.0, 20, 100, 15, 75, 403.f, 541.f);
    painter->DrawEllipseWedge(150.0, 89.0, 15, 75, 0, 0, 181.f, 403.f);
    // Smooth path:
    painter->DrawEllipticArc(100.0, 89.0, 20, 100, 0, 360);
    painter->DrawEllipticArc(100.0, 89.0, 15, 15, 0, 360);
    // Partial path:
    painter->DrawEllipticArc(150.0, 89.0, 20, 100, 43, 181);

    // Remove the transform:
    painter->PopMatrix();

    // Toss some images in:
    vtkNew<vtkRTAnalyticSource> imageSrc;
    imageSrc->SetWholeExtent(0, 49, 0, 49, 0, 0);
    imageSrc->SetMaximum(1.0);
    imageSrc->Update();
    vtkImageData* image = imageSrc->GetOutput();

    // convert to RGB bytes:
    vtkFloatArray* vals = static_cast<vtkFloatArray*>(image->GetPointData()->GetScalars());
    float imgRange[2];
    vals->GetValueRange(imgRange);
    float invRange                = 1.f / (imgRange[1] - imgRange[0]);
    vtkUnsignedCharArray* scalars = vtkUnsignedCharArray::New();
    scalars->SetNumberOfComponents(3);
    scalars->SetNumberOfTuples(vals->GetNumberOfTuples());
    for (vtkIdType i = 0; i < vals->GetNumberOfTuples(); ++i)
    {
        float val = vals->GetValue(i);
        val       = (val - imgRange[0]) * invRange; // normalize to (0, 1)
        scalars->SetComponent(i, 0, val * 255);
        scalars->SetComponent(i, 1, (1.f - val) * 255);
        scalars->SetComponent(i, 2, (val * val) * 255);
    }
    image->GetPointData()->SetScalars(scalars);
    scalars->Delete();
    painter->DrawImage(10, 525, image);
    painter->DrawImage(65, 500, 2.f, image);
    painter->DrawImage(vtkRectf(170, 537.5f, 25, 25), image);

    // Test transparent text over geometry:
    painter->GetTextProp()->SetOrientation(0.);
    painter->GetTextProp()->SetFontSize(175);
    painter->GetTextProp()->SetColor(1., 0., 0.);
    painter->GetTextProp()->SetOpacity(0.25);
    painter->GetTextProp()->SetBold(1);
    painter->GetTextProp()->SetJustificationToCentered();
    painter->GetTextProp()->SetVerticalJustificationToCentered();
    painter->DrawString(600, 450, "T");

    // Test text alignment:
    float alignX = 600.f;
    float alignY = 250.f;
    float alignW = 100.f * 0.5f;
    float alignH = 50.f * 0.5f;
    painter->GetPen()->SetWidth(1.f);
    painter->GetPen()->SetColor(0, 0, 0, 255);
    painter->DrawLine(alignX, alignY - alignH, alignX, alignY + alignH);
    painter->DrawLine(alignX - alignW, alignY, alignX + alignW, alignY);
    painter->GetTextProp()->SetFontSize(32);
    painter->GetTextProp()->SetJustificationToRight();
    painter->GetTextProp()->SetVerticalJustificationToBottom();
    painter->DrawString(alignX, alignY, "dag");
    painter->GetTextProp()->SetJustificationToLeft();
    painter->GetTextProp()->SetVerticalJustificationToBottom();
    painter->DrawString(alignX, alignY, "dig");
    painter->GetTextProp()->SetJustificationToRight();
    painter->GetTextProp()->SetVerticalJustificationToTop();
    painter->DrawString(alignX, alignY, "dog");
    painter->GetTextProp()->SetJustificationToLeft();
    painter->GetTextProp()->SetVerticalJustificationToTop();
    painter->DrawString(alignX, alignY, "dug");
    // Centering:
    float rect[4];
    const char* centerString = "Center";
    painter->ComputeStringBounds(centerString, rect);
    rect[0] += 350.f;
    rect[1] += 550.f;
    painter->GetBrush()->SetColor(0, 0, 0, 0);
    painter->DrawRect(rect[0], rect[1], rect[2], rect[3]);
    painter->GetTextProp()->SetJustificationToCentered();
    painter->GetTextProp()->SetVerticalJustificationToCentered();
    painter->DrawString(rect[0] + rect[2] * 0.5f, rect[1] + rect[3] * 0.5f, centerString);

    // Texturing:
    vtkNew<vtkImageData> pattern;
    pattern->SetDimensions(6, 6, 1);
    vtkNew<vtkUnsignedCharArray> patternScalars;
    patternScalars->SetNumberOfComponents(3);
    patternScalars->SetNumberOfTuples(36);
    patternScalars->FillValue(0);
    for (vtkIdType row = 0; row < 6; ++row)
    {
        for (vtkIdType col = 0; col < 6; ++col)
        {
            // Alternate red/blue:
            vtkIdType i    = row * 6 + col;
            vtkIdType test = row + col;
            patternScalars->SetTypedComponent(i, (test % 2 == 0) ? 0 : 2, 255);
        }
    }
    pattern->GetPointData()->SetScalars(patternScalars);
    painter->GetBrush()->SetTexture(pattern);
    painter->GetBrush()->SetOpacity(0);

    // Stretching:
    painter->GetBrush()->SetTextureProperties(vtkBrush::Nearest | vtkBrush::Stretch);
    painter->DrawQuad(200, 485, 300, 400, 190, 420, 125, 390);

    // Tiling:
    painter->GetBrush()->SetTextureProperties(vtkBrush::Linear | vtkBrush::Repeat);
    painter->DrawQuad(300, 585, 400, 500, 290, 520, 230, 560);

    painter->GetBrush()->SetTexture(nullptr);

    // Some point sprites:
    vtkNew<vtkImageData> sprite;
    sprite->SetDimensions(25, 25, 1);
    vtkNew<vtkUnsignedCharArray> spriteScalars;
    spriteScalars->SetNumberOfComponents(3);
    spriteScalars->SetNumberOfTuples(25 * 25);
    spriteScalars->FillValue(0);
    sprite->GetPointData()->SetScalars(spriteScalars);
    std::vector<float> spritePoints;
    spritePoints.reserve(50);
    std::vector<unsigned char> spriteColors;
    spriteColors.reserve(100);
    for (int i = 0; i < 25; ++i)
    {
        this->SetSpritePoint(i, 0, sprite);
        this->SetSpritePoint(0, i, sprite);
        this->SetSpritePoint(i, i, sprite);
        this->SetSpritePoint(10, i, sprite);
        this->SetSpritePoint(i, 10, sprite);
        spritePoints.push_back(790.f);                                           // x
        spritePoints.push_back(50.f + i * 20);                                   // y
        spriteColors.push_back(static_cast<unsigned char>(127 + 128 / (i + 1))); // r
        spriteColors.push_back(static_cast<unsigned char>(255 - 128 / (i + 1))); // g
        spriteColors.push_back(static_cast<unsigned char>(64 + 128 / (i + 1)));  // b
        spriteColors.push_back(static_cast<unsigned char>(64 + 191 / (i + 1)));  // a
    }
    for (int i = 0; i < 10; ++i)
    {
        this->SetSpritePoint(24 - i, i, sprite);
    }
    painter->GetPen()->SetWidth(18);
    painter->DrawPointSprites(sprite, spritePoints.data(), 25, spriteColors.data(), 4);

    painter->GetPen()->SetColor(0, 255, 0, 64);
    painter->GetPen()->SetWidth(1);
    painter->DrawLine(781, 0, 781, 600);
    painter->DrawLine(799, 0, 799, 600);

    // Leaving this untested, since I can't find a viewer that seems to implement
    // clipPaths...webkit seems to not support userSpaceOnUse clipPath coords,
    // and forces objectBoundBox coords.
#if 0
  // Test clipping (why does Context2D not expose these directly??)
    vtkContextDevice2D* dev = painter->GetDevice();
    std::array<int, 4> clipRect = { 325, 385, 150, 100 }; // xywh
    painter->GetPen()->SetColor(0, 128, 255, 255);
    painter->GetBrush()->SetColor(0, 128, 255, 64);
    painter->DrawQuad(clipRect[0], clipRect[1],
        clipRect[0] + clipRect[2], clipRect[1],
        clipRect[0] + clipRect[2], clipRect[1] + clipRect[3],
        clipRect[0], clipRect[1] + clipRect[3]);

    dev->SetClipping(clipRect.data());
    dev->EnableClipping(true);
    painter->GetBrush()->SetColor(0, 255, 128, 128);
    painter->DrawWedge(clipRect[0] + clipRect[2] / 2.f,
        clipRect[1] + clipRect[3] / 2.f,
        75.f, 50.f, 0.f, 360.f);
    dev->EnableClipping(false);
#endif

    return true;
}

//------------------------------------------------------------------------------
void ContextSVGTest::SetSpritePoint(int x, int y, vtkImageData* sprite)
{
    unsigned char* ptr = static_cast<unsigned char*>(sprite->GetScalarPointer(x, y, 0));
    std::fill(ptr, ptr + 3, 255);
}

#endif // TEST50

#ifdef TEST52

#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkJPEGReader.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkXMLPolyDataReader.h>

#include <string>

namespace {

class vtkBorderCallback2 : public vtkCommand
{
public:
    vtkBorderCallback2()
    {
    }

    static vtkBorderCallback2* New()
    {
        return new vtkBorderCallback2;
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {

        vtkBorderWidget* borderWidget = reinterpret_cast<vtkBorderWidget*>(caller);

        // 获取到的是viewport坐标，且pos2表示的是width和height并不是实际坐标
        auto pos1 = static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->GetPosition();
        auto pos2 = static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->GetPosition2();

        // std::cout << "======== pos\t" << pos1[0] << ",\t" << pos1[1] << ",\t" << pos2[0] << ",\t" << pos2[1] << "\n";

        // Get the world coordinates of the two corners of the box
        auto lowerLeftCoordinate = static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->GetPositionCoordinate();
        auto lowerLeft           = lowerLeftCoordinate->GetComputedWorldValue(this->LeftRenderer);

        auto upperRightCoordinate = static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->GetPosition2Coordinate();
        auto upperRight           = upperRightCoordinate->GetComputedWorldValue(this->LeftRenderer);
        // std::cout << "World:\t\t" << lowerLeft[0] << ", \t"<< lowerLeft[1] << ", \t" << lowerLeft[2] << ", \t" << upperRight[0] << ", \t" <<
        // upperRight[1] << ", \t" << upperRight[2] << std::endl;

        auto ret1 = upperRightCoordinate->GetComputedDisplayValue(this->LeftRenderer);
        auto ret2 = lowerLeftCoordinate->GetComputedDisplayValue(this->LeftRenderer);
        // std::cout << "Display:\t" << ret1[0] << ", \t" << ret1[1] << ", \t" << ret1[2] << ", \t" << ret2[0] << ", \t" << ret2[1] << ", \t" <<
        // ret2[2] << std::endl;

        auto ret3 = upperRightCoordinate->GetComputedViewportValue(this->LeftRenderer);
        auto ret4 = lowerLeftCoordinate->GetComputedViewportValue(this->LeftRenderer);
        // std::cout << "Viewport:\t" << ret3[0] << ", \t" << ret3[1] << ", \t" << ret3[2] <<", \t" << ret4[0] << ", \t" << ret4[1] << ", \t" <<
        // ret4[2] << std::endl;

        auto ret5 = upperRightCoordinate->GetComputedLocalDisplayValue(this->LeftRenderer);
        auto ret6 = lowerLeftCoordinate->GetComputedLocalDisplayValue(this->LeftRenderer);
        // std::cout << "Local:\t\t" << ret5[0] << ", \t" << ret5[1] << ", \t" << ret5[2] << ", \t" << ret6[0] << ", \t" << ret6[1] << ", \t" <<
        // ret6[2] << std::endl;

        auto size = this->LeftRenderer->GetSize();
        std::cout << size[0] << "," << size[1] << '\n';

        double* bounds = this->ImageActor->GetBounds();
        double xmin    = bounds[0];
        double xmax    = bounds[1];
        double ymin    = bounds[2];
        double ymax    = bounds[3];

        if ((lowerLeft[0] > xmin) && (upperRight[0] < xmax) && (lowerLeft[1] > ymin) && (upperRight[1] < ymax))
        {
            this->ClipFilter->SetOutputWholeExtent(
                vtkMath::Round(lowerLeft[0]), vtkMath::Round(upperRight[0]), vtkMath::Round(lowerLeft[1]), vtkMath::Round(upperRight[1]), 0, 1);

            // std::cout << lowerLeft[0] << "," << upperRight[0] << "," << lowerLeft[1] << "," << upperRight[1] << std::endl;

            // this->ClipFilter->SetOutputWholeExtent(100, 100, 200, 200, 0, 1);
        }
        else
        {
            // std::cout << "box is NOT inside image" << std::endl;
        }
    }

    void SetLeftRenderer(vtkSmartPointer<vtkRenderer> renderer)
    {
        this->LeftRenderer = renderer;
    }

    void SetImageActor(vtkSmartPointer<vtkImageActor> actor)
    {
        this->ImageActor = actor;
    }

    void SetClipFilter(vtkSmartPointer<vtkImageClip> clip)
    {
        this->ClipFilter = clip;
    }

private:
    vtkSmartPointer<vtkRenderer> LeftRenderer;
    vtkSmartPointer<vtkImageActor> ImageActor;
    vtkSmartPointer<vtkImageClip> ClipFilter;
};

} // namespace

/*
vtkBorderWidget设置位置
    static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->SetPosition(x1,y1)
    static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->SetPosition2(x2,y2)
    x1,y1是窗口左下角的viewport位置（即最大为1，最小为0）
    x2,y2实际是窗口的宽和高对应的比例，例如pos1(0.5,0.5),pos2(0.2,0.2)相当于setViewPort(0.5,0.5,0.7,0.7)
    vtk左下角为(0,0)右上角为(1,1)
*/

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> color;

    std::string inputFilename = "resource/test1.jpg";

    // Read the image
    vtkNew<vtkJPEGReader> jPEGReader;

    if (!jPEGReader->CanReadFile(inputFilename.c_str()))
    {
        std::cout << "Error: cannot read " << inputFilename << std::endl;
        return EXIT_FAILURE;
    }

    jPEGReader->SetFileName(inputFilename.c_str());
    jPEGReader->Update();

    int extent[6];
    jPEGReader->GetOutput()->GetExtent(extent);
    // xmin, xmax, ymin, ymax
    std::cout << "extent: " << extent[0] << " " << extent[1] << " " << extent[2] << " " << extent[3] << " " << extent[4] << " " << extent[5]
              << std::endl;

    vtkNew<vtkImageActor> imageActor;
    imageActor->GetMapper()->SetInputConnection(jPEGReader->GetOutputPort());

    vtkNew<vtkRenderWindow> renderWindow;

    vtkNew<vtkRenderWindowInteractor> interactor;

    // vtkNew<vtkInteractorStyleImage> style;
    vtkNew<vtkInteractorStyleRubberBand2D> style;
    interactor->SetInteractorStyle(style);

    vtkNew<vtkBorderWidget> borderWidget;
    borderWidget->SetInteractor(interactor);
    // static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
    //     ->GetBorderProperty()
    //     ->SetColor(color->GetColor3d("Lime").GetData());

    auto ret = borderWidget->GetBorderRepresentation(); // 返回空，所以必须获取borderRepresentation再转换

    static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->SetBorderColor(1, 0, 0);

    static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation());
    borderWidget->SelectableOff();

    interactor->SetRenderWindow(renderWindow);

    // Define viewport ranges in normalized coordinates
    // (xmin, ymin, xmax, ymax)
    double leftViewport[4]  = { 0.0, 0.0, 0.5, 1.0 };
    double rightViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

    // Setup both renderers
    vtkNew<vtkRenderer> leftRenderer;
    renderWindow->AddRenderer(leftRenderer);
    leftRenderer->SetViewport(leftViewport);
    leftRenderer->SetBackground(color->GetColor3d("DarkSlateGray").GetData());

    vtkNew<vtkRenderer> rightRenderer;
    renderWindow->AddRenderer(rightRenderer);
    rightRenderer->SetViewport(rightViewport);
    rightRenderer->SetBackground(color->GetColor3d("DimGray").GetData());

    leftRenderer->AddActor(imageActor);

    leftRenderer->ResetCamera();
    rightRenderer->ResetCamera();

    vtkNew<vtkImageClip> imageClip;
    imageClip->SetInputConnection(jPEGReader->GetOutputPort());
    jPEGReader->UpdateInformation();
    imageClip->SetOutputWholeExtent(jPEGReader->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
    imageClip->ClipDataOn();

    vtkNew<vtkImageActor> clipActor;
    clipActor->GetMapper()->SetInputConnection(imageClip->GetOutputPort());

    rightRenderer->AddActor(clipActor);

    vtkNew<vtkBorderCallback2> borderCallback;
    borderCallback->SetLeftRenderer(leftRenderer);
    borderCallback->SetImageActor(imageActor);
    borderCallback->SetClipFilter(imageClip);

    borderWidget->AddObserver(vtkCommand::InteractionEvent, borderCallback);

    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("ImageClip");
    renderWindow->Render();
    borderWidget->On();
    interactor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST52

#ifdef TEST53

#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkXMLPolyDataReader.h>

#include <string>

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> color;
    std::string inputFilename = "file\\57_test1.jpg";

    // Read the image
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName(inputFilename.c_str());
    jPEGReader->Update();
    // 图片大小
    int extent[6];
    jPEGReader->GetOutput()->GetExtent(extent);

    vtkNew<vtkRenderWindow> renderWindow;
    vtkNew<vtkRenderWindowInteractor> interactor;

    // 图片交互器，滑动鼠标会让图片对比度变化，一会亮一会暗
    // vtkNew<vtkInteractorStyleImage> style;
    // interactor->SetInteractorStyle(style);

    interactor->SetRenderWindow(renderWindow);

    // 裁剪图片
    vtkNew<vtkImageClip> imageClip;
    imageClip->SetInputConnection(jPEGReader->GetOutputPort());
    // jPEGReader->UpdateInformation();
    // imageClip->SetOutputWholeExtent(jPEGReader->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
    imageClip->SetOutputWholeExtent(0, 100, 0, 100, 0, 0);
    imageClip->ClipDataOn();
    imageClip->Update();

    // 添加图片actor
    vtkNew<vtkImageActor> imageActor;
    imageActor->GetMapper()->SetInputData(imageClip->GetOutput()); // 如果不调用vtkImageClip的Updata函数，此处不能用数据集直接设置
    // imageActor->GetMapper()->SetInputConnection(imageClip->GetOutputPort());
    vtkNew<vtkRenderer> renderer;
    renderWindow->AddRenderer(renderer);
    renderer->AddActor(imageActor);
    // renderer->ResetCamera();

    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("ImageClip");
    renderWindow->Render();
    renderWindow->Frame();
    renderWindow->CopyResultFrame();
    interactor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST53

#ifdef TEST54
#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageViewer2.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vtkImageCanvasSource2D.h>

#include <string>

std::string file = "test.jpg";

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName(file.c_str());
    jPEGReader->Update();
    // 图片大小
    int extent[6];
    jPEGReader->GetOutput()->GetExtent(extent);

    // 创建一个纯色图片，用来给原图做阴影效果
    // vtkNew<vtkImageData> image;
    // image->SetDimensions(extent[1] + 1, extent[3] + 1, extent[5] + 1);

    // Create an image
    // vtkNew<vtkImageCanvasSource2D> drawing;
    // drawing->SetScalarTypeToUnsignedChar();
    // drawing->SetNumberOfScalarComponents(4);  //设置颜色分量（r,g,b,a)共4个，设置为3则只有(r,g,b)
    ////drawing->SetExtent(0, 200, 0, 200, 0, 0); //大小
    // drawing->SetExtent(0, extent[1]+1, 0, extent[3]+1, 0, 0); //大小

    // drawing->SetDrawColor(255, 0, 0, 255);  // 注意此处的颜色值为[0,255]，第4个参数是alpha，可以理解为透明度
    // drawing->FillBox(0, 200, 0, 200);   //画一个矩形（实心）
    // drawing->SetDrawColor(255, 0, 0, 0);
    // drawing->FillBox(50, 100, 50, 100);
    ////drawing->DrawCircle(100, 100, 50); //画一个圆圈（非实心）
    // drawing->Update();

    vtkNew<vtkImageBlend> blend;
    blend->AddInputConnection(jPEGReader->GetOutputPort());
    blend->AddInputConnection(jPEGReader->GetOutputPort());
    // blend->AddInputData(jPEGReader->GetOutput());
    // blend->AddInputData(drawing->GetOutput());
    //  设置不透明度
    blend->SetOpacity(0, 0.5);
    blend->SetOpacity(1, 0.5);

    // Display the result
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

    vtkNew<vtkImageViewer2> imageViewer;
    imageViewer->SetInputConnection(blend->GetOutputPort());
    imageViewer->SetupInteractor(renderWindowInteractor);
    imageViewer->GetRenderer()->ResetCamera();
    imageViewer->GetRenderer()->SetBackground(colors->GetColor3d("Peru").GetData());
    imageViewer->GetRenderWindow()->SetWindowName("CombineImages");

    imageViewer->GetRenderer()->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST54

#ifdef TEST55

#include <vtkActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkLogoRepresentation.h>
#include <vtkLogoWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <array>

int main(int, char*[])
{

    vtkNew<vtkNamedColors> colors;
    // colors->SetColor("Bkg", 0.2, 0.3, 0.4);

    // A sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(4.0);
    sphereSource->SetPhiResolution(4);
    sphereSource->SetThetaResolution(8);
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
    renderWindow->SetWindowName("LogoWidget");

    renderer->AddActor(actor);

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Create an image
    vtkNew<vtkImageCanvasSource2D> drawing;
    drawing->SetScalarTypeToUnsignedChar();
    drawing->SetNumberOfScalarComponents(4);  // 设置颜色分量（r,g,b,a)共4个，设置为3则只有(r,g,b)
    drawing->SetExtent(0, 200, 0, 200, 0, 0); // 大小

    // Clear the image
    // Note: SetDrawColour() uses double values of the rgb colors in the
    //       range [0 ... 255]
    //       So SetDrawColour(255, 255, 255) is white.
    drawing->SetDrawColor(255, 0, 0, 255);
    drawing->FillBox(0, 200, 0, 200);
    drawing->SetDrawColor(255, 0, 0, 0);
    drawing->FillBox(50, 100, 50, 100);
    // drawing->DrawCircle(100, 100, 50);
    drawing->Update();

    vtkNew<vtkLogoRepresentation> logoRepresentation;
    logoRepresentation->SetImage(drawing->GetOutput());
    logoRepresentation->SetPosition(0, 0);
    logoRepresentation->SetPosition2(0.4, 0.4);
    logoRepresentation->GetImageProperty()->SetOpacity(.7);

    vtkNew<vtkLogoWidget> logoWidget;
    logoWidget->SetInteractor(renderWindowInteractor);
    logoWidget->SetRepresentation(logoRepresentation);

    renderWindow->Render();
    logoWidget->On();
    renderer->SetBackground(colors->GetColor3d("MidnightBLue").GetData());
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST55

#ifdef TEST56

#include <vtkImageBlend.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
// #include <vtkImageReader2.h>
// #include <vtkImageReader2Factory.h>
// #include <vtkImageViewer2.h>
#include <vtkImageActor.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <array>

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkRenderer> render;
    vtkNew<vtkImageActor> actor;
    vtkNew<vtkRenderWindow> window;

    std::string file = "test.jpg";

    vtkNew<vtkJPEGReader> reader;
    reader->SetFileName(file.c_str());
    reader->Update();

    vtkImageData* image = reader->GetOutput();
    auto extent         = image->GetExtent();

    // Draw a circle in the center of the image
    // vtkNew<vtkImageCanvasSource2D> drawing;

    vtkSmartPointer drawing = vtkSmartPointer<vtkImageCanvasSource2D>::New();
    drawing->SetScalarTypeToUnsignedChar();
    drawing->SetNumberOfScalarComponents(4); // 设置颜色为4维，即rgba
    drawing->SetExtent(image->GetExtent());
    drawing->Update();

    auto type1 = reader->GetOutput()->GetScalarType();
    auto type2 = drawing->GetOutput()->GetScalarType();
    auto type3 = drawing->GetOutput()->GetScalarPointer();

    drawing->SetDrawColor(0, 0, 0, 100); // 黑色，alpha为100
    drawing->FillBox(0, extent[1], 0, extent[3]);
    // FillBox的参数一定要注意：0:lowerX,1:upperX,2:lowerY,3:upperY
    drawing->SetDrawColor(255, 255, 255, 0); // 白色，alpha为0，即完全透明
    drawing->FillBox(50, 500, 50, 500);
    // drawing->SetDrawColor(drawColor2.data());
    // drawing->DrawCircle(center[0], center[1], radius);

    vtkNew<vtkImageBlend> blend;
    // blend->AddInputConnection(imgReader->GetOutputPort());
    // blend->AddInputConnection(drawing->GetOutputPort());
    blend->AddInputData(reader->GetOutput());
    blend->AddInputData(drawing->GetOutput());
    blend->SetOpacity(0, 1.); // 第0张输入图片，不透明度为1
    blend->SetOpacity(1, 1.);
    blend->Update();

    // 重新绘制可以覆盖掉之前的
    drawing->SetDrawColor(0, 0, 0, 100); // 黑色，alpha为100
    drawing->FillBox(0, extent[1], 0, extent[3]);
    drawing->SetDrawColor(255, 0, 0, 255);
    drawing->FillBox(500, 800, 500, 800);
    drawing->Update(); // 如果要覆盖之前的，必须在此处调用update

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    // actor->GetMapper()->SetInputData(blend->GetOutput());  //使用独立数据集方式绘制不出来，得使用管道方式
    actor->GetMapper()->SetInputConnection(blend->GetOutputPort());
    render->AddActor(actor);
    window->AddRenderer(render);
    window->SetInteractor(renderWindowInteractor);
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST56

#ifdef TEST57

#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <filesystem>

class Callback : public vtkCommand
{
public:
    static Callback* New()
    {
        return new Callback;
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {

        auto window = static_cast<vtkWindow*>(caller);
        auto w      = window->GetSize()[0];
        auto h      = window->GetSize()[1];
        double w_h  = static_cast<double>(w) / static_cast<double>(h);
        double x_y  = m_xDistance / m_yDistance;

        std::cout << "resize window:\t" << m_camera->GetParallelScale() << '\t' << w << ',' << h << '\t' << m_xDistance << ',' << m_yDistance
                  << "       \t" << w_h << "      \t" << x_y << '\n';

        if (w_h > x_y)
        {
            m_camera->SetParallelScale(0.5 * m_yDistance);
        }
        else
        {
            m_camera->SetParallelScale(0.5 * m_xDistance / w_h);
        }

        // if (w > h)
        //{
        //     m_camera->SetParallelScale(0.5 * m_yDistance);
        // }
        // else
        //{
        //     m_camera->SetParallelScale(0.5 * m_yDistance / w_h);
        // }
    }

    void SetData(vtkSmartPointer<vtkCamera> camera, double x, double y)
    {
        m_camera    = camera;
        m_xDistance = x;
        m_yDistance = y;
    }

private:
    vtkSmartPointer<vtkCamera> m_camera;
    double m_xDistance { 0.0 };
    double m_yDistance { 0.0 };
};

int main(int, char*[])
{
    auto path        = std::filesystem::current_path();
    std::string file = path.string() + "\\file\\57_test1.jpg";
    // std::string file = "57_test2.jpg";

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

    // float d = camera->GetDistance();
    // camera->SetParallelScale(0.5f * static_cast<float>(yd));
    // camera->SetParallelScale(0.5f * static_cast<float>(xd));
    // camera->SetFocalPoint(xc, yc, 0.0);
    // camera->SetPosition(xc, yc, +d);

    // if (renderWindow->GetSize()[0] < renderWindow->GetSize()[1])
    //{
    //     camera->Zoom(double(renderWindow->GetSize()[0]) / renderWindow->GetSize()[1]);
    // }
    renderWindow->SetSize(400, 800);
    auto w = renderWindow->GetSize()[0];
    auto h = renderWindow->GetSize()[1];
    // w>h x>y
    // w>h x<y
    // camera->SetParallelScale(0.5 * yd);
    // w<h x<y
    // w<h x>y
    camera->SetParallelScale(0.5 * xd / ((double)w / h));

    auto cb = Callback::New();
    cb->SetData(camera, xd, yd);
    renderWindow->AddObserver(vtkCommand::WindowResizeEvent, cb);

    // 1和2效果一样
    // *** 1
    // camera->SetParallelScale(0.5 * yd);
    // camera->Zoom(w/h);
    // *** 2
    // camera->SetParallelScale(0.5 * yd/((double)w/h));

    /*zoom的源码：*/
    // this->SetParallelScale(this->ParallelScale / amount);

    // Setup render window interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkInteractorStyleImage> style;
    renderWindowInteractor->SetInteractorStyle(style);
    // renderWindowInteractor->SetInteractorStyle(nullptr);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST57

#ifdef TEST63

#define vtk_official

#ifdef vtk_official
// https://kitware.github.io/vtk-examples/site/Cxx/Qt/ImageDataToQImage/
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>

#include <QApplication>
#include <QColor>
#include <QImage>

// Create a green 50x50 imageData for demonstration purposes
vtkSmartPointer<vtkImageData> createDemoImageData()
{
    vtkNew<vtkImageData> image;
    image->SetDimensions(50, 50, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    int width  = image->GetDimensions()[0];
    int height = image->GetDimensions()[1];

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
            pixel[0]             = 0;
            pixel[1]             = 255;
            pixel[2]             = 0;
        }
    }

    return image;
}

// The actual conversion code
QImage vtkImageDataToQImage(vtkSmartPointer<vtkImageData> imageData)
{
    if (!imageData)
    {
        return QImage();
    }

    /// \todo retrieve just the UpdateExtent
    int width  = imageData->GetDimensions()[0];
    int height = imageData->GetDimensions()[1];
    QImage image(width, height, QImage::Format_RGB32);
    QRgb* rgbPtr             = reinterpret_cast<QRgb*>(image.bits()) + width * (height - 1);
    unsigned char* colorsPtr = reinterpret_cast<unsigned char*>(imageData->GetScalarPointer());

    // Loop over the vtkImageData contents.
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // Swap the vtkImageData RGB values with an equivalent QColor
            *(rgbPtr++) = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2]).rgb();
            colorsPtr += imageData->GetNumberOfScalarComponents();
        }

        rgbPtr -= width * 2;
    }

    return image;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QImage qimage = vtkImageDataToQImage(createDemoImageData());
    qimage.save("qimage.png");

    return EXIT_SUCCESS;
}

#else

// https://tieba.baidu.com/p/4297027534?red_tag=3118028977
#include <QImage>
#include <vtkDataArray.h>
#include <vtkImageData.h>
#include <vtkJPEGReader.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>

#include <string>
QImage createQImage1(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage2(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage3(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage4(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage(vtkImageData* imageData);

int main()
{
    std::string inputFilename  = "test.jpg";
    std::string outputFilename = "save.jpg";

    vtkNew<vtkJPEGReader> reader;
    reader->SetFileName(inputFilename.c_str());
    reader->Update(); // 必须Update，不然GetOutput返回值为空

    auto image = createQImage(reader->GetOutput());
    image.save(outputFilename.c_str());

    return 0;
}

QImage createQImage1(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex   = 0;

    QRgb* qImageBits            = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++);

            QRgb color                         = qRgba(tuple[0], tuple[0], tuple[0], 255);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }

    return qImage;
}

QImage createQImage2(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex   = 0;

    QRgb* qImageBits            = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++ * 2);

            QRgb color                         = qRgba(tuple[0], tuple[0], tuple[0], tuple[1]);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }

    return qImage;
}

QImage createQImage3(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex   = 0;

    QRgb* qImageBits            = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++ * 3);

            QRgb color                         = qRgba(tuple[0], tuple[1], tuple[2], 255);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }

    // Qt图片和VTK图片的原点不同，qt是左上角，vtk是左下角
    return qImage.mirrored(false, true);
}

QImage createQImage4(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex   = 0;

    QRgb* qImageBits            = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++ * 4);

            QRgb color                         = qRgba(tuple[0], tuple[1], tuple[2], tuple[3]);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }

    return qImage;
}

QImage createQImage(vtkImageData* imageData)
{
    if (!imageData)
        return QImage();

    int width                     = imageData->GetDimensions()[0];
    int height                    = imageData->GetDimensions()[1];
    vtkUnsignedCharArray* scalars = vtkUnsignedCharArray::SafeDownCast(imageData->GetPointData()->GetScalars());

    if (!width || !height || !scalars)
        return QImage();

    switch (scalars->GetNumberOfComponents())
    {
    case 1:
        return createQImage1(width, height, scalars);
    case 2:
        return createQImage2(width, height, scalars);
    case 3:
        return createQImage3(width, height, scalars);
    case 4:
        return createQImage4(width, height, scalars);
    }

    return QImage();
}
#endif // vtk_official

#endif // TEST63