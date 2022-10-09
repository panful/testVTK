#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include <QQuickVTKRenderWindow.h>
#include <QQuickVTKRenderItem.h>
#include <vtkPolyDataMapper.h>
#include <vtkConeSource.h>

/*
int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QQuickVTKRenderWindow::setupGraphicsBackend();

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("QQuickVTK.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
*/

int main(int argc, char* argv[])
{
    QQuickVTKRenderWindow::setupGraphicsBackend();
    QGuiApplication app(argc, argv);
    
    QQmlApplicationEngine engine;
    engine.load(QUrl("QQuickVTK.qml"));
    
    QObject* topLevel = engine.rootObjects().value(0);
    QQuickWindow* window = qobject_cast<QQuickWindow*>(topLevel);
    
    window->show();
    
    // Fetch the QQuick window using the standard object name set up in the constructor
    QQuickVTKRenderItem* qquickvtkItem = topLevel->findChild<QQuickVTKRenderItem*>("ConeView");
    
    // Create a cone pipeline and add it to the view
    vtkNew<vtkActor> actor;
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkConeSource> cone;
    mapper->SetInputConnection(cone->GetOutputPort());
    actor->SetMapper(mapper);
    qquickvtkItem->renderer()->AddActor(actor);
    qquickvtkItem->renderer()->ResetCamera();
    qquickvtkItem->renderer()->SetBackground(1.0, 0.0, 0.0);
    qquickvtkItem->renderer()->SetBackground2(0.0, 1.0, 0.0);
    qquickvtkItem->renderer()->SetGradientBackground(true);
    qquickvtkItem->update();

    return app.exec();
}