///-------------------------------------------------------
///  @file         screenCapture.h
///  @brief     截图窗口 
///  @version   V1.0
///  @copyright Leo
///  @author    Leo
///  @date      2022.5.9
///-------------------------------------------------------

#ifndef _SCREEN_CAPTURE_H_
#define _SCREEN_CAPTURE_H_

#include <QVTKRenderWidget.h>
#include <QPixmap>
#include <memory>

class vtkRenderer;
class vtkGenericOpenGLRenderWindow;
class vtkImageData;

class LeoScreenCapture :public QVTKRenderWidget
{
public:
    explicit LeoScreenCapture(QWidget* parent = nullptr);
    ~LeoScreenCapture() = default;

public:
    void SetWidget(QWidget*);
    LeoScreenCapture* GetWidget();
private:
    vtkSmartPointer<vtkRenderer> m_renderer{ nullptr };
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_window{ nullptr };
    vtkSmartPointer<vtkImageData> m_windowImage{ nullptr };

    int m_width{ 0 };
    int m_height{ 0 };
};

#endif // !_SCREEN_CAPTURE_H_
