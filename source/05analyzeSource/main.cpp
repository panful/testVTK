
/**
 * 1. 整个渲染的流程 生成vbo和ibo DrawCall
 * 2. shader源码的生成、修改、编译、uniform的设置
 * 3. 渐变背景色
 * 4. 设置每个单元的颜色
 * 5. 使用颜色映射表设置图元（顶点）颜色，纹理坐标的生成，纹理的生成
 * 6. 多边形的边框
 * 7. 只有一个图层，有多个vtkRenderer
 * 8. 多个vtkRenderer，多个图层，让后面的vtkRenderer始终在之前的上面
 * 9. 自带的光照
 * 10.拾取vtkProp vtkPropPicker
 * 11.顺序无关透明，深度剥离
 * 12.打印shader code
 * 13.同一个面的正面和背面设置不同的属性
 * 14.隐藏线消除 hidden line removal(HLR)
 * 15.vtkChartXY 源码分析
 * 16.基于视锥体覆盖的剔除（可见性），
 *
 * 201. vtkAlgorithm vtkExecutive vtkInformation 管道端口的作用  UpdatePiece
 */

#define TEST16

#ifdef TEST1

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <iostream>

/*
 vtkRenderWindow::Render()->vtkRenderer::Render()->vtkActor::Render()->vtkMapper::Render()

 vtkOpenGLRenderer.cxx                  [298]   UpdateGeometry()
 vtkOpenGLPolyDataMapper.cxx            [3845]  BuildBufferObjects() 顶点着色器的输入 vertexMC normalMC scalarColor tcoord tangentMC
 vtkDataSetMapper.cxx                   [120]   Render() vtkUnstructuredGrid 等类型的网格会先转换为 vtkPolyData 然后渲染

创建VBO IBO，加载顶点、拓扑数据
 vtkOpenGLVertexBufferObjectGroup.cxx   [322]   BuildAllVBOs()
 vtkOpenGLBufferObject.cxx              [137]   UploadInternal()  glBufferData

根据vtkPolyData的Cell类型创建IBO
 vtkOpenGLPolyDataMapper.cxx            [4002]  BuildIBO()
 vtkOpenGLIndexBufferObject.cxx                 CreateTriangleIndexBuffer()
                                                CreatePointIndexBuffer()
                                                CreateTriangleLineIndexBuffer
                                                CreateLineIndexBuffer CreateStripIndexBuffer
                                                CreateVertexIndexBuffer
                                                CreateEdgeFlagIndexBuffer
 vtkOpenGLBufferObject.cxx              [137]   UploadInternal()  glBufferData

调用DrawCall
 vtkOpenGLPolyDataMapper.cxx            [3444]  RenderPieceDraw()   glDrawRangeElements() 绘制点、线、三角面

离屏渲染
 vtkRenderWindow.cxx                    [547]   CopyResultFrame()
 vtkOpenGLRenderWindow.cxx              [1110]  Frame() 将渲染的结果（FBO）提交给窗口FBO对应的着色器
 vtkOpenGLQuadHelper.cxx                [109]   glDrawArrays() 将渲染的结果（铺满屏幕的quad）绘制到屏幕上

设置状态，点的大小，线的粗细，开启关闭深度测试、开启关闭混合等
 vtkOpenGLState.cxx

shader文件
 .../Rendering/OpenGL2/glsl
*/

/*
    vtkOpenGLRenderer.cxx [295]

    this->UpdateCamera();           glViewport glClear
    this->UpdateLightGeometry();    LightFollowCamera
    this->UpdateLights();
    this->UpdateGeometry();         vbo ibo => drawCall
*/
int main()
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(10., 0., 0.);
    points->InsertNextPoint(5., 10., 0.);

    cells->InsertNextCell({ 0, 1, 2 });

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // interactor syle
    vtkNew<vtkInteractorStyleRubberBand3D> style;
    iren->SetInteractorStyle(style);

    // render
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkOpenGLFramebufferObject.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkPixelBufferObject.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShaderProperty.h>

#include <iostream>

