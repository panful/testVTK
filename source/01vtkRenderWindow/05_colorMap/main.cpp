/*
* 100 云图，渐变线，点和单元设置流场参数，颜色过渡方式，颜色映射表，色卡
* 101 vtkColorLegend  https://vtk.org/doc/nightly/html/classvtkColorLegend.html
*
* 201 云图 stl文件
* 202 vtkStreamTracer 流线图 展示流体流动的轨迹和流动方向 有点像线框式的云图 vtkOpenFOAMReader
* 203 vtkContourFilter  等值面
*
* 300 vtkGlyph3D 官方示例 矢量图
* 301 vtkGlyph2D 官方示例 vtkGlyph2D继承自vtkGlyph3D
* 302 vtkGlyph3D 常用函数，色卡
* 303 vtkGlyph3DMapper 官方示例 vtkGlyph3D在GPU上计算的版本
* 304 vtkGlyph3DMapper 使用方法
* 305 从vtkGlyph3D获取源数据 从经过vtkAlgorithm变换后的数据获取源数据
* 306 矢量图标签大小的设置
*

* 8 矢量图颜色设置以及恢复
* 21 官方例子，球面法向量  矢量图
* 10 2D直线 vtkGlyph3D 矢量图箭头方向，箭头起始段末端翻转
* 40 vtkCellDataToPointData 单元标量数据转顶点数据，等值线vtkContourFilter
* 42 vtkCellCenters 获取单元中心即格心，并用球体标注格心(矢量图）
*/


#define TEST307

#ifdef TEST100

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCellData.h>
#include <vtkScalarBarActor.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkImageMapToColors.h>
#include <vtkViewport.h>
#include <vtkCommand.h>
#include <vtkScalarBarWidget.h>

class Callback :
    public vtkCommand
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
            int rect[4]{ 0 };
            m_bar->GetScalarBarRect(rect, m_renderer);
            std::cout << "ScalarBarRect : " << rect[0] << '\t' << rect[1] << '\t' << rect[2] << '\t' << rect[3] << '\n';

            auto range = m_bar->GetLookupTable()->GetRange();
            std::cout << "Range: " << range[0] << '\t' << range[1] << '\n';

            // 获取指定scalar在颜色映射表中的索引
            if (auto lut = vtkLookupTable::SafeDownCast(m_bar->GetLookupTable()))
            {
                double scalar = 10.0;
                auto index = lut->GetIndex(scalar);
                auto numColors = lut->GetNumberOfColors();
                auto indexLu = lut->GetIndexedLookup();
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
    vtkSmartPointer<vtkScalarBarActor> m_bar{ nullptr };
    vtkSmartPointer<vtkRenderer> m_renderer{ nullptr };
};

#define line_fill  // 上下左右四视图，格点格心数据，线框面显示模式的区别
//#define scalar_bar    // 色卡颜色查找表的使用


int main(int, char* [])
{

#ifdef line_fill

    // 顶点，共有40个顶点
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (size_t i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i + 0, 0, 0);
        points->InsertNextPoint(i + 1, 1, 0);
        points->InsertNextPoint(i + 0, 2, 0);
        points->InsertNextPoint(i + 1, 3, 0);
    }

    // 多边形拓扑，共有27个四边形
    vtkSmartPointer<vtkCellArray> cell_poly = vtkSmartPointer<vtkCellArray>::New();
    for (long long i = 0; i < 33; i += 4)
    {
        cell_poly->InsertNextCell({ i + 0,i + 1,i + 5,i + 4 });
        cell_poly->InsertNextCell({ i + 1,i + 2,i + 6,i + 5 });
        cell_poly->InsertNextCell({ i + 2,i + 3,i + 7,i + 6 });
    }

    // 线段拓扑，共有66条线段
    vtkSmartPointer<vtkCellArray> cell_line = vtkSmartPointer<vtkCellArray>::New();
    for (long long i = 0; i < 37; i += 3)
    {
        cell_line->InsertNextCell({ i + 0, i + 1 });
        cell_line->InsertNextCell({ i + 1, i + 2 });
        cell_line->InsertNextCell({ i + 2, i + 3 });
    }
    for (long long i = 0; i < 33; i += 4)
    {
        cell_line->InsertNextCell({ i + 0,i + 4 });
        cell_line->InsertNextCell({ i + 1,i + 5 });
        cell_line->InsertNextCell({ i + 2,i + 6 });
        cell_line->InsertNextCell({ i + 3,i + 7 });
    }

    const int numPoints = 40;
    const int numPolys = 27;
    const int numLines = 66;

    // 为每个顶点设置标量属性
    vtkNew<vtkFloatArray> scalar_points;
    scalar_points->SetNumberOfValues(numPoints);
    for (int i = 0; i < numPoints - 3; i += 4)
    {
        // 为属性数组中的每个元素设置标量值（这个标量值可以当作颜色值）
        scalar_points->SetValue(i + 0, i);
        scalar_points->SetValue(i + 1, i);
        scalar_points->SetValue(i + 2, i);
        scalar_points->SetValue(i + 3, i);
    }

    // 为每个多边形设置标量属性
    vtkNew<vtkFloatArray> scalar_cell_poly;
    scalar_cell_poly->SetNumberOfValues(numPolys);
    for (int i = 0; i < numPolys; i += 3)
    {
        scalar_cell_poly->SetValue(i + 0, i);
        scalar_cell_poly->SetValue(i + 1, i);
        scalar_cell_poly->SetValue(i + 2, i);
    }

    // 为每条线段设置标量属性
    vtkNew<vtkFloatArray> scalar_cell_line;
    scalar_cell_line->SetNumberOfValues(numLines);
    for (long long i = 0; i < 37; i += 3)
    {
        scalar_cell_line->SetValue(i + 0, i);
        scalar_cell_line->SetValue(i + 1, i);
        scalar_cell_line->SetValue(i + 2, i);
    }
    for (long long i = 30; i < 63; i += 4)
    {
        scalar_cell_line->SetValue(i + 0, i - 30);
        scalar_cell_line->SetValue(i + 1, i - 30);
        scalar_cell_line->SetValue(i + 2, i - 30);
        scalar_cell_line->SetValue(i + 3, i - 30);
    }

    // 多边形，格点数据
    vtkNew<vtkPolyData> polyData_poly_scalarPoint;
    polyData_poly_scalarPoint->SetPoints(points);
    polyData_poly_scalarPoint->SetPolys(cell_poly);
    polyData_poly_scalarPoint->GetPointData()->SetScalars(scalar_points);

    // 线段，格点数据
    vtkNew<vtkPolyData> polyData_line_scalarPoint;
    polyData_line_scalarPoint->SetPoints(points);
    polyData_line_scalarPoint->SetLines(cell_line);
    polyData_line_scalarPoint->GetPointData()->SetScalars(scalar_points);

    // 多边形，格心数据
    vtkNew<vtkPolyData> polyData_poly_scalarCell;
    polyData_poly_scalarCell->SetPoints(points);
    polyData_poly_scalarCell->SetPolys(cell_poly);
    polyData_poly_scalarCell->GetCellData()->SetScalars(scalar_cell_poly);

    // 线段，格心数据
    vtkNew<vtkPolyData> polyData_line_scalarCell;
    polyData_line_scalarCell->SetPoints(points);
    polyData_line_scalarCell->SetLines(cell_line);
    polyData_line_scalarCell->GetPointData()->SetScalars(scalar_cell_line);

    // 创建颜色查找表
    vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->SetRampToSQRT();
    hueLut->Build();

    // mapper
    vtkNew<vtkPolyDataMapper> mapperPoly_scalarCell;
    mapperPoly_scalarCell->SetInputData(polyData_poly_scalarCell);
    mapperPoly_scalarCell->SetLookupTable(hueLut);
    mapperPoly_scalarCell->SetScalarRange(scalar_cell_poly->GetRange());

    vtkNew<vtkPolyDataMapper> mapperLine_scalarCell;
    mapperLine_scalarCell->SetInputData(polyData_line_scalarCell);
    mapperLine_scalarCell->SetLookupTable(hueLut);
    mapperLine_scalarCell->SetScalarRange(scalar_cell_line->GetRange());

    vtkNew<vtkPolyDataMapper> mapperPoly_scalarPoint;
    mapperPoly_scalarPoint->SetInputData(polyData_poly_scalarPoint);
    mapperPoly_scalarPoint->SetLookupTable(hueLut);
    mapperPoly_scalarPoint->SetScalarRange(scalar_points->GetRange());

    vtkNew<vtkPolyDataMapper> mapperLine_scalarPoint;
    mapperLine_scalarPoint->SetInputData(polyData_line_scalarPoint);
    mapperLine_scalarPoint->SetLookupTable(hueLut);
    mapperLine_scalarPoint->SetScalarRange(scalar_points->GetRange());

    vtkNew<vtkActor> actorPolyPoint;
    vtkNew<vtkActor> actorPolyCell;
    vtkNew<vtkActor> actorLinePoint;
    vtkNew<vtkActor> actorLineCell;
    actorPolyPoint->SetMapper(mapperPoly_scalarPoint);
    actorPolyCell->SetMapper(mapperPoly_scalarCell);
    actorLinePoint->SetMapper(mapperLine_scalarPoint);
    actorLineCell->SetMapper(mapperLine_scalarCell);

    // 以点，线框，面方式显示
    actorPolyPoint->GetProperty()->SetRepresentationToWireframe();
    //actorPolyPoint->GetProperty()->SetRepresentationToSurface();
    //actorPolyPoint->GetProperty()->SetRepresentationToPoints();
    //actorPolyCell->GetProperty()->SetRepresentationToWireframe();
    actorPolyPoint->GetProperty()->EdgeVisibilityOn();
    actorPolyPoint->GetProperty()->SetEdgeColor(1, 1, 1);

    // 设置云图的颜色
    mapperPoly_scalarCell->ScalarVisibilityOff();
    actorPolyCell->GetProperty()->SetColor(1, 1, 0);
    mapperPoly_scalarCell->ScalarVisibilityOn();

    vtkNew<vtkRenderer> topLeftRenderer;
    vtkNew<vtkRenderer> topRightRenderer;
    vtkNew<vtkRenderer> bottomLeftRenderer;
    vtkNew<vtkRenderer> bottomRightRenderer;
    topLeftRenderer->AddActor(actorPolyPoint);
    topRightRenderer->AddActor(actorPolyCell);
    bottomLeftRenderer->AddActor(actorLinePoint);
    bottomRightRenderer->AddActor(actorLineCell);

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(topLeftRenderer);
    topLeftRenderer->SetViewport(0, 0.5, 0.5, 1);
    window->AddRenderer(topRightRenderer);
    topRightRenderer->SetViewport(0.5, 0.5, 1, 1);
    window->AddRenderer(bottomLeftRenderer);
    bottomLeftRenderer->SetViewport(0, 0, 0.5, 0.5);
    window->AddRenderer(bottomRightRenderer);
    bottomRightRenderer->SetViewport(0.5, 0, 1, 0.5);


    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(window);
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    window->SetSize(800, 600);
    window->Render();
    renderWindowInteractor->Start();

#endif // line_fill

