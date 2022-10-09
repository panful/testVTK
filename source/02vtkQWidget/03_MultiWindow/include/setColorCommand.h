
#ifndef SetColor_H
#define SetColor_H


#include "command.h"
#include "mainWindow.h"
#include "viewer.h"


class SetIntCommand : public Command {
private:
    int m_int;

public:
    explicit SetIntCommand(int a) :
        m_int(a)
    {
    }

    void Execute() const override
    {
        std::cout << "第一个命令\n";
    }
};

class SetColor : public Command
{
public:
    SetColor() = default;
    SetColor(std::shared_ptr<LeoRenderWindow> window, std::array<double, 3> color) :
        m_win(window),
        m_color(color)
    {
    }

    void Execute() const override
    {
        m_win->SetColor(m_color);
    }

    std::array<double, 3> m_color;
    std::shared_ptr<LeoRenderWindow> m_win;
};

#endif // !SetColor_H