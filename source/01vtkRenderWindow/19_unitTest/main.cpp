/**
 * 1. Actor2D
 *
 */

#define TEST1

#ifdef TEST1

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCoordinate.h>
#include <vtkLineSource.h>
#include <vtkLookupTable.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <vtkRegressionTestImage.h>
#include <vtkTestUtilities.h>

#include <iostream>

int main(int argc, char* argv[])
{
    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(6);
    lut->SetTableRange(0.0, 1.0);

    vtkNew<vtkPlaneSource> planeSource1;
    planeSource1->SetOrigin(0.0, 0.0, 0.0);
    planeSource1->SetPoint1(0.5, 0.0, 0.0);
    planeSource1->SetPoint2(0.0, 0.5, 0.0);

    vtkNew<vtkPolyDataMapper> mapper1;
    mapper1->SetInputConnection(planeSource1->GetOutputPort());
    mapper1->ScalarVisibilityOn();
    mapper1->SetLookupTable(lut);
    mapper1->UseLookupTableScalarRangeOn();
    mapper1->SetScalarModeToUsePointFieldData();
    mapper1->ColorByArrayComponent("TextureCoordinates", 0);
    mapper1->InterpolateScalarsBeforeMappingOn();

    vtkNew<vtkActor> actor1;
    actor1->SetMapper(mapper1);
    actor1->GetProperty()->SetColor(1.0, 0.0, 0.0);

    vtkNew<vtkPlaneSource> planeSource2;
    planeSource2->SetOrigin(-0.5, 0.0, 0.0);
    planeSource2->SetPoint1(0.0, 0.0, 0.0);
    planeSource2->SetPoint2(-0.5, 0.5, 0.0);

    vtkNew<vtkCoordinate> pCoord;
    pCoord->SetCoordinateSystemToWorld();

    vtkNew<vtkCoordinate> coord;
    coord->SetCoordinateSystemToNormalizedViewport();
    coord->SetReferenceCoordinate(pCoord);

    vtkNew<vtkPolyDataMapper2D> mapper2;
    mapper2->SetInputConnection(planeSource2->GetOutputPort());
    mapper2->SetLookupTable(lut);
    mapper2->ScalarVisibilityOff();
    mapper2->SetTransformCoordinate(coord);

    vtkNew<vtkActor2D> actor2;
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(1.0, 1.0, 0.0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor1);
    renderer->AddActor(actor2);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    renWin->Render();
    renderer->ResetCamera();
    renderer->SetBackground(1.0, 0.0, 0.0);
    renWin->SetSize(300, 300);

    int c     = 4;
    // 需要对比的图片路径、文件名
    char* v[] = { "-D", "../resources", "-V", "TestActor2D.png" };

    // 使用渲染窗口的结果(vtkWindowToImageFilter)和提供的图片像素进行对比(vtkImageDifference)
    // 如果误差小于提供的目标，则返回 vtkRegressionTester::PASSED
    int retVal = vtkTesting::Test(c, v, renWin, 0.15);

    std::cout << "Regression test result: " << retVal << std::endl;

    if (retVal == vtkRegressionTester::DO_INTERACTOR)
    {
        iren->Start();
    }

    return !retVal;
}

#endif // TEST1
