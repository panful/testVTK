/*
* 1. 长按左键一直旋转，定时器的使用
*/

#define TEST1

#ifdef TEST1

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleJoystickCamera.h>

class Style : public vtkInteractorStyleJoystickCamera
{
public:
    static Style* New() {
        return new Style();
    }

    vtkTypeMacro(Style, vtkInteractorStyleJoystickCamera);

protected:
    void OnLeftButtonDown() override
    {
        // 会开启一个定时器，只要鼠标没有抬起就一直旋转
        // Win32使用 WM_TIMER
        // Qt使用 QTimer
        Superclass::OnLeftButtonDown();
    }
    void Rotate() override
    {
        std::cout << "rotate\n";
        Superclass::Rotate();
    }
protected:
    Style() = default;
    ~Style() = default;
};

int main(int, char* [])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<Style> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST1
