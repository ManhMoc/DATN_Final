#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QPixmap>
#include <QtCore>
#include "main.h"
#include<string.h>
using namespace cv;
using namespace std;

#define NUMBER_CHECK_PLATE 5
static cv::Mat imgOriginalScene;
static string plateCarRecogned="hello world";
Mat myImage;//Declaring a matrix to load the frames//
VideoCapture cap(0);//Declaring an object to capture stream of frames from default camera//
PossiblePlate PLATE;
//static uint8_t CardID[5] = { 0x00, };
string str_CardID="";
QString Q_str_CardID="";
QString Q_str_Plate;
int NUMBER_CAR=0;

bool Main_Lcd16x2()
{
    lcd1602Init(1, 0x27);
    lcd1602WriteString("Plate recogntion:");
    lcd1602SetCursor(0,1);
    lcd1602WriteString("   ");
    lcd1602WriteString(&plateCarRecogned[0]);

    return true;
}
bool Main_LcdStatus()
{
    lcd1602Init(1, 0x27);
    lcd1602WriteString("Status now:");
    lcd1602SetCursor(0,1);
    lcd1602WriteString("  ");
    string num=to_string(NUMBER_CAR);
    lcd1602WriteString(&num[0]);
    lcd1602WriteString("/100 Cars");

    return true;
}
static bool RFID() {



    MFRC522_Status_t ret;
    //Recognized card ID
    static uint8_t CardID[5] = { 0x00, };
    //static char command_buffer[1024];

    ret = MFRC522_Init('A');
    if (ret < 0) {
        perror("Failed to initialize");
        return false;
    }
    printf("User Space RC522 Application\r\n");
    while (1) {
        ret = MFRC522_Check(CardID);
        if (ret != MI_OK) {
            printf(".");
            fflush(stdout);
            //continue;
            return false;
        }
        ret |= tag_select(CardID);
        if (ret == MI_OK) {
            //ret = scan_loop(CardID);
            printf("Success\n");
            return true;
        }
        else {
            printf("Error\n");
            return false;
        }
    }
    return false;
}
bool PlateRecognition()
{
    int count=0;
    int indexPlate=-1;
    int arr[NUMBER_CHECK_PLATE]={0};
    string Plate[NUMBER_CHECK_PLATE];
    bool blnKNNTrainingSuccessful = loadKNNDataAndTrainKNN();           // attempt KNN training
    if (blnKNNTrainingSuccessful == false) {                            // if KNN training was not successful
                                                                        // show error message
        std::cout << std::endl << std::endl << "error: error: KNN traning was not successful" << std::endl << std::endl;
        return false;                                                      // and exit program
    }

    Mat myImage;//Declaring a matrix to load the frames//
    while (count < NUMBER_CHECK_PLATE){ //Taking an everlasting loop to show the video//
        cap >> imgOriginalScene;
        std::vector<PossiblePlate> vectorOfPossiblePlates = detectPlatesInScene(imgOriginalScene);          // detect plates
        vectorOfPossiblePlates = detectCharsInPlates(vectorOfPossiblePlates);                               // detect chars in plates
        if (vectorOfPossiblePlates.empty()) {
            cout<<".";                                              // if no plates were found
            count++;
            continue;
        }
        else
        {// sort the vector of possible plates in DESCENDING order (most number of chars to least number of chars)
            std::sort(vectorOfPossiblePlates.begin(), vectorOfPossiblePlates.end(), PossiblePlate::sortDescendingByNumberOfChars);
                PossiblePlate licPlate = vectorOfPossiblePlates.front();
            if (licPlate.strChars.length() != 8)
                    continue;
            Plate[count++]=licPlate.strChars;
            PLATE=licPlate;
        }
    }
    for(int i=0;i<NUMBER_CHECK_PLATE;i++)
    {
    for(int j=i+1;j<NUMBER_CHECK_PLATE;j++)
    {
        if(Plate[j]==Plate[i]) arr[i]++;
    }
    if(arr[i]>indexPlate) indexPlate=i;
    }
    plateCarRecogned=Plate[indexPlate];

    return true;
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pix("/home/pi/opencv/demo2/image_vn1.jpg");
    ui->label_pic->setPixmap(pix.scaled(ui->label_pic->width(),ui->label_pic->height(),Qt::KeepAspectRatio));
    Main_LcdStatus();
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    MFRC522_Status_t ret;
    //Recognized card ID
    //uint8_t CardID[5] = { 0x00, };
    static uint8_t CardID[5] = { 0x00, };
    static char command_buffer[1024];
    ret = MFRC522_Init('B');
    //RFID();
    QImage image2;
    while (1){ //Taking an everlasting loop to show the video//
        //waitKey(1500);
        cap >> myImage;
        image2 =QImage((uchar*) myImage.data,myImage.cols,myImage.rows,myImage.step,QImage::Format_RGB888);
        ui->label_pic->setPixmap(QPixmap::fromImage(image2));

        ret = MFRC522_Check(CardID);
        if (ret == MI_OK) {
             str_CardID="";
             for(int i=0;i<5;i++)
             {
                 string cancat=to_string(CardID[i]);
                 str_CardID +=cancat;
             }
            Q_str_CardID = QString::fromStdString(str_CardID);
            ui->label_id_card->setText(Q_str_CardID);
            MainWindow::on_pushButton_2_clicked();
        }
        QApplication::processEvents();
        //break;
    }
}

