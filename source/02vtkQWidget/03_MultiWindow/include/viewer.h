///-------------------------------------------------------
///  @file         viewer.h
///  @brief     渲染窗口
///  @version   V1.0
///  @copyright Leo
///  @author    Leo
///  @date      2022.5.9
///-------------------------------------------------------

#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <QVTKRenderWidget.h>

class LeoViewer :public QVTKRenderWidget
{
public:
    explicit LeoViewer(QWidget* parent = nullptr);
    ~LeoViewer() = default;

    vtkRenderer* renderer;
public:
    void TestPrint();
    void SetColor(std::array<double, 3> color);
};

#endif // !_VIEWER_H_
