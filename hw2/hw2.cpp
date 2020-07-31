#include <opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <bits/stdc++.h>
using namespace std;
using namespace cv;

void drawCorners(Mat& src, Mat& response)
{
	auto it = response.begin<uchar>();
	auto it_end = response.end<uchar>();
	for (int i = 0; it != it_end; i++, it++){
		if (*it){
			circle(src, Point(i%src.cols, i / src.cols), 1, Scalar(0, 0, 255), 1);
		}
	}
}
Mat convert(Mat &src){
    Mat gray_pic;
    cvtColor(src, gray_pic, CV_BGR2GRAY);
    return gray_pic;
}

Mat diff(Mat &src, int para){
    Mat res;
    if(para == 0){
        Sobel(src, res, CV_32FC1, 1, 0, 3, BORDER_DEFAULT);
    }else{
        Sobel(src, res, CV_32FC1, 0, 1, 3, BORDER_DEFAULT);
    }
    return res;
}
int main(){
    //read pic
    Mat src = imread("./pic/horse.png");
    // convert RGB to gray
    Mat gray_pic = convert(src);
    // compute diff
    Mat dx = diff(src, 0);
    Mat dy = diff(src, 1);

    Mat dx2, dy2, dxdy;
    multiply(dx, dx, dx2);
    multiply(dy, dy, dy2);
    multiply(dx, dy, dxdy);

    // using Gaussian window, wsize = 3
    GaussianBlur(dx2, dx2, Size(3, 3), 2.0);
    GaussianBlur(dy2, dy2, Size(3, 3), 2.0);
    GaussianBlur(dxdy, dxdy, Size(3, 3), 2.0);

    //compute R
    Mat detM, traceM, tmp1, tmp2;
    multiply(dx2, dy2, tmp1);
    multiply(dxdy, dxdy, tmp2);
    detM = tmp1 - tmp2;
    traceM = dx2 + dy2;

    float k = 0.05;
    Mat traceM2;
    multiply(traceM, traceM, traceM2);
    Mat R = detM - k * traceM2;

    double maxVal, minVal;
	minMaxLoc(R, NULL,&maxVal, NULL, NULL);
    Mat dstImage = R > 0.005 * maxVal;
    // draw corners
    Mat draw_pic = src.clone();
    drawCorners(draw_pic, dstImage);

    // compute eig
    Mat l_eig, s_eig, sqTmp;
    Mat tmp = traceM2 - 4 * detM;
    sqrt(tmp, sqTmp);
    l_eig = 0.5 * (traceM + sqTmp);
    s_eig = 0.5 * (traceM - sqTmp);

    minMaxLoc(l_eig, NULL, &maxVal, NULL, NULL);
    l_eig = l_eig > 0.01 * maxVal;
    minMaxLoc(s_eig, NULL, &maxVal, NULL, NULL);
    s_eig = s_eig > 0.01 * maxVal;

    // print and save res
    imshow("original picture", src);
    imwrite("./result/origin_pic.png", src);
    imshow("R", dstImage);
    imwrite("./result/R.png", dstImage);
    imshow("l_eig", l_eig);
    imwrite("./result/l_eig.png", l_eig);
    imshow("s_eig", s_eig);
    imwrite("./result/s_eig.png", s_eig);
    imshow("drawed pic", draw_pic);
    imwrite("./result/draw_pic.png", draw_pic);

    waitKey();

    return 0;
}
