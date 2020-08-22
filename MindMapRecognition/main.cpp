// ConsoleApplication7.cpp : Ётот файл содержит функцию "main". «десь начинаетс€ и заканчиваетс€ выполнение программы.
//


#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

#include "Characteristic.h"

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
    //imageChange
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
        vector<vector<Point> > contours2;
        contours2.push_back(mapVector);

        int mapHeight = src_gray.size().height;
        int mapWidth = src_gray.size().width;


        Mat mapMat(src_gray.size(), CV_8UC3,Scalar(255,255,255));

        fillPoly(mapMat, contours2, Scalar(0, 0, 0));

        namedWindow("MapAttempt");
        imshow("MapAttempt", mapMat);


        vector<vector<Characteristic>> characteristicMatrix(mapHeight,
            vector<Characteristic>(mapWidth, Characteristic(0,0,0,0)));
        
        //вычисление вертикальной характеристики
        int val = 0;
        int u = 0;
        cout << mapWidth << endl;
        cout << mapHeight << endl;
        cout << mapMat.size().width;

        for (int c = 0; c < mapWidth; ++c) {
             //дл€ первого р€да
            if (mapMat.at<Vec3b>(0, c) == Vec3b(0, 0, 0)) {
                characteristicMatrix[0][c].setVertical(1);
            }

            //дл€ средней части
            for (int r = 1; r < mapHeight - 1; ++r) {
                if (mapMat.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                    val = characteristicMatrix[r - 1][c].getVertical() + 1;
                    characteristicMatrix[r][c].setVertical(val);

                    if (mapMat.at<Vec3b>(r + 1, c) == Vec3b(255, 255, 255)) {
                        u = r-1;

                        while (mapMat.at<Vec3b>(u, c) == Vec3b(0, 0, 0)) {
                            characteristicMatrix[u][c].setVertical(val);
                            --u;
                        }
                    }
                }
            }

            //дл€ последнего р€да
            if (mapMat.at<Vec3b>(mapHeight - 1, c) == Vec3b(0, 0, 0)) {
                val = characteristicMatrix[mapHeight - 2][c].getVertical() + 1;
                characteristicMatrix[mapHeight - 1][c].setVertical(val);
                
                u = mapHeight - 2;

                while (mapMat.at<Vec3b>(u, c) == Vec3b(0, 0, 0)) {
                    characteristicMatrix[u][c].setVertical(val);
                    --u;
                }

            }
        }
        
        //вычисление горизонтальной характеристики
        for (int r = 0; r < mapHeight; ++r) {

            // дл€ первого столбца
            if (mapMat.at<Vec3b>(r, 0) == Vec3b(0, 0, 0)) {
                characteristicMatrix[r][0].setHorizontal(1);
            }

            //дл€ средней части
            for (int c = 1; c < mapWidth - 1; ++c) {
                if (mapMat.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                    val = characteristicMatrix[r][c-1].getHorizontal() + 1;
                    characteristicMatrix[r][c].setHorizontal(val);

                    if (mapMat.at<Vec3b>(r, c+1) == Vec3b(255, 255, 255)) {
                        u = c-1;

                        while (mapMat.at<Vec3b>(r, u) == Vec3b(0, 0, 0)) {
                            characteristicMatrix[r][u].setHorizontal(val);
                            --u;
                        }
                    }
                }
            }

            //дл€ последнего столбца
            if (mapMat.at<Vec3b>(r, mapWidth - 1) == Vec3b(0, 0, 0)) {
                val = characteristicMatrix[r][mapWidth -2 ].getHorizontal() + 1;
                characteristicMatrix[r][mapWidth - 1].setHorizontal(val);

                u = mapWidth - 2;

                while (mapMat.at<Vec3b>(r, u) == Vec3b(0, 0, 0)) {
                    characteristicMatrix[r][u].setHorizontal(val);
                    --u;
                }

            }
        }


        int r1 = 0, c1 = 0;

        //вычисление диагональных характеристик
        // обработка первого столбца
        for (int r = 0; r < mapHeight; ++r) {
            if (mapMat.at<Vec3b>(r, 0) == Vec3b(0, 0, 0)) {
                characteristicMatrix[r][0].setMainDiagonal(1);
                characteristicMatrix[r][0].setSideDiagonal(1);
            }
        }

        for (int c = 1; c < mapWidth; ++c) {

            // обработка первой строки
            if (mapMat.at<Vec3b>(0, c) == Vec3b(0, 0, 0)) {
                characteristicMatrix[0][c].setMainDiagonal(1);
                
                val = characteristicMatrix[1][c-1].getSideDiagonal() + 1;
                characteristicMatrix[0][c].setSideDiagonal(val);

                r1 = 1;
                c1 = c - 1;

                while (mapMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
                    characteristicMatrix[r1][c1].setSideDiagonal(val);
                    ++r1;
                    --c1;
                }

            }

            // средн€€ часть

            for (int r = 1; r < mapHeight - 1; ++r) {
                if (mapMat.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                    r1 = r - 1;
                    c1 = c - 1;
                    val = characteristicMatrix[r1][c1].getMainDiagonal()+1; 
                    characteristicMatrix[r][c].setMainDiagonal(val);

                    if (mapMat.at<Vec3b>(r+1, c + 1) == Vec3b(255, 255, 255)) {
                        while (mapMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
                            characteristicMatrix[r1][c1].setMainDiagonal(val);
                            --r1;
                            --c1;
                        }
                    }

                    r1 = r + 1;
                    c1 = c - 1;
                    val = characteristicMatrix[r1][c1].getSideDiagonal() + 1;
                    characteristicMatrix[r][c].setSideDiagonal(val);

                    if (mapMat.at<Vec3b>(r - 1, c + 1) == Vec3b(255, 255, 255)) {
                        while (mapMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
                            characteristicMatrix[r1][c1].setSideDiagonal(val);
                            ++r1;
                            --c1;
                        }
                    }
                }
            }

            // нижний р€д
            if (mapMat.at<Vec3b>(mapHeight-1, c) == Vec3b(0, 0, 0)) {
                characteristicMatrix[mapHeight - 1][c].setSideDiagonal(1);

                val = characteristicMatrix[mapHeight - 2][c-1].getMainDiagonal() + 1;
                characteristicMatrix[mapHeight - 1][c].setMainDiagonal(val);

                r1 = mapHeight - 2;
                c1 = c-1;

                while (mapMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
                    characteristicMatrix[r1][c1].setMainDiagonal(val);
                    --r1;
                    --c1;
                }

            }

        }

        // длины с пор€дком 3 всех точек
        int maxLength = 0;
        vector<int> lenghtFor3DirArr;
        vector<Point> mapPoints;

        
        for (int r = 0; r < mapHeight; ++r) {
            for (int c = 0; c < mapWidth; ++c) {
                if (mapMat.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                    mapPoints.push_back(Point(c, r));

                    val = characteristicMatrix[r][c].getCharacteristicByOrder(2);
                    lenghtFor3DirArr.push_back(val);

                    maxLength = std::max(maxLength, val);
                }
            }
        }

        Mat mapMat2(src_gray.size(), CV_8UC3, Scalar(255, 255, 255));

        const double num1 = 255.0 / maxLength;

        for (int i = 0; i < mapPoints.size(); ++i) {
            val = num1 * lenghtFor3DirArr[i];
            //cout << "val: " << val;
            
            //cout << mapPoints[i] << endl;
            if (val < 20) {
                mapMat2.at<Vec3b>(mapPoints[i]) = Vec3b(0, 255, 0);

            }
            else
            {
                mapMat2.at<Vec3b>(mapPoints[i]) = Vec3b(val, val, val);

            }
        }

        vector<int> lengthDistribution(maxLength+1,0);
        for (int i = 0; i < lenghtFor3DirArr.size(); ++i) {
            lengthDistribution[lenghtFor3DirArr[i]]++;
        }

        cout << endl;
        for (int i = 0; i < lengthDistribution.size(); ++i) {
            cout << "num of " << i << " = " << lengthDistribution[i] << endl;
        }

        namedWindow("MapAttempt1");
        imshow("MapAttempt1", mapMat2);
    }
}


