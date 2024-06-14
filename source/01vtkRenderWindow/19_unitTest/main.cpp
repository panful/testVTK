/**
 * 1. Actor2D
 * 2. SeedWidget
 * 3. BoxWidget
 *
 */

#define TEST4

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
    char* v[] = { "-D", "../resources", "-V", "TestActor2D.png" }; // 需要对比的图片路径、文件名

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

#ifdef TEST2

#include "vtkActor.h"
#include "vtkAxisActor2D.h"
#include "vtkCommand.h"
#include "vtkCoordinate.h"
#include "vtkDebugLeaks.h"
#include "vtkHandleWidget.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkMath.h"
#include "vtkPointHandleRepresentation2D.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSeedRepresentation.h"
#include "vtkSeedWidget.h"
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkTesting.h"

const char TestSeedWidgetEventLog[] = "# StreamVersion 1 i\n"
                                      "CharEvent 185 179 0 0 105 1 i\n"
                                      "KeyReleaseEvent 185 179 0 0 105 1 i\n"
                                      "MouseMoveEvent 138 180 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 137 180 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 136 180 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 135 180 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 134 180 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 133 180 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 132 180 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 131 180 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 130 180 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 129 181 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 128 181 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 127 181 0 0 0 0 0 i\n"
                                      "LeftButtonPressEvent 127 181 0 0 0 0 0 i\n"
                                      "RenderEvent 127 181 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 124 181 0 0 0 0 0 i\n"
                                      "RenderEvent 124 181 0 0 0 0 0 i\n"
                                      "LeftButtonReleaseEvent 126 181 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 124 181 0 0 0 0 0 i\n"
                                      "RenderEvent 124 181 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 96 144 0 0 0 0 0 i\n"
                                      "RenderEvent 96 144 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 96 143 0 0 0 0 0 i\n"
                                      "RenderEvent 96 143 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 96 142 0 0 0 0 0 i\n"
                                      "RenderEvent 96 142 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 96 141 0 0 0 0 0 i\n"
                                      "RenderEvent 96 141 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 96 140 0 0 0 0 0 i\n"
                                      "RenderEvent 96 140 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 96 139 0 0 0 0 0 i\n"
                                      "RenderEvent 96 139 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 96 138 0 0 0 0 0 i\n"
                                      "RenderEvent 96 138 0 0 0 0 0 i\n"
                                      "LeftButtonPressEvent 96 138 0 0 0 0 0 i\n"
                                      "RenderEvent 96 138 0 0 0 0 0 i\n"
                                      "LeftButtonReleaseEvent 96 138 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 96 137 0 0 0 0 0 i\n"
                                      "RenderEvent 96 137 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 97 137 0 0 0 0 0 i\n"
                                      "RenderEvent 97 137 0 0 0 0 0 i\n"
                                      "MouseMoveEvent 164 113 0 0 0 0 t i\n"
                                      "RenderEvent 164 113 0 0 0 0 t i\n"
                                      "MouseMoveEvent 163 113 0 0 0 0 t i\n"
                                      "RenderEvent 163 113 0 0 0 0 t i\n"
                                      "MouseMoveEvent 162 113 0 0 0 0 t i\n"
                                      "RenderEvent 162 113 0 0 0 0 t i\n"
                                      "MouseMoveEvent 161 113 0 0 0 0 t i\n"
                                      "RenderEvent 161 113 0 0 0 0 t i\n"
                                      "MouseMoveEvent 161 114 0 0 0 0 t i\n"
                                      "RenderEvent 161 114 0 0 0 0 t i\n"
                                      "LeftButtonPressEvent 161 114 0 0 0 0 t i\n"
                                      "RenderEvent 161 114 0 0 0 0 t i\n"
                                      "LeftButtonReleaseEvent 161 114 0 0 0 0 t i\n"
                                      "MouseMoveEvent 161 115 0 0 0 0 t i\n"
                                      "RenderEvent 161 115 0 0 0 0 t i\n"
                                      "MouseMoveEvent 161 116 0 0 0 0 t i\n"
                                      "RenderEvent 161 116 0 0 0 0 t i\n"
                                      "MouseMoveEvent 161 117 0 0 0 0 t i\n"
                                      "RenderEvent 161 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 185 158 0 0 0 0 t i\n"
                                      "RenderEvent 185 158 0 0 0 0 t i\n"
                                      "MouseMoveEvent 185 159 0 0 0 0 t i\n"
                                      "RenderEvent 185 159 0 0 0 0 t i\n"
                                      "MouseMoveEvent 186 159 0 0 0 0 t i\n"
                                      "RenderEvent 186 159 0 0 0 0 t i\n"
                                      "LeftButtonPressEvent 186 159 0 0 0 0 t i\n"
                                      "RenderEvent 186 159 0 0 0 0 t i\n"
                                      "LeftButtonReleaseEvent 186 159 0 0 0 0 t i\n"
                                      "MouseMoveEvent 185 159 0 0 0 0 t i\n"
                                      "RenderEvent 185 159 0 0 0 0 t i\n"
                                      "MouseMoveEvent 184 159 0 0 0 0 t i\n"
                                      "RenderEvent 184 159 0 0 0 0 t i\n"
                                      "MouseMoveEvent 183 159 0 0 0 0 t i\n"
                                      "RenderEvent 183 159 0 0 0 0 t i\n"
                                      "MouseMoveEvent 182 159 0 0 0 0 t i\n"
                                      "RenderEvent 182 159 0 0 0 0 t i\n"
                                      "MouseMoveEvent 181 160 0 0 0 0 t i\n"
                                      "RenderEvent 181 160 0 0 0 0 t i\n"
                                      "MouseMoveEvent 131 176 0 0 0 0 t i\n"
                                      "RenderEvent 131 176 0 0 0 0 t i\n"
                                      "MouseMoveEvent 130 176 0 0 0 0 t i\n"
                                      "RenderEvent 130 176 0 0 0 0 t i\n"
                                      "MouseMoveEvent 130 177 0 0 0 0 t i\n"
                                      "RenderEvent 130 177 0 0 0 0 t i\n"
                                      "MouseMoveEvent 129 177 0 0 0 0 t i\n"
                                      "RenderEvent 129 177 0 0 0 0 t i\n"
                                      "MouseMoveEvent 128 177 0 0 0 0 t i\n"
                                      "RenderEvent 128 177 0 0 0 0 t i\n"
                                      "MouseMoveEvent 128 178 0 0 0 0 t i\n"
                                      "RenderEvent 128 178 0 0 0 0 t i\n"
                                      "MouseMoveEvent 127 179 0 0 0 0 t i\n"
                                      "RenderEvent 127 179 0 0 0 0 t i\n"
                                      "MouseMoveEvent 127 180 0 0 0 0 t i\n"
                                      "RenderEvent 127 180 0 0 0 0 t i\n"
                                      "LeftButtonPressEvent 127 180 0 0 0 0 t i\n"
                                      "RenderEvent 127 180 0 0 0 0 t i\n"
                                      "MouseMoveEvent 127 179 0 0 0 0 t i\n"
                                      "RenderEvent 127 179 0 0 0 0 t i\n"
                                      "MouseMoveEvent 128 178 0 0 0 0 t i\n"
                                      "RenderEvent 128 178 0 0 0 0 t i\n"
                                      "MouseMoveEvent 129 177 0 0 0 0 t i\n"
                                      "RenderEvent 129 177 0 0 0 0 t i\n"
                                      "MouseMoveEvent 129 176 0 0 0 0 t i\n"
                                      "RenderEvent 129 176 0 0 0 0 t i\n"
                                      "MouseMoveEvent 130 175 0 0 0 0 t i\n"
                                      "RenderEvent 130 175 0 0 0 0 t i\n"
                                      "MouseMoveEvent 131 173 0 0 0 0 t i\n"
                                      "RenderEvent 131 173 0 0 0 0 t i\n"
                                      "MouseMoveEvent 132 172 0 0 0 0 t i\n"
                                      "RenderEvent 132 172 0 0 0 0 t i\n"
                                      "MouseMoveEvent 133 171 0 0 0 0 t i\n"
                                      "RenderEvent 133 171 0 0 0 0 t i\n"
                                      "MouseMoveEvent 137 167 0 0 0 0 t i\n"
                                      "RenderEvent 137 167 0 0 0 0 t i\n"
                                      "MouseMoveEvent 138 166 0 0 0 0 t i\n"
                                      "RenderEvent 138 166 0 0 0 0 t i\n"
                                      "MouseMoveEvent 138 164 0 0 0 0 t i\n"
                                      "RenderEvent 138 164 0 0 0 0 t i\n"
                                      "MouseMoveEvent 140 163 0 0 0 0 t i\n"
                                      "RenderEvent 140 163 0 0 0 0 t i\n"
                                      "MouseMoveEvent 140 162 0 0 0 0 t i\n"
                                      "RenderEvent 140 162 0 0 0 0 t i\n"
                                      "MouseMoveEvent 141 161 0 0 0 0 t i\n"
                                      "RenderEvent 141 161 0 0 0 0 t i\n"
                                      "MouseMoveEvent 142 160 0 0 0 0 t i\n"
                                      "RenderEvent 142 160 0 0 0 0 t i\n"
                                      "MouseMoveEvent 143 159 0 0 0 0 t i\n"
                                      "RenderEvent 143 159 0 0 0 0 t i\n"
                                      "MouseMoveEvent 144 158 0 0 0 0 t i\n"
                                      "RenderEvent 144 158 0 0 0 0 t i\n"
                                      "MouseMoveEvent 144 157 0 0 0 0 t i\n"
                                      "RenderEvent 144 157 0 0 0 0 t i\n"
                                      "MouseMoveEvent 145 156 0 0 0 0 t i\n"
                                      "RenderEvent 145 156 0 0 0 0 t i\n"
                                      "MouseMoveEvent 146 155 0 0 0 0 t i\n"
                                      "RenderEvent 146 155 0 0 0 0 t i\n"
                                      "MouseMoveEvent 147 154 0 0 0 0 t i\n"
                                      "RenderEvent 147 154 0 0 0 0 t i\n"
                                      "MouseMoveEvent 148 153 0 0 0 0 t i\n"
                                      "RenderEvent 148 153 0 0 0 0 t i\n"
                                      "MouseMoveEvent 148 152 0 0 0 0 t i\n"
                                      "RenderEvent 148 152 0 0 0 0 t i\n"
                                      "MouseMoveEvent 149 151 0 0 0 0 t i\n"
                                      "RenderEvent 149 151 0 0 0 0 t i\n"
                                      "MouseMoveEvent 150 150 0 0 0 0 t i\n"
                                      "RenderEvent 150 150 0 0 0 0 t i\n"
                                      "MouseMoveEvent 151 149 0 0 0 0 t i\n"
                                      "RenderEvent 151 149 0 0 0 0 t i\n"
                                      "MouseMoveEvent 152 147 0 0 0 0 t i\n"
                                      "RenderEvent 152 147 0 0 0 0 t i\n"
                                      "MouseMoveEvent 153 146 0 0 0 0 t i\n"
                                      "RenderEvent 153 146 0 0 0 0 t i\n"
                                      "MouseMoveEvent 154 144 0 0 0 0 t i\n"
                                      "RenderEvent 154 144 0 0 0 0 t i\n"
                                      "MouseMoveEvent 156 143 0 0 0 0 t i\n"
                                      "RenderEvent 156 143 0 0 0 0 t i\n"
                                      "MouseMoveEvent 157 142 0 0 0 0 t i\n"
                                      "RenderEvent 157 142 0 0 0 0 t i\n"
                                      "MouseMoveEvent 158 141 0 0 0 0 t i\n"
                                      "RenderEvent 158 141 0 0 0 0 t i\n"
                                      "MouseMoveEvent 159 140 0 0 0 0 t i\n"
                                      "RenderEvent 159 140 0 0 0 0 t i\n"
                                      "MouseMoveEvent 160 139 0 0 0 0 t i\n"
                                      "RenderEvent 160 139 0 0 0 0 t i\n"
                                      "MouseMoveEvent 161 138 0 0 0 0 t i\n"
                                      "RenderEvent 161 138 0 0 0 0 t i\n"
                                      "MouseMoveEvent 162 138 0 0 0 0 t i\n"
                                      "RenderEvent 162 138 0 0 0 0 t i\n"
                                      "MouseMoveEvent 163 137 0 0 0 0 t i\n"
                                      "RenderEvent 163 137 0 0 0 0 t i\n"
                                      "MouseMoveEvent 164 136 0 0 0 0 t i\n"
                                      "RenderEvent 164 136 0 0 0 0 t i\n"
                                      "MouseMoveEvent 165 135 0 0 0 0 t i\n"
                                      "RenderEvent 165 135 0 0 0 0 t i\n"
                                      "MouseMoveEvent 171 133 0 0 0 0 t i\n"
                                      "RenderEvent 171 133 0 0 0 0 t i\n"
                                      "MouseMoveEvent 172 131 0 0 0 0 t i\n"
                                      "RenderEvent 172 131 0 0 0 0 t i\n"
                                      "MouseMoveEvent 174 130 0 0 0 0 t i\n"
                                      "RenderEvent 174 130 0 0 0 0 t i\n"
                                      "MouseMoveEvent 176 129 0 0 0 0 t i\n"
                                      "RenderEvent 176 129 0 0 0 0 t i\n"
                                      "MouseMoveEvent 180 125 0 0 0 0 t i\n"
                                      "RenderEvent 180 125 0 0 0 0 t i\n"
                                      "MouseMoveEvent 181 124 0 0 0 0 t i\n"
                                      "RenderEvent 181 124 0 0 0 0 t i\n"
                                      "MouseMoveEvent 183 123 0 0 0 0 t i\n"
                                      "RenderEvent 183 123 0 0 0 0 t i\n"
                                      "MouseMoveEvent 184 122 0 0 0 0 t i\n"
                                      "RenderEvent 184 122 0 0 0 0 t i\n"
                                      "MouseMoveEvent 186 121 0 0 0 0 t i\n"
                                      "RenderEvent 186 121 0 0 0 0 t i\n"
                                      "MouseMoveEvent 187 121 0 0 0 0 t i\n"
                                      "RenderEvent 187 121 0 0 0 0 t i\n"
                                      "MouseMoveEvent 188 120 0 0 0 0 t i\n"
                                      "RenderEvent 188 120 0 0 0 0 t i\n"
                                      "MouseMoveEvent 189 120 0 0 0 0 t i\n"
                                      "RenderEvent 189 120 0 0 0 0 t i\n"
                                      "MouseMoveEvent 189 119 0 0 0 0 t i\n"
                                      "RenderEvent 189 119 0 0 0 0 t i\n"
                                      "MouseMoveEvent 190 119 0 0 0 0 t i\n"
                                      "RenderEvent 190 119 0 0 0 0 t i\n"
                                      "MouseMoveEvent 191 119 0 0 0 0 t i\n"
                                      "RenderEvent 191 119 0 0 0 0 t i\n"
                                      "MouseMoveEvent 191 118 0 0 0 0 t i\n"
                                      "RenderEvent 191 118 0 0 0 0 t i\n"
                                      "MouseMoveEvent 192 118 0 0 0 0 t i\n"
                                      "RenderEvent 192 118 0 0 0 0 t i\n"
                                      "MouseMoveEvent 193 118 0 0 0 0 t i\n"
                                      "RenderEvent 193 118 0 0 0 0 t i\n"
                                      "MouseMoveEvent 194 118 0 0 0 0 t i\n"
                                      "RenderEvent 194 118 0 0 0 0 t i\n"
                                      "MouseMoveEvent 194 117 0 0 0 0 t i\n"
                                      "RenderEvent 194 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 195 117 0 0 0 0 t i\n"
                                      "RenderEvent 195 117 0 0 0 0 t i\n"
                                      "LeftButtonReleaseEvent 195 117 0 0 0 0 t i\n"
                                      "RenderEvent 195 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 194 117 0 0 0 0 t i\n"
                                      "RenderEvent 194 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 193 117 0 0 0 0 t i\n"
                                      "RenderEvent 193 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 192 117 0 0 0 0 t i\n"
                                      "RenderEvent 192 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 191 117 0 0 0 0 t i\n"
                                      "RenderEvent 191 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 190 117 0 0 0 0 t i\n"
                                      "RenderEvent 190 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 189 117 0 0 0 0 t i\n"
                                      "RenderEvent 189 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 188 117 0 0 0 0 t i\n"
                                      "RenderEvent 188 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 187 117 0 0 0 0 t i\n"
                                      "RenderEvent 187 117 0 0 0 0 t i\n"
                                      "MouseMoveEvent 186 116 0 0 0 0 t i\n"
                                      "RenderEvent 186 116 0 0 0 0 t i\n"
                                      "MouseMoveEvent 185 116 0 0 0 0 t i\n"
                                      "RenderEvent 185 116 0 0 0 0 t i\n"
                                      "MouseMoveEvent 184 116 0 0 0 0 t i\n"
                                      "RenderEvent 184 116 0 0 0 0 t i\n"
                                      "MouseMoveEvent 184 115 0 0 0 0 t i\n"
                                      "RenderEvent 184 115 0 0 0 0 t i\n"
                                      "MouseMoveEvent 183 115 0 0 0 0 t i\n"
                                      "RenderEvent 183 115 0 0 0 0 t i\n"
                                      "MouseMoveEvent 182 115 0 0 0 0 t i\n"
                                      "RenderEvent 182 115 0 0 0 0 t i\n"
                                      "MouseMoveEvent 181 114 0 0 0 0 t i\n"
                                      "RenderEvent 181 114 0 0 0 0 t i\n"
                                      "MouseMoveEvent 180 114 0 0 0 0 t i\n"
                                      "RenderEvent 180 114 0 0 0 0 t i\n"
                                      "MouseMoveEvent 179 114 0 0 0 0 t i\n"
                                      "RenderEvent 179 114 0 0 0 0 t i\n"
                                      "MouseMoveEvent 178 114 0 0 0 0 t i\n"
                                      "RenderEvent 178 114 0 0 0 0 t i\n"
                                      "MouseMoveEvent 177 113 0 0 0 0 t i\n"
                                      "RenderEvent 177 113 0 0 0 0 t i\n"
                                      "MouseMoveEvent 176 113 0 0 0 0 t i\n"
                                      "RenderEvent 176 113 0 0 0 0 t i\n"
                                      "MouseMoveEvent 174 112 0 0 0 0 t i\n"
                                      "RenderEvent 174 112 0 0 0 0 t i\n"
                                      "MouseMoveEvent 173 112 0 0 0 0 t i\n"
                                      "RenderEvent 173 112 0 0 0 0 t i\n"
                                      "MouseMoveEvent 171 112 0 0 0 0 t i\n"
                                      "RenderEvent 171 112 0 0 0 0 t i\n"
                                      "MouseMoveEvent 170 112 0 0 0 0 t i\n"
                                      "RenderEvent 170 112 0 0 0 0 t i\n"
                                      "MouseMoveEvent 169 112 0 0 0 0 t i\n"
                                      "RenderEvent 169 112 0 0 0 0 t i\n"
                                      "MouseMoveEvent 167 112 0 0 0 0 t i\n"
                                      "RenderEvent 167 112 0 0 0 0 t i\n"
                                      "MouseMoveEvent 166 111 0 0 0 0 t i\n"
                                      "RenderEvent 166 111 0 0 0 0 t i\n"
                                      "MouseMoveEvent 165 111 0 0 0 0 t i\n"
                                      "RenderEvent 165 111 0 0 0 0 t i\n"
                                      "MouseMoveEvent 164 111 0 0 0 0 t i\n"
                                      "RenderEvent 164 111 0 0 0 0 t i\n"
                                      "MouseMoveEvent 163 111 0 0 0 0 t i\n"
                                      "RenderEvent 163 111 0 0 0 0 t i\n"
                                      "MouseMoveEvent 162 110 0 0 0 0 t i\n"
                                      "RenderEvent 162 110 0 0 0 0 t i\n"
                                      "MouseMoveEvent 161 110 0 0 0 0 t i\n"
                                      "RenderEvent 161 110 0 0 0 0 t i\n"
                                      "MouseMoveEvent 160 110 0 0 0 0 t i\n"
                                      "RenderEvent 160 110 0 0 0 0 t i\n"
                                      "MouseMoveEvent 160 111 0 0 0 0 t i\n"
                                      "RenderEvent 160 111 0 0 0 0 t i\n"
                                      "MouseMoveEvent 159 111 0 0 0 0 t i\n"
                                      "RenderEvent 159 111 0 0 0 0 t i\n"
                                      "MouseMoveEvent 159 112 0 0 0 0 t i\n"
                                      "RenderEvent 159 112 0 0 0 0 t i\n"
                                      "MouseMoveEvent 159 113 0 0 0 0 t i\n"
                                      "RenderEvent 159 113 0 0 0 0 t i\n"
                                      "MouseMoveEvent 159 114 0 0 0 0 t i\n"
                                      "RenderEvent 159 114 0 0 0 0 t i\n"
                                      "LeftButtonPressEvent 159 114 0 0 0 0 t i\n"
                                      "RenderEvent 159 114 0 0 0 0 t i\n"
                                      "MouseMoveEvent 136 178 0 0 0 0 t i\n"
                                      "RenderEvent 136 178 0 0 0 0 t i\n"
                                      "MouseMoveEvent 135 179 0 0 0 0 t i\n"
                                      "RenderEvent 135 179 0 0 0 0 t i\n"
                                      "MouseMoveEvent 135 180 0 0 0 0 t i\n"
                                      "RenderEvent 135 180 0 0 0 0 t i\n"
                                      "MouseMoveEvent 134 181 0 0 0 0 t i\n"
                                      "RenderEvent 134 181 0 0 0 0 t i\n"
                                      "MouseMoveEvent 134 182 0 0 0 0 t i\n"
                                      "RenderEvent 134 182 0 0 0 0 t i\n"
                                      "LeftButtonReleaseEvent 134 182 0 0 0 0 t i\n"
                                      "RenderEvent 134 182 0 0 0 0 t i\n"
                                      "MouseMoveEvent 134 181 0 0 0 0 t i\n"
                                      "RenderEvent 134 181 0 0 0 0 t i\n"
                                      "MouseMoveEvent 72 222 0 0 0 0 t i\n"
                                      "RenderEvent 72 222 0 0 0 0 t i\n"
                                      "MouseMoveEvent 71 223 0 0 0 0 t i\n"
                                      "RenderEvent 71 223 0 0 0 0 t i\n"
                                      "MouseMoveEvent 71 224 0 0 0 0 t i\n"
                                      "RenderEvent 71 224 0 0 0 0 t i\n"
                                      "MouseMoveEvent 71 225 0 0 0 0 t i\n"
                                      "RenderEvent 71 225 0 0 0 0 t i\n"
                                      "LeftButtonPressEvent 71 225 0 0 0 0 t i\n"
                                      "RenderEvent 71 225 0 0 0 0 t i\n"
                                      "LeftButtonReleaseEvent 71 225 0 0 0 0 t i\n"
                                      "MouseMoveEvent 70 225 0 0 0 0 t i\n"
                                      "RenderEvent 70 225 0 0 0 0 t i\n"
                                      "MouseMoveEvent 70 224 0 0 0 0 t i\n"
                                      "RenderEvent 70 224 0 0 0 0 t i\n"
                                      "MouseMoveEvent 69 223 0 0 0 0 t i\n"
                                      "RenderEvent 69 223 0 0 0 0 t i\n"
                                      "MouseMoveEvent 185 162 0 0 0 0 t i\n"
                                      "RenderEvent 185 162 0 0 0 0 t i\n"
                                      "MouseMoveEvent 184 162 0 0 0 0 t i\n"
                                      "RenderEvent 184 162 0 0 0 0 t i\n"
                                      "MouseMoveEvent 183 162 0 0 0 0 t i\n"
                                      "RenderEvent 183 162 0 0 0 0 t i\n"
                                      "MouseMoveEvent 182 162 0 0 0 0 t i\n"
                                      "RenderEvent 182 162 0 0 0 0 t i\n"
                                      "MouseMoveEvent 183 162 0 0 0 0 t i\n"
                                      "RenderEvent 183 162 0 0 0 0 t i\n"
                                      "MouseMoveEvent 184 162 0 0 0 0 t i\n"
                                      "RenderEvent 184 162 0 0 0 0 t i\n"
                                      "MouseMoveEvent 184 161 0 0 0 0 t i\n"
                                      "RenderEvent 184 161 0 0 0 0 t i\n"
                                      "MouseMoveEvent 185 161 0 0 0 0 t i\n"
                                      "RenderEvent 185 161 0 0 0 0 t i\n"
                                      "LeftButtonPressEvent 185 161 0 0 0 0 t i\n"
                                      "RenderEvent 185 161 0 0 0 0 t i\n"
                                      "MouseMoveEvent 185 160 0 0 0 0 t i\n"
                                      "RenderEvent 185 160 0 0 0 0 t i\n"
                                      "MouseMoveEvent 129 108 0 0 0 0 t i\n"
                                      "RenderEvent 129 108 0 0 0 0 t i\n"
                                      "MouseMoveEvent 129 107 0 0 0 0 t i\n"
                                      "RenderEvent 129 107 0 0 0 0 t i\n"
                                      "MouseMoveEvent 127 107 0 0 0 0 t i\n"
                                      "RenderEvent 127 107 0 0 0 0 t i\n"
                                      "MouseMoveEvent 126 106 0 0 0 0 t i\n"
                                      "RenderEvent 126 106 0 0 0 0 t i\n"
                                      "MouseMoveEvent 125 105 0 0 0 0 t i\n"
                                      "RenderEvent 125 105 0 0 0 0 t i\n"
                                      "MouseMoveEvent 124 105 0 0 0 0 t i\n"
                                      "RenderEvent 124 105 0 0 0 0 t i\n"
                                      "MouseMoveEvent 124 104 0 0 0 0 t i\n"
                                      "RenderEvent 124 104 0 0 0 0 t i\n"
                                      "LeftButtonReleaseEvent 124 104 0 0 0 0 t i\n";

