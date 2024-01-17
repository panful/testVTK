/**
 * 1. vtkRenderView vtkView
 * 2. vtkViewNode SceneGraph
 * 3. vtkGraphLayoutView 图 有向图 Graph
 *
 * 201. vtkChartXY 的基本使用
 * 202. 一个窗口显示多个 vtkChart
 * 203. 动态添加 vtkChartXY 的值，适配窗口
 * 204. 实时更新 vtkChartXY 
 *
 */

#define TEST204

#ifdef TEST1

#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderView.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

    vtkNew<vtkRenderView> renderView;
    renderView->SetInteractionMode(vtkRenderView::INTERACTION_MODE_3D);
    renderView->GetRenderer()->AddActor(actor);
    renderView->RenderOnMouseMoveOn();
    renderView->Update();
    renderView->GetRenderer()->SetBackground(colors->GetColor3d("Beige").GetData());

    renderView->ResetCamera();
    renderView->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

#endif // TEST1

#ifdef TEST2

// VTK目录：Rendering/SceneGraph/Testing/Cxx

#include "vtkActor.h"
#include "vtkActorNode.h"
#include "vtkCamera.h"
#include "vtkCameraNode.h"
#include "vtkLight.h"
#include "vtkLightNode.h"
#include "vtkMapperNode.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererNode.h"
#include "vtkSphereSource.h"
#include "vtkViewNodeFactory.h"
#include "vtkWindowNode.h"

#include <string>

namespace {
std::string resultS;
}

//------------------------------------------------------------------------------
// ViewNode subclasses specialized for this test
class vtkMyActorNode : public vtkActorNode
{
public:
    static vtkMyActorNode* New();
    vtkTypeMacro(vtkMyActorNode, vtkActorNode);

    void Render(bool prepass) override
    {
        if (prepass)
        {
            cerr << "Render " << this << " " << this->GetClassName() << endl;
            resultS += "Render ";
            resultS += this->GetClassName();
            resultS += "\n";
        }
    }

    vtkMyActorNode()           = default;
    ~vtkMyActorNode() override = default;
};

vtkStandardNewMacro(vtkMyActorNode);

class vtkMyCameraNode : public vtkCameraNode
{
public:
    static vtkMyCameraNode* New();
    vtkTypeMacro(vtkMyCameraNode, vtkCameraNode);

    void Render(bool prepass) override
    {
        if (prepass)
        {
            cerr << "Render " << this << " " << this->GetClassName() << endl;
            resultS += "Render ";
            resultS += this->GetClassName();
            resultS += "\n";
        }
    }

    vtkMyCameraNode()           = default;
    ~vtkMyCameraNode() override = default;
};

vtkStandardNewMacro(vtkMyCameraNode);

class vtkMyLightNode : public vtkLightNode
{
public:
    static vtkMyLightNode* New();
    vtkTypeMacro(vtkMyLightNode, vtkLightNode);

    void Render(bool prepass) override
    {
        if (prepass)
        {
            cerr << "Render " << this << " " << this->GetClassName() << endl;
            resultS += "Render ";
            resultS += this->GetClassName();
            resultS += "\n";
        }
    }

    vtkMyLightNode()           = default;
    ~vtkMyLightNode() override = default;
};

vtkStandardNewMacro(vtkMyLightNode);

class vtkMyMapperNode : public vtkMapperNode
{
public:
    static vtkMyMapperNode* New();
    vtkTypeMacro(vtkMyMapperNode, vtkMapperNode);

    void Render(bool prepass) override
    {
        if (prepass)
        {
            cerr << "Render " << this << " " << this->GetClassName() << endl;
            resultS += "Render ";
            resultS += this->GetClassName();
            resultS += "\n";
        }
    }

    vtkMyMapperNode() = default;

    ~vtkMyMapperNode() override = default;
};

vtkStandardNewMacro(vtkMyMapperNode);

class vtkMyRendererNode : public vtkRendererNode
{
public:
    static vtkMyRendererNode* New();
    vtkTypeMacro(vtkMyRendererNode, vtkRendererNode);

    void Render(bool prepass) override
    {
        if (prepass)
        {
            cerr << "Render " << this << " " << this->GetClassName() << endl;
            resultS += "Render ";
            resultS += this->GetClassName();
            resultS += "\n";
        }
    }

