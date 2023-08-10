//
// cgns文件下载地址：https://cgns.github.io/CGNSFiles.html
//

/*
 * 1. 结构cgns
 * 2. 非结构cgns
 * 3. FLUENT *.cas *.dat 只绘制图形，不映射流场数据 FLUENT控制台使用 f c n wcd fileName 导出 fileName.cas和fileName.dat
 * 4. FLUENT *.cas *.dat 将流场数据映射到颜色
 * 5. 构造vtkUnstructuredGrid，四面体、六面体、棱柱、棱锥等图形
 */

#define TEST5

#ifdef TEST1

#include <vtkActor.h>
#include <vtkCGNSReader.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>

int main(int argc, char* argv[])
{
    vtkNew<vtkDataSetMapper> mapper;
    // std::string fileName("../resources/dlr-f6.coar.cgns");
    std::string fileName("../resources/sqnz_s.adf.cgns");

    std::cout << "Opening " << fileName.c_str() << endl;
    vtkNew<vtkCGNSReader> mixedReader;
    mixedReader->SetFileName(fileName.c_str());
    mixedReader->Update();

    auto mb = mixedReader->GetOutput();
    auto nb = mb->GetNumberOfBlocks();

    std::cout << "number of blocks: " << nb << '\n';

    vtkNew<vtkRenderer> renderer;
    for (unsigned int i = 0; i < nb; ++i)
    {
        auto mb2 = vtkMultiBlockDataSet::SafeDownCast(mb->GetBlock(i));
        for (unsigned int j = 0; j < mb2->GetNumberOfBlocks(); ++j)
        {
            auto sg = vtkStructuredGrid::SafeDownCast(mb2->GetBlock(j));
            std::cout << "------------------------------------------------------------\n=== Block ===\t(" << i << "," << j << ")\t=== Block ===\n";
            sg->Print(std::cout);

            vtkNew<vtkDataSetMapper> mapper;
            mapper->SetInputData(sg);
            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper);

            renderer->AddActor(actor);
        }
    }

    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST1

#ifdef TEST2

#include <vtkActor.h>
#include <vtkCGNSReader.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>

int main(int argc, char* argv[])
{
    vtkNew<vtkDataSetMapper> mapper;
    // std::string fileName("../resources/yf17final.cgns");
    std::string fileName("../resources/StaticMixer.cgns");

    std::cout << "Opening " << fileName.c_str() << endl;
    vtkNew<vtkCGNSReader> mixedReader;
    mixedReader->SetFileName(fileName.c_str());
    mixedReader->Update();

    auto mb = mixedReader->GetOutput();
    auto nb = mb->GetNumberOfBlocks();

    std::cout << "number of blocks: " << nb << '\n';

    vtkNew<vtkRenderer> renderer;
    for (unsigned int i = 0; i < nb; ++i)
    {
        auto mb2 = vtkMultiBlockDataSet::SafeDownCast(mb->GetBlock(i));
        for (unsigned int j = 0; j < mb2->GetNumberOfBlocks(); ++j)
        {
            auto ug = vtkUnstructuredGrid::SafeDownCast(mb2->GetBlock(j));
            std::cout << "------------------------------------------------------------\n=== Block ===\t(" << i << "," << j << ")\t=== Block ===\n";
            ug->Print(std::cout);

            vtkNew<vtkDataSetMapper> mapper;
            mapper->SetInputData(ug);
            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper);

            renderer->AddActor(actor);
        }
    }

    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkFLUENTReader.h>
#include <vtkFieldData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

