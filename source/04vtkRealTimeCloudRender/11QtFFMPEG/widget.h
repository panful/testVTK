#include <QVTKRenderWidget.h>
#include <vtkSmartPointer.h>

class vtkGenericOpenGLRenderWindow;
class vtkRenderer;
class vtkFFMPEGWriter;
class vtkWindowToImageFilter;
class QLabel;

class VTKWidget :public QVTKRenderWidget
{
    Q_OBJECT
public:
    explicit VTKWidget(QWidget* parent = nullptr);
    ~VTKWidget();
private:
    void GenImage(int w, int h, unsigned char* pixels, QImage* image);
protected:
    virtual void paintGL()override;
    bool event(QEvent* evt) override;
private slots:
    void testEvent();
private:
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;
    //vtkSmartPointer<vtkFFMPEGWriter> m_ffmpeg;
    vtkSmartPointer<vtkWindowToImageFilter> m_image;
    QWidget* m_imageWidget;
    QLabel* m_label;
};