//qt框选截图 https://blog.csdn.net/GoForwardToStep/article/details/54237689

#ifdef TEST101

// https://kitware.github.io/vtk-examples/site/Cxx/Qt/ImageDataToQImage/

#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>

#include <QApplication>
#include <QColor>
#include <QImage>

/// @brief 创建一个vtkImageData，左边红色，右边绿色
/// @return
vtkSmartPointer<vtkImageData> createDemoImageData()
{
    vtkNew<vtkImageData> image;
    image->SetDimensions(500, 500, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    int width  = image->GetDimensions()[0];
    int height = image->GetDimensions()[1];

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
            if (x < width / 2)
            {
                pixel[0] = 255;
                pixel[1] = 0;
                pixel[2] = 0;
            }
            else
            {
                pixel[0] = 0;
                pixel[1] = 255;
                pixel[2] = 0;
            }
        }
    }

    return image;
}

/// @brief 将vtkImangeData转换为QImage
/// @param imageData
/// @return
QImage vtkImageDataToQImage(vtkSmartPointer<vtkImageData>&& imageData)
{
    if (!imageData)
    {
        return QImage();
    }

    int width  = imageData->GetDimensions()[0];
    int height = imageData->GetDimensions()[1];

    QImage image(width, height, QImage::Format_RGB32);

    auto rgbPtr    = reinterpret_cast<QRgb*>(image.bits()) + width * (height - 1);
    auto colorsPtr = reinterpret_cast<unsigned char*>(imageData->GetScalarPointer());

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // Swap the vtkImageData RGB values with an equivalent QColor
            *(rgbPtr++) = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2]).rgb();
            colorsPtr += imageData->GetNumberOfScalarComponents();
        }

        rgbPtr -= width * 2;
    }

    return image;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QImage qimage = vtkImageDataToQImage(createDemoImageData());

    // 必须在当前程序下存在"./中文"文件夹才可以保存
    if (qimage.save("./中文/qim中文age.TIFF"))
    {
        qDebug() << "Image file saved successfully";
    }
    else
    {
        qDebug() << "Failed to save Image file";
    }

    return EXIT_SUCCESS;
}

#endif // TEST201

#ifdef TEST1202

// https://tieba.baidu.com/p/4297027534?red_tag=3118028977

#include <QImage>
#include <vtkDataArray.h>
#include <vtkImageData.h>
#include <vtkJPEGReader.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>

#include <string>

QImage createQImage1(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage2(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage3(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage4(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage(vtkImageData* imageData);

int main()
{
    std::string inputFilename  = "test.jpg";
    std::string outputFilename = "save.jpg";

    vtkNew<vtkJPEGReader> reader;
    reader->SetFileName(inputFilename.c_str());
    reader->Update(); // 必须Update，不然GetOutput返回值为空

    auto image = createQImage(reader->GetOutput());
    image.save(outputFilename.c_str());

    return 0;
}

QImage createQImage1(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex   = 0;

    QRgb* qImageBits            = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++);

            QRgb color                         = qRgba(tuple[0], tuple[0], tuple[0], 255);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }

    return qImage;
}

QImage createQImage2(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex   = 0;

    QRgb* qImageBits            = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++ * 2);

            QRgb color                         = qRgba(tuple[0], tuple[0], tuple[0], tuple[1]);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }

    return qImage;
}

QImage createQImage3(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex   = 0;

    QRgb* qImageBits            = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++ * 3);

            QRgb color                         = qRgba(tuple[0], tuple[1], tuple[2], 255);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }

    // Qt图片和VTK图片的原点不同，qt是左上角，vtk是左下角
    return qImage.mirrored(false, true);
}

QImage createQImage4(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex   = 0;

    QRgb* qImageBits            = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++ * 4);

            QRgb color                         = qRgba(tuple[0], tuple[1], tuple[2], tuple[3]);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }

    return qImage;
}

QImage createQImage(vtkImageData* imageData)
{
    if (!imageData)
        return QImage();

    int width                     = imageData->GetDimensions()[0];
    int height                    = imageData->GetDimensions()[1];
    vtkUnsignedCharArray* scalars = vtkUnsignedCharArray::SafeDownCast(imageData->GetPointData()->GetScalars());

    if (!width || !height || !scalars)
        return QImage();

    switch (scalars->GetNumberOfComponents())
    {
    case 1:
        return createQImage1(width, height, scalars);
    case 2:
        return createQImage2(width, height, scalars);
    case 3:
        return createQImage3(width, height, scalars);
    case 4:
        return createQImage4(width, height, scalars);
    }

    return QImage();
}

#endif // TEST202