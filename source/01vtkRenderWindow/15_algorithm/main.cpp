/**
 * 101. vtkCellLocator 查找距离输入点最近的单元
 * 102. vtkStaticCellLocator 使用了多线程，比 vtkCellLocator 更快
 *
 * 201. vtkPointLocator 查找距离输入点最近的网格顶点
 * 202. vtkStaticPointLocator 使用了多线程，当数据集很大时比 vtkPointLocator 更快
 *
 * 301. vtkOBBTree 几何体与线段的交点
 *
 * 401. 多边形每条边的外法向量
 * 402. vtkPolyDataNormals 生成多边形的法线
 *
 * 501. vtkSurfaceReconstructionFilter 从无组织的点集重建曲面
 * 502. vtkDelaunay2D 对输入的点集进行三角剖分，输出是一个 vtkPolyData 单元是三角形（或线段、顶点）
 * 503. vtkDelaunay3D 对输入的点集进行三角剖分，输出是一个 vtkUnstructuredGrid 单元是四面体（或三角形、线段、顶点）
 * 504. vtkDelaunay2D 对输入数据集的指定区域进行三角剖分，或者对指定区域不进行三角剖分
 *
 * 601. vtkTriangleFilter 将输入的多边形三角化，分解单元
 *
 */

#define TEST601

#ifdef TEST101

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkCubeSource.h>
#include <vtkGenericCell.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->SetXLength(3.0);
    source->SetYLength(3.0);
    source->SetZLength(3.0);
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.5);

    //-----------------------------------------------------------------------
    // 查找最近的单元，两个函数的结果是一样的
    vtkNew<vtkCellLocator> cellLocator;
    cellLocator->SetDataSet(source->GetOutput());
    cellLocator->BuildLocator();

    double point[3] { 4., 4., 0. }; // 需要查找的输入点坐标
    double closestPoint[3] {};      // 最近点的坐标(单元上的点，不一定是单元的顶点)
    vtkIdType cellId { 0 };         // 距离输入点最近的单元ID
    int subId { 0 };                // 单元的子单元ID，例如三角形带会有子单元
    double dist2 { 0.0 };           // 单元格到查找点的平方距离

    cellLocator->FindClosestPoint(point, closestPoint, cellId, subId, dist2); // 线程不安全

    std::cout << "closest point:\t" << closestPoint[0] << '\t' << closestPoint[1] << '\t' << closestPoint[2] << "\ncell id:\t" << cellId
              << "\nsub id:\t\t" << subId << "\ndist2:\t\t" << dist2 << '\n';

    vtkNew<vtkGenericCell> genericCell;
    cellLocator->FindClosestPoint(point, closestPoint, genericCell, cellId, subId, dist2); // 线程安全，并且速度更快

    std::cout << "closest point:\t" << closestPoint[0] << '\t' << closestPoint[1] << '\t' << closestPoint[2] << "\ncell id:\t" << cellId
              << "\nsub id:\t\t" << subId << "\ndist2:\t\t" << dist2 << '\n';

    //-----------------------------------------------------------------------
    // 查找到的最近点和查找点的连线
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(point);
    points->InsertNextPoint(closestPoint);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(polyData);

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(1, 0, 0);

    //-----------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST101

#ifdef TEST102

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGenericCell.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStaticCellLocator.h>

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->SetXLength(3.0);
    source->SetYLength(3.0);
    source->SetZLength(3.0);
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.5);

    //-----------------------------------------------------------------------
    // 查找最近的单元，两个函数的结果是一样的
    vtkNew<vtkStaticCellLocator> cellLocator;
    cellLocator->SetDataSet(source->GetOutput());
    cellLocator->BuildLocator();

    double point[3] { 4., 4., 0. }; // 需要查找的输入点坐标
    double closestPoint[3] {};      // 最近点的坐标(单元上的点，不一定是单元的顶点)
    vtkIdType cellId { 0 };         // 距离输入点最近的单元ID
    int subId { 0 };                // 单元的子单元ID，例如三角形带会有子单元
    double dist2 { 0.0 };           // 单元格到查找点的平方距离

    cellLocator->FindClosestPoint(point, closestPoint, cellId, subId, dist2); // 线程不安全

    std::cout << "closest point:\t" << closestPoint[0] << '\t' << closestPoint[1] << '\t' << closestPoint[2] << "\ncell id:\t" << cellId
              << "\nsub id:\t\t" << subId << "\ndist2:\t\t" << dist2 << '\n';

    vtkNew<vtkGenericCell> genericCell;
    cellLocator->FindClosestPoint(point, closestPoint, genericCell, cellId, subId, dist2); // 线程安全，并且速度更快

    std::cout << "closest point:\t" << closestPoint[0] << '\t' << closestPoint[1] << '\t' << closestPoint[2] << "\ncell id:\t" << cellId
              << "\nsub id:\t\t" << subId << "\ndist2:\t\t" << dist2 << '\n';

    //-----------------------------------------------------------------------
    // 查找到的最近点和查找点的连线
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(point);
    points->InsertNextPoint(closestPoint);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(polyData);

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(1, 0, 0);

    //-----------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST102