#ifdef scalar_bar

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

    auto GenPolyData = [vertices, indicesPoly, fields]()
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cellsLine;
        vtkNew<vtkCellArray> cellsPoly;

        for (size_t i = 0; i < vertices.size(); i += 2)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], 0.0);
        }

        for (size_t i = 0; i < indicesPoly.size(); i += 4)
        {
            cellsPoly->InsertNextCell({ indicesPoly[i],indicesPoly[i + 1], indicesPoly[i + 2],indicesPoly[i + 3] });
        }
        polyData->SetPoints(points);
        polyData->SetPolys(cellsPoly);

        vtkNew<vtkFloatArray> scalars;
        for (size_t i = 0; i < 16; ++i)
        {
            scalars->InsertNextValue(fields[i]);
        }

        polyData->GetPointData()->SetScalars(scalars);

        return polyData;
    };

    //-------------------------------------------------------------------

    // 创建颜色查找表
    vtkNew<vtkLookupTable> hueLut;
    std::cout << "------------------------------------------\n";
    std::cout << "GetNumberOfColors\t" << hueLut->GetNumberOfColors() << '\n';
    std::cout << "GetNumberOfTableValues\t" << hueLut->GetNumberOfTableValues() << '\n';
    std::cout << "GetNumberOfColorsMinValue\t" << hueLut->GetNumberOfColorsMinValue() << '\n';
    std::cout << "GetNumberOfColorsMaxValue\t" << hueLut->GetNumberOfColorsMaxValue() << '\n';
    std::cout << "GetNumberOfAnnotatedValues\t" << hueLut->GetNumberOfAnnotatedValues() << '\n';
    std::cout << "GetNumberOfAvailableColors\t" << hueLut->GetNumberOfAvailableColors() << '\n';

    hueLut->SetNumberOfColors(10);          // 指定颜色查找表中有多少种颜色，默认256
    hueLut->SetHueRange(0.67, 0.0);         // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    //hueLut->SetRampToSQRT();
    //hueLut->UseAboveRangeColorOn();
    //hueLut->SetAboveRangeColor(0.0, 1.0, 0.0, 0.0);
    //hueLut->SetBelowRangeColor(1.0, 1.0, 0.0, 0.0);
    //hueLut->SetNumberOfTableValues(20);
    //hueLut->SetAlphaRange(1.0, 1.0);
    //hueLut->SetValueRange(1, 1);
    //hueLut->SetSaturationRange(1, 1);
    hueLut->SetRange(GenPolyData()->GetPointData()->GetScalars()->GetRange());
    hueLut->Build();

    //---------------------------------------------------------

    vtkNew<vtkPolyDataMapper> mapper;
    std::cout << "------------------------------------------\n";
    std::cout << "GetScalarVisibility : " << mapper->GetScalarVisibility() << '\n';
    std::cout << "GetColorModeAsString : " << mapper->GetColorModeAsString() << '\n';
    std::cout << "GetScalarModeAsString" << mapper->GetScalarModeAsString() << '\n';

    //mapper->SeamlessUOn();
    //mapper->SeamlessVOn();
    mapper->SetInputData(GenPolyData());
    mapper->SetScalarRange(mapper->GetInput()->GetPointData()->GetScalars()->GetRange());            // 设置标量值的范围
    mapper->ScalarVisibilityOn(); // 默认开启标量属性显示

    mapper->SetLookupTable(hueLut);
    //mapper->InterpolateScalarsBeforeMappingOn();  // 设置颜色映射模式为梯度平滑，类似纹理贴图
    mapper->InterpolateScalarsBeforeMappingOff();   // 颜色平滑过度

    //mapper->SetColorMode(1);
    //mapper->SetColorModeToMapScalars();        // 无论变量数据是何种类型，该方法都通过查询表对标量数据进行映射
    //mapper->SetColorModeToDefault();           // 默认的映射器行为，即把unsigned char类型的标量属性数据当作颜色值，不执行隐式。对于其他类型的标量数据，将通过查询表映射。
    //mapper->SetColorModeToDirectScalars();

    //mapper->SetScalarModeToUseCellData();
    //mapper->SetScalarModeToUseCellFieldData();
    //mapper->SetScalarModeToUsePointFieldData();
    //mapper->SetScalarModeToUseFieldData();
    //mapper->SetScalarModeToUsePointData();

    //---------------------------------------------------------

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    std::cout << "------------------------------------------\n";
    std::cout << "Interpolation\t" << actor->GetProperty()->GetInterpolationAsString() << '\n';
    actor->SetMapper(mapper);

    // 设置着色方法
    //actor->GetProperty()->SetInterpolation(0);
    //actor->GetProperty()->SetInterpolationToFlat();
    //actor->GetProperty()->SetInterpolationToPBR();
    //actor->GetProperty()->SetInterpolationToGouraud();
    actor->GetProperty()->SetInterpolationToPhong();

    //---------------------------------------------------------
    vtkNew<vtkScalarBarActor> scalarBar;
    std::cout << "------------------------------------------\n";
    std::cout << "GetNumberOfLabels \t" << scalarBar->GetNumberOfLabels() << '\n';
    std::cout << "GetNumberOfConsumers\t" << scalarBar->GetNumberOfConsumers() << '\n';
    std::cout << "GetNumberOfLabelsMinValue\t" << scalarBar->GetNumberOfLabelsMinValue() << '\n';
    std::cout << "GetNumberOfLabelsMaxValue\t" << scalarBar->GetNumberOfLabelsMaxValue() << '\n';
    std::cout << "GetNumberOfPaths\t" << scalarBar->GetNumberOfPaths() << '\n';
    std::cout << "GetNumberOfColors\t" << vtkLookupTable::SafeDownCast(mapper->GetLookupTable())->GetNumberOfColors() << '\n';
    std::cout << "GetPostion\t" << scalarBar->GetPosition()[0] << '\t' << scalarBar->GetPosition()[1] << '\n';
    std::cout << "GetPostion2\t" << scalarBar->GetPosition2()[0] << '\t' << scalarBar->GetPosition2()[1] << '\n';

    scalarBar->SetLookupTable(mapper->GetLookupTable());
    scalarBar->SetTextPad(5);           //设置文本框周围的填充量，就是标签文本距离颜色映射表的距离
    scalarBar->SetPosition(.2, .2);     //设置位置，左下角
    scalarBar->SetPosition2(.8, .8);    //右上角，是相对最下角的位置，不是实际位置，还包括Title的大小
    std::cout << "GetPostion\t" << scalarBar->GetPosition()[0] << '\t' << scalarBar->GetPosition()[1] << '\n';
    std::cout << "GetPostion2\t" << scalarBar->GetPosition2()[0] << '\t' << scalarBar->GetPosition2()[1] << '\n';
    std::cout << "GetBarRatio\t" << scalarBar->GetBarRatio() << '\n'; // 获取颜色条相对于小部件框架的厚度。

    std::cout << "GetPositionCoordinate\t" << scalarBar->GetPositionCoordinate()->GetValue()[0] << '\t' << scalarBar->GetPositionCoordinate()->GetValue()[1] << '\n';
    std::cout << "GetPosition2Coordinate\t" << scalarBar->GetPosition2Coordinate()->GetValue()[0] << '\t' << scalarBar->GetPosition2Coordinate()->GetValue()[1] << '\n';

    std::cout << "GetHeight\t" << scalarBar->GetHeight() << '\n';
    std::cout << "GetWidth\t" << scalarBar->GetWidth() << '\n';

    //scalarBar->SetUseOpacity(1);          //设置不透明度
    //scalarBar->UseOpacityOn();
    //scalarBar->SetLayerNumber(0);
    //scalarBar->SetTextureGridWidth(10);
    //scalarBar->SetDrawAnnotations(0);

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
    //scalarBar->GetLabelTextProperty()->SetColor(0, 0, 0);
    //scalarBar->GetLabelTextProperty()->SetFontFamilyToArial();
    //scalarBar->GetLabelTextProperty()->SetFontFamilyToCourier();
    //scalarBar->GetLabelTextProperty()->SetFontSize(20);
    std::cout << "------------------------------------------\n";


    //设置色卡最大颜色分段个数，如果LookupTable大于该数，也只能显示MaximumNumberOfColors个颜色。太小会导致两头的颜色不显示，默认64
    scalarBar->SetMaximumNumberOfColors(10);

    scalarBar->SetMaximumWidthInPixels(80);
    scalarBar->SetMaximumHeightInPixels(900);


    vtkNew<vtkTextProperty> textProperty;
    textProperty->SetColor(0, 0, 0);
    textProperty->SetFontFamilyToTimes();
    scalarBar->SetLabelTextProperty(textProperty);

    //scalarBar->VisibilityOff(); // 不显示色卡


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

    renderer->AddActor(actor);
    //renderer->AddActor2D(scalarBar);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

#endif scalar_bar

    return 0;
}

#endif // TEST100

#ifdef TEST101

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkElevationFilter.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkMaskPoints.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricTorus.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkReverseSense.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkSuperquadricSource.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkVariant.h>
#include <vtkVariantArray.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 90020210809ULL
#define HAS_COW
#include <vtkCameraOrientationWidget.h>
#endif

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace {
    //! Generate elevations over the surface.
    /*!
    @param src - the vtkPolyData source.
    @return elev - the elevations.
    */
    vtkSmartPointer<vtkPolyData> GetElevations(vtkPolyData* src);

    vtkSmartPointer<vtkPolyData> GetHills();
    vtkSmartPointer<vtkPolyData> GetParametricHills();
    vtkSmartPointer<vtkPolyData> GetParametricTorus();
    vtkSmartPointer<vtkPolyData> GetPlane();
    vtkSmartPointer<vtkPolyData> GetSphere();
    vtkSmartPointer<vtkPolyData> GetTorus();
    vtkSmartPointer<vtkPolyData> GetSource(std::string const& source);

    vtkSmartPointer<vtkColorSeries> GetColorSeries();

    vtkSmartPointer<vtkLookupTable> GetCategoricalLUT();

    vtkSmartPointer<vtkLookupTable> GetOrdinaLUT();

    vtkSmartPointer<vtkLookupTable> GetDivergingLUT();

    vtkSmartPointer<vtkLookupTable> ReverseLUT(vtkLookupTable* lut);

    //!  Glyph the normals on the surface.
    /*!
    @param src - the vtkPolyData source.
    #param scaleFactor - the scale factor for the glyphs.
    @param reverseNormals - if True the normals on the surface are reversed.
    @return The glyphs.
    */
    vtkNew<vtkGlyph3D> GetGlyphs(vtkPolyData* src, double const& scaleFactor = 1.0,
        bool const& reverseNormals = false);

    std::map<int, std::vector<double>> GetBands(double const dR[2],
        int const& numberOfBands,
        int const& precision = 2,
        bool const& nearestInteger = false);

    //! Divide a range into custom bands
    /*!
    You need to specify each band as an array [r1, r2] where r1 < r2 and
    append these to a vector.
    The vector should ultimately look
    like this: [[r1, r2], [r2, r3], [r3, r4]...]

    @param dR - [min, max] the range that is to be covered by the bands.
    @param numberOfBands - the number of bands, a positive integer.
    @param myBands - the bands.
    @return  A map consisting of the band inxex and [min, midpoint, max] for each
    band.
    */
    std::map<int, std::vector<double>>
        GetCustomBands(double const dR[2], int const& numberOfBands,
            std::vector<std::array<double, 2>> const& myBands);

    //! Divide a range into integral bands
    /*!
    Divide a range into bands
    @param dR - [min, max] the range that is to be covered by the bands.
    @returnA map consisting of the band inxex and [min, midpoint, max] for each
    band.
    */
    std::map<int, std::vector<double>> GetIntegralBands(double const dR[2]);

    //! Count the number of scalars in each band.
    /*
     * The scalars used are the active scalars in the polydata.
     *
     * @param bands - the bands.
     * @param src - the vtkPolyData source.
     * @return The frequencies of the scalars in each band.
     */
    std::map<int, int> GetFrequencies(std::map<int, std::vector<double>>& bands,
        vtkPolyData* src);
    //!
    /*
     * The bands and frequencies are adjusted so that the first and last
     *  frequencies in the range are non-zero.
     * @param bands: The bands.
     * @param freq: The frequencies.
     */
    void AdjustFrequencyRanges(std::map<int, std::vector<double>>& bands,
        std::map<int, int>& freq);

    void PrintBandsFrequencies(std::map<int, std::vector<double>> const& bands,
        std::map<int, int>& freq, int const& precision = 2);

} // namespace

int main(int, char* [])
{
    // Get the surface
    std::string desiredSurface{ "RandomHills" };
    // desiredSurface = "Hills";
    // desiredSurface = "ParametricTorus";
    // desiredSurface = "Plane";
    // desiredSurface = "RandomHills";
    // desiredSurface = "Sphere";
    // desiredSurface = "Torus";
    auto source = GetSource(desiredSurface);

    // The length of the normal arrow glyphs.
    auto scaleFactor = 1.0;
    if (desiredSurface == "Hills")
    {
        scaleFactor = 0.5;
    }
    if (desiredSurface == "Sphere")
    {
        scaleFactor = 2.0;
    }
    std::cout << desiredSurface << std::endl;

    source->GetPointData()->SetActiveScalars("Elevation");
    auto scalarRange =
        source->GetPointData()->GetScalars("Elevation")->GetRange();

    auto lut = GetCategoricalLUT();
    auto lut1 = GetOrdinaLUT();
    lut->SetTableRange(scalarRange);
    lut1->SetTableRange(scalarRange);
    vtkIdType numberOfBands = lut->GetNumberOfTableValues();
    auto precision = 10;
    auto bands = GetBands(scalarRange, numberOfBands, precision, false);

    if (desiredSurface == "RandomHills")
    {
        // These are my custom bands.
        // Generated by first running:
        // bands = GetBands(scalarRange, numberOfBands, precision, false);
        // then:
        //  std::vector<int> freq = Frequencies(bands, source);
        //  PrintBandsFrequencies(bands, freq);
        // Finally using the output to create this table:
        std::vector<std::array<double, 2>> myBands = {
            {0, 1.0},   {1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0},
            {4.0, 5.0}, {5.0, 6.0}, {6.0, 7.0}, {7.0, 8.0} };
        // Comment this out if you want to see how allocating
        // equally spaced bands works.
        bands = GetCustomBands(scalarRange, numberOfBands, myBands);
        // bands = GetBands(scalarRange, numberOfBands, precision, false);
        // Adjust the number of table values
    }
    lut->SetNumberOfTableValues(static_cast<vtkIdType>(bands.size()));
    lut1->SetNumberOfTableValues(static_cast<vtkIdType>(bands.size()));

    // Let's do a frequency table.
    auto freq = GetFrequencies(bands, source);
    AdjustFrequencyRanges(bands, freq);
    PrintBandsFrequencies(bands, freq);

    scalarRange[0] = bands.begin()->second[0];
    scalarRange[1] = std::prev(bands.end())->second[2];
    lut->SetTableRange(scalarRange);
    lut->SetNumberOfTableValues(bands.size());

    // We will use the midpoint of the band as the label.
    std::vector<std::string> labels;
    for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
        p != bands.end(); ++p)
    {
        std::ostringstream os;
        os << std::fixed << std::setw(6) << std::setprecision(2) << p->second[1];
        labels.push_back(os.str());
    }

    // Annotate
    vtkNew<vtkVariantArray> values;
    for (size_t i = 0; i < labels.size(); ++i)
    {
        values->InsertNextValue(vtkVariant(labels[i]));
    }
    for (vtkIdType i = 0; i < values->GetNumberOfTuples(); ++i)
    {
        lut->SetAnnotation(i, values->GetValue(i).ToString());
    }

    // Create a lookup table with the colors reversed.
    auto lutr = ReverseLUT(lut);

    // Create the contour bands.
    vtkNew<vtkBandedPolyDataContourFilter> bcf;
    bcf->SetInputData(source);
    // Use either the minimum or maximum value for each band.
    int i = 0;
    for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
        p != bands.end(); ++p)
    {
        bcf->SetValue(i, p->second[2]);
        ++i;
    }
    // We will use an indexed lookup table.
    bcf->SetScalarModeToIndex();
    bcf->GenerateContourEdgesOn();

    // Generate the glyphs on the original surface.
    auto glyph = GetGlyphs(source, scaleFactor, false);

    // ------------------------------------------------------------
    // Create the mappers and actors
    // ------------------------------------------------------------
    vtkNew<vtkNamedColors> colors;

    // Set the background color.
    colors->SetColor("BkgColor",
        std::array<unsigned char, 4>{179, 204, 255, 255}.data());
    colors->SetColor("ParaViewBkg",
        std::array<unsigned char, 4>{82, 87, 110, 255}.data());

    vtkNew<vtkPolyDataMapper> srcMapper;
    srcMapper->SetInputConnection(bcf->GetOutputPort());
    srcMapper->SetScalarRange(scalarRange);
    srcMapper->SetLookupTable(lut);
    srcMapper->SetScalarModeToUseCellData();

    vtkNew<vtkActor> srcActor;
    srcActor->SetMapper(srcMapper);

    // Create contour edges
    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputData(bcf->GetContourEdgesOutput());
    edgeMapper->SetResolveCoincidentTopologyToPolygonOffset();

    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

    vtkNew<vtkPolyDataMapper> glyphMapper;
    glyphMapper->SetInputConnection(glyph->GetOutputPort());
    glyphMapper->SetScalarModeToUsePointFieldData();
    glyphMapper->SetColorModeToMapScalars();
    glyphMapper->ScalarVisibilityOn();
    glyphMapper->SelectColorArray("Elevation");
    // Colour by scalars.
    glyphMapper->SetLookupTable(lut1);
    glyphMapper->SetScalarRange(scalarRange);

    vtkNew<vtkActor> glyphActor;
    glyphActor->SetMapper(glyphMapper);

    auto windowWidth = 800;
    auto windowHeight = 800;

    // Add scalar bars.
    vtkNew<vtkScalarBarActor> scalarBar;
    // This LUT puts the lowest value at the top of the scalar bar.
    // scalarBar->SetLookupTable(lut);
    // Use this LUT if you want the highest value at the top.
    scalarBar->SetLookupTable(lutr);
    scalarBar->SetTitle("Elevation");
    scalarBar->GetTitleTextProperty()->SetColor(
        colors->GetColor3d("AliceBlue").GetData());
    scalarBar->GetLabelTextProperty()->SetColor(
        colors->GetColor3d("AliceBlue").GetData());
    scalarBar->GetAnnotationTextProperty()->SetColor(
        colors->GetColor3d("AliceBlue").GetData());
    scalarBar->UnconstrainedFontSizeOn();
    scalarBar->SetMaximumWidthInPixels(windowWidth / 8);
    scalarBar->SetMaximumHeightInPixels(windowHeight / 3);
    scalarBar->SetPosition(0.85, 0.05);

    // ------------------------------------------------------------
    // Create the RenderWindow, Renderer and Interactor
    // ------------------------------------------------------------
    vtkNew<vtkRenderer> ren;
    vtkNew<vtkRenderWindow> renWin;
    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->AddRenderer(ren);
    // Important: The interactor must be set prior to enabling the widget.
    iren->SetRenderWindow(renWin);
