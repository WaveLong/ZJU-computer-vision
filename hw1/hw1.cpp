#include <opencv.hpp>
#include <bits/stdc++.h>
using namespace std;
using namespace cv;
//是否录制，是否画线
bool recordFlag = false, drawingFlag = false;
//当前存了多少个录制的视频，用于命名
int cnt_vedio = 0;
//存储鼠标点击的位置
vector<vector<Point> >line_list;
//获取当前时间并转成字符串
string  getCurrentTimeStr(){
	time_t t = time(NULL);
	char ch[64] = {0};
	strftime(ch, sizeof(ch) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));     //年-月-日 时-分-秒
	return ch;
}
//贴图
void put_pic(Mat src, Mat dst, int x, int y){
//    Mat mask = src.clone();
//    cvtColor(mask, mask, CV_RGB2GRAY);
    Mat imageROI = dst(Rect(x, y, src.cols, src.rows));
    src.copyTo(imageROI);
}
//放置文字
void put_text(Mat dst, const string text, Point pos,//
               int font = FONT_HERSHEY_COMPLEX_SMALL, Scalar color = cvScalar(200,200,250)){
    putText(dst, text, pos, font, 1.0, color, 1, CV_AA);
}
//鼠标响应函数
void on_mouse(int event, int x, int y, int flags, void *){
    switch(event){
    case EVENT_LBUTTONDOWN:
        drawingFlag = true;
        line_list.push_back(vector<Point>{Point(x, y)});
        break;
    case EVENT_MOUSEMOVE:
        if(drawingFlag){
            line_list.back().push_back(Point(x, y));
        }
        break;
    case EVENT_LBUTTONUP:
        drawingFlag = false;
        break;
    }
}
//画线
void draw_line(Mat &img, vector<vector<Point> > &line_list){
    for(int i = 0; i < line_list.size(); ++i){
        for(int j = 1; j < line_list[i].size(); ++j){
            line(img, line_list[i][j-1], line_list[i][j], Scalar(0,0,255), 2, 5, 0);
        }
    }
}

int main(){
    Mat run = imread("run.png", 1);
    Mat stop = imread("stop.png", 1);
    Mat photo = imread("bighead.jpg", 1);
    resize(photo, photo, Size(100, 100));
    resize(run, run, Size(50, 50));
    resize(stop, stop, Size(50, 50));

    //初始化摄像头信息
    VideoWriter writer;
    bool isColor = true;
    int frameFps = 20;
    int frameWidth = 640;
    int frameHeight = 480;
    Size finalSize = Size(frameWidth,frameHeight);
    //打开摄像头
    VideoCapture capture(0);
	if (!capture.isOpened()){
        cout << "open camera failed!" << endl;
		return -1;
	}
    //设置窗口信息
    namedWindow("窗口1");
	setMouseCallback("窗口1", on_mouse, 0);
	while(1){
        //读取一帧
        Mat frame, frameResized;
        capture >> frame;
        resize(frame, frameResized, finalSize);
        //贴照片
        put_pic(photo, frameResized, 540, 0);
        //放置文字
        string t = getCurrentTimeStr();
        string text = "21921164 wangxin " + t;
        put_text(frameResized, text, cvPoint(55,460));

        //绘制鼠标图形
        draw_line(frameResized, line_list);
        //等待键盘命令
        char k = waitKey(1);
        if(k == 'q'){
            break;
        }else if(k == ' '){
            if(recordFlag){
                writer.release();
                recordFlag = false;
                cnt_vedio += 1;
            }else{
                recordFlag = true;
                writer = VideoWriter("output"+to_string(cnt_vedio) + ".avi", //
                                     CV_FOURCC('X', 'V', 'I', 'D'), frameFps, finalSize,isColor);
            }
        }else if(k == 'c'){
            line_list.clear();
        }
        //放置录制状态图标
        if(recordFlag){
            put_pic(run, frameResized, 0, 430);
            writer.write(frameResized);
        }else{
            put_pic(stop, frameResized, 0, 430);
        }
        //显示
        imshow("窗口1", frameResized);
        waitKey(1);
	}
	capture.release();//释放资源
	destroyAllWindows();
	return 0;
}
