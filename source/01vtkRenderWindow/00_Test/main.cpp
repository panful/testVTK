
/*
* 1. 引用计数
* 2. 主动释放vtk对象
*/

#define TEST1

#ifdef TEST1

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkObjectFactory.h>

#include <vector>
#include <stack>
#include <iostream>

class CustomWidget :public vtkOrientationMarkerWidget
{
public:
    static CustomWidget* New();
    vtkTypeMacro(CustomWidget, vtkOrientationMarkerWidget);

protected:
    CustomWidget()
    {
        std::cout << "construct\n";
        std::cout << "ref count:" << this->GetReferenceCount() << '\n'; //1
    }
    ~CustomWidget() override
    {
        std::cout << "destruct\n";
        std::cout << "ref count:" << this->GetReferenceCount() << '\n'; //0
    }

private:
    CustomWidget(const CustomWidget&) = delete;
    CustomWidget& operator=(const CustomWidget&) = delete;
};

vtkStandardNewMacro(CustomWidget);

int main(int, char* [])
{
    {
        std::stack<vtkSmartPointer<vtkCamera>> cameras;
        vtkNew<vtkCamera> camera1;
        cameras.emplace(camera1);
        cameras.pop();
    }

    {
        std::stack<vtkSmartPointer<CustomWidget>> widgets;
        vtkNew<CustomWidget> widget;
        widgets.emplace(widget);
        widgets.pop();
    }


    std::cout << "11111111111\n";
    {
        std::vector<vtkSmartPointer<CustomWidget>> widgets;
        vtkNew<CustomWidget> w;
        vtkSmartPointer<CustomWidget> w1 = vtkSmartPointer<CustomWidget>::New();
        std::cout << w->GetReferenceCount() << '\n';
        widgets.emplace_back(w);
        std::cout << w->GetReferenceCount() << '\n';

        //widgets.front()->Delete();  //崩溃
        //widgets.front()->FastDelete();//崩溃
        //w->Delete();//崩溃
        //w = nullptr;//不能赋值为nullptr

        std::cout << "==========\n";

    }

    std::cout << "2222222222222\n";
    {
        std::vector<vtkSmartPointer<CustomWidget>> widgets;
        vtkSmartPointer<CustomWidget> w = vtkSmartPointer<CustomWidget>::New();
        std::cout << w->GetReferenceCount() << '\n';
        widgets.emplace_back(w);
        std::cout << w->GetReferenceCount() << '\n';
        //widgets.front()->Delete();
        //widgets.front()->FastDelete();
        //w->Delete();

        //w = nullptr;
        widgets.front() = nullptr;
        w = nullptr;

        std::cout << "==========\n";
    }

    std::cout << "3333333333333\n";
    {

        vtkSmartPointer<CustomWidget> w = vtkSmartPointer<CustomWidget>::New();
        std::cout << w->GetReferenceCount() << '\n';
        //widgets.front()->Delete();
        //widgets.front()->FastDelete();
        //w->Delete();

        std::cout << "==========\n";
    }

    return 0;
}
#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <vector>
#include <vtkObject.h> 
#include <vtkObjectFactory.h> 
#include <vtkSmartPointer.h> 
class vtkMyClass : public vtkObject {
public:
    vtkTypeMacro(vtkMyClass, vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent) {}
    static vtkMyClass* New();
protected:
    vtkMyClass();
    ~vtkMyClass();
};
vtkStandardNewMacro(vtkMyClass)
vtkMyClass::vtkMyClass() {
    std::cerr << "constructor called\n";
}
vtkMyClass::~vtkMyClass() {
    std::cerr << "destructor called\n";
}
int main()
{
    {
        //std::vector<vtkSmartPointer<vtkMyClass>> vec;
        std::cerr << __LINE__ << '\n';
        vtkSmartPointer<vtkMyClass> myObject;
        std::cerr << __LINE__ << '\n';
        myObject = vtkSmartPointer<vtkMyClass>::New();
        //vec.emplace_back(myObject);
        std::cerr << __LINE__ << '\n';
        myObject = NULL; // calls destructor 
        std::cerr << __LINE__ << '\n';
    }

    return 0;
}
#endif // TEST2


