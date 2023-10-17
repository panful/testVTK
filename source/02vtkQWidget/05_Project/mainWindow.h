#pragma once

#include "rinterface.h"
#include <QBoxLayout>
#include <QDockWidget>
#include <QMainWindow>
#include <QPushButton>
#include <memory>

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget* p = nullptr) : QMainWindow(p)
    {
        auto rw = new QWidget();
        this->setCentralWidget(rw);
        m_ri->CreateRenderWindow(rw);

        {
            auto widget = new QWidget();
            auto btn    = new QPushButton("SetColor");
            auto layout = new QVBoxLayout();
            auto dock   = new QDockWidget();

            layout->addWidget(btn);
            widget->setLayout(layout);
            dock->setMinimumHeight(100);
            dock->setWidget(widget);
            dock->setStyleSheet("background-color:rgb(25,50,75);");

            QObject::connect(btn, &QPushButton::clicked,
                [this]()
                {
                    float color[3] { .1f, .2f, .3f };
                    m_ri->SetBackground(color);
                });

            this->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dock);
        }

        {
            auto widget = new QWidget();
            auto btn    = new QPushButton("SetColor");
            auto layout = new QVBoxLayout();
            auto dock   = new QDockWidget();

            layout->addWidget(btn);
            widget->setLayout(layout);
            dock->setMinimumHeight(100);
            dock->setWidget(widget);
            dock->setStyleSheet("background-color:rgb(75,50,25);");

            QObject::connect(btn, &QPushButton::clicked,
                [this]()
                {
                    float color[3] { .3f, .2f, .1f };
                    m_ri->SetBackground(color);
                });

            this->addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, dock);
        }
    }

private:
    std::unique_ptr<RenderInterface> m_ri { std::make_unique<RenderInterface>() };
};
