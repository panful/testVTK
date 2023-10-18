#pragma once

#include "iviewer.h"
#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

class VTKFilter
{
public:
    static vtkSmartPointer<vtkProp> Convert(const ActorData* data)
    {
        vtkNew<vtkCubeSource> source;
        source->Update();

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(source->GetOutput());

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        return actor;
    }
};