/*
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

// import the VTK module
import VTK 9.1

// window containing the application
ApplicationWindow {
  // title of the application
  visible:true
  title: qsTr("VTK QtQuick App")
  width: 800
  height: 800
  color: palette.window

  SystemPalette {
    id: palette
    colorGroup: SystemPalette.Active
  }

  // Instantiate the vtk render window
  VTKRenderWindow {
    id: vtkwindow
    width: 400
    height: 400
  }

  // add one or more vtk render items
  VTKRenderItem {
    objectName: "ConeView"
    x: 200
    y: 200
    width: 200
    height: 200
    // Provide the handle to the render window
    renderWindow: vtkwindow
  }
  VTKRenderItem {
    objectName: "VolumeView"
    x: 0
    y: 0
    width: 200
    height: 200
    // Provide the handle to the render window
    renderWindow: vtkwindow
  }
}
*/

// import related modules
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

// import the VTK module
import VTK 9.1

// window containing the application
ApplicationWindow {
  // title of the application
  title: qsTr("VTK QtQuick App")
  width: 500
  height: 500
  color: palette.window

  SystemPalette {
    id: palette
    colorGroup: SystemPalette.Active
  }

  // Instantiate the vtk render window
  VTKRenderWindow {
    id: vtkwindow
    width: 400
    height: 400
  }

  // add one or more vtk render items
  VTKRenderItem {
    objectName: "ConeView"
    x: 100
    y: 100
    width: 300
    height: 300
    // Provide the handle to the render window
    renderWindow: vtkwindow
  }
}