// This callback is responsible for setting the seed label.
class vtkSeedCallback : public vtkCommand
{
public:
    static vtkSeedCallback* New()
    {
        return new vtkSeedCallback;
    }

    void Execute(vtkObject*, unsigned long event, void* calldata) override
    {
        if (event == vtkCommand::PlacePointEvent)
        {
            std::cout << "Point placed, total of: " << this->SeedRepresentation->GetNumberOfSeeds() << std::endl;
        }
        if (event == vtkCommand::StartInteractionEvent)
        {
            if (calldata)
            {
                std::cout << "Start interacting with seed : " << *(static_cast<int*>(calldata)) << std::endl;
            }
        }
        if (event == vtkCommand::InteractionEvent)
        {
            if (calldata)
            {
                std::cout << "Interacting with seed : " << *(static_cast<int*>(calldata)) << std::endl;
            }
        }
    }

    vtkSeedCallback()
        : SeedRepresentation(nullptr)
    {
    }

    vtkSeedRepresentation* SeedRepresentation;
};

// The actual test function
int main(int argc, char* argv[])
{
    // Create the RenderWindow, Renderer and both Actors
    //
    vtkSmartPointer<vtkRenderer> ren1       = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(ren1);

    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    iren->SetInteractorStyle(style);

    // Create a test pipeline
    //
    vtkSmartPointer<vtkSphereSource> ss       = vtkSmartPointer<vtkSphereSource>::New();
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(ss->GetOutputPort());
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // Create the widget and its representation
    vtkSmartPointer<vtkPointHandleRepresentation2D> handle = vtkSmartPointer<vtkPointHandleRepresentation2D>::New();
    handle->GetProperty()->SetColor(1, 0, 0);
    vtkSmartPointer<vtkSeedRepresentation> rep = vtkSmartPointer<vtkSeedRepresentation>::New();
    rep->SetHandleRepresentation(handle);

    vtkSmartPointer<vtkSeedWidget> widget = vtkSmartPointer<vtkSeedWidget>::New();
    widget->SetInteractor(iren);
    widget->SetRepresentation(rep);

    vtkSmartPointer<vtkSeedCallback> scbk = vtkSmartPointer<vtkSeedCallback>::New();
    scbk->SeedRepresentation              = rep;
    widget->AddObserver(vtkCommand::PlacePointEvent, scbk);
    widget->AddObserver(vtkCommand::StartInteractionEvent, scbk);
    widget->AddObserver(vtkCommand::InteractionEvent, scbk);

    // Add the actors to the renderer, set the background and size
    //
    ren1->AddActor(actor);
    ren1->SetBackground(0.1, 0.2, 0.4);
    renWin->SetSize(300, 300);
    renWin->SetMultiSamples(0);

    // render the image

    iren->Initialize();
    renWin->Render();

    int retVal = vtkTesting::InteractorEventLoop(argc, argv, iren, TestSeedWidgetEventLog);

    // test removing seeds
    const int startNumSeeds = rep->GetNumberOfSeeds();
    for (int s = 0; s < startNumSeeds; s++)
    {
        widget->DeleteSeed(0);
    }

    const int endNumSeeds = rep->GetNumberOfSeeds();
    if (endNumSeeds != 0)
    {
        std::cerr << "After deleting " << startNumSeeds << ", now have " << endNumSeeds << std::endl;
        retVal = EXIT_FAILURE;

        if (widget->GetSeed(0) != nullptr)
        {
            vtkSeedRepresentation* seedRep = vtkSeedRepresentation::SafeDownCast(widget->GetRepresentation());
            const int widgetStartNumSeeds  = seedRep->GetNumberOfSeeds();
            std::cerr << "Still have a seed 0 after deleting all seeds, " << "widget thinks it's rep has " << widgetStartNumSeeds << std::endl;
        }
    }

    return retVal;
}

