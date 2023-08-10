#include <QVTKRenderWidget.h> 
#include <QMainWindow>

class VTKWidget :public QVTKRenderWidget
{
public:
    explicit VTKWidget(QWidget* parent = nullptr);
    ~VTKWidget() = default;

protected:
    bool event(QEvent* evt) override;
};