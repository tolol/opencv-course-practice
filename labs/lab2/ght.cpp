#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
using namespace cv;
using namespace std;

const int splitSet = 10;

void ratioTestMatching(DescriptorMatcher& descriptorMatcher, const Mat& descriptors1, const Mat& descriptors2,
                       vector<DMatch>& filteredMatches12, float ratio = 0.6f)
{
  const int knn = 2;
  filteredMatches12.clear();
  vector<vector<DMatch> > matches12;
  descriptorMatcher.knnMatch(descriptors1, descriptors2, matches12, 2);
  for (size_t m = 0; m < matches12.size(); m++)
  {
    if (matches12[m][0].distance / matches12[m][1].distance < ratio)
      filteredMatches12.push_back(matches12[m][0]);
  }
}

void crossCheckMatching(Ptr<DescriptorMatcher>& descriptorMatcher, const Mat& descriptors1, const Mat& descriptors2,
                        vector<DMatch>& filteredMatches12, int knn = 1)
{
  filteredMatches12.clear();
  vector<vector<DMatch> > matches12, matches21;
  descriptorMatcher->knnMatch(descriptors1, descriptors2, matches12, knn);
  descriptorMatcher->knnMatch(descriptors2, descriptors1, matches21, knn);
  for (size_t m = 0; m < matches12.size(); m++)
  {
    bool findCrossCheck = false;
    for (size_t fk = 0; fk < matches12[m].size(); fk++)
    {
      DMatch forward = matches12[m][fk];

      for (size_t bk = 0; bk < matches21[forward.trainIdx].size(); bk++)
      {
        DMatch backward = matches21[forward.trainIdx][bk];
        if (backward.trainIdx == forward.queryIdx)
        {
          filteredMatches12.push_back(forward);
          findCrossCheck = true;
          break;
        }
      }
      if (findCrossCheck)
        break;
    }
  }
}

void drawX(Point& p, Mat& unionImage)
{
  int x = p.x;
  int y = p.y;
  Point pt1(x - 5, y - 5);
  Point pt2(x + 5, y + 5);
  line(unionImage, pt1, pt2, Scalar(0, 0, 255), 2);
  pt1 = Point(x - 5, y + 5);
  pt2 = Point(x + 5, y - 5);
  line(unionImage, pt1, pt2, Scalar(0, 0, 255), 2);
}

void drawKpt(Mat& img, const KeyPoint& p, const Scalar& color, int flags, Point offset = Point(0, 0))
{
  Point center(cvRound(p.pt.x)+offset.x, cvRound(p.pt.y)+offset.y);

  if (flags & DrawMatchesFlags::DRAW_RICH_KEYPOINTS)
  {
    int radius = cvRound(p.size / 2); // KeyPoint::size is a diameter

    // draw the circles around keypoints with the keypoints size
    circle(img, center, radius, color, 1, CV_AA);

    // draw orientation of the keypoint, if it is applicable
    if (p.angle != -1)
    {
      float srcAngleRad = p.angle * (float)CV_PI / 180.f;
      Point orient(cvRound(cos(srcAngleRad) * radius), cvRound(sin(srcAngleRad) * radius));
      line(img, center, center + orient, color, 1, CV_AA);
    }
  }
  else
  {
    // draw center with R=3
    int radius = 3;
    circle(img, center, radius, color, 1, CV_AA);
  }
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    cout << "Format: ght <image.png> <image_in_scene.png>" << endl;
    return -1;
  }

  cout << "< Creating detector, descriptor extractor and descriptor matcher ..." << endl;
  Ptr<FeatureDetector> detector = FeatureDetector::create("ORB");
  Ptr<DescriptorExtractor> descriptorExtractor = DescriptorExtractor::create("ORB");
  // Ptr<DescriptorMatcher> descriptorMatcher = new BruteForceMatcher<L1<float> > ();
  BFMatcher descriptorMatcher(NORM_HAMMING);

  cout << "< Reading the images..." << endl;
  Mat image = imread(argv[1]), scene = imread(argv[2]);
  cout << ">" << endl;
  if (image.empty() || scene.empty())
  {
    cout << "Can not read images" << endl;
    return -1;
  }

  cout << endl << "< Extracting keypoints from first image..." << endl;
  vector<KeyPoint> keypoints1;
  detector->detect(image, keypoints1);
  cout << keypoints1.size() << " points" << endl << ">" << endl;

  cout << "< Computing descriptors for keypoints from first image..." << endl;
  Mat descriptors1;
  descriptorExtractor->compute(image, keypoints1, descriptors1);
  cout << ">" << endl;

  namedWindow("keypoints", 0);
  Mat drawImg;
  drawKeypoints(image, keypoints1, drawImg, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  imshow("keypoints", drawImg);
  waitKey(0);

  cout << endl << "< Extracting keypoints from second image..." << endl;
  vector<KeyPoint> keypoints2;
  detector->detect(scene, keypoints2);
  cout << keypoints2.size() << " points" << endl << ">" << endl;

  cout << "< Computing descriptors for keypoints from second image..." << endl;
  Mat descriptors2;
  descriptorExtractor->compute(scene, keypoints2, descriptors2);
  cout << ">" << endl;

  drawKeypoints(scene, keypoints2, drawImg, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  imshow("keypoints", drawImg);
  waitKey(0);

  vector<DMatch> matches;
  //crossCheckMatching(descriptorMatcher, descriptors1, descriptors2, matches, 1);
  ratioTestMatching(descriptorMatcher, descriptors1, descriptors2, matches, 0.8);
  namedWindow("matches", 0);
  drawMatches(image, keypoints1, scene, keypoints2, matches, drawImg, Scalar(0, 255, 0), Scalar(0, 0, 255), Mat(),
              DrawMatchesFlags::DEFAULT);
  imshow("matches", drawImg);
  waitKey(0);

  Point center(image.cols / 2, image.rows / 2);
  Mat hgt(scene.rows/splitSet, scene.cols/splitSet, CV_32S, Scalar::all(0));

  for (int matchInd = 0; matchInd < matches.size(); matchInd++)
  {

  }
  Point maxP;
  minMaxLoc(hgt, 0, 0, 0, &maxP, Mat());
  Point resultCenter(maxP.x*splitSet, maxP.y*splitSet);
  namedWindow("result", 0);
  drawX(resultCenter, scene);
  imshow("result", scene);
  waitKey(0);
  return 0;
}