int main()
{
    // *.dat必须和*.cas在同一路径下，且前缀相同（文件名相同）
    const char* fileName = "../resources/sqnz_s.adf.cas";
    vtkNew<vtkFLUENTReader> fluent;
    fluent->SetFileName(fileName);
    fluent->EnableAllCellArrays();
    fluent->Update();

    auto dataSet   = fluent->GetOutput();
    auto numBlocks = dataSet->GetNumberOfBlocks();

    std::cout << "NumOfBlocks: " << numBlocks << '\n';

    vtkNew<vtkRenderer> renderer;
    for (uint32_t i = 0; i < numBlocks; ++i)
    {
        auto usg = vtkUnstructuredGrid::SafeDownCast(dataSet->GetBlock(i));
        std::cout << "---------------------------------------------\n=== Block " << i << " ===\n";
        usg->Print(std::cout);

        vtkNew<vtkDataSetMapper> mapper;
        mapper->SetInputData(usg);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        renderer->AddActor(actor);
    }

    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkFLUENTReader.h>
#include <vtkFieldData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

/*
 *   Array 0 name = PRESSURE
 *   Array 1 name = X_VELOCITY
 *   Array 2 name = Y_VELOCITY
 *   Array 3 name = Z_VELOCITY
 *   Array 4 name = TKE
 *   Array 5 name = SDR
 *   Array 6 name = DENSITY
 *   Array 7 name = MU_LAM
 *   Array 8 name = MU_TURB
 *   Array 9 name = WALL_DIST
 *   Array 10 name = BODY_FORCES
 */

int main()
{
    // *.dat必须和*.cas在同一路径下，且前缀相同（文件名相同）
    const char* fileName = "../resources/sqnz_s.adf.cas";
    vtkNew<vtkFLUENTReader> fluent;
    fluent->SetFileName(fileName);
    fluent->EnableAllCellArrays();
    fluent->Update();

    auto dataSet   = fluent->GetOutput();
    auto numBlocks = dataSet->GetNumberOfBlocks();

    std::cout << "NumOfBlocks: " << numBlocks << '\n';

    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(20);
    lut->SetHueRange(0.67, 0);
    lut->SetRange(-3.6, 0.69);
    lut->Build();

    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLookupTable(lut);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(scalarBar);

    for (uint32_t i = 0; i < numBlocks; ++i)
    {
        auto usg = vtkUnstructuredGrid::SafeDownCast(dataSet->GetBlock(i));
        std::cout << "---------------------------------------------\n=== Block " << i << " ===\n";

        auto tmp_range = usg->GetCellData()->GetArray("PRESSURE")->GetRange();
        std::cout << "Range: " << tmp_range[0] << '\t' << tmp_range[1] << '\n';

        vtkNew<vtkDataSetMapper> mapper;
        mapper->SetInputData(usg);
        mapper->SetLookupTable(lut);
        mapper->ScalarVisibilityOn();
        mapper->SetScalarModeToUseCellFieldData();
        mapper->SelectColorArray("PRESSURE");
        mapper->SetScalarRange(-3.6, 0.69);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        renderer->AddActor(actor);
    }

    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST4

#ifdef TEST5

#include <iostream>
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

int main()
{
    vtkNew<vtkPoints> points;
    // 0,1,2
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(2, 0, 0);
    points->InsertNextPoint(1, 2, 0);

    // 3,4,5,6
    points->InsertNextPoint(3, 0, 0);
    points->InsertNextPoint(5, 0, 0);
    points->InsertNextPoint(4, 0, 2);
    points->InsertNextPoint(4, 2, 1);

    // 7,8,9,10,11,12,13,14
    points->InsertNextPoint(6, 0, 0);
    points->InsertNextPoint(8, 0, 0);
    points->InsertNextPoint(8, 0, 2);
    points->InsertNextPoint(6, 0, 2);
    points->InsertNextPoint(6, 2, 0);
    points->InsertNextPoint(9, 2, 0);
    points->InsertNextPoint(9, 2, 3);
    points->InsertNextPoint(6, 2, 3);

    // 15, 16, 17, 18, 19, 20
    points->InsertNextPoint(10, 0, 0);
    points->InsertNextPoint(11, 0, 2);
    points->InsertNextPoint(12, 0, 0);
    points->InsertNextPoint(10, 2, 0);
    points->InsertNextPoint(11, 2, 2);
    points->InsertNextPoint(12, 2, 0);

    // 21, 22, 23, 24, 25
    points->InsertNextPoint(13, 0, 0);
    points->InsertNextPoint(15, 0, 0);
    points->InsertNextPoint(15, 0, 2);
    points->InsertNextPoint(13, 0, 2);
    points->InsertNextPoint(14, 2, 1);

    vtkNew<vtkUnstructuredGrid> usg;
    usg->SetPoints(points);

    // 三角形
    vtkIdType ids_triangle[] { 0, 1, 2 };
    usg->InsertNextCell(VTK_TRIANGLE, 3, ids_triangle);

    // 四面体
    vtkIdType ids_tetra[] { 3, 4, 5, 6 };
    usg->InsertNextCell(VTK_TETRA, 4, ids_tetra);

    // 六面体
    vtkIdType ids_hexahedron[] { 7, 8, 9, 10, 11, 12, 13, 14 };
    usg->InsertNextCell(VTK_HEXAHEDRON, 8, ids_hexahedron);

    // 三棱柱
    vtkIdType ids_wedge[] { 15, 16, 17, 18, 19, 20 };
    usg->InsertNextCell(VTK_WEDGE, 6, ids_wedge);

    // 四棱锥
    vtkIdType ids_pyramid[] { 21, 22, 23, 24, 25 };
    usg->InsertNextCell(VTK_PYRAMID, 5, ids_pyramid);

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(usg);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST5
