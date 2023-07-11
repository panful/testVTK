/*
 * 1. 官方示例
 * 2. 官方示例
 */

#define TEST2

#ifdef TEST1

// https://examples.vtk.org/site/Cxx/Animation/AnimateActors/

#include <vtkActor.h>
#include <vtkAnimationCue.h>
#include <vtkAnimationScene.h>
#include <vtkCamera.h>
#include <vtkColor.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkLogger.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkVector.h>
#include <vtkVectorOperators.h>

class ActorAnimator
{
public:
    ActorAnimator() : Actor(nullptr), StartPosition(0, 0, 0), EndPosition(0.5, 0.5, 0.5)
    {
    }

    ~ActorAnimator() = default;

    void SetActor(vtkActor* actor)
    {
        this->Actor = actor;
    }

    void SetStartPosition(const vtkVector3d& position)
    {
        this->StartPosition = position;
    }

    void SetEndPosition(const vtkVector3d& position)
    {
        this->EndPosition = position;
    }

    void AddObserversToCue(vtkAnimationCue* cue)
    {
        cue->AddObserver(vtkCommand::StartAnimationCueEvent, this, &ActorAnimator::Start);
        cue->AddObserver(vtkCommand::EndAnimationCueEvent, this, &ActorAnimator::End);
        cue->AddObserver(vtkCommand::AnimationCueTickEvent, this, &ActorAnimator::Tick);
    }

private:
    //@{
    /**
     * These are callbacks that called when corresponding events are fired by the
     * cue (see AddObserversToCue)
     */
    void Start()
    {
        this->Actor->SetPosition(this->StartPosition.GetData());
    }

    void Tick(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(event), void* calldata)
    {
        // 根据当前时刻计算Actor此刻的位置
        vtkAnimationCue::AnimationCueInfo* info = reinterpret_cast<vtkAnimationCue::AnimationCueInfo*>(calldata);
        const double t                          = (info->AnimationTime - info->StartTime) / (info->EndTime - info->StartTime);
        vtkVector3d position                    = this->StartPosition + (this->EndPosition - this->StartPosition) * t;
        this->Actor->SetPosition(position.GetData());
    }

    void End()
    {
        this->Actor->SetPosition(this->EndPosition.GetData());
    }

    //@}

    vtkSmartPointer<vtkActor> Actor;
    vtkVector3d StartPosition;
    vtkVector3d EndPosition;
};

int main(int argc, char* argv[])
{
    // Colors
    vtkNew<vtkNamedColors> colors;
    vtkColor3d coneColor       = colors->GetColor3d("Tomato");
    vtkColor3d sphereColor     = colors->GetColor3d("Banana");
    vtkColor3d backgroundColor = colors->GetColor3d("Peacock");

    // Create the graphics structure. The renderer renders into the
    // render window.
    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<vtkRenderer> ren1;
    ren1->SetBackground(backgroundColor.GetData());

    vtkNew<vtkRenderWindow> renWin;
    renWin->SetSize(800, 600);
    renWin->AddRenderer(ren1);
    iren->SetRenderWindow(renWin);

    // Generate a sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetPhiResolution(31);
    sphereSource->SetThetaResolution(31);

    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> sphere;
    sphere->SetMapper(sphereMapper);
    sphere->GetProperty()->SetDiffuseColor(sphereColor.GetData());
    sphere->GetProperty()->SetDiffuse(.7);
    sphere->GetProperty()->SetSpecular(.3);
    sphere->GetProperty()->SetSpecularPower(30.0);

    ren1->AddActor(sphere);

    // Generate a cone
    auto coneSource = vtkSmartPointer<vtkConeSource>::New();
    coneSource->SetResolution(31);

    auto coneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    coneMapper->SetInputConnection(coneSource->GetOutputPort());
    auto cone = vtkSmartPointer<vtkActor>::New();
    cone->SetMapper(coneMapper);
    cone->GetProperty()->SetDiffuseColor(coneColor.GetData());

    ren1->AddActor(cone);

    // Create an Animation Scene
    auto scene = vtkSmartPointer<vtkAnimationScene>::New();
    scene->SetModeToRealTime();
    // scene->SetModeToSequence();

    scene->SetLoop(0);
    scene->SetFrameRate(5);
    scene->SetStartTime(0);
    scene->SetEndTime(20);
    scene->AddObserver(vtkCommand::AnimationCueTickEvent, renWin.GetPointer(), &vtkWindow::Render);

    // Create an Animation Cue for each actor
    auto cue1 = vtkSmartPointer<vtkAnimationCue>::New();
    cue1->SetStartTime(5);
    cue1->SetEndTime(23);
    scene->AddCue(cue1);

    auto cue2 = vtkSmartPointer<vtkAnimationCue>::New();
    cue2->SetStartTime(1);
    cue2->SetEndTime(10);
    scene->AddCue(cue2);

    // Create an ActorAnimator for each actor;
    ActorAnimator animateSphere;
    animateSphere.SetActor(sphere);
    animateSphere.AddObserversToCue(cue1);

    ActorAnimator animateCone;
    animateCone.SetEndPosition(vtkVector3d(-1, -1, -1));
    animateCone.SetActor(cone);
    animateCone.AddObserversToCue(cue2);

    renWin->SetWindowName("AnimateActors");

    renWin->Render();
    ren1->ResetCamera();
    ren1->GetActiveCamera()->Dolly(.5);
    ren1->ResetCameraClippingRange();

    // Create Cue observer.
    scene->Play();
    scene->Stop();

    iren->Start();
    return EXIT_SUCCESS;
}

