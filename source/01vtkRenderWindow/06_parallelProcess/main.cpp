
/*
* 1 vtk并行处理，并行算法 https://www.kitware.com/vtk-shared-memory-parallelism-tools-2021-updates/
* 2.异步创建actor std::future 多线程
* 3.vtkMultiThreader 和 std::thread
* 4 MPI
* 5 vtkParallelRenderManager
*/

#define TEST4


#ifdef TEST1

#include <vtkSMPTools.h>
#include <vtkSMPThreadLocal.h>
#include <vtkSMPThreadLocalObject.h>
#include <vtkParallelMPIModule.h>
#include <vtkParallelCoreModule.h>

#include <vtkPointDataToCellData.h>

int main()
{
    // 该类内部就使用了并行算法
    vtkNew<vtkPointDataToCellData> pd2cd;

    return 0;
}

#endif // TEST1

#ifdef TEST2


#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>

#include <chrono>
#include <thread>
#include <future>

constexpr size_t MaxNumOfActors = 10;

vtkSmartPointer<vtkActor> CreateActor(double x, double y, double z)
{
    vtkNew<vtkCubeSource> cube;
    cube->SetCenter(x, y, z);

    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return cubeActor;
}

#define MultiThread

int main(int argc, char* argv[])
{
    vtkNew<vtkRenderer> renderer;

    auto start = std::chrono::high_resolution_clock::now();

#ifdef MultiThread
    std::vector<std::future<vtkSmartPointer<vtkActor>>> vecActors;
    vecActors.reserve(MaxNumOfActors);

    for (size_t i = 0; i < MaxNumOfActors; ++i)
    {
        auto axis = static_cast<double>(i);
        vecActors.emplace_back(std::async(std::launch::async, CreateActor, axis, axis, axis));
    }

    for (auto& actor : vecActors)
    {
        renderer->AddActor(actor.get());
    }
#else
    for (size_t i = 0; i < MaxNumOfActors; ++i)
    {
        auto axis = static_cast<double>(i);
        renderer->AddActor(CreateActor(axis, axis, axis));
    }

#endif // MultiThread

    std::cout << "took:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);//设置window大小

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    std::thread t([&]() {renWin->Render(); iren->Start(); });
    t.join(); // 此处不能用detach()

    //renWin->Render();
    //iren->Start();

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include "vtkMultiThreader.h"
#include "vtkSmartPointer.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <vector>

VTK_THREAD_RETURN_TYPE MyFunction(void*)
{
    //for (const auto& elem : { 1,2,3,4,5 })
    //{
    //    std::cout << elem << '\t';
    //}
    //std::cout << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(3));

    return VTK_THREAD_RETURN_VALUE;
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    constexpr size_t TreadNum = 8;

    // vtk多线程
    {
        vtkNew<vtkMultiThreader> multiThread;
        // 设置共有8个线程
        multiThread->SetNumberOfThreads(TreadNum);
        // 将MyFunction放在多线程中执行
        multiThread->SetSingleMethod(MyFunction, nullptr);
        // 执行线程
        multiThread->SingleMethodExecute();
    }
    auto vtkTookTime = std::chrono::high_resolution_clock::now();
    std::cout << "vtk took:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(vtkTookTime - start).count() << '\n';

    // 标准库多线程
    {
        std::vector<std::thread> vecThreads;
        vecThreads.reserve(TreadNum);

        for (size_t i = 0; i < TreadNum; ++i)
        {
            vecThreads.emplace_back(std::thread(MyFunction, nullptr));
        }
        for (auto& thread : vecThreads)
        {
            thread.join();
        }
    }

    // 以上两种方式效果是一样的
    std::cout << "std took:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - vtkTookTime).count() << '\n';

}

#endif // TEST3

#ifdef TEST4

/*
在VTK中实现基于MPI的渲染的一般步骤：
初始化MPI环境：可以使用MPI_Init或MPI_Init_thread函数来初始化MPI环境。
创建MPI通信器：可以使用MPI_COMM_WORLD或使用MPI_COMM_split创建新的通信器。
划分数据：使用MPI_Scatter或MPI_Bcast等MPI函数，在进程之间划分数据。
渲染数据：每个进程都有自己的数据，可以使用VTK在本地渲染数据。
收集结果：在每个进程呈现其数据后，使用MPI_Gather等MPI函数收集根进程的结果。
完成MPI环境：使用完MPI后，使用MPI_Finalize完成MPI环境。
请注意，MPI是一个复杂的主题，有很多方法可以用VTK实现它。这是对所涉及步骤的高级概述，您可能需要查阅其他资源，以了解如何在VTK中实现基于MPI的渲染的更多详细信息。
*/

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkMPIController.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <vtk_mpi.h>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    vtkSmartPointer<vtkMPIController> controller =
        vtkSmartPointer<vtkMPIController>::New();
    controller->Initialize(&argc, &argv, 1);



    int myid = controller->GetLocalProcessId();
    int numProcs = controller->GetNumberOfProcesses();

    vtkSmartPointer<vtkSphereSource> sphere =
        vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetThetaResolution(100);
    sphere->SetPhiResolution(50);
    sphere->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphere->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);

    vtkSmartPointer<vtkRenderWindow> renderWindow =
        vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);

    if (myid == 0)
    {
        renderWindow->SetSize(640, 480);
        renderWindow->Render();
        interactor->Start();

        std::cout << "id: " << myid << '\n';
    }
    else
    {
        interactor->Initialize();
        interactor->Start();

        std::cout << "id: " << myid << '\n';
    }

    //MPI_Finalize();
    controller->Finalize();

    return 0;
}