namespace {

class MyStyle : public vtkInteractorStyleRubberBand3D
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleRubberBand3D);

    void OnLeftButtonUp() override
    {
        Superclass::OnLeftButtonUp();
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

/*
 * 生成shader vtkOpenGLPolyDataMapper.cxx [350] vtkOpenGLPolyDataMapper::BuildShaders
 * 根据vtkActor的属性生成指定的shader vtkOpenGLPolyDataMapper.cxx [2461] vtkOpenGLPolyDataMapper::ReplaceShaderValues
 * 编译shader vtkShader.cxx [43] vtkShader::Compile()
 * 设置uniform vtkShaderProgram.cxx
 * 
 * vtkRenderWindow 有一个成员 State(vtkOpenGLState*)，它又有一个成员 ShaderCache(vtkOpenGLShaderCache*)
 * ShaderCache 通过计算着色器代码的MD5值，会将该着色器代码对应的 MD5 和 ShaderProgram 保存到一个 std::map 中
 * 当渲染时会先去查找当前 Mapper 的着色器代码对应的 ShaderProgram 有没有在 std::map 中保存
 * 如果有则直接使用它，如果没有才会创建一个新的 ShaderProgram
 * 因此当一个窗口绘制多个图元，并且这些图元使用的着色器源码相同时，只会创建一个 ShaderProgram ，而不是所有的 Actor 都有一个 ShaderProgram
 * 在 vtkOpenGLShaderCache::GetShaderProgram 这个函数中判断是否需要创建一个新的 ShaderProgram
 * 
 * /

int main()
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(10., 0., 0.);
    points->InsertNextPoint(5., 10., 0.);

    cells->InsertNextCell({ 0, 1, 2 });

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0., 1., 0.);

    // 将片段着色器的输出设置为指定值，只是在光照后边将默认的输出覆盖掉，具体怎么设置输出还不太清楚
    // 将shader模板文件的"//VTK::Light::Impl"替换为 "//VTK::Light::Impl\nfragOutput0 = vec4(1.0, 0.0, 0.0, 1.0);\n"
    auto sp = actor->GetShaderProperty();
    sp->AddFragmentShaderReplacement("//VTK::Light::Impl", true,
        "//VTK::Light::Impl\n"
        "  fragOutput0 = vec4(1.0, 0.0, 0.0, 1.0);\n",
        false);

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->LightFollowCameraOff();
    renderer->RemoveAllLights();
    renderer->AddActor(actor);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // interactor syle
    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    // render
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// 通过生成一个纹理，将这个纹理绘制到最下面就是背景
// [473] int vtkRenderer::UpdateCamera()
// [634] void vtkOpenGLRenderer::Clear()
// [51]  int vtkTexturedActor2D::RenderOverlay(vtkViewport* viewport)
// [575] void vtkOpenGLPolyDataMapper2D::RenderOverlay(vtkViewport* viewport, vtkActor2D* actor)

int main()
{
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> window;
    vtkNew<vtkRenderWindowInteractor> interactor;

    renderer->GradientBackgroundOn();
    renderer->SetBackground(0.1, 0.1, 0.1);
    renderer->SetBackground2(0.9, 0.9, 0.9);

    window->AddRenderer(renderer);
    window->SetSize(800, 600);

    interactor->SetRenderWindow(window);

    window->Render();
    interactor->Start();

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonUp() override
    {
        return Superclass::OnLeftButtonUp();
    }

    void SetMapper(const vtkSmartPointer<vtkPolyDataMapper>& m)
    {
        m_mapper = m;
    }

private:
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;
};

vtkStandardNewMacro(MyStyle);
} // namespace

// 片段着色器使用 gl_PrimitiveID 和 texelFetch 从samplerBuffer查找每一个单元的颜色
// gl_PrimitiveID是当前片段的单元ID
// samplerBuffer 是绑定到纹理的 TBO GL_TEXTURE_BUFFER
// texelFetch 可以通过一个索引返回第N个像素的值

int main(int, char*[])
{
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;
    vtkNew<vtkFloatArray> scalars;

    for (size_t i = 0; i < 10; ++i)
    {
        points->InsertNextPoint(static_cast<double>(i), 0.0, 0.0);
        points->InsertNextPoint(static_cast<double>(i), 1.0, 0.0);
    }

    for (vtkIdType i = 0; i < 17; i += 2)
    {
        cells->InsertNextCell({ i + 0, i + 1, i + 3, i + 2 });
    }

    for (size_t i = 0; i < 9; ++i)
    {
        // 9个单元，每个单元设置一个scalar
        scalars->InsertNextValue(static_cast<float>(i));
    }

    // 多边形，格点数据
    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetCellData()->SetScalars(scalars);

    // 创建颜色查找表
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetNumberOfColors(20);
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->SetRange(scalars->GetRange());
    hueLut->Build();

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->SetLookupTable(hueLut);
    mapper->UseLookupTableScalarRangeOn();
    mapper->ScalarVisibilityOn();
    mapper->InterpolateScalarsBeforeMappingOn();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    // actor->GetProperty()->EdgeVisibilityOn();
    // actor->GetProperty()->SetEdgeColor(1, 1, 1);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(window);

    vtkNew<MyStyle> style;
    style->SetMapper(mapper);
    renderWindowInteractor->SetInteractorStyle(style);

    window->SetSize(800, 600);
    window->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonUp() override
    {
        if (auto coords = m_mapper->GetColorCoordinates())
        {
            auto numCom = coords->GetNumberOfComponents();
            auto numVal = coords->GetNumberOfValues();
            auto numTup = coords->GetNumberOfTuples();
            std::cout << "components: " << numCom << "\tvalues: " << numVal << "\ttuples: " << numTup << '\n';

            // 打印生成的纹理坐标
            // 纹理坐标的u不是从0开始，也不是到1结束，因为纹理的最开始和最末尾各有一个边界颜色
            for (vtkIdType i = 0; i < numTup; ++i)
            {
                // 40个顶点，40个纹理坐标(uv)
                auto val = coords->GetTuple2(i);
                std::cout << "point: " << i << "\t: " << val[0] << '\t' << val[1] << '\n';
            }
        }

        std::cout << "--------------------------------------\n";

        if (auto imageData = m_mapper->GetColorTextureMap())
        {
            // 获取图像的维度，x的维度等于颜色查找表的颜色个数加2，多余的两个是边界的颜色
            int* dimensions = imageData->GetDimensions();

            // 获取像素数据的指针
            unsigned char* scalarPointer = static_cast<unsigned char*>(imageData->GetScalarPointer());

            // 获取像素数据的组件数（通道数）
            int numComponents = imageData->GetNumberOfScalarComponents();

            std::cout << "dimensions: " << dimensions[0] << ", " << dimensions[1] << ", " << dimensions[2] << "\tnumComponents: " << numComponents
                      << '\n';

            // 遍历每个像素
            for (int z = 0; z < dimensions[2]; z++)
            {
                for (int y = 0; y < 1 /*dimensions[1]*/; y++)
                {
                    for (int x = 0; x < dimensions[0]; x++)
                    {
                        // 计算像素在指针中的偏移量
                        int offset = (x + y * dimensions[0] + z * dimensions[0] * dimensions[1]) * numComponents;

                        // 通过指针获取每个通道的值
                        unsigned char redValue   = scalarPointer[offset + 0];
                        unsigned char greenValue = scalarPointer[offset + 1];
                        unsigned char blueValue  = scalarPointer[offset + 2];
                        unsigned char alphaValue = scalarPointer[offset + 3];

                        std::cout << "Pixel at (" << x << ", " << y << ", " << z << "):\t" << "R: " << static_cast<int>(redValue) << ",\t"
                                  << "G: " << static_cast<int>(greenValue) << ",\t" << "B: " << static_cast<int>(blueValue) << ",\t"
                                  << "A: " << static_cast<int>(alphaValue) << '\n';
                    }
                }
            }
        }

        return Superclass::OnLeftButtonUp();
    }

    void SetMapper(const vtkSmartPointer<vtkPolyDataMapper>& m)
    {
        m_mapper = m;
    }

