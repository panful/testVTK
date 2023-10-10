//
// cgns文件下载地址：https://cgns.github.io/CGNSFiles.html
//

/**
 * 101. 结构cgns
 * 102. 非结构cgns
 *
 * 201. FLUENT *.cas *.dat 只绘制图形，不映射流场数据 FLUENT控制台使用 f c n wcd fileName 导出 fileName.cas和fileName.dat
 * 202. FLUENT *.cas *.dat 将流场数据映射到颜色
 *
 * 301. 构造 vtkUnstructuredGrid，四面体、六面体、棱柱、棱锥等图形
 * 302. 读写 vtkUnstructuredGrid vtkPolyData
 * 303. *.vtu XML格式的非结构网格读取
 * 304. 使用SetCells构造 vtkUnstructuredGrid 的单元
 *
 * 401. vtkPolyData vtkDataSet vtkUnstructuredGrid vtkStructuredGrid 互相转换
 * 402. vtkPolyData 设置多种单元(line poly...)，获取单元的索引
 */

#define TEST304

#ifdef TEST101

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

#endif // TEST101

#ifdef TEST102

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

#endif // TEST102

#ifdef TEST201

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

#endif // TEST201

#ifdef TEST202

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

#endif // TEST202

#ifdef TEST301

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

    // 大多数几何单元只需要隐式定义的几何类型和一组点及其顺序，对于非多面体单元类型，npts是单元格中唯一点的数目；
    // int type:                单元类型
    // vtkIdType npts:          几何点个数
    // const vtkIdType ptIds[]：几何点的ID数组
    // vtkIdType InsertNextCell(int type, vtkIdType npts, const vtkIdType ptIds[])

    // 用于创建多面体的接口
    // npts     单元中几何点的数量;
    // pts      单元点ID的列表;
    // nfaces   单元中的面数;
    // faces    多个面的信息流[numFace0Pts，id1，id2，id3，numFace1Pts，id1，id2，id3，…];
    // vtkIdType InsertNextCell(int type, vtkIdType npts, const vtkIdType ptIds[], vtkIdType nfaces, const vtkIdType faces[])

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

#endif // TEST301

#ifdef TEST302

#include <iostream>
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>

vtkSmartPointer<vtkUnstructuredGrid> usgWR()
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(2, 0, 0);
    points->InsertNextPoint(1, 0, 2);
    points->InsertNextPoint(1, 2, 1);

    vtkNew<vtkUnstructuredGrid> usg;
    usg->SetPoints(points);

    // 四面体
    vtkIdType ids_tetra[] { 0, 1, 2, 3 };
    usg->InsertNextCell(VTK_TETRA, 4, ids_tetra);

    vtkNew<vtkUnstructuredGridWriter> usgWriter;
    usgWriter->SetFileName("usg.vtk");
    usgWriter->SetInputData(usg);
    usgWriter->Write();

    vtkNew<vtkUnstructuredGridReader> usgReader;
    usgReader->SetFileName("usg.vtk");
    usgReader->Update();

    return usgReader->GetOutput();
}

vtkSmartPointer<vtkPolyData> polyDataWR()
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(3, 0, 0);
    points->InsertNextPoint(5, 0, 0);
    points->InsertNextPoint(4, 2, 0);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1, 2 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    vtkNew<vtkPolyDataWriter> polyDataWriter;
    polyDataWriter->SetFileName("polyData.vtk");
    polyDataWriter->SetInputData(polyData);
    polyDataWriter->Write();

    vtkNew<vtkPolyDataReader> polyDataReader;
    polyDataReader->SetFileName("polyData.vtk");
    polyDataReader->Update();

    return polyDataReader->GetOutput();
}

int main()
{
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(usgWR());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(polyDataWR());

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);
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

#endif // TEST302

#ifdef TEST303

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLUnstructuredGridReader.h>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::string inputFilename = "../resources/Disc_BiQuadraticQuads_0_0.vtu";

    // Read a xml unstructured grid dataset
    vtkNew<vtkXMLUnstructuredGridReader> reader;
    reader->SetFileName(inputFilename.c_str());
    reader->Update();

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputConnection(reader->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
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

    return EXIT_SUCCESS;
}

#endif // TEST303

