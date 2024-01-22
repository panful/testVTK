/**
 * 101. vtkRenderView vtkView 的基础使用，通过往 vtkRenderer 添加 vtkActor 绘制图形
 * 102. vtkRenderView 不给 vtkRenderer 添加 vtkActor，绘制图形
 *
 * 201. vtkChartXY 的基本使用
 * 202. 一个窗口显示多个 vtkChart
 * 203. 动态添加 vtkChartXY 的值，适配窗口
 * 204. 实时更新 vtkChartXY
 * 
 * 301. vtkViewNode 自定义vtk的后端 Scene Graph
 *
 */

#define TEST301

#ifdef TEST101

#include <vtkActor.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderView.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

// vtkRenderView 通常被用来与GUI(比如Qt)进行集成
// vtkRenderView 包含了一个 vtkRenderWindow 和 vtkRenderer

int main(int, char*[])
{
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1., 0., 0.);

    vtkNew<vtkRenderView> renderView;
    renderView->GetRenderer()->AddActor(actor);
    renderView->GetRenderer()->SetBackground(.3, .4, .5);               // 默认两个背景色渐变
    renderView->SetInteractionMode(vtkRenderView::INTERACTION_MODE_2D); // 交互模式，默认只有两种，也可以自己设置Style
    renderView->GetRenderWindow()->SetSize(800, 600);
    renderView->Update();

    renderView->ResetCamera();
    renderView->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

#endif // TEST101

#ifdef TEST102

#include <vtkActor.h>
#include <vtkGraphLayout.h>
#include <vtkGraphLayoutView.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSimple2DLayoutStrategy.h>

// 在类 vtkRenderedGraphRepresentation 中创建 vtkActor
// vtkRenderedGraphRepresentation 保存在 vtkView 的成员变量 Implementation 中

