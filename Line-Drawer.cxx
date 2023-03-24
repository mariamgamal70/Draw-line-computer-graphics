#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkPointPicker.h>
#include <vtkRendererCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>
#include <vtkTextWidget.h>

#include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QFileDialog>
#include <QComboBox>
#include <QColorDialog>

#include <cmath>
#include <cstdlib>
#include <random>
#include <iostream>
#include <fstream>
using namespace std;

ofstream myfile;
ifstream readfile;
int filecounter = 0;

namespace {
    // Define interaction style
    class customMouseInteractorStyle : public vtkInteractorStyleTrackballCamera
    {
    public:
        static customMouseInteractorStyle* New();
        vtkTypeMacro(customMouseInteractorStyle, vtkInteractorStyleTrackballCamera);

        virtual void OnLeftButtonDown() override
        {
            click++;
            vtkRenderWindowInteractor* interactor = this->Interactor;
            if (click == 1) {
                this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],//pick the first point using mouse x&y
                    this->Interactor->GetEventPosition()[1],
                    0, // always zero.
                    this->Interactor->GetRenderWindow()
                    ->GetRenderers()
                    ->GetFirstRenderer());//The renderer in which the picking operation will be performed.
                double pickedone[3];
                this->Interactor->GetPicker()->GetPickPosition(pickedone);
                UpdateFirstPoint(pickedone);
            }
            if (click == 2) {
                double pickedtwo[3];
                this->Interactor->GetPicker()->GetPickPosition(pickedtwo);
                vtkRenderWindowInteractor* interactor = this->Interactor;
                this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],//pick the first point using mouse x&y
                    this->Interactor->GetEventPosition()[1],
                    0, // always zero.
                    this->Interactor->GetRenderWindow()
                    ->GetRenderers()
                    ->GetFirstRenderer());//The renderer in which the picking operation will be performed.
                UpdateSecondPoint(pickedtwo);
                click = 0;
            }
            double* point1 = LineSource->GetPoint1();
            double* point2 = LineSource->GetPoint2();
            char text[100];
            sprintf(text, "Line coordinates: (%.2f, %.2f) - (%.2f, %.2f)", point1[0], point1[1], point2[0], point2[1]);
            TextActor->SetInput(text);
            TextActor->Modified();
            //writeInFile();
            // Forward events
            vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
        }
        void setLineSource(vtkLineSource* linesource) {
            this->LineSource = linesource;
        }
        void setTextActor(vtkTextActor* actor) {
            TextActor = actor;
        }
        void UpdateFirstPoint(double* pickedone) {
            LineSource->SetPoint1(pickedone[0], pickedone[1], pickedone[2]);
        }
        void UpdateSecondPoint(double* pickedtwo) {
            LineSource->SetPoint2(pickedtwo[0], pickedtwo[1], pickedtwo[2]);
        }
        void setVTKActor(vtkActor* lineActor) {
            this->lineActor = lineActor;
        }

    private:
        vtkLineSource* LineSource;
        vtkTextActor* TextActor;
        vtkActor* lineActor;
        int click = 0;
    };
    vtkStandardNewMacro(customMouseInteractorStyle);

  //-------------------------------------------------------------------------------------------------------------------------------------------

    void writeInFile(vtkLineSource* linesource, vtkActor* lineActor) {
        QString fileName = QFileDialog::getSaveFileName(nullptr, "Save File", ".", "Text Files (*.txt)");
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            double* point1 = linesource->GetPoint1();
            double* point2 = linesource->GetPoint2();
            out << point1[0] << " " << point1[1] << Qt::endl; // write data to the file
            out << point2[0] << " " << point2[1] << Qt::endl;
            out << lineActor->GetProperty()->GetColor()[0] << " "
                << lineActor->GetProperty()->GetColor()[1] << " "
                << lineActor->GetProperty()->GetColor()[2] << Qt::endl;
            out<<lineActor->GetProperty()->GetLineWidth() << Qt::endl;
            file.close();
        }
    }

    void updateTextCoordinates(vtkLineSource* linesource, vtkTextActor* TextActor, vtkActor* lineActor) {
        double* point1 = linesource->GetPoint1();
        double* point2 = linesource->GetPoint2();
        char text[100];
        sprintf(text, "Line coordinates: (%.2f, %.2f) - (%.2f, %.2f)", point1[0], point1[1], point2[0], point2[1]);
        TextActor->SetInput(text);
        TextActor->Modified();
    }
    void setFirstCoordinate(vtkLineSource* linesource, vtkGenericOpenGLRenderWindow* window, vtkTextActor* TextActor, vtkActor* lineActor) {
        double x1 = QInputDialog::getDouble(NULL, "Enter first coordinates", "x1 coordinate", 0, -1000, 1000, 2);
        double y1 = QInputDialog::getDouble(NULL, "Enter first coordinates", "y1 coordinate", 0, -1000, 1000, 2);
        linesource->SetPoint1(x1, y1, 0.0);
        updateTextCoordinates(linesource, TextActor, lineActor);
        window->Render();
    }
    void setSecondCoordinate(vtkLineSource* linesource, vtkGenericOpenGLRenderWindow* window, vtkTextActor* TextActor, vtkActor* lineActor) {
        double x2 = QInputDialog::getDouble(NULL, "Enter second coordinates", "x2 coordinate", 0, -1000, 1000, 2);
        double y2 = QInputDialog::getDouble(NULL, "Enter second coordinates", "y2 coordinate", 0, -1000, 1000, 2);
        linesource->SetPoint2(x2, y2, 0.0);
        updateTextCoordinates(linesource, TextActor, lineActor);
        window->Render();
    }

    void readInputFile(vtkLineSource* linesource, vtkGenericOpenGLRenderWindow* window, vtkTextActor* TextActor, vtkActor* lineActor) {
        QString fileObject = QFileDialog::getOpenFileName(nullptr, "Open File", ".", "Text Files (*.txt)");
        if (fileObject.isEmpty()) {
            return;  // Dialog was cancelled
        }
        QFile file(fileObject);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        else {
            QTextStream in(&file);
            double x1, y1, x2, y2;
            if (!in.atEnd()) {
                QStringList linepoint1 = in.readLine().split(" ");
                linesource->SetPoint1(linepoint1[0].toDouble(), linepoint1[1].toDouble(), 0.0);
            }
            if (!in.atEnd()) {
                QStringList linepoint2 = in.readLine().split(" ");
                linesource->SetPoint2(linepoint2[0].toDouble(), linepoint2[1].toDouble(), 0.0);
            }
            if (!in.atEnd()) {
                QStringList rgb = in.readLine().split(" ");
                double rgbarr[3];
                rgbarr[0] = rgb.at(0).toDouble();
                rgbarr[1] = rgb.at(1).toDouble();
                rgbarr[2] = rgb.at(2).toDouble();
                lineActor->GetProperty()->SetColor(rgbarr);
            }
            if (!in.atEnd()) {
                QStringList lineWidthString= in.readLine().split(" ");;
                double lineWidth = lineWidthString.at(0).toDouble();
                lineActor->GetProperty()->SetLineWidth(lineWidth);
            }
            window->Render();
            updateTextCoordinates(linesource, TextActor, lineActor);
            file.close(); //close the file object.
        }
    }
    void openColorWindow(vtkGenericOpenGLRenderWindow* window, vtkActor* lineActor) {
        QColorDialog colorDialog;
        if (colorDialog.exec() == QDialog::Accepted) {
            QColor color = colorDialog.currentColor();
            int red = color.red();
            int green = color.green();
            int blue = color.blue();
            lineActor->GetProperty()->SetColor(red,green,blue);
            window->Render();
        }
    }

    //void changeLineProperty(int index, vtkGenericOpenGLRenderWindow* window, vtkActor* lineActor) {
    //    cout<<lineActor->GetProperty()->GetLineStipplePattern();
    //    if (index == 0) {
    //        lineActor->GetProperty()->SetLineStipplePattern(65535);
    //        lineActor->GetProperty()->SetLineStippleRepeatFactor(1);
    //        //window->Render();
    //    }
    //    else if (index == 1) {//dashed
    //        lineActor->GetProperty()->SetLineStipplePattern(255); 
    //        lineActor->GetProperty()->SetLineStippleRepeatFactor(1);
    //        //window->Render();
    //    }
    //    else if (index == 2) {//dotted
    //        lineActor->GetProperty()->SetLineStipplePattern(43690);
    //        lineActor->GetProperty()->SetLineStippleRepeatFactor(1);
    //        //window->Render();
    //    }
    //}
    
    void changeLineWidth(int value, vtkGenericOpenGLRenderWindow* window, vtkActor* lineActor) {
        lineActor->GetProperty()->SetLineWidth(value);
        window->Render();
    }

} // namespace