#endif // TEST4

#ifdef TEST5

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    PrmMagnify.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  Copyright 2005 Sandia Corporation. Under the terms of Contract
  DE-AC04-94AL85000, there is a non-exclusive license for use of this work by
  or on behalf of the U.S. Government. Redistribution and use in source and
  binary forms, with or without modification, are permitted provided that this
  Notice and any statement of authorship are reproduced on all copies.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkActor.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkDummyController.h"
#include "vtkIdFilter.h"
#include "vtkObjectFactory.h"
#include "vtkParallelRenderManager.h"
#include "vtkPolyDataMapper.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"

#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageMandelbrotSource.h"
#include "vtkImageMapper3D.h"
#include "vtkImageShiftScale.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"

#include "vtkSmartPointer.h"

#define VTK_CREATE(type, var) vtkSmartPointer<type> var = vtkSmartPointer<type>::New()

//------------------------------------------------------------------------------

class vtkTestMagnifyRenderManager : public vtkParallelRenderManager
{
public:
    vtkTypeMacro(vtkTestMagnifyRenderManager, vtkParallelRenderManager);
    static vtkTestMagnifyRenderManager* New();

protected:
    vtkTestMagnifyRenderManager();
    ~vtkTestMagnifyRenderManager() override;

    void PreRenderProcessing() override;
    void PostRenderProcessing() override;

    void ReadReducedImage() override;

    vtkImageMandelbrotSource* Mandelbrot;

private:
    vtkTestMagnifyRenderManager(const vtkTestMagnifyRenderManager&) = delete;
    void operator=(const vtkTestMagnifyRenderManager&) = delete;
};

vtkStandardNewMacro(vtkTestMagnifyRenderManager);

vtkTestMagnifyRenderManager::vtkTestMagnifyRenderManager()
{
    this->Mandelbrot = vtkImageMandelbrotSource::New();
}

vtkTestMagnifyRenderManager::~vtkTestMagnifyRenderManager()
{
    this->Mandelbrot->Delete();
}

void vtkTestMagnifyRenderManager::PreRenderProcessing()
{
    this->RenderWindowImageUpToDate = 0;
    this->RenderWindow->SwapBuffersOff();
}