int main(int, char*[])
{
    // 可编辑的有向图
    vtkNew<vtkMutableDirectedGraph> g;

    vtkIdType v1 = g->AddVertex();
    vtkIdType v2 = g->AddVertex();
    vtkIdType v3 = g->AddVertex();

    g->AddEdge(v1, v2);
    g->AddEdge(v2, v3);
    g->AddEdge(v3, v1);

    vtkNew<vtkSimple2DLayoutStrategy> strategy;
    vtkNew<vtkGraphLayout> layout;
    layout->SetInputData(g);
    layout->SetLayoutStrategy(strategy);

    // 手动布局，获取边缘箭头的位置
    // vtkGraphLayoutView 继承自 vtkRenderView
    vtkNew<vtkGraphLayoutView> graphLayoutView;
    // 使用提供的顶点布局
    graphLayoutView->SetLayoutStrategyToPassThrough();
    // 箭头在两个顶点之间的直线上，不为平行边绘制圆弧
    graphLayoutView->SetEdgeLayoutStrategyToPassThrough();
    // 将图形添加到视图中，将会渲染顶点和边，箭头不会被渲染
    // 该函数会创建一个 vtkRenderedGraphRepresentation
    graphLayoutView->AddRepresentationFromInputConnection(layout->GetOutputPort());

    //--------------------------------------------------------------------
    graphLayoutView->GetRenderer()->SetBackground(.3, .4, .5);
    graphLayoutView->GetRenderWindow()->SetSize(800, 600);
    graphLayoutView->ResetCamera();

    graphLayoutView->Render();
    graphLayoutView->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

#endif // TEST102

#ifdef TEST201

#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include <vtkPlot.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

int main(int, char*[])
{
    vtkNew<vtkFloatArray> arrX;
    arrX->SetName("X Axis");

    vtkNew<vtkFloatArray> arrC;
    arrC->SetName("Cosine");

    vtkNew<vtkTable> table;
    table->AddColumn(arrX);
    table->AddColumn(arrC);
    table->SetNumberOfRows(3);
    table->SetValue(0, 0, 0);  // X0 三个参数分别表示：行、列、值
    table->SetValue(0, 1, 0);  // Y0
    table->SetValue(1, 0, 2);  // X1
    table->SetValue(1, 1, 4);  // Y1
    table->SetValue(2, 0, 4);  // X2
    table->SetValue(2, 1, 16); // Y2

    vtkNew<vtkChartXY> chart;
    vtkPlot* line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 1); // 第二个参数表示图标的横坐标使用table的第几列数据，第三个参数表示纵轴使用第几列数据
    line->SetColor(0, 255, 0, 255);
    line->SetWidth(2.f);
    line->Update();
    chart->Update();

    vtkNew<vtkContextView> view;
    view->GetScene()->AddItem(chart);
    view->GetRenderWindow()->SetSize(800, 600);
    view->GetRenderer()->SetBackground(.4, .5, .6);

    view->GetRenderWindow()->Render();
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

#endif // TEST201

#ifdef TEST202

#include <vtkAxis.h>
#include <vtkBrush.h>
#include <vtkChartXY.h>
#include <vtkContextActor.h>
#include <vtkContextScene.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlotPoints.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkVersion.h>

#include <array>
#include <cmath>
#include <vector>

#if VTK_VERSION_NUMBER >= 90220220630ULL
#define VTK_HAS_SETCOLORF 1
#endif

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkRenderWindow> renWin;
    renWin->SetWindowName("MultiplePlots");
    vtkNew<vtkRenderWindowInteractor> iRen;
    iRen->SetRenderWindow(renWin);

    // Setup the viewports
    auto gridDimensionsX = 2;
    auto gridDimensionsY = 1;
    auto rendererSizeX   = 320;
    auto rendererSizeY   = 240;
    renWin->SetSize(rendererSizeX * gridDimensionsX, rendererSizeY * gridDimensionsY);
    std::cout << rendererSizeX * gridDimensionsX << " " << gridDimensionsY * gridDimensionsY << std::endl;
    std::vector<std::array<double, 4>> viewPorts;
    for (auto row = 0; row < gridDimensionsY; ++row)
    {
        for (auto col = 0; col < gridDimensionsX; ++col)
        {
            // index = row * gridDimensionsX + col

            // (xmin, ymin, xmax, ymax)
            viewPorts.push_back(std::array<double, 4> { static_cast<double>(col) / gridDimensionsX,
                static_cast<double>(gridDimensionsY - (row + 1.0)) / gridDimensionsY, static_cast<double>(col + 1.0) / gridDimensionsX,
                static_cast<double>(gridDimensionsY - static_cast<double>(row)) / gridDimensionsY });
        }
    }

    // Link the renderers to the viewports.
    vtkNew<vtkRenderer> leftRenderer;
    leftRenderer->SetBackground(colors->GetColor3d("AliceBlue").GetData());
    leftRenderer->SetViewport(viewPorts[0].data());
    renWin->AddRenderer(leftRenderer);

    vtkNew<vtkRenderer> rightRenderer;
    rightRenderer->SetBackground(colors->GetColor3d("Lavender").GetData());
    rightRenderer->SetViewport(viewPorts[1].data());
    renWin->AddRenderer(rightRenderer);

    // Create the charts.
    vtkNew<vtkChartXY> leftChart;
    vtkNew<vtkContextScene> leftChartScene;
    vtkNew<vtkContextActor> leftChartActor;

    leftChartScene->AddItem(leftChart);
    leftChartActor->SetScene(leftChartScene);

    leftRenderer->AddActor(leftChartActor);
    leftChartScene->SetRenderer(leftRenderer);

    auto xAxis = leftChart->GetAxis(vtkAxis::BOTTOM);
    xAxis->GetGridPen()->SetColor(colors->GetColor4ub("LightGrey"));
    xAxis->SetTitle("x");
    auto yAxis = leftChart->GetAxis(vtkAxis::LEFT);
    yAxis->GetGridPen()->SetColor(colors->GetColor4ub("LightGrey"));
    yAxis->SetTitle("cos(x)");
#if VTK_HAS_SETCOLORF
    leftChart->GetBackgroundBrush()->SetColorF(colors->GetColor4d("MistyRose").GetData());
#else
    // leftChart->GetBackgroundBrush()->SetColor(colors->GetColor4d("MistyRose").GetData());
#endif
    leftChart->GetBackgroundBrush()->SetOpacityF(0.4);
    leftChart->SetTitle("Cosine");

    vtkNew<vtkChartXY> rightChart;
    vtkNew<vtkContextScene> rightChartScene;
    vtkNew<vtkContextActor> rightChartActor;

    rightChartScene->AddItem(rightChart);
    rightChartActor->SetScene(rightChartScene);

    rightRenderer->AddActor(rightChartActor);
    rightChartScene->SetRenderer(rightRenderer);

    xAxis = rightChart->GetAxis(vtkAxis::BOTTOM);
    xAxis->GetGridPen()->SetColor(colors->GetColor4ub("LightCyan"));
    xAxis->SetTitle("x");
    yAxis = rightChart->GetAxis(vtkAxis::LEFT);
    yAxis->GetGridPen()->SetColor(colors->GetColor4ub("LightCyan"));
    yAxis->SetTitle("sin(x)");
#if VTK_HAS_SETCOLORF
    rightChart->GetBackgroundBrush()->SetColorF(colors->GetColor4d("Thistle").GetData());
#else
    // rightChart->GetBackgroundBrush()->SetColor(colors->GetColor4d("Thistle").GetData());
#endif
    rightChart->GetBackgroundBrush()->SetOpacityF(0.4);
    rightChart->SetTitle("Sine");

    // Create a table with some points in it.
    vtkNew<vtkTable> table;

    vtkNew<vtkFloatArray> arrX;
    arrX->SetName("X Axis");
    table->AddColumn(arrX);

    vtkNew<vtkFloatArray> arrC;
    arrC->SetName("Cosine");
    table->AddColumn(arrC);

    vtkNew<vtkFloatArray> arrS;
    arrS->SetName("Sine");
    table->AddColumn(arrS);

    // Fill in the table with some example values.
    auto numPoints = 40;
    auto inc       = 7.5 / (numPoints - 1.0);
    table->SetNumberOfRows(numPoints);
    for (int i = 0; i < numPoints; ++i)
    {
        table->SetValue(i, 0, i * inc);
        table->SetValue(i, 1, cos(i * inc));
        table->SetValue(i, 2, sin(i * inc));
    }

    auto ptColor = colors->GetColor4ub("Black");

    auto points = leftChart->AddPlot(vtkChart::POINTS);
    points->SetInputData(table, 0, 1);
    points->SetColor(ptColor.GetRed(), ptColor.GetGreen(), ptColor.GetBlue(), ptColor.GetAlpha());
    points->SetWidth(1.0);
    dynamic_cast<vtkPlotPoints*>(points)->SetMarkerStyle(vtkPlotPoints::CROSS);

    points = rightChart->AddPlot(vtkChart::POINTS);
    points->SetInputData(table, 0, 2);
    points->SetColor(ptColor.GetRed(), ptColor.GetGreen(), ptColor.GetBlue(), ptColor.GetAlpha());
    points->SetWidth(1.0);
    dynamic_cast<vtkPlotPoints*>(points)->SetMarkerStyle(vtkPlotPoints::PLUS);

    renWin->Render();
    iRen->Initialize();
    iRen->Start();

    return EXIT_SUCCESS;
}

