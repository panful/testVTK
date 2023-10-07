#include "clientWidget.h"

#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <sstream>

#include <QTcpServer>
#include <QTcpSocket>

VTKWidget::VTKWidget(QWidget* parent)
    :QWidget(parent)
{
    std::cout << "=== Start ===\n";

    m_socket = new QTcpSocket();
    m_socket->connectToHost(QHostAddress::LocalHost, 7788); // ip,port

    //connect(m_socket, SIGNAL(connected()), this, SLOT(OnSend()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(OnWrite()));  // 当服务器有数据来，就会响应该信号
}

void VTKWidget::OnSend()
{
    char buf[] = "1234";
    m_socket->write(buf, sizeof(buf)); //发送数据
}

VTKWidget::~VTKWidget()
{
    std::cout << "===End===\n";
}

void VTKWidget::OnWrite()
{
    //char buf[1024] = { 0 };
    //m_socket->read(buf, 1024);  // 读取数据
    //std::cout << buf << std::endl;

    //char* pixels = new char[1920000]();

    std::stringstream ss;

    int recvSize = 0;
    while (true)
    {
        if (recvSize + 1024 > 1920000)
        {
            int lastSize = 1920000 - recvSize;
            char* recvBuffer = new char[lastSize + 1]();
            auto recSize = m_socket->read(recvBuffer, lastSize);

            ss.write(recvBuffer, lastSize);

            //delete[] recvBuffer;
            //recvBuffer = nullptr;
            break;
        }
        else
        {
            char* recvBuffer = new char[1025]();
            auto recSize = m_socket->read(recvBuffer, 1024);


            ss.write(recvBuffer, 1024);

            //delete[] recvBuffer;
            //recvBuffer = nullptr;

            std::cout << recvSize<<'\t'<<(int)((unsigned char)recvBuffer[0]) << '\t' << (int)((unsigned char)recvBuffer[1]) << '\t' << (int)((unsigned char)recvBuffer[2]) << '\n';
            recvSize += 1024;
        }
    }
    auto str = ss.str();
    auto len = str.size();
    auto pixels = str.c_str();

    auto pix = const_cast<char*>(pixels);
    auto pixs = reinterpret_cast<unsigned char*>(pix);

    auto ret = (unsigned char*)(pixels);

    int w = 800;
    int h = 600;
    QImage image (w, h, QImage::Format::Format_RGB32);
    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
        {
            auto r = static_cast<int>(ret[(i * w + j) * 4 + 0]);
            auto g = static_cast<int>(ret[(i * w + j) * 4 + 1]);
            auto b = static_cast<int>(ret[(i * w + j) * 4 + 2]);

            // vtk左下角开始，qt左上角开始
            image.setPixelColor(j, h - i - 1, QColor(r, g, b));
        }
    }

    image.save("out.bmp");
}



