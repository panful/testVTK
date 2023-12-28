/**
 * 101. vtkLegendScaleActor 比例尺
 * 102. 通过继承重写vtkLegendScaleActor的函数，指定主比例尺的位置
 * 103. 比例尺的比例
 *
 * 201. vtkScalarBarActor 色卡
 * 202. 修复色卡相邻两个颜色相同的问题
 * 203. 获取色卡某一颜色的具体位置，用来标记某一个颜色条
 * 204. 自定义色卡的标签，解决因为double的精度导致标签计算错误
 * 205. vtkScalarBarWidget 拖动色卡
 *
 * 301. vtkLogoWidget 显示logo图片
 *
 * 401. vtkAxesActor 箭头坐标轴
 * 402. vtkAnnotatedCubeActor 立方体坐标轴
 * 403. vtkCameraOrientationWidget 拾取到某一个方向后，以动画方式跳转到该方向
 * 404. vtkOrientationMarkerWidget 用来操作标记的2D小部件，比如将坐标轴放到该部件中，就可以随意移动坐标轴
 *
 * 501. vtkButtonWidget 按钮
 *
 * 601. vtkBorderWidget 边框，vtkLogoWidget vtkTextWidget vtkScalarBarWidget 等继承自 vtkBorderWidget
 *
 */

#define TEST203

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

#ifdef TEST103

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLegendScaleActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

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
            auto renderer = this->GetCurrentRenderer();
            auto camera   = renderer->GetActiveCamera();
            std::cout << "-----------------------------------------------\n";
            camera->Print(std::cout);
        }
    }
};

vtkStandardNewMacro(CustomStyle);

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cubeSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkLegendScaleActor> scale;
    scale->BottomAxisVisibilityOff();
    scale->LeftAxisVisibilityOff();
    scale->RightAxisVisibilityOff();
    scale->TopAxisVisibilityOff();

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor(actor);
    renderer->AddActor(scale);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->SetParallelProjection(true); // 平行投影

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<CustomStyle> style;
    iren->SetInteractorStyle(style);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST103

#ifdef TEST201

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
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetNumberOfColors(20);
    hueLut->SetNumberOfTableValues(20);
    hueLut->SetHueRange(0.0, 0.67);
    hueLut->Build();

    //---------------------------------------------------------
    // 如果LookupTable的颜色个数大于MaximumNumberOfColors，则显示MaximumNumberOfColors个颜色
    // 如果LookupTable的颜色个数小于MaximumNumberOfColors，则显示的颜色个数等于LookupTable的颜色个数
    // MaximumNumberOfColors太小会导致两头的颜色不显示
    // 默认显示个颜色64
    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLookupTable(hueLut);
    scalarBar->SetNumberOfLabels(20);        // 设置标签个数
    scalarBar->SetMaximumNumberOfColors(10); // 设置最大颜色个数
    scalarBar->SetMaximumWidthInPixels(80); // 设置色卡的宽所占最大像素，该值较小时，标签为负值和正值时字体可能大小不一样
    scalarBar->SetMaximumHeightInPixels(900); // 设置色卡的高所占最大像素
    scalarBar->SetPosition(.1, .1);           // 设置左下角位置
    scalarBar->SetPosition2(.3, .8);          // 设置宽和高

    auto pos1 = scalarBar->GetPosition();
    auto pos2 = scalarBar->GetPosition2();

    std::cout << "pos1: " << pos1[0] << '\t' << pos1[1] << '\n';
    std::cout << "pos2: " << pos2[0] << '\t' << pos2[1] << '\n';

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor2D(scalarBar);
    renderer->SetBackground(0., 0., 0.);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST201

#ifdef TEST202

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetNumberOfColors(20);
    hueLut->SetHueRange(0.0, 0.67);
    hueLut->SetRange(-8, 8);
    hueLut->Build();

    //---------------------------------------------------------
    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLookupTable(hueLut);
    scalarBar->SetMaximumNumberOfColors(20);
    scalarBar->SetNumberOfLabels(20);

    //
    // vtkScalarBarActor::ConfigureScalarBar()在该函数中设置色卡的每一个颜色时，
    // 可能会因为浮点数精度问题导致设置的颜色错误，即两个相邻的颜色值相同
    // 修改：rgbval = range[0] + (range[1] - range[0]) * (i / static_cast<double>(this->P->NumColors));
    // rgbval = range[0] + (range[1] - range[0]) * ((i + 0.5) / static_cast<double>(this->P->NumColors));
    // 继承一个vtkScalarBarActor重写该方法即可
    //

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor2D(scalarBar);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST202

