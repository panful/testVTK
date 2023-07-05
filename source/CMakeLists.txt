
add_subdirectory ("01vtkRenderWindow")

#find_package(Qt6 QUIET)
#if(Qt6_FOUND)
#    message("---------------------------- Qt6 found ----------------------------")
#    add_subdirectory ("02vtkQWidget")
#    add_subdirectory ("03vtkQQuick")
#else()
#    message("---------------------------- Qt6 not found ----------------------------")
#endif()

#add_subdirectory("04vtkRealTimeCloudRender")
add_subdirectory("05analyzeSource")