    vtkMyRendererNode()           = default;
    ~vtkMyRendererNode() override = default;
};

vtkStandardNewMacro(vtkMyRendererNode);

class vtkMyWindowNode : public vtkWindowNode
{
public:
    static vtkMyWindowNode* New();
    vtkTypeMacro(vtkMyWindowNode, vtkWindowNode);

    void Render(bool prepass) override
    {
        if (prepass)
        {
            cerr << "Render " << this << " " << this->GetClassName() << endl;
            resultS += "Render ";
            resultS += this->GetClassName();
            resultS += "\n";
        }
    }

    vtkMyWindowNode()           = default;
    ~vtkMyWindowNode() override = default;
};

vtkStandardNewMacro(vtkMyWindowNode);

//------------------------------------------------------------------------------

// builders that produce the specialized ViewNodes
vtkViewNode* act_maker()
{
    vtkMyActorNode* vn = vtkMyActorNode::New();
    cerr << "make actor node " << vn << endl;
    resultS += "make actor\n";
    return vn;
}

vtkViewNode* cam_maker()
{
    vtkMyCameraNode* vn = vtkMyCameraNode::New();
    cerr << "make camera node " << vn << endl;
    resultS += "make camera\n";
    return vn;
}

vtkViewNode* light_maker()
{
    vtkMyLightNode* vn = vtkMyLightNode::New();
    cerr << "make light node " << vn << endl;
    resultS += "make light\n";
    return vn;
}

vtkViewNode* mapper_maker()
{
    vtkMyMapperNode* vn = vtkMyMapperNode::New();
    cerr << "make mapper node " << vn << endl;
    resultS += "make mapper\n";
    return vn;
}

vtkViewNode* ren_maker()
{
    vtkMyRendererNode* vn = vtkMyRendererNode::New();
    cerr << "make renderer node " << vn << endl;
    resultS += "make renderer\n";
    return vn;
}

vtkViewNode* win_maker()
{
    vtkMyWindowNode* vn = vtkMyWindowNode::New();
    cerr << "make window node " << vn << endl;
    resultS += "make window\n";
    return vn;
}

