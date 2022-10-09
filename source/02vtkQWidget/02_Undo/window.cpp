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
    auto btnRedo = new QPushButton("redo");
    auto btnUndo = new QPushButton("undo");
    auto vlayout = new QVBoxLayout();
    auto hLayout = new QHBoxLayout();
    m_renderWidget = new VTKWidget();

    hLayout->addWidget(btnRedo);
    hLayout->addWidget(btnUndo);

    vlayout->addLayout(hLayout);
    vlayout->addWidget(m_renderWidget);

    this->setLayout(vlayout);

    connect(btnRedo, SIGNAL(clicked()), this, SLOT(OnRedo()));
    connect(btnUndo, SIGNAL(clicked()), this, SLOT(OnUndo()));
}

void Window::OnUndo()
{
    auto style = m_renderWidget->renderWindow()->GetInteractor()->GetInteractorStyle();

    auto myStyle = dynamic_cast<Style*>(style);
    myStyle->UndoCamera();
}

void Window::OnRedo()
{
    auto style = m_renderWidget->renderWindow()->GetInteractor()->GetInteractorStyle();

    auto myStyle = dynamic_cast<Style*>(style);
    myStyle->RedoCamera();
}