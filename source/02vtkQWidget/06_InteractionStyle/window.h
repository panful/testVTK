#include <QWidget>

class VTKWidget;

class Window :
    public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent = nullptr);
    ~Window() = default;

private:
    VTKWidget* m_renderWidget{ nullptr };
};