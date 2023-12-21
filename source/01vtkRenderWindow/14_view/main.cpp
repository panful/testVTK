/**
 * 1. vtkRenderView vtkView
 * 2. vtkViewNode SceneGraph
 * 3. vtkGraphLayoutView 图 有向图 Graph
 */

#define TEST3

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