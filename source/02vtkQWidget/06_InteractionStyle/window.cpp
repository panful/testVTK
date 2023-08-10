#include "window.h"
#include "widget.h"
#include "Style.h"
#include <vtkRenderWindow.h>
#include <QVBoxLayout>
#include <QPUshButton>
#include <iostream>

Window::Window(QWidget* parent):
    QWidget(parent)
{
    auto btn1 = new QPushButton("Rotate");
    auto btn2 = new QPushButton("Zoom");
    auto btn3 = new QPushButton("Translation");
    auto vlayout = new QVBoxLayout();
    auto hLayout = new QHBoxLayout();
    m_renderWidget = new VTKWidget();

    hLayout->addWidget(btn1);
    hLayout->addWidget(btn2);
    hLayout->addWidget(btn3);

    vlayout->addLayout(hLayout);
    vlayout->addWidget(m_renderWidget);

    this->setLayout(vlayout);

    connect(btn1, &QPushButton::clicked, [this]() 
        {
            dynamic_cast<Style*>(m_renderWidget->renderWindow()->GetInteractor()->GetInteractorStyle())
                ->SetInteractionMode(Style::InteractionMode::Rotate); 
        });

    connect(btn2, &QPushButton::clicked, [this]()
        {
            dynamic_cast<Style*>(m_renderWidget->renderWindow()->GetInteractor()->GetInteractorStyle())
                ->SetInteractionMode(Style::InteractionMode::Zoom);
        });

    connect(btn3, &QPushButton::clicked, [this]()
        {
            dynamic_cast<Style*>(m_renderWidget->renderWindow()->GetInteractor()->GetInteractorStyle())
                ->SetInteractionMode(Style::InteractionMode::Translation);
        });
}
