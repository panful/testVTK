/**
 * 101. vtkCellLocator 查找距离输入点最近的单元
 * 102. vtkStaticCellLocator 使用了多线程，比 vtkCellLocator 更快
 *
 * 201. vtkPointLocator 查找距离输入点最近的网格顶点
 * 202. vtkStaticPointLocator 使用了多线程，当数据集很大时比 vtkPointLocator 更快
 *
 * 301. vtkOBBTree 几何体与线段的交点
 */

#define TEST202

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
#include <vtkStaticCellLocator.h>
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
#include <vtkStaticPointLocator.h>
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