private:
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;
};

vtkStandardNewMacro(MyStyle);
} // namespace

// 将Scalars转换为纹理坐标UV
// [3860] vtkUnsignedCharArray* vtkMapper::MapScalars(double alpha)
// [830]  void vtkMapper::MapScalarsToTexture(vtkAbstractArray* scalars, double alpha)

// 创建纹理
// [3360] void vtkOpenGLPolyDataMapper::RenderPieceStart(vtkRenderer* ren, vtkActor* actor)
// [129]  void vtkOpenGLTexture::Load(vtkRenderer* ren)
// [1451] bool vtkTextureObject::Create2DFromRaw

// 设置纹理环绕方式为ClampToEdge或ClampToBorder(根据OpenGL版本设置)
// [3868] void vtkOpenGLPolyDataMapper::BuildBufferObjects

/*
 * 1.每个顶点设置标量值且关闭（Mapper默认关闭） InterpolateScalarsBeforeMapping
 * vtkUnsignedCharArray* vtkMapper::MapScalars      将每个顶点的标量值映射为 uchar 类型的颜色值
 * void vtkOpenGLPolyDataMapper::BuildBufferObjects 顶点的颜色值会被当作顶点属性 scalarColor 输入到顶点着色器
 *
 * 2.每个顶点设置标量值且开启 InterpolateScalarsBeforeMapping
 * 顶点着色器输入 tcoord 通过在片段着色器中使用纹理映射图元颜色
 *
 * 当 Mapper 修改时，例如修改当前激活的颜色映射Array，会调用以下函数判断是否需要重新生成VBO
 * bool vtkOpenGLPolyDataMapper::GetNeedToRebuildBufferObjects
 * 再调用 BuildBufferObjects 重新生成VBO 最后通过glBufferData提交数据
 */

int main(int, char*[])
{
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;
    vtkNew<vtkFloatArray> scalars;

    for (size_t i = 0; i < 10; ++i)
    {
        points->InsertNextPoint(static_cast<double>(i), 0.0, 0.0);
        points->InsertNextPoint(static_cast<double>(i), 1.0, 0.0);

        scalars->InsertNextValue(static_cast<float>(i));
        scalars->InsertNextValue(static_cast<float>(i));
    }

    for (vtkIdType i = 0; i < 17; i += 2)
    {
        cells->InsertNextCell({ i + 0, i + 1, i + 3, i + 2 });
    }

    // 多边形，格点数据
    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetPointData()->SetScalars(scalars);

    // 创建颜色查找表
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetNumberOfColors(20);
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->SetRange(scalars->GetRange()[0], scalars->GetRange()[1] / 2.0);
    hueLut->UseBelowRangeColorOn();
    hueLut->SetBelowRangeColor(0.5, 0.5, 0.5, 0.5);
    hueLut->UseAboveRangeColorOn();
    hueLut->SetAboveRangeColor(0.5, 0.5, 0.5, 0.5);
    hueLut->Build();

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->SetLookupTable(hueLut);
    mapper->UseLookupTableScalarRangeOn();
    mapper->ScalarVisibilityOn();
    mapper->InterpolateScalarsBeforeMappingOn();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1, 1, 1);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(window);

    vtkNew<MyStyle> style;
    style->SetMapper(mapper);
    renderWindowInteractor->SetInteractorStyle(style);

    window->SetSize(800, 600);
    window->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonUp() override
    {
        return Superclass::OnLeftButtonUp();
    }

    void SetMapper(const vtkSmartPointer<vtkPolyDataMapper>& m)
    {
        m_mapper = m;
    }

