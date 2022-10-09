///-------------------------------------------------------
///  @file         window.h
///  @brief     渲染窗口 
///  @version   V1.0
///  @copyright Leo
///  @author    Leo
///  @date      2022.5.9
///-------------------------------------------------------

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <map>
#include <array>
#include <memory>
#include <QObject>

class QWidget;
class QGridLayout;
class QMainWindow;
class QDockWidget;
class QStackedWidget;
class LeoViewer;

class LeoRenderWindow :public QObject
{
    Q_OBJECT
public:
    enum class ViewerMode
    {
        VM_0 = 0,
        VM_1 = 1,
        VM_2 = 2,
    };
    explicit LeoRenderWindow(QWidget* parent);
    ~LeoRenderWindow() = default;

private slots:
    void TestSlot(bool arg);



public:
    void ScreenCapture();
    void BeforeWindow();
    void SetViewerMode(ViewerMode);

    void AddGeometryData(const std::string& fileName);

    void SetColor(std::array<double, 3> color);

private:
    QWidget* m_parentWindow{ nullptr };
    QStackedWidget* m_stackedWidget{ nullptr };
    QMainWindow* m_renderWidget{ nullptr };
    QWidget* m_captureWidget{ nullptr };
    QWidget* m_previewWidget{ nullptr };

    QWidget* m_lastCapWidget{ nullptr };

    //LeoViewer* m_viewer1{ nullptr };

    //std::map<size_t, QDockWidget*> m_viewers{ {0, nullptr}, {1, nullptr}, {2, nullptr}, {3, nullptr} };

    std::array<std::shared_ptr<LeoViewer>, 4> m_viewers;

};
#endif // !_WINDOW_H_
