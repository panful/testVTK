#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <queue>

class Window :
    public QWidget
{
public:
    explicit Window(QWidget* parent = nullptr);
    ~Window() = default;

    void ShowPixels();

    void AddPixels(const std::string& pixels);

protected:
    bool event(QEvent* evt) override;

private slots:
    void OnBtnStart();
    void OnBtnTest();
private:
    QPushButton* m_btnStart;
    QPushButton* m_btnTest;
    QLabel* m_label;
    std::queue<std::string> m_pixels;
};

#endif // !_WINDOW_H_
