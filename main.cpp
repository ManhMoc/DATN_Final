#include "mainwindow.h"
#include <QApplication>
#include "main.h"

using namespace cv;
using namespace std;

#define NUMBER_CHECK 5
static cv::Mat imgOriginalScene;
static string plateCarRecogned;
bool Main_PlateRecognition()
{
    int count=0;
    int indexPlate=-1;
    int arr[10]={0};
    string Plate[10];
    bool blnKNNTrainingSuccessful = loadKNNDataAndTrainKNN();           // attempt KNN training
    if (blnKNNTrainingSuccessful == false) {                            // if KNN training was not successful
                                                                        // show error message
        std::cout << std::endl << std::endl << "error: error: KNN traning was not successful" << std::endl << std::endl;
        return false;                                                      // and exit program
    }

    Mat myImage;//Declaring a matrix to load the frames//
    namedWindow("Video Player");//Declaring the video to show the video//
    VideoCapture cap(0);//Declaring an object to capture stream of frames from default camera//
    if (!cap.isOpened()){ //This section prompt an error message if no video stream is found//
    cout << "No video stream detected" << endl;
    system("pause");
    return false;
    }
    while (count < 10){ //Taking an everlasting loop to show the video//
    cap >> imgOriginalScene;
    std::vector<PossiblePlate> vectorOfPossiblePlates = detectPlatesInScene(imgOriginalScene);          // detect plates
    vectorOfPossiblePlates = detectCharsInPlates(vectorOfPossiblePlates);                               // detect chars in plates
    if (vectorOfPossiblePlates.empty()) {
        cout<<".";                                              // if no plates were found
            continue;
    }
    else
    {// sort the vector of possible plates in DESCENDING order (most number of chars to least number of chars)
        std::sort(vectorOfPossiblePlates.begin(), vectorOfPossiblePlates.end(), PossiblePlate::sortDescendingByNumberOfChars);
            PossiblePlate licPlate = vectorOfPossiblePlates.front();
        if (licPlate.strChars.length() != 8)
                continue;
        Plate[count++]=licPlate.strChars;
    }
    }
    for(int i=0;i<10;i++)
    {
    for(int j=i+1;j<10;j++)
    {
        if(Plate[j]==Plate[i]) arr[i]++;
    }
    if(arr[i]>indexPlate) indexPlate=i;
    }
    plateCarRecogned=Plate[indexPlate];

    return true;
}
bool Main_RFID() {



    MFRC522_Status_t ret;
    //Recognized card ID
    uint8_t CardID[5] = { 0x00, };
    static char command_buffer[1024];

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
            continue;
        }
        ret |= tag_select(CardID);
        if (ret == MI_OK) {
            //ret = scan_loop(CardID);
            printf("Success\n");
            //return true;
        }
        else {
            printf("Error\n");
            //return false;
        }
    }
}

bool Main_Servo(int angle,int GPIO_Number)
{

    float hightime = 10.3*(float)angle + 546;
    int numberPulses=50;

    if(!bcm2835_init())
    return false;
    bcm2835_gpio_fsel(GPIO_Number, BCM2835_GPIO_FSEL_OUTP); //GPIO18 :pin 12
    for(int j=0; j< numberPulses; j++)//exucting pulses
    {
        bcm2835_gpio_set(GPIO_Number);//GPIO18 high
        bcm2835_delayMicroseconds((int)hightime);
        bcm2835_gpio_clr(GPIO_Number);//GPIO18 low
        bcm2835_delayMicroseconds(20000 - (int)hightime);//each pulse is 20ms
    }

    return true;

}


int main(int argc, char *argv[])
{
    //printf("hello");
    //Main_Lcd16x2();
    //Main_Servo(0,18);//GPIO18 :pin 12, 0 degree
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void drawRedRectangleAroundPlate(cv::Mat &imgOriginalScene, PossiblePlate &licPlate) {
    cv::Point2f p2fRectPoints[4];

    licPlate.rrLocationOfPlateInScene.points(p2fRectPoints);            // get 4 vertices of rotated rect

    for (int i = 0; i < 4; i++) {                                       // draw 4 red lines
        cv::line(imgOriginalScene, p2fRectPoints[i], p2fRectPoints[(i + 1) % 4], SCALAR_RED, 2);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void writeLicensePlateCharsOnImage(cv::Mat &imgOriginalScene, PossiblePlate &licPlate) {
    cv::Point ptCenterOfTextArea;                   // this will be the center of the area the text will be written to
    cv::Point ptLowerLeftTextOrigin;                // this will be the bottom left of the area that the text will be written to

    int intFontFace = cv::FONT_HERSHEY_SIMPLEX;                              // choose a plain jane font
    double dblFontScale = (double)licPlate.imgPlate.rows / 30.0;            // base font scale on height of plate area
    int intFontThickness = (int)std::round(dblFontScale * 1.5);             // base font thickness on font scale
    int intBaseline = 0;

    cv::Size textSize = cv::getTextSize(licPlate.strChars, intFontFace, dblFontScale, intFontThickness, &intBaseline);      // call getTextSize

    ptCenterOfTextArea.x = (int)licPlate.rrLocationOfPlateInScene.center.x;         // the horizontal location of the text area is the same as the plate

    if (licPlate.rrLocationOfPlateInScene.center.y < (imgOriginalScene.rows * 0.75)) {      // if the license plate is in the upper 3/4 of the image
                                                                                            // write the chars in below the plate
        ptCenterOfTextArea.y = (int)std::round(licPlate.rrLocationOfPlateInScene.center.y) + (int)std::round((double)licPlate.imgPlate.rows * 1.6);
    }
    else {                                                                                // else if the license plate is in the lower 1/4 of the image
                                                                                          // write the chars in above the plate
        ptCenterOfTextArea.y = (int)std::round(licPlate.rrLocationOfPlateInScene.center.y) - (int)std::round((double)licPlate.imgPlate.rows * 1.6);
    }

    ptLowerLeftTextOrigin.x = (int)(ptCenterOfTextArea.x - (textSize.width / 2));           // calculate the lower left origin of the text area
    ptLowerLeftTextOrigin.y = (int)(ptCenterOfTextArea.y + (textSize.height / 2));          // based on the text area center, width, and height

                                                                                            // write the text on the image
    cv::putText(imgOriginalScene, licPlate.strChars, ptLowerLeftTextOrigin, intFontFace, dblFontScale, SCALAR_YELLOW, intFontThickness);
}



int tag_select(uint8_t *CardID) {
    int ret_int;
    printf(
            "Card detected    0x%02X 0x%02X 0x%02X 0x%02X, Check Sum = 0x%02X\r\n",
            CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
    ret_int = MFRC522_SelectTag(CardID);
    if (ret_int == 0) {
        printf("Card Select Failed\r\n");
        return -1;
    } else {
        //printf("Card Selected, Type:%s\r\n",
                //MFRC522_TypeToString(MFRC522_ParseType(ret_int)));
    }
    ret_int = 0;
    return ret_int;
}
