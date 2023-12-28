#include "window.h"
#include "socket.h"

#include <QDebug>
#include <QPixmap>
#include <QBoxLayout>
#include <QEvent>
#include <QMouseEvent>

#include <thread>
#include <sstream>
#include <map>

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
}

bool Window::event(QEvent* evt)
{
    std::map<Qt::MouseButton, long> pressEventMap{
        { Qt::LeftButton, 1 },
        { Qt::RightButton, 5 },
        { Qt::MiddleButton, 3 },
    };

    std::map<Qt::MouseButton, long> releaseEventMap{
        { Qt::LeftButton, 2 },
        { Qt::RightButton, 6 },
        { Qt::MiddleButton, 4 },
    };

    std::stringstream ss;

    switch (evt->type())
    {
    case QEvent::Type::MouseButtonPress:
    {
        qDebug() << "mouse press";

        auto me = reinterpret_cast<QMouseEvent*>(evt);
        switch (me->button())
        {
        case Qt::LeftButton:
        case Qt::RightButton:
        case Qt::MiddleButton:
        {
            long type = pressEventMap[me->button()];
            int x_ = me->position().x();
            int y_ = me->position().y();
            ss.write(reinterpret_cast<char*>(&type), 4);
            ss.write(reinterpret_cast<char*>(&x_), 4);
            ss.write(reinterpret_cast<char*>(&y_), 4);

            break;
        }
        default:
            break;
        }
        break;
    }
    case QEvent::Type::MouseButtonRelease:
    {
        qDebug() << "mouse release";

        auto me = reinterpret_cast<QMouseEvent*>(evt);
        switch (me->button())
        {
        case Qt::LeftButton:
        case Qt::RightButton:
        case Qt::MiddleButton:
        {
            long type = releaseEventMap[me->button()];
            int x_ = me->position().x();
            int y_ = me->position().y();
            ss.write(reinterpret_cast<char*>(&type), 4);
            ss.write(reinterpret_cast<char*>(&x_), 4);
            ss.write(reinterpret_cast<char*>(&y_), 4);

            break;
        }
        default:
            break;
        }

        break;
    }
    case QEvent::Type::Wheel:
    {
        qDebug() << "mouse wheel";

        QWheelEvent* e2 = reinterpret_cast<QWheelEvent*>(evt);

        double horizontalDelta = e2->angleDelta().x();
        double verticalDelta = e2->angleDelta().y();
        const int threshold = 120;
        auto  accumulatedDelta = verticalDelta + horizontalDelta;

        if (accumulatedDelta >= threshold && verticalDelta != 0.0)
        {
            long type = 9;
            int x_ = 0;
            int y_ = 0;
            ss.write(reinterpret_cast<char*>(&type), 4);
            ss.write(reinterpret_cast<char*>(&x_), 4);
            ss.write(reinterpret_cast<char*>(&y_), 4);

            qDebug() << "mouse wheel forward event";
        }
        else if (accumulatedDelta <= -threshold && verticalDelta != 0.0)
        {
            long type = 10;
            int x_ = 0;
            int y_ = 0;
            ss.write(reinterpret_cast<char*>(&type), 4);
            ss.write(reinterpret_cast<char*>(&x_), 4);
            ss.write(reinterpret_cast<char*>(&y_), 4);

            qDebug() << "mouse wheel backward event";
        }

        break;
    }
    case QEvent::Type::MouseMove:
    {
        auto e2 = reinterpret_cast<QMouseEvent*>(evt);

        long type = 11;
        int x_ = e2->position().x();
        int y_ = e2->position().y();
        ss.write(reinterpret_cast<char*>(&type), 4);
        ss.write(reinterpret_cast<char*>(&x_), 4);
        ss.write(reinterpret_cast<char*>(&y_), 4);

        break;
    }
    case QEvent::Type::KeyPress:
    {
        qDebug() << "key press";
        break;
    }
    case QEvent::Type::KeyRelease:
    {
        qDebug() << "key release";
        break;
    }
    default:
        break;
    }

    ClientSocket::GetInstance()->AddBuffer(ss.str());

    return false;
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

void Window::OnBtnStart()
{
    qDebug() << "start\n";
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
    pixmap.load("../resources/test1.jpg");
    m_label->setPixmap(pixmap);
}
