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
            //int clickpositionone[2];
            //this->Interactor->GetEventPosition(clickpositionone); //get mouse coordinates x and y
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
        //void writeInFile() {
        //    myfile.open("myfile.txt", ios::out); // Open the file for writing
        //    if (myfile.is_open()) { // Check if file opened successfully
        //        double* point1 = LineSource->GetPoint1();
        //        double* point2 = LineSource->GetPoint2();
        //        myfile << point1[0] << " " << point1[1] << endl; // Write data to the file
        //        myfile << point2[0] << " " << point2[1] << endl;
        //        //myfile << lineActor->GetProperty()->GetColor() << endl;
        //        myfile.close(); // Close the file
        //    }
        //    else {
        //        cout << "Unable to create or open the file." << endl;
        //    }
        //}

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
            file.close();
        }
        //filecounter++;
        //string filename = "myfile" + to_string(filecounter) + ".txt";
        //myfile.open(filename, ios::out); // open the file for writing

        //if (myfile.is_open()) { // check if file opened successfully
        //    double* point1 = linesource->GetPoint1();
        //    double* point2 = linesource->GetPoint2();
        //    myfile << point1[0] << " " << point1[1] << endl; // write data to the file
        //    myfile << point2[0] << " " << point2[1] << endl;
        //    //myfile << lineactor->getproperty()-> getcolor() << endl;
        //    myfile.close(); // close the file
        //}
        //else {
        //    cout << "unable to create or open the file." << endl;
        //}
    }

    void updateTextCoordinates(vtkLineSource* linesource, vtkTextActor* TextActor, vtkActor* lineActor) {
        double* point1 = linesource->GetPoint1();
        double* point2 = linesource->GetPoint2();
        char text[100];
        sprintf(text, "Line coordinates: (%.2f, %.2f) - (%.2f, %.2f)", point1[0], point1[1], point2[0], point2[1]);
        TextActor->SetInput(text);
        TextActor->Modified();
        //writeInFile(linesource,lineActor);
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
            //QString color, property;
            if (!in.atEnd()) {
                QStringList linepoint1 = in.readLine().split(" ");
                /*x1 = linepoint1[0].toDouble();
                y1 = linepoint1[1].toDouble();*/
                linesource->SetPoint1(linepoint1[0].toDouble(), linepoint1[1].toDouble(), 0.0);
            }
            if (!in.atEnd()) {
               QStringList linepoint2 = in.readLine().split(" ");
               /*x2 = linepoint2[0].toDouble();
               y2 = linepoint2[1].toDouble();*/
               linesource->SetPoint2(linepoint2[0].toDouble(), linepoint2[1].toDouble(), 0.0);
            }
            if (!in.atEnd()) {
                QStringList rgb = in.readLine().split(" ");
                double rgbarr[3];
                rgbarr[0]=rgb.at(0).toDouble();
                rgbarr[1] = rgb.at(1).toDouble();
                rgbarr[2] = rgb.at(2).toDouble();
                lineActor->GetProperty()->SetColor(rgbarr);
            }
            /*if (!in.atEnd()) {
                property = in.readLine();
            }*/
            window->Render();
            updateTextCoordinates(linesource, TextActor, lineActor);
            file.close(); //close the file object.
            }
            //readfile.open(fileName, ios::in); //open a file to perform read operation using file object
            //if (readfile.is_open()) { //checking whether the file is open
            //    double x1, y1, x2, y2;
            //    readfile >> x1 >> y1 >> x2 >> y2;
            //    linesource->SetPoint1(x1, y1,0.0);
            //    linesource->SetPoint2(x2, y2, 0.0);
            //    window->Render();
            //    updateTextCoordinates(linesource, TextActor, lineActor);
            //    readfile.close(); //close the file object.
            //}
        
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

    //vtkNew<vtkRenderWindow> renderWindow;
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(lineactor);
    window->AddRenderer(renderer);
    //renderWindow->AddRenderer(renderer);

    //vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkPointPicker> pointPicker;
    window->SetInteractor(vtkRenderWidget->interactor());
    window->GetInteractor()->SetPicker(pointPicker);
    //renderWindowInteractor->SetPicker(pointPicker);
    //renderWindowInteractor->SetRenderWindow(window);
    //renderWindowInteractor->SetRenderWindow(renderWindow);
    //interactor->SetInteractorStyle(style);

    vtkNew<customMouseInteractorStyle> style;
    style->setLineSource(linesource);
    style->setVTKActor(lineactor);
    window->GetInteractor()->SetInteractorStyle(style);
    //window->GetInteractor()->Start();

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

    //renderWindowInteractor->SetInteractorStyle(style);
    //window->SetInteractor(renderWindowInteractor);
    //window->Render();
    //renderWindow->AddRenderer(renderer);
    //renderWindowInteractor->Initialize();
    //renderWindowInteractor->Start();
    //// setup initial status
    //std::mt19937 randEng(0);
    //::Randomize(sphere, mapper, window, randEng);

    //// connect the buttons
    QObject::connect(&setFirstCoordinate, &QPushButton::released,
        [&]() { ::setFirstCoordinate(linesource,window,textActor,lineactor); });

    QObject::connect(&setSecondCoordinate, &QPushButton::released,
        [&]() { ::setSecondCoordinate(linesource,window,textActor, lineactor); });

    QObject::connect(&readFile, &QPushButton::released,
        [&]() { ::readInputFile(linesource, window, textActor, lineactor); });

    QObject::connect(&writeFile, &QPushButton::released,
        [&]() { ::writeInFile(linesource,lineactor); });
    
    mainWindow.show();

    return app.exec();
    //return EXIT_SUCCESS;
}
