#include <vtkSmartPointer.h>

class vtkRenderWindow;
class vtkRenderer;

enum class EventType
{
    None = 0,

    LeftButtonPressEvent,
    LeftButtonReleaseEvent,

    MiddleButtonPressEvent,
    MiddleButtonReleaseEvent,

    RightButtonPressEvent,
    RightButtonReleaseEvent,

    KeyPressEvent,
    KeyReleaseEvent,

    MouseWheelForwardEvent,
    MouseWheelBackwardEvent,

    MouseMoveEvent,
};

class WindowVTK
{
public:
    WindowVTK();
    ~WindowVTK() = default;

    bool ProcessEvent(long t, int, int, void* data = nullptr);
    void MyStart();

private:
    vtkSmartPointer<vtkRenderWindow> m_window;
    vtkSmartPointer<vtkRenderer> m_renderer;
};
