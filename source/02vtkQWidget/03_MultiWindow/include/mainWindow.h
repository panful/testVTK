///-------------------------------------------------------
///  @file         mainWindow.h
///  @brief     主窗口 
///  @version   V1.0
///  @copyright Leo
///  @author    Leo
///  @date      2022.5.9
///-------------------------------------------------------


#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QMainWindow>
#include <memory>

class LeoRenderWindow;
class LeoInvoker;

namespace Ui
{
    class MainWindow;
}

class LeoMainWindow :public QMainWindow
{
    Q_OBJECT
public:
    explicit LeoMainWindow(QWidget* parent = nullptr);
    ~LeoMainWindow() = default;

private:
    void InitViewer();

private slots:
    void OnBtn_1();
    void OnBtn_2();
    void OnBtn_3();
    void OnBtn_4();
    void OnBtn_5();
    void OnBtn_6();
    void OnBtn_7();

private:
    Ui::MainWindow* ui;
    std::shared_ptr<LeoRenderWindow> m_renderWindow;
    std::shared_ptr<LeoInvoker> m_invoker;
};


#endif // !_MAIN_WINDOW_H_
