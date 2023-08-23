/*
 * 100 上下左右四视图，格点、格心数据，线框、面显示模式的区别
 * 101 色卡 vtkScalarBarActor 颜色查找表 vtkLookupTable
 * 102 vtkScalarBarWidget 拖动色卡，获取色卡色带部分的具体位置，用来标记某一个颜色条
 * 103 scalars范围超出lookuptable的范围时，超出部分不显示或指定颜色
 * 104 自定义色卡的标签，解决因为double的精度导致标签计算错误
 * 105 给vtkPolyData添加多个vtkDataArray，指定某个Array映射颜色
 *
 * 300 vtkGlyph3D 官方示例 矢量图(箭头)
 * 301 vtkGlyph2D 官方示例 vtkGlyph2D继承自vtkGlyph3D
 * 302 vtkGlyph3D 常用函数，设置标签的大小、方向、颜色
 * 303 vtkGlyph3DMapper vtkGlyph3D 在GPU上计算的版本
 *
 * 306 矢量图标签大小范围设置，避免标签图形过大或过小
 * 307 动态开启关闭矢量图颜色映射
 * 308 官方例子，球面法向量 矢量图
 * 309 线框网格矢量图箭头方向，箭头起始段末端翻转，闭合多边形的内法线、外法线
 *
 * 500 粒子追踪（迹线）
 * 501 拉格朗日粒子追踪，官方示例
 * 502 获取生成的迹线（粒子追踪）对应的标量/矢量数据
 *
 * 600 OpenFOAM 文件读取
 * 601 vtkStreamTracer 流线图 展示流体流动的轨迹和流动方向
 * 602 流线、流管、流面
 * 603 流线生成需要的数据
 *
 * 701 vtkContourFilter 等值面 vtkSampleFunction使用采样函数提取等值面
 * 702 vtkFlyingEdges3D 从体素数据提取等值面(isosurfaces)
 * 703 vtkContourTriangulator 将等值线包裹的区域转换为三角形网格
 * 704 vtkBandedPolyDataContourFilter 生成带状轮廓，将顶点的数据(Scalars)处于某一指定范围的区域生成多个单元（多边形带）
 */

#define TEST704

#ifdef TEST100

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
#include <vtkSTLReader.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkViewport.h>

int main(int, char*[])
{
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
        cell_poly->InsertNextCell({ i + 0, i + 1, i + 5, i + 4 });
        cell_poly->InsertNextCell({ i + 1, i + 2, i + 6, i + 5 });
        cell_poly->InsertNextCell({ i + 2, i + 3, i + 7, i + 6 });
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
        cell_line->InsertNextCell({ i + 0, i + 4 });
        cell_line->InsertNextCell({ i + 1, i + 5 });
        cell_line->InsertNextCell({ i + 2, i + 6 });
        cell_line->InsertNextCell({ i + 3, i + 7 });
    }

    const int numPoints = 40;
    const int numPolys  = 27;
    const int numLines  = 66;

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
    // actorPolyPoint->GetProperty()->SetRepresentationToSurface();
    // actorPolyPoint->GetProperty()->SetRepresentationToPoints();
    // actorPolyCell->GetProperty()->SetRepresentationToWireframe();
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

    return 0;
}

#endif // TEST100

#ifdef TEST101

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
        -8.f, 8.f, 8.f, -8.f
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

    // 指定颜色查找表中有多少种颜色，默认256
    hueLut->SetNumberOfColors(numOfColors);
    hueLut->SetNumberOfTableValues(numOfColors);
    // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    // hueLut->SetHueRange(0.0,0.67);           // 设置色调范围
    // hueLut->SetSaturationRange(1.0, 1.0);    // 设置饱和度范围 0表示灰度图，1表示彩色图
    // hueLut->SetValueRange(1.0, 1.0);         // 设置亮度范围
    // hueLut->SetRampToSQRT();                 // 设置颜色插值方式为SQRT
    // hueLut->SetRampToLinear();               // 设置颜色插值方式为线性
    // hueLut->SetRampToSCurve();               // 设置颜色插值方式为S曲线
    // hueLut->UseAboveRangeColorOn();
    // hueLut->SetAboveRangeColor(0.0, 1.0, 0.0, 0.0);
    // hueLut->SetBelowRangeColor(1.0, 1.0, 0.0, 0.0);
    // hueLut->SetNumberOfTableValues(20);
    // hueLut->SetAlphaRange(1.0, 1.0);
    // hueLut->SetValueRange(1, 1);
    // hueLut->SetRange(GenPolyData()->GetPointData()->GetScalars()->GetRange());
    // hueLut->Build();

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
    // 设置色卡最大颜色分段个数，如果LookupTable大于该数，也只能显示MaximumNumberOfColors个颜色。太小会导致两头的颜色不显示，默认64
    scalarBar->SetMaximumNumberOfColors(numOfColors);
    // 设置色卡最大占多少像素
    // scalarBar->SetMaximumWidthInPixels(80);
    // scalarBar->SetMaximumHeightInPixels(900);

    //
    // vtkScalarBarActor::ConfigureScalarBar()在该函数中设置色卡的每一个颜色时，
    // 可能会因为浮点数精度问题导致设置的颜色错误，即两个相邻的颜色值相同
    // 修改：rgbval = range[0] + (range[1] - range[0]) * (i / static_cast<double>(this->P->NumColors));
    // rgbval = range[0] + (range[1] - range[0]) * ((i + 0.5) / static_cast<double>(this->P->NumColors));
    // 继承一个vtkScalarBarActor重写该方法即可
    //

    //---------------------------------------------------------
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

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

#endif // TEST101

#ifdef TEST102

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
    // clang-format off
    std::vector<float> vertices { 
        0., 0., 0., 1., 0., 2., 0., 3.,
        1., 3., 1., 2., 1., 1., 1., 0.,
        2., 0., 2., 1., 2., 2., 2., 3.,
        3., 3., 3., 2., 3., 1., 3., 0.,
    };

    std::vector<int> indicesPoly {
         0,         1,         6,         7,
         1,         2,         5,         6,
         2,         3,         4,         5,
         4,         5,        10,        11,
         5,         6,         9,        10,
         6,         7,         8,         9,
         8,         9,        14,        15,
         9,        10,        13,        14,
        10,        11,        12,        13,
    };

    std::vector<float> fields {
        1.0f,        1.0f,        1.0f,        1.0f,
        2.0f,        2.0f,        2.0f,        2.0f,
        3.0f,        3.0f,        3.0f,        3.0f,
        4.0f,        4.0f,        4.0f,        4.0f,
    };
    // clang-format on

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
            cellsPoly->InsertNextCell({ indicesPoly[i], indicesPoly[i + 1], indicesPoly[i + 2], indicesPoly[i + 3] });
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
    hueLut->SetNumberOfColors(10);
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->SetRange(GenPolyData()->GetPointData()->GetScalars()->GetRange());
    hueLut->Build();

    //---------------------------------------------------------
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(GenPolyData());
    mapper->SetScalarRange(mapper->GetInput()->GetPointData()->GetScalars()->GetRange()); // 设置标量值的范围
    mapper->ScalarVisibilityOn();                                                         // 默认开启标量属性显示
    mapper->SetLookupTable(hueLut);

    //---------------------------------------------------------
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

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

    renderer->AddActor(actor);
    // renderer->AddActor2D(scalarBar);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST102

#ifdef TEST103

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
    vtkNew<vtkPoints> points;
    vtkNew<vtkFloatArray> scalars;
    for (size_t i = 0; i < 10; ++i)
    {
        points->InsertNextPoint(static_cast<double>(i), 0.0, 0.0);
        points->InsertNextPoint(static_cast<double>(i), 1.0, 0.0);

        scalars->InsertNextValue(static_cast<float>(i));
        scalars->InsertNextValue(static_cast<float>(i));
    }

    vtkNew<vtkCellArray> cells;
    for (vtkIdType i = 0; i < 17; i += 2)
    {
        cells->InsertNextCell({ i + 0, i + 1, i + 3, i + 2 });
    }

    // polyData
    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetPointData()->SetScalars(scalars);

    // lookup table
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->SetRange(scalars->GetRange()[0], scalars->GetRange()[1] / 2.0);

    // 设置超出部分的颜色，将alpha设置为0.0就可以实现超出部分不显示
    hueLut->UseBelowRangeColorOn();
    hueLut->SetBelowRangeColor(1., 1., 1., 1.);
    hueLut->UseAboveRangeColorOn();
    hueLut->SetAboveRangeColor(1., 1., 1., 1.);

    hueLut->Build();

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->SetLookupTable(hueLut);
    mapper->SetScalarRange(scalars->GetRange()[0], scalars->GetRange()[1] / 2.0);
    mapper->ScalarVisibilityOn();
    mapper->InterpolateScalarsBeforeMappingOn();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1, 1, 1);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);
    window->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(window);

    window->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST103

#ifdef TEST104

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

#endif // TEST104

