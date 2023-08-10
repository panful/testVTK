
#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkObjectFactory.h>

class CustomInteractor :
    public vtkRenderWindowInteractor
{
public:
    static CustomInteractor* New();
    vtkTypeMacro(CustomInteractor, vtkRenderWindowInteractor);
public:
    void Start() override
    {
        while (1)
            ;
    }
};

vtkStandardNewMacro(CustomInteractor);

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

    void OnMouseMove() override
    {
        std::cout << "mouse move\n";
        Superclass::OnMouseMove();
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

int main(int, char* [])
{
    vtkNew<vtkCubeSource> cube;

    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);

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
    renWin->SetSize(600, 600);//设置window大小

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    //vtkNew<CustomInteractor> iren;
    iren->SetRenderWindow(renWin);

    // Interactor style
    vtkNew<CustomStyle> style;
    iren->SetInteractorStyle(style);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}