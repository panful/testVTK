#include "mainWindow.h"
#include "ui_MainWindow.h"
#include "viewer.h"
#include "window.h"
#include "command.h"
#include "setColorCommand.h"

#include <QGridLayout>
#include <QPainter>

LeoMainWindow::LeoMainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow())
{
    this->ui->setupUi(this);

    connect(this->ui->pushButton_1, SIGNAL(clicked()), this, SLOT(OnBtn_1()));
    connect(this->ui->pushButton_2, SIGNAL(clicked()), this, SLOT(OnBtn_2()));
    connect(this->ui->pushButton_3, SIGNAL(clicked()), this, SLOT(OnBtn_3()));
    connect(this->ui->pushButton_4, SIGNAL(clicked()), this, SLOT(OnBtn_4()));
    connect(this->ui->pushButton_5, SIGNAL(clicked()), this, SLOT(OnBtn_5()));
    connect(this->ui->pushButton_6, SIGNAL(clicked()), this, SLOT(OnBtn_6()));
    connect(this->ui->pushButton_7, SIGNAL(clicked()), this, SLOT(OnBtn_7()));

    m_renderWindow = std::make_shared<LeoRenderWindow>(this->ui->renderWindow);
    m_invoker = std::make_shared<LeoInvoker>();
}

void LeoMainWindow::InitViewer()
{
}

void LeoMainWindow::OnBtn_1()
{
    m_renderWindow->ScreenCapture();
}

void LeoMainWindow::OnBtn_2()
{
    m_renderWindow->BeforeWindow();
}

void LeoMainWindow::OnBtn_3()
{
    m_renderWindow->SetViewerMode(LeoRenderWindow::ViewerMode::VM_2);
}

void LeoMainWindow::OnBtn_4()
{
    m_renderWindow->SetViewerMode(LeoRenderWindow::ViewerMode::VM_0);
}
void LeoMainWindow::OnBtn_5()
{
    m_renderWindow->SetViewerMode(LeoRenderWindow::ViewerMode::VM_1);
}
void LeoMainWindow::OnBtn_6()
{
    m_invoker->AddCommand(new SetColor(m_renderWindow, { 1,0,0 }));
    m_invoker->Notify();
}
void LeoMainWindow::OnBtn_7()
{
    m_invoker->AddCommand(new SetColor(m_renderWindow, { 0,0,1 }));
    m_invoker->Notify();
}