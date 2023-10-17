#pragma once

struct ActorData
{
};

struct IViewer
{
    virtual void SetBackground(float*)      = 0;
    virtual void AddActor(const ActorData*) = 0;
};