#endif // TEST202

#ifdef TEST203

#include <vtkChartXY.h>
#include <vtkContextActor.h>
#include <vtkContextInteractorStyle.h>
#include <vtkContextScene.h>
#include <vtkFloatArray.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTable.h>
#include <vtkVariantArray.h>

namespace {
class MyStyle : public vtkContextInteractorStyle
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkContextInteractorStyle);

    void OnLeftButtonUp() override
    {
        Superclass::OnLeftButtonUp();

        this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

        if (!m_axisX || !m_axisY || !m_table || !this->Interactor || !this->CurrentRenderer || !m_chart)
        {
            return;
        }

        // 更新 vtkChart 的三种方式
        int type { 2 };
        switch (type)
        {
        case 0:
        {
            m_table->SetNumberOfRows(4); // 需要重新赋值原来位置的值，不然重新申请空间后，原来位置的值会变为非法值
            m_table->SetValue(0, 0, 0.f);
            m_table->SetValue(0, 1, 0.f);
            m_table->SetValue(1, 0, 1.f);
            m_table->SetValue(1, 1, 1.f);
            m_table->SetValue(2, 0, 2.f);
            m_table->SetValue(2, 1, 0.f);
            m_table->SetValue(3, 0, 3.f);
            m_table->SetValue(3, 1, 1.f);
            m_table->Modified(); // 必须调用Modify()，否则新添加的值不会被更新
            break;
        }
        case 1:
        {
            vtkNew<vtkVariantArray> variant;
            variant->SetNumberOfValues(2);
            variant->SetVariantValue(0, 3.f);
            variant->SetVariantValue(1, 1.f);
            m_table->InsertNextRow(variant);
            m_table->Modified(); // 必须调用Modify()，否则table中的数据不会更新
            break;
        }
        case 2:
        {
            m_axisX->InsertNextValue(3.f);
            m_axisY->InsertNextValue(1.f);
            m_table->Modified(); // 必须调用Modify()，否则table中的数据不会更新
            break;
        }
        }

        m_chart->RecalculateBounds(); // 重新计算范围，让整个折线充满窗口
        this->Interactor->Render();
    }

    void SetTable(vtkTable* t)
    {
        m_table = t;
    }

    void SetChart(vtkChartXY* c)
    {
        m_chart = c;
    }

    void SetFloatArray(vtkFloatArray* x, vtkFloatArray* y)
    {
        m_axisX = x;
        m_axisY = y;
    }