// exercises the scene graph related classes
int main(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
    vtkWindowNode* wvn = vtkWindowNode::New();
    cerr << "made " << wvn << endl;
    wvn->Delete();

    vtkViewNode* vn         = nullptr;
    vtkViewNodeFactory* vnf = vtkViewNodeFactory::New();
    cerr << "CREATE pre override" << endl;
    vn = vnf->CreateNode(nullptr);
    if (vn)
    {
        cerr << "Shouldn't have made anything" << endl;
        return 1;
    }
    cerr << "factory made nothing as it should have" << endl;

    vtkRenderWindow* rwin = vtkRenderWindow::New();
    vnf->RegisterOverride(rwin->GetClassName(), win_maker);
    cerr << "CREATE node for renderwindow" << endl;
    vn = vnf->CreateNode(rwin);

    cerr << "factory makes" << endl;
    cerr << vn << endl;
    cerr << "BUILD [" << endl;
    vn->Traverse(vtkViewNode::build);
    cerr << "]" << endl;

    cerr << "add renderer" << endl;
    vtkRenderer* ren = vtkRenderer::New();
    vnf->RegisterOverride(ren->GetClassName(), ren_maker);
    rwin->AddRenderer(ren);

    vtkLight* light = vtkLight::New();
    vnf->RegisterOverride(light->GetClassName(), light_maker);
    ren->AddLight(light);
    light->Delete();

    vnf->RegisterOverride("vtkMapper", mapper_maker);

    vtkCamera* cam = vtkCamera::New();
    vnf->RegisterOverride(cam->GetClassName(), cam_maker);
    cam->Delete();

    vtkActor* actor = vtkActor::New();
    vnf->RegisterOverride(actor->GetClassName(), act_maker);
    ren->AddActor(actor);
    actor->Delete();

    vtkSphereSource* sphere = vtkSphereSource::New();
    vtkPolyDataMapper* pmap = vtkPolyDataMapper::New();
    pmap->SetInputConnection(sphere->GetOutputPort());
    actor->SetMapper(pmap);

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(rwin);

    rwin->Render();
    interactor->Start();

    sphere->Delete();
    pmap->Delete();

    cerr << "BUILD [" << endl;
    vn->Traverse(vtkViewNode::build);
    cerr << "]" << endl;
    cerr << "SYNCHRONIZE [" << endl;
    vn->Traverse(vtkViewNode::synchronize);
    cerr << "]" << endl;
    cerr << "RENDER [" << endl;
    vn->Traverse(vtkViewNode::render);
    cerr << "]" << endl;

    vn->Delete();
    ren->Delete();
    rwin->Delete();

    vnf->Delete();

    cerr << "Results is [" << endl;
    cerr << resultS << "]" << endl;
    std::string ok_res = "make window\nmake renderer\nmake light\nmake actor\nmake camera\nmake "
                         "mapper\nRender vtkMyWindowNode\nRender vtkMyRendererNode\nRender "
                         "vtkMyLightNode\nRender vtkMyActorNode\nRender vtkMyMapperNode\nRender "
                         "vtkMyCameraNode\n";
    if (resultS != ok_res)
    {
        cerr << "Which does not match [" << endl;
        cerr << ok_res << "]" << endl;
        return 1;
    }
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <vtkActor.h>
#include <vtkGlyph3D.h>
#include <vtkGlyphSource2D.h>
#include <vtkGraphLayout.h>
#include <vtkGraphLayoutView.h>
#include <vtkGraphToPolyData.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSimple2DLayoutStrategy.h>

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkMutableDirectedGraph> g;

    vtkIdType v1 = g->AddVertex();
    vtkIdType v2 = g->AddVertex();
    vtkIdType v3 = g->AddVertex();

    g->AddEdge(v1, v2);
    g->AddEdge(v2, v3);
    g->AddEdge(v3, v1);

    // Do layout manually before handing graph to the view.
    // This allows us to know the positions of edge arrows.
    vtkNew<vtkGraphLayoutView> graphLayoutView;

    vtkNew<vtkGraphLayout> layout;
    vtkNew<vtkSimple2DLayoutStrategy> strategy;
    layout->SetInputData(g);
    layout->SetLayoutStrategy(strategy);

    // Tell the view to use the vertex layout we provide
    graphLayoutView->SetLayoutStrategyToPassThrough();
    // The arrows will be positioned on a straight line between two
    // vertices so tell the view not to draw arcs for parallel edges
    graphLayoutView->SetEdgeLayoutStrategyToPassThrough();

    // Add the graph to the view. This will render vertices and edges,
    // but not edge arrows.
    graphLayoutView->AddRepresentationFromInputConnection(layout->GetOutputPort());

    // Manually create an actor containing the glyphed arrows.
    vtkNew<vtkGraphToPolyData> graphToPoly;
    graphToPoly->SetInputConnection(layout->GetOutputPort());
    graphToPoly->EdgeGlyphOutputOn();

    // Set the position (0: edge start, 1: edge end) where
    // the edge arrows should go.
    graphToPoly->SetEdgeGlyphPosition(0.98);

    // Make a simple edge arrow for glyphing.
    vtkNew<vtkGlyphSource2D> arrowSource;
    arrowSource->SetGlyphTypeToEdgeArrow();
    arrowSource->SetScale(0.1);
    arrowSource->Update();

    // Use Glyph3D to repeat the glyph on all edges.
    vtkNew<vtkGlyph3D> arrowGlyph;
    arrowGlyph->SetInputConnection(0, graphToPoly->GetOutputPort(1));
    arrowGlyph->SetInputConnection(1, arrowSource->GetOutputPort());

    // Add the edge arrow actor to the view.
    vtkNew<vtkPolyDataMapper> arrowMapper;
    arrowMapper->SetInputConnection(arrowGlyph->GetOutputPort());
    vtkNew<vtkActor> arrowActor;
    arrowActor->SetMapper(arrowMapper);
    graphLayoutView->GetRenderer()->AddActor(arrowActor);

    graphLayoutView->GetRenderer()->SetBackground(colors->GetColor3d("SaddleBrown").GetData());
    graphLayoutView->GetRenderer()->SetBackground2(colors->GetColor3d("Wheat").GetData());
    graphLayoutView->GetRenderWindow()->SetWindowName("VisualizeDirectedGraph");
    graphLayoutView->ResetCamera();
    graphLayoutView->Render();
    graphLayoutView->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

#endif // TEST3

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
