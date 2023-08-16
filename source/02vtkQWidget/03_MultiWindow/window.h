#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QMainWindow>
#include <QDockWidget>

#include <QVTKRenderWidget.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkCubeSource.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>

// 01_02_TEST66 多个vtkRenderWindow共享vtkActor
// vtkGenericOpenGLRenderWindow是不能共享上下文的，即不能共享actor mapper
// 但是可以把一个window中的actor删除，然后添加到另一个上
// 即不能同时在不同的window中添加同一个actor

class Window:
    public QWidget
{
public:
    Window(QWidget* parent = nullptr)
    {
        auto mainLayout = new QVBoxLayout();
        auto btnLayout = new QHBoxLayout();
        auto mainWindow = new QMainWindow();

        btn1 = new QPushButton("test1");
        btn2 = new QPushButton("test2");
        btn3 = new QPushButton("test3");

        InitConnect();

        btnLayout->addWidget(btn1);
        btnLayout->addWidget(btn2);
        btnLayout->addWidget(btn3);

        mainLayout->addLayout(btnLayout);
        mainLayout->addWidget(mainWindow);

        this->setLayout(mainLayout);

        // 添加三个VTK窗口
        //--------------------------------------------------------
        auto vtkWindow1 = new QVTKRenderWidget();
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow1;
        vtkWindow1->setRenderWindow(renderWindow1);
        m_renderer1 = vtkSmartPointer<vtkRenderer>::New();
        m_renderer1->SetBackground(0, 1, 0);
        renderWindow1->AddRenderer(m_renderer1);

        auto vtkWindow2 = new QVTKRenderWidget();
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow2;
        vtkWindow2->setRenderWindow(renderWindow2);
        m_renderer2 = vtkSmartPointer<vtkRenderer>::New();
        m_renderer2->SetBackground(0, 0, 1);
        renderWindow2->AddRenderer(m_renderer2);

        auto vtkWindow3 = new QVTKRenderWidget();
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow3;
        //renderWindow3->SetSharedRenderWindow(renderWindow2);
        vtkWindow3->setRenderWindow(renderWindow3);
        m_renderer3 = vtkSmartPointer<vtkRenderer>::New();
        m_renderer3->SetBackground(1, 0, 0);
        renderWindow3->AddRenderer(m_renderer3);

        auto dock1 = new QDockWidget();
        dock1->setWidget(vtkWindow2);
        auto dock2 = new QDockWidget();
        dock2->setWidget(vtkWindow3);

        mainWindow->addDockWidget(Qt::LeftDockWidgetArea, dock1);
        mainWindow->addDockWidget(Qt::RightDockWidgetArea, dock2);
        mainWindow->setCentralWidget(vtkWindow1);

        // 添加actor
        //--------------------------------------------------------
        m_actor1 = GenVTKActor(0);
        m_actor2 = GenVTKActor(1);

        m_renderer1->AddActor(m_actor1);
        m_renderer2->AddActor(m_actor2);
        //m_renderer3->AddActor(m_actor2);
    }
    ~Window() = default;

public:
    void InitConnect()
    {
        connect(btn1, &QPushButton::clicked, [this]() {
                m_renderer2->RemoveActor(m_actor2);
                m_renderer3->AddActor(m_actor2);
                Render();
            });
        connect(btn2, &QPushButton::clicked, [this]() {
                vtkNew<vtkActor> actor;
                m_actor1->ShallowCopy(actor);
                Render();
            });
        connect(btn3, &QPushButton::clicked, [this]() {
                m_actor2->GetProperty()->SetColor(0, 1, 0);
                Render();
            });
    }

    void Render()
    {
        m_renderer1->GetRenderWindow()->Render();
        m_renderer2->GetRenderWindow()->Render();
        m_renderer3->GetRenderWindow()->Render();
    }

    vtkSmartPointer<vtkActor> GenVTKActor(int type)
    {
        vtkNew<vtkActor> actor;
        vtkNew<vtkPolyDataMapper> mapper;
        
        if (type == 0)
        {
            vtkNew<vtkCubeSource> source;
            mapper->SetInputConnection(source->GetOutputPort());
        }
        else if (type == 1)
        {
            vtkNew<vtkSphereSource> source;
            mapper->SetInputConnection(source->GetOutputPort());
        }

        actor->SetMapper(mapper);

        return actor;
    }

private:
    QPushButton* btn1{ nullptr };
    QPushButton* btn2{ nullptr };
    QPushButton* btn3{ nullptr };

    vtkSmartPointer<vtkRenderer> m_renderer1{ nullptr };
    vtkSmartPointer<vtkRenderer> m_renderer2{ nullptr };
    vtkSmartPointer<vtkRenderer> m_renderer3{ nullptr };

    vtkSmartPointer<vtkActor> m_actor1{ nullptr };
    vtkSmartPointer<vtkActor> m_actor2{ nullptr };
};