#ifdef TEST203

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkCommand.h>
#include <vtkFloatArray.h>
#include <vtkImageMapToColors.h>
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
#include <vtkScalarBarWidget.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkViewport.h>

class Callback : public vtkCommand
{
public:
    static Callback* New()
    {
        return new Callback;
    }

    void Execute(vtkObject* caller, unsigned long x, void* y) override
    {
        if (auto interactor = vtkRenderWindowInteractor::SafeDownCast(caller))
        {
            auto size = interactor->GetRenderWindow()->GetSize();
            auto pos  = interactor->GetEventPosition();
            std::cout << "Window size : " << size[0] << ',' << size[1] << '\t';
            std::cout << "Mouse pos : " << pos[0] << ',' << pos[1] << '\n';
        }

        if (m_bar && m_renderer)
        {
            std::cout << "-------------------------------------------------------------\n";

            // 色卡的色带部分在渲染窗口中的屏幕坐标
            int rect[4] { 0 };
            m_bar->GetScalarBarRect(rect, m_renderer);
            std::cout << "ScalarBarRect : " << rect[0] << '\t' << rect[1] << '\t' << rect[2] << '\t' << rect[3] << '\n';

            auto range = m_bar->GetLookupTable()->GetRange();
            std::cout << "Range: " << range[0] << '\t' << range[1] << '\n';

            // 获取指定scalar在颜色映射表中的索引
            if (auto lut = vtkLookupTable::SafeDownCast(m_bar->GetLookupTable()))
            {
                double scalar  = 5.0;
                auto index     = lut->GetIndex(scalar);
                auto numColors = lut->GetNumberOfColors();
                auto indexLu   = lut->GetIndexedLookup();
                std::cout << "indexedLookup: " << indexLu << "\tcolor num: " << numColors << "\tscalar: " << scalar << "\tindex: " << index << '\n';
            }
        }
    }

    void SetScalarBar(vtkSmartPointer<vtkScalarBarActor> bar)
    {
        m_bar = bar;
    }

    void SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
    {
        m_renderer = renderer;
    }

private:
    vtkSmartPointer<vtkScalarBarActor> m_bar { nullptr };
    vtkSmartPointer<vtkRenderer> m_renderer { nullptr };
};