private:
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;
};

vtkStandardNewMacro(MyStyle);
} // namespace

// 实体的边框
// 几何着色器中生成一个【三角形的边缘方程】
// 然后通过一个纹理获取三角形的三个边是不是多边形的边框（用二进制0B000表示三条边）
// 片段着色器判断当前片段是边框还是实体，然后设置不同的颜色

int main(int, char*[])
{
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(2.0, 0.0, 0.0);
    points->InsertNextPoint(2.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    cells->InsertNextCell({ 0, 1, 4, 5 });
    cells->InsertNextCell({ 1, 2, 3, 4 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->LightingOff();
    actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(0.0, 1.0, 0.0);
    actor->GetProperty()->SetLineWidth(5);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(window);

    vtkNew<MyStyle> style;
    style->SetMapper(mapper);
    renderWindowInteractor->SetInteractorStyle(style);

    window->SetSize(800, 600);
    window->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

// [472] int vtkRenderer::UpdateCamera()
// [47]  void vtkOpenGLCamera::Render(vtkRenderer* ren)
// glViewport glEnable(GL_SCISSOR_TEST) glScissor

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cube;
    cube->Update();
    vtkNew<vtkSphereSource> sphere;
    sphere->Update();

    vtkNew<vtkPolyDataMapper> mapperCube;
    mapperCube->SetInputData(cube->GetOutput());
    vtkNew<vtkPolyDataMapper> mapperSphere;
    mapperSphere->SetInputData(sphere->GetOutput());

    vtkNew<vtkActor> actorCube;
    vtkNew<vtkActor> actorSphere;
    actorCube->SetMapper(mapperCube);
    actorSphere->SetMapper(mapperSphere);

    vtkNew<vtkRenderer> leftRenderer;
    vtkNew<vtkRenderer> rightRenderer;

    leftRenderer->AddActor(actorCube);
    leftRenderer->SetBackground(.1, .2, .3);
    leftRenderer->SetViewport(0.05, 0.05, 0.45, 0.45);

    rightRenderer->AddActor(actorSphere);
    rightRenderer->SetBackground(.3, .2, .1);
    rightRenderer->SetViewport(0.55, 0.55, 0.95, 0.95);

    vtkNew<vtkRenderWindow> window;
    window->SetSize(800, 600);
    window->AddRenderer(leftRenderer);
    window->AddRenderer(rightRenderer);

    std::cout << "number of layers: " << window->GetNumberOfLayers() << '\n';

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(window);
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    window->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST7

#ifdef TEST8

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkCoordinate.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <array>
#include <iostream>

// 下面的vtkRenderer将GL_COLOR_BUFFER_BIT和GL_DEPTH_BUFFER_BIT都清除
// 上面的vtkRenderer只清除GL_DEPTH_BUFFER_BIT
// 前面绘制的颜色缓存一直保留，后面绘制的就会覆盖在之前的renderer上面
// 将深度缓存清除，就可以让后面绘制的renderer一直在最上面
// [24]  void vtkRendererCollection::Render()
// [634] void vtkOpenGLRenderer::Clear()

int main()
{
    vtkNew<vtkRenderer> renderer1;
    vtkNew<vtkRenderer> renderer2;

    // renderer1（底下的图层）添加一个红色的矩形
    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(-1.f, -1.f, 0.f);
        points->InsertNextPoint(1.f, -1.f, 0.f);
        points->InsertNextPoint(1.f, 1.f, 0.f);
        points->InsertNextPoint(-1.f, 1.f, 0.f);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);
        renderer1->AddActor(actor);
    }

    // renderer2（上层的图层）添加一个绿色的三角形
    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(-1.f, -1.f, 0.1f);
        points->InsertNextPoint(1.f, -1.f, 0.1f);
        points->InsertNextPoint(0.f, 1.f, 0.1f);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0, 1, 0);
        renderer2->AddActor(actor);
    }

    renderer1->SetBackground(.5, .5, .0);
    renderer1->ResetCamera();
    renderer1->SetLayer(0);

    renderer2->SetBackground(0., .5, .5);
    renderer2->SetActiveCamera(renderer1->GetActiveCamera()); // renderer2和renderer1使用同一个camera
    renderer2->SetLayer(1);                                   // renderer2始终在上面

    // 开启关闭交互
    renderer1->InteractiveOn();
    renderer2->InteractiveOff();

    std::cout << std::boolalpha;
    std::cout << "below renderer: " << (bool)renderer1->GetInteractive() << '\n';
    std::cout << "above renderer: " << (bool)renderer2->GetInteractive() << '\n';

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(600, 600);
    renderWindow->SetNumberOfLayers(2);

    // 如果当前有多个renderer都开起了交互，则鼠标交互响应的最后添加的renderer
    // 如果当前只有一个renderer开启了交互，则鼠标交互响应的是开启交互的renderer
    renderWindow->AddRenderer(renderer2);
    renderWindow->AddRenderer(renderer1);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST8

#ifdef TEST9

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShaderProgram.h>
#include <vtkSphereSource.h>

namespace {

class MyMapper : public vtkOpenGLPolyDataMapper
{
public:
    static MyMapper* New();
    vtkTypeMacro(MyMapper, vtkOpenGLPolyDataMapper);

    void PrintShaderSource()
    {
        for (size_t i = 0; i < PrimitiveEnd; ++i)
        {
            if (auto program = this->Primitives[i].Program)
            {
                auto vs = program->GetVertexShader()->GetSource();
                std::cout << vs << std::endl;
                auto fs = program->GetFragmentShader()->GetSource();
                std::cout << fs << std::endl;
            }
        }
    }
};

vtkStandardNewMacro(MyMapper);

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void SetActor(vtkActor* a)
    {
        m_actor = a;
    }

protected:
    void OnLeftButtonUp() override
    {
        MyMapper::SafeDownCast(m_actor->GetMapper())->PrintShaderSource();
        Superclass::OnLeftButtonUp();
    }

private:
    vtkActor* m_actor { nullptr };
};

vtkStandardNewMacro(MyStyle);
} // namespace

