#include "vtkInteractorStyleRubberBand3D.h"
#include "vtkSmartPointer.h"
#include <stack>

class vtkCamera;
class vtkVector2d;

class Style :public vtkInteractorStyleRubberBand3D
{
public:
    enum class InteractionMode
    {
        Rotate = 0,
        Zoom = 1,
        Translation = 2,
    };
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

    void OnChar() override;

    // 以鼠标所在位置为中心进行缩放（仅支持平行投影）
    void ZoomByFocalPoint(const vtkVector2d& focalPoint, bool zoomIn);
    void TranslateCameraToFocalPoint(const vtkVector2d& point, bool forwardTo);

public:
    void SetInteractionMode(InteractionMode mode);

protected:
    Style() = default;
    ~Style() override = default;

private:
    InteractionMode m_interactionMode{ -1 };
};