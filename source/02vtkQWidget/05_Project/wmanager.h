#pragma once

#include "iviewer.h"
#include "widget.h"
#include <map>

class WindowManager
{
public:
    void CreateRenderWindow(QWidget* w)
    {
        auto window   = new Window(w);
        auto windowUi = new WindowUi(window);

        m_windows[0] = windowUi;
    }

    IViewer* GetActiveViewer() const
    {
        return dynamic_cast<IViewer*>(m_windows.at(0)->GetIViewer());
    }

private:
    std::map<uint32_t, Window*> m_windows;
};