private:
    vtkSmartPointer<vtkTable> m_table { nullptr };
    vtkSmartPointer<vtkChartXY> m_chart { nullptr };
    vtkSmartPointer<vtkFloatArray> m_axisX { nullptr };
    vtkSmartPointer<vtkFloatArray> m_axisY { nullptr };
};

vtkStandardNewMacro(MyStyle);

} // namespace

// vtkPlotPoints::Update() [114] 更新折线的数据

int main(int, char*[])
{
    vtkNew<vtkFloatArray> arrX;
    arrX->SetName("X Axis");

    vtkNew<vtkFloatArray> arrY;
    arrY->SetName("Y Axis");

    vtkNew<vtkTable> table;
    table->AddColumn(arrX);
    table->AddColumn(arrY);
    table->SetNumberOfRows(3);
    table->SetValue(0, 0, 0.f);
    table->SetValue(0, 1, 0.f);
    table->SetValue(1, 0, 1.f);
    table->SetValue(1, 1, 1.f);
    table->SetValue(2, 0, 2.f);
    table->SetValue(2, 1, 0.f);

    vtkNew<vtkChartXY> chart;
    vtkPlot* line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 1);
    line->SetColor(0, 255, 0, 255);
    line->SetWidth(2.f);
    line->Update();
    chart->Update();

    vtkNew<vtkContextScene> scene;
    scene->AddItem(chart);

    vtkNew<vtkContextActor> actor;
    actor->SetScene(scene);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->SetBackground(.4, .5, .6);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<MyStyle> style;
    style->SetScene(scene);
    style->SetFloatArray(arrX, arrY);
    style->SetTable(table);
    style->SetChart(chart);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST203

#ifdef TEST204

#include <random>
#include <thread>
#include <vtkChartXY.h>
#include <vtkContextInteractorStyle.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include <vtkPlot.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

namespace {
class MyStyle : public vtkContextInteractorStyle
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkContextInteractorStyle);

    void OnLeftButtonUp() override
    {
        Superclass::OnLeftButtonUp();

        this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

        if (!m_axisX || !m_axisY || !m_table || !this->Interactor || !m_chart)
        {
            return;
        }

        static std::default_random_engine engine;
        static std::uniform_real_distribution<float> distribution(0.f, 1.f);
        static int x { 2 };

        for (size_t i = 0; i < 100; ++i)
        {
            m_axisX->InsertNextValue(static_cast<float>(x++));
            m_axisY->InsertNextValue(distribution(engine));
            m_table->Modified();          // 必须调用Modify()，否则table中的数据不会更新
            m_chart->RecalculateBounds(); // 重新计算范围，让整个折线充满窗口
            this->Interactor->Render();

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    void SetTable(vtkTable* t)
    {
        m_table = t;
    }

    void SetChart(vtkChartXY* c)
    {
        m_chart = c;
    }

    void SetFloatArray(vtkFloatArray* x, vtkFloatArray* y)
    {
        m_axisX = x;
        m_axisY = y;
    }

private:
    vtkSmartPointer<vtkTable> m_table { nullptr };
    vtkSmartPointer<vtkChartXY> m_chart { nullptr };
    vtkSmartPointer<vtkFloatArray> m_axisX { nullptr };
    vtkSmartPointer<vtkFloatArray> m_axisY { nullptr };
};

vtkStandardNewMacro(MyStyle);

} // namespace