int main(int argc, char** argv)
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication app(argc, argv);

    // main window
    QMainWindow mainWindow;
    mainWindow.resize(1200, 900);

    // control area
    QDockWidget controlDock;
    mainWindow.addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

    QLabel controlDockTitle("Control Dock");
    controlDockTitle.setMargin(20);
    controlDock.setTitleBarWidget(&controlDockTitle);

    QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
    QWidget layoutContainer;
    layoutContainer.setLayout(dockLayout);
    controlDock.setWidget(&layoutContainer);

    QPushButton setFirstCoordinate;
    setFirstCoordinate.setText("Set First Coordinate");
    dockLayout->addWidget(&setFirstCoordinate, 0, Qt::AlignTop);

    QPushButton setSecondCoordinate;
    setSecondCoordinate.setText("Set Second Coordinate");
    dockLayout->addWidget(&setSecondCoordinate, 1, Qt::AlignTop);

    QPushButton readFile;
    readFile.setText("Read Input File");
    dockLayout->addWidget(&readFile, 0, Qt::AlignTop);

    QPushButton writeFile;
    writeFile.setText("Write Input File");
    dockLayout->addWidget(&writeFile, 1, Qt::AlignTop);

    QPushButton colorButton;
    colorButton.setText("Select color");
    dockLayout->addWidget(&colorButton, 0, Qt::AlignTop);

    QSlider slider;
    slider.setMinimum(0);
    slider.setMaximum(10);
    slider.setValue(0);
    slider.setOrientation(Qt::Horizontal);
    dockLayout->addWidget(&slider, 1, Qt::AlignTop);

    /*QComboBox comboBox ;
    comboBox.addItem(QApplication::tr("solid"));
    comboBox.addItem(QApplication::tr("dashed"));
    comboBox.addItem(QApplication::tr("dotted"));
    dockLayout->addWidget(&comboBox, 1, Qt::AlignTop);*/

     //render area
    QPointer<QVTKOpenGLNativeWidget> vtkRenderWidget = new QVTKOpenGLNativeWidget();
    mainWindow.setCentralWidget(vtkRenderWidget);
    mainWindow.setWindowTitle("VTK Line Example");



    // VTK part
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    vtkRenderWidget->setRenderWindow(window);

    vtkNew<vtkLineSource> linesource;
    vtkNew<vtkPolyDataMapper> linemapper;
    linemapper->SetInputConnection(linesource->GetOutputPort());

    vtkNew<vtkActor> lineactor;
    lineactor->SetMapper(linemapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(lineactor);
    window->AddRenderer(renderer);

    vtkNew<vtkPointPicker> pointPicker;
    window->SetInteractor(vtkRenderWidget->interactor());
    window->GetInteractor()->SetPicker(pointPicker);

    vtkNew<customMouseInteractorStyle> style;
    style->setLineSource(linesource);
    style->setVTKActor(lineactor);

    window->GetInteractor()->SetInteractorStyle(style);

    vtkNew<vtkTextActor> textActor;
    textActor->SetInput("Line coordinates: (0, 0) - (0, 0)");
    textActor->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
    textActor->GetTextProperty()->SetFontSize(40);
    style->setTextActor(textActor);
    renderer->AddActor(textActor);

    vtkNew<vtkTextRepresentation> textRepresentation;
    textRepresentation->GetPositionCoordinate()->SetValue(0.15, 0.15);
    textRepresentation->GetPosition2Coordinate()->SetValue(0.7, 0.2);
    
    vtkNew<vtkTextWidget> textWidget;
    textWidget->SetRepresentation(textRepresentation);
    textWidget->SelectableOff();
    textWidget->SetInteractor(vtkRenderWidget->interactor());


    //// connect the buttons
    QObject::connect(&setFirstCoordinate, &QPushButton::released,
        [&]() { ::setFirstCoordinate(linesource,window,textActor,lineactor); });

    QObject::connect(&setSecondCoordinate, &QPushButton::released,
        [&]() { ::setSecondCoordinate(linesource,window,textActor, lineactor); });

    QObject::connect(&readFile, &QPushButton::released,
        [&]() { ::readInputFile(linesource, window, textActor, lineactor); });

    QObject::connect(&writeFile, &QPushButton::released,
        [&]() { ::writeInFile(linesource,lineactor); });

    QObject::connect(&colorButton, &QPushButton::released,
        [&]() { ::openColorWindow(window,lineactor); });

    /*QObject::connect(&comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int index) {
        ::changeLineProperty(index,window,lineactor);
        });*/
    
    QObject::connect(&slider, &QSlider::valueChanged, [&](int value) {
        // Do something with the value
        ::changeLineWidth(value, window, lineactor);
        });

    mainWindow.show();

    return app.exec();
}
