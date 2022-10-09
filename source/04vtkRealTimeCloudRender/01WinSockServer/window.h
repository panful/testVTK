#ifndef _WINDOW_H_
#define _WINDOW_H_

class Window
{
public:
    Window(short index, int width, int height);
    ~Window() = default;

public:
    void SetSize(int width, int height);

};

#endif // !_WINDOW_H_
