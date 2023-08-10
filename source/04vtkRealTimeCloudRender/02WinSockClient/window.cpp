#include "window.h"
#include "socket.h"

#include <QDebug>
#include <QPixmap>
#include <QBoxLayout>
#include <thread>

Window::Window(QWidget* parent) :
    QWidget(parent)
{
    m_btnStart = new QPushButton("Start");
    m_btnTest = new QPushButton("Test");
    m_label = new QLabel();

    QVBoxLayout* vMainLayout = new QVBoxLayout();
    QHBoxLayout* hBtnLayout = new QHBoxLayout();
    
    hBtnLayout->addWidget(m_btnStart);
    hBtnLayout->addWidget(m_btnTest);
    vMainLayout->addLayout(hBtnLayout);
    vMainLayout->addWidget(m_label);

    this->setLayout(vMainLayout);

    connect(m_btnStart, &QPushButton::clicked, [this]() {this->OnBtnStart(); });
    connect(m_btnTest, &QPushButton::clicked, [this]() {this->OnBtnTest(); });

    QImage image(800, 600, QImage::Format::Format_RGB32);
    m_label->setPixmap(QPixmap::fromImage(image));

    std::thread t([this]() {this->ShowPixels(); });
    t.detach();

    int i = 0;
}

void Window::ShowPixels()
{
    while (true)
    {
        if (!m_pixels.empty())
        {
            auto strPixels = m_pixels.front();
            m_pixels.pop();

            auto pixels = reinterpret_cast<const unsigned char*>(strPixels.c_str());

            int w = 800;
            int h = 600;
            QImage image(w, h, QImage::Format::Format_RGB32);

            for (size_t i = 0; i < h; i++)
            {
                for (size_t j = 0; j < w; j++)
                {
                    auto r = static_cast<int>(pixels[(i * w + j) * 4 + 0]);
                    auto g = static_cast<int>(pixels[(i * w + j) * 4 + 1]);
                    auto b = static_cast<int>(pixels[(i * w + j) * 4 + 2]);

                    // vtk左下角开始，qt左上角开始
                    image.setPixelColor(static_cast<int>(j), static_cast<int>(h - i - 1), QColor(r, g, b));
                }
            }

            m_label->setPixmap(QPixmap::fromImage(image));
        }
    }
}

void Window::ExecuteSendQueue()
{
    while (true)
    {
        if (!m_sendQueue.empty())
        {
            auto strSendBuffer = m_sendQueue.front();
            m_sendQueue.pop();

            Socket::GetInstance()->Send("");
        }
    }
}

void Window::OnBtnStart()
{
    qDebug() << "start\n";

    Socket::GetInstance()->InitSocket();
    Socket::GetInstance()->ConnectSocket();

    std::thread t([]() {Socket::GetInstance()->Recv2(); });
    t.detach();
}

void Window::AddPixels(const std::string& pixels)
{
    if (!pixels.empty())
    {
        m_pixels.emplace(pixels);
    }
}

void Window::OnBtnTest()
{
    qDebug() << "test\n";

    QPixmap pixmap;
    pixmap.load("p4.jpg");
    m_label->setPixmap(pixmap);
}