void vtkTestMagnifyRenderManager::PostRenderProcessing()
{
    this->FullImage->SetNumberOfComponents(4);
    this->FullImage->SetNumberOfTuples(this->FullImageSize[0] * this->FullImageSize[1]);

    int fullImageViewport[4], reducedImageViewport[4];

    // Read in image as RGBA.
    this->UseRGBA = 1;
    this->ReducedImageUpToDate = 0;
    this->ReadReducedImage();

    fullImageViewport[0] = 0;
    fullImageViewport[1] = 0;
    fullImageViewport[2] = this->FullImageSize[0] / 2;
    fullImageViewport[3] = this->FullImageSize[1] / 2;
    reducedImageViewport[0] = 0;
    reducedImageViewport[1] = 0;
    reducedImageViewport[2] = this->ReducedImageSize[0] / 2;
    reducedImageViewport[3] = this->ReducedImageSize[1] / 2;
    vtkTestMagnifyRenderManager::MagnifyImageNearest(this->FullImage, this->FullImageSize,
        this->ReducedImage, this->ReducedImageSize, fullImageViewport, reducedImageViewport);

    fullImageViewport[0] = this->FullImageSize[0] / 2;
    fullImageViewport[1] = 0;
    fullImageViewport[2] = this->FullImageSize[0];
    fullImageViewport[3] = this->FullImageSize[1] / 2;
    reducedImageViewport[0] = this->ReducedImageSize[0] / 2;
    reducedImageViewport[1] = 0;
    reducedImageViewport[2] = this->ReducedImageSize[0];
    reducedImageViewport[3] = this->ReducedImageSize[1] / 2;
    vtkTestMagnifyRenderManager::MagnifyImageLinear(this->FullImage, this->FullImageSize,
        this->ReducedImage, this->ReducedImageSize, fullImageViewport, reducedImageViewport);

    // Read in image as RGB.
    this->UseRGBA = 0;
    this->ReducedImageUpToDate = 0;
    this->ReadReducedImage();

    fullImageViewport[0] = 0;
    fullImageViewport[1] = this->FullImageSize[1] / 2;
    fullImageViewport[2] = this->FullImageSize[0] / 2;
    fullImageViewport[3] = this->FullImageSize[1];
    reducedImageViewport[0] = 0;
    reducedImageViewport[1] = this->ReducedImageSize[1] / 2;
    reducedImageViewport[2] = this->ReducedImageSize[0] / 2;
    reducedImageViewport[3] = this->ReducedImageSize[1];
    vtkTestMagnifyRenderManager::MagnifyImageNearest(this->FullImage, this->FullImageSize,
        this->ReducedImage, this->ReducedImageSize, fullImageViewport, reducedImageViewport);

    fullImageViewport[0] = this->FullImageSize[0] / 2;
    fullImageViewport[1] = this->FullImageSize[1] / 2;
    fullImageViewport[2] = this->FullImageSize[0];
    fullImageViewport[3] = this->FullImageSize[1];
    reducedImageViewport[0] = this->ReducedImageSize[0] / 2;
    reducedImageViewport[1] = this->ReducedImageSize[1] / 2;
    reducedImageViewport[2] = this->ReducedImageSize[0];
    reducedImageViewport[3] = this->ReducedImageSize[1];
    vtkTestMagnifyRenderManager::MagnifyImageLinear(this->FullImage, this->FullImageSize,
        this->ReducedImage, this->ReducedImageSize, fullImageViewport, reducedImageViewport);

    this->FullImageUpToDate = 1;

    this->WriteFullImage();

    this->RenderWindow->SwapBuffersOn();
    this->RenderWindow->Frame();
}