#ifdef TEST201

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointLocator.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
    vtkNew<vtkSphereSource> source;
    source->SetRadius(4.);
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.5);
    actor->GetProperty()->EdgeVisibilityOn();

    //-----------------------------------------------------------------------
    // 查找距离输入点最近的网格顶点
    double point[3] { 5., 0., 0. };
    vtkNew<vtkPointLocator> locator;
    locator->SetDataSet(source->GetOutput());
    locator->BuildLocator();

    // 查找到的顶点ID
    auto pointId = locator->FindClosestPoint(point);

    //-----------------------------------------------------------------------
    // 查找到的最近顶点和查找点的连线
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(point);
    points->InsertNextPoint(source->GetOutput()->GetPoint(pointId));

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(polyData);

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(1, 0, 0);

    //-----------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST201

#ifdef TEST202

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkStaticPointLocator.h>

int main(int, char*[])
{
    vtkNew<vtkSphereSource> source;
    source->SetRadius(4.);
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.5);
    actor->GetProperty()->EdgeVisibilityOn();

    //-----------------------------------------------------------------------
    // 查找距离输入点最近的网格顶点
    double point[3] { 5., 0., 0. };
    vtkNew<vtkStaticPointLocator> locator;
    locator->SetDataSet(source->GetOutput());
    locator->BuildLocator();

    // 查找到的顶点ID
    auto pointId = locator->FindClosestPoint(point);

    //-----------------------------------------------------------------------
    // 查找到的最近顶点和查找点的连线
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(point);
    points->InsertNextPoint(source->GetOutput()->GetPoint(pointId));

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(polyData);

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(1, 0, 0);

    //-----------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST202

