cmake_minimum_required (VERSION 3.8)

add_subdirectory ("01vtkRenderWindow")
#add_subdirectory ("02vtkQWidget")
#add_subdirectory ("03vtkQQuick")
#add_subdirectory ("04vtkRealTimeCloudRender")
add_subdirectory("05_analyzeSource")

install(DIRECTORY resource DESTINATION .)