#ifdef TEST105

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
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
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkDoubleArray> pressure_scalars;
    pressure_scalars->SetName("pressure");
    pressure_scalars->InsertNextValue(0.0);
    pressure_scalars->InsertNextValue(1.0);
    pressure_scalars->InsertNextValue(1.0);
    pressure_scalars->InsertNextValue(0.0);

    vtkNew<vtkDoubleArray> density_scalars;
    density_scalars->SetName("density");
    density_scalars->InsertNextValue(0.0);
    density_scalars->InsertNextValue(0.0);
    density_scalars->InsertNextValue(2.0);
    density_scalars->InsertNextValue(2.0);

    double pressure_range[2] { 0.0, 1.0 };
    double density_range[2] { 0.0, 2.0 };

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetPointData()->AddArray(pressure_scalars);
    polyData->GetPointData()->AddArray(density_scalars);

    polyData->Print(std::cout);

    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetNumberOfColors(20);
    hueLut->SetNumberOfTableValues(20);
    hueLut->SetHueRange(0.0, 0.67);
    hueLut->SetRange(pressure_range);
    // hueLut->SetRange(density_range);
    hueLut->Build();

    //---------------------------------------------------------
    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLookupTable(hueLut);
    scalarBar->SetMaximumNumberOfColors(20);

    // ScalarMode 是 UsePointFieldData 或 UseCellFieldData时，才能使用SelectColorArray()设置颜色
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->ScalarVisibilityOn();
    mapper->SetLookupTable(hueLut);
    mapper->InterpolateScalarsBeforeMappingOn();
    mapper->SetScalarModeToUsePointFieldData();
    mapper->SetScalarRange(pressure_range);
    mapper->SelectColorArray("pressure");
    // mapper->SetScalarRange(density_range);
    // mapper->SelectColorArray("density");

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

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

#endif // TEST105

#ifdef TEST300

// https://kitware.github.io/vtk-examples/site/Cxx/Filtering/Glyph3D/
// https://kitware.github.io/vtk-examples/site/Cxx/Points/NormalEstimation/ 标记球面法向量

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

int main(int, char*[])
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

// https://examples.vtk.org/site/Cxx/Filtering/Glyph2D/

#include <vtkActor.h>
#include <vtkArrowSource.h>
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

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 1, 0);
    points->InsertNextPoint(2, 2, 0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    // Create anything you want here, we will use a polygon for the demo.
    vtkNew<vtkRegularPolygonSource> source; // default is 6 sides
    // vtkNew<vtkArrowSource> source;

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

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>

int main(int, char*[])
{
    // 顶点数据，即矢量图位置
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    // 每一个顶点的标量
    vtkNew<vtkDoubleArray> pointScalars;
    pointScalars->InsertNextValue(1.);
    pointScalars->InsertNextValue(4.);
    pointScalars->InsertNextValue(3.);
    pointScalars->InsertNextValue(2.);

    // 每一个顶点的向量
    vtkNew<vtkDoubleArray> pointVectors;
    pointVectors->SetNumberOfComponents(3); // 3维向量 {x,y,z}
    pointVectors->InsertNextTuple3(5.0, 0.0, 0.0);
    pointVectors->InsertNextTuple3(0.0, 3.0, 0.0);
    pointVectors->InsertNextTuple3(1.0, 1.0, 1.0);
    pointVectors->InsertNextTuple3(-1.0, -1.0, 0.0);

    // 每一个顶点的法线
    vtkNew<vtkDoubleArray> pointNormals;
    pointNormals->SetNumberOfComponents(3);
    pointNormals->InsertNextTuple3(0., 0., 10.);
    pointNormals->InsertNextTuple3(0., 0., -1.);
    pointNormals->InsertNextTuple3(0., 0., 1.);
    pointNormals->InsertNextTuple3(0., 0., -10.);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);
    polydata->GetPointData()->SetScalars(pointScalars);
    polydata->GetPointData()->SetVectors(pointVectors);
    polydata->GetPointData()->SetNormals(pointNormals);

    //------------------------------------------------------------------------
    // 矢量图的图案，可以使用任意图形标记
    vtkNew<vtkArrowSource> source;
    source->Update();

    // 颜色映射表
    vtkNew<vtkLookupTable> colorTable;
    colorTable->SetHueRange(0.67, 0.0);
    colorTable->SetRange(1, 4);
    colorTable->Build();

    // 色卡
    vtkNew<vtkScalarBarActor> legendActor;
    legendActor->SetNumberOfLabels(20);
    legendActor->SetLookupTable(colorTable);
    legendActor->SetMaximumNumberOfColors(20);

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputData(polydata);             // 顶点数据，即矢量图每一个图案的起始位置
    glyph->SetSourceData(source->GetOutput()); // 资源数据，即矢量图的图案
    glyph->SetScaleFactor(1.);                 // 缩放比例
    // glyph->SetClamping(true); // 设置数据是否被限制在某一范围，避免数据出现极端的情况（过大或过小）
    // glyph->SetRange(0, 1);

    // 默认颜色由标量决定
    // 默认大小由标量决定
    // 默认方向由向量决定
    std::cout << "--------------------------------------------"
              << "\ncolor:\t" << glyph->GetColorModeAsString() << "\nscale:\t" << glyph->GetScaleModeAsString() << "\nvector:\t"
              << glyph->GetVectorModeAsString() << '\n';

    //------------------------------------------------------------------------
    // 颜色
    glyph->SetColorModeToColorByScalar(); // 颜色由标量决定
    // glyph->SetColorModeToColorByScale();             // 颜色由标量决定
    // glyph->SetColorModeToColorByVector();            // 颜色由向量决定

    //------------------------------------------------------------------------
    // 大小
    glyph->SetScaleModeToScaleByScalar(); // 大小由标量决定
    // glyph->SetScaleModeToDataScalingOff();          // 关闭由于数据（标量或向量）导致的缩放，所有矢量图的图案大小一致
    // glyph->SetScaleModeToScaleByVectorComponents(); // 画出来的箭头有的是平面的，有的没有显示
    // glyph->SetScaleModeToScaleByVector();           // 大小由向量决定

    //------------------------------------------------------------------------
    // 方向
    glyph->SetVectorModeToUseVector(); // 方向由向量决定
    // glyph->SetVectorModeToFollowCameraDirection();  // 方向跟随相机
    // glyph->SetVectorModeToUseNormal();              // 方向由法线决定
    // glyph->SetVectorModeToVectorRotationOff();      // 关闭跟随向量旋转
    // glyph->OrientOff();                             // 禁用输入几何体沿向量/法线的方向

    std::cout << "--------------------------------------------"
              << "\ncolor:\t" << glyph->GetColorModeAsString() << "\nscale:\t" << glyph->GetScaleModeAsString() << "\nvector:\t"
              << glyph->GetVectorModeAsString() << '\n';

    //------------------------------------------------------------------------
    // mapper
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetLookupTable(colorTable);
    mapper->SetScalarRange(colorTable->GetRange());
    mapper->SetInputConnection(glyph->GetOutputPort());

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

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST302

#ifdef TEST303

// https://kitware.github.io/vtk-examples/site/Cxx/Visualization/Glyph3DMapper/

#include <vtkActor.h>
#include <vtkArrowSource.h>
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

int main(int, char*[])
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

    // vtkNew<vtkCubeSource> source;
    vtkNew<vtkArrowSource> source;

    vtkNew<vtkGlyph3DMapper> glyph3Dmapper;
    glyph3Dmapper->SetSourceConnection(source->GetOutputPort());
    glyph3Dmapper->SetInputData(polydata);
    glyph3Dmapper->OrientOn();                              // 开启方向设置
    glyph3Dmapper->SetOrientationArray("Colors");           // 设置方向
    glyph3Dmapper->SetScalarModeToUsePointFieldData();      // 颜色才能生效
    glyph3Dmapper->SetScaleArray("Scale Factors");          // 设置大小
    glyph3Dmapper->SetScaleModeToScaleByVectorComponents(); // 大小才能生效
    glyph3Dmapper->SelectColorArray("Colors");              // 设置颜色
    glyph3Dmapper->Update();

    vtkNew<vtkActor> actor;
    actor->SetMapper(glyph3Dmapper);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("Glyph3DMapper");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST303

#ifdef TEST306

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph2D.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkLookupTable.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>

int main(int, char*[])
{
    // 矢量图的图案
    vtkNew<vtkArrowSource> source;
    source->Update();

    // 颜色映射表
    vtkNew<vtkLookupTable> colorTable;
    colorTable->SetNumberOfColors(20);
    colorTable->SetHueRange(.0, .67);
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
    glyph->SetRange(0, 5); // 箭头大小映射范围，如果实际的范围超出这个范围，则箭头大小会被压缩到这个范围内，避免某些箭头过大或过小
    glyph->SetColorModeToColorByScalar(); // 颜色由标量决定
    glyph->SetScaleModeToScaleByVector(); // 大小由向量决定
    glyph->SetVectorModeToUseVector();    // 方向由向量决定
    glyph->Update();

    double range[2] { 0 };
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

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>

class CustomStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleTrackballCamera);

protected:
    void OnLeftButtonUp() override
    {
        Superclass::OnLeftButtonUp();

        if (this->Interactor && m_actor)
        {
            std::cout << "visibility off\n";
            m_actor->GetProperty()->SetColor(1, 1, 1);
            m_actor->GetMapper()->ScalarVisibilityOff();
            this->Interactor->Render();
        }
    }

    void OnRightButtonUp() override
    {
        Superclass::OnRightButtonUp();

        if (this->Interactor && m_actor)
        {
            std::cout << "visibility on\n";
            m_actor->GetMapper()->ScalarVisibilityOn();
            this->Interactor->Render();
        }
    }

public:
    void SetActor(const vtkSmartPointer<vtkActor>& actor)
    {
        m_actor = actor;
    }

private:
    vtkSmartPointer<vtkActor> m_actor { nullptr };
};

