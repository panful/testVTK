#include "window.h"

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkInteractorStyleRubberBand3D.h>

// socket必须在vtk头文件之后包含
#include "socket.h"
#include <sstream>

class CustomStyle :
    public vtkInteractorStyleRubberBand3D
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleRubberBand3D);

protected:
    void OnChar() override
    {
        if (!this->Interactor || !this->CurrentRenderer)
        {
            return;
        }

        switch (this->Interactor->GetKeyCode())
        {
        case 'x':
        case 'X':
        {
            std::cout << "press x\n";
            break;
        }
        case 'Z':
        case 'z':
        {
            std::cout << "press z\n";
            break;
        }
        default:
            break;
        }

        return Superclass::OnChar();
    }

    void OnMouseWheelForward() override
    {
        std::cout << "mouse wheel forward\n";
        Superclass::OnMouseWheelForward();
    }
    void OnMouseWheelBackward() override
    {
        std::cout << "mouse wheel backward\n";
        Superclass::OnMouseWheelBackward();
    }
};

vtkStandardNewMacro(CustomStyle);

class RenderEventCB :
    public vtkCommand
{
public:
    static RenderEventCB* New();
    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        if (auto interactor = dynamic_cast<vtkRenderWindowInteractor*>(caller))
        {
            auto window = interactor->GetRenderWindow();
            auto width = window->GetSize()[0];
            auto height = window->GetSize()[1];

            const auto pixelsSize = width * height * 4;
            const auto pixels = window->GetRGBACharPixelData(0, 0, width - 1, height - 1, 1);

            // width:int:4  height:int:4 
            std::stringstream ssPixels;
            ssPixels.write(reinterpret_cast<const char*>(&width), 4);
            ssPixels.write(reinterpret_cast<const char*>(&height), 4);
            ssPixels.write(reinterpret_cast<const char*>(pixels), pixelsSize);

            delete[] pixels;

            ServerSocket::GetInstance()->AddBuffer(ssPixels.str());
        }
    }
};

vtkStandardNewMacro(RenderEventCB);

Window::Window(short index, int width, int height)
{
    vtkNew<vtkCubeSource> cube;

    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1, 0, 0);

    //camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);//设置相机位置
    camera->SetFocalPoint(0, 0, 0);//设置相机焦点

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(width, height);//设置window大小

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<CustomStyle> style;
    iren->SetInteractorStyle(style);

    vtkNew<RenderEventCB> renderEventCB;
    iren->AddObserver(vtkCommand::RenderEvent, renderEventCB);

    //数据交互
    renWin->Render();
    iren->Start();
}

void Window::SetSize(int w, int h)
{

}

