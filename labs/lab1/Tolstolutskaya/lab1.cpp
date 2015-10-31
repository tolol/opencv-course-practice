#include "iostream"
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "cv.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv){
	Mat src, src_gray,image, dst, finish;
	Mat detected_edges;
	image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	src = image.clone();

	cvtColor(src, src_gray, CV_RGB2GRAY);
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	Mat grad, integR, integG, integB;
	finish = image.clone();



	// окно для отображения картинки
	cvNamedWindow("original",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("NewImage",CV_WINDOW_AUTOSIZE);


	int lowThreshold = 70;
	int ratio = 3;
	int kernel_size = 3;
	/// Canny detector
	Canny( src_gray, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

	//инверсия
	detected_edges =  cv::Scalar::all(255) - detected_edges;

	//Карта расстояний dst
	distanceTransform (detected_edges, dst, CV_DIST_L2, 5);
	//normalize(dst, dst, 0.0, 1.0, NORM_MINMAX);

	Mat chan[3];
	vector<Mat> fin_chan(3);
	split(image, chan);
	split(image, fin_chan);

	integral (chan[0], integB, -1);//интеграл синий
	integral (chan[1], integG, -1);//зеленый
	integral (chan[2], integR, -1);//красный


	Point a,b,c,d;

	a = Point (0,0);
	b = Point (0,0);
	c = Point (0,0);
	d = Point (0,0);

	for (int j = 0; j < src_gray.cols; j++) {
		for (int i = 0; i < src_gray.rows; i++){
			int di = dst.at<float>(i,j);

			a.x = (i - di) < 0 ? 0 : i - di;
			a.y = (j - di) < 0 ? 0 : j - di;
			b.x = (i - di) < 0 ? 0 : i - di;
			b.y = (j + di + 1) > src_gray.cols ? src_gray.cols: j + di + 1;
			c.x = (i + di + 1) > src_gray.rows ? src_gray.rows: (i + di + 1);
			c.y = (j - di) < 0 ? 0 : j - di;
			d.x = (i + di + 1) > src_gray.rows ? src_gray.rows: (i + di + 1);
			d.y = (j + di + 1) > src_gray.cols ? src_gray.cols: j + di + 1;

			int weight = b.y - a.y;
			int height = c.x - a.x;
			int sqare = weight*height;

			int tlR= (int)integR.at<int>((a.x),(a.y));
			int trR= (int)integR.at<int>((b.x),(b.y));
			int blR= (int)integR.at<int>(c.x,c.y);
			int brR= (int)integR.at<int>(d.x,d.y);

			int tlB= (int)integB.at<int>((a.x),(a.y));
			int trB= (int)integB.at<int>((b.x),(b.y));
			int blB= (int)integB.at<int>(c.x,c.y);
			int brB= (int)integB.at<int>(d.x,d.y);

			int tlG= (int)integG.at<int>((a.x),(a.y));
			int trG= (int)integG.at<int>((b.x),(b.y));
			int blG= (int)integG.at<int>(c.x,c.y);
			int brG= (int)integG.at<int>(d.x,d.y);

			int regionB = (int)((brB-blB-trB+tlB)/ sqare);
			int regionG = (int)((brG-blG-trG+tlG)/ sqare);
			int regionR = (int)((brR-blR-trR+tlR)/ sqare);

			fin_chan[0].at<uchar>(i,j) = (uchar) regionB;
			fin_chan[1].at<uchar>(i,j) = (uchar) regionG;
			fin_chan[2].at<uchar>(i,j) = (uchar) regionR;
		}
	}
	cv::merge(fin_chan, finish);
	// показываем картинку
	imshow("original", image);
	imshow("NewImage", finish);

	cv::waitKey(0);

	cvDestroyWindow("original");
	cvDestroyWindow("NewImage");

	return 0;
}
