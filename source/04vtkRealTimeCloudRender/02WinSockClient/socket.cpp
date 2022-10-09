#include "socket.h"
#include <mutex>
#include <iostream>
#include <sstream>
#include <QImage>

#include <thread>

Socket* Socket::m_instance = nullptr;

Socket* Socket::GetInstance()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() {m_instance = new Socket(); });
    return m_instance;
}

Socket::Socket() :
    m_socket(0),
    m_window(nullptr)
{
}

void Socket::InitSocket()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0)
    {
        throw("init socket failed!\n");
    }

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        throw("invalid socket!\n");
    }
}

void Socket::ConnectSocket()
{
    sockaddr_in serAddr{};
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    if (connect(m_socket, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        closesocket(m_socket);
        throw("connect failed!");
    }
}

const std::string Socket::Recv()
{
    while (true)
    {
        constexpr int sizeOfpixelsSizeBuffer = 4;
        char pixelsSizeBuffer[sizeOfpixelsSizeBuffer]{ 0 };
        // 接收当前帧像素大小
        recv(m_socket, pixelsSizeBuffer, sizeOfpixelsSizeBuffer, 0);
        auto pixelsSize = *reinterpret_cast<int*>(pixelsSizeBuffer);
        static int index = 0;
        std::cout << index++ << "\t===\t" << pixelsSize << std::endl;

        if (pixelsSize > 0)
        {
            std::stringstream ssPixels;
            constexpr int bufferSize = 1024;
            char buffer[bufferSize]{ 0 };
            int recvSize = 0;
            int haveRecv = 0;

            while (haveRecv + 1024 <= pixelsSize)
            {
                recvSize = recv(m_socket, buffer, bufferSize, 0);
                ssPixels.write(buffer, recvSize);
                haveRecv += recvSize;
            }
            recvSize = recv(m_socket, buffer, pixelsSize - haveRecv, 0);
            ssPixels.write(buffer, recvSize);
            std::cout << "have recv:" << haveRecv + recvSize << std::endl;

            auto strPixels = ssPixels.str();
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

                    //std::cout << r << '\t' << g << '\t' << b << '\n';

                    // vtk左下角开始，qt左上角开始
                    image.setPixelColor(j, h - i - 1, QColor(r, g, b));
                }
            }

            static int index = 0;
            image.save("Image_" + QString::number(index++) + ".jpg");
        }
    }

    return std::string();
}

void Socket::Recv2()
{
    while (true)
    {
        constexpr int sizeOfpixelsSizeBuffer = 8;
        char pixelsSizeBuffer[sizeOfpixelsSizeBuffer]{ 0 };
        // 接收当前帧像素大小
        recv(m_socket, pixelsSizeBuffer, sizeOfpixelsSizeBuffer, 0);
        auto width = reinterpret_cast<int*>(pixelsSizeBuffer)[0];
        auto height = reinterpret_cast<int*>(pixelsSizeBuffer)[1];

        auto pixelsSize = width * height * 4;

        if (pixelsSize > 0)
        {
            std::stringstream ss;

            constexpr int bufferSize = 1024;
            char buffer[bufferSize]{ 0 };
            int recvSize = 0;
            int haveRecv = 0;

            while (haveRecv + 1024 <= pixelsSize)
            {
                recvSize = recv(m_socket, buffer, bufferSize, 0);
                ss.write(buffer, recvSize);
                haveRecv += recvSize;
            }
            recvSize = recv(m_socket, buffer, pixelsSize - haveRecv, 0);
            ss.write(buffer, recvSize);

            std::cout << "have recv:" << haveRecv + recvSize << std::endl;

            std::string str = ss.str();

            if (m_window)
            {
                m_window->AddPixels(str);
            }
        }
    }
}

void Socket::Send(const char* buffer)
{

}

void Socket::ClostSocket()
{
    closesocket(m_socket);
    WSACleanup();
}
