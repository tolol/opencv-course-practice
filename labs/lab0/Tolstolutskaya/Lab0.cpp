#include "iostream"
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "cv.h"

using namespace cv;
using namespace std;



int main(int argc, char** argv){
	Mat image, srcB,srcC, imageGray, srcH;
	image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	srcB = image.clone();
	srcC = image.clone();
	cvtColor(image, imageGray, CV_BGR2GRAY);
	srcH = imageGray.clone();

	// окно для отображения картинки
	cvNamedWindow("original",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("BLUR",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("CANNY",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("EQUALISE_HIST",CV_WINDOW_AUTOSIZE);

	//фильтр Blur
	Point anchor=Point(-1,-1);
	Size ksize = Size(25, 25);
	cv::blur(image, srcB, ksize, anchor,BORDER_DEFAULT);

	// фильтр canny
	int lowThreshold = 20;
	int ratio = 7;
	int kernel_size = 3;
	Canny( image, srcC, lowThreshold, lowThreshold*ratio, kernel_size );

	//EQUALISE_HIST

	equalizeHist (imageGray, srcH);

	// показываем картинку
	imshow("original",image);
	imshow("BLUR",srcB);
	imshow("CANNY",srcC);
	imshow("EQUALISE_HIST",srcH);

	cv::waitKey(0);

	cvDestroyWindow("original");
	cvDestroyWindow("BLUR");
	cvDestroyWindow("CANNY");
	cvDestroyWindow("EQUALISE_HIST");

	return 0;
}