/**
 *  默认光照是一个平行于 相机方向 的平行光
 *
 *  模型带有法线数据：normalVCVSOutput
 *  vec3 normalVCVSOutput = normalize(normalVCVSOutput);                    标准化法线
 *  if (gl_FrontFacing == false) { normalVCVSOutput = -normalVCVSOutput; }  如果是背面则对法线反向
 *  float df = max(0.0,normalVCVSOutput.z);                                 漫反射系数
 *  float sf = pow(df, specularPower);                                      镜面反射系数
 *
 *  模型没有法线数据：
 *  vec3 fdx = dFdx(vertexVC.xyz);  获取相邻片段在屏幕空间中的梯度
 *  vec3 fdy = dFdy(vertexVC.xyz);
 *  vec3 normalVCVSOutput = normalize(cross(fdx,fdy));  使用叉乘求出法线
 */

int main()
{
    vtkNew<vtkSphereSource> source;
    // source->GenerateNormalsOff(); // 不生成法线
    source->Update();

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(2, 0, 0);
    points->InsertNextPoint(1, 2, 0);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1, 2 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    vtkNew<MyMapper> mapper;
    mapper->SetInputData(source->GetOutput());
    // mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    // actor->GetProperty()->LightingOff();

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    style->SetActor(actor);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST9

#ifdef TEST10

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <map>
#include <string>

namespace {
std::map<vtkSmartPointer<vtkActor>, std::string> theActors;

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override
    {
        int* clickPos = this->GetInteractor()->GetEventPosition();

        vtkNew<vtkPropPicker> picker;
        picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

        if (auto actor = picker->GetActor())
        {
            std::cout << "picked actor:\t" << theActors[actor] << '\n';
        }

        // 世界坐标
        // 通过 vtkWorldPointPicker 获取世界坐标，实际就是对相机属性的一些转换
        if (auto pos = picker->GetPickPosition())
        {
            std::cout << "pick position:\t" << pos[0] << '\t' << pos[1] << '\t' << pos[2] << '\n';
        }

        // 屏幕坐标
        if (auto point = picker->GetSelectionPoint())
        {
            std::cout << "selection point:\t" << point[0] << '\t' << point[1] << '\t' << point[2] << '\n';
        }

        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

// [1859] vtkAssemblyPath* vtkRenderer::PickProp
// [263]  bool vtkHardwareSelector::CaptureBuffers()
// [260]  void vtkRenderWindow::Render() 将所有的图元在一个新的FBO上绘制一遍，背景色设置为0，一些拾取用不到的属性都跳过
// 片段着色器 fragOutput0 = vec4(mapperIndex,1.0); // 将图元的颜色设置为 ID
// [385]  void vtkHardwareSelector::SavePixelBuffer(int passNo) 将拾取的颜色保存起来
// [887]  vtkOpenGLRenderWindow.cxx => glReadPixels
// [666]  vtkHardwareSelector::GetPixelInformation 将拾取到的颜色转换为vtkProp以及ID
// [416]  void vtkSelection::SetNode(const std::string& name, vtkSelectionNode* node) // 将拾取到的vtkProp保存起来

// 如果拾取的是顶点、单元等其他类型，会使用着色器的gl_VertexID和gl_PrimitiveID变量

int main(int argc, char* argv[])
{
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<MyStyle> style;
    style->SetDefaultRenderer(renderer);
    renderWindowInteractor->SetInteractorStyle(style);

    size_t nRow = 3;
    size_t nCol = 3;

    for (size_t row = 0; row < nRow; ++row)
    {
        for (size_t col = 0; col < nCol; ++col)
        {
            vtkNew<vtkSphereSource> source;
            source->SetCenter((double)row, (double)col, 0.0);
            source->Update();

            vtkNew<vtkPolyDataMapper> mapper;
            mapper->SetInputData(source->GetOutput());

            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper);
            actor->GetProperty()->SetColor(1, 1, 1);

            renderer->AddActor(actor);

            theActors.try_emplace(actor, '(' + std::to_string(row) + ", " + std::to_string(col) + ')');
        }
    }

    renderer->SetBackground(.1, .2, .3);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST10

#ifdef TEST11

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShaderProgram.h>

namespace {

class MyMapper : public vtkOpenGLPolyDataMapper
{
public:
    static MyMapper* New();
    vtkTypeMacro(MyMapper, vtkOpenGLPolyDataMapper);

    void PrintShaderSource()
    {
        for (size_t i = 0; i < PrimitiveEnd; ++i)
        {
            if (auto program = this->Primitives[i].Program)
            {
                auto vs = program->GetVertexShader()->GetSource();
                std::cout << vs << std::endl;
                auto fs = program->GetFragmentShader()->GetSource();
                std::cout << fs << std::endl;
            }
        }
    }
};

vtkStandardNewMacro(MyMapper);

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void SetActor(vtkActor* a)
    {
        m_actor = a;
    }

protected:
    void OnLeftButtonUp() override
    {
        MyMapper::SafeDownCast(m_actor->GetMapper())->PrintShaderSource();
        Superclass::OnLeftButtonUp();
    }

private:
    vtkActor* m_actor { nullptr };
};

vtkStandardNewMacro(MyStyle);
} // namespace

namespace {
vtkActor* AddActor(vtkRenderer* renderer)
{
    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(-0.5, 0.5, 0.0);
        points->InsertNextPoint(-0.5, -0.5, 0.0);
        points->InsertNextPoint(0.5, -0.5, 0.0);
        points->InsertNextPoint(0.5, 0.5, 0.0);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<MyMapper> mapper;
        mapper->SetInputData(polyData);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1., 0., 0.);
        actor->GetProperty()->SetOpacity(.5);
        actor->GetProperty()->LightingOff();

        renderer->AddActor(actor);
    }

    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0.0, 0.5, -0.5);
        points->InsertNextPoint(0.0, -0.5, -0.5);
        points->InsertNextPoint(0.0, -0.5, 0.5);
        points->InsertNextPoint(0.0, 0.5, 0.5);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<MyMapper> mapper;
        mapper->SetInputData(polyData);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0., 1., 0.);
        actor->GetProperty()->SetOpacity(.5);
        actor->GetProperty()->LightingOff();

        renderer->AddActor(actor);

        return actor;
    }
}
} // namespace

