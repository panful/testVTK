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
## (01)vtkRenderWindow
### 00_Test
vtk函数测试
01. 引用计数
### 01_Template
使用vtkRenderWindow渲染的最简单示例代码
### 02_vtkRenderWindow

### 03_vtkRenderWindow

### 04_cameraTransform
- 平移、缩放、旋转
- 广告牌
### 05_colorMap
- 颜色映射表
- 色卡
- 云图
- 矢量图（箭头图）
- 流线图
- 迹线图（粒子追踪）
### 06_parallelProcess
- 并行处理数据、并行渲染
- 多线程
- MPI
### 07_pick
- 物体拾取、图元拾取、点拾取、单元拾取
- GPU拾取、CPU拾取
- 多窗口拾取
- 多图层拾取
### 08_interactor
- 交互器Style
- 定时器
### 09_image
- 图片保存、加载、裁剪、混合
- 像素转换
- 矢量图片
### 10_grid
- 加载各种类型的网格
- 非结构、结构网格
### 11_animation
- 动画
### 12_shadow_lighting
- 光照
- 阴影
### 13_filter
- 插值
- 提取网格、裁剪网格
- 格心格点数据互相转换
- 合并网格
### 14_view
- 窗口
- 场景图
### 15_algorithm
- 图形学常见算法
- 八叉树
- KD树
- BSP树
### 16_geometry
- vtk自带的几何模型
- 球体、立方体、圆锥、圆柱、箭头
- 超椭球体

## (02)vtkQWidget
使用QWidget渲染
### 01_Template
使用QVTKRenderWidget+vtkGenericOpenGLRenderWindow的最简单模板
### 02_Undo
鼠标事件回退（撤销）前进
### 03_MultiWindow
多窗口显示
### 04_ShareCamera
## (03)vtkQQuick
使用QQuick渲染
## (04)vtkRealTimeCloudRender
实时云渲染
## (05)analyzeSource
vtk源码分析
