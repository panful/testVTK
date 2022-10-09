# 一、软件版本
|软件|版本|
|--|--|
|Windows|11|
|VTK|9.1.0|
|Qt|6.2.2|
|VS|2022|
|FFMPEG||

# 二、环境变量
|环境变量|值|
|--|--|
|QTDIR|\Qt\6.2.2\msvc2019_64\bin|
|VTK_DIR|\VTK\lib\cmake\vtk-9.1|
|FFMPEG||

# 三、注意事项
QML:VTK使用camke生成时和qt相关的项目设置为WANT，将...\VTK\lib\qml\Debug\VTK.9.1文件夹下的文件拷贝到...\Qt\6.2.2\msvc2019_64\qml\VTK目录下

vtk: vtk-config.cmake所在路径添加到环境变量 VTK_DIR

# 四、代码
## 01vtkRenderWindow
### 00_Test
vtk函数测试
01. 引用计数
### 01_Template
使用vtkRenderWindow渲染的最简单代码
### 02_vtkRenderWindow
01. 鼠标左键弹起后修改actor颜色
02. 从文件读取顶点
03. （顶点 + 索引）绘制立方体（和opengl ebo方式类似），每个顶点添加一个颜色值
04. 简单的平面，使用顶点属性数据设置颜色 vtkDataArray
05. 通过定义三个指定x-y-z方向坐标的阵列来创建直线栅格。
06. 批量生成一大堆线条
07. 点构成线条，使用索引
08. 照相机 https://blog.csdn.net/liushao1031177/article/details/116903698
09. 绘制点，线，面，类似Opengl 顶点+索引的绘制方式
10. 2D网格绘制
11. 拟合样条曲线生成柱状体
12. vtkAppendPolyData合并多个polydata
13. 多窗口使用相同的数据同一个相机（即多个窗口显示完全一样的东西，交互也是同步的）
14. vtkGlyph3D + vtkProbeFilter https://kitware.github.io/vtk-examples/site/Cxx/Visualization/RandomProbe/
15. 箭头，可以控制箭头的方向
16. 云图 stl文件
17. 矢量图 vtkGlyph3D
18. 带流场参数的立方体
19. 颜色映射表示例图
20. 可以随意拖动的坐标轴
21. 官方例子，球面法向量  矢量图
22. 将点（向量）投影到平面
23. 照相机交互方式 左键画框，右键旋转，中键缩放平移
24. 云图，渐变线，点和单元设置流场参数，颜色过渡方式，颜色映射表
25. 颜色过渡，同一个窗口两个绘制对象，同一数据，不同相机 （viewPort)
26. 纹理，光照
27. 框选拾取
28. actor拾取
29. 框选拾取actor
30. 比例尺(scalarBar)
31. 比例尺(vtkLegendScaleActor)
32. colorlegend
33. vtkContourFilter  等值面
34. vtkprobefilter    探针
35. 流线图  展示流体流动的轨迹和流动方向 有点像线框式的云图
36. 探针
37. 探针 https://blog.csdn.net/liushao1031177/article/details/122860254
38. plot https://kitware.github.io/vtk-examples/site/Cxx/Plotting/LinePlot/
39. 以单元形式设置标量数据
40. 单元标量数据转顶点数据，等值线vtkContourFilter
41. 顶点标量数据转单元数据
42. 获取单元中心即格心，并用球体标注格心(矢量图）
43. reverse access ,从经过算法(vtkAlgorithm,vtkPolyDataAlgorithm)变换的数据中获取源数据（vtkPolyData,vtkDataSet）
44. vtkChart https://kitware.github.io/vtk-examples/site/Cxx/DataStructures/OctreeTimingDemo/
45. 从Actor中获取polyData，并修改polyData的属性会改变原来Actor的状态，深拷贝可以不改变源Actor状态
46. reference count
47. 保存图片 (位图，矢量图）  qt框选截图 https://blog.csdn.net/GoForwardToStep/article/details/54237689
48. 矢量图svg保存
49. vtkImageView  https://blog.csdn.net/liushao1031177/article/details/115799500
50. 保存矢量图svg文件
51. 利用vtk橡皮筋交互模式，实现鼠标绘制矩形
52. 左右两个窗口，一个拖动选择框，一个显示框选的内容（官方例子）vtkBorderWidget   坐标转换
53. vtkImageClip 裁剪图片
54. vtkImageBlend 合成图片，利用多个图片的z值，将多张图片放在一起
55. logowidget  vtkImageCanvasSource2D
56. vtkImageBlend vtkImageCanvasSource2D 将多张图片叠加在一起，可以实现截图框选中为原图颜色，没选中的加阴影
57. 图片铺满整个窗口  https://kitware.github.io/vtk-examples/site/Cxx/Images/FillWindow/
58. 窗口大小改变回调函数 resizeWindowEvent
59. 微信群里文件
60. 文字显示，汉字
61. 2D橡皮筋交互模式，重载鼠标事件
62. vtkInteractorStyleRubberBandPick 款选拾取
63. vtkImageData转QImage
64. DICOM  MPR 医学ct图四象限
65. vtkBorderWidget事件
66. shared actor 多个vtkRenderWindow共享actor
67. vtkBoxWidget
68. vtkBalloonWidget 鼠标放在图元上一会可以显示文本提示或者图像提示
### 03_vtkRenderWindow
01. 写ffmpeg
02. vtkOpenGLFramebufferObject
03. 图片满屏显示，适配窗口
04. 写ffmpeg
05. vtkFFMPEGVideoSource
06. vtk自带按钮动态设置窗口大小
07. 切片 vtkCutter
08. 给图元添加标签
09. 命令模式
10. 坐标轴，群里下载的
11. 获取OpenGL版本  https://gitlab.kitware.com/vtk/vtk/-/blob/v9.1.0/Rendering/OpenGL2/Testing/Cxx/TestWindowBlits.cxx
## 02vtkQWidget
使用QWidget渲染
### 01_Template
使用QVTKRenderWidget+vtkGenericOpenGLRenderWindow的最简单模板
### 02_Undo
鼠标事件回退（撤销）前进
### 03_MultiWindow
多窗口显示
### 04_ShareCamera