#endif // TEST2

#ifdef TEST3

#include "vtkAppendPolyData.h"
#include "vtkBoxWidget.h"
#include "vtkCommand.h"
#include "vtkConeSource.h"
#include "vtkGlyph3D.h"
#include "vtkInteractorEventRecorder.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkTransform.h"

// Callback for the interaction
class vtkBWCallback : public vtkCommand
{
public:
    static vtkBWCallback* New()
    {
        return new vtkBWCallback;
    }

    void Execute(vtkObject* caller, unsigned long, void*) override
    {
        vtkBoxWidget* boxWidget = reinterpret_cast<vtkBoxWidget*>(caller);
        boxWidget->GetTransform(this->Transform);
        this->Actor->SetUserTransform(this->Transform);
    }

    vtkBWCallback()
        : Transform(nullptr)
        , Actor(nullptr)
    {
    }

    vtkTransform* Transform;
    vtkActor* Actor;
};

const char BoxWidgetEventLog[] = "# StreamVersion 1\n"
                                 "CharEvent 187 242 0 0 105 1 i\n"
                                 "KeyReleaseEvent 187 242 0 0 105 1 i\n"
                                 "MouseMoveEvent 187 241 0 0 0 0 i\n"
                                 "MouseMoveEvent 186 241 0 0 0 0 i\n"
                                 "MouseMoveEvent 184 241 0 0 0 0 i\n"
                                 "MouseMoveEvent 182 242 0 0 0 0 i\n"
                                 "MouseMoveEvent 178 242 0 0 0 0 i\n"
                                 "MouseMoveEvent 177 243 0 0 0 0 i\n"
                                 "MouseMoveEvent 175 244 0 0 0 0 i\n"
                                 "MouseMoveEvent 174 244 0 0 0 0 i\n"
                                 "MouseMoveEvent 173 245 0 0 0 0 i\n"
                                 "MouseMoveEvent 172 245 0 0 0 0 i\n"
                                 "MouseMoveEvent 171 245 0 0 0 0 i\n"
                                 "MouseMoveEvent 170 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 169 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 167 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 166 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 165 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 164 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 163 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 162 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 161 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 160 247 0 0 0 0 i\n"
                                 "MouseMoveEvent 159 248 0 0 0 0 i\n"
                                 "MouseMoveEvent 158 248 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 248 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 249 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 250 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 251 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 252 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 252 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 252 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 253 0 0 0 0 i\n"
                                 "LeftButtonPressEvent 153 253 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 252 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 252 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 251 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 250 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 249 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 248 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 247 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 246 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 245 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 244 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 243 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 242 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 241 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 240 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 239 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 238 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 237 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 236 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 235 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 234 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 232 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 231 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 230 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 229 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 228 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 228 0 0 0 0 i\n"
                                 "LeftButtonReleaseEvent 154 228 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 228 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 227 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 226 0 0 0 0 i\n"
                                 "MouseMoveEvent 151 226 0 0 0 0 i\n"
                                 "MouseMoveEvent 150 225 0 0 0 0 i\n"
                                 "MouseMoveEvent 148 223 0 0 0 0 i\n"
                                 "MouseMoveEvent 146 223 0 0 0 0 i\n"
                                 "MouseMoveEvent 144 222 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 222 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 221 0 0 0 0 i\n"
                                 "MouseMoveEvent 141 221 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 219 0 0 0 0 i\n"
                                 "MouseMoveEvent 130 216 0 0 0 0 i\n"
                                 "MouseMoveEvent 120 212 0 0 0 0 i\n"
                                 "MouseMoveEvent 111 209 0 0 0 0 i\n"
                                 "MouseMoveEvent 100 204 0 0 0 0 i\n"
                                 "MouseMoveEvent 97 204 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 202 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 201 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 200 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 199 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 198 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 197 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 196 0 0 0 0 i\n"
                                 "LeftButtonPressEvent 96 196 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 195 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 194 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 193 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 192 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 191 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 190 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 189 0 0 0 0 i\n"
                                 "MouseMoveEvent 96 188 0 0 0 0 i\n"
                                 "MouseMoveEvent 97 188 0 0 0 0 i\n"
                                 "MouseMoveEvent 97 187 0 0 0 0 i\n"
                                 "MouseMoveEvent 98 186 0 0 0 0 i\n"
                                 "MouseMoveEvent 98 185 0 0 0 0 i\n"
                                 "MouseMoveEvent 99 185 0 0 0 0 i\n"
                                 "MouseMoveEvent 99 184 0 0 0 0 i\n"
                                 "MouseMoveEvent 99 182 0 0 0 0 i\n"
                                 "MouseMoveEvent 100 182 0 0 0 0 i\n"
                                 "MouseMoveEvent 101 179 0 0 0 0 i\n"
                                 "MouseMoveEvent 103 178 0 0 0 0 i\n"
                                 "MouseMoveEvent 106 177 0 0 0 0 i\n"
                                 "MouseMoveEvent 109 177 0 0 0 0 i\n"
                                 "MouseMoveEvent 112 176 0 0 0 0 i\n"
                                 "MouseMoveEvent 115 175 0 0 0 0 i\n"
                                 "MouseMoveEvent 117 174 0 0 0 0 i\n"
                                 "MouseMoveEvent 118 174 0 0 0 0 i\n"
                                 "MouseMoveEvent 119 174 0 0 0 0 i\n"
                                 "MouseMoveEvent 119 173 0 0 0 0 i\n"
                                 "MouseMoveEvent 120 173 0 0 0 0 i\n"
                                 "MouseMoveEvent 121 172 0 0 0 0 i\n"
                                 "MouseMoveEvent 121 171 0 0 0 0 i\n"
                                 "MouseMoveEvent 122 171 0 0 0 0 i\n"
                                 "MouseMoveEvent 123 171 0 0 0 0 i\n"
                                 "MouseMoveEvent 123 170 0 0 0 0 i\n"
                                 "MouseMoveEvent 124 170 0 0 0 0 i\n"
                                 "MouseMoveEvent 125 170 0 0 0 0 i\n"
                                 "MouseMoveEvent 125 169 0 0 0 0 i\n"
                                 "MouseMoveEvent 128 169 0 0 0 0 i\n"
                                 "MouseMoveEvent 129 168 0 0 0 0 i\n"
                                 "MouseMoveEvent 130 168 0 0 0 0 i\n"
                                 "MouseMoveEvent 132 168 0 0 0 0 i\n"
                                 "MouseMoveEvent 133 168 0 0 0 0 i\n"
                                 "MouseMoveEvent 135 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 138 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 140 168 0 0 0 0 i\n"
                                 "MouseMoveEvent 141 168 0 0 0 0 i\n"
                                 "MouseMoveEvent 142 168 0 0 0 0 i\n"
                                 "MouseMoveEvent 142 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 144 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 145 166 0 0 0 0 i\n"
                                 "MouseMoveEvent 146 166 0 0 0 0 i\n"
                                 "MouseMoveEvent 147 166 0 0 0 0 i\n"
                                 "MouseMoveEvent 148 166 0 0 0 0 i\n"
                                 "MouseMoveEvent 149 166 0 0 0 0 i\n"
                                 "MouseMoveEvent 150 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 151 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 152 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 152 168 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 168 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 167 0 0 0 0 i\n"
                                 "LeftButtonReleaseEvent 155 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 166 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 166 0 0 0 0 i\n"
                                 "MouseMoveEvent 158 166 0 0 0 0 i\n"
                                 "MouseMoveEvent 159 167 0 0 0 0 i\n"
                                 "MouseMoveEvent 159 168 0 0 0 0 i\n"
                                 "MouseMoveEvent 159 169 0 0 0 0 i\n"
                                 "MouseMoveEvent 158 169 0 0 0 0 i\n"
                                 "MouseMoveEvent 158 170 0 0 0 0 i\n"
                                 "MouseMoveEvent 157 170 0 0 0 0 i\n"
                                 "MouseMoveEvent 157 171 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 172 0 0 0 0 i\n"
                                 "RightButtonPressEvent 156 172 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 173 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 173 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 173 0 0 0 0 i\n"
                                 "MouseMoveEvent 150 174 0 0 0 0 i\n"
                                 "MouseMoveEvent 150 176 0 0 0 0 i\n"
                                 "MouseMoveEvent 149 177 0 0 0 0 i\n"
                                 "MouseMoveEvent 148 178 0 0 0 0 i\n"
                                 "MouseMoveEvent 148 181 0 0 0 0 i\n"
                                 "MouseMoveEvent 147 184 0 0 0 0 i\n"
                                 "MouseMoveEvent 147 186 0 0 0 0 i\n"
                                 "MouseMoveEvent 147 187 0 0 0 0 i\n"
                                 "MouseMoveEvent 146 187 0 0 0 0 i\n"
                                 "MouseMoveEvent 146 188 0 0 0 0 i\n"
                                 "MouseMoveEvent 146 189 0 0 0 0 i\n"
                                 "MouseMoveEvent 145 190 0 0 0 0 i\n"
                                 "MouseMoveEvent 144 190 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 192 0 0 0 0 i\n"
                                 "MouseMoveEvent 141 194 0 0 0 0 i\n"
                                 "MouseMoveEvent 140 194 0 0 0 0 i\n"
                                 "MouseMoveEvent 140 195 0 0 0 0 i\n"
                                 "MouseMoveEvent 139 195 0 0 0 0 i\n"
                                 "MouseMoveEvent 138 195 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 196 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 197 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 198 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 199 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 200 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 201 0 0 0 0 i\n"
                                 "MouseMoveEvent 138 202 0 0 0 0 i\n"
                                 "MouseMoveEvent 138 203 0 0 0 0 i\n"
                                 "MouseMoveEvent 139 203 0 0 0 0 i\n"
                                 "MouseMoveEvent 140 203 0 0 0 0 i\n"
                                 "MouseMoveEvent 141 202 0 0 0 0 i\n"
                                 "MouseMoveEvent 142 202 0 0 0 0 i\n"
                                 "MouseMoveEvent 145 203 0 0 0 0 i\n"
                                 "MouseMoveEvent 149 204 0 0 0 0 i\n"
                                 "MouseMoveEvent 150 205 0 0 0 0 i\n"
                                 "MouseMoveEvent 151 205 0 0 0 0 i\n"
                                 "RightButtonReleaseEvent 151 205 0 0 0 0 i\n"
                                 "MouseMoveEvent 151 205 0 0 0 0 i\n"
                                 "MouseMoveEvent 151 204 0 0 0 0 i\n"
                                 "MouseMoveEvent 150 204 0 0 0 0 i\n"
                                 "RightButtonPressEvent 150 204 0 0 0 0 i\n"
                                 "MouseMoveEvent 150 203 0 0 0 0 i\n"
                                 "MouseMoveEvent 150 202 0 0 0 0 i\n"
                                 "MouseMoveEvent 151 201 0 0 0 0 i\n"
                                 "MouseMoveEvent 152 201 0 0 0 0 i\n"
                                 "MouseMoveEvent 152 200 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 200 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 199 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 198 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 196 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 195 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 194 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 191 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 190 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 189 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 188 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 187 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 186 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 185 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 183 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 182 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 181 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 180 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 179 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 178 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 177 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 176 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 175 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 176 0 0 0 0 i\n"
                                 "RightButtonReleaseEvent 153 176 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 176 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 176 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 177 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 178 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 179 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 180 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 180 0 0 0 0 i\n"
                                 "MiddleButtonPressEvent 155 180 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 180 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 181 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 181 0 0 0 0 i\n"
                                 "MouseMoveEvent 152 181 0 0 0 0 i\n"
                                 "MouseMoveEvent 151 181 0 0 0 0 i\n"
                                 "MouseMoveEvent 148 181 0 0 0 0 i\n"
                                 "MouseMoveEvent 141 182 0 0 0 0 i\n"
                                 "MouseMoveEvent 139 181 0 0 0 0 i\n"
                                 "MouseMoveEvent 139 182 0 0 0 0 i\n"
                                 "MouseMoveEvent 138 183 0 0 0 0 i\n"
                                 "MouseMoveEvent 138 184 0 0 0 0 i\n"
                                 "MouseMoveEvent 139 185 0 0 0 0 i\n"
                                 "MouseMoveEvent 140 186 0 0 0 0 i\n"
                                 "MouseMoveEvent 141 186 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 186 0 0 0 0 i\n"
                                 "MouseMoveEvent 147 189 0 0 0 0 i\n"
                                 "MouseMoveEvent 151 189 0 0 0 0 i\n"
                                 "MouseMoveEvent 153 190 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 190 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 191 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 192 0 0 0 0 i\n"
                                 "MouseMoveEvent 157 192 0 0 0 0 i\n"
                                 "MouseMoveEvent 157 193 0 0 0 0 i\n"
                                 "MouseMoveEvent 158 194 0 0 0 0 i\n"
                                 "MouseMoveEvent 159 194 0 0 0 0 i\n"
                                 "MouseMoveEvent 159 195 0 0 0 0 i\n"
                                 "MouseMoveEvent 160 195 0 0 0 0 i\n"
                                 "MouseMoveEvent 160 197 0 0 0 0 i\n"
                                 "MouseMoveEvent 162 198 0 0 0 0 i\n"
                                 "MouseMoveEvent 166 199 0 0 0 0 i\n"
                                 "MouseMoveEvent 167 200 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 200 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 201 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 202 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 203 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 204 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 205 0 0 0 0 i\n"
                                 "MouseMoveEvent 167 205 0 0 0 0 i\n"
                                 "MouseMoveEvent 167 206 0 0 0 0 i\n"
                                 "MouseMoveEvent 166 208 0 0 0 0 i\n"
                                 "MouseMoveEvent 165 209 0 0 0 0 i\n"
                                 "MouseMoveEvent 165 210 0 0 0 0 i\n"
                                 "MouseMoveEvent 164 210 0 0 0 0 i\n"
                                 "MouseMoveEvent 164 211 0 0 0 0 i\n"
                                 "MouseMoveEvent 165 211 0 0 0 0 i\n"
                                 "MouseMoveEvent 165 212 0 0 0 0 i\n"
                                 "MouseMoveEvent 166 212 0 0 0 0 i\n"
                                 "MouseMoveEvent 166 213 0 0 0 0 i\n"
                                 "MouseMoveEvent 166 214 0 0 0 0 i\n"
                                 "MouseMoveEvent 166 215 0 0 0 0 i\n"
                                 "MouseMoveEvent 166 216 0 0 0 0 i\n"
                                 "MouseMoveEvent 166 217 0 0 0 0 i\n"
                                 "MouseMoveEvent 167 218 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 218 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 219 0 0 0 0 i\n"
                                 "MouseMoveEvent 169 220 0 0 0 0 i\n"
                                 "MouseMoveEvent 169 221 0 0 0 0 i\n"
                                 "MouseMoveEvent 169 222 0 0 0 0 i\n"
                                 "MouseMoveEvent 169 223 0 0 0 0 i\n"
                                 "MouseMoveEvent 169 224 0 0 0 0 i\n"
                                 "MouseMoveEvent 169 223 0 0 0 0 i\n"
                                 "MouseMoveEvent 170 222 0 0 0 0 i\n"
                                 "MouseMoveEvent 170 221 0 0 0 0 i\n"
                                 "MiddleButtonReleaseEvent 170 221 0 0 0 0 i\n"
                                 "MouseMoveEvent 170 221 0 0 0 0 i\n"
                                 "MouseMoveEvent 170 220 0 0 0 0 i\n"
                                 "MouseMoveEvent 170 219 0 0 0 0 i\n"
                                 "MouseMoveEvent 170 218 0 0 0 0 i\n"
                                 "MouseMoveEvent 170 217 0 0 0 0 i\n"
                                 "MouseMoveEvent 169 216 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 216 0 0 0 0 i\n"
                                 "MouseMoveEvent 168 215 0 0 0 0 i\n"
                                 "MouseMoveEvent 167 214 0 0 0 0 i\n"
                                 "MouseMoveEvent 164 214 0 0 0 0 i\n"
                                 "MouseMoveEvent 163 213 0 0 0 0 i\n"
                                 "MouseMoveEvent 163 212 0 0 0 0 i\n"
                                 "MouseMoveEvent 162 210 0 0 0 0 i\n"
                                 "MouseMoveEvent 162 209 0 0 0 0 i\n"
                                 "MouseMoveEvent 162 206 0 0 0 0 i\n"
                                 "MouseMoveEvent 161 202 0 0 0 0 i\n"
                                 "MouseMoveEvent 160 197 0 0 0 0 i\n"
                                 "MouseMoveEvent 160 192 0 0 0 0 i\n"
                                 "MouseMoveEvent 160 187 0 0 0 0 i\n"
                                 "MouseMoveEvent 158 182 0 0 0 0 i\n"
                                 "MouseMoveEvent 157 174 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 169 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 164 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 157 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 156 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 152 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 151 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 148 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 146 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 145 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 143 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 142 0 0 0 0 i\n"
                                 "MouseMoveEvent 156 141 0 0 0 0 i\n"
                                 "MouseMoveEvent 155 140 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 140 0 0 0 0 i\n"
                                 "MouseMoveEvent 154 139 0 0 0 0 i\n"
                                 "MouseMoveEvent 151 137 0 0 0 0 i\n"
                                 "MouseMoveEvent 149 136 0 0 0 0 i\n"
                                 "MouseMoveEvent 147 135 0 0 0 0 i\n"
                                 "MouseMoveEvent 144 132 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 132 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 131 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 130 0 0 0 0 i\n"
                                 "LeftButtonPressEvent 143 130 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 129 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 126 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 125 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 123 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 120 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 118 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 116 0 0 0 0 i\n"
                                 "MouseMoveEvent 143 115 0 0 0 0 i\n"
                                 "MouseMoveEvent 142 115 0 0 0 0 i\n"
                                 "MouseMoveEvent 142 114 0 0 0 0 i\n"
                                 "MouseMoveEvent 142 113 0 0 0 0 i\n"
                                 "MouseMoveEvent 141 113 0 0 0 0 i\n"
                                 "MouseMoveEvent 141 112 0 0 0 0 i\n"
                                 "MouseMoveEvent 141 111 0 0 0 0 i\n"
                                 "MouseMoveEvent 141 110 0 0 0 0 i\n"
                                 "MouseMoveEvent 140 109 0 0 0 0 i\n"
                                 "MouseMoveEvent 140 108 0 0 0 0 i\n"
                                 "MouseMoveEvent 139 108 0 0 0 0 i\n"
                                 "MouseMoveEvent 138 107 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 104 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 103 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 102 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 101 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 100 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 99 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 98 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 97 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 96 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 95 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 94 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 93 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 92 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 91 0 0 0 0 i\n"
                                 "MouseMoveEvent 137 90 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 89 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 88 0 0 0 0 i\n"
                                 "LeftButtonReleaseEvent 136 88 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 88 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 89 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 91 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 92 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 91 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 90 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 89 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 90 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 91 0 0 0 0 i\n"
                                 "MouseMoveEvent 136 92 0 0 0 0 i\n"
                                 "MouseMoveEvent 135 93 0 0 0 0 i\n";

