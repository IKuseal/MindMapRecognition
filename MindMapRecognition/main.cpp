// ConsoleApplication7.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;
Mat src_gray;
int thresh = 100;
RNG rng(12345);
void thresh_callback(int, void*);

std::string basePath = "C:/Users/Kuseal/Downloads/img_";
std::string version = "_pv_1";
std::string contourSign = "_contour";
std::string extensionSign = ".png";
std::string imagePath = "";
std::string imageContourPath = "";

static void processImageWithNumber(int num) {
    imagePath = basePath + to_string(num) + extensionSign;
    imageContourPath = basePath + to_string(num) + contourSign + version + extensionSign;

    Mat src = imread(imagePath, IMREAD_COLOR); // Load an image

    if (src.empty())
    {
        cout << "Could not open or find the image!\n" << endl;
    }

    cvtColor(src, src_gray, COLOR_BGR2GRAY);


    threshold(src_gray, src_gray,
        230, 255,
        CV_THRESH_BINARY);
    /*adaptiveThreshold(src_gray, src_gray, 255, 1, 0, 1001,0);
    adaptiveThreshold(src_gray, src_gray, 255, 0, 0, 5, 0);*/


    /*const char* source_window1 = "Binary";
    namedWindow(source_window1);*/



    /*imshow(source_window1, src_gray);*/

    //blur(src_gray, src_gray, Size(9, 1));

    //const char* source_window = "Source";
    //namedWindow(source_window);
    //imshow(source_window, src);

    //const int max_thresh = 255;
    //createTrackbar("Canny thresh:", source_window, &thresh, max_thresh, thresh_callback);



    thresh_callback(0, 0);
    return;
}

int main(int argc, char** argv)
{
    for (int i = 46; i <= 46; ++i) {
        processImageWithNumber(i);
    }

    waitKey();
    return 0;
}

void thresh_callback(int, void*)
{
    const char* source_window2 = "Canny";
    //namedWindow(source_window2);

    //Mat canny_output;
    //Canny(src_gray, canny_output, thresh, thresh * 2);
    //imshow("Canny", canny_output);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    findContours(src_gray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    if (!contours.empty()) {
        Mat drawing = Mat::zeros(src_gray.size(), CV_8UC3);
        int maxPoints = contours[0].size();
        int contourIndexWithMaxPoints = 0;
        for (int i = 1; i < contours.size(); i++)
        {
            Scalar color = Scalar(rng.uniform(100, 256), rng.uniform(0, 256), rng.uniform(0, 256));
            if (contours[i].size() > maxPoints) {
                maxPoints = contours[i].size();
                drawContours(drawing, contours, contourIndexWithMaxPoints, color, 2, LINE_8, hierarchy, 0);
                contourIndexWithMaxPoints = i;
            }
            else {
                drawContours(drawing, contours, i, color, 2, LINE_8, hierarchy, 0);
            }
        }

        Scalar color = Scalar(60, 60, 60);
        drawContours(drawing, contours, contourIndexWithMaxPoints, color, 2, LINE_8, hierarchy, 0);

        //imshow("Contours", drawing);

        imwrite(imageContourPath, drawing);



        // начинаем сегментацию

        vector<Point> mapVector = contours[contourIndexWithMaxPoints];


    }

}


