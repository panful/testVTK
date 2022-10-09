#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <queue>
#include <atomic>

class Window :
    public QWidget
{
public:
    explicit Window(QWidget* parent = nullptr);
    ~Window() = default;

    void ShowPixels();

    void ExecuteSendQueue();

    void AddPixels(const std::string& pixels);

private slots:
    void OnBtnStart();
    void OnBtnTest();
private:
    QPushButton* m_btnStart;
    QPushButton* m_btnTest;
    QLabel* m_label;
    std::queue<std::string> m_pixels;
    //std::atomic<std::queue<std::string>> m_pixels;
    std::queue<std::string> m_sendQueue;
};

#endif // !_WINDOW_H_
