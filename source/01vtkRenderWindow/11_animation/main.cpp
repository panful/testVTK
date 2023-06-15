/*
 * 1. https://examples.vtk.org/site/Cxx/Animation/AnimateActors/
 */

#define TEST1

#ifdef TEST1

#include <vtkActor.h>
#include <vtkAnimationCue.h>
#include <vtkCommand.h>
#include <vtkSmartPointer.h>
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

int main(int argc, char* argv[])
{
    vtkLogger::Init(argc, argv);

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
    iren->SetRenderWindow(renWin);
    renWin->AddRenderer(ren1);

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
    if (argc >= 2 && strcmp(argv[1], "-real") == 0)
    {
        vtkLogF(INFO, "real-time mode");
        scene->SetModeToRealTime();
    }
    else
    {
        vtkLogF(INFO, "sequence mode");
        scene->SetModeToSequence();
    }
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