#ifdef HAS_COW
    vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
    camOrientManipulator->SetParentRenderer(ren);
    // Enable the widget.
    camOrientManipulator->On();
#endif

    // add actors
    ren->AddViewProp(srcActor);
    ren->AddViewProp(edgeActor);
    ren->AddViewProp(glyphActor);
    ren->AddActor2D(scalarBar);

    ren->SetBackground(colors->GetColor3d("ParaViewBkg").GetData());
    renWin->SetSize(windowWidth, windowHeight);
    renWin->SetWindowName("ElevationBandsWithGlyphs");

    if (desiredSurface == "RandomHills")
    {
        auto camera = ren->GetActiveCamera();
        camera->SetPosition(10.9299, 59.1505, 24.9823);
        camera->SetFocalPoint(2.21692, 7.97545, 7.75135);
        camera->SetViewUp(-0.230136, 0.345504, -0.909761);
        camera->SetDistance(54.6966);
        camera->SetClippingRange(36.3006, 77.9852);
        renWin->Render();
    }

    iren->Start();

    return EXIT_SUCCESS;
}

namespace {
    vtkSmartPointer<vtkPolyData> GetElevations(vtkPolyData* src)
    {
        double bounds[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
        src->GetBounds(bounds);
        if (std::abs(bounds[2]) < 1.0e-8 && std::abs(bounds[3]) < 1.0e-8)
        {
            bounds[3] = bounds[2] + 1;
        }
        vtkNew<vtkElevationFilter> elevFilter;
        elevFilter->SetInputData(src);
        elevFilter->SetLowPoint(0, bounds[2], 0);
        elevFilter->SetHighPoint(0, bounds[3], 0);
        elevFilter->SetScalarRange(bounds[2], bounds[3]);
        elevFilter->Update();

        return elevFilter->GetPolyDataOutput();
    }

    vtkSmartPointer<vtkPolyData> GetHills()
    {
        // Create four hills on a plane.
        // This will have regions of negative, zero and positive Gsaussian curvatures.

        auto xRes = 50;
        auto yRes = 50;
        auto xMin = -5.0;
        auto xMax = 5.0;
        auto dx = (xMax - xMin) / (xRes - 1.0);
        auto yMin = -5.0;
        auto yMax = 5.0;
        auto dy = (yMax - yMin) / (xRes - 1.0);

        // Make a grid.
        vtkNew<vtkPoints> points;
        for (auto i = 0; i < xRes; ++i)
        {
            auto x = xMin + i * dx;
            for (auto j = 0; j < yRes; ++j)
            {
                auto y = yMin + j * dy;
                points->InsertNextPoint(x, y, 0);
            }
        }

        // Add the grid points to a polydata object.
        vtkNew<vtkPolyData> plane;
        plane->SetPoints(points);

        // Triangulate the grid.
        vtkNew<vtkDelaunay2D> delaunay;
        delaunay->SetInputData(plane);
        delaunay->Update();

        auto polydata = delaunay->GetOutput();

        vtkNew<vtkDoubleArray> elevation;
        elevation->SetNumberOfTuples(points->GetNumberOfPoints());

        //  We define the parameters for the hills here.
        // [[0: x0, 1: y0, 2: x variance, 3: y variance, 4: amplitude]...]
        std::vector<std::array<double, 5>> hd{ {-2.5, -2.5, 2.5, 6.5, 3.5},
                                              {2.5, 2.5, 2.5, 2.5, 2},
                                              {5.0, -2.5, 1.5, 1.5, 2.5},
                                              {-5.0, 5, 2.5, 3.0, 3} };
        std::array<double, 2> xx{ 0.0, 0.0 };
        for (auto i = 0; i < points->GetNumberOfPoints(); ++i)
        {
            auto x = polydata->GetPoint(i);
            for (size_t j = 0; j < hd.size(); ++j)
            {
                xx[0] = std::pow(x[0] - hd[j][0] / hd[j][2], 2.0);
                xx[1] = std::pow(x[1] - hd[j][1] / hd[j][3], 2.0);
                x[2] += hd[j][4] * std::exp(-(xx[0] + xx[1]) / 2.0);
            }
            polydata->GetPoints()->SetPoint(i, x);
            elevation->SetValue(i, x[2]);
        }

        vtkNew<vtkFloatArray> textures;
        textures->SetNumberOfComponents(2);
        textures->SetNumberOfTuples(2 * polydata->GetNumberOfPoints());
        textures->SetName("Textures");

        for (auto i = 0; i < xRes; ++i)
        {
            float tc[2];
            tc[0] = i / (xRes - 1.0);
            for (auto j = 0; j < yRes; ++j)
            {
                // tc[1] = 1.0 - j / (yRes - 1.0);
                tc[1] = j / (yRes - 1.0);
                textures->SetTuple(static_cast<vtkIdType>(i) * yRes + j, tc);
            }
        }

        polydata->GetPointData()->SetScalars(elevation);
        polydata->GetPointData()->GetScalars()->SetName("Elevation");
        polydata->GetPointData()->SetTCoords(textures);

        vtkNew<vtkPolyDataNormals> normals;
        normals->SetInputData(polydata);
        normals->SetInputData(polydata);
        normals->SetFeatureAngle(30);
        normals->SplittingOff();

        vtkNew<vtkTransform> tr1;
        tr1->RotateX(-90);

        vtkNew<vtkTransformPolyDataFilter> tf1;
        tf1->SetInputConnection(normals->GetOutputPort());
        tf1->SetTransform(tr1);
        tf1->Update();

        return tf1->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetParametricHills()
    {
        vtkNew<vtkParametricRandomHills> fn;
        fn->AllowRandomGenerationOn();
        fn->SetRandomSeed(1);
        fn->SetNumberOfHills(30);

        vtkNew<vtkParametricFunctionSource> source;
        source->SetParametricFunction(fn);
        source->SetUResolution(50);
        source->SetVResolution(50);
        source->SetScalarModeToZ();
        source->Update();

        // Name the arrays (not needed in VTK 6.2+ for vtkParametricFunctionSource).
        // source->GetOutput()->GetPointData()->GetNormals()->SetName("Normals");
        // source->GetOutput()->GetPointData()->GetScalars()->SetName("Scalars");
        // Rename the scalars to "Elevation" since we are using the Z-scalars as
        // elevations.
        source->GetOutput()->GetPointData()->GetScalars()->SetName("Elevation");

        vtkNew<vtkTransform> transform;
        transform->Translate(0.0, 5.0, 15.0);
        transform->RotateX(-90.0);
        vtkNew<vtkTransformPolyDataFilter> transformFilter;
        transformFilter->SetInputConnection(source->GetOutputPort());
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetParametricTorus()
    {
        vtkNew<vtkParametricTorus> fn;
        fn->SetRingRadius(5);
        fn->SetCrossSectionRadius(2);

        vtkNew<vtkParametricFunctionSource> source;
        source->SetParametricFunction(fn);
        source->SetUResolution(50);
        source->SetVResolution(50);
        source->SetScalarModeToZ();
        source->Update();

        // Name the arrays (not needed in VTK 6.2+ for vtkParametricFunctionSource).
        // source->GetOutput()->GetPointData()->GetNormals()->SetName("Normals");
        // source->GetOutput()->GetPointData()->GetScalars()->SetName("Scalars");
        // Rename the scalars to "Elevation" since we are using the Z-scalars as
        // elevations.
        source->GetOutput()->GetPointData()->GetScalars()->SetName("Elevation");

        vtkNew<vtkTransform> transform;
        transform->RotateX(-90.0);
        vtkNew<vtkTransformPolyDataFilter> transformFilter;
        transformFilter->SetInputConnection(source->GetOutputPort());
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetPlane()
    {
        vtkNew<vtkPlaneSource> source;
        source->SetOrigin(-10.0, -10.0, 0.0);
        source->SetPoint2(-10.0, 10.0, 0.0);
        source->SetPoint1(10.0, -10.0, 0.0);
        source->SetXResolution(20);
        source->SetYResolution(20);
        source->Update();

        vtkNew<vtkTransform> transform;
        transform->Translate(0.0, 0.0, 0.0);
        transform->RotateX(-90.0);
        vtkNew<vtkTransformPolyDataFilter> transformFilter;
        transformFilter->SetInputConnection(source->GetOutputPort());
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        // We have a m x n array of quadrilaterals arranged as a regular tiling in a
        // plane. So pass it through a triangle filter since the curvature filter only
        // operates on polys.
        vtkNew<vtkTriangleFilter> tri;
        tri->SetInputConnection(transformFilter->GetOutputPort());

        // Pass it though a CleanPolyDataFilter and merge any points which
        // are coincident, or very close
        vtkNew<vtkCleanPolyData> cleaner;
        cleaner->SetInputConnection(tri->GetOutputPort());
        cleaner->SetTolerance(0.005);
        cleaner->Update();

        return cleaner->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetSphere()
    {
        vtkNew<vtkSphereSource> source;
        source->SetCenter(0.0, 0.0, 0.0);
        source->SetRadius(10.0);
        source->SetThetaResolution(32);
        source->SetPhiResolution(32);
        source->Update();

        return source->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetTorus()
    {
        vtkNew<vtkSuperquadricSource> source;
        source->SetCenter(0.0, 0.0, 0.0);
        source->SetCenter(1.0, 1.0, 1.0);
        source->SetPhiResolution(64);
        source->SetThetaResolution(64);
        source->SetThetaRoundness(1);
        source->SetThickness(0.5);
        source->SetSize(10);
        source->SetToroidal(1);
        source->Update();

        // The quadric is made of strips, so pass it through a triangle filter as
        // the curvature filter only operates on polys
        vtkNew<vtkTriangleFilter> tri;
        tri->SetInputConnection(source->GetOutputPort());

        // The quadric has nasty discontinuities from the way the edges are generated
        // so let's pass it though a CleanPolyDataFilter and merge any points which
        // are coincident, or very close
        vtkNew<vtkCleanPolyData> cleaner;
        cleaner->SetInputConnection(tri->GetOutputPort());
        cleaner->SetTolerance(0.005);
        cleaner->Update();

        return cleaner->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetSource(std::string const& source)
    {
        std::string surface = source;
        std::transform(surface.begin(), surface.end(), surface.begin(),
            [](unsigned char c) { return std::tolower(c); });
        std::map<std::string, int> available_surfaces = {
            {"hills", 0},       {"parametrictorus", 1}, {"plane", 2},
            {"randomhills", 3}, {"sphere", 4},          {"torus", 5} };
        if (available_surfaces.find(surface) == available_surfaces.end())
        {
            std::cout << "The surface is not available." << std::endl;
            std::cout << "Using RandomHills instead." << std::endl;
            surface = "randomhills";
        }
        switch (available_surfaces[surface])
        {
        case 0:
            return GetHills();
            break;
        case 1:
            return GetParametricTorus();
            break;
        case 2:
            return GetElevations(GetPlane());
            break;
        case 3:
            return GetParametricHills();
            break;
        case 4:
            return GetElevations(GetSphere());
            break;
        case 5:
            return GetElevations(GetTorus());
            break;
        }
        return GetParametricHills();
    }

    vtkSmartPointer<vtkColorSeries> GetColorSeries()
    {

        vtkNew<vtkColorSeries> colorSeries;
        // Select a color scheme.
        int colorSeriesEnum;
        // colorSeriesEnum = colorSeries->BREWER_DIVERGING_BROWN_BLUE_GREEN_9;
        // colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_10;
        // colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_3;
        // colorSeriesEnum = colorSeries->BREWER_DIVERGING_PURPLE_ORANGE_9;
        // colorSeriesEnum = colorSeries->BREWER_SEQUENTIAL_BLUE_PURPLE_9;
        // colorSeriesEnum = colorSeries->BREWER_SEQUENTIAL_BLUE_GREEN_9;
        colorSeriesEnum = colorSeries->BREWER_QUALITATIVE_SET3;
        // colorSeriesEnum = colorSeries->CITRUS;
        colorSeries->SetColorScheme(colorSeriesEnum);
        return colorSeries;
    }

    vtkSmartPointer<vtkLookupTable> GetCategoricalLUT()
    {
        vtkSmartPointer<vtkColorSeries> colorSeries = GetColorSeries();
        // Make the lookup table.
        vtkNew<vtkLookupTable> lut;
        colorSeries->BuildLookupTable(lut, vtkColorSeries::CATEGORICAL);
        lut->SetNanColor(0, 0, 0, 1);

        return lut;
    }

    vtkSmartPointer<vtkLookupTable> GetOrdinaLUT()
    {
        vtkSmartPointer<vtkColorSeries> colorSeries = GetColorSeries();
        // Make the lookup table.
        vtkNew<vtkLookupTable> lut;
        colorSeries->BuildLookupTable(lut, vtkColorSeries::ORDINAL);
        lut->SetNanColor(0, 0, 0, 1);

        return lut;
    }

    // clang-format off
    /**
     * See: [Diverging Color Maps for Scientific Visualization](https://www.kennethmoreland.com/color-maps/)
     *
     *                   start point         midPoint            end point
     * cool to warm:     0.230, 0.299, 0.754 0.865, 0.865, 0.865 0.706, 0.016, 0.150
     * purple to orange: 0.436, 0.308, 0.631 0.865, 0.865, 0.865 0.759, 0.334, 0.046
     * green to purple:  0.085, 0.532, 0.201 0.865, 0.865, 0.865 0.436, 0.308, 0.631
     * blue to brown:    0.217, 0.525, 0.910 0.865, 0.865, 0.865 0.677, 0.492, 0.093
     * green to red:     0.085, 0.532, 0.201 0.865, 0.865, 0.865 0.758, 0.214, 0.233
     *
     */
     // clang-format on
    vtkSmartPointer<vtkLookupTable> GetDivergingLUT()
    {

        vtkNew<vtkColorTransferFunction> ctf;
        ctf->SetColorSpaceToDiverging();
        ctf->AddRGBPoint(0.0, 0.085, 0.532, 0.201);
        ctf->AddRGBPoint(0.5, 0.865, 0.865, 0.865);
        ctf->AddRGBPoint(1.0, 0.758, 0.214, 0.233);

        auto tableSize = 256;
        vtkNew<vtkLookupTable> lut;
        lut->SetNumberOfTableValues(tableSize);
        lut->Build();

        for (auto i = 0; i < lut->GetNumberOfColors(); ++i)
        {
            std::array<double, 3> rgb;
            ctf->GetColor(static_cast<double>(i) / lut->GetNumberOfColors(),
                rgb.data());
            std::array<double, 4> rgba{ 0.0, 0.0, 0.0, 1.0 };
            std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
            lut->SetTableValue(static_cast<vtkIdType>(i), rgba.data());
        }

        return lut;
    }

    vtkSmartPointer<vtkLookupTable> ReverseLUT(vtkLookupTable* lut)
    {
        // First do a deep copy just to get the whole structure
        // and then reverse the colors and annotations.
        vtkNew<vtkLookupTable> lutr;
        lutr->DeepCopy(lut);
        vtkIdType t = lut->GetNumberOfTableValues() - 1;
        for (vtkIdType i = t; i >= 0; --i)
        {
            std::array<double, 3> rgb{ 0.0, 0.0, 0.0 };
            std::array<double, 4> rgba{ 0.0, 0.0, 0.0, 1.0 };
            lut->GetColor(i, rgb.data());
            std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
            rgba[3] = lut->GetOpacity(i);
            lutr->SetTableValue(t - i, rgba.data());
        }
        t = lut->GetNumberOfAnnotatedValues() - 1;
        for (vtkIdType i = t; i >= 0; --i)
        {
            lutr->SetAnnotation(t - i, lut->GetAnnotation(i));
        }

        return lutr;
    }

    vtkNew<vtkGlyph3D> GetGlyphs(vtkPolyData* src, double const& scaleFactor,
        bool const& reverseNormals)
    {
        // Sometimes the contouring algorithm can create a volume whose gradient
        // vector and ordering of polygon(using the right hand rule) are
        // inconsistent. vtkReverseSense cures this problem.
        vtkNew<vtkReverseSense> reverse;
        vtkNew<vtkMaskPoints> maskPts;
        maskPts->SetOnRatio(5);
        maskPts->RandomModeOn();
        if (reverseNormals)
        {
            reverse->SetInputData(src);
            reverse->ReverseCellsOn();
            reverse->ReverseNormalsOn();
            maskPts->SetInputConnection(reverse->GetOutputPort());
        }
        else
        {
            maskPts->SetInputData(src);
        }

        // Source for the glyph filter
        vtkNew<vtkArrowSource> arrow;
        arrow->SetTipResolution(16);
        arrow->SetTipLength(0.3);
        arrow->SetTipRadius(0.1);

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetSourceConnection(arrow->GetOutputPort());
        glyph->SetInputConnection(maskPts->GetOutputPort());
        glyph->SetVectorModeToUseNormal();
        glyph->SetScaleFactor(scaleFactor);
        glyph->SetColorModeToColorByVector();
        glyph->SetScaleModeToScaleByVector();
        glyph->OrientOn();
        glyph->Update();

        return glyph;
    }

    std::map<int, std::vector<double>> GetBands(double const dR[2],
        int const& numberOfBands,
        int const& precision,
        bool const& nearestInteger)
    {
        auto prec = abs(precision);
        prec = (prec > 14) ? 14 : prec;

        auto RoundOff = [&prec](const double& x) {
            auto pow_10 = std::pow(10.0, prec);
            return std::round(x * pow_10) / pow_10;
        };

        std::map<int, std::vector<double>> bands;
        if ((dR[1] < dR[0]) || (numberOfBands <= 0))
        {
            return bands;
        }
        double x[2];
        for (int i = 0; i < 2; ++i)
        {
            x[i] = dR[i];
        }
        if (nearestInteger)
        {
            x[0] = std::floor(x[0]);
            x[1] = std::ceil(x[1]);
        }
        double dx = (x[1] - x[0]) / static_cast<double>(numberOfBands);
        std::vector<double> b;
        b.push_back(x[0]);
        b.push_back(x[0] + dx / 2.0);
        b.push_back(x[0] + dx);
        for (int i = 0; i < numberOfBands; ++i)
        {
            if (i == 0)
            {
                for (std::vector<double>::iterator p = b.begin(); p != b.end(); ++p)
                {
                    *p = RoundOff(*p);
                }
                b[0] = x[0];
            }
            bands[i] = b;
            for (std::vector<double>::iterator p = b.begin(); p != b.end(); ++p)
            {
                *p = RoundOff(*p + dx);
            }
        }
        return bands;
    }

    std::map<int, std::vector<double>>
        GetCustomBands(double const dR[2], int const& numberOfBands,
            std::vector<std::array<double, 2>> const& myBands)
    {
        std::map<int, std::vector<double>> bands;
        if ((dR[1] < dR[0]) || (numberOfBands <= 0))
        {
            return bands;
        }

        std::vector<std::array<double, 2>> x;
        std::copy(myBands.begin(), myBands.end(), std::back_inserter(x));

        // Determine the index of the range minimum and range maximum.
        int idxMin = 0;
        for (auto idx = 0; idx < static_cast<int>(myBands.size()); ++idx)
        {
            if (dR[0] < myBands[idx][1] && dR[0] >= myBands[idx][0])
            {
                idxMin = idx;
                break;
            }
        }
        int idxMax = static_cast<int>(myBands.size()) - 1;
        for (int idx = static_cast<int>(myBands.size()) - 1; idx >= 0; --idx)
        {
            if (dR[1] < myBands[idx][1] && dR[1] >= myBands[idx][0])
            {
                idxMax = static_cast<int>(idx);
                break;
            }
        }

        // Set the minimum to match the range minimum.
        x[idxMin][0] = dR[0];
        x[idxMax][1] = dR[1];
        for (int i = idxMin; i < idxMax + 1; ++i)
        {
            std::vector<double> b(3);
            b[0] = x[i][0];
            b[1] = x[i][0] + (x[i][1] - x[i][0]) / 2.0;
            b[2] = x[i][1];
            bands[i] = b;
        }
        return bands;
    }

    std::map<int, std::vector<double>> GetIntegralBands(double const dR[2])
    {
        std::map<int, std::vector<double>> bands;
        if (dR[1] < dR[0])
        {
            return bands;
        }
        double x[2];
        for (int i = 0; i < 2; ++i)
        {
            x[i] = dR[i];
        }
        x[0] = std::floor(x[0]);
        x[1] = std::ceil(x[1]);
        int numberOfBands = static_cast<int>(std::abs(x[1]) + std::abs(x[0]));
        return GetBands(x, numberOfBands, false);
    }

    std::map<int, int> GetFrequencies(std::map<int, std::vector<double>>& bands,
        vtkPolyData* src)
    {
        std::map<int, int> freq;
        for (auto i = 0; i < static_cast<int>(bands.size()); ++i)
        {
            freq[i] = 0;
        }
        vtkIdType tuples = src->GetPointData()->GetScalars()->GetNumberOfTuples();
        for (int i = 0; i < tuples; ++i)
        {
            double* x = src->GetPointData()->GetScalars()->GetTuple(i);
            for (auto j = 0; j < static_cast<int>(bands.size()); ++j)
            {
                if (*x <= bands[j][2])
                {
                    freq[j] = freq[j] + 1;
                    break;
                }
            }
        }
        return freq;
    }

    void AdjustFrequencyRanges(std::map<int, std::vector<double>>& bands,
        std::map<int, int>& freq)
    {
        // Get the indices of the first and last non-zero elements.
        auto first = 0;
        for (auto i = 0; i < static_cast<int>(freq.size()); ++i)
        {
            if (freq[i] != 0)
            {
                first = i;
                break;
            }
        }
        std::vector<int> keys;
        for (std::map<int, int>::iterator it = freq.begin(); it != freq.end(); ++it)
        {
            keys.push_back(it->first);
        }
        std::reverse(keys.begin(), keys.end());
        auto last = keys[0];
        for (size_t i = 0; i < keys.size(); ++i)
        {
            if (freq[keys[i]] != 0)
            {
                last = keys[i];
                break;
            }
        }
        // Now adjust the ranges.
        std::map<int, int>::iterator freqItr;
        freqItr = freq.find(first);
        freq.erase(freq.begin(), freqItr);
        freqItr = ++freq.find(last);
        freq.erase(freqItr, freq.end());
        std::map<int, std::vector<double>>::iterator bandItr;
        bandItr = bands.find(first);
        bands.erase(bands.begin(), bandItr);
        bandItr = ++bands.find(last);
        bands.erase(bandItr, bands.end());
        // Reindex freq and bands.
        std::map<int, int> adjFreq;
        int idx = 0;
        for (auto p : freq)
        {
            adjFreq[idx] = p.second;
            ++idx;
        }
        std::map<int, std::vector<double>> adjBands;
        idx = 0;
        for (auto p : bands)
        {
            adjBands[idx] = p.second;
            ++idx;
        }
        bands = adjBands;
        freq = adjFreq;
    }

    void PrintBandsFrequencies(std::map<int, std::vector<double>> const& bands,
        std::map<int, int>& freq, int const& precision)
    {
        auto prec = abs(precision);
        prec = (prec > 14) ? 14 : prec;

        if (bands.size() != freq.size())
        {
            std::cout << "Bands and frequencies must be the same size." << std::endl;
            return;
        }
        std::ostringstream os;
        os << "Bands & Frequencies:\n";
        size_t idx = 0;
        auto total = 0;
        auto width = prec + 6;
        for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
            p != bands.end(); ++p)
        {
            total += freq[p->first];
            for (std::vector<double>::const_iterator q = p->second.begin();
                q != p->second.end(); ++q)
            {
                if (q == p->second.begin())
                {
                    os << std::setw(4) << idx << " [";
                }
                if (q == std::prev(p->second.end()))
                {
                    os << std::fixed << std::setw(width) << std::setprecision(prec) << *q
                        << "]: " << std::setw(8) << freq[p->first] << "\n";
                }
                else
                {
                    os << std::fixed << std::setw(width) << std::setprecision(prec) << *q
                        << ", ";
                }
            }
            ++idx;
        }
        width = 3 * width + 13;
        os << std::left << std::setw(width) << "Total" << std::right << std::setw(8)
            << total << std::endl;
        std::cout << os.str() << endl;
    }

} // namespace

#endif // TEST101


#ifdef TEST200


#endif // TEST200

#ifdef TEST201

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkInteractorStyleTrackballCamera.h>


int main(int, char* [])
{
    // 加载一个STL模型
    vtkSmartPointer<vtkSTLReader> source = vtkSmartPointer<vtkSTLReader>::New();
    source->SetFileName("test1.stl");
    source->Update();

    int numPts = source->GetOutput()->GetPoints()->GetNumberOfPoints();                    // 获取模型的顶点数量
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();        // 创建存储顶点属性的float数组
    scalars->SetNumberOfValues(numPts);
    for (int i = 0; i < numPts; ++i)        // 为属性数组中的每个元素设置标量值（这个标量值可以当作颜色值）
        scalars->SetValue(i, i);

    vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
    poly->DeepCopy(source->GetOutput());
    poly->GetPointData()->SetScalars(scalars);

    // 创建颜色查找表
    vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
    hueLut->SetNumberOfColors(numPts);        // 指定颜色查找表中有多少种颜色
    hueLut->SetHueRange(0.67, 0.0);            // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    hueLut->Build();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(poly);
    mapper->SetScalarRange(0, numPts);            // 设置标量值的范围
    mapper->ScalarVisibilityOn();
    //mapper->SetColorModeToMapScalars();        // 无论变量数据是何种类型，该方法都通过查询表对标量数据进行映射
    mapper->SetColorModeToDefault();            // 默认的映射器行为，即把unsigned char类型的标量属性数据当作颜色值，不执行隐式。对于其他类型的标量数据，将通过查询表映射。
    mapper->SetLookupTable(hueLut);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->GradientBackgroundOn();
    renderer->SetBackground(1, 1, 1);
    renderer->SetBackground2(0, 0, 0);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(actor);
    renderWindow->SetSize(600, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST201

#ifdef TEST202

//https://blog.csdn.net/muye2356/article/details/115144581


#include <vtkOpenFOAMReader.h>
#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPointData.h>
#include <vtkDataSetMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
#include <vtkTextProperty.h>
#include <vtkLineSource.h>
#include <vtkStreamTracer.h>
#include <vtkPolyData.h>
#include <vtkArrayCalculator.h>

int main()
{
    vtkSmartPointer<vtkOpenFOAMReader> openFOAMReader = vtkSmartPointer<vtkOpenFOAMReader>::New();
    openFOAMReader->SetFileName("04vtk\\resource\\test.foam");//设置读取文件路径
    openFOAMReader->SetCreateCellToPoint(1);
    openFOAMReader->SetSkipZeroTime(1);//开启跳过0时刻
    openFOAMReader->SetTimeValue(298.0);//设置需要读取的时刻
    openFOAMReader->Update();

    vtkUnstructuredGrid* block0 = vtkUnstructuredGrid::SafeDownCast(openFOAMReader->GetOutput()->GetBlock(0));

    //设置生成流线的位置
    vtkSmartPointer<vtkLineSource> line1 = vtkSmartPointer<vtkLineSource>::New();
    line1->SetPoint1(-0.019, 0.0254, 0.0005);
    line1->SetPoint2(-0.0206, 0, 0.0005);
    line1->SetResolution(10);
    line1->Update();

    vtkSmartPointer<vtkStreamTracer> streamline = vtkSmartPointer<vtkStreamTracer>::New();
    streamline->SetSourceConnection(line1->GetOutputPort());
    streamline->SetInputData(block0);
    streamline->SetIntegratorTypeToRungeKutta45();//设置流线的积分类型
    streamline->SetMaximumPropagation(5000);//设置流线长度
    streamline->SetIntegrationStepUnit(2);//设置流线积分步长单位
    streamline->SetInitialIntegrationStep(0.2);//设置流线积分初始步长
    streamline->SetMinimumIntegrationStep(0.01);//设置流线积分最小步长
    streamline->SetMaximumIntegrationStep(0.5);//设置流线积分最大步长
    streamline->SetMaximumNumberOfSteps(2000);//设置流线积分最大步数
    streamline->SetIntegrationDirectionToBoth();//设置流线积分方向
    streamline->SetTerminalSpeed(1e-12);//设置流线积分终止速度
    streamline->SetMaximumError(1e-6);
    streamline->Update();

    //计算速度的模
    vtkSmartPointer<vtkArrayCalculator> calc = vtkSmartPointer<vtkArrayCalculator>::New();
    calc->SetInputConnection(streamline->GetOutputPort());
    calc->AddVectorArrayName("U");
    calc->SetFunction("mag(U)");
    calc->SetResultArrayName("u_mag");
    calc->Update();


    vtkPolyData* streamlinedata = vtkPolyData::SafeDownCast(calc->GetOutput());
    streamlinedata->GetPointData()->SetActiveScalars("u_mag");

    //计算速度范围
    double scalarRange[2]{ 0.0,1.0 };
    if (auto sxalars_ = streamlinedata->GetPointData()->GetScalars())
    {
        scalarRange[0] = sxalars_->GetRange()[0];
        scalarRange[1] = sxalars_->GetRange()[1];
    }

    vtkSmartPointer<vtkScalarBarActor> scalarBar =
        vtkSmartPointer<vtkScalarBarActor>::New();
    vtkSmartPointer<vtkLookupTable> pColorTable =
        vtkSmartPointer<vtkLookupTable>::New();
    pColorTable->SetNumberOfTableValues(31);
    pColorTable->SetHueRange(0.67, 0);//标量条颜色范围，从蓝到红
    pColorTable->SetAlphaRange(1.0, 1.0);
    pColorTable->SetValueRange(1, 1);
    pColorTable->SetSaturationRange(1, 1);
    pColorTable->SetRange(scalarRange);
    pColorTable->Build();

    scalarBar->SetTitle("U (m/s)");
    scalarBar->GetTitleTextProperty()->SetColor(0, 0, 0);
    scalarBar->GetTitleTextProperty()->SetFontFamilyToArial();
    scalarBar->GetTitleTextProperty()->SetFontSize(20);
    scalarBar->GetLabelTextProperty()->SetColor(0, 0, 0);
    scalarBar->SetLabelFormat("%5.3f");
    scalarBar->GetLabelTextProperty()->SetFontFamilyToArial();
    scalarBar->GetLabelTextProperty()->SetFontSize(20);
    scalarBar->SetNumberOfLabels(7);
    scalarBar->SetUnconstrainedFontSize(1);
    scalarBar->SetMaximumWidthInPixels(80);
    scalarBar->SetMaximumHeightInPixels(900);
    scalarBar->SetLookupTable(pColorTable);


    vtkSmartPointer<vtkPolyDataMapper> streamlinemappper = vtkSmartPointer<vtkPolyDataMapper>::New();
    streamlinemappper->SetInputData(streamlinedata);
    streamlinemappper->SetLookupTable(pColorTable);
    streamlinemappper->SetScalarRange(scalarRange);

    vtkSmartPointer<vtkActor> streamlineactor = vtkSmartPointer<vtkActor>::New();
    streamlineactor->SetMapper(streamlinemappper);

    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
    ren->AddActor(streamlineactor);
    ren->AddActor(scalarBar);
    ren->SetBackground(1, 1, 1);//设置背景色为白色
    ren->ResetCamera();

    vtkSmartPointer<vtkRenderWindow> renWin =
        vtkSmartPointer<vtkRenderWindow>::New();

    vtkSmartPointer<vtkRenderWindowInteractor> iren =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> TrackballCamera =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    iren->SetInteractorStyle(TrackballCamera);

    renWin->AddRenderer(ren.GetPointer());
    renWin->SetSize(500, 500);
    renWin->Render();


    iren->Initialize();
    iren->Start();

    return 0;
}

#endif // TEST202



#ifdef TEST203
//https://kitware.github.io/vtk-examples/site/Cxx/VisualizationAlgorithms/FilledContours/
#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarsToColors.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkCubeSource.h>

#include <vector>

int main(int argc, char* argv[])
{
    //if (argc < 3)
    //{
    //    std::cerr
    //        << "Usage: " << argv[0]
    //        << " InputPolyDataFile(.vtp) NumberOfContours e.g filledContours.vtp 10"
    //        << std::endl;
    //    return EXIT_FAILURE;
    //}

    vtkNew<vtkCubeSource> cube;


    vtkNew<vtkNamedColors> colors;

    // Read the file
    //vtkNew<vtkXMLPolyDataReader> reader;

    //reader->SetFileName(argv[1]);
    //reader->Update(); // Update so that we can get the scalar range

    vtkNew<vtkFloatArray> scalars;

    // Load the point, cell, and data attributes.
    for (auto i = 0ul; i < cube->GetOutput()->GetPointData()->GetNumberOfTuples(); ++i)
    {
        scalars->InsertTuple1(i, i);
    }

    cube->GetOutput()->GetPointData()->SetScalars(scalars);

    double scalarRange[2];
    //reader->GetOutput()->GetPointData()->GetScalars()->GetRange(scalarRange);
    cube->GetOutput()->GetPointData()->GetScalars()->GetRange(scalarRange);

    vtkNew<vtkAppendPolyData> appendFilledContours;

    //int numberOfContours = atoi(argv[2]);
    int numberOfContours = 5;

    double delta = (scalarRange[1] - scalarRange[0]) /
        static_cast<double>(numberOfContours - 1);

    // Keep the clippers alive
    std::vector<vtkSmartPointer<vtkClipPolyData>> clippersLo;
    std::vector<vtkSmartPointer<vtkClipPolyData>> clippersHi;

    for (int i = 0; i < numberOfContours; i++)
    {
        double valueLo = scalarRange[0] + static_cast<double>(i) * delta;
        double valueHi = scalarRange[0] + static_cast<double>(i + 1) * delta;
        clippersLo.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersLo[i]->SetValue(valueLo);
        if (i == 0)
        {
            //clippersLo[i]->SetInputConnection(reader->GetOutputPort());
            clippersLo[i]->SetInputConnection(cube->GetOutputPort());
        }
        else
        {
            clippersLo[i]->SetInputConnection(clippersHi[i - 1]->GetOutputPort(1));
        }
        clippersLo[i]->InsideOutOff();
        clippersLo[i]->Update();

        clippersHi.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersHi[i]->SetValue(valueHi);
        clippersHi[i]->SetInputConnection(clippersLo[i]->GetOutputPort());
        clippersHi[i]->GenerateClippedOutputOn();
        clippersHi[i]->InsideOutOn();
        clippersHi[i]->Update();
        if (clippersHi[i]->GetOutput()->GetNumberOfCells() == 0)
        {
            continue;
        }

        vtkNew<vtkFloatArray> cd;
        cd->SetNumberOfComponents(1);
        cd->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
        cd->FillComponent(0, valueLo);

        clippersHi[i]->GetOutput()->GetCellData()->SetScalars(cd);
        appendFilledContours->AddInputConnection(clippersHi[i]->GetOutputPort());
    }

    vtkNew<vtkCleanPolyData> filledContours;
    filledContours->SetInputConnection(appendFilledContours->GetOutputPort());

    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(numberOfContours + 1);
    lut->Build();
    vtkNew<vtkPolyDataMapper> contourMapper;
    contourMapper->SetInputConnection(filledContours->GetOutputPort());
    contourMapper->SetScalarRange(scalarRange[0], scalarRange[1]);
    contourMapper->SetScalarModeToUseCellData();
    contourMapper->SetLookupTable(lut);

    vtkNew<vtkActor> contourActor;
    contourActor->SetMapper(contourMapper);
    contourActor->GetProperty()->SetInterpolationToFlat();

    vtkNew<vtkContourFilter> contours;
    contours->SetInputConnection(filledContours->GetOutputPort());
    contours->GenerateValues(numberOfContours, scalarRange[0], scalarRange[1]);

    vtkNew<vtkPolyDataMapper> contourLineMapperer;
    contourLineMapperer->SetInputConnection(contours->GetOutputPort());
    contourLineMapperer->SetScalarRange(scalarRange[0], scalarRange[1]);
    contourLineMapperer->ScalarVisibilityOff();

    vtkNew<vtkActor> contourLineActor;
    contourLineActor->SetMapper(contourLineMapperer);
    contourLineActor->GetProperty()->SetLineWidth(2);

    // The usual renderer, render window and interactor
    vtkNew<vtkRenderer> ren1;
    vtkNew<vtkRenderWindow> renWin;
    vtkNew<vtkRenderWindowInteractor> iren;

    renWin->AddRenderer(ren1);
    renWin->SetWindowName("FilledContours");

    iren->SetRenderWindow(renWin);

    // Add the actors
    ren1->AddActor(contourActor);
    ren1->AddActor(contourLineActor);
    ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

    // Begin interaction
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST203

#ifdef TEST300

// https://kitware.github.io/vtk-examples/site/Cxx/Filtering/Glyph3D/

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 1, 0);
    points->InsertNextPoint(2, 2, 0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    // Create anything you want here, we will use a cube for the demo.
    vtkNew<vtkCubeSource> cubeSource;

    vtkNew<vtkGlyph3D> glyph3D;
    glyph3D->SetSourceConnection(cubeSource->GetOutputPort());
    glyph3D->SetInputData(polydata);
    glyph3D->Update();

    // Visualize
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(glyph3D->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Salmon").GetData());

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderWindow->SetWindowName("Glyph3D");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST300

#ifdef TEST301

#include <vtkArrowSource.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkGlyph2D.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 1, 0);
    points->InsertNextPoint(2, 2, 0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    // Create anything you want here, we will use a polygon for the demo.
    //vtkNew<vtkRegularPolygonSource> source; // default is 6 sides
    vtkNew< vtkArrowSource> source;

    vtkNew<vtkGlyph2D> glyph2D;
    glyph2D->SetSourceConnection(source->GetOutputPort());
    glyph2D->SetInputData(polydata);
    glyph2D->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(glyph2D->GetOutputPort());
    mapper->Update();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Salmon").GetData());

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    vtkNew<vtkInteractorStyleRubberBand3D> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetWindowName("Glyph2D");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST301

#ifdef TEST302

#include <vtkArrowSource.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph2D.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkGlyph3DMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>


int main(int, char* [])
{
    // 矢量图的图案
    vtkNew<vtkArrowSource> source;
    source->Update();

    // 颜色映射表
    vtkNew<vtkLookupTable> colorTable;
    colorTable->SetNumberOfColors(20);
    colorTable->SetHueRange(.0, .3334); // red 0 green 0.3334 blue 0.6667
    colorTable->SetRange(1, 4);
    colorTable->Build();

    // 色卡
    vtkNew<vtkScalarBarActor> legendActor;
    legendActor->SetNumberOfLabels(20);
    legendActor->SetLookupTable(colorTable);
    legendActor->SetMaximumNumberOfColors(20);

    // 顶点数据，即矢量图位置
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    // 每一个顶点的标量
    vtkNew<vtkDoubleArray> pointScalars;
    auto numComponents = pointScalars->GetNumberOfComponents(); // 默认为1
    pointScalars->InsertNextValue(1.);
    pointScalars->InsertNextValue(4.);
    pointScalars->InsertNextValue(3.);
    pointScalars->InsertNextValue(2.);
    polydata->GetPointData()->SetScalars(pointScalars);

    // 每一个顶点的向量
    vtkNew<vtkDoubleArray> pointVectors;
    pointVectors->SetNumberOfComponents(3); // 3维向量 {x,y,z}
    pointVectors->InsertNextTuple3(5.0, 0.0, 0.0);
    pointVectors->InsertNextTuple3(0.0, 3.0, 0.0);
    pointVectors->InsertNextTuple3(1.0, 1.0, 1.0);
    pointVectors->InsertNextTuple3(-1.0, -1.0, 0.0);
    polydata->GetPointData()->SetVectors(pointVectors);

    // 每一个顶点的法线
    vtkNew<vtkDoubleArray> pointNormals;
    pointNormals->SetNumberOfComponents(3);
    pointNormals->InsertNextTuple3(0., 0., 10.);
    pointNormals->InsertNextTuple3(0., 0., -1.);
    pointNormals->InsertNextTuple3(0., 0., 1.);
    pointNormals->InsertNextTuple3(0., 0., -10.);
    polydata->GetPointData()->SetNormals(pointNormals);

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputData(polydata);             // 顶点数据，即矢量图每一个图案的位置
    glyph->SetSourceData(source->GetOutput()); // 资源数据，即矢量图的图案
    glyph->SetScaleFactor(1.);                 // 缩放比例
    //glyph->SetClamping(true);
    //glyph->SetRange(0, 1);                     // 

    // 默认颜色由标量决定
    // 默认大小由标量决定
    // 默认方向由向量决定
    std::cout << "--------------------------------------------\n" <<
        "color: " << glyph->GetColorModeAsString() <<
        "\nscale: " << glyph->GetScaleModeAsString() <<
        "\nvector: " << glyph->GetVectorModeAsString() << '\n';

    // 颜色
    glyph->SetColorModeToColorByScalar();           // 颜色由标量决定
    //glyph->SetColorModeToColorByScale();            // 颜色由标量决定
    //glyph->SetColorModeToColorByVector();           // 颜色由向量决定

    // 大小
    //glyph->SetScaleModeToDataScalingOff();          // 关闭由于数据（标量或向量）导致的缩放，所有矢量图的图案大小一致
    //glyph->SetScaleModeToScaleByVectorComponents(); // 画出来的箭头有的是平面的，有的没有显示
    glyph->SetScaleModeToScaleByScalar();           // 大小由标量决定
    //glyph->SetScaleModeToScaleByVector();           // 大小由向量决定

    // 方向
    //glyph->OrientOff();                             // 禁用输入几何体沿向量/法线的方向
    //glyph->SetVectorModeToFollowCameraDirection();  // 
    //glyph->SetVectorModeToUseNormal();              // 方向由法线决定
    glyph->SetVectorModeToUseVector();              // 方向由向量决定
    //glyph->SetVectorModeToVectorRotationOff();      // 关闭跟随向量旋转


    std::cout << "--------------------------------------------\n" <<
        "color: " << glyph->GetColorModeAsString() <<
        "\nscale: " << glyph->GetScaleModeAsString() <<
        "\nvector: " << glyph->GetVectorModeAsString() << '\n';

    // mapper
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetLookupTable(colorTable);
    mapper->SetScalarRange(colorTable->GetRange());
    mapper->SetInputConnection(glyph->GetOutputPort());

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(legendActor);
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    renderWindow->SetWindowName("vector");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST302

#ifdef TEST303

// https://kitware.github.io/vtk-examples/site/Cxx/Visualization/Glyph3DMapper/

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnsignedCharArray.h>

int main(int, char* [])
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 1, 1);
    points->InsertNextPoint(2, 2, 2);

    vtkNew<vtkFloatArray> scaleFactors;
    scaleFactors->SetNumberOfComponents(3);
    scaleFactors->SetName("Scale Factors");
    scaleFactors->InsertNextTuple3(0.7, 1.0, 1.0);
    scaleFactors->InsertNextTuple3(1.0, 0.7, 1.0);
    scaleFactors->InsertNextTuple3(1.0, 1.0, 0.7);

    vtkNew<vtkNamedColors> namedColors;

    vtkNew<vtkUnsignedCharArray> colors;
    colors->SetName("Colors");
    colors->SetNumberOfComponents(3);
    colors->InsertNextTypedTuple(namedColors->GetColor3ub("Red").GetData());
    colors->InsertNextTypedTuple(namedColors->GetColor3ub("Green").GetData());
    colors->InsertNextTypedTuple(namedColors->GetColor3ub("Blue").GetData());

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);
    polydata->GetPointData()->AddArray(colors);
    polydata->GetPointData()->AddArray(scaleFactors);

    // Create anything you want here, we will use a cube for the demo.
    vtkNew<vtkCubeSource> cubeSource;

    vtkNew<vtkGlyph3DMapper> glyph3Dmapper;
    glyph3Dmapper->SetSourceConnection(cubeSource->GetOutputPort());
    glyph3Dmapper->SetInputData(polydata);
    glyph3Dmapper->SetScalarModeToUsePointFieldData();
    glyph3Dmapper->SetScaleArray("Scale Factors");
    glyph3Dmapper->SetScaleModeToScaleByVectorComponents();
    glyph3Dmapper->SelectColorArray("Colors");
    glyph3Dmapper->Update();

    vtkNew<vtkActor> actor;
    actor->SetMapper(glyph3Dmapper);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("Glyph3DMapper");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actor to the scene
    renderer->AddActor(actor);
    renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

    // Position the camera
    renderer->GetActiveCamera()->SetPosition(-10, 5, 0);
    renderer->GetActiveCamera()->SetFocalPoint(1, 1, 1);

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST303

#ifdef TEST304

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkArrowSource.h>
#include <vtkCubeSource.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkInteractorStyleRubberBand3D.h>

int main(int, char* [])
{
    //vtkNew<vtkArrowSource> cubeSource;
    vtkNew<vtkCubeSource> cubeSource;
    vtkNew<vtkNamedColors> namedColors;

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 1, 0);
    points->InsertNextPoint(2, 2, 0);

    vtkNew<vtkFloatArray> scaleFactors;
    scaleFactors->SetNumberOfComponents(3);
    scaleFactors->SetName("theScaleFactors");
    scaleFactors->InsertNextTuple3(0.7, 1.0, 1.0);
    scaleFactors->InsertNextTuple3(1.0, 0.7, 1.0);
    scaleFactors->InsertNextTuple3(1.0, 1.0, 0.7);

    vtkNew<vtkUnsignedCharArray> colors;
    colors->SetName("theColors");
    colors->SetNumberOfComponents(3);
    colors->InsertNextTypedTuple(namedColors->GetColor3ub("Red").GetData());
    colors->InsertNextTypedTuple(namedColors->GetColor3ub("Green").GetData());
    colors->InsertNextTypedTuple(namedColors->GetColor3ub("Blue").GetData());

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);
    polydata->GetPointData()->AddArray(colors);
    polydata->GetPointData()->AddArray(scaleFactors);

    vtkNew<vtkGlyph3DMapper> glyph3Dmapper;
    glyph3Dmapper->SetSourceConnection(cubeSource->GetOutputPort());
    glyph3Dmapper->SetInputData(polydata);
    glyph3Dmapper->SetScalarModeToUsePointFieldData();
    glyph3Dmapper->SetScaleArray("theScaleFactors"); // 和前面给polyData设置的AddArray有关
    glyph3Dmapper->SetScaleModeToScaleByVectorComponents();
    glyph3Dmapper->SelectColorArray("theColors");
    glyph3Dmapper->Update();

    vtkNew<vtkActor> actor;
    actor->SetMapper(glyph3Dmapper);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("Glyph3DMapper");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew< vtkInteractorStyleRubberBand3D> style;
    renderWindowInteractor->SetInteractorStyle(style);

    // Add the actor to the scene
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST304

#ifdef TEST305

#include <vtkArrowSource.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph2D.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkGlyph3DMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>

#include <vtkAlgorithmOutput.h>

#include <iostream>

int main(int, char* [])
{
    // 矢量图的图案
    vtkNew<vtkArrowSource> source;
    source->Update();

    // 颜色映射表
    vtkNew<vtkLookupTable> colorTable;
    colorTable->SetNumberOfColors(20);
    colorTable->SetHueRange(.0, .3334); // red 0 green 0.3334 blue 0.6667
    colorTable->SetRange(1, 4);
    colorTable->Build();

    // 色卡
    vtkNew<vtkScalarBarActor> legendActor;
    legendActor->SetNumberOfLabels(20);
    legendActor->SetLookupTable(colorTable);
    legendActor->SetMaximumNumberOfColors(20);

    // 顶点数据，即矢量图位置
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    // 每一个顶点的标量
    vtkNew<vtkDoubleArray> pointScalars;
    pointScalars->InsertNextValue(1.);
    pointScalars->InsertNextValue(4.);
    pointScalars->InsertNextValue(3.);
    pointScalars->InsertNextValue(2.);
    polydata->GetPointData()->SetScalars(pointScalars);

    // 每一个顶点的向量
    vtkNew<vtkDoubleArray> pointVectors;
    pointVectors->SetNumberOfComponents(3); // 3维向量 {x,y,z}
    pointVectors->InsertNextTuple3(5.0, 0.0, 0.0);
    pointVectors->InsertNextTuple3(0.0, 3.0, 0.0);
    pointVectors->InsertNextTuple3(1.0, 1.0, 1.0);
    pointVectors->InsertNextTuple3(-1.0, -1.0, 0.0);
    polydata->GetPointData()->SetVectors(pointVectors);

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputData(polydata);             // 顶点数据，即矢量图每一个图案的位置
    glyph->SetSourceData(source->GetOutput()); // 资源数据，即矢量图的图案

    // 颜色
    glyph->SetColorModeToColorByScalar();           // 颜色由标量决定
    // 大小
    glyph->SetScaleModeToScaleByScalar();           // 大小由标量决定
    // 方向
    glyph->SetVectorModeToUseVector();              // 方向由向量决定

    // mapper
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetLookupTable(colorTable);
    mapper->SetScalarRange(colorTable->GetRange());
    mapper->SetInputConnection(glyph->GetOutputPort());

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    //--------------------------------------------------------------------------------------
    // 从经过vtkAlgorithm变换后的数据获取源数据（注意使用的是独立数据集还是管道连接方式）
    if (vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer())
    {
        // 如果没有经过algorithm变换，reference为nullptr
        if (auto reference = vtkGlyph3D::SafeDownCast(algorithm.GetPointer()))
        {
            if (auto originSource = vtkPolyData::SafeDownCast(reference->GetInput()))
            {
                auto originPointNum = originSource->GetNumberOfPoints(); // 源数据顶点个数
                
                for (long long i = 0; i < originPointNum; i++)
                {
                    double pt[3]{ 0 };
                    originSource->GetPoint(i, pt);
                    std::cout << pt[0] << '\t' << pt[1] << '\t' << pt[2] << '\n';
                }
            }
        }
    }
    //--------------------------------------------------------------------------------------

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(legendActor);
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    renderWindow->SetWindowName("vector");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST305

#ifdef TEST306

#include <vtkArrowSource.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph2D.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkGlyph3DMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>


int main(int, char* [])
{
    // 矢量图的图案
    vtkNew<vtkArrowSource> source;
    source->Update();

    // 颜色映射表
    vtkNew<vtkLookupTable> colorTable;
    colorTable->SetNumberOfColors(20);
    colorTable->SetHueRange(.0, .3334); // red 0 green 0.3334 blue 0.6667
    colorTable->SetRange(1, 4);
    colorTable->Build();

    // 色卡
    vtkNew<vtkScalarBarActor> legendActor;
    legendActor->SetNumberOfLabels(20);
    legendActor->SetLookupTable(colorTable);
    legendActor->SetMaximumNumberOfColors(20);

    // 顶点数据，即矢量图位置
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    // 每一个顶点的标量
    vtkNew<vtkDoubleArray> pointScalars;
    pointScalars->InsertNextValue(1.);
    pointScalars->InsertNextValue(4.);
    pointScalars->InsertNextValue(3.);
    pointScalars->InsertNextValue(2.);
    polydata->GetPointData()->SetScalars(pointScalars);

    // 每一个顶点的向量
    vtkNew<vtkDoubleArray> pointVectors;
    pointVectors->SetNumberOfComponents(3); // 3维向量 {x,y,z}
    pointVectors->InsertNextTuple3(5.0, 0.0, 0.0);
    pointVectors->InsertNextTuple3(0.0, 3.0, 0.0);
    pointVectors->InsertNextTuple3(1.0, 1.0, 1.0);
    pointVectors->InsertNextTuple3(-1.0, -1.0, 0.0);
    polydata->GetPointData()->SetVectors(pointVectors);

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputData(polydata);             // 顶点数据，即矢量图每一个图案的位置
    glyph->SetSourceData(source->GetOutput()); // 资源数据，即矢量图的图案

    glyph->SetScaleFactor(1.);                 // 缩放比例
    glyph->SetClamping(true);                  // 开启大小映射，开启后必须调用SetRange()，不然箭头大小只能在{0,1}之间映射
    glyph->SetRange(0, 5);                     // 箭头大小映射表，因为大小由向量决定，所以这里的范围应该为{0,所有点的向量模长最大值}，向量模长始终不小于0

    // 颜色
    glyph->SetColorModeToColorByScalar();           // 颜色由标量决定
    // 大小
    glyph->SetScaleModeToScaleByVector();           // 大小由向量决定
    // 方向
    glyph->SetVectorModeToUseVector();              // 方向由向量决定
    glyph->Update();

    double range[2]{ 0 };
    glyph->GetRange(range);
    std::cout << "range: " << range[0] << '\t' << range[1] << '\n';

    // mapper
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetLookupTable(colorTable);
    mapper->SetScalarRange(colorTable->GetRange());
    mapper->SetInputConnection(glyph->GetOutputPort());

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(legendActor);
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    renderWindow->SetWindowName("vector");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST306

#ifdef TEST307

#include <vtkArrowSource.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph2D.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkGlyph3DMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>

#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkInteractorStyleRubberBand3D.h>

class CustomStyle :
    public vtkInteractorStyleRubberBand3D
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleRubberBand3D);

protected:
    void OnLeftButtonUp() override
    {
        if (this->Interactor && m_actor && m_mapper)
        {
            m_actor->GetProperty()->SetColor(1, 1, 1);
            m_mapper->ScalarVisibilityOff();
            this->Interactor->Render();
        }

        Superclass::OnLeftButtonUp();
    }

    void OnMiddleButtonUp() override
    {
        if (this->Interactor && m_actor && m_mapper)
        {
            std::cout << "visibility: " << m_mapper->GetScalarVisibility() << '\n';
        }

        Superclass::OnMiddleButtonUp();
    }

    void OnRightButtonUp() override
    {
        if (this->Interactor && m_actor && m_mapper)
        {
            m_mapper->ScalarVisibilityOn();
            this->Interactor->Render();
        }

        Superclass::OnRightButtonUp();
    }
public:
    void SetActor(const vtkSmartPointer<vtkActor>& actor)
    {
        m_actor = actor;
    }

    void SetMapper(const vtkSmartPointer<vtkMapper>& mapper)
    {
        m_mapper = mapper;
    }
private:
    vtkSmartPointer<vtkActor> m_actor{ nullptr };
    vtkSmartPointer<vtkMapper> m_mapper{ nullptr };
};

vtkStandardNewMacro(CustomStyle);


int main(int, char* [])
{
    // 矢量图的图案
    vtkNew<vtkArrowSource> source;
    source->Update();

    // 颜色映射表
    vtkNew<vtkLookupTable> colorTable;
    colorTable->SetNumberOfColors(20);
    colorTable->SetHueRange(.0, .3334); // red 0 green 0.3334 blue 0.6667
    colorTable->SetRange(1, 4);
    colorTable->Build();

    // 色卡
    vtkNew<vtkScalarBarActor> legendActor;
    legendActor->SetNumberOfLabels(20);
    legendActor->SetLookupTable(colorTable);
    legendActor->SetMaximumNumberOfColors(20);

    // 顶点数据，即矢量图位置
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    // 每一个顶点的标量
    vtkNew<vtkDoubleArray> pointScalars;
    pointScalars->InsertNextValue(1.);
    pointScalars->InsertNextValue(4.);
    pointScalars->InsertNextValue(3.);
    pointScalars->InsertNextValue(2.);
    polydata->GetPointData()->SetScalars(pointScalars);

    // 每一个顶点的向量
    vtkNew<vtkDoubleArray> pointVectors;
    pointVectors->SetNumberOfComponents(3); // 3维向量 {x,y,z}
    pointVectors->InsertNextTuple3(5.0, 0.0, 0.0);
    pointVectors->InsertNextTuple3(0.0, 3.0, 0.0);
    pointVectors->InsertNextTuple3(1.0, 1.0, 1.0);
    pointVectors->InsertNextTuple3(-1.0, -1.0, 0.0);
    polydata->GetPointData()->SetVectors(pointVectors);

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputData(polydata);             // 顶点数据，即矢量图每一个图案的位置
    glyph->SetSourceData(source->GetOutput()); // 资源数据，即矢量图的图案

    glyph->SetScaleFactor(1.);                 // 缩放比例
    glyph->SetClamping(true);                  // 开启大小映射，开启后必须调用SetRange()，不然箭头大小只能在{0,1}之间映射
    glyph->SetRange(0, 5);                     // 箭头大小映射表，因为大小由向量决定，所以这里的范围应该为{0,所有点的向量模长最大值}，向量模长始终不小于0

    // 颜色
    glyph->SetColorModeToColorByScalar();           // 颜色由标量决定
    // 大小
    glyph->SetScaleModeToScaleByVector();           // 大小由向量决定
    // 方向
    glyph->SetVectorModeToUseVector();              // 方向由向量决定
    glyph->Update();

    // mapper
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetLookupTable(colorTable);
    mapper->SetScalarRange(colorTable->GetRange());
    mapper->SetInputConnection(glyph->GetOutputPort());
    mapper->ScalarVisibilityOn();

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(legendActor);
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<CustomStyle> style;
    style->SetActor(actor);
    style->SetMapper(mapper);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetWindowName("vector");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST307





#ifdef TEST21

// https://kitware.github.io/vtk-examples/site/Cxx/Points/NormalEstimation/

#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPCANormalEstimation.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>

#include <vtkPolyDataNormals.h>

namespace {
    void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph);
}

int main(int, char* [])
{
    double radius = 1.0;
    vtkNew<vtkPointSource> points;
    points->SetNumberOfPoints(1000);
    points->SetRadius(radius);
    points->SetCenter(0.0, 0.0, 0.0);
    points->SetDistributionToShell();

    double p0[3] = { 1.0, 0.0, 0.0 };
    double p1[3] = { 0.0, 1.0, 0.0 };

    vtkNew<vtkLineSource> lineSource;
    lineSource->SetPoint1(p0);
    lineSource->SetPoint2(p1);

    vtkNew<vtkPolyDataNormals> nor;

    int sampleSize = 10;
    vtkNew<vtkPCANormalEstimation> normals;
    normals->SetInputConnection(points->GetOutputPort());
    normals->SetSampleSize(sampleSize);
    normals->SetNormalOrientationToGraphTraversal();
    normals->Update();

    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkGlyph3D> glyph3D;
    MakeGlyphs(normals->GetOutput(), radius * 0.2, glyph3D.GetPointer());

    vtkNew<vtkPolyDataMapper> glyph3DMapper;
    glyph3DMapper->SetInputConnection(glyph3D->GetOutputPort());

    vtkNew<vtkActor> glyph3DActor;
    glyph3DActor->SetMapper(glyph3DMapper);
    glyph3DActor->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("Banana").GetData());

    vtkNew<vtkSphereSource> sphere;
    sphere->SetRadius(1.0);
    sphere->SetThetaResolution(41);
    sphere->SetPhiResolution(21);

    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphere->GetOutputPort());

    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("Tomato").GetData());

    // Create graphics stuff
    //
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(640, 480);
    renderWindow->SetWindowName("NormalEstimation");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    // Add the actors to the renderer, set the background and size
    //
    renderer->AddActor(glyph3DActor);
    renderer->AddActor(sphereActor);

    // Generate an interesting view
    //
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(0);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->GetActiveCamera()->Dolly(1.0);
    renderer->ResetCameraClippingRange();

    renderWindow->Render();
    interactor->Initialize();
    interactor->Start();

    return EXIT_SUCCESS;
}
namespace {
    void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph)
    {
        // Source for the glyph filter
        vtkNew<vtkArrowSource> arrow;
        arrow->SetTipResolution(16);
        arrow->SetTipLength(0.3);
        arrow->SetTipRadius(0.1);

        glyph->SetSourceConnection(arrow->GetOutputPort());
        glyph->SetInputData(src);
        glyph->SetVectorModeToUseNormal();
        glyph->SetScaleModeToScaleByVector();
        glyph->SetScaleFactor(size);
        glyph->OrientOn();
        glyph->Update();
    }
} // namespace

#endif // TEST21

#ifdef TEST10

// 01_02 TEST17

#include <vtkArrowSource.h>

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkVector.h>
#include <vtkCleanPolyData.h>
#include <vtkAlgorithmOutput.h>
#include <vtkPolyDataNormals.h>
#include <vtkCellData.h>
#include <vtkCellCenters.h>
#include <vtkTransform.h>

#include <array>

int main(int, char* [])
{
    // Points
    vtkNew<vtkPoints> points1;
    points1->InsertNextPoint(0.0, 0.0, 0.0);
    points1->InsertNextPoint(0.0, 1.0, 0.0);
    points1->InsertNextPoint(0.0, 2.0, 0.0);
    points1->InsertNextPoint(0.0, 3.0, 0.0);

    vtkNew<vtkPoints> points2;
    points2->InsertNextPoint(2.0, 0.0, 0.0);
    points2->InsertNextPoint(2.0, 1.0, 0.0);
    points2->InsertNextPoint(2.0, 2.0, 0.0);
    points2->InsertNextPoint(2.0, 3.0, 0.0);

    vtkNew<vtkPoints> points3;
    points3->InsertNextPoint(4.0, 3.0, 0.0);
    points3->InsertNextPoint(4.0, 2.0, 0.0);
    points3->InsertNextPoint(4.0, 1.0, 0.0);
    points3->InsertNextPoint(4.0, 0.0, 0.0);

    //vtkNew<vtkPoints> points4;
    //points4->InsertNextPoint(0.0, 4.0, 0.0);
    //points4->InsertNextPoint(1.0, 5.0, 0.0);
    //points4->InsertNextPoint(2.0, 6.0, 0.0);
    //points4->InsertNextPoint(3.0, 7.0, 0.0);

    vtkNew<vtkPoints> points4;
    points4->InsertNextPoint(6.0, 3.0, 0.0);
    points4->InsertNextPoint(6.0, 2.0, 0.0);
    points4->InsertNextPoint(6.0, 1.0, 0.0);
    points4->InsertNextPoint(6.0, 0.0, 0.0);

    // Cell : shared index data
    vtkNew<vtkCellArray> lines;
    lines->InsertNextCell({ 0,1 });
    lines->InsertNextCell({ 1,2 });
    lines->InsertNextCell({ 2,3 });

    // PolyData Mapper Actor
    vtkNew<vtkPolyData> poly1;
    poly1->SetPoints(points1);
    poly1->SetLines(lines);
    vtkNew<vtkPolyDataMapper> mapper1;
    mapper1->SetInputData(poly1);
    vtkNew<vtkActor> actor1;
    actor1->SetMapper(mapper1);

    vtkNew<vtkPolyData> poly2;
    poly2->SetPoints(points2);
    poly2->SetLines(lines);
    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(poly2);
    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);

    vtkNew<vtkPolyData> poly3;
    poly3->SetPoints(points3);
    poly3->SetLines(lines);
    vtkNew<vtkPolyDataMapper> mapper3;
    mapper3->SetInputData(poly3);
    vtkNew<vtkActor> actor3;
    actor3->SetMapper(mapper3);

    vtkNew<vtkPolyData> poly4;
    poly4->SetPoints(points4);
    poly4->SetLines(lines);
    vtkNew<vtkPolyDataMapper> mapper4;
    mapper4->SetInputData(poly4);
    vtkNew<vtkActor> actor4;
    actor4->SetMapper(mapper4);

    // Arrow
    vtkNew<vtkActor> glyphActor1;
    {
        vtkNew<vtkFloatArray> centerVectors;
        centerVectors->SetNumberOfComponents(3);

        double dir = -1.0;

        for (size_t i = 1; i < static_cast<size_t>(poly1->GetNumberOfPoints()); ++i)
        {
            std::array<double, 2>  startPoint = { poly1->GetPoint(i - 1)[0],poly1->GetPoint(i - 1)[1] };
            std::array<double, 2>  endPoint = { poly1->GetPoint(i)[0],poly1->GetPoint(i)[1] };

            //线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0],endPoint[1] - startPoint[1] };

            // 如果平行于y轴
            if (std::abs(lineVector[0] - 0.0) < 1e-6 && std::abs(lineVector[1] - 0.0) > 1e-6)
            {
                centerVectors->InsertNextTuple3(dir, 0.0, 0.0);
                continue;
            }

            // 如果平行于x轴
            if (std::abs(lineVector[0] - 0.0) > 1e-6 && std::abs(lineVector[1] - 0.0) < 1e-6)
            {
                centerVectors->InsertNextTuple3(0.0, dir, 0.0);
                continue;
            }

            centerVectors->InsertNextTuple3(dir, -dir * (lineVector[0] / lineVector[1]), 0.0);
        }

        vtkNew<vtkCellCenters> cellCenters;
        cellCenters->SetInputData(poly1);
        cellCenters->Update(); // 必须在调用SetVectors之前Update

        cellCenters->GetOutput()->GetPointData()->SetVectors(centerVectors);
        cellCenters->Update();

        vtkNew<vtkArrowSource> arrow;
        arrow->Update();

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetInputData(cellCenters->GetOutput());
        glyph->SetSourceConnection(arrow->GetOutputPort());
        glyph->SetVectorModeToUseVector();
        glyph->Update();

        vtkNew<vtkPolyDataMapper> glyphMapper;
        glyphMapper->SetInputConnection(glyph->GetOutputPort());
        glyphActor1->SetMapper(glyphMapper);
    }

    vtkNew<vtkActor> glyphActor2;
    {
        vtkNew<vtkFloatArray> centerVectors;
        centerVectors->SetNumberOfComponents(3);

        double dir = 1.0;

        for (size_t i = 1; i < static_cast<size_t>(poly2->GetNumberOfPoints()); ++i)
        {
            std::array<double, 2>  startPoint = { poly2->GetPoint(i - 1)[0],poly2->GetPoint(i - 1)[1] };
            std::array<double, 2>  endPoint = { poly2->GetPoint(i)[0],poly2->GetPoint(i)[1] };

            //线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0],endPoint[1] - startPoint[1] };

            // 如果平行于y轴
            if (std::abs(lineVector[0] - 0.0) < 1e-6 && std::abs(lineVector[1] - 0.0) > 1e-6)
            {
                centerVectors->InsertNextTuple3(dir, 0.0, 0.0);
                continue;
            }

            // 如果平行于x轴
            if (std::abs(lineVector[0] - 0.0) > 1e-6 && std::abs(lineVector[1] - 0.0) < 1e-6)
            {
                centerVectors->InsertNextTuple3(0.0, dir, 0.0);
                continue;
            }

            centerVectors->InsertNextTuple3(dir, -dir * (lineVector[0] / lineVector[1]), 0.0);
        }

        vtkNew<vtkCellCenters> cellCenters;
        cellCenters->SetInputData(poly2);
        cellCenters->Update(); // 必须在调用SetVectors之前Update
        cellCenters->GetOutput()->GetPointData()->SetVectors(centerVectors);

        vtkNew<vtkArrowSource> arrow;
        arrow->SetInvert(true);  // 将箭头的起始端和末端翻转
        arrow->Update();

        vtkNew<vtkTransform> transform;
        transform->RotateZ(180);
        transform->Update();

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetInputConnection(cellCenters->GetOutputPort());
        glyph->SetSourceData(arrow->GetOutput());
        glyph->SetVectorModeToUseVector();
        //glyph->SetSourceTransform(transform);
        glyph->Update();

        vtkNew<vtkPolyDataMapper> glyphMapper;
        glyphMapper->SetInputConnection(glyph->GetOutputPort());
        glyphActor2->SetMapper(glyphMapper);
    }

    vtkNew<vtkActor> glyphActor3;
    {
        vtkNew<vtkFloatArray> centerVectors;
        centerVectors->SetNumberOfComponents(3);

        double dir = -1.0;

        for (size_t i = 1; i < static_cast<size_t>(poly3->GetNumberOfPoints()); ++i)
        {
            std::array<double, 2>  startPoint = { poly3->GetPoint(i - 1)[0],poly3->GetPoint(i - 1)[1] };
            std::array<double, 2>  endPoint = { poly3->GetPoint(i)[0],poly3->GetPoint(i)[1] };

            //线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0],endPoint[1] - startPoint[1] };

            // 如果平行于y轴
            if (std::abs(lineVector[0] - 0.0) < 1e-6 && std::abs(lineVector[1] - 0.0) > 1e-6)
            {
                centerVectors->InsertNextTuple3(dir, 0.0, 0.0);
                continue;
            }

            // 如果平行于x轴
            if (std::abs(lineVector[0] - 0.0) > 1e-6 && std::abs(lineVector[1] - 0.0) < 1e-6)
            {
                centerVectors->InsertNextTuple3(0.0, dir, 0.0);
                continue;
            }

            centerVectors->InsertNextTuple3(dir, -dir * (lineVector[0] / lineVector[1]), 0.0);
        }

        vtkNew<vtkCellCenters> cellCenters;
        cellCenters->SetInputData(poly3);
        cellCenters->Update(); // 必须在调用SetVectors之前Update
        cellCenters->GetOutput()->GetPointData()->SetVectors(centerVectors);

        vtkNew<vtkArrowSource> arrow;
        arrow->Update();

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetInputConnection(cellCenters->GetOutputPort());
        glyph->SetSourceConnection(arrow->GetOutputPort());
        glyph->SetVectorModeToUseVector();
        //glyph->OrientOff();  关闭箭头和输入的向量方向相同
        glyph->Update();

        vtkNew<vtkPolyDataMapper> glyphMapper;
        glyphMapper->SetInputConnection(glyph->GetOutputPort());
        glyphActor3->SetMapper(glyphMapper);
    }

    vtkNew<vtkActor> glyphActor4;
    {
        vtkNew<vtkFloatArray> centerVectors;
        centerVectors->SetNumberOfComponents(3);

        double dir = -1.0;

        for (size_t i = 1; i < static_cast<size_t>(poly4->GetNumberOfPoints()); ++i)
        {
            std::array<double, 2>  startPoint = { poly4->GetPoint(i - 1)[0],poly4->GetPoint(i - 1)[1] };
            std::array<double, 2>  endPoint = { poly4->GetPoint(i)[0],poly4->GetPoint(i)[1] };

            //线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0],endPoint[1] - startPoint[1] };

            // 如果平行于y轴
            if (std::abs(lineVector[0] - 0.0) < 1e-6 && std::abs(lineVector[1] - 0.0) > 1e-6)
            {
                centerVectors->InsertNextTuple3(dir, 0.0, 0.0);
                continue;
            }

            // 如果平行于x轴
            if (std::abs(lineVector[0] - 0.0) > 1e-6 && std::abs(lineVector[1] - 0.0) < 1e-6)
            {
                centerVectors->InsertNextTuple3(0.0, dir, 0.0);
                continue;
            }

            centerVectors->InsertNextTuple3(dir, -dir * (lineVector[0] / lineVector[1]), 0.0);
        }

        vtkNew<vtkCellCenters> cellCenters;
        cellCenters->SetInputData(poly4);
        cellCenters->Update(); // 必须在调用SetVectors之前Update
        cellCenters->GetOutput()->GetPointData()->SetVectors(centerVectors);

        vtkNew<vtkArrowSource> arrow;
        arrow->Update();

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetInputConnection(cellCenters->GetOutputPort());
        glyph->SetSourceConnection(arrow->GetOutputPort());
        glyph->SetVectorModeToUseVector();
        //glyph->OrientOff();  关闭箭头和输入的向量方向相同
        glyph->Update();

        vtkNew<vtkPolyDataMapper> glyphMapper;
        glyphMapper->SetInputConnection(glyph->GetOutputPort());
        glyphActor4->SetMapper(glyphMapper);
    }

    // Renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor1);
    renderer->AddActor(actor2);
    renderer->AddActor(actor3);
    renderer->AddActor(actor4);

    renderer->AddActor(glyphActor1);
    renderer->AddActor(glyphActor2);
    renderer->AddActor(glyphActor3);
    renderer->AddActor(glyphActor4);

    // Window
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->SetBackground(.1, .2, .3);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST10

#ifdef TEST40

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkCellDataToPointData.h>
#include <vtkDataSetMapper.h>
#include <vtkContourFilter.h>

int main(int, char* [])
{
    // 顶点
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (size_t i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, 0, 0);
        points->InsertNextPoint(i + 1, 1, 0);
        points->InsertNextPoint(i, 2, 0);
        points->InsertNextPoint(i + 1, 3, 0);
    }

    // 拓扑
    vtkSmartPointer<vtkCellArray> cell_poly = vtkSmartPointer<vtkCellArray>::New();
    for (long long i = 0; i < 33; i += 4)
    {
        cell_poly->InsertNextCell({ i,i + 1,i + 5,i + 4 });
        cell_poly->InsertNextCell({ i + 1,i + 2,i + 6,i + 5 });
        cell_poly->InsertNextCell({ i + 2,i + 3,i + 7,i + 6 });
    }

    // 标量
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
    scalars->SetNumberOfValues(27);
    for (int i = 0; i < 27; i++)
    {
        scalars->SetValue(i, i);
    }

    vtkSmartPointer<vtkPolyData> poly1 = vtkSmartPointer<vtkPolyData>::New();
    poly1->SetPoints(points);
    poly1->SetPolys(cell_poly);
    poly1->GetCellData()->SetScalars(scalars);

    // 单元标量数据转为顶点数据
    vtkNew<vtkCellDataToPointData> cellToPoint;
    cellToPoint->SetInputData(poly1);
    cellToPoint->PassCellDataOff();
    cellToPoint->Update(); //updata不能少

    vtkNew<vtkContourFilter> contourFilter;
    contourFilter->SetInputData(cellToPoint->GetOutput());
    contourFilter->GenerateValues(50, 0, 26);
    contourFilter->Update();

    // 创建颜色查找表
    vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
    hueLut->SetHueRange(0.67, 0.0);            // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    hueLut->Build();

    vtkSmartPointer<vtkDataSetMapper> mapper1 = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper1->SetInputData(cellToPoint->GetOutput());
    mapper1->SetInputData(contourFilter->GetOutput());
    mapper1->SetScalarRange(0, 26);            // 设置标量值的范围
    mapper1->SetLookupTable(hueLut);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper1);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    renderer->SetBackground(.1, .2, .3);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(actor);

    renderWindow->SetSize(600, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST40

#ifdef TEST42

#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellCenters.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNew.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <algorithm>
#include <vector>

int main(int argc, char* argv[])
{
    vtkNew<vtkUnstructuredGridReader> reader;
    reader->SetFileName("elbow.vtk");
    reader->Update();

    // 获取三个方向的最大最小值
    std::vector<double> bounds(6);
    reader->GetOutput()->GetBounds(bounds.data());

    // 获取三个方向的范围值
    std::vector<double> range(3);
    for (int i = 0; i < 3; ++i)
    {
        range[i] = bounds[2 * i + 1] - bounds[2 * i];
    }

    vtkNew<vtkCellCenters> centers;
    centers->SetInputData(reader->GetOutput());
    centers->Update();

    // 对于vtkCellCenters过滤器，最后的输出只存在节点，不存在单元
    // 获取单元中心的坐标值
    // vtkPolyData* cellCenter = centers->GetOutput();
    // for (vtkIdType pointIndex = 0; pointIndex < cellCenter->GetNumberOfPoints(); ++pointIndex)
    // {
    //     double pointCoord[3];
    //     cellCenter->GetPoint(pointIndex, pointCoord);
    //     std::cout << "单元 " << pointIndex << " 的中心的坐标为：" << std::endl
    //               << "X : " << pointCoord[0] << std::endl
    //               << "Y : " << pointCoord[1] << std::endl
    //               << "Z : " << pointCoord[2] << std::endl;
    // }

    // 获取三个方向范围的最大值
    double maxValue = *std::max_element(range.begin(), range.end());
    double r = maxValue * 0.001;

    // 用球体来表示单元中心
    vtkNew<vtkSphereSource> sphere;
    sphere->SetPhiResolution(11);
    sphere->SetThetaResolution(11);
    sphere->SetRadius(r);

    vtkNew<vtkGlyph3DMapper> pointMapper;
    pointMapper->SetInputConnection(centers->GetOutputPort());
    // 以球的形式表现单元中心
    pointMapper->SetSourceConnection(sphere->GetOutputPort());

    vtkNew<vtkActor> pointActor;
    pointActor->SetMapper(pointMapper);

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(reader->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.5);
    actor->GetProperty()->EdgeVisibilityOn(); // 边界线
    actor->GetProperty()->SetInterpolationToFlat();

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->AddActor(actor);
    renderer->AddActor(pointActor);

    renderer->GradientBackgroundOn();
    renderer->SetBackground(1, 1, 1);
    renderer->SetBackground2(0.4, 0.55, 0.75);
    renderer->ResetCamera();

    renderWindow->Render();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
#endif // TEST42