/**
 * 默认使用的是alpha blending(alpha 混合)
 * 调用glDepthMask(GL_TRUE)先渲染不透明的vtkActor，再使用顺序无关透明渲染透明的vtkActor
 * uniform opacityUniform
 * gl_FragDepth
 *
 * 使用深度剥离 https://vtk.org/Wiki/VTK/Depth_Peeling
 * CPU深度排序 vtkDepthSortPolyData https://examples.vtk.org/site/Cxx/Visualization/CorrectlyRenderTranslucentGeometry/
 */

int main()
{
    vtkNew<vtkRenderer> renderer;
    renderer->UseDepthPeelingOn();
    renderer->SetMaximumNumberOfPeels(100);
    renderer->SetOcclusionRatio(0.1);
    auto actor = AddActor(renderer);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AlphaBitPlanesOn();
    renWin->SetMultiSamples(0);
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    style->SetActor(actor);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST11

#ifdef TEST12

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShaderProgram.h>

namespace {

class MyMapper : public vtkOpenGLPolyDataMapper
{
public:
    static MyMapper* New();
    vtkTypeMacro(MyMapper, vtkOpenGLPolyDataMapper);

    void PrintShaderSource()
    {
        for (size_t i = 0; i < PrimitiveEnd; ++i)
        {
            if (auto program = this->Primitives[i].Program)
            {
                auto vs = program->GetVertexShader()->GetSource();
                std::cout << vs << std::endl;
                auto fs = program->GetFragmentShader()->GetSource();
                std::cout << fs << std::endl;
            }
        }
    }
};

vtkStandardNewMacro(MyMapper);

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void SetActor(vtkActor* a)
    {
        m_actor = a;
    }

protected:
    void OnLeftButtonUp() override
    {
        MyMapper::SafeDownCast(m_actor->GetMapper())->PrintShaderSource();
        Superclass::OnLeftButtonUp();
    }

private:
    vtkActor* m_actor { nullptr };
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main()
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(-0.5, 0.5, 0.0);
    points->InsertNextPoint(-0.5, -0.5, 0.0);
    points->InsertNextPoint(0.5, -0.5, 0.0);
    points->InsertNextPoint(0.5, 0.5, 0.0);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    vtkNew<MyMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    style->SetActor(actor);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST12

#ifdef TEST13

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOpenGLActor.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkOpenGLShaderProperty.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShaderProgram.h>
#include <vtkShaderProperty.h>

namespace {

class MyMapper : public vtkOpenGLPolyDataMapper
{
public:
    static MyMapper* New();
    vtkTypeMacro(MyMapper, vtkOpenGLPolyDataMapper);

    void PrintShaderSource()
    {
        for (size_t i = 0; i < PrimitiveEnd; ++i)
        {
            if (auto program = this->Primitives[i].Program)
            {
                auto vs = program->GetVertexShader()->GetSource();
                std::cout << vs << std::endl;
                auto fs = program->GetFragmentShader()->GetSource();
                std::cout << fs << std::endl;
            }
        }
    }
};

vtkStandardNewMacro(MyMapper);

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void SetActor(vtkActor* a)
    {
        m_actor = a;
    }

protected:
    void OnLeftButtonUp() override
    {
        MyMapper::SafeDownCast(m_actor->GetMapper())->PrintShaderSource();
        Superclass::OnLeftButtonUp();
    }

private:
    vtkActor* m_actor { nullptr };
};

vtkStandardNewMacro(MyStyle);
} // namespace

/**
 * 片段着色器中通过判断当前片段是正面还是背面，使用不同的着色方式
 * gl_FrontFacing == false 背面
 * gl_FrontFacing == true  正面
 */

int main()
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(-0.5, 0.5, 0.0);
    points->InsertNextPoint(-0.5, -0.5, 0.0);
    points->InsertNextPoint(0.5, -0.5, 0.0);
    points->InsertNextPoint(0.5, 0.5, 0.0);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    vtkNew<MyMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkProperty> backfaceProperty;
    backfaceProperty->SetColor(0, 1, 0);
    backfaceProperty->LightingOff();

    vtkNew<vtkProperty> frontfaceProperty;
    frontfaceProperty->SetColor(1, 0, 0);
    frontfaceProperty->LightingOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetProperty(frontfaceProperty);
    actor->SetBackfaceProperty(backfaceProperty);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    style->SetActor(actor);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST13

#ifdef TEST14

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

/// @brief 创建由12条线构成的立方体
/// @return
vtkSmartPointer<vtkPolyData> CreateWireframe()
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 1);
    points->InsertNextPoint(1, 0, 1);
    points->InsertNextPoint(1, 1, 1);
    points->InsertNextPoint(0, 1, 1);
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 0, 0);
    points->InsertNextPoint(1, 1, 0);
    points->InsertNextPoint(0, 1, 0);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1 });
    cells->InsertNextCell({ 1, 2 });
    cells->InsertNextCell({ 2, 3 });
    cells->InsertNextCell({ 3, 0 });
    cells->InsertNextCell({ 4, 5 });
    cells->InsertNextCell({ 5, 6 });
    cells->InsertNextCell({ 6, 7 });
    cells->InsertNextCell({ 7, 4 });
    cells->InsertNextCell({ 2, 6 });
    cells->InsertNextCell({ 3, 7 });
    cells->InsertNextCell({ 4, 0 });
    cells->InsertNextCell({ 1, 5 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    return polyData;
}

/// @brief 创建由6个面构成的立方体
/// @return
vtkSmartPointer<vtkPolyData> CreateSurface()
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    return source->GetOutput();
}

