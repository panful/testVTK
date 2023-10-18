#pragma once

#include "rinterface.h"
#include <QBoxLayout>
#include <QDockWidget>
#include <QMainWindow>
#include <QPushButton>
#include <memory>
#include <random>
#include <vector>

class MainWindow : public QMainWindow
{
    Q_OBJECT;

public:
    MainWindow(QWidget* p = nullptr) : QMainWindow(p)
    {
        auto rw = new QWidget();
        this->setCentralWidget(rw);
        m_ri->CreateRenderWindow(rw);

        {
            auto widget = new QWidget();
            auto layout = new QVBoxLayout();
            auto dock   = new QDockWidget();

            widget->setLayout(layout);
            dock->setMinimumHeight(100);
            dock->setWidget(widget);
            dock->setStyleSheet("background-color:rgb(200,200,200);");

            auto btn_addGeometry   = new QPushButton("AddGeometry");
            auto btn_createMesh    = new QPushButton("CreateMesh");
            auto btn_createContour = new QPushButton("CreateContour");
            auto btn_createVector  = new QPushButton("CreateVector");
            auto btn_showEntity    = new QPushButton("ShowEntity");
            auto btn_hideEntity    = new QPushButton("HideEntity");

            layout->addWidget(btn_addGeometry);
            layout->addWidget(btn_createMesh);
            layout->addWidget(btn_createContour);
            layout->addWidget(btn_createVector);
            layout->addWidget(btn_showEntity);
            layout->addWidget(btn_hideEntity);

            QObject::connect(btn_addGeometry, &QPushButton::clicked,
                [this]()
                {
                    int index { 0 };
                    std::vector<float> vertices {};
                    std::vector<int> indices {};
                    m_ri->AddGeometry(index, vertices, indices);
                });

            QObject::connect(btn_createMesh, &QPushButton::clicked,
                [this]()
                {
                    int index { 0 };
                    m_entities.emplace_back(m_ri->CreateMesh(index));
                });

            QObject::connect(btn_createContour, &QPushButton::clicked,
                [this]()
                {
                    // int index { 0 };
                    // m_entities.emplace_back(m_ri->CreateMesh(index));
                });

            QObject::connect(btn_createVector, &QPushButton::clicked,
                [this]()
                {
                    // int index { 0 };
                    // m_entities.emplace_back(m_ri->CreateMesh(index));
                });

            QObject::connect(btn_showEntity, &QPushButton::clicked,
                [this]()
                {
                    auto entity = m_entities.front();
                    m_ri->ShowEntity(entity);
                });

            QObject::connect(btn_hideEntity, &QPushButton::clicked,
                [this]()
                {
                    auto entity = m_entities.front();
                    m_ri->HideEntity(entity);
                });

            this->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dock);
        }

        {
            auto widget = new QWidget();
            auto layout = new QHBoxLayout();
            auto dock   = new QDockWidget();

            widget->setLayout(layout);
            dock->setMinimumHeight(100);
            dock->setWidget(widget);
            dock->setStyleSheet("background-color:rgb(200,200,200);");

            auto btn_setColor = new QPushButton("SetColor");
            layout->addWidget(btn_setColor);

            QObject::connect(btn_setColor, &QPushButton::clicked,
                [this]()
                {
                    static std::default_random_engine engine;
                    std::uniform_real_distribution<float> dist(0.f, 1.f);
                    auto entity = m_entities.front();
                    float color[3] { dist(engine), dist(engine), dist(engine) };
                    m_ri->SetColor(entity, color);
                });

            this->addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, dock);
        }
    }

private:
    std::unique_ptr<RenderInterface> m_ri { std::make_unique<RenderInterface>() };
    std::vector<Entity> m_entities;
};