#ifdef TEST301

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractCells.h>
#include <vtkIdList.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkOBBTree.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetPhiResolution(10);
    sphereSource->SetThetaResolution(10);
    sphereSource->Update();

    vtkNew<vtkOBBTree> tree;
    tree->SetDataSet(sphereSource->GetOutput());
    tree->BuildLocator();

    // 容差
    double tol = 1.e-8;
    tree->SetTolerance(tol);

    // 几何图形与输入线段的交点
    double lineP0[3] = { -0.5, -0.5, -0.5 };
    double lineP1[3] = { .5, .5, .5 };
    vtkNew<vtkPoints> intersectPoints;
    vtkNew<vtkIdList> intersectCells;
    tree->IntersectWithLine(lineP0, lineP1, intersectPoints, intersectCells);

    std::cout << "NumPoints:\t" << intersectPoints->GetNumberOfPoints() << std::endl;
    std::cout << "NumCells:\t" << intersectCells->GetNumberOfIds() << std::endl;

    //-----------------------------------------------------------------
    // 输入的线段
    vtkNew<vtkLineSource> lineSource;
    lineSource->SetPoint1(lineP0);
    lineSource->SetPoint2(lineP1);

    vtkNew<vtkPolyDataMapper> lineMapper;
    lineMapper->SetInputConnection(lineSource->GetOutputPort());
    vtkNew<vtkActor> lineActor;
    lineActor->SetMapper(lineMapper);

    //-----------------------------------------------------------------
    // 需要求交点的几何体
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetRepresentationToWireframe();
    sphereActor->GetProperty()->SetColor(1, 0, 0);

    //-----------------------------------------------------------------
    // 交点所在的单元
    vtkNew<vtkExtractCells> cellSource;
    cellSource->SetInputConnection(sphereSource->GetOutputPort());
    cellSource->SetCellList(intersectCells);

    vtkNew<vtkDataSetMapper> cellMapper;
    cellMapper->SetInputConnection(cellSource->GetOutputPort());
    vtkNew<vtkActor> cellActor;
    cellActor->SetMapper(cellMapper);
    cellActor->GetProperty()->SetColor(0, 1, 0);

    //-----------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(lineActor);
    renderer->AddActor(sphereActor);
    renderer->AddActor(cellActor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST301

#ifdef TEST401

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCellArray.h>
#include <vtkCellCenters.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3D.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main()
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(1., 0., 0.);
    points->InsertNextPoint(1., 1., 0.);
    points->InsertNextPoint(0., 1., 0.);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1 });
    cells->InsertNextCell({ 1, 2 });
    cells->InsertNextCell({ 2, 3 });
    cells->InsertNextCell({ 3, 0 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    // 取单元的中心
    vtkNew<vtkCellCenters> cellCenters;
    cellCenters->SetInputData(polyData);
    cellCenters->Update();

    // 设置单元的法向量
    vtkNew<vtkFloatArray> centerVectors;
    centerVectors->SetNumberOfComponents(3);
    for (vtkIdType i = 0; i < polyData->GetNumberOfCells(); ++i)
    {
        vtkNew<vtkIdList> pts;
        polyData->GetCellPoints(i, pts);

        double p0[3] {}, p1[3] {};
        polyData->GetPoint(pts->GetId(0), p0);
        polyData->GetPoint(pts->GetId(1), p1);

        // 逆时针多边形的外法线，公式见 https://zhuanlan.zhihu.com/p/272517099
        centerVectors->InsertNextTuple3(p1[1] - p0[1], p0[0] - p1[0], 0.);
    }
    cellCenters->GetOutput()->GetPointData()->SetVectors(centerVectors);

    vtkNew<vtkArrowSource> arrow;
    arrow->Update();

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputData(cellCenters->GetOutput());
    glyph->SetSourceConnection(arrow->GetOutputPort());
    glyph->SetScaleModeToDataScalingOff();
    glyph->SetVectorModeToUseVector();
    glyph->SetScaleFactor(0.2);
    glyph->Update();

    //-------------------------------------------------------------
    vtkNew<vtkPolyDataMapper> glyphMapper;
    glyphMapper->SetInputConnection(glyph->GetOutputPort());

    vtkNew<vtkPolyDataMapper> polyDataMapper;
    polyDataMapper->SetInputData(polyData);

    vtkNew<vtkActor> glyphActor;
    glyphActor->SetMapper(glyphMapper);
    glyphActor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkActor> polyDataActor;
    polyDataActor->SetMapper(polyDataMapper);
    polyDataActor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(polyDataActor);
    renderer->AddActor(glyphActor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST401

#ifdef TEST402

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCellArray.h>
#include <vtkGenericCell.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main()
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(1., 0., 0.);
    points->InsertNextPoint(1., 1., 0.);
    points->InsertNextPoint(0., 1., 0.);
    points->InsertNextPoint(0., 0., 1.);
    points->InsertNextPoint(1., 0., 1.);
    points->InsertNextPoint(1., 1., 1.);
    points->InsertNextPoint(0., 1., 1.);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1, 2, 3 });
    cells->InsertNextCell({ 1, 5, 6, 2 });
    cells->InsertNextCell({ 5, 4, 7, 6 });
    cells->InsertNextCell({ 4, 0, 3, 7 });
    cells->InsertNextCell({ 3, 2, 6, 7 });
    cells->InsertNextCell({ 4, 5, 1, 0 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputData(polyData);
    normals->ComputePointNormalsOn();  // 计算顶点的法线
    normals->FlipNormalsOn();          // 翻转法线
    normals->SetFeatureAngle(30.);     // 指定锐边的角度，如果相邻多边形的角度大于该值，则视为尖锐
    normals->SplittingOn();            // 开启锐边的分割
    normals->NonManifoldTraversalOn(); //
    normals->ConsistencyOn();
    normals->Update();

    // 锐边分割后，会将立方体的顶点扩展到4*6 = 24个，即每个四边形的四个顶点都有一个法线
    for (vtkIdType i = 0; i < normals->GetOutput()->GetNumberOfPoints(); ++i)
    {
        double pt[3] {};
        normals->GetOutput()->GetPoint(i, pt);
        std::cout << "point " << i << " : " << pt[0] << ' ' << pt[1] << ' ' << pt[2] << '\n';
    }

    for (vtkIdType i = 0; i < normals->GetOutput()->GetNumberOfCells(); ++i)
    {
        vtkNew<vtkIdList> pts;
        normals->GetOutput()->GetCellPoints(i, pts);
        std::cout << "cell " << i << " : " << pts->GetId(0) << ' ' << pts->GetId(1) << ' ' << pts->GetId(2) << ' ' << pts->GetId(3) << '\n';
    }

    vtkNew<vtkArrowSource> arrow;
    arrow->Update();

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputData(normals->GetOutput());
    glyph->SetSourceConnection(arrow->GetOutputPort());
    glyph->SetScaleModeToDataScalingOff();
    glyph->SetVectorModeToUseNormal();
    glyph->SetScaleFactor(0.2);
    glyph->Update();

    //-------------------------------------------------------------
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(normals->GetOutput());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(glyph->GetOutput());
    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST402

#ifdef TEST501

#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSurfaceReconstructionFilter.h>

namespace {
// clang-format off
std::vector<float> vertices { 
    0., 0., 0., 1., 0., 2., 0., 3.,
    1., 3., 1., 2., 1., 1., 1., 0.,
    2., 0., 2., 1., 2., 2., 2., 3.,
    3., 3., 3., 2., 3., 1., 3., 0. 
};

std::vector<int> indices {
    // 四条外部线
    0,  1,  1,  2,  2,  3,
   12, 13, 13, 14, 14, 15,
    0,  7,  7,  8,  8, 15,
    3,  4,  4, 11, 11, 12,

    // 内部线
    1, 6, 6,  9,  9, 14,
    2, 5, 5, 10, 10, 13,
    4, 5, 5,  6,  6,  7,
    8, 9, 9, 10, 10, 11,
};

std::vector<int> indicesPoly {
    0, 1,  6,  7, 1,  2,  5,  6,  2,  3,  4,  5,
    4, 5, 10, 11, 5,  6,  9, 10,  6,  7,  8,  9,
    8, 9, 14, 15, 9, 10, 13, 14, 10, 11, 12, 13,
};

std::vector<float> fields {
    1.0f,    1.0f,    1.0f,    1.0f,
    2.0f,    2.0f,    2.0f,    2.0f,
    3.0f,    3.0f,    3.0f,    3.0f,
    4.0f,    4.0f,    4.0f,    4.0f,
};

// clang-format on

enum class PolyType
{
    Vert,
    Line,
    Poly
};

vtkSmartPointer<vtkPolyData> GenPolyData(PolyType type)
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cellsVert;
    vtkNew<vtkCellArray> cellsLine;
    vtkNew<vtkCellArray> cellsPoly;

    for (size_t i = 0; i < vertices.size(); i += 2)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], 0.0);
    }
    for (size_t i = 0; i < vertices.size() / 2; i++)
    {
        auto id = static_cast<vtkIdType>(i);
        cellsVert->InsertNextCell(1, &id);
    }
    for (size_t i = 0; i < indices.size(); i += 2)
    {
        cellsLine->InsertNextCell({ indices[i], indices[i + 1] });
    }
    for (size_t i = 0; i < indicesPoly.size(); i += 4)
    {
        cellsPoly->InsertNextCell({ indicesPoly[i], indicesPoly[i + 1], indicesPoly[i + 2], indicesPoly[i + 3] });
    }

    polyData->SetPoints(points);
    switch (type)
    {
    case PolyType::Vert:
        polyData->SetVerts(cellsVert);
        break;
    case PolyType::Line:
        polyData->SetLines(cellsLine);
        break;
    case PolyType::Poly:
        polyData->SetPolys(cellsPoly);
        break;
    default:
        break;
    }

    vtkNew<vtkFloatArray> scalars;
    for (size_t i = 0; i < 16; ++i)
    {
        scalars->InsertNextValue(fields[i]);
    }

    polyData->GetPointData()->SetScalars(scalars);

    return polyData;
}
} // namespace

