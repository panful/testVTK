/**
 * 1. vtkTextActor
 * 2. vtkTextActor 显示中文
 * 3. vtkTextWidget
 */

#define TEST2

#ifdef TEST1

#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

/**
 * FTC_Manager 使用缓存方式加载FreeType字体
 */

int main(int, char*[])
{
    vtkNew<vtkTextActor> txt;
    txt->SetInput("Hello World!");
    txt->SetDisplayPosition(20, 30);

    vtkTextProperty* txtprop = txt->GetTextProperty();
    txtprop->SetFontFamilyToArial();
    txtprop->ItalicOff();                        // 斜体
    txtprop->FrameOff();                         // 文本框
    txtprop->SetJustificationToLeft();           // 对齐方式为左对齐
    txtprop->SetVerticalJustificationToBottom(); // 以底部垂直对齐
    txtprop->SetOpacity(.5);                     // 不透明度
    txtprop->BoldOff();                          // 加粗
    txtprop->SetFontSize(36);                    // 字体大小
    txtprop->ShadowOff();                        // 文本阴影
    txtprop->SetShadowOffset(4, 4);              // 阴影偏移量，即文本到其阴影的距离，单位与FontSize相同。
    txtprop->SetColor(1., 0., 0.);

    vtkNew<vtkRenderer> ren;
    ren->AddActor(txt);
    ren->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renWin;
    renWin->SetWindowName("TextActor");
    renWin->AddRenderer(ren);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST1

#ifdef TEST2

#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

int main(int, char*[])
{
    vtkNew<vtkTextActor> txt;
    txt->SetInput("Hello 你好，这是中文 world");
    txt->SetDisplayPosition(20, 30);

    vtkTextProperty* txtprop = txt->GetTextProperty();
    txtprop->SetFontFamily(VTK_FONT_FILE);                // 使用字体文件（默认的字体不支持中文）
    txtprop->SetFontFile("../resources/fonts/msyhl.ttc"); // 字体文件路径
    txtprop->SetFontSize(32);
    txtprop->SetColor(1., 0., 0.);

    vtkNew<vtkRenderer> ren;
    ren->AddActor(txt);
    ren->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renWin;
    renWin->SetWindowName("TextActor");
    renWin->AddRenderer(ren);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST2

#ifdef TEST3

#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>
#include <vtkTextWidget.h>

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    // Create the RenderWindow, Renderer and both Actors.
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    // Create a test pipeline.
    vtkNew<vtkSphereSource> sphereSource;
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // Create the widget
    vtkNew<vtkTextActor> textActor;
    textActor->SetInput("This is a test");
    textActor->GetTextProperty()->SetColor(colors->GetColor3d("Lime").GetData());

    vtkNew<vtkTextWidget> textWidget;

    vtkNew<vtkTextRepresentation> textRepresentation;
    textRepresentation->GetPositionCoordinate()->SetValue(0.15, 0.15);
    textRepresentation->GetPosition2Coordinate()->SetValue(0.7, 0.2);
    textWidget->SetRepresentation(textRepresentation);

    textWidget->SetInteractor(interactor);
    textWidget->SetTextActor(textActor);
    textWidget->SelectableOff();

    // Add the actors to the renderer, set the background and size.
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
    renderWindow->SetSize(300, 300);
    renderWindow->SetWindowName("TextWidget");

    interactor->Initialize();
    renderWindow->Render();
    textWidget->On();
    renderWindow->Render();

    interactor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST3
