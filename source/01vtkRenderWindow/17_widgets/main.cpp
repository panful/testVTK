/**
 * 101. 比例尺(vtkLegendScaleActor)
 * 102. 通过继承重写vtkLegendScaleActor的函数，指定主比例尺的位置
 *
 */

#define TEST102

#ifdef TEST101

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLegendScaleActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

int main(int, char*[])
{
    vtkNew<vtkLegendScaleActor> legendScaleActor;
    // legendScaleActor->VisibilityOff();           // 所有都不显示
    // legendScaleActor->LegendVisibilityOff();     // 比例尺不显示
    // legendScaleActor->BottomAxisVisibilityOff(); // 下面的轴不显示
    // legendScaleActor->LeftAxisVisibilityOff();   // 左边
    // legendScaleActor->RightAxisVisibilityOff();  // 右边
    // legendScaleActor->TopAxisVisibilityOff();    // 上边

    // 设置4个轴的标签模式
    legendScaleActor->SetLabelModeToXYCoordinates(); // 视角平行于Z，使用XY坐标
    legendScaleActor->SetLabelModeToDistance();      // 两点之间的距离

    // 设置四个轴到窗口边缘的距离
    legendScaleActor->SetTopBorderOffset(50);
    legendScaleActor->SetRightBorderOffset(50);
    legendScaleActor->SetBottomBorderOffset(50);
    legendScaleActor->SetLeftBorderOffset(50);

    legendScaleActor->SetCornerOffsetFactor(10); // 设置四个次比例尺相交距离
    legendScaleActor->UseBoundsOff();            // 计算边界时，是否考虑该比例尺的边界，例如 ResetCamera

    //---------------------------------------------------------------------------
    // 文本属性
    auto labelProperty = legendScaleActor->GetLegendLabelProperty(); // 主比例尺标签文本属性
    auto titleProperty = legendScaleActor->GetLegendTitleProperty(); // 主比例尺标题文本属性

    labelProperty->SetColor(1, 0, 0);
    labelProperty->SetLineOffset(-10); // 标签距离屏幕下方边界的像素个数，标签越往上参数应该越小
    labelProperty->SetFontSize(labelProperty->GetFontSize() * 2);

    titleProperty->SetColor(0, 1, 0);
    titleProperty->SetFontSize(titleProperty->GetFontSize() * 2);
    titleProperty->SetLineOffset(-100); // 好像不起作用

    //---------------------------------------------------------------------------
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(legendScaleActor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST101

#ifdef TEST102

#include <vtkActor.h>
#include <vtkAxisActor2D.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLegendScaleActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

namespace {
class MyLegend : public vtkLegendScaleActor
{
public:
    static MyLegend* New();
    vtkTypeMacro(MyLegend, vtkLegendScaleActor);

    void BuildRepresentation(vtkViewport* viewport) override
    {
        if (true)
        {
            // Specify the locations of the axes.
            const int* size = viewport->GetSize();

            this->RightAxis->GetPositionCoordinate()->SetValue(
                size[0] - this->RightBorderOffset, this->CornerOffsetFactor * this->BottomBorderOffset, 0.0);
            this->RightAxis->GetPosition2Coordinate()->SetValue(
                size[0] - this->RightBorderOffset, size[1] - this->CornerOffsetFactor * this->TopBorderOffset, 0.0);

            this->TopAxis->GetPositionCoordinate()->SetValue(
                size[0] - this->CornerOffsetFactor * this->RightBorderOffset, size[1] - this->TopBorderOffset, 0.0);
            this->TopAxis->GetPosition2Coordinate()->SetValue(
                this->CornerOffsetFactor * this->LeftBorderOffset, size[1] - this->TopBorderOffset, 0.0);

            this->LeftAxis->GetPositionCoordinate()->SetValue(
                this->LeftBorderOffset, size[1] - this->CornerOffsetFactor * this->TopBorderOffset, 0.0);
            this->LeftAxis->GetPosition2Coordinate()->SetValue(this->LeftBorderOffset, this->CornerOffsetFactor * this->BottomBorderOffset, 0.0);

            if (this->LegendVisibility)
            {
                this->BottomAxis->GetPositionCoordinate()->SetValue(
                    this->CornerOffsetFactor * this->LeftBorderOffset, 2 * this->BottomBorderOffset, 0.0);
                this->BottomAxis->GetPosition2Coordinate()->SetValue(
                    size[0] - this->CornerOffsetFactor * this->RightBorderOffset, 2 * this->BottomBorderOffset, 0.0);
            }
            else
            {
                this->BottomAxis->GetPositionCoordinate()->SetValue(this->CornerOffsetFactor * this->LeftBorderOffset, this->BottomBorderOffset, 0.0);
                this->BottomAxis->GetPosition2Coordinate()->SetValue(
                    size[0] - this->CornerOffsetFactor * this->RightBorderOffset, this->BottomBorderOffset, 0.0);
            }

            // Now specify the axis values
            if (this->LabelMode == XY_COORDINATES)
            {
                double* xL = this->RightAxis->GetPositionCoordinate()->GetComputedWorldValue(viewport);
                double* xR = this->RightAxis->GetPosition2Coordinate()->GetComputedWorldValue(viewport);
                this->RightAxis->SetRange(xL[1], xR[1]);

                xL = this->TopAxis->GetPositionCoordinate()->GetComputedWorldValue(viewport);
                xR = this->TopAxis->GetPosition2Coordinate()->GetComputedWorldValue(viewport);
                this->TopAxis->SetRange(xL[0], xR[0]);

                xL = this->LeftAxis->GetPositionCoordinate()->GetComputedWorldValue(viewport);
                xR = this->LeftAxis->GetPosition2Coordinate()->GetComputedWorldValue(viewport);
                this->LeftAxis->SetRange(xL[1], xR[1]);

                xL = this->BottomAxis->GetPositionCoordinate()->GetComputedWorldValue(viewport);
                xR = this->BottomAxis->GetPosition2Coordinate()->GetComputedWorldValue(viewport);
                this->BottomAxis->SetRange(xL[0], xR[0]);
            }
            else // distance between points
            {
                double d;

                double* xL = this->RightAxis->GetPositionCoordinate()->GetComputedWorldValue(viewport);
                double* xR = this->RightAxis->GetPosition2Coordinate()->GetComputedWorldValue(viewport);
                d          = sqrt(vtkMath::Distance2BetweenPoints(xL, xR));
                this->RightAxis->SetRange(-d / 2.0, d / 2.0);

                xL = this->TopAxis->GetPositionCoordinate()->GetComputedWorldValue(viewport);
                xR = this->TopAxis->GetPosition2Coordinate()->GetComputedWorldValue(viewport);
                d  = sqrt(vtkMath::Distance2BetweenPoints(xL, xR));
                this->TopAxis->SetRange(d / 2.0, -d / 2.0);

                xL = this->LeftAxis->GetPositionCoordinate()->GetComputedWorldValue(viewport);
                xR = this->LeftAxis->GetPosition2Coordinate()->GetComputedWorldValue(viewport);
                d  = sqrt(vtkMath::Distance2BetweenPoints(xL, xR));
                this->LeftAxis->SetRange(d / 2.0, -d / 2.0);

                xL = this->BottomAxis->GetPositionCoordinate()->GetComputedWorldValue(viewport);
                xR = this->BottomAxis->GetPosition2Coordinate()->GetComputedWorldValue(viewport);
                d  = sqrt(vtkMath::Distance2BetweenPoints(xL, xR));
                this->BottomAxis->SetRange(-d / 2.0, d / 2.0);
            }

            if (this->LegendVisibility)
            {
                // Update the position
                double x1   = 0.33333 * size[0];
                double delX = x1 / 4.0;

                double offset_y { 100. }; // 给主比例尺加一个y方向的偏移

                // 主比例尺的十个顶点
                // clang-format off
                this->LegendPoints->SetPoint(0, x1,             10 + offset_y, 0);
                this->LegendPoints->SetPoint(1, x1 + delX,      10 + offset_y, 0);
                this->LegendPoints->SetPoint(2, x1 + 2 * delX,  10 + offset_y, 0);
                this->LegendPoints->SetPoint(3, x1 + 3 * delX,  10 + offset_y, 0);
                this->LegendPoints->SetPoint(4, x1 + 4 * delX,  10 + offset_y, 0);
                this->LegendPoints->SetPoint(5, x1,             20 + offset_y, 0);
                this->LegendPoints->SetPoint(6, x1 + delX,      20 + offset_y, 0);
                this->LegendPoints->SetPoint(7, x1 + 2 * delX,  20 + offset_y, 0);
                this->LegendPoints->SetPoint(8, x1 + 3 * delX,  20 + offset_y, 0);
                this->LegendPoints->SetPoint(9, x1 + 4 * delX,  20 + offset_y, 0);
                this->LegendPoints->Modified();
                // clang-format on

                // Specify the position of the legend title
                this->LabelActors[5]->SetPosition(0.5 * size[0], 22 + offset_y); // 主比例尺的标题位置，y方向同样需要偏移
                this->Coordinate->SetValue(0.33333 * size[0], 15, 0.0);
                double* x = this->Coordinate->GetComputedWorldValue(viewport);
                double xL[3];
                xL[0] = x[0];
                xL[1] = x[1];
                xL[2] = x[2];
                this->Coordinate->SetValue(0.66667 * size[0], 15, 0.0);
                x = this->Coordinate->GetComputedWorldValue(viewport);
                double xR[3];
                xR[0]      = x[0];
                xR[1]      = x[1];
                xR[2]      = x[2];
                double len = sqrt(vtkMath::Distance2BetweenPoints(xL, xR));
                char buf[256];
                snprintf(buf, sizeof(buf), "Scale 1 : %g", len); // 主比例尺标题内容
                this->LabelMappers[5]->SetInput(buf);

                // Now specify the position of the legend labels 主比例尺的标签位置
                x = this->LegendPoints->GetPoint(0);
                this->LabelActors[0]->SetPosition(x[0], x[1] - 1);
                x = this->LegendPoints->GetPoint(1);
                this->LabelActors[1]->SetPosition(x[0], x[1] - 1);
                x = this->LegendPoints->GetPoint(2);
                this->LabelActors[2]->SetPosition(x[0], x[1] - 1);
                x = this->LegendPoints->GetPoint(3);
                this->LabelActors[3]->SetPosition(x[0], x[1] - 1);
                x = this->LegendPoints->GetPoint(4);
                this->LabelActors[4]->SetPosition(x[0], x[1] - 1);
            }

            this->BuildTime.Modified();
        }
    }

protected:
};

vtkStandardNewMacro(MyLegend);
} // namespace

int main(int, char*[])
{ 
    // 位置可以通过继承vtkLegendScaleActor并重新实现BuildRepresentation方法并修改LegendPoints的坐标来修改
    vtkNew<MyLegend> legendScaleActor;
    legendScaleActor->BottomAxisVisibilityOff(); // 下面的轴不显示
    legendScaleActor->LeftAxisVisibilityOff();   // 左边
    legendScaleActor->RightAxisVisibilityOff();  // 右边
    legendScaleActor->TopAxisVisibilityOff();    // 上边

    //---------------------------------------------------------------------------
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(legendScaleActor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST102