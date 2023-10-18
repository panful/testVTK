#pragma once

#include "iviewer.h"
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

class InFilter
{
public:
    InFilter(vtkUnstructuredGrid* grid) : m_grid(grid)
    {
    }

    virtual ActorData* GetActorData() const = 0;

protected:
    vtkSmartPointer<vtkUnstructuredGrid> m_grid;
};

class MeshFilter : public InFilter
{
public:
    MeshFilter(vtkUnstructuredGrid* grid) : InFilter(grid)
    {
    }

    ActorData* GetActorData() const
    {
        auto actor = new ActorData();
        actor->id  = 0;
        return actor;
    }
};

class ContourFilter : public InFilter
{
};