int main(int, char*[])
{
    vtkNew<vtkFloatArray> arrX;
    arrX->SetName("X Axis");

    vtkNew<vtkFloatArray> arrY;
    arrY->SetName("Y Axis");

    vtkNew<vtkTable> table;
    table->AddColumn(arrX);
    table->AddColumn(arrY);

    table->SetNumberOfRows(2);

    table->SetValue(0, 0, 0);
    table->SetValue(0, 1, 0);
    table->SetValue(1, 0, 1);
    table->SetValue(1, 1, 1);

    vtkNew<vtkChartXY> chart;
    vtkPlot* line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 1);
    line->SetColor(0, 255, 0, 255);
    line->SetWidth(2.0);

    vtkNew<vtkContextView> view;
    view->GetScene()->AddItem(chart);
    view->GetRenderWindow()->SetSize(800, 600);
    view->GetRenderer()->SetBackground(.4, .5, .6);

    vtkNew<MyStyle> style;
    style->SetScene(view->GetScene());
    style->SetFloatArray(arrX, arrY);
    style->SetTable(table);
    style->SetChart(chart);

    view->GetInteractor()->SetInteractorStyle(style);
    view->GetRenderWindow()->Render();
    view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

#endif // TEST204

#ifdef TEST301

#include <vtkActor.h>
#include <vtkActorNode.h>
#include <vtkCamera.h>
#include <vtkCameraNode.h>
#include <vtkMapperNode.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapperNode.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRendererNode.h>
#include <vtkViewNodeFactory.h>
#include <vtkWindowNode.h>

// vtkXXXNode 可以重写为自定义的后端，不使用vtk提供的默认后端(OpenGL)
// 一般需要重写 Build() Render() Synchronize() 等函数
// OSPRay Anari 就是通过这样的方式定义新的后端

int main()
{
    // vtk提供的默认后端
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkActor> actor;
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> window;
    vtkNew<vtkCamera> camera;

    // 注册创建后端的函数
    vtkNew<vtkViewNodeFactory> factory;
    factory->RegisterOverride(mapper->GetClassName(), []() { return static_cast<vtkViewNode*>(vtkPolyDataMapperNode::New()); });
    factory->RegisterOverride(actor->GetClassName(), []() { return static_cast<vtkViewNode*>(vtkActorNode::New()); });
    factory->RegisterOverride(renderer->GetClassName(), []() { return static_cast<vtkViewNode*>(vtkRendererNode::New()); });
    factory->RegisterOverride(window->GetClassName(), []() { return static_cast<vtkViewNode*>(vtkWindowNode::New()); });
    factory->RegisterOverride(camera->GetClassName(), []() { return static_cast<vtkViewNode*>(vtkCameraNode::New()); });

    vtkNew<vtkPolyDataMapperNode> mapperNode;
    mapperNode->SetRenderable(mapper);
    mapperNode->SetMyFactory(factory);

    vtkNew<vtkActorNode> actorNode;
    actorNode->SetRenderable(actor);
    actorNode->SetMyFactory(factory);

    vtkNew<vtkRendererNode> rendererNode;
    rendererNode->SetRenderable(renderer);
    rendererNode->SetMyFactory(factory);
    rendererNode->Traverse(vtkViewNode::build);

    vtkNew<vtkWindowNode> windowNode;
    windowNode->SetRenderable(window);
    windowNode->SetMyFactory(factory);
    windowNode->TraverseAllPasses();           // 遍历所有pass
    windowNode->Traverse(vtkViewNode::render); // 遍历单独的render pass

    // 渲染结果
    auto result_color   = windowNode->GetColorBuffer();
    auto result_zbuffer = windowNode->GetZBuffer();
}

#endif // TEST301
