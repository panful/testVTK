//
// cgns文件下载地址：https://cgns.github.io/CGNSFiles.html
//

/*
 * 1. 结构cgns
 * 2. 非结构cgns
 * 3.
 */

#define TEST1

#ifdef TEST1

#include <vtkActor.h>
#include <vtkCGNSReader.h>
#include <vtkCamera.h>
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

    for (unsigned int i = 0; i < nb; ++i)
    {
        auto mb2 = vtkMultiBlockDataSet::SafeDownCast(mb->GetBlock(i));
        for (unsigned int j = 0; j < mb2->GetNumberOfBlocks(); ++j)
        {
            auto ug = vtkStructuredGrid::SafeDownCast(mb2->GetBlock(j));
            auto np = ug->GetNumberOfPoints();
            auto nc = ug->GetNumberOfCells();

            auto cd = ug->GetCellData();
            auto pd = ug->GetPointData();

            auto cdScalars  = cd->GetScalars();
            auto cdVectors  = cd->GetVectors();
            auto cdTensors  = cd->GetTensors();
            auto cdTCoords  = cd->GetTCoords();
            auto cdTangents = cd->GetTangents();
            auto cdNormals  = cd->GetNormals();

            auto pdScalars  = pd->GetScalars();
            auto pdVectors  = pd->GetVectors();
            auto pdTensors  = pd->GetTensors();
            auto pdTCoords  = pd->GetTCoords();
            auto pdTangents = pd->GetTangents();
            auto pdNormals  = pd->GetNormals();

            mapper->SetInputData(ug);

            std::cout << "number of cells: " << nc << "\nnumber of points: " << np << '\n';

            for (vtkIdType k = 0; k < ug->GetNumberOfCells(); ++k)
            {
                vtkCell* cell = ug->GetCell(k);
            }
        }
    }

    mapper->ScalarVisibilityOn();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    // actor->GetProperty()->SetColor(1, 0, 0);

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

#endif // TEST1

#ifdef TEST2

#include <vtkActor.h>
#include <vtkCGNSReader.h>
#include <vtkCamera.h>
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

    for (unsigned int i = 0; i < nb; ++i)
    {
        auto mb2 = vtkMultiBlockDataSet::SafeDownCast(mb->GetBlock(i));
        for (unsigned int j = 0; j < mb2->GetNumberOfBlocks(); ++j)
        {
            auto ug = vtkUnstructuredGrid::SafeDownCast(mb2->GetBlock(j));
            auto np = ug->GetNumberOfPoints();
            auto nc = ug->GetNumberOfCells();

            auto cd = ug->GetCellData();
            auto pd = ug->GetPointData();

            auto cdScalars  = cd->GetScalars();
            auto cdVectors  = cd->GetVectors();
            auto cdTensors  = cd->GetTensors();
            auto cdTCoords  = cd->GetTCoords();
            auto cdTangents = cd->GetTangents();
            auto cdNormals  = cd->GetNormals();

            auto pdScalars  = pd->GetScalars();
            auto pdVectors  = pd->GetVectors();
            auto pdTensors  = pd->GetTensors();
            auto pdTCoords  = pd->GetTCoords();
            auto pdTangents = pd->GetTangents();
            auto pdNormals  = pd->GetNormals();

            mapper->SetInputData(ug);

            std::cout << "number of cells: " << nc << "\nnumber of points: " << np << '\n';

            for (vtkIdType k = 0; k < ug->GetNumberOfCells(); ++k)
            {
                vtkCell* cell = ug->GetCell(k);
            }
        }
    }

    mapper->ScalarVisibilityOn();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    // actor->GetProperty()->SetColor(1, 0, 0);

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

#endif // TEST2
