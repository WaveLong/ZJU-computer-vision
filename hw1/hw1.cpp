#include <opencv.hpp>
#include <bits/stdc++.h>
using namespace std;
using namespace cv;
//�Ƿ�¼�ƣ��Ƿ���
bool recordFlag = false, drawingFlag = false;
//��ǰ���˶��ٸ�¼�Ƶ���Ƶ����������
int cnt_vedio = 0;
//�洢�������λ��
vector<vector<Point> >line_list;
//��ȡ��ǰʱ�䲢ת���ַ���
string  getCurrentTimeStr(){
	time_t t = time(NULL);
	char ch[64] = {0};
	strftime(ch, sizeof(ch) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));     //��-��-�� ʱ-��-��
	return ch;
}
//��ͼ
void put_pic(Mat src, Mat dst, int x, int y){
//    Mat mask = src.clone();
//    cvtColor(mask, mask, CV_RGB2GRAY);
    Mat imageROI = dst(Rect(x, y, src.cols, src.rows));
    src.copyTo(imageROI);
}
//��������
void put_text(Mat dst, const string text, Point pos,//
               int font = FONT_HERSHEY_COMPLEX_SMALL, Scalar color = cvScalar(200,200,250)){
    putText(dst, text, pos, font, 1.0, color, 1, CV_AA);
}
//�����Ӧ����
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
//����
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

    //��ʼ������ͷ��Ϣ
    VideoWriter writer;
    bool isColor = true;
    int frameFps = 20;
    int frameWidth = 640;
    int frameHeight = 480;
    Size finalSize = Size(frameWidth,frameHeight);
    //������ͷ
    VideoCapture capture(0);
	if (!capture.isOpened()){
        cout << "open camera failed!" << endl;
		return -1;
	}
    //���ô�����Ϣ
    namedWindow("����1");
	setMouseCallback("����1", on_mouse, 0);
	while(1){
        //��ȡһ֡
        Mat frame, frameResized;
        capture >> frame;
        resize(frame, frameResized, finalSize);
        //����Ƭ
        put_pic(photo, frameResized, 540, 0);
        //��������
        string t = getCurrentTimeStr();
        string text = "21921164 wangxin " + t;
        put_text(frameResized, text, cvPoint(55,460));

        //�������ͼ��
        draw_line(frameResized, line_list);
        //�ȴ���������
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
        //����¼��״̬ͼ��
        if(recordFlag){
            put_pic(run, frameResized, 0, 430);
            writer.write(frameResized);
        }else{
            put_pic(stop, frameResized, 0, 430);
        }
        //��ʾ
        imshow("����1", frameResized);
        waitKey(1);
	}
	capture.release();//�ͷ���Դ
	destroyAllWindows();
	return 0;
}
