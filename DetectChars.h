// DetectChars.h

#ifndef DETECT_CHARS_H
#define DETECT_CHARS_H

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

//#include "Main.h"
#include "PossibleChar.h"
#include "PossiblePlate.h"
#include "Preprocess.h"



// external global variables //////////////////////////////////////////////////////////////////////
extern const bool blnShowSteps;
extern cv::Ptr<cv::ml::KNearest>  kNearest;

// function prototypes ////////////////////////////////////////////////////////////////////////////

bool loadKNNDataAndTrainKNN(void);

std::vector<PossiblePlate> detectCharsInPlates(std::vector<PossiblePlate> &vectorOfPossiblePlates);

std::vector<PossibleChar> findPossibleCharsInPlate(cv::Mat &imgGrayscale, cv::Mat &imgThresh);

bool checkIfPossibleChar(PossibleChar &possibleChar);

std::vector<std::vector<PossibleChar> > findVectorOfVectorsOfMatchingChars(const std::vector<PossibleChar> &vectorOfPossibleChars);

std::vector<PossibleChar> findVectorOfMatchingChars(const PossibleChar &possibleChar, const std::vector<PossibleChar> &vectorOfChars);

double distanceBetweenChars(const PossibleChar &firstChar, const PossibleChar &secondChar);

double angleBetweenChars(const PossibleChar &firstChar, const PossibleChar &secondChar);

std::vector<PossibleChar> removeInnerOverlappingChars(std::vector<PossibleChar> &vectorOfMatchingChars);

std::string recognizeCharsInPlate(cv::Mat &imgThresh, std::vector<PossibleChar> &vectorOfMatchingChars);


#endif	// DETECT_CHARS_H