int main(int, char*[])
{
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetNumberOfColors(10);
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->SetRange(0., 10.);
    hueLut->Build();

    //---------------------------------------------------------
    vtkNew<vtkScalarBarActor> scalarBar;
    std::cout << "------------------------------------------\n";
    std::cout << "GetNumberOfLabels \t" << scalarBar->GetNumberOfLabels() << '\n';
    std::cout << "GetNumberOfConsumers\t" << scalarBar->GetNumberOfConsumers() << '\n';
    std::cout << "GetNumberOfLabelsMinValue\t" << scalarBar->GetNumberOfLabelsMinValue() << '\n';
    std::cout << "GetNumberOfLabelsMaxValue\t" << scalarBar->GetNumberOfLabelsMaxValue() << '\n';
    std::cout << "GetNumberOfPaths\t" << scalarBar->GetNumberOfPaths() << '\n';
    std::cout << "GetPostion\t" << scalarBar->GetPosition()[0] << '\t' << scalarBar->GetPosition()[1] << '\n';
    std::cout << "GetPostion2\t" << scalarBar->GetPosition2()[0] << '\t' << scalarBar->GetPosition2()[1] << '\n';

    scalarBar->SetLookupTable(hueLut);
    scalarBar->SetTextPad(5);        // 设置文本框周围的填充量，就是标签文本距离颜色映射表的距离
    scalarBar->SetPosition(.2, .2);  // 设置位置，左下角
    scalarBar->SetPosition2(.2, .6); // 宽和高，还包括Title的大小
    std::cout << "GetPostion\t" << scalarBar->GetPosition()[0] << '\t' << scalarBar->GetPosition()[1] << '\n';
    std::cout << "GetPostion2\t" << scalarBar->GetPosition2()[0] << '\t' << scalarBar->GetPosition2()[1] << '\n';
    std::cout << "GetBarRatio\t" << scalarBar->GetBarRatio() << '\n'; // 获取颜色条相对于小部件框架的厚度。

    std::cout << "GetPositionCoordinate\t" << scalarBar->GetPositionCoordinate()->GetValue()[0] << '\t'
              << scalarBar->GetPositionCoordinate()->GetValue()[1] << '\n';
    std::cout << "GetPosition2Coordinate\t" << scalarBar->GetPosition2Coordinate()->GetValue()[0] << '\t'
              << scalarBar->GetPosition2Coordinate()->GetValue()[1] << '\n';

    std::cout << "GetHeight\t" << scalarBar->GetHeight() << '\n';
    std::cout << "GetWidth\t" << scalarBar->GetWidth() << '\n';

    // scalarBar->SetUseOpacity(.5);          //设置不透明度
    // scalarBar->UseOpacityOn();
    // scalarBar->SetLayerNumber(0);
    // scalarBar->SetTextureGridWidth(10);
    // scalarBar->SetDrawAnnotations(0);

    // 色卡标题
    scalarBar->SetTitle("U (m/s)");
    scalarBar->GetTitleTextProperty()->SetColor(0, 0, 0);
    scalarBar->GetTitleTextProperty()->SetFontFamilyToArial();
    scalarBar->GetTitleTextProperty()->SetFontSize(20);

    std::cout << "------------------------------------------\n";
    std::cout << "GetTitleRatio\t" << scalarBar->GetTitleRatio() << '\n';
    std::cout << "GetTitleRatioMaxValue\t" << scalarBar->GetTitleRatioMaxValue() << '\n';
    std::cout << "GetTitleRatioMinValue\t" << scalarBar->GetTitleRatioMinValue() << '\n';

    // 色卡标签
    scalarBar->UnconstrainedFontSizeOn(); // 设置标题和标签的字体大小是否不受限制。默认设置为禁用。
    scalarBar->SetNumberOfLabels(5);      // 标签个数，和颜色个数没关系，颜色个数由SetLookupTable的NumberOfColors决定
    scalarBar->SetLabelFormat("%5.3f");
    // scalarBar->GetLabelTextProperty()->SetColor(0, 0, 0);
    // scalarBar->GetLabelTextProperty()->SetFontFamilyToArial();
    // scalarBar->GetLabelTextProperty()->SetFontFamilyToCourier();
    // scalarBar->GetLabelTextProperty()->SetFontSize(20);
    std::cout << "------------------------------------------\n";

    vtkNew<vtkTextProperty> textProperty;
    textProperty->SetColor(0, 0, 0);
    textProperty->SetFontFamilyToTimes();
    scalarBar->SetLabelTextProperty(textProperty);

    //---------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor2D(scalarBar);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // 设置回调函数，用来返回色卡的位置
    auto callback = Callback::New();
    callback->SetScalarBar(scalarBar);
    callback->SetRenderer(renderer);
    renderWindowInteractor->AddObserver(vtkCommand::LeftButtonPressEvent, callback);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST203

#ifdef TEST204

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkCommand.h>
#include <vtkFloatArray.h>
#include <vtkImageMapToColors.h>
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
#include <vtkScalarBarWidget.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkViewport.h>

#include <numbers>

int main(int, char*[])
{
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetNumberOfColors(10);
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->SetRange(-1.e20, 100.0);
    hueLut->Build();

    // Double类型变量的精度是保留15-17位小数，因为Double类型的表示方式为1个符号位、11位指数位和52位精度（即尾数）位。
    // 所以双精度浮点数一共有53个二进制位。其中，最高位是符号位，0表示正数，1表示负数，接着11位是指数位，
    // 也就是可存储的数据范围，剩余的52位是精度位，也就是小数部分的数据精度。

    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLookupTable(hueLut);
    scalarBar->SetPosition(.1, .1);  // 设置位置，左下角
    scalarBar->SetPosition2(.8, .8); // 右上角，是相对最下角的位置，不是实际位置，还包括Title的大小
    // scalarBar->SetLabelFormat("%.3e");
    scalarBar->SetLabelFormat("%.3f");

    // 自定义标签
    // 标签的值需要保证在颜色映射表的Range中，且位置会根据Range自动计算
    vtkNew<vtkDoubleArray> labels;
    labels->InsertNextValue(-1.e20);
    labels->InsertNextValue(-8.e19);
    labels->InsertNextValue(-6.e19);
    labels->InsertNextValue(-4.e19);
    labels->InsertNextValue(-2.e19);
    labels->InsertNextValue(100.0);

    scalarBar->UseCustomLabelsOn();
    scalarBar->SetCustomLabels(labels);

    //---------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor2D(scalarBar);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST204

#ifdef TEST205

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkCommand.h>
#include <vtkFloatArray.h>
#include <vtkImageMapToColors.h>
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
#include <vtkScalarBarWidget.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkViewport.h>

class Callback : public vtkCommand
{
public:
    static Callback* New()
    {
        return new Callback;
    }

    virtual void Execute(vtkObject* caller, unsigned long x, void* y)
    {
        if (auto interactor = vtkRenderWindowInteractor::SafeDownCast(caller))
        {
            auto size = interactor->GetRenderWindow()->GetSize();
            std::cout << "Window size : " << size[0] << ',' << size[1] << '\t';

            std::cout << "Mouse pos : " << interactor->GetEventPosition()[0] << ',' << interactor->GetEventPosition()[1] << '\n';
        }

        if (m_bar && m_renderer)
        {
            std::cout << "----------------------\n";

            // 色卡的色带部分在渲染窗口中的像素位置
            int rect[4] { 0 };
            m_bar->GetScalarBarRect(rect, m_renderer);
            std::cout << "ScalarBarRect : " << rect[0] << '\t' << rect[1] << '\t' << rect[2] << '\t' << rect[3] << '\n';

            auto range = m_bar->GetLookupTable()->GetRange();
            std::cout << "Range: " << range[0] << '\t' << range[1] << '\n';

            // 获取指定scalar在颜色映射表中的索引
            if (auto lut = vtkLookupTable::SafeDownCast(m_bar->GetLookupTable()))
            {
                double scalar  = 10.0;
                auto index     = lut->GetIndex(scalar);
                auto numColors = lut->GetNumberOfColors();
                auto indexLu   = lut->GetIndexedLookup();
                std::cout << "indexedLookup: " << indexLu << "\tcolor num: " << numColors << "\tscalar: " << scalar << "\tindex: " << index << '\n';
            }
        }
    }

    void SetScalarBar(vtkSmartPointer<vtkScalarBarActor> bar)
    {
        m_bar = bar;
    }

    void SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
    {
        m_renderer = renderer;
    }

private:
    vtkSmartPointer<vtkScalarBarActor> m_bar { nullptr };
    vtkSmartPointer<vtkRenderer> m_renderer { nullptr };
};

int main(int, char*[])
{
    //-------------------------------------------------------------------
    // 创建颜色查找表
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetNumberOfColors(10);
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->SetRange(1., 4.);
    hueLut->Build();

    //---------------------------------------------------------
    vtkNew<vtkScalarBarActor> scalarBar;
    std::cout << "------------------------------------------\n";
    std::cout << "GetNumberOfLabels \t" << scalarBar->GetNumberOfLabels() << '\n';
    std::cout << "GetNumberOfConsumers\t" << scalarBar->GetNumberOfConsumers() << '\n';
    std::cout << "GetNumberOfLabelsMinValue\t" << scalarBar->GetNumberOfLabelsMinValue() << '\n';
    std::cout << "GetNumberOfLabelsMaxValue\t" << scalarBar->GetNumberOfLabelsMaxValue() << '\n';
    std::cout << "GetNumberOfPaths\t" << scalarBar->GetNumberOfPaths() << '\n';
    std::cout << "GetPostion\t" << scalarBar->GetPosition()[0] << '\t' << scalarBar->GetPosition()[1] << '\n';
    std::cout << "GetPostion2\t" << scalarBar->GetPosition2()[0] << '\t' << scalarBar->GetPosition2()[1] << '\n';

    scalarBar->SetLookupTable(hueLut);
    scalarBar->SetTextPad(5);        // 设置文本框周围的填充量，就是标签文本距离颜色映射表的距离
    scalarBar->SetPosition(.2, .2);  // 设置位置，左下角
    scalarBar->SetPosition2(.8, .8); // 右上角，是相对最下角的位置，不是实际位置，还包括Title的大小
    std::cout << "GetPostion\t" << scalarBar->GetPosition()[0] << '\t' << scalarBar->GetPosition()[1] << '\n';
    std::cout << "GetPostion2\t" << scalarBar->GetPosition2()[0] << '\t' << scalarBar->GetPosition2()[1] << '\n';
    std::cout << "GetBarRatio\t" << scalarBar->GetBarRatio() << '\n'; // 获取颜色条相对于小部件框架的厚度。

    std::cout << "GetPositionCoordinate\t" << scalarBar->GetPositionCoordinate()->GetValue()[0] << '\t'
              << scalarBar->GetPositionCoordinate()->GetValue()[1] << '\n';
    std::cout << "GetPosition2Coordinate\t" << scalarBar->GetPosition2Coordinate()->GetValue()[0] << '\t'
              << scalarBar->GetPosition2Coordinate()->GetValue()[1] << '\n';

    std::cout << "GetHeight\t" << scalarBar->GetHeight() << '\n';
    std::cout << "GetWidth\t" << scalarBar->GetWidth() << '\n';

    // scalarBar->SetUseOpacity(1);          //设置不透明度
    // scalarBar->UseOpacityOn();
    // scalarBar->SetLayerNumber(0);
    // scalarBar->SetTextureGridWidth(10);
    // scalarBar->SetDrawAnnotations(0);

    // 色卡标题
    scalarBar->SetTitle("U (m/s)");
    scalarBar->GetTitleTextProperty()->SetColor(0, 0, 0);
    scalarBar->GetTitleTextProperty()->SetFontFamilyToArial();
    scalarBar->GetTitleTextProperty()->SetFontSize(20);

    std::cout << "------------------------------------------\n";
    std::cout << "GetTitleRatio\t" << scalarBar->GetTitleRatio() << '\n';
    std::cout << "GetTitleRatioMaxValue\t" << scalarBar->GetTitleRatioMaxValue() << '\n';
    std::cout << "GetTitleRatioMinValue\t" << scalarBar->GetTitleRatioMinValue() << '\n';

    // 色卡标签
    scalarBar->UnconstrainedFontSizeOn(); // 设置标题和标签的字体大小是否不受限制。默认设置为禁用。
    scalarBar->SetNumberOfLabels(5);      // 标签个数，和颜色个数没关系，颜色个数由SetLookupTable的NumberOfColors决定
    scalarBar->SetLabelFormat("%5.3f");
    // scalarBar->GetLabelTextProperty()->SetColor(0, 0, 0);
    // scalarBar->GetLabelTextProperty()->SetFontFamilyToArial();
    // scalarBar->GetLabelTextProperty()->SetFontFamilyToCourier();
    // scalarBar->GetLabelTextProperty()->SetFontSize(20);
    std::cout << "------------------------------------------\n";

    // 设置色卡最大颜色分段个数，如果LookupTable大于该数，也只能显示MaximumNumberOfColors个颜色。太小会导致两头的颜色不显示，默认64
    scalarBar->SetMaximumNumberOfColors(10);

    scalarBar->SetMaximumWidthInPixels(80);
    scalarBar->SetMaximumHeightInPixels(900);

    vtkNew<vtkTextProperty> textProperty;
    textProperty->SetColor(0, 0, 0);
    textProperty->SetFontFamilyToTimes();
    scalarBar->SetLabelTextProperty(textProperty);

    //---------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // 设置回调函数，用来返回色卡的位置
    auto callback = Callback::New();
    callback->SetScalarBar(scalarBar);
    callback->SetRenderer(renderer);
    renderWindowInteractor->AddObserver(vtkCommand::LeftButtonPressEvent, callback);

    // 添加色卡小部件，可以使用鼠标拖动色卡，缩放色卡
    vtkNew<vtkScalarBarWidget> scalarBarWidget;
    scalarBarWidget->SetInteractor(renderWindowInteractor);
    scalarBarWidget->SetScalarBarActor(scalarBar);
    scalarBarWidget->On();

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    // renderer->AddActor(actor);
    //  renderer->AddActor2D(scalarBar);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST205

#ifdef TEST301

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGReader.h>
#include <vtkLogoRepresentation.h>
#include <vtkLogoWidget.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0., 1., 0.);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(600, 400);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    //------------------------------------------------------------------
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName("../resources/test1.jpg");
    jPEGReader->Update();

    // 输入图片大小
    int extent[6] {};
    jPEGReader->GetOutput()->GetExtent(extent);
    std::cout << "extent: " << extent[0] << " " << extent[1] << " " << extent[2] << " " << extent[3] << " " << extent[4] << " " << extent[5] << '\n';

    vtkNew<vtkLogoRepresentation> logoRepresentation;
    logoRepresentation->SetImage(jPEGReader->GetOutput());
    logoRepresentation->SetPosition(0., 0.);
    logoRepresentation->SetPosition2(0.2, 0.2); // 图片能否占满整个widget，需要考虑窗口的大小以及这两个坐标
    logoRepresentation->VisibilityOn();
    logoRepresentation->SetBorderColor(1., 0., 0.); // 边框颜色
    logoRepresentation->SetBorderThickness(5.f);    // 边框线条的粗细（像素）
    logoRepresentation->SetShowBorderToActive();    // 设置边框何时显示，Active表示鼠标处于内部时显示
    logoRepresentation->SetShowVerticalBorder(vtkBorderRepresentation::BORDER_ON); // 设置垂直边框何时显示
    logoRepresentation->GetImageProperty()->SetOpacity(1.);

    vtkNew<vtkLogoWidget> logoWidget;
    logoWidget->SetInteractor(renderWindowInteractor);
    logoWidget->SetRepresentation(logoRepresentation);
    logoWidget->SetParent(nullptr);
    logoWidget->SetSelectable(true);
    logoWidget->SetResizable(true);
    logoWidget->SetKeyPressActivation(true);     // 启用使用按键打开和关闭交互器
    logoWidget->SetKeyPressActivationValue('a'); // 设置打开和关闭交互器的按键
    logoWidget->SetManagesCursor(true);          // 开启光标的管理，创建复合窗口一般会禁用子窗口游标管理
    logoWidget->SetPickingManaged(true);         // 使用管理器处理拾取
    logoWidget->SetPriority(1.0);                // 设置交互优先级
    logoWidget->ProcessEventsOn();               // 启用响应交互事件
    logoWidget->On();                            // 开启交互器

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST301

#ifdef TEST401

#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>

int main(int, char*[])
{
    vtkNew<vtkAxesActor> axesActor;
    axesActor->SetShaftTypeToCylinder(); // 轴的类型：线、圆柱、用户自定义
    axesActor->SetXAxisLabelText("XX");  // 文本标签
    axesActor->SetYAxisLabelText("YY");
    axesActor->SetZAxisLabelText("ZZ");
    axesActor->SetCylinderRadius(.05); // 轴的粗细
    axesActor->SetConeRadius(.5);      // 圆锥的大小

    axesActor->GetXAxisCaptionActor2D()->GetCaptionTextProperty(); // 文本属性

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(axesActor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST401

#ifdef TEST402

#include <vtkActor.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>

int main(int, char*[])
{
    vtkNew<vtkAnnotatedCubeActor> axesActor;
    axesActor->SetXPlusFaceText("AA"); // 设置指定面的文本
    axesActor->SetFaceTextScale(0.1);  // 文本缩放比例，如果太大，文本可能会比立方体还大

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(axesActor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST402

#ifdef TEST403

#include <vtkActor.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

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
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    vtkNew<vtkCameraOrientationWidget> cow;
    cow->SetParentRenderer(renderer); // 设置操作的渲染器（相机）
    cow->AnimateOn();                 // 开启动画
    cow->On();

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST403

#ifdef TEST404

#include <vtkAxesActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> ren;
    ren->AddActor(actor);
    ren->SetBackground(.1, .2, .3);
    ren->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren);
    renWin->SetSize(800, 600);
    renWin->Render();

    vtkNew<vtkRenderWindowInteractor> iRen;
    iRen->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iRen->SetInteractorStyle(style);

    //------------------------------------------------------------------------
    vtkNew<vtkAxesActor> axes;
    vtkNew<vtkOrientationMarkerWidget> omw;
    omw->SetOrientationMarker(axes);
    omw->SetViewport(0., 0., .2, .2);              // 位置（标准化窗口坐标[0,1]）
    omw->SetShouldConstrainSize(true);             // 开启最大最小尺寸限制
    omw->SetSizeConstraintDimensionSizes(10, 300); // 设置最大和最小尺寸
    omw->SetInteractor(iRen);
    omw->SetCurrentRenderer(ren);
    omw->EnabledOn();
    omw->InteractiveOn();

    renWin->Render();
    iRen->Start();

    return EXIT_SUCCESS;
}

#endif // TEST404

#ifdef TEST501

#include <vtkActor.h>
#include <vtkButtonWidget.h>
#include <vtkCallbackCommand.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTexturedButtonRepresentation2D.h>

int main(int, char*[])
{
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1., 0., 0.);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    //--------------------------------------------------------------------------
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName("../resources/test1.jpg");
    jPEGReader->Update();

    vtkNew<vtkJPEGReader> jPEGReader2;
    jPEGReader2->SetFileName("../resources/test3.jpg");
    jPEGReader2->Update();

    double bds[6] { 680., 800., 520., 600., 0., 0. };
    vtkNew<vtkTexturedButtonRepresentation2D> buttonRepresentation;
    buttonRepresentation->SetNumberOfStates(2);                          // 状态的数量
    buttonRepresentation->SetButtonTexture(0, jPEGReader->GetOutput());  // 第0个状态的纹理
    buttonRepresentation->SetButtonTexture(1, jPEGReader2->GetOutput()); // 第1个状态的纹理
    buttonRepresentation->SetState(1);                                   // 设置当前的状态（纹理序号）
    buttonRepresentation->SetPlaceFactor(1.);                            // 设置PlaceWidget()的缩放因子，默认是0.5
    buttonRepresentation->PlaceWidget(bds);                              // 按钮的位置，使用的是屏幕坐标

    static auto lambda = [&actor]()
    {
        static int color { 0 };
        actor->GetProperty()->SetColor(static_cast<double>(color++ % 2), 0., 0.);
    };

    // 当按钮状态改变时，会执行该回调函数
    // 在按钮位置按下鼠标左键，在任意位置松开左键后，就会改变状态
    vtkNew<vtkCallbackCommand> callback;
    callback->SetCallback(
        [](vtkObject* caller, unsigned long, void*, void*)
        {
            if (auto btn = vtkButtonWidget::SafeDownCast(caller))
            {
                if (auto representation = vtkTexturedButtonRepresentation2D::SafeDownCast(btn->GetRepresentation()))
                {
                    std::cout << "Current state: " << representation->GetState() << '\n';
                    lambda();
                }
            }
        });

    vtkNew<vtkButtonWidget> buttonWidget;
    buttonWidget->AddObserver(vtkCommand::StateChangedEvent, callback);
    buttonWidget->SetInteractor(renderWindowInteractor);
    buttonWidget->SetRepresentation(buttonRepresentation);
    buttonWidget->On();

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST501

#ifdef TEST601

#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
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
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);
    interactor->SetInteractorStyle(style);

    //------------------------------------------------------------------------
    vtkNew<vtkBorderWidget> borderWidget;
    borderWidget->SetInteractor(interactor);
    borderWidget->GetBorderRepresentation(); // 返回空，所以必须获取 Representation 再转换
    borderWidget->SelectableOn(); // 开启可以选择小部件内部区域，否则允许使用鼠标移动小部件，开启时会执行SelectRegion()方法
    borderWidget->ResizableOn(); // 开启使用鼠标控制缩放边框
    borderWidget->On();

    auto representation = vtkBorderRepresentation::SafeDownCast(borderWidget->GetRepresentation());
    representation->SetBorderColor(1, 0, 0); // 边框颜色
    representation->SetBorderThickness(5.f); // 边框线宽
    representation->SetPosition(.2, .2);     // 左下角的位置，范围为：[0,1]
    representation->SetPosition2(.2, .2);    // 宽度和高度，注意不是右上角的位置

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST601