vtkStandardNewMacro(CustomStyle);

int main(int, char*[])
{
    // 矢量图的图案
    vtkNew<vtkArrowSource> source;
    source->Update();

    // 颜色映射表
    vtkNew<vtkLookupTable> colorTable;
    colorTable->SetNumberOfColors(20);
    colorTable->SetHueRange(.0, .67);
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

    // 每一个顶点的标量
    vtkNew<vtkDoubleArray> pointScalars;
    pointScalars->InsertNextValue(1.);
    pointScalars->InsertNextValue(4.);
    pointScalars->InsertNextValue(3.);
    pointScalars->InsertNextValue(2.);

    // 每一个顶点的向量
    vtkNew<vtkDoubleArray> pointVectors;
    pointVectors->SetNumberOfComponents(3); // 3维向量 {x,y,z}
    pointVectors->InsertNextTuple3(5.0, 0.0, 0.0);
    pointVectors->InsertNextTuple3(0.0, 3.0, 0.0);
    pointVectors->InsertNextTuple3(1.0, 1.0, 1.0);
    pointVectors->InsertNextTuple3(-1.0, -1.0, 0.0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);
    polydata->GetPointData()->SetScalars(pointScalars);
    polydata->GetPointData()->SetVectors(pointVectors);

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputData(polydata);             // 顶点数据，即矢量图每一个图案的位置
    glyph->SetSourceData(source->GetOutput()); // 资源数据，即矢量图的图案
    glyph->SetColorModeToColorByScalar();      // 颜色由标量决定
    glyph->SetScaleModeToScaleByVector();      // 大小由向量决定
    glyph->SetVectorModeToUseVector();         // 方向由向量决定
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
    renderWindow->SetWindowName("vector");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<CustomStyle> style;
    style->SetActor(actor);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST307

#ifdef TEST309

#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkArrowSource.h>
#include <vtkCellArray.h>
#include <vtkCellCenters.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3D.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkVector.h>

#include <array>

int main(int, char*[])
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

    vtkNew<vtkPoints> points4;
    points4->InsertNextPoint(6.0, 3.0, 0.0);
    points4->InsertNextPoint(6.0, 2.0, 0.0);
    points4->InsertNextPoint(6.0, 1.0, 0.0);
    points4->InsertNextPoint(6.0, 0.0, 0.0);

    // Cell : shared index data
    vtkNew<vtkCellArray> lines;
    lines->InsertNextCell({ 0, 1 });
    lines->InsertNextCell({ 1, 2 });
    lines->InsertNextCell({ 2, 3 });

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
            std::array<double, 2> startPoint = { poly1->GetPoint(i - 1)[0], poly1->GetPoint(i - 1)[1] };
            std::array<double, 2> endPoint   = { poly1->GetPoint(i)[0], poly1->GetPoint(i)[1] };

            // 线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0], endPoint[1] - startPoint[1] };

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
            std::array<double, 2> startPoint = { poly2->GetPoint(i - 1)[0], poly2->GetPoint(i - 1)[1] };
            std::array<double, 2> endPoint   = { poly2->GetPoint(i)[0], poly2->GetPoint(i)[1] };

            // 线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0], endPoint[1] - startPoint[1] };

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
        arrow->SetInvert(true); // 将箭头的起始端和末端翻转
        arrow->Update();

        vtkNew<vtkTransform> transform;
        transform->RotateZ(180);
        transform->Update();

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetInputConnection(cellCenters->GetOutputPort());
        glyph->SetSourceData(arrow->GetOutput());
        glyph->SetVectorModeToUseVector();
        // glyph->SetSourceTransform(transform);
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
            std::array<double, 2> startPoint = { poly3->GetPoint(i - 1)[0], poly3->GetPoint(i - 1)[1] };
            std::array<double, 2> endPoint   = { poly3->GetPoint(i)[0], poly3->GetPoint(i)[1] };

            // 线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0], endPoint[1] - startPoint[1] };

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
        // glyph->OrientOff();  关闭箭头和输入的向量方向相同
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
            std::array<double, 2> startPoint = { poly4->GetPoint(i - 1)[0], poly4->GetPoint(i - 1)[1] };
            std::array<double, 2> endPoint   = { poly4->GetPoint(i)[0], poly4->GetPoint(i)[1] };

            // 线段的方向
            std::array<double, 2> lineVector = { endPoint[0] - startPoint[0], endPoint[1] - startPoint[1] };

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
        // glyph->OrientOff();  关闭箭头和输入的向量方向相同
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

#endif // TEST309

#ifdef TEST500

#include "vtkActor.h"
#include "vtkCellData.h"
#include "vtkDataSetMapper.h"
#include "vtkDoubleArray.h"
#include "vtkImageData.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkLagrangianMatidaIntegrationModel.h"
#include "vtkLagrangianParticleTracker.h"
#include "vtkPointData.h"
#include "vtkPointSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRTAnalyticSource.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRungeKutta2.h"
#include "vtkSphereSource.h"

int main(int, char*[])
{
    //--------------------------------------------------------------------
    // 种子点数据
    vtkNew<vtkPointSource> seeds;
    seeds->SetNumberOfPoints(10);
    seeds->SetRadius(4);
    seeds->Update();

    vtkPolyData* seedPolyData = seeds->GetOutput();

    vtkNew<vtkDoubleArray> partVel;
    partVel->SetNumberOfComponents(3); // tuple3
    partVel->SetNumberOfTuples(seedPolyData->GetNumberOfPoints());
    partVel->SetName("InitialVelocity");
    partVel->FillComponent(0, 2); // 将每个tuple3的第0个分量设置为2，每一个tuple3的值都一样
    partVel->FillComponent(1, 5); // 将每个tuple3的第0个分量设置为5
    partVel->FillComponent(2, 1); // 将每个tuple3的第0个分量设置为1

    vtkNew<vtkDoubleArray> partDens;
    partDens->SetNumberOfComponents(1);
    partDens->SetNumberOfTuples(seedPolyData->GetNumberOfPoints());
    partDens->SetName("ParticleDensity");
    partDens->FillComponent(0, 1920);

    vtkNew<vtkDoubleArray> partDiam;
    partDiam->SetNumberOfComponents(1);
    partDiam->SetNumberOfTuples(seedPolyData->GetNumberOfPoints());
    partDiam->SetName("ParticleDiameter");
    partDiam->FillComponent(0, 0.1);

    seedPolyData->GetPointData()->AddArray(partVel);
    seedPolyData->GetPointData()->AddArray(partDens);
    seedPolyData->GetPointData()->AddArray(partDiam);

    //---------------------------------------------------------------------------------
    // 网格数据
    vtkNew<vtkRTAnalyticSource> wavelet;
    wavelet->Update();

    vtkImageData* waveletImg = wavelet->GetOutput();

    std::cout << waveletImg->GetNumberOfCells() << '\t' << waveletImg->GetNumberOfPoints() << '\n';

    // 设置流场数据
    vtkNew<vtkDoubleArray> flowVel;
    flowVel->SetNumberOfComponents(3);
    flowVel->SetNumberOfTuples(waveletImg->GetNumberOfCells());
    flowVel->SetName("FlowVelocity");
    flowVel->FillComponent(0, -0.3);
    flowVel->FillComponent(1, -0.3);
    flowVel->FillComponent(2, -0.3);

    vtkNew<vtkDoubleArray> flowDens;
    flowDens->SetNumberOfComponents(1);
    flowDens->SetNumberOfTuples(waveletImg->GetNumberOfCells());
    flowDens->SetName("FlowDensity");
    flowDens->FillComponent(0, 1000);

    vtkNew<vtkDoubleArray> flowDynVisc;
    flowDynVisc->SetNumberOfComponents(1);
    flowDynVisc->SetNumberOfTuples(waveletImg->GetNumberOfCells());
    flowDynVisc->SetName("FlowDynamicViscosity");
    flowDynVisc->FillComponent(0, 0.894);

    // 给每一个单元设置速度、压力、流动阻力
    waveletImg->GetCellData()->AddArray(flowVel);
    waveletImg->GetCellData()->AddArray(flowDens);
    waveletImg->GetCellData()->AddArray(flowDynVisc);

    vtkNew<vtkRungeKutta2> integrator;
    vtkNew<vtkLagrangianMatidaIntegrationModel> integrationModel;
    integrationModel->SetInputArrayToProcess(0, 1, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "InitialVelocity");
    integrationModel->SetInputArrayToProcess(3, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "FlowVelocity");
    integrationModel->SetInputArrayToProcess(4, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "FlowDensity");
    integrationModel->SetInputArrayToProcess(5, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "FlowDynamicViscosity");
    integrationModel->SetInputArrayToProcess(6, 1, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "ParticleDiameter");
    integrationModel->SetInputArrayToProcess(7, 1, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "ParticleDensity");

    vtkNew<vtkLagrangianParticleTracker> tracker;
    tracker->SetIntegrator(integrator);             // 设置求解器
    tracker->SetIntegrationModel(integrationModel); // 积分模型
    tracker->SetStepFactor(0.1);                    // 积分步长
    tracker->SetStepFactorMin(0.1);                 // 最小积分步长
    tracker->SetStepFactorMax(0.1);                 // 最大积分步长
    tracker->SetCellLengthComputationMode(vtkLagrangianParticleTracker::STEP_CUR_CELL_VEL_DIR);
    tracker->AdaptiveStepReintegrationOn();   // 是否自适应步长，默认false
    tracker->GenerateParticlePathsOutputOn(); // 是否生成粒子路径（迹线），默认true
    tracker->SetMaximumIntegrationTime(10.0); // 最大生成时间，默认-1，没有时间限制
    tracker->SetMaximumNumberOfSteps(300);    // 最大步数，默认100，-1表示没有限制
    tracker->SetInputData(waveletImg);        // 网格
    tracker->SetSourceData(seedPolyData);     // 种子点
    tracker->Update();

    // tracker->Print(std::cout);
    // tracker->GetOutput()->Print(std::cout);

    //----------------------------------------------------------------------
    // 绘制迹线
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(vtkPolyData::SafeDownCast(tracker->GetOutput()));
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST500

#ifdef TEST501

// https://gitlab.kitware.com/vtk/vtk/-/blob/v9.2.0/Filters/FlowPaths/Testing/Cxx/TestLagrangianParticleTracker.cxx

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkDataSetMapper.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkDoubleArray.h"
#include "vtkGlyph3D.h"
#include "vtkImageData.h"
#include "vtkImageDataToPointSet.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkLagrangianMatidaIntegrationModel.h"
#include "vtkLagrangianParticleTracker.h"
#include "vtkMath.h"
#include "vtkMultiBlockDataGroupFilter.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkNew.h"
#include "vtkPlaneSource.h"
#include "vtkPointData.h"
#include "vtkPointSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRTAnalyticSource.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRungeKutta2.h"
#include "vtkSphereSource.h"

int main(int, char*[])
{
    // Create a point source
    vtkNew<vtkPointSource> seeds;
    seeds->SetNumberOfPoints(10);
    seeds->SetRadius(4);
    seeds->Update();

    vtkPolyData* seedPolyData = seeds->GetOutput();
    vtkPointData* seedData    = seedPolyData->GetPointData();
    auto numOfPoints          = seedPolyData->GetNumberOfPoints();

    // Create seed data
    vtkNew<vtkDoubleArray> partVel;
    partVel->SetNumberOfComponents(3);
    partVel->SetNumberOfTuples(numOfPoints);
    partVel->SetName("InitialVelocity");
    partVel->FillComponent(0, 2);
    partVel->FillComponent(1, 5);
    partVel->FillComponent(2, 1);

    vtkNew<vtkDoubleArray> partDens;
    partDens->SetNumberOfComponents(1);
    partDens->SetNumberOfTuples(numOfPoints);
    partDens->SetName("ParticleDensity");
    partDens->FillComponent(0, 1920);

    vtkNew<vtkDoubleArray> partDiam;
    partDiam->SetNumberOfComponents(1);
    partDiam->SetNumberOfTuples(numOfPoints);
    partDiam->SetName("ParticleDiameter");
    partDiam->FillComponent(0, 0.1);

    seedData->AddArray(partVel);
    seedData->AddArray(partDens);
    seedData->AddArray(partDiam);

    // Create a wavelet
    vtkNew<vtkRTAnalyticSource> wavelet;
    wavelet->Update();
    vtkImageData* waveletImg = wavelet->GetOutput();

    vtkCellData* cd = waveletImg->GetCellData();

    // Create flow data
    vtkNew<vtkDoubleArray> flowVel;
    flowVel->SetNumberOfComponents(3);
    flowVel->SetNumberOfTuples(waveletImg->GetNumberOfCells());
    flowVel->SetName("FlowVelocity");

    vtkNew<vtkDoubleArray> flowDens;
    flowDens->SetNumberOfComponents(1);
    flowDens->SetNumberOfTuples(waveletImg->GetNumberOfCells());
    flowDens->SetName("FlowDensity");

    vtkNew<vtkDoubleArray> flowDynVisc;
    flowDynVisc->SetNumberOfComponents(1);
    flowDynVisc->SetNumberOfTuples(waveletImg->GetNumberOfCells());
    flowDynVisc->SetName("FlowDynamicViscosity");

    flowVel->FillComponent(0, -0.3);
    flowVel->FillComponent(1, -0.3);
    flowVel->FillComponent(2, -0.3);
    flowDens->FillComponent(0, 1000);
    flowDynVisc->FillComponent(0, 0.894);

    cd->AddArray(flowVel);
    cd->AddArray(flowDens);
    cd->AddArray(flowDynVisc);

    // Create surface
    vtkNew<vtkDataSetSurfaceFilter> surface;
    surface->SetInputConnection(wavelet->GetOutputPort());
    surface->Update();
    vtkPolyData* surfacePd = surface->GetOutput();

    // Create Surface data
    vtkNew<vtkDoubleArray> surfaceTypeTerm;
    surfaceTypeTerm->SetNumberOfComponents(1);
    surfaceTypeTerm->SetName("SurfaceType");
    surfaceTypeTerm->SetNumberOfTuples(surfacePd->GetNumberOfCells());
    surfaceTypeTerm->FillComponent(0, vtkLagrangianBasicIntegrationModel::SURFACE_TYPE_TERM);
    surfacePd->GetCellData()->AddArray(surfaceTypeTerm);

    // Create plane passThrough
    vtkNew<vtkPlaneSource> surfacePass;
    surfacePass->SetOrigin(-10, -10, 0);
    surfacePass->SetPoint1(10, -10, 0);
    surfacePass->SetPoint2(-10, 10, 0);
    surfacePass->Update();
    vtkPolyData* passPd = surfacePass->GetOutput();

    // Create Surface data
    vtkNew<vtkDoubleArray> surfaceTypePass;
    surfaceTypePass->SetNumberOfComponents(1);
    surfaceTypePass->SetName("SurfaceType");
    surfaceTypePass->SetNumberOfTuples(passPd->GetNumberOfCells());
    surfaceTypePass->FillComponent(0, vtkLagrangianBasicIntegrationModel::SURFACE_TYPE_PASS);
    passPd->GetCellData()->AddArray(surfaceTypePass);

    // Create plane passThrough
    vtkNew<vtkPlaneSource> surfaceBounce;
    surfaceBounce->SetOrigin(-2, -2, -2);
    surfaceBounce->SetPoint1(5, -2, -2);
    surfaceBounce->SetPoint2(-2, 5, -2);
    surfaceBounce->Update();
    vtkPolyData* bouncePd = surfaceBounce->GetOutput();

    // Create Surface data
    vtkNew<vtkDoubleArray> surfaceTypeBounce;
    surfaceTypeBounce->SetNumberOfComponents(1);
    surfaceTypeBounce->SetName("SurfaceType");
    surfaceTypeBounce->SetNumberOfTuples(bouncePd->GetNumberOfCells());
    surfaceTypeBounce->FillComponent(0, vtkLagrangianBasicIntegrationModel::SURFACE_TYPE_BOUNCE);
    bouncePd->GetCellData()->AddArray(surfaceTypeBounce);

    vtkNew<vtkMultiBlockDataGroupFilter> groupSurface;
    groupSurface->AddInputDataObject(surfacePd);
    groupSurface->AddInputDataObject(passPd);
    groupSurface->AddInputDataObject(bouncePd);

    vtkNew<vtkMultiBlockDataGroupFilter> groupFlow;
    groupFlow->AddInputDataObject(waveletImg);

    vtkNew<vtkImageDataToPointSet> ugFlow;
    ugFlow->AddInputData(waveletImg);

    vtkNew<vtkMultiBlockDataGroupFilter> groupSeed;
    groupSeed->AddInputDataObject(seedPolyData);

    // Create Integrator
    vtkNew<vtkRungeKutta2> integrator;

    // Create Integration Model
    vtkNew<vtkLagrangianMatidaIntegrationModel> integrationModel;
    integrationModel->SetInputArrayToProcess(0, 1, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "InitialVelocity");
    integrationModel->SetInputArrayToProcess(2, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "SurfaceType");
    integrationModel->SetInputArrayToProcess(3, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "FlowVelocity");
    integrationModel->SetInputArrayToProcess(4, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "FlowDensity");
    integrationModel->SetInputArrayToProcess(5, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "FlowDynamicViscosity");
    integrationModel->SetInputArrayToProcess(6, 1, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "ParticleDiameter");
    integrationModel->SetInputArrayToProcess(7, 1, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "ParticleDensity");
    integrationModel->SetNumberOfTrackedUserData(13);

    // Put in tracker
    vtkNew<vtkLagrangianParticleTracker> tracker;
    tracker->SetIntegrator(nullptr);
    tracker->SetIntegrationModel(nullptr);
    tracker->Print(cout);
    tracker->SetIntegrator(integrator);
    tracker->SetIntegrationModel(integrationModel);
    tracker->SetInputConnection(groupFlow->GetOutputPort());
    tracker->SetStepFactor(0.1);
    tracker->SetStepFactorMin(0.1);
    tracker->SetStepFactorMax(0.1);
    tracker->SetMaximumNumberOfSteps(150);
    tracker->SetSourceConnection(groupSeed->GetOutputPort());
    tracker->SetSurfaceData(surfacePd);
    tracker->SetCellLengthComputationMode(vtkLagrangianParticleTracker::STEP_CUR_CELL_VEL_DIR);
    tracker->AdaptiveStepReintegrationOn();
    tracker->GenerateParticlePathsOutputOff();
    tracker->Update();
    tracker->GenerateParticlePathsOutputOn();
    tracker->SetInputConnection(ugFlow->GetOutputPort());
    tracker->SetMaximumNumberOfSteps(30);
    tracker->SetCellLengthComputationMode(vtkLagrangianParticleTracker::STEP_CUR_CELL_DIV_THEO);
    tracker->Update();
    tracker->SetMaximumNumberOfSteps(-1);
    tracker->SetMaximumIntegrationTime(10.0);
    tracker->Update();
    tracker->SetInputData(waveletImg);    // inputData
    tracker->SetSourceData(seedPolyData); // sourceData
    tracker->SetMaximumNumberOfSteps(300);
    tracker->SetMaximumIntegrationTime(-1.0);
    tracker->SetSurfaceConnection(groupSurface->GetOutputPort());
    tracker->SetCellLengthComputationMode(vtkLagrangianParticleTracker::STEP_CUR_CELL_VEL_DIR);
    tracker->AdaptiveStepReintegrationOff();
    tracker->Update();
    tracker->Print(cout);

    //----------------------------------------------------------------------
    // Glyph for interaction points
    vtkNew<vtkSphereSource> sphereGlyph;
    sphereGlyph->SetRadius(0.1);

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 1, 1);
    points->InsertNextPoint(2, 2, 2);
    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetSourceConnection(sphereGlyph->GetOutputPort());
    vtkMultiBlockDataSet* mbInter = vtkMultiBlockDataSet::SafeDownCast(tracker->GetOutput(1));
    glyph->SetInputData(mbInter->GetBlock(1));

    // Setup actor and mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(vtkPolyData::SafeDownCast(tracker->GetOutput()));

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkPolyDataMapper> surfaceMapper;
    surfaceMapper->SetInputConnection(surfaceBounce->GetOutputPort());
    vtkNew<vtkActor> surfaceActor;
    surfaceActor->SetMapper(surfaceMapper);
    vtkNew<vtkPolyDataMapper> surfaceMapper2;
    surfaceMapper2->SetInputConnection(surfacePass->GetOutputPort());
    vtkNew<vtkActor> surfaceActor2;
    surfaceActor2->SetMapper(surfaceMapper2);

    vtkNew<vtkPolyDataMapper> glyphMapper;
    glyphMapper->SetInputConnection(glyph->GetOutputPort());
    vtkNew<vtkActor> glyphActor;
    glyphActor->SetMapper(glyphMapper);

    // Setup render window, renderer, and interactor
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    // renderer->AddActor(surfaceActor);
    // renderer->AddActor(surfaceActor2);
    // renderer->AddActor(glyphActor);
    renderer->SetBackground(0.1, .5, 1);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST501

#ifdef TEST502

#include "vtkActor.h"
#include "vtkCellData.h"
#include "vtkDataSetMapper.h"
#include "vtkDoubleArray.h"
#include "vtkImageData.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkLagrangianMatidaIntegrationModel.h"
#include "vtkLagrangianParticleTracker.h"
#include "vtkPointData.h"
#include "vtkPointSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRTAnalyticSource.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRungeKutta2.h"
#include "vtkSphereSource.h"
#include <vtkLookupTable.h>
#include <vtkProbeFilter.h>

int main(int, char*[])
{
    //--------------------------------------------------------------------
    // 种子点数据
    vtkNew<vtkPointSource> seeds;
    seeds->SetNumberOfPoints(10);
    seeds->SetRadius(4);
    seeds->Update();

    vtkPolyData* seedPolyData = seeds->GetOutput();

    vtkNew<vtkDoubleArray> partVel;
    partVel->SetNumberOfComponents(3); // tuple3
    partVel->SetNumberOfTuples(seedPolyData->GetNumberOfPoints());
    partVel->SetName("InitialVelocity");
    partVel->FillComponent(0, 2); // 将每个tuple3的第0个分量设置为2，每一个tuple3的值都一样
    partVel->FillComponent(1, 5); // 将每个tuple3的第0个分量设置为5
    partVel->FillComponent(2, 1); // 将每个tuple3的第0个分量设置为1

    vtkNew<vtkDoubleArray> partDens;
    partDens->SetNumberOfComponents(1);
    partDens->SetNumberOfTuples(seedPolyData->GetNumberOfPoints());
    partDens->SetName("ParticleDensity");
    partDens->FillComponent(0, 1920);

    vtkNew<vtkDoubleArray> partDiam;
    partDiam->SetNumberOfComponents(1);
    partDiam->SetNumberOfTuples(seedPolyData->GetNumberOfPoints());
    partDiam->SetName("ParticleDiameter");
    partDiam->FillComponent(0, 0.1);

    seedPolyData->GetPointData()->AddArray(partVel);
    seedPolyData->GetPointData()->AddArray(partDens);
    seedPolyData->GetPointData()->AddArray(partDiam);

    //---------------------------------------------------------------------------------
    // 网格数据
    vtkNew<vtkRTAnalyticSource> wavelet;
    wavelet->Update();
    vtkImageData* waveletImg = wavelet->GetOutput();

    // 设置流场数据
    vtkNew<vtkDoubleArray> flowVel;
    flowVel->SetNumberOfComponents(3);
    flowVel->SetNumberOfTuples(waveletImg->GetNumberOfCells());
    flowVel->SetName("FlowVelocity");
    flowVel->FillComponent(0, -0.3);
    flowVel->FillComponent(1, -0.3);
    flowVel->FillComponent(2, -0.3);

    vtkNew<vtkDoubleArray> flowDens;
    flowDens->SetNumberOfComponents(1);
    flowDens->SetNumberOfTuples(waveletImg->GetNumberOfCells());
    flowDens->SetName("FlowDensity");
    flowDens->FillComponent(0, 1000);

    vtkNew<vtkDoubleArray> flowDynVisc;
    flowDynVisc->SetNumberOfComponents(1);
    flowDynVisc->SetNumberOfTuples(waveletImg->GetNumberOfCells());
    flowDynVisc->SetName("FlowDynamicViscosity");
    flowDynVisc->FillComponent(0, 0.894);

    // 给每一个单元设置速度、压力、流动阻力
    waveletImg->GetCellData()->AddArray(flowVel);
    waveletImg->GetCellData()->AddArray(flowDens);
    waveletImg->GetCellData()->AddArray(flowDynVisc);

    vtkNew<vtkRungeKutta2> integrator;
    vtkNew<vtkLagrangianMatidaIntegrationModel> integrationModel;
    integrationModel->SetInputArrayToProcess(0, 1, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "InitialVelocity");
    integrationModel->SetInputArrayToProcess(3, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "FlowVelocity");
    integrationModel->SetInputArrayToProcess(4, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "FlowDensity");
    integrationModel->SetInputArrayToProcess(5, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "FlowDynamicViscosity");
    integrationModel->SetInputArrayToProcess(6, 1, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "ParticleDiameter");
    integrationModel->SetInputArrayToProcess(7, 1, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "ParticleDensity");

    vtkNew<vtkLagrangianParticleTracker> tracker;
    tracker->SetIntegrator(integrator);             // 设置求解器
    tracker->SetIntegrationModel(integrationModel); // 积分模型
    tracker->SetStepFactor(0.1);                    // 积分步长
    tracker->SetStepFactorMin(0.1);                 // 最小积分步长
    tracker->SetStepFactorMax(0.1);                 // 最大积分步长
    tracker->SetCellLengthComputationMode(vtkLagrangianParticleTracker::STEP_CUR_CELL_VEL_DIR);
    tracker->AdaptiveStepReintegrationOn();   // 是否自适应步长，默认false
    tracker->GenerateParticlePathsOutputOn(); // 是否生成粒子路径（迹线），默认true
    tracker->SetMaximumIntegrationTime(10.0); // 最大生成时间，默认-1，没有时间限制
    tracker->SetMaximumNumberOfSteps(300);    // 最大步数，默认100，-1表示没有限制
    tracker->SetInputData(waveletImg);        // 网格
    tracker->SetSourceData(seedPolyData);     // 种子点
    tracker->Update();

    // tracker->Print(std::cout);
    // tracker->GetOutput()->Print(std::cout);

    //----------------------------------------------------------------------
    // 使用探针获取迹线的每一个顶点在网格中的标量或矢量值
    vtkNew<vtkPointSet> pointSet;
    pointSet->CopyStructure(vtkPolyData::SafeDownCast(tracker->GetOutput())); // 迹线上的每一个顶点

    vtkNew<vtkProbeFilter> probe;
    probe->SetInputData(pointSet);
    probe->SetSourceData(waveletImg);
    probe->Update();
    // probe->GetOutput()->Print(std::cout);

    // Array 0 name = RTData
    // Array 1 name = FlowDensity
    // Array 2 name = FlowDynamicViscosity
    // Array 3 name = FlowVelocity
    // Array 4 name = vtkValidPointMask
    const char* varName = "RTData";

    // 获取生成的迹线的标量/矢量数据
    auto pointArray = vtkPointSet::SafeDownCast(probe->GetOutput())->GetPointData()->GetArray(varName);
    auto range      = pointArray->GetRange();
    // pointArray->Print(std::cout);

    // 给迹线的顶点设置标量值
    auto trackerPolyData = vtkPolyData::SafeDownCast(tracker->GetOutput());
    trackerPolyData->GetPointData()->AddArray(pointArray);
    trackerPolyData->GetPointData()->SetActiveScalars(varName);

    vtkNew<vtkLookupTable> lut;
    lut->SetHueRange(0.67, 0);
    lut->SetRange(range);
    lut->Build();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(trackerPolyData);
    mapper->ScalarVisibilityOn();
    mapper->SetLookupTable(lut);
    mapper->SetScalarRange(range);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST502

#ifdef TEST600

#include <vtkActor.h>
#include <vtkArrayCalculator.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkOpenFOAMReader.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkStreamTracer.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>

int main()
{
    vtkNew<vtkOpenFOAMReader> openFOAMReader;
    openFOAMReader->SetFileName("../resources/cavity/cavity.foam"); // 设置读取文件路径
    openFOAMReader->SetCreateCellToPoint(1);
    openFOAMReader->SetSkipZeroTime(1); // 开启跳过0时刻
    openFOAMReader->SetTimeValue(2.0);  // 设置需要读取的时刻
    openFOAMReader->Update();

    auto numOfBlocks = openFOAMReader->GetOutput()->GetNumberOfBlocks();
    std::cout << "Number of blocks:\t" << numOfBlocks << '\n';

    vtkUnstructuredGrid* block0 = vtkUnstructuredGrid::SafeDownCast(openFOAMReader->GetOutput()->GetBlock(0));
    double bounds[6] { 0.0 };
    block0->GetBounds(bounds);
    std::cout << "bound x:\t" << bounds[0] << '\t' << bounds[1] << "\nbound y:\t" << bounds[2] << '\t' << bounds[3] << "\nbound z:\t" << bounds[4]
              << '\t' << bounds[5] << '\n';

    std::cout << "Number of cells:\t" << block0->GetNumberOfCells() << '\n';
    std::cout << "Number of points:\t" << block0->GetNumberOfPoints() << '\n';

    double scalarRange[2] { 0.0 };
    block0->GetScalarRange(scalarRange);
    std::cout << "scalar range:\t" << scalarRange[0] << '\t' << scalarRange[1] << '\n';

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(block0);
    mapper->SetScalarRange(block0->GetScalarRange());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> ren;
    ren->AddActor(actor);
    ren->SetBackground(1, 1, 1); // 设置背景色为白色
    ren->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> TrackballCamera;
    iren->SetInteractorStyle(TrackballCamera);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST600

#ifdef TEST601

#include <vtkActor.h>
#include <vtkArrayCalculator.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkOpenFOAMReader.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkStreamTracer.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>

int main()
{
    vtkNew<vtkOpenFOAMReader> openFOAMReader;
    openFOAMReader->SetFileName("../resources/cavity/cavity.foam"); // 设置读取文件路径
    openFOAMReader->SetCreateCellToPoint(1);
    openFOAMReader->SetSkipZeroTime(1); // 开启跳过0时刻
    openFOAMReader->SetTimeValue(1.5);  // 设置需要读取的时刻
    openFOAMReader->Update();

    vtkUnstructuredGrid* block0 = vtkUnstructuredGrid::SafeDownCast(openFOAMReader->GetOutput()->GetBlock(0));

    block0->Print(std::cout);

    // 设置生成流线的位置
    vtkNew<vtkLineSource> line;
    line->SetPoint1(0.01, 0.01, 0.001);
    line->SetPoint2(0.02, 0.02, 0.002);
    line->SetResolution(10);
    line->Update();

    vtkNew<vtkStreamTracer> streamline;
    streamline->SetSourceConnection(line->GetOutputPort());
    streamline->SetInputData(block0);
    streamline->SetIntegratorTypeToRungeKutta45(); // 设置流线的积分类型
    // streamline->SetMaximumPropagation(5000);       // 设置流线长度
    // streamline->SetIntegrationStepUnit(2);         // 设置流线积分步长单位
    streamline->SetInitialIntegrationStep(0.2);  // 设置流线积分初始步长
    streamline->SetMinimumIntegrationStep(0.01); // 设置流线积分最小步长
    streamline->SetMaximumIntegrationStep(0.5);  // 设置流线积分最大步长
    streamline->SetMaximumNumberOfSteps(2000);   // 设置流线积分最大步数
    streamline->SetIntegrationDirectionToBoth(); // 设置流线积分方向
    // streamline->SetTerminalSpeed(1e-12);           // 设置流线积分终止速度
    // streamline->SetMaximumError(1e-6);
    streamline->Update();

    // 计算速度的模
    vtkNew<vtkArrayCalculator> calc;
    calc->SetInputConnection(streamline->GetOutputPort());
    calc->AddVectorArrayName("U");
    calc->SetFunction("mag(U)");
    calc->SetResultArrayName("u_mag");
    calc->Update();

    vtkPolyData* streamlinedata = vtkPolyData::SafeDownCast(calc->GetOutput());
    streamlinedata->GetPointData()->SetActiveScalars("u_mag");

    // 计算速度范围
    double scalarRange[2] { 0.0 };
    streamlinedata->GetPointData()->GetScalars()->GetRange(scalarRange);
    std::cout << "point scalar:\t" << scalarRange[0] << '\t' << scalarRange[1] << '\n';

    vtkNew<vtkScalarBarActor> scalarBar;
    vtkNew<vtkLookupTable> pColorTable;
    pColorTable->SetNumberOfTableValues(31);
    pColorTable->SetHueRange(0.67, 0); // 标量条颜色范围，从蓝到红
    pColorTable->SetAlphaRange(1.0, 1.0);
    pColorTable->SetValueRange(1, 1);
    pColorTable->SetSaturationRange(1, 1);
    pColorTable->SetRange(scalarRange);
    pColorTable->Build();

    scalarBar->SetLookupTable(pColorTable);

    vtkNew<vtkPolyDataMapper> streamlinemappper;
    streamlinemappper->SetInputData(streamlinedata);
    streamlinemappper->SetLookupTable(pColorTable);
    streamlinemappper->SetScalarRange(scalarRange);

    vtkNew<vtkActor> streamlineactor;
    streamlineactor->SetMapper(streamlinemappper);

    vtkNew<vtkRenderer> ren;
    ren->AddActor(streamlineactor);
    ren->AddActor(scalarBar);
    ren->SetBackground(1, 1, 1); // 设置背景色为白色
    ren->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> TrackballCamera;
    iren->SetInteractorStyle(TrackballCamera);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST601

#ifdef TEST602

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkLineWidget.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRibbonFilter.h>
#include <vtkRuledSurfaceFilter.h>
#include <vtkRungeKutta4.h>
#include <vtkStreamTracer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkTubeFilter.h>

int main(int argc, char* argv[])
{
    auto numOfStreamLines = 25;
    auto illustration     = false;
    std::string fileName1 = "../resources/combxyz.bin";
    std::string fileName2 = "../resources/combq.bin";

    vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
    pl3d->SetXYZFileName(fileName1.c_str());
    pl3d->SetQFileName(fileName2.c_str());
    pl3d->SetScalarFunctionNumber(100); // Density
    pl3d->SetVectorFunctionNumber(202); // Momentum
    pl3d->Update();

    vtkStructuredGrid* pl3d_output = dynamic_cast<vtkStructuredGrid*>(pl3d->GetOutput()->GetBlock(0));

    double bounds[6] { 0.0 };
    pl3d_output->GetBounds(bounds);
    std::cout << "bound x:\t" << bounds[0] << '\t' << bounds[1] << "\nbound y:\t" << bounds[2] << '\t' << bounds[3] << "\nbound z:\t" << bounds[4]
              << '\t' << bounds[5] << '\n';

    std::cout << "Number of cells:\t" << pl3d_output->GetNumberOfCells() << '\n';
    std::cout << "Number of points:\t" << pl3d_output->GetNumberOfPoints() << '\n';

    double scalarRange[2] { 0.0 };
    pl3d_output->GetScalarRange(scalarRange);
    std::cout << "scalar range:\t" << scalarRange[0] << '\t' << scalarRange[1] << '\n';

    vtkNew<vtkRenderer> renderer;

    //----------------------------------------------------------------
    // 显示整个网格
    // vtkNew<vtkDataSetMapper> mapper;
    // mapper->SetInputData(pl3d_output);

    // vtkNew<vtkActor> actor;
    // actor->SetMapper(mapper);
    // actor->GetProperty()->SetRepresentationToWireframe();
    // renderer->AddActor(actor);

    //----------------------------------------------------------------
    // 提取轮廓线
    vtkNew<vtkStructuredGridOutlineFilter> outLine;
    outLine->SetInputData(pl3d_output);
    outLine->Update();

    vtkNew<vtkPolyDataMapper> outLineMapper;
    outLineMapper->SetInputData(outLine->GetOutput());

    vtkNew<vtkActor> outLineActor;
    outLineActor->SetMapper(outLineMapper);
    renderer->AddActor(outLineActor);

    //----------------------------------------------------------------
    // 流线
    vtkNew<vtkRungeKutta4> integ;
    vtkNew<vtkStreamTracer> streamer;
    streamer->SetIntegrator(integ);                // 设置求解器
    streamer->SetInputData(pl3d_output);           // 输入数据
    streamer->SetStartPosition(15, 5, 32);         // 设置初始点，从哪个点开始计算流线，一条流线
    streamer->SetMaximumPropagation(100);          // 设置迭代次数
    streamer->SetInitialIntegrationStep(0.1);      // 设置迭代步长
    streamer->SetIntegrationDirectionToBackward(); // 设置方向，向前向后
    streamer->Update();

    // 流管，将流线生成为流管（圆柱的管子）
    vtkNew<vtkTubeFilter> streamTube;
    streamTube->SetInputData(streamer->GetOutput());
    streamTube->SetRadius(0.05);      // 半径
    streamTube->SetNumberOfSides(10); // 面的数量，圆柱面
    streamTube->Update();

    vtkNew<vtkPolyDataMapper> streamMapper;
    streamMapper->SetInputData(streamTube->GetOutput());
    streamMapper->SetScalarRange(pl3d_output->GetPointData()->GetScalars()->GetRange());

    vtkNew<vtkActor> streamActor;
    streamActor->SetMapper(streamMapper);
    renderer->AddActor(streamActor);

    // 使用种子点生成多个流线的起始点
    vtkNew<vtkLineSource> seeds;
    seeds->SetPoint1(15, -5, 32); // 设置端点1
    seeds->SetPoint2(15, 5, 32);  // 设置端点2
    seeds->SetResolution(20);     // 在这条线上生成几个点
    seeds->Update();

    vtkNew<vtkStreamTracer> streamer2;
    streamer2->SetIntegrator(integ);
    streamer2->SetInputData(pl3d_output);
    streamer2->SetMaximumPropagation(100);
    streamer2->SetInitialIntegrationStep(0.1);
    streamer2->SetIntegrationDirectionToBackward();
    streamer2->SetSourceConnection(seeds->GetOutputPort()); // 为流线设置多个起始种子点，多条流线
    streamer2->Update();

    vtkNew<vtkPolyDataMapper> stream2Mapper;
    stream2Mapper->SetInputData(streamer2->GetOutput());
    stream2Mapper->SetScalarRange(pl3d_output->GetPointData()->GetScalars()->GetRange());

    vtkNew<vtkActor> stream2Actor;
    stream2Actor->SetMapper(stream2Mapper);
    renderer->AddActor(stream2Actor);

    //----------------------------------------------------------------
    // 流面，将流线生成为面
    vtkNew<vtkRuledSurfaceFilter> streamSurface;
    streamSurface->SetInputData(streamer2->GetOutput());
    streamSurface->SetRuledModeToPointWalk(); // 设置生成方法
    // streamSurface->SetRuledModeToResample();
    streamSurface->Update();

    vtkNew<vtkPolyDataMapper> streamSurfaceMapper;
    streamSurfaceMapper->SetInputData(streamSurface->GetOutput());
    streamSurfaceMapper->SetScalarRange(pl3d_output->GetPointData()->GetScalars()->GetRange());

    vtkNew<vtkActor> streamSurfaceActor;
    streamSurfaceActor->SetMapper(streamSurfaceMapper);
    renderer->AddActor(streamSurfaceActor);

    //-----------------------------------------------------------------
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST602

#ifdef TEST603

#include <vtkActor.h>
#include <vtkArrayCalculator.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkOpenFOAMReader.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkStreamTracer.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>

int main()
{
    vtkNew<vtkOpenFOAMReader> openFOAMReader;
    openFOAMReader->SetFileName("../resources/cavity/cavity.foam"); // 设置读取文件路径
    openFOAMReader->SetCreateCellToPoint(1);
    openFOAMReader->SetSkipZeroTime(1); // 开启跳过0时刻
    openFOAMReader->SetTimeValue(1.5);  // 设置需要读取的时刻
    openFOAMReader->Update();

    vtkUnstructuredGrid* block0 = vtkUnstructuredGrid::SafeDownCast(openFOAMReader->GetOutput()->GetBlock(0));
    // block0->Print(std::cout);

    auto vectors = block0->GetPointData()->GetVectors();
    auto scalars = block0->GetPointData()->GetScalars();
    std::cout << "Vectors name: " << vectors->GetName() << "\nScalars name: " << scalars->GetName() << '\n';

    // 生成流线 向量数据是必须的，因为计算的是速度（矢量）的切线

    vtkNew<vtkUnstructuredGrid> grid;
    grid->SetPoints(block0->GetPoints());
    grid->GetPointData()->SetVectors(vectors);
    // grid->GetPointData()->SetScalars(scalars); // 没有标量数据也可以生成流线，因为流线是速度的切线，只需要矢量型的速度数据即可

    for (vtkIdType i = 0; i < block0->GetNumberOfCells(); ++i)
    {
        // grid必须有单元数据才能生成流线？
        auto cell = block0->GetCell(i);
        grid->InsertNextCell(cell->GetCellType(), cell->GetPointIds());
    }

    // 设置生成流线的位置
    vtkNew<vtkLineSource> line;
    line->SetPoint1(0.01, 0.01, 0.001);
    line->SetPoint2(0.02, 0.02, 0.002);
    line->SetResolution(10);
    line->Update();

    vtkNew<vtkStreamTracer> streamline;
    streamline->SetSourceConnection(line->GetOutputPort());
    streamline->SetInputData(grid);
    streamline->SetIntegratorTypeToRungeKutta45(); // 设置流线的积分类型
    streamline->SetMaximumPropagation(5000);       // 设置流线长度
    streamline->SetIntegrationStepUnit(2);         // 设置流线积分步长单位
    streamline->SetInitialIntegrationStep(0.2);    // 设置流线积分初始步长
    streamline->SetMinimumIntegrationStep(0.01);   // 设置流线积分最小步长
    streamline->SetMaximumIntegrationStep(0.5);    // 设置流线积分最大步长
    streamline->SetMaximumNumberOfSteps(2000);     // 设置流线积分最大步数
    streamline->SetIntegrationDirectionToBoth();   // 设置流线积分方向
    streamline->SetTerminalSpeed(1e-12);           // 设置流线积分终止速度
    streamline->SetMaximumError(1e-6);
    streamline->Update();

    // 计算速度的模
    vtkNew<vtkArrayCalculator> calc;
    calc->SetInputData(streamline->GetOutput());
    calc->AddVectorArrayName("U");
    calc->SetFunction("mag(U)");
    calc->SetResultArrayName("u_mag");
    calc->Update();

    vtkPolyData* streamlinedata = vtkPolyData::SafeDownCast(calc->GetOutput());
    streamlinedata->GetPointData()->SetActiveScalars("u_mag");

    double scalarRange[2] { 0.0 };
    if (auto _ponitdata = streamlinedata->GetPointData())
    {
        if (auto _scalars = _ponitdata->GetScalars())
        {
            _scalars->GetRange(scalarRange);
            std::cout << "Range: " << scalarRange[0] << '\t' << scalarRange[1] << '\n';
        }
    }

    vtkNew<vtkScalarBarActor> scalarBar;
    vtkNew<vtkLookupTable> pColorTable;
    pColorTable->SetHueRange(0.67, 0); // 标量条颜色范围，从蓝到红
    pColorTable->SetRange(scalarRange);
    pColorTable->Build();

    scalarBar->SetLookupTable(pColorTable);

    vtkNew<vtkPolyDataMapper> streamlinemappper;
    streamlinemappper->SetInputData(streamlinedata);
    streamlinemappper->SetLookupTable(pColorTable);
    streamlinemappper->SetScalarRange(scalarRange);

    vtkNew<vtkActor> streamlineactor;
    streamlineactor->SetMapper(streamlinemappper);

    vtkNew<vtkRenderer> ren;
    ren->AddActor(streamlineactor);
    ren->AddActor(scalarBar);
    ren->SetBackground(.1, .2, .3);
    ren->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> TrackballCamera;
    iren->SetInteractorStyle(TrackballCamera);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST603

#ifdef TEST701

#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkQuadric.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>

int main(int argc, char* argv[])
{
    auto numOfStreamLines = 25;
    auto illustration     = false;
    std::string fileName1 = "../resources/combxyz.bin";
    std::string fileName2 = "../resources/combq.bin";

    vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
    pl3d->SetXYZFileName(fileName1.c_str());
    pl3d->SetQFileName(fileName2.c_str());
    pl3d->SetScalarFunctionNumber(100); // Density
    pl3d->SetVectorFunctionNumber(202); // Momentum
    pl3d->Update();

    vtkStructuredGrid* pl3d_output = dynamic_cast<vtkStructuredGrid*>(pl3d->GetOutput()->GetBlock(0));

    double bounds[6] { 0.0 };
    pl3d_output->GetBounds(bounds);
    std::cout << "bound x:\t" << bounds[0] << '\t' << bounds[1] << "\nbound y:\t" << bounds[2] << '\t' << bounds[3] << "\nbound z:\t" << bounds[4]
              << '\t' << bounds[5] << '\n';

    std::cout << "Number of cells:\t" << pl3d_output->GetNumberOfCells() << '\n';
    std::cout << "Number of points:\t" << pl3d_output->GetNumberOfPoints() << '\n';

    double scalarRange[2] { 0.0 };
    pl3d_output->GetScalarRange(scalarRange);
    std::cout << "scalar range:\t" << scalarRange[0] << '\t' << scalarRange[1] << '\n';

    vtkNew<vtkRenderer> renderer;

    //----------------------------------------------------------------
    // 显示整个网格
    // vtkNew<vtkDataSetMapper> mapper;
    // mapper->SetInputData(pl3d_output);

    // vtkNew<vtkActor> actor;
    // actor->SetMapper(mapper);
    // actor->GetProperty()->SetRepresentationToWireframe();
    // renderer->AddActor(actor);

    //----------------------------------------------------------------
    // 提取轮廓线
    vtkNew<vtkStructuredGridOutlineFilter> outLine;
    outLine->SetInputData(pl3d_output);
    outLine->Update();

    vtkNew<vtkPolyDataMapper> outLineMapper;
    outLineMapper->SetInputData(outLine->GetOutput());

    vtkNew<vtkActor> outLineActor;
    outLineActor->SetMapper(outLineMapper);
    renderer->AddActor(outLineActor);

    //----------------------------------------------------------------
    // 等值面
    vtkNew<vtkContourFilter> contour;
    contour->SetInputData(pl3d_output);
    contour->GenerateValues(5, scalarRange); // 设置在scalarRange范围内提取5个等值面
    contour->Update();

    vtkNew<vtkPolyDataMapper> contourMapper;
    contourMapper->SetInputData(contour->GetOutput());

    vtkNew<vtkActor> contourActor;
    contourActor->SetMapper(contourMapper);
    renderer->AddActor(contourActor);

    //----------------------------------------------------------------
    // 使用函数提取等值面
    vtkNew<vtkQuadric> quadric;
    quadric->SetCoefficients(.5, 1., .2, 0., .1, 0., 0., .2, 0., 0.); // 设置二次函数的9个参数

    vtkNew<vtkSampleFunction> sampler;        // 采样函数
    sampler->SetSampleDimensions(30, 30, 30); // 设置精度
    sampler->SetImplicitFunction(quadric);
    sampler->Update();

    vtkNew<vtkContourFilter> contourFunction;
    contourFunction->SetInputConnection(sampler->GetOutputPort());
    contourFunction->GenerateValues(3, scalarRange); // 设置在scalarRange范围内提取3个等值面
    contourFunction->Update();

    vtkNew<vtkPolyDataMapper> contourFunctionMapper;
    contourFunctionMapper->SetInputData(contourFunction->GetOutput());

    vtkNew<vtkActor> contourFunctionActor;
    contourFunctionActor->SetMapper(contourFunctionMapper);
    // renderer->AddActor(contourFunctionActor);

    //----------------------------------------------------------------
    // 提取平面
    vtkNew<vtkStructuredGridGeometryFilter> plane;
    plane->SetInputData(pl3d_output);
    plane->SetExtent(1, 100, 1, 100, 7, 7); // 提取一个2维平面，xyz的范围分别为x[1,100] y[1,100] z = 7
    plane->Update();

    vtkNew<vtkPolyDataMapper> planeMapper;
    planeMapper->SetInputData(plane->GetOutput());

    vtkNew<vtkActor> planeActor;
    planeActor->SetMapper(planeMapper);
    // renderer->AddActor(planeActor);

    //-----------------------------------------------------------------
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST701

#ifdef TEST702

#include <vtkActor.h>
#include <vtkCylinder.h>
#include <vtkFlyingEdges3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>

// https://examples.vtk.org/site/Cxx/Visualization/IsosurfaceSampling/

int main(int argc, char* argv[])
{
    int resolution = 50;
    if (argc > 1)
    {
        resolution = atoi(argv[1]);
    }

    vtkNew<vtkNamedColors> colors;

    // Create a sampled sphere.
    vtkNew<vtkSphere> implicitSphere;
    double radius = 1.0;
    implicitSphere->SetRadius(radius);

    vtkNew<vtkSampleFunction> sampledSphere;
    sampledSphere->SetSampleDimensions(resolution, resolution, resolution);
    double xMin = -radius * 2.0;
    double xMax = radius * 2.0;
    sampledSphere->SetModelBounds(xMin, xMax, xMin, xMax, xMin, xMax);
    sampledSphere->SetImplicitFunction(implicitSphere);

    vtkNew<vtkFlyingEdges3D> isoSphere;
    isoSphere->SetValue(0, 1.0);
    isoSphere->SetInputConnection(sampledSphere->GetOutputPort());

    // Create a sampled cylinder.
    vtkNew<vtkCylinder> implicitCylinder;
    implicitCylinder->SetRadius(radius / 2.0);
    vtkNew<vtkSampleFunction> sampledCylinder;
    sampledCylinder->SetSampleDimensions(resolution, resolution, resolution);
    sampledCylinder->SetModelBounds(xMin, xMax, xMin, xMax, xMin, xMax);
    sampledCylinder->SetImplicitFunction(implicitCylinder);

    // Probe cylinder with the sphere isosurface.
    vtkNew<vtkProbeFilter> probeCylinder;
    probeCylinder->SetInputConnection(0, isoSphere->GetOutputPort());
    probeCylinder->SetInputConnection(1, sampledCylinder->GetOutputPort());
    probeCylinder->Update();

    // Restore the original normals.
    probeCylinder->GetOutput()->GetPointData()->SetNormals(isoSphere->GetOutput()->GetPointData()->GetNormals());

    std::cout << "Scalar range: " << probeCylinder->GetOutput()->GetScalarRange()[0] << ", " << probeCylinder->GetOutput()->GetScalarRange()[1]
              << std::endl;

    // Create a mapper and actor.
    vtkNew<vtkPolyDataMapper> mapSphere;
    mapSphere->SetInputConnection(probeCylinder->GetOutputPort());
    mapSphere->SetScalarRange(probeCylinder->GetOutput()->GetScalarRange());

    vtkNew<vtkActor> sphere;
    sphere->SetMapper(mapSphere);

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("IsosurfaceSampling");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(sphere);
    renderer->SetBackground(colors->GetColor3d("AliceBlue").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST702

#ifdef TEST703

// https://kitware.github.io/vtk-examples/site/Cxx/Modelling/ContourTriangulator/

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourTriangulator.h>
#include <vtkDataSetMapper.h>
#include <vtkMarchingSquares.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
    std::string inputFileName = "../resources/fullhead15.png";
    int isoValue              = 500;

    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPNGReader> reader;
    if (!reader->CanReadFile(inputFileName.c_str()))
    {
        std::cerr << "Error: Could not read " << inputFileName << ".\n";
        return EXIT_FAILURE;
    }
    reader->SetFileName(inputFileName.c_str());
    reader->Update();

    vtkNew<vtkMarchingSquares> iso;
    iso->SetInputConnection(reader->GetOutputPort());
    iso->SetValue(0, isoValue);

    vtkNew<vtkDataSetMapper> isoMapper;
    isoMapper->SetInputConnection(iso->GetOutputPort());
    isoMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> isoActor;
    isoActor->SetMapper(isoMapper);
    isoActor->GetProperty()->SetColor(colors->GetColor3d("MediumOrchid").GetData());

    vtkNew<vtkContourTriangulator> poly;
    poly->SetInputConnection(iso->GetOutputPort());

    vtkNew<vtkDataSetMapper> polyMapper;
    polyMapper->SetInputConnection(poly->GetOutputPort());
    polyMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> polyActor;
    polyActor->SetMapper(polyMapper);
    polyActor->GetProperty()->SetColor(colors->GetColor3d("Gray").GetData());

    // Standard rendering classes
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(polyActor); // 三角面
    renderer->AddActor(isoActor);  // 等值线
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->SetMultiSamples(0);
    renWin->AddRenderer(renderer);
    renWin->SetWindowName("ContourTriangulator");
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renWin->Render();
    iren->Initialize();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST703

#ifdef TEST704

#include <vtkActor.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
    // 顶点
    vtkNew<vtkPoints> points;
    for (int i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, 0, 0);
        points->InsertNextPoint(i, 1, 0);
        points->InsertNextPoint(i, 2, 0);
        points->InsertNextPoint(i, 3, 0);
    }

    // 拓扑
    vtkNew<vtkCellArray> cells;
    for (vtkIdType i = 0; i < 33; i += 4)
    {
        cells->InsertNextCell({ i, i + 1, i + 5, i + 4 });
        cells->InsertNextCell({ i + 1, i + 2, i + 6, i + 5 });
        cells->InsertNextCell({ i + 2, i + 3, i + 7, i + 6 });
    }

    // 每个顶点的标量数据
    vtkNew<vtkFloatArray> scalars;
    for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
    {
        scalars->InsertNextValue(static_cast<float>(i));
    }

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetPointData()->SetScalars(scalars);

    // vtkBandedPolyDataContourFilter 生成带状轮廓，原始数据必须有顶点的Scalars
    // 生成的 vtkPolyData 顶点数据不变，新增poly类型的单元，单元的Scalars值对应 GenerateValues() 设置的参数
    vtkNew<vtkBandedPolyDataContourFilter> bf;
    bf->SetInputData(polyData);
    bf->GenerateValues(10, 10, 20); // 等值带的个数，等值带的最小值、等值带的最大值
    bf->Update();

    // bf->Print(std::cout);
    // bf->GetOutput()->Print(std::cout);

    double range[2] {};
    if (auto&& ps = bf->GetOutput()->GetPointData()->GetScalars())
    {
        ps->GetRange(range);
        std::cout << "Point scalars range: " << range[0] << '\t' << range[1] << '\n';
    }
    if (auto&& cs = bf->GetOutput()->GetCellData()->GetScalars())
    {
        cs->GetRange(range);
        std::cout << "Cell scalars range: " << range[0] << '\t' << range[1] << '\n';
    }

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(bf->GetOutput());
    mapper->SetScalarRange(0, 11);
    mapper->ScalarVisibilityOn();
    mapper->SetScalarModeToUseCellData();
    // mapper->SetScalarModeToUsePointData(); // 顶点的数据值还是原来的值，没有改变

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    renderer->SetBackground(.1, .2, .3);
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("BandedPolyDataContourFilter");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST704