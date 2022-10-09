#include "vtkInteractorStyleRubberBand3D.h"
#include "vtkSmartPointer.h"
#include <stack>
#include <array>

class vtkCamera;

class Style :public vtkInteractorStyleRubberBand3D
{
public:
    static Style* New();

    vtkTypeMacro(Style, vtkInteractorStyleRubberBand3D);

public:
    void OnLeftButtonDown() override;
    void OnLeftButtonUp() override;

    void OnMiddleButtonDown() override;
    void OnMiddleButtonUp() override;

    void OnRightButtonDown() override;
    void OnRightButtonUp() override;

    void OnMouseMove() override;
    void OnMouseWheelForward() override;
    void OnMouseWheelBackward() override;

public:
    void UndoCamera();
    void RedoCamera();

protected:
    Style() = default;
    ~Style() override = default;

private:
    std::array<int, 2>  m_rightStartPos;

private:
    std::stack<vtkSmartPointer<vtkCamera>> m_undoCameras;
    std::stack<vtkSmartPointer<vtkCamera>> m_redoCameras;
};