// 判断当前是否开启UseHiddenLineRemoval以及是否存在线框图元
// 创建vtkHiddenLineRemovalPass，先绘制不是线框的图元，然后设置偏移（为了保证线框清晰）
// 再以多边形形式绘制线框图元（使用glColorMask关闭颜色写入），最后再以线框形式绘制线框图元，并恢复之前的偏移值
// vtkOpenGLRenderer.cxx [480] void vtkOpenGLRenderer::DeviceRenderOpaqueGeometry(vtkFrameBufferObjectBase* fbo)
// vtkHiddenLineRemovalPass [51] void vtkHiddenLineRemovalPass::Render(const vtkRenderState* s)
// 注意：只有原本是面数据以线框渲染才可以隐藏线消除

int main(int, char*[])
{
    vtkNew<vtkPolyDataMapper> mapper;
    // mapper->SetInputData(CreateWireframe());
    mapper->SetInputData(CreateSurface());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetRepresentationToWireframe(); // 以线框形式渲染
    actor->GetProperty()->SetColor(0., 1., 0.);
    actor->GetProperty()->LightingOff();

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->UseHiddenLineRemovalOn(); // 消除隐藏线
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST14

#ifdef TEST15

#include <vtkChartXY.h>
#include <vtkContextActor.h>
#include <vtkContextInteractorStyle.h>
#include <vtkContextScene.h>
#include <vtkFloatArray.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTable.h>

// Window => Renderer => Actor
// vtkContextActor.cxx  [238]   this->Scene->Paint(this->Context)   参数是类型为 vtkContext2D 的成员变量
// vtkContextScene.cxx  [114]   this->Children->PaintItems(painter)
// vtkChartXY.cxx       [394]   Paint(vtkContext2D)

// vtkChartXY.cxx       [430]   this->Update()
// vtkChartXY.cxx       [430]   this->UpdateLayout()
// vtkChartXY.cxx       [475]   this->PaintChildren(vtkContext2D)

// vtkContextClip.cxx   [52]    this->PaintChildren(vtkContext2D)
// vtkPlotGrid.cxx      [48]    Paint(vtkContext2D)  坐标轴内部的网格
// vtkPlotLine.cxx      [43]    Paint(vtkContext2D)  实际的折线
// vtkAxis.cxx          [299]   Paint(vtkContext2D)  坐标轴（XY轴，坐标轴标题、刻度等）

// vtkContext2D 有一个类型为 vtkContextDevice2D 的成员变量 Device
// OpenGL函数都在 vtkOpenGLContextDevice2D 中调用
// vtkOpenGLContextDevice2D 继承自 vtkContextDevice2D

int main(int, char*[])
{
    vtkNew<vtkFloatArray> arrX;
    arrX->SetName("X Axis");

    vtkNew<vtkFloatArray> arrY;
    arrY->SetName("Y Axis");

    vtkNew<vtkTable> table;
    table->AddColumn(arrX);
    table->AddColumn(arrY);
    table->SetNumberOfRows(3);   // 三行两列数据
    table->SetValue(0, 0, 0.f);  // X0 三个参数分别表示：行、列、值
    table->SetValue(0, 1, 0.f);  // Y0
    table->SetValue(1, 0, 2.f);  // X1
    table->SetValue(1, 1, 4.f);  // Y1
    table->SetValue(2, 0, 4.f);  // X2
    table->SetValue(2, 1, 16.f); // Y2

    vtkNew<vtkChartXY> chart;
    vtkPlot* line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 1); // 第二个参数表示图标的横坐标使用table的第几列数据，第三个参数表示纵轴使用第几列数据
    line->SetColor(0, 255, 0, 255);
    line->SetWidth(2.f);
    line->Update();
    chart->Update();

    vtkNew<vtkContextScene> scene;
    scene->AddItem(chart);

    vtkNew<vtkContextActor> actor;
    actor->SetScene(scene);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->SetBackground(.4, .5, .6);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkContextInteractorStyle> style;
    style->SetScene(scene);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST15

#ifdef TEST16

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCullerCollection.h>
#include <vtkFrustumCoverageCuller.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

void AddActors(vtkRenderer* ren)
{
    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0, 0, 8);
        points->InsertNextPoint(1, 0, 8);
        points->InsertNextPoint(1, 1, 8);
        points->InsertNextPoint(0, 1, 8);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0, 1, 0);
        ren->AddActor(actor);
    }

    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0, 0, 0);
        points->InsertNextPoint(1, 0, 0);
        points->InsertNextPoint(1, 1, 0);
        points->InsertNextPoint(0, 1, 0);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);
        ren->AddActor(actor);
    }
}