/**
 * 重建的曲面可能要比原来顶点的范围大一些
 *
 * 使用VTKsurfaceReconstructionFilter时，主要涉及两个参数，分别使用函数SetNeighborhoodSize（）和SetSampleSpacing（）进行设置。
 * SetNeighborhoodSize：
 *     设置邻域点的个数；而这些邻域点则用来估计每个点的局部切平面。
 *     邻域点的个数默认为20，能够处理大多数重建问题。个数设置越多，计算消耗时间越长。当点云分布严重不均匀情况下，可以考虑增加该值。
 * SetSampleSpacing：
 *     用于设置划分网格的网格间距，间距越小，网格越密集，一般采用默认值0.05.
 */

int main()
{
    auto polyData = GenPolyData(PolyType::Vert);

    vtkNew<vtkPolyData> points;
    points->SetPoints(polyData->GetPoints()); // 获得网格模型中的几何数据：点集

    // 生成的是一个 vtkImageData
    vtkNew<vtkSurfaceReconstructionFilter> surf;
    surf->SetInputData(points);
    surf->SetNeighborhoodSize(20);
    surf->SetSampleSpacing(0.5);
    surf->Update();

    vtkNew<vtkContourFilter> contour;
    contour->SetInputConnection(surf->GetOutputPort());
    contour->SetValue(0, 0.0);
    contour->Update();

    //----------------------------------------------------------
    auto np = polyData->GetNumberOfPoints();
    auto nc = polyData->GetNumberOfCells();
    double bounds[6] {};
    polyData->GetBounds(bounds);
    std::cout << "Points:\t" << np << "\tCells:\t" << nc << '\n'
              << "Bounds:\t" << bounds[0] << ' ' << bounds[1] << '\t' << bounds[2] << ' ' << bounds[3] << '\t' << bounds[4] << ' ' << bounds[5]
              << '\n';
    vtkNew<vtkPolyDataMapper> pointMapper;
    pointMapper->SetInputData(polyData);
    pointMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> pointActor;
    pointActor->SetMapper(pointMapper);
    pointActor->GetProperty()->SetColor(1, 0, 0);
    pointActor->GetProperty()->SetPointSize(4);

    //----------------------------------------------------------
    auto np2 = contour->GetOutput()->GetNumberOfPoints();
    auto nc2 = contour->GetOutput()->GetNumberOfCells();
    contour->GetOutput()->GetBounds(bounds);
    std::cout << "Points:\t" << np2 << "\tCells:\t" << nc2 << '\n'
              << "Bounds:\t" << bounds[0] << ' ' << bounds[1] << '\t' << bounds[2] << ' ' << bounds[3] << '\t' << bounds[4] << ' ' << bounds[5]
              << '\n';

    vtkNew<vtkPolyDataMapper> contourMapper;
    contourMapper->SetInputData(contour->GetOutput());

    vtkNew<vtkActor> contourActor;
    contourActor->SetMapper(contourMapper);
    contourActor->GetProperty()->EdgeVisibilityOn();

    //----------------------------------------------------------
    vtkNew<vtkRenderer> leftRender;
    leftRender->AddActor(pointActor);
    leftRender->SetViewport(0., 0., .5, 1.);
    leftRender->SetBackground(.1, .2, .3);
    leftRender->ResetCamera();

    vtkNew<vtkRenderer> rightRender;
    rightRender->AddActor(contourActor);
    rightRender->SetViewport(.5, 0., 1., 1.);
    rightRender->SetBackground(.3, .2, .1);
    rightRender->SetActiveCamera(leftRender->GetActiveCamera());
    rightRender->ResetCamera();

    vtkNew<vtkRenderWindow> rw;
    rw->AddRenderer(leftRender);
    rw->AddRenderer(rightRender);
    rw->SetSize(1200, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> rwi;
    rwi->SetRenderWindow(rw);
    rwi->SetInteractorStyle(style);

    rw->Render();
    rwi->Start();

    return 0;
}

#endif // TEST501

#ifdef TEST502

#include <vtkActor.h>
#include <vtkBoundedPointSource.h>
#include <vtkDelaunay2D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPlaneSource.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

enum class PrimitiveType
{
    Sphere, // 在一个球内生成一堆顶点
    Plane,  // 在一个平面上生成一堆顶点（Z值不变）
    Cube,   // 在一个立方体内生成一堆顶点
};

vtkSmartPointer<vtkPolyData> GenPolyData(PrimitiveType pt)
{
    vtkNew<vtkPointSource> point;
    point->SetCenter(0.0, 0.0, 0.0);
    point->SetNumberOfPoints(100);
    point->SetDistributionToUniform();
    point->SetRadius(10.);
    point->Update();

    vtkNew<vtkPlaneSource> plane;
    plane->SetResolution(10, 10);
    plane->SetPoint1(10., 0., 0.);
    plane->SetPoint2(0., 10., 0.);
    plane->Update();

    vtkNew<vtkBoundedPointSource> cube;
    cube->SetNumberOfPoints(100);
    cube->SetBounds(-2., 2., -2., 2., -2., 2);
    cube->Update();

    vtkNew<vtkCellArray> cells;
    for (vtkIdType i = 0; i < 100; ++i)
    {
        vtkIdType pts = i;
        cells->InsertNextCell(1, &pts);
    }

    cube->GetOutput()->SetVerts(cells);

    switch (pt)
    {
    case PrimitiveType::Sphere:
        return point->GetOutput();
    case PrimitiveType::Plane:
        return plane->GetOutput();
    case PrimitiveType::Cube:
        return cube->GetOutput();
    default:
        return plane->GetOutput();
    }
}
} // namespace