#ifdef TEST304

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
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
    points->SetDataTypeToFloat();
    points->Allocate(15);

    // 0,1,2
    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(2., 0., 0.);
    points->InsertNextPoint(1., 2., 0.);

    // 3,4,5,6
    points->InsertNextPoint(3., 0., 0.);
    points->InsertNextPoint(5., 0., 0.);
    points->InsertNextPoint(4., 0., 2.);
    points->InsertNextPoint(4., 2., 1.);

    // 7,8,9,10,11,12,13,14
    points->InsertNextPoint(6., 0., 0.);
    points->InsertNextPoint(8., 0., 0.);
    points->InsertNextPoint(8., 0., 2.);
    points->InsertNextPoint(6., 0., 2.);
    points->InsertNextPoint(6., 2., 0.);
    points->InsertNextPoint(9., 2., 0.);
    points->InsertNextPoint(9., 2., 3.);
    points->InsertNextPoint(6., 2., 3.);

    // 单元类型
    std::vector<int> cellTypes { VTK_TRIANGLE, VTK_TETRA, VTK_HEXAHEDRON };
    // 每个单元的顶点个数
    std::vector<int> cellNpts { 3, 4, 8 };
    // 每个单元的顶点索引
    std::vector<vtkIdType> cellIndices { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

    vtkNew<vtkCellArray> cells;
    size_t index { 0 };
    for (size_t i = 0; i < cellNpts.size(); ++i)
    {
        cells->InsertNextCell(cellNpts[i]);
        for (size_t j = index; j < index + cellNpts[i]; ++j)
        {
            cells->InsertCellPoint(cellIndices[j]);
        }
        index += cellNpts[i];
    }

    vtkNew<vtkUnstructuredGrid> usg;
    usg->SetPoints(points);
    usg->SetCells(cellTypes.data(), cells);

    //-------------------------------------------------------------------
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(usg);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
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

#endif // TEST304

#ifdef TEST401

// vtkPolyData 的使用
// https://zhuanlan.zhihu.com/p/336743251

// vtkDataSet 的使用
// http://t.zoukankan.com/ybqjymy-p-14241014.html

// vtkUnstructuredGrid 的使用以及vtk单元类型枚举
// https://blog.csdn.net/liushao1031177/article/details/120708061

// vtkStructuredGrid 官方示例
// https://kitware.github.io/vtk-examples/site/Cxx/StructuredGrid/SGrid/

#include <iostream>

int main()
{
    std::cout << "please see 13_filter\n";

    // 13_filter TEST601 vtkDataSetSurfaceFilter      将 vtkUnstructuredGrid 转换为 vtkPolyData
    // 13_filter TEST603 vtkDataSetToDataObjectFilter 将数据集(vtkDataSet)转换为数据对象(vtkDataObject)，然后写入文本文件

    // vtkPolyDataToUnstructuredGrid
}

#endif // TEST401

#ifdef TEST402

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkInteractorStyleTrackballCamera.h>
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
    points->InsertNextPoint(0, 0, 0); // 第一条线段
    points->InsertNextPoint(0, 1, 0);
    points->InsertNextPoint(1, 0, 0); // 第一个三角形
    points->InsertNextPoint(2, 0, 0);
    points->InsertNextPoint(2, 1, 0);
    points->InsertNextPoint(3, 0, 0); // 第二条线段
    points->InsertNextPoint(3, 1, 0);
    points->InsertNextPoint(4, 0, 0); // 第二个三角形
    points->InsertNextPoint(5, 0, 0);
    points->InsertNextPoint(5, 1, 0);
    points->InsertNextPoint(6, 0, 0); // 单独的顶点
    points->InsertNextPoint(6, 1, 0); // 单独的顶点

    vtkNew<vtkCellArray> verts;
    vtkNew<vtkCellArray> lines;
    vtkNew<vtkCellArray> triangles;

    vtkIdType pt0Id = 10;
    vtkIdType pt1Id = 11;
    verts->InsertNextCell(1, &pt0Id);
    verts->InsertNextCell(1, &pt1Id);
    lines->InsertNextCell({ 0, 1 });
    lines->InsertNextCell({ 5, 6 });
    triangles->InsertNextCell({ 2, 3, 4 });
    triangles->InsertNextCell({ 7, 8, 9 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);

    polyData->SetPolys(triangles);
    polyData->SetLines(lines);
    polyData->SetVerts(verts);

    // 单元的索引和调用SetPolys() SetLines() SetVerts()的顺序无关
    // 固定顺序依次为：Verts Lines Polys Strips
    for (vtkIdType i = 0; i < polyData->GetNumberOfCells(); ++i)
    {
        vtkNew<vtkIdList> pts;
        polyData->GetCellPoints(i, pts);

        std::cout << "Cell: " << i << "  npts: " << pts->GetNumberOfIds() << "  points: ";
        for (vtkIdType j = 0; j < pts->GetNumberOfIds(); ++j)
        {
            std::cout << pts->GetId(j) << '\t';
        }
        std::cout << '\n';
    }

    //-----------------------------------------------
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->GetProperty()->SetPointSize(3);
    actor->GetProperty()->SetLineWidth(3);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
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