/**
 * vtkFrustumCoverageCuller 根据视锥体的覆盖范围来剔除 Actor
 * 当 Actor 的覆盖范围小于指定的值时，就会从需要渲染的 Actors 中剔除，即跳过该 Actor 的渲染
 * vtkRenderer 中添加的 Actors 和实际真正需要渲染的 Actors 是两个列表
 * 真正需要渲染的 Actor 列表是经过 Culler 剔除之后的 Actors
 * 在 void vtkRenderer::AllocateTime() 函数中执行剔除操作
 */

int main()
{
    vtkNew<vtkRenderer> renderer;
    AddActors(renderer);
    renderer->ResetCamera();
    renderer->SetBackground(.1, .2, .3);

    auto pCullers = renderer->GetCullers();
    auto nCullers = pCullers->GetNumberOfItems(); // 默认有一个剔除器

    // 对需要渲染的图元进行排序，根据 Actors 到视锥体近平面的距离
    vtkNew<vtkFrustumCoverageCuller> culler;
    culler->SetSortingStyle(VTK_CULLER_SORT_FRONT_TO_BACK); // 设置排序方式，默认不排序

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST16

#ifdef TEST201

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// vtkAlgorithm 有一个函数 ProcessRequest()，通过它来执行实际的算法
// 常用的算法都会执行一个函数 RequestData()，在它内部生成实际的数据

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->SetXLength(1.);
    source->SetYLength(2.);
    source->SetZLength(3.);
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(source->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST201
