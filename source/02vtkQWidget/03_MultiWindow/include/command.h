
#ifndef Command_H
#define Command_H


#include <iostream>
#include <string>
#include <queue>
#include <thread>

class Command
{
public:
    virtual ~Command() = default;
    virtual void Execute() const = 0;
};

// 应该设置为单例
class LeoInvoker {
public:
    LeoInvoker()
    {
        //std::thread t(&LeoInvoker::run, this);
        //t.detach();
    }

    void run()
    {
        while (true)
        {
            //std::cout << m_command.size() << " === \n";

            if (m_command.empty())
                continue;
            std::cout << "===============\n";
            m_command.front()->Execute();
            m_command.pop();
        }
    }

    void AddCommand(Command* command)
    {
        //if (dynamic_cast<SetIntCommand*>(command))
        //    std::cout << "success\n";

        m_command.emplace(command);
    }

    void Notify()
    {
        while (!m_command.empty())
        {
            m_command.front()->Execute();
            m_command.pop();
        }
    }

    // 起一个线程，一直循环遍历 command
    std::queue<Command*> m_command;
};

class Receiver {
public:
    void SetInt(int n)
    {

    }
};

#endif // !Command_H