int main()
{
    auto polyData = GenPolyData(PrimitiveType::Plane);

    vtkNew<vtkPolyData> points;
    points->SetPoints(polyData->GetPoints()); // 获得网格模型中的几何数据：点集

    vtkNew<vtkDelaunay2D> delaunay;
    delaunay->SetInputData(points);
    // delaunay->SetAlpha(10.);
    // delaunay->SetBoundingTriangulation(true);    // 和Alpha一起控制输出图形半径
    // delaunay->SetOffset(1.);
    // delaunay->SetTolerance(0.00001);             // 设置公差，控制密集点的丢弃
    // delaunay->SetSourceData(nullptr);
    // delaunay->SetProjectionPlaneMode(VTK_BEST_FITTING_PLANE); // 3维点投影到2维平面的方法 默认：VTK_DELAUNAY_XY_PLANE
    delaunay->Update();

    //----------------------------------------------------------
    // 输入数据集
    auto np = polyData->GetNumberOfPoints();
    auto nc = polyData->GetNumberOfCells();
    double bounds[6] {};
    polyData->GetBounds(bounds);
    std::cout << "Points:\t" << np << "\tCells:\t" << nc << '\n'
              << "Bounds:\t" << bounds[0] << ' ' << bounds[1] << '\t' << bounds[2] << ' ' << bounds[3] << '\t' << bounds[4] << ' ' << bounds[5]
              << '\n';
    vtkNew<vtkPolyDataMapper> pointMapper;
    pointMapper->SetInputData(polyData);
    pointMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> pointActor;
    pointActor->SetMapper(pointMapper);
    pointActor->GetProperty()->SetColor(1., 0., 0.);
    pointActor->GetProperty()->SetPointSize(4.f);

    //----------------------------------------------------------
    // 三角剖分的结果
    auto np2 = delaunay->GetOutput()->GetNumberOfPoints();
    auto nc2 = delaunay->GetOutput()->GetNumberOfCells();
    delaunay->GetOutput()->GetBounds(bounds);
    std::cout << "Points:\t" << np2 << "\tCells:\t" << nc2 << '\n'
              << "Bounds:\t" << bounds[0] << ' ' << bounds[1] << '\t' << bounds[2] << ' ' << bounds[3] << '\t' << bounds[4] << ' ' << bounds[5]
              << '\n';

    vtkNew<vtkPolyDataMapper> delaunayMapper;
    delaunayMapper->SetInputData(delaunay->GetOutput());

    vtkNew<vtkActor> delaunayActor;
    delaunayActor->SetMapper(delaunayMapper);
    delaunayActor->GetProperty()->SetColor(0., 1., 0.);
    delaunayActor->GetProperty()->EdgeVisibilityOn();

    //----------------------------------------------------------
    vtkNew<vtkRenderer> leftRender;
    leftRender->AddActor(pointActor);
    leftRender->SetViewport(0., 0., .5, 1.);
    leftRender->SetBackground(.1, .2, .3);
    leftRender->ResetCamera();

    vtkNew<vtkRenderer> rightRender;
    rightRender->AddActor(delaunayActor);
    rightRender->SetViewport(.5, 0., 1., 1.);
    rightRender->SetBackground(.3, .2, .1);
    rightRender->SetActiveCamera(leftRender->GetActiveCamera());
    rightRender->ResetCamera();

    vtkNew<vtkRenderWindow> rw;
    rw->AddRenderer(leftRender);
    rw->AddRenderer(rightRender);
    rw->SetSize(1200, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> rwi;
    rwi->SetRenderWindow(rw);
    rwi->SetInteractorStyle(style);

    rw->Render();
    rwi->Start();

    return 0;
}

#endif // TEST502

#ifdef TEST503

#include <vtkActor.h>
#include <vtkBoundedPointSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDelaunay3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPlaneSource.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>

namespace {

enum class PrimitiveType
{
    Sphere, // 在一个球内生成一堆顶点
    Plane,  // 在一个平面上生成一堆顶点（Z值不变）
    Cube,   // 在一个立方体内生成一堆顶点
};

vtkSmartPointer<vtkPolyData> GenPolyData(PrimitiveType pt)
{
    vtkNew<vtkPointSource> point;
    point->SetCenter(0.0, 0.0, 0.0);
    point->SetNumberOfPoints(100);
    point->SetDistributionToUniform();
    point->SetRadius(10.);
    point->Update();

    vtkNew<vtkPlaneSource> plane;
    plane->SetResolution(10, 10);
    plane->SetPoint1(10., 0., 0.);
    plane->SetPoint2(0., 10., 0.);
    plane->Update();

    vtkNew<vtkBoundedPointSource> cube;
    cube->SetNumberOfPoints(100);
    cube->SetBounds(-2., 2., -2., 2., -2., 2);
    cube->Update();

    vtkNew<vtkCellArray> cells;
    for (vtkIdType i = 0; i < 100; ++i)
    {
        vtkIdType pts = i;
        cells->InsertNextCell(1, &pts);
    }

    cube->GetOutput()->SetVerts(cells);

    switch (pt)
    {
    case PrimitiveType::Sphere:
        return point->GetOutput();
    case PrimitiveType::Plane:
        return plane->GetOutput();
    case PrimitiveType::Cube:
        return cube->GetOutput();
    default:
        return plane->GetOutput();
    }
}
} // namespace

int main()
{
    auto polyData = GenPolyData(PrimitiveType::Cube);

    vtkNew<vtkPolyData> points;
    points->SetPoints(polyData->GetPoints()); // 获得网格模型中的几何数据：点集

    vtkNew<vtkDelaunay3D> delaunay;
    delaunay->SetInputData(points);
    // delaunay->SetAlpha(3.);
    // delaunay->SetAlphaLines(true);
    // delaunay->SetAlphaTris(true);
    // delaunay->SetAlphaTets(true);
    // delaunay->SetAlphaVerts(true);
    // delaunay->SetBoundingTriangulation(true); // 和Alpha一起控制输出图形半径
    // delaunay->SetOffset(1.);
    // delaunay->SetTolerance(0.00001); // 设置公差，控制密集点的丢弃
    // delaunay->SetLocator(nullptr);   // 设置用于合并点的空间定位器
    delaunay->Update();

    //----------------------------------------------------------
    // 输入数据集
    auto np = polyData->GetNumberOfPoints();
    auto nc = polyData->GetNumberOfCells();
    double bounds[6] {};
    polyData->GetBounds(bounds);
    std::cout << "Points:\t" << np << "\tCells:\t" << nc << '\n'
              << "Bounds:\t" << bounds[0] << ' ' << bounds[1] << '\t' << bounds[2] << ' ' << bounds[3] << '\t' << bounds[4] << ' ' << bounds[5]
              << '\n';
    vtkNew<vtkPolyDataMapper> pointMapper;
    pointMapper->SetInputData(polyData);
    pointMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> pointActor;
    pointActor->SetMapper(pointMapper);
    pointActor->GetProperty()->SetColor(1., 0., 0.);
    pointActor->GetProperty()->SetPointSize(4.f);

    //----------------------------------------------------------
    // 三角剖分的结果
    auto np2 = delaunay->GetOutput()->GetNumberOfPoints();
    auto nc2 = delaunay->GetOutput()->GetNumberOfCells();
    delaunay->GetOutput()->GetBounds(bounds);
    std::cout << "Points:\t" << np2 << "\tCells:\t" << nc2 << '\n'
              << "Bounds:\t" << bounds[0] << ' ' << bounds[1] << '\t' << bounds[2] << ' ' << bounds[3] << '\t' << bounds[4] << ' ' << bounds[5]
              << '\n';

    for (vtkIdType i = 0; i < delaunay->GetOutput()->GetNumberOfCells(); ++i)
    {
        // VTK_TETRA 四面体
        // std::cout<< delaunay->GetOutput()->GetCellType(i) << '\n';
    }

    vtkNew<vtkDataSetMapper> delaunayMapper;
    delaunayMapper->SetInputData(delaunay->GetOutput());

    vtkNew<vtkActor> delaunayActor;
    delaunayActor->SetMapper(delaunayMapper);
    delaunayActor->GetProperty()->SetColor(0., 1., 0.);
    delaunayActor->GetProperty()->EdgeVisibilityOn();

    //----------------------------------------------------------
    vtkNew<vtkRenderer> leftRender;
    leftRender->AddActor(pointActor);
    leftRender->SetViewport(0., 0., .5, 1.);
    leftRender->SetBackground(.1, .2, .3);
    leftRender->ResetCamera();

    vtkNew<vtkRenderer> rightRender;
    rightRender->AddActor(delaunayActor);
    rightRender->SetViewport(.5, 0., 1., 1.);
    rightRender->SetBackground(.3, .2, .1);
    rightRender->SetActiveCamera(leftRender->GetActiveCamera());
    rightRender->ResetCamera();

    vtkNew<vtkRenderWindow> rw;
    rw->AddRenderer(leftRender);
    rw->AddRenderer(rightRender);
    rw->SetSize(1200, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> rwi;
    rwi->SetRenderWindow(rw);
    rwi->SetInteractorStyle(style);

    rw->Render();
    rwi->Start();

    return 0;
}

#endif // TEST503

#ifdef TEST504

#include <vtkActor.h>
#include <vtkDelaunay2D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

vtkSmartPointer<vtkPolyData> GenPolyData()
{
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;
    for (size_t i = 0; i < 10; ++i)
    {
        for (size_t j = 0; j < 10; ++j)
        {
            points->InsertNextPoint(static_cast<double>(i), static_cast<double>(j), 0.);
            vtkIdType id = i * 10 + j;
            cells->InsertNextCell(1, &id);
        }
    }

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetVerts(cells);

    return polyData;
}

vtkSmartPointer<vtkPolyData> GenSourceData(vtkPolyData* input)
{
    // 顺时针多边形
    vtkNew<vtkCellArray> cw_cells;
    cw_cells->InsertNextCell({ 33, 34, 35, 36, 46, 56, 66, 65, 64, 63, 53, 43 });
    // 逆时针多边形
    vtkNew<vtkCellArray> ccw_cells;
    ccw_cells->InsertNextCell({ 33, 43, 53, 63, 64, 65, 66, 56, 46, 36, 35, 34 });

    // 线段构成的环形闭合区域
    vtkNew<vtkCellArray> lines;
    lines->InsertNextCell({ 33, 34 });
    lines->InsertNextCell({ 34, 44 });
    lines->InsertNextCell({ 44, 43 });
    lines->InsertNextCell({ 43, 33 });

    vtkNew<vtkPolyData> output;
    output->SetPoints(input->GetPoints()); // 必须和需要进行三角剖分的顶点相同
    output->SetPolys(cw_cells); // 顺时针对应的多边形区域不进行三角剖分，逆时针只对逆时针所在的区域进行三角剖分
    // output->SetLines(lines); // 线段即使能构成一个环形的闭合区域，也不能用作三角剖分的SourceData

    return output;
}

} // namespace

int main()
{
    auto polyData   = GenPolyData();
    auto sourceData = GenSourceData(polyData);

    vtkNew<vtkPolyData> points;
    points->SetPoints(polyData->GetPoints()); // 获得网格模型中的几何数据：点集

    vtkNew<vtkDelaunay2D> delaunay;
    delaunay->SetInputData(points);
    delaunay->SetSourceData(sourceData);
    delaunay->Update();

    //----------------------------------------------------------
    // 输入数据集
    auto np = polyData->GetNumberOfPoints();
    auto nc = polyData->GetNumberOfCells();
    double bounds[6] {};
    polyData->GetBounds(bounds);
    std::cout << "Points:\t" << np << "\tCells:\t" << nc << '\n'
              << "Bounds:\t" << bounds[0] << ' ' << bounds[1] << '\t' << bounds[2] << ' ' << bounds[3] << '\t' << bounds[4] << ' ' << bounds[5]
              << '\n';
    vtkNew<vtkPolyDataMapper> pointMapper;
    pointMapper->SetInputData(polyData);
    pointMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> pointActor;
    pointActor->SetMapper(pointMapper);
    pointActor->GetProperty()->SetColor(1., 0., 0.);
    pointActor->GetProperty()->SetPointSize(4.f);

    //----------------------------------------------------------
    // 三角剖分的结果
    auto np2 = delaunay->GetOutput()->GetNumberOfPoints();
    auto nc2 = delaunay->GetOutput()->GetNumberOfCells();
    delaunay->GetOutput()->GetBounds(bounds);
    std::cout << "Points:\t" << np2 << "\tCells:\t" << nc2 << '\n'
              << "Bounds:\t" << bounds[0] << ' ' << bounds[1] << '\t' << bounds[2] << ' ' << bounds[3] << '\t' << bounds[4] << ' ' << bounds[5]
              << '\n';

    vtkNew<vtkPolyDataMapper> delaunayMapper;
    delaunayMapper->SetInputData(delaunay->GetOutput());

    vtkNew<vtkActor> delaunayActor;
    delaunayActor->SetMapper(delaunayMapper);
    delaunayActor->GetProperty()->SetColor(0., 1., 0.);
    delaunayActor->GetProperty()->EdgeVisibilityOn();

    //----------------------------------------------------------
    vtkNew<vtkRenderer> leftRender;
    leftRender->AddActor(pointActor);
    leftRender->SetViewport(0., 0., .5, 1.);
    leftRender->SetBackground(.1, .2, .3);
    leftRender->ResetCamera();

    vtkNew<vtkRenderer> rightRender;
    rightRender->AddActor(delaunayActor);
    rightRender->SetViewport(.5, 0., 1., 1.);
    rightRender->SetBackground(.3, .2, .1);
    rightRender->SetActiveCamera(leftRender->GetActiveCamera());
    rightRender->ResetCamera();

    vtkNew<vtkRenderWindow> rw;
    rw->AddRenderer(leftRender);
    rw->AddRenderer(rightRender);
    rw->SetSize(1200, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> rwi;
    rwi->SetRenderWindow(rw);
    rwi->SetInteractorStyle(style);

    rw->Render();
    rwi->Start();

    return 0;
}

#endif // TEST504

#ifdef TEST601

#include <vector>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTriangleFilter.h>

namespace {

enum class PrimitiveType
{
    Vert,   // 一堆顶点，所有顶点只有一个单元
    Verts,  // 一堆顶点，几个顶点则几个单元
    Lines,  // 四条线构成一个四边形
    Line,   // 一条折线构成一个四边形
    Poly,   // 四边形
    Strip,  // 三角形带
    Plane,  // 四边形构成的平面
    Circle, // 圆（一个多边形）
};

vtkSmartPointer<vtkPolyData> GenPolyData(PrimitiveType pt)
{
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> verts;
    std::vector<vtkIdType> tmpCell;
    for (size_t i = 0; i < 10; ++i)
    {
        for (size_t j = 0; j < 10; ++j)
        {
            points->InsertNextPoint(static_cast<double>(i), static_cast<double>(j), 0.);
            vtkIdType id = i * 10 + j;
            verts->InsertNextCell(1, &id);
            tmpCell.emplace_back(id);
        }
    }

    vtkNew<vtkCellArray> vert;
    vert->InsertNextCell(static_cast<vtkIdType>(tmpCell.size()), tmpCell.data());

    vtkNew<vtkCellArray> lines;
    lines->InsertNextCell({ 0, 90 });
    lines->InsertNextCell({ 90, 99 });
    lines->InsertNextCell({ 99, 9 });
    lines->InsertNextCell({ 9, 0 });

    vtkNew<vtkCellArray> line;
    line->InsertNextCell({ 0, 90, 99, 9, 0 });

    vtkNew<vtkCellArray> polys;
    polys->InsertNextCell({ 0, 90, 99, 9 });

    // 三角形带实际只有一个单元，但是有多个三角形
    // 经过三角形过滤器之后就会将每个三角形作为一个单元
    vtkNew<vtkCellArray> strips;
    strips->InsertNextCell({ 9, 0, 19, 10, 29, 20, 99, 90 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);

    vtkNew<vtkRegularPolygonSource> regular;
    regular->SetNumberOfSides(10);
    regular->SetRadius(5.0);
    regular->GeneratePolygonOn();
    regular->GeneratePolylineOff();
    regular->Update();

    vtkNew<vtkPlaneSource> plane;
    plane->SetResolution(10, 10);
    plane->SetPoint1(10., 0., 0.);
    plane->SetPoint2(0., 10., 0.);
    plane->Update();

    switch (pt)
    {
    case PrimitiveType::Vert:
        polyData->SetVerts(vert);
        break;
    case PrimitiveType::Verts:
        polyData->SetVerts(verts);
        break;
    case PrimitiveType::Lines:
        polyData->SetLines(lines);
        break;
    case PrimitiveType::Line:
        polyData->SetLines(line);
        break;
    case PrimitiveType::Strip:
        polyData->SetStrips(strips);
        break;
    case PrimitiveType::Poly:
        polyData->SetPolys(polys);
        break;
    case PrimitiveType::Plane:
        return plane->GetOutput();
    case PrimitiveType::Circle:
        return regular->GetOutput();
    default:
        break;
    }

    return polyData;
}

} // namespace

// vtkTriangleFilter 可以将多边形分解为多个三角形
// 将三角形带分解为多个三角形
// 将折线（一个单元多个线条）分解为多个线条（每个线条一个单元）
// 将顶点单元分解为多个顶点（同折线）
// 不能将一堆顶点构造为三角形（只能对多边形三角化）

int main()
{
    auto polyData = GenPolyData(PrimitiveType::Circle);

    vtkNew<vtkTriangleFilter> filter;
    filter->SetInputData(polyData);
    filter->PassLinesOn(); // 如果开启会将输入的折线分解为单独的线段，关闭则不会有任何线类型的单元输出，默认开启
    filter->PassVertsOn(); // 如果开启，会将输入的顶点单元分解为单独的顶点单元，关闭则不会输出任何顶点单元，默认开启
    filter->Update();

    //----------------------------------------------------------
    // 输入数据集
    auto np = polyData->GetNumberOfPoints();
    auto nc = polyData->GetNumberOfCells();
    double bounds[6] {};
    polyData->GetBounds(bounds);
    std::cout << "Points:\t" << np << "\tCells:\t" << nc << '\n'
              << "Bounds:\t" << bounds[0] << ' ' << bounds[1] << '\t' << bounds[2] << ' ' << bounds[3] << '\t' << bounds[4] << ' ' << bounds[5]
              << '\n';
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1., 0., 0.);
    actor->GetProperty()->SetPointSize(4.f);
    actor->GetProperty()->EdgeVisibilityOn();

    //----------------------------------------------------------
    // 过滤器的结果
    auto np2 = filter->GetOutput()->GetNumberOfPoints();
    auto nc2 = filter->GetOutput()->GetNumberOfCells();
    filter->GetOutput()->GetBounds(bounds);
    std::cout << "Points:\t" << np2 << "\tCells:\t" << nc2 << '\n'
              << "Bounds:\t" << bounds[0] << ' ' << bounds[1] << '\t' << bounds[2] << ' ' << bounds[3] << '\t' << bounds[4] << ' ' << bounds[5]
              << '\n';

    vtkNew<vtkPolyDataMapper> filterMapper;
    filterMapper->SetInputData(filter->GetOutput());

    vtkNew<vtkActor> filterActor;
    filterActor->SetMapper(filterMapper);
    filterActor->GetProperty()->SetColor(0., 1., 0.);
    filterActor->GetProperty()->EdgeVisibilityOn();

    //----------------------------------------------------------
    vtkNew<vtkRenderer> leftRender;
    leftRender->AddActor(actor);
    leftRender->SetViewport(0., 0., .5, 1.);
    leftRender->SetBackground(.1, .2, .3);
    leftRender->ResetCamera();

    vtkNew<vtkRenderer> rightRender;
    rightRender->AddActor(filterActor);
    rightRender->SetViewport(.5, 0., 1., 1.);
    rightRender->SetBackground(.3, .2, .1);
    rightRender->SetActiveCamera(leftRender->GetActiveCamera());
    rightRender->ResetCamera();

    vtkNew<vtkRenderWindow> rw;
    rw->AddRenderer(leftRender);
    rw->AddRenderer(rightRender);
    rw->SetSize(1200, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> rwi;
    rwi->SetRenderWindow(rw);
    rwi->SetInteractorStyle(style);

    rw->Render();
    rwi->Start();

    return 0;
}

#endif // TEST601