void vtkTestMagnifyRenderManager::ReadReducedImage()
{
    if (this->ReducedImageUpToDate)
        return;

    this->Mandelbrot->SetWholeExtent(
        0, this->ReducedImageSize[0] - 1, 0, this->ReducedImageSize[1] - 1, 0, 0);
    this->Mandelbrot->SetMaximumNumberOfIterations(255);
    this->Mandelbrot->Update();

    vtkIdType numpixels = this->ReducedImageSize[0] * this->ReducedImageSize[1];

    vtkDataArray* src = this->Mandelbrot->GetOutput()->GetPointData()->GetScalars();
    if (src->GetNumberOfTuples() != numpixels)
    {
        vtkErrorMacro("Image is wrong size!");
        return;
    }

    if (this->UseRGBA)
    {
        this->ReducedImage->SetNumberOfComponents(4);
    }
    else
    {
        this->ReducedImage->SetNumberOfComponents(3);
    }
    this->ReducedImage->SetNumberOfTuples(numpixels);

    double color[4];
    color[3] = 255;

    for (vtkIdType i = 0; i < numpixels; i++)
    {
        double value = src->GetComponent(i, 0);
        color[0] = value;
        color[1] = (value < 128) ? value : (255 - value);
        color[2] = 255 - value;
        this->ReducedImage->SetTuple(i, color);
    }
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    VTK_CREATE(vtkDummyController, controller);
    controller->Initialize(&argc, &argv);

    VTK_CREATE(vtkTestMagnifyRenderManager, prm);
    prm->SetController(controller);

    //   VTK_CREATE(vtkSphereSource, sphere);
    //   sphere->SetEndPhi(90.0);
    //   sphere->SetPhiResolution(4);

    //   VTK_CREATE(vtkIdFilter, colors);
    //   colors->SetInputConnection(sphere->GetOutputPort());
    //   colors->PointIdsOff();
    //   colors->CellIdsOn();
    //   colors->FieldDataOff();
    //   colors->Update();

    //   VTK_CREATE(vtkPolyDataMapper, mapper);
    //   mapper->SetInputConnection(colors->GetOutputPort());
    //   mapper->UseLookupTableScalarRangeOff();
    //   mapper->SetScalarRange(colors->GetOutput()->GetCellData()
    //                          ->GetScalars()->GetRange());

    //   VTK_CREATE(vtkActor, actor);
    //   actor->SetMapper(mapper);

    VTK_CREATE(vtkImageMandelbrotSource, mandelbrot);
    mandelbrot->SetWholeExtent(0, 73, 0, 73, 0, 0);
    mandelbrot->SetMaximumNumberOfIterations(255);

    VTK_CREATE(vtkImageShiftScale, charImage);
    charImage->SetInputConnection(mandelbrot->GetOutputPort());
    charImage->SetShift(0);
    charImage->SetScale(1);
    charImage->SetOutputScalarTypeToUnsignedChar();

    VTK_CREATE(vtkImageActor, actor);
    actor->GetMapper()->SetInputConnection(charImage->GetOutputPort());
    actor->InterpolateOff();

    vtkSmartPointer<vtkRenderer> renderer = prm->MakeRenderer();
    renderer->Delete(); // Remove duplicate reference.
    renderer->AddActor(actor);
    renderer->SetBackground(1, 0, 0);

    vtkSmartPointer<vtkRenderWindow> renwin = prm->MakeRenderWindow();
    renwin->Delete(); // Remove duplicate reference.
    renwin->SetSize(256, 256);
    renwin->AddRenderer(renderer);
    prm->SetRenderWindow(renwin);

    prm->ResetAllCameras();
    prm->SetImageReductionFactor(8);

    // Run the regression test.
    renwin->Render();
    int retVal = vtkRegressionTestImage(renwin);
    //if (retVal == vtkRegressionTester::DO_INTERACTOR)
    //{
        VTK_CREATE(vtkRenderWindowInteractor, iren);
        iren->SetRenderWindow(renwin);
        renwin->Render();
        iren->Start();
        retVal = vtkRegressionTester::PASSED;
    //}

    controller->Finalize();

    return !retVal;
}


#endif // TEST5
