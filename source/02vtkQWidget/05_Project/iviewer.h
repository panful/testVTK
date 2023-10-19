#pragma once

struct ActorData
{
    uint32_t id;
};

struct IViewer
{
    virtual void SetBackground(float*)         = 0;
    virtual void AddActor(const ActorData*)    = 0;
    virtual void RemoveActor(const ActorData*) = 0;
    virtual void SetColor(uint32_t, float*)    = 0;
    virtual void SetVisible(uint32_t, bool)    = 0;
    virtual void FitView()                     = 0;
    virtual void Render()                      = 0;
};