#endif // TEST1

#ifdef TEST2

// https://examples.vtk.org/site/Cxx/Utilities/AnimationScene/

#include <vtkAnimationCue.h>
#include <vtkAnimationScene.h>
#include <vtkCommand.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
class CueAnimator
{
public:
    CueAnimator()
    {
        this->SphereSource = 0;
        this->Mapper       = 0;
        this->Actor        = 0;
    }

    ~CueAnimator()
    {
        this->Cleanup();
    }

    void StartCue(vtkAnimationCue::AnimationCueInfo* vtkNotUsed(info), vtkRenderer* ren)
    {
        vtkNew<vtkNamedColors> colors;

        cout << "*** IN StartCue " << endl;
        this->SphereSource = vtkSphereSource::New();
        this->SphereSource->SetRadius(0.5);

        this->Mapper = vtkPolyDataMapper::New();
        this->Mapper->SetInputConnection(this->SphereSource->GetOutputPort());

        this->Actor = vtkActor::New();
        this->Actor->SetMapper(this->Mapper);
        this->Actor->GetProperty()->SetSpecular(0.6);
        this->Actor->GetProperty()->SetSpecularPower(30);
        this->Actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

        ren->AddActor(this->Actor);
        ren->ResetCamera();
        ren->Render();
    }

    // 动画开始之后会循环执行该函数，直到动画结束
    void Tick(vtkAnimationCue::AnimationCueInfo* info, vtkRenderer* ren)
    {
        auto deltaTime = static_cast<double>(info->AnimationTime - info->StartTime) / static_cast<double>(info->EndTime - info->StartTime);

        // 随着时间改变半径
        double newRadius = 0.1 + deltaTime;
        this->SphereSource->SetRadius(newRadius);

        // 随着时间向x方向移动
        // double newX = deltaTime;
        // this->SphereSource->SetCenter(newX, 0.0, 0.0);

        this->SphereSource->Update();
        ren->Render();
    }

    void EndCue(vtkAnimationCue::AnimationCueInfo* vtkNotUsed(info), vtkRenderer* ren)
    {
        (void)ren;
        // don't remove the actor for the regression image.
        //      ren->RemoveActor(this->Actor);
        this->Cleanup();
    }

protected:
    vtkSphereSource* SphereSource;
    vtkPolyDataMapper* Mapper;
    vtkActor* Actor;

    void Cleanup()
    {
        if (this->SphereSource != 0)
        {
            this->SphereSource->Delete();
            this->SphereSource = 0;
        }

        if (this->Mapper != 0)
        {
            this->Mapper->Delete();
            this->Mapper = 0;
        }
        if (this->Actor != 0)
        {
            this->Actor->Delete();
            this->Actor = 0;
        }
    }
};

class vtkAnimationCueObserver : public vtkCommand
{
public:
    static vtkAnimationCueObserver* New()
    {
        return new vtkAnimationCueObserver;
    }

    // 执行动画的开始、活动、结束三个状态
    virtual void Execute(vtkObject* vtkNotUsed(caller), unsigned long event, void* calldata)
    {
        if (this->Animator != 0 && this->Renderer != 0)
        {
            vtkAnimationCue::AnimationCueInfo* info = static_cast<vtkAnimationCue::AnimationCueInfo*>(calldata);
            switch (event)
            {
            case vtkCommand::StartAnimationCueEvent:
                this->Animator->StartCue(info, this->Renderer);
                break;
            case vtkCommand::EndAnimationCueEvent:
                this->Animator->EndCue(info, this->Renderer);
                break;
            case vtkCommand::AnimationCueTickEvent:
                this->Animator->Tick(info, this->Renderer);
                break;
            }
        }
        if (this->RenWin != 0)
        {
            this->RenWin->Render();
        }
    }

    vtkRenderer* Renderer;
    vtkRenderWindow* RenWin;
    CueAnimator* Animator;

protected:
    vtkAnimationCueObserver()
    {
        this->Renderer = 0;
        this->Animator = 0;
        this->RenWin   = 0;
    }
};
} // namespace

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    // Create the graphics structure. The renderer renders into the
    // render window.
    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<vtkRenderer> ren1;
    vtkNew<vtkRenderWindow> renWin;
    renWin->SetSize(800, 600);
    renWin->SetMultiSamples(0);
    renWin->SetWindowName("AnimationScene");

    iren->SetRenderWindow(renWin);
    renWin->AddRenderer(ren1);
    ren1->SetBackground(colors->GetColor3d("MistyRose").GetData());
    renWin->Render();

    // Create an Animation Scene
    vtkNew<vtkAnimationScene> scene;

    scene->SetModeToRealTime();
    // scene->SetModeToSequence();

    scene->SetLoop(0);
    scene->SetFrameRate(5);
    scene->SetStartTime(3);
    scene->SetEndTime(20);

    // Create an Animation Cue.
    vtkNew<vtkAnimationCue> cue1;
    cue1->SetStartTime(5);
    cue1->SetEndTime(23);
    scene->AddCue(cue1);

    // Create cue animator;
    CueAnimator animator;

    // Create Cue observer.
    vtkNew<vtkAnimationCueObserver> observer;
    observer->Renderer = ren1;
    observer->Animator = &animator;
    observer->RenWin   = renWin;

    cue1->AddObserver(vtkCommand::StartAnimationCueEvent, observer);
    cue1->AddObserver(vtkCommand::EndAnimationCueEvent, observer);
    cue1->AddObserver(vtkCommand::AnimationCueTickEvent, observer);

    scene->Play();
    scene->Stop();

    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST2
