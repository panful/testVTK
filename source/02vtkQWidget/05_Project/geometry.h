#pragma once

#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

#include <atomic>
#include <map>
#include <vector>

class GeometryManager
{
public:
    template <typename T>
    void SetVertices(int id, const std::vector<T>& v)
    {
        if (0 == m_geo.count(id))
        {
            m_geo[id] = vtkSmartPointer<vtkUnstructuredGrid>::New();
        }

        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0., 0., 0.);
        points->InsertNextPoint(2., 0., 0.);
        points->InsertNextPoint(1., 2., 0.);

        m_geo.at(id)->SetPoints(points);
    }

    template <typename T>
    void SetIndices(int id, const std::vector<T>& i)
    {
        if (0 == m_geo.count(id))
        {
            m_geo[id] = vtkSmartPointer<vtkUnstructuredGrid>::New();
        }

        vtkIdType ptIds[3] { 0, 1, 2 };
        m_geo.at(id)->InsertNextCell(VTK_TRIANGLE, 3, ptIds);
    }

    vtkUnstructuredGrid* GetGeometry(int id) const
    {
        return m_geo.at(id);
    }

private:
    std::map<int, vtkSmartPointer<vtkUnstructuredGrid>> m_geo;
};