void MainWindow::on_pushButton_2_clicked()//Cature a picture
{
   /* Mat myImage;//Declaring a matrix to load the frames//
    cap >> myImage;
    QImage image2 =QImage((uchar*) myImage.data,myImage.cols,myImage.rows,myImage.step,QImage::Format_RGB888);
    ui->label_3->setPixmap(QPixmap::fromImage(image2));*/
    bool isGoIn=true;
    if(PlateRecognition())
    {
        Q_str_Plate = QString::fromStdString(plateCarRecogned);
        ui->label_plate->setText(Q_str_Plate);

        QImage image2 =QImage((uchar*) (PLATE.imgPlate).data,(PLATE.imgPlate).cols,(PLATE.imgPlate).rows,(PLATE.imgPlate).step,QImage::Format_RGB888);
        ui->label_img->setPixmap(QPixmap::fromImage(image2));
        for(int row=0;row<NUMBER_CAR;row++)
        {
            QString id=ui->table_1->item(row,0)->text();
            QString plate=ui->table_1->item(row,1)->text();
            if(id==Q_str_CardID)
            {
                if(plate == Q_str_Plate)
                {
                    ui->table_1->removeRow(row);
                    NUMBER_CAR--;
                }
                return;
            }
        }
        ui->table_1->insertRow(NUMBER_CAR);
        ui->table_1->setItem(NUMBER_CAR,0,new QTableWidgetItem(Q_str_CardID));
        ui->table_1->setItem(NUMBER_CAR,1,new QTableWidgetItem(Q_str_Plate));
        Main_Lcd16x2();
        NUMBER_CAR++;
        MainWindow::on_pushButton_3_clicked();
    }
    

}

void MainWindow::on_pushButton_3_clicked()//Open Door
{
    int angle = 90;
    uint8_t GPIO_Number=18;
    float hightime = 10.3*(float)angle + 546;
    int numberPulses=50;

    if(!bcm2835_init())
    {

    }
    bcm2835_gpio_fsel(GPIO_Number, BCM2835_GPIO_FSEL_OUTP); //GPIO18 :pin 12
    for(int j=0; j< numberPulses; j++)//exucting pulses
    {
        bcm2835_gpio_set(GPIO_Number);//GPIO18 high
        bcm2835_delayMicroseconds((int)hightime);
        bcm2835_gpio_clr(GPIO_Number);//GPIO18 low
        bcm2835_delayMicroseconds(20000 - (int)hightime);//each pulse is 20ms
    }
    Main_LcdStatus();

}

void MainWindow::on_pushButton_4_clicked()// Close Door
{
    int angle = 0;
    uint8_t GPIO_Number=18;
    float hightime = 10.3*(float)angle + 546;
    int numberPulses=50;

    if(!bcm2835_init())
    {

    }
    bcm2835_gpio_fsel(GPIO_Number, BCM2835_GPIO_FSEL_OUTP); //GPIO18 :pin 12
    for(int j=0; j< numberPulses; j++)//exucting pulses
    {
        bcm2835_gpio_set(GPIO_Number);//GPIO18 high
        bcm2835_delayMicroseconds((int)hightime);
        bcm2835_gpio_clr(GPIO_Number);//GPIO18 low
        bcm2835_delayMicroseconds(20000 - (int)hightime);//each pulse is 20ms
    }
}
void MainWindow::on_pushButton_5_clicked()
{
    if(PlateRecognition())
    {
        QString qstr1 = QString::fromStdString(plateCarRecogned);
        ui->label_3->setText(qstr1);

    }
}

