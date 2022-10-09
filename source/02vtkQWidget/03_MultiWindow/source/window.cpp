#include "window.h"
#include "viewer.h"
#include "screenCapture.h"

#include <QMainWindow>
#include <QDockWidget>
#include <QGridLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QBoxLayout>

#include <stdexcept>

class CustomDockWidget :public QDockWidget
{
public:
    void closeEvent(QCloseEvent* event) override
    {
        std::cout << "closed\n";


        this->parentWidget();
    }
};

void LeoRenderWindow::TestSlot(bool arg)
{
    std::cout << arg << "\t***Test slot\n";
}

LeoRenderWindow::LeoRenderWindow(QWidget* parent) :
    m_parentWindow(parent)
{
    if (parent == nullptr)
    {
        throw std::invalid_argument("LeoRenderWindow::LeoRenderWindow(QWidget* parent) parameter is null.");
    }

    m_stackedWidget = new QStackedWidget();
    m_renderWidget = new QMainWindow();
    //m_renderWidget->layout()->setContentsMargins(0, 0, 0, 0);
    //auto defaultWidget = new CustomDockWidget();
    m_viewers[0] = std::make_shared<LeoViewer>();
    //auto defaultDock = std::make_shared<QDockWidget>();
    auto defaultDock = new QDockWidget();
    defaultDock->setFeatures(defaultDock->features() & ~CustomDockWidget::DockWidgetClosable);
    defaultDock->setWindowTitle("Viewer 1");
    defaultDock->setWidget(m_viewers[0].get());

    m_renderWidget->addDockWidget(Qt::BottomDockWidgetArea, defaultDock);

    m_stackedWidget->addWidget(m_renderWidget);
    m_stackedWidget->setCurrentWidget(m_renderWidget);

    auto mainLayout = new QVBoxLayout();
    QPushButton* toolBtn1 = new QPushButton("tool btn");
    QPushButton* toolBtn2 = new QPushButton("tool btn");
    toolBtn1->setMaximumSize(100, 50);
    toolBtn2->setMaximumSize(100, 50);
    auto btnLayout = new QHBoxLayout();
    btnLayout->addWidget(toolBtn1);
    btnLayout->addWidget(toolBtn2);

    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(m_stackedWidget);

    m_parentWindow->setLayout(mainLayout);
}

void LeoRenderWindow::ScreenCapture()
{
    if (m_stackedWidget->currentWidget() == m_captureWidget)
    {
        return;
    }

    if (m_captureWidget == nullptr)
    {
        m_captureWidget = new QWidget();
        auto layout = new QGridLayout();
        layout->setContentsMargins(0, 0, 0, 0);
        m_captureWidget->setLayout(layout);
        m_stackedWidget->addWidget(m_captureWidget);
    }

    if (m_lastCapWidget == nullptr)
    {
        auto captureWidget = new LeoScreenCapture();
        captureWidget->SetWidget(m_renderWidget);
        m_lastCapWidget = captureWidget->GetWidget();
        m_captureWidget->layout()->addWidget(m_lastCapWidget);
    }
    else
    {
        auto newWidget = new LeoScreenCapture();
        newWidget->SetWidget(m_renderWidget);
        m_captureWidget->layout()->replaceWidget(m_lastCapWidget, newWidget->GetWidget());
        m_captureWidget->layout()->removeWidget(m_lastCapWidget);
        m_captureWidget->layout()->addWidget(newWidget->GetWidget());
        m_lastCapWidget = newWidget->GetWidget();
    }

    m_stackedWidget->setCurrentWidget(m_captureWidget);
}

void LeoRenderWindow::BeforeWindow()
{
    if (m_stackedWidget->currentWidget() == m_renderWidget)
    {
        return;
    }

    m_stackedWidget->setCurrentWidget(m_renderWidget);
}

void LeoRenderWindow::SetViewerMode(ViewerMode mode)
{
    //switch (mode)
    //{
    //case LeoRenderWindow::ViewerMode::VM_0:
    //{
    //    dynamic_cast<QMainWindow*>(m_renderWidget)->removeDockWidget(m_viewers[1]);
    //}
    //break;
    //case LeoRenderWindow::ViewerMode::VM_1:
    //    break;
    //case LeoRenderWindow::ViewerMode::VM_2:
    //{
    //    auto defaultWidget = new CustomDockWidget();
    //    m_viewers[1] = defaultWidget;
    //    m_viewer1 = new LeoViewer();
    //    defaultWidget->setWidget(m_viewer1);
    //    //defaultWidget->setFeatures(defaultWidget->features() & ~QDockWidget::DockWidgetMovable);   // 不能用鼠标拖出来，但是可以通过点击标题栏按钮拖出来
    //    //defaultWidget->setFeatures(defaultWidget->features() & ~QDockWidget::DockWidgetClosable);   // 不能关闭
    //    //defaultWidget->setFeatures(QDockWidget::NoDockWidgetFeatures); // 不能关闭，移动，漂浮
    //    //defaultWidget->setWindowTitle("test");
    //    //QWidget* emptyWidget = new QWidget();
    //    //QPushButton* btn = new QPushButton("test btn", emptyWidget);
    //    //defaultWidget->setTitleBarWidget(emptyWidget);
    //    //defaultWidget->titleBarWidget()->setMinimumHeight(50);
    //    connect(defaultWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(TestSlot(bool)));
    //    dynamic_cast<QMainWindow*>(m_renderWidget)->addDockWidget(Qt::TopDockWidgetArea, defaultWidget);
    //    //dynamic_cast<QMainWindow*>(m_renderWidget)->removeDockWidget();
    //    //dynamic_cast<QMainWindow*>(m_renderWidget)->tabifyDockWidget();
    //    //dynamic_cast<QMainWindow*>(m_renderWidget)->restoreDockWidget();

    //}
    //break;
    //default:
    //    break;
    //}
}

void LeoRenderWindow::AddGeometryData(const std::string& fileName)
{
    //auto ret = dynamic_cast<LeoViewer*>(m_viewers[0]);
    //dynamic_cast<LeoViewer*>(m_viewers[0])->TestPrint();
    //m_viewers[0]
    //m_viewer1->TestPrint();
}


void LeoRenderWindow::SetColor(std::array<double, 3> color)
{
    //auto ret1 = m_viewers[0]->widget();
    //auto ret2 = dynamic_cast<LeoViewer*>(ret1);
    //auto ret = dynamic_cast<LeoViewer*>(m_viewers[0]);
    //dynamic_cast<LeoViewer*>(m_viewers[0])->SetColor(color);
    //ret2->SetColor(color);
}