int main(int, char*[])
{
    vtkSmartPointer<vtkRenderer> renderer   = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    vtkSmartPointer<vtkBoxWidget> boxWidget = vtkSmartPointer<vtkBoxWidget>::New();
    boxWidget->SetInteractor(iren);
    boxWidget->SetPlaceFactor(1.25);

    vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
    cone->SetResolution(6);
    vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetThetaResolution(8);
    sphere->SetPhiResolution(8);
    vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputConnection(sphere->GetOutputPort());
    glyph->SetSourceConnection(cone->GetOutputPort());
    glyph->SetVectorModeToUseNormal();
    glyph->SetScaleModeToScaleByVector();
    glyph->SetScaleFactor(0.25);

    vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
    append->AddInputConnection(glyph->GetOutputPort());
    append->AddInputConnection(sphere->GetOutputPort());

    vtkSmartPointer<vtkPolyDataMapper> maceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    maceMapper->SetInputConnection(append->GetOutputPort());

    vtkSmartPointer<vtkActor> maceActor = vtkSmartPointer<vtkActor>::New();
    maceActor->SetMapper(maceMapper);

    renderer->AddActor(maceActor);
    renderer->SetBackground(0, 0, 0);
    renWin->SetSize(300, 300);

    // Configure the box widget including callbacks
    vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
    boxWidget->SetProp3D(maceActor);
    boxWidget->PlaceWidget();

    vtkSmartPointer<vtkBWCallback> myCallback = vtkSmartPointer<vtkBWCallback>::New();
    myCallback->Transform                     = t;
    myCallback->Actor                         = maceActor;
    boxWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

    // record events
    vtkSmartPointer<vtkInteractorEventRecorder> recorder = vtkSmartPointer<vtkInteractorEventRecorder>::New();
    recorder->SetInteractor(iren);
    //  recorder->SetFileName("c:/record.log");
    //  recorder->Record();
    recorder->ReadFromInputStringOn();
    recorder->SetInputString(BoxWidgetEventLog);

    // interact with data
    // render the image
    //
    iren->Initialize();
    renWin->Render();
    recorder->Play();

    // Remove the observers so we can go interactive. Without this the "-I"
    // testing option fails.
    recorder->Off();

    iren->Start();

    // Clean up
    recorder->Off();

    return EXIT_SUCCESS;
}

#endif // TEST3
