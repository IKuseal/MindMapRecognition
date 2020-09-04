// ConsoleApplication7.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>

#include "Characteristic.h"
#include "Neighbors.h"
#include "myNode.h"

using namespace cv;
using namespace std;

Mat src_gray;
int thresh = 100;
RNG rng(12345);
void thresh_callback(int, void*);


//stages
std::string binarizationStageSign = "_binarization";
std::string binarizationStagePath = "";

std::string findingContoursStageSign = "_contour";
std::string findingContoursStagePath = "";

std::string identificationStageSign1 = "_identification1";
std::string identificationStagePath1 = "";

std::string identificationStageSign2 = "_identification2";
std::string identificationStagePath2 = "";

std::string extractionStageSign = "_extraction";
std::string extractionStagePath = "";

std::string basePath = "C:/Users/Kuseal/Downloads/imgs/img_";
std::string version = "_pv_1";
std::string extensionSign = ".png";
std::string imagePath = "";



vector<vector<Point> > nodesContours;
vector<Vec4i> nodesContoursHierarchy;

vector<vector<Point> > linesContours;
vector<Vec4i> linesContoursHierarchy;


Mat chooseCentralNodeImage;
Mat src;

static void drawPolygon(Mat & source, vector<Point> & polygon, Vec3b color) {
    for (int i = 0; i < polygon.size(); ++i) {

        source.at<Vec3b>(polygon.at(i)) = color;
    }
}

static void findPolygons(Mat & source, vector<vector<Point>> & outputPolygons) {
    vector<Point> polygon, processedArr;
    Point processedPoint, tempPoint;
    int x, y;

    for (int c = 0; c < source.size().width; ++c) {

        for (int r = 0; r < source.size().height; ++r) {

            if (source.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                polygon = vector<Point>();

                processedArr.push_back(Point(c, r));
                polygon.push_back(Point(c, r));

                source.at<Vec3b>(r, c) = Vec3b(255, 255, 255);
                
                while (!processedArr.empty()) {
                    processedPoint = processedArr.back();
                    processedArr.pop_back();

                    x = processedPoint.x;
                    y = processedPoint.y;

                    tempPoint = Point(x - 1, y - 1);

                    if (source.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                        processedArr.push_back(tempPoint);
                        source.at<Vec3b>(tempPoint) = Vec3b(255, 255, 255);
                        polygon.push_back(tempPoint);
                    }

                    tempPoint = Point(x - 1, y);

                    if (source.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                        processedArr.push_back(tempPoint);
                        source.at<Vec3b>(tempPoint) = Vec3b(255, 255, 255);
                        polygon.push_back(tempPoint);
                    }

                    tempPoint = Point(x - 1, y +1);

                    if (source.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                        processedArr.push_back(tempPoint);
                        source.at<Vec3b>(tempPoint) = Vec3b(255, 255, 255);
                        polygon.push_back(tempPoint);
                    }

                    tempPoint = Point(x, y + 1);

                    if (source.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                        processedArr.push_back(tempPoint);
                        source.at<Vec3b>(tempPoint) = Vec3b(255, 255, 255);
                        polygon.push_back(tempPoint);
                    }

                    tempPoint = Point(x + 1, y + 1);

                    if (source.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                        processedArr.push_back(tempPoint);
                        source.at<Vec3b>(tempPoint) = Vec3b(255, 255, 255);
                        polygon.push_back(tempPoint);
                    }

                    tempPoint = Point(x + 1, y);

                    if (source.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                        processedArr.push_back(tempPoint);
                        source.at<Vec3b>(tempPoint) = Vec3b(255, 255, 255);
                        polygon.push_back(tempPoint);
                    }

                    tempPoint = Point(x + 1, y - 1);

                    if (source.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                        processedArr.push_back(tempPoint);
                        source.at<Vec3b>(tempPoint) = Vec3b(255, 255, 255);
                        polygon.push_back(tempPoint);
                    }

                    tempPoint = Point(x, y - 1);

                    if (source.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                        processedArr.push_back(tempPoint);
                        source.at<Vec3b>(tempPoint) = Vec3b(255, 255, 255);
                        polygon.push_back(tempPoint);
                    }
                }

                outputPolygons.push_back(polygon);
            }
        }
    }
}

static double calcDistBetweenPoints(Point2f p1, Point2f p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}
static double calcDistBetweenContours(vector<Point> &con1, vector <Point> &con2) {

    double min, dist;
    
    min = std::abs(pointPolygonTest(con2, con1[0], true));

    for (int i = 1; i < con1.size(); ++i) {
        dist = std::abs(pointPolygonTest(con2, con1[i], true));

        if (dist < min) {
            min = dist;
        }
    }

    return min;
}
static double myCalcDistBetweenContours(vector<Point> &con1, vector <Point> &con2) {

    double min, dist;

    min = calcDistBetweenPoints(con1[0], con2[0]);

    for (int i = 0; i < con1.size(); ++i) {
        for (int j = 0; j < con2.size(); ++j) {
            dist = calcDistBetweenPoints(con1[i], con2[j]);

            if (dist < min) {
                min = dist;
            }
        }        
    }

    return min;
}
static void extractMap(int centralNodeId) {
    // жёсткие костыли, потом исправить
    vector<int > remainingNodes(nodesContours.size());
    vector<int > remainingLines(linesContours.size());

    for (int i = 0; i < remainingNodes.size(); ++i) {
        remainingNodes[i] = i;
    }

    for (int i = 0; i < remainingLines.size(); ++i) {
        remainingLines[i] = i;
    }


    queue <int> processedNodes = queue <int>();
    queue <Scalar> colorsQueue;


    std::vector<int>::iterator iterator;
    // переменные цикла
    vector<Point> temp(1, Point(0, 0));
    vector<Point> nodeContour,nodeContour2, lineContour;
    //vector<Point> &nodeContour(temp), &lineContour(temp);
    vector<int> linesToErase;
    int nodeContourId(-1), nodeContourId2(-1), lineContourId(-1);
    double dist(-1), dist2(-1);
    Point connectionPoint;

    double minVal = 100;

    Scalar red(0, 0, 255);
    Scalar blue(255, 0, 0);
    Scalar color(red), color2(red);

    processedNodes.push(centralNodeId);
    colorsQueue.push(red);

    //drawContours(chooseCentralNodeImage, nodesContours, 12, color, 4, LINE_8, nodesContoursHierarchy, 0);

    while (!processedNodes.empty()) {
        nodeContourId = processedNodes.front();
        nodeContour = nodesContours[nodeContourId];
        processedNodes.pop();
        iterator = find(remainingNodes.begin(), remainingNodes.end(), nodeContourId);
        cout << "eraseNode1 = " << "id = " << nodeContourId << endl;
        remainingNodes.erase(iterator);
        cout << "eraseNode2" << endl;

        color = colorsQueue.front();
        colorsQueue.pop();

        if (color == red) {
            color2 = blue;
        }
        else color2 = red;

        drawContours(chooseCentralNodeImage, nodesContours, nodeContourId, color, 4, LINE_8, nodesContoursHierarchy, 0);
        //drawPolygon(chooseCentralNodeImage, nodesContours.at(nodeContourId), Vec3b(color[0],color[1],color[2]));

        for (int i = 0; i < remainingLines.size(); ++i) {

            lineContourId = remainingLines[i];
            lineContour = linesContours[lineContourId];

            dist = myCalcDistBetweenContours(nodeContour, lineContour);

            if (dist < 2) {

                for (int k = 0; k < remainingNodes.size(); ++k) {
                    nodeContourId2 = remainingNodes[k];
                    nodeContour2 = nodesContours[nodeContourId2];

                    dist2 = myCalcDistBetweenContours(lineContour, nodeContour2);


                    if (dist2 < 2) {

                        processedNodes.push(nodeContourId2);

                        colorsQueue.push(color2);

                        linesToErase.push_back(lineContourId);
                        break;
                    }

                }

            }
        }

        while (!linesToErase.empty()) {
            lineContourId = linesToErase.back();
            linesToErase.pop_back();


            iterator = find(remainingLines.begin(), remainingLines.end(), lineContourId);
            cout << "eraseLine1" << endl;
            remainingLines.erase(iterator);
            cout << "eraseLine2" << endl;

            drawContours(chooseCentralNodeImage, linesContours, lineContourId, color, 4, LINE_8, linesContoursHierarchy, 0);
            //drawPolygon(chooseCentralNodeImage, linesContours.at(lineContourId), Vec3b(color[0], color[1], color[2]));
        }

    }



    imwrite(extractionStagePath, chooseCentralNodeImage);

}

static void processImageWithNumber(int num) {

    imagePath = basePath + to_string(num) + extensionSign;
    binarizationStagePath = basePath + to_string(num) + binarizationStageSign + version + extensionSign;
    findingContoursStagePath = basePath + to_string(num) + findingContoursStageSign + version + extensionSign;
    identificationStagePath1 = basePath + to_string(num) + identificationStageSign1 + version + extensionSign;
    identificationStagePath2 = basePath + to_string(num) + identificationStageSign2 + version + extensionSign;
    extractionStagePath = basePath + to_string(num) + extractionStageSign + version + extensionSign;

    IplImage *image;
    src = imread(imagePath, IMREAD_COLOR); // Load an image

    imwrite(imagePath, src);

    if (src.empty())
    {
        cout << "Could not open or find the image!\n" << endl;
    }

    cvtColor(src, src_gray, COLOR_BGR2GRAY);


    threshold(src_gray, src_gray,
        230, 255,
        CV_THRESH_BINARY);

    imwrite(binarizationStagePath, src_gray);
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

static void imageToElectronicMapTransfer(int x, int y) {
    /// Calculate the distances to the contour
    
    int choosedNodeIndex = -1;
    Point clickPoint(x, y);
    
    double dist = std::abs(pointPolygonTest(nodesContours[0], clickPoint, true));
    choosedNodeIndex = 0;
    double minDist = dist;


    for (int i = 1; i < nodesContours.size(); ++i) {
        dist = std::abs(pointPolygonTest(nodesContours[i],clickPoint, true));

        if (dist < minDist) {
            minDist = dist;
            choosedNodeIndex = i;
        }
        
    }

    //drawContours(chooseCentralNodeImage, nodesContours, choosedNodeIndex, Scalar(255,0,0), 4, LINE_8, nodesContoursHierarchy, 0);

    extractMap(choosedNodeIndex);
}

static void mouseCallback(int event, int x, int y, int flags, void* param)
{
    switch (event) {
    case CV_EVENT_MOUSEMOVE:
        break;

    case CV_EVENT_LBUTTONDOWN:
        imageToElectronicMapTransfer(x,y);
        break;

    case CV_EVENT_LBUTTONUP:
        break;
    }

    return;
}



int main(int argc, char** argv)
{
  
    //imageChange
    int low = 6;
    int up = low;
    for (int i = low; i <= up; ++i) {
        cout << "i = " << i << endl;
        processImageWithNumber(i);
    }

    

    waitKey();
/*
    int a;
    cin >> a;*/
    return 0;
}

static int drawMapContours(vector<vector<Point>>& contours, vector<Vec4i>& hierarchy){
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

    //вернуть
    imwrite(findingContoursStagePath, drawing);

    return contourIndexWithMaxPoints;
}

static void calculateCharacteristic(Mat & source,vector<vector<Point>> &contours, vector<vector<Characteristic>> &characteristicMatrix, Mat & resultMat) {
    resultMat = Mat(source.size(), CV_8UC3, Scalar(255, 255, 255));


    int mapHeight = source.size().height;
    int mapWidth = source.size().width;

    int maxPoints = contours[0].size();
    int contourIndexWithMaxPoints = 0;

    for (int i = 1; i < contours.size(); ++i) {
        if (contours[i].size() > maxPoints) {
            maxPoints = contours[i].size();
            contourIndexWithMaxPoints = i;
        }
    }

    vector<Point> mapVector = contours[contourIndexWithMaxPoints];
    vector<vector<Point> > contours2;
    contours2.push_back(mapVector);

    fillPoly(resultMat, contours2, Scalar(0, 0, 0));

    /*namedWindow("MapAttempt");
    imshow("MapAttempt", mapMat);*/


    //вычисление вертикальной характеристики
    int val = 0;
    int u = 0;
    cout << mapWidth << endl;
    cout << mapHeight << endl;
    cout << resultMat.size().width;

    for (int c = 0; c < mapWidth; ++c) {
        //для первого ряда
        if (resultMat.at<Vec3b>(0, c) == Vec3b(0, 0, 0)) {
            characteristicMatrix[0][c].setVerticalPixLeng(1);
        }

        //для средней части
        for (int r = 1; r < mapHeight - 1; ++r) {
            if (resultMat.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                val = characteristicMatrix[r - 1][c].getVerticalPixLeng() + 1;
                characteristicMatrix[r][c].setVerticalPixLeng(val);

                if (resultMat.at<Vec3b>(r + 1, c) == Vec3b(255, 255, 255)) {
                    u = r - 1;

                    while (resultMat.at<Vec3b>(u, c) == Vec3b(0, 0, 0)) {
                        characteristicMatrix[u][c].setVerticalPixLeng(val);
                        --u;
                    }
                }
            }
        }

        //для последнего ряда
        if (resultMat.at<Vec3b>(mapHeight - 1, c) == Vec3b(0, 0, 0)) {
            val = characteristicMatrix[mapHeight - 2][c].getVerticalPixLeng() + 1;
            characteristicMatrix[mapHeight - 1][c].setVerticalPixLeng(val);

            u = mapHeight - 2;

            while (resultMat.at<Vec3b>(u, c) == Vec3b(0, 0, 0)) {
                characteristicMatrix[u][c].setVerticalPixLeng(val);
                --u;
            }

        }
    }

    //вычисление горизонтальной характеристики
    for (int r = 0; r < mapHeight; ++r) {

        // для первого столбца
        if (resultMat.at<Vec3b>(r, 0) == Vec3b(0, 0, 0)) {
            characteristicMatrix[r][0].setHorizontalPixLeng(1);
        }

        //для средней части
        for (int c = 1; c < mapWidth - 1; ++c) {
            if (resultMat.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                val = characteristicMatrix[r][c - 1].getHorizontalPixLeng() + 1;
                characteristicMatrix[r][c].setHorizontalPixLeng(val);

                if (resultMat.at<Vec3b>(r, c + 1) == Vec3b(255, 255, 255)) {
                    u = c - 1;

                    while (resultMat.at<Vec3b>(r, u) == Vec3b(0, 0, 0)) {
                        characteristicMatrix[r][u].setHorizontalPixLeng(val);
                        --u;
                    }
                }
            }
        }

        //для последнего столбца
        if (resultMat.at<Vec3b>(r, mapWidth - 1) == Vec3b(0, 0, 0)) {
            val = characteristicMatrix[r][mapWidth - 2].getHorizontalPixLeng() + 1;
            characteristicMatrix[r][mapWidth - 1].setHorizontalPixLeng(val);

            u = mapWidth - 2;

            while (resultMat.at<Vec3b>(r, u) == Vec3b(0, 0, 0)) {
                characteristicMatrix[r][u].setHorizontalPixLeng(val);
                --u;
            }

        }
    }


    int r1 = 0, c1 = 0;
    int r2 = 0, c2 = 0;

    ////вычисление диагональных характеристик
    //// обработка первого столбца
    //for (int r = 0; r < mapHeight; ++r) {
    //    if (mapMat.at<Vec3b>(r, 0) == Vec3b(0, 0, 0)) {
    //        characteristicMatrix[r][0].setMainDiagonalPixLeng(1);
    //        characteristicMatrix[r][0].setSideDiagonalPixLeng(1);
    //    }
    //}

    for (int c = 1; c < mapWidth; ++c) {

        //// обработка первой строки
        //if (mapMat.at<Vec3b>(0, c) == Vec3b(0, 0, 0)) {
        //    characteristicMatrix[0][c].setMainDiagonalPixLeng(1);
        //    
        //    val = characteristicMatrix[1][c-1].getSideDiagonalPixLeng() + 1;
        //    characteristicMatrix[0][c].setSideDiagonalPixLeng(val);

        //    r1 = 1;
        //    c1 = c - 1;

        //    while (mapMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
        //        characteristicMatrix[r1][c1].setSideDiagonalPixLeng(val);
        //        ++r1;
        //        --c1;
        //    }

        //}

        // средняя часть

        for (int r = 1; r < mapHeight - 1; ++r) {
            if (resultMat.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                r1 = r - 1;
                c1 = c - 1;
                val = characteristicMatrix[r1][c1].getMainDiagonalPixLeng() + 1;
                characteristicMatrix[r][c].setMainDiagonalPixLeng(val);

                if (resultMat.at<Vec3b>(r + 1, c + 1) == Vec3b(255, 255, 255)) {
                    while (resultMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
                        characteristicMatrix[r1][c1].setMainDiagonalPixLeng(val);
                        --r1;
                        --c1;
                    }
                }

                r1 = r + 1;
                c1 = c - 1;
                val = characteristicMatrix[r1][c1].getSideDiagonalPixLeng() + 1;
                characteristicMatrix[r][c].setSideDiagonalPixLeng(val);

                if (resultMat.at<Vec3b>(r - 1, c + 1) == Vec3b(255, 255, 255)) {
                    while (resultMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
                        characteristicMatrix[r1][c1].setSideDiagonalPixLeng(val);
                        ++r1;
                        --c1;
                    }
                }
            }
        }

        //// нижний ряд
        //if (mapMat.at<Vec3b>(mapHeight-1, c) == Vec3b(0, 0, 0)) {
        //    characteristicMatrix[mapHeight - 1][c].setSideDiagonalPixLeng(1);

        //    val = characteristicMatrix[mapHeight - 2][c-1].getMainDiagonalPixLeng() + 1;
        //    characteristicMatrix[mapHeight - 1][c].setMainDiagonalPixLeng(val);

        //    r1 = mapHeight - 2;
        //    c1 = c-1;

        //    while (mapMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
        //        characteristicMatrix[r1][c1].setMainDiagonalPixLeng(val);
        //        --r1;
        //        --c1;
        //    }

        //}

    }

    //вычисление дополнительных диагональных характеристик
    //для простоты пока что считаю, что у меня результат не зависит границы слоем 2 пикселя

    for (int c = 2; c < mapWidth - 2; ++c) {

        //средняя часть

        for (int r = 2; r < mapHeight - 2; ++r) {
            if (resultMat.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {

                //up main
                r1 = r - 1;
                r2 = r + 1;
                if (r % 2 == 0) {
                    c1 = c - 1;
                    c2 = c;
                }
                else {
                    c1 = c;
                    c2 = c + 1;
                }
                val = characteristicMatrix[r1][c1].getUpMainDiagonalPixLeng() + 1;
                characteristicMatrix[r][c].setUpMainDiagonalPixLeng(val);

                if (resultMat.at<Vec3b>(r2, c2) == Vec3b(255, 255, 255)) {
                    while (resultMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
                        characteristicMatrix[r1][c1].setUpMainDiagonalPixLeng(val);
                        if (r1 % 2 == 0) {
                            --c1;
                        }
                        --r1;
                    }
                }

                //down main
                c1 = c - 1;
                c2 = c + 1;
                if (c % 2 == 0) {
                    r1 = r - 1;
                    r2 = r;
                }
                else {
                    r1 = r;
                    r2 = r + 1;
                }
                val = characteristicMatrix[r1][c1].getDownMainDiagonalPixLeng() + 1;
                characteristicMatrix[r][c].setDownMainDiagonalPixLeng(val);

                if (resultMat.at<Vec3b>(r2, c2) == Vec3b(255, 255, 255)) {
                    while (resultMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
                        characteristicMatrix[r1][c1].setDownMainDiagonalPixLeng(val);
                        if (c1 % 2 == 0) {
                            --r1;
                        }
                        --c1;
                    }
                }

                //down side
                c1 = c - 1;
                c2 = c + 1;
                if (c % 2 == 0) {
                    r1 = r + 1;
                    r2 = r;
                }
                else {
                    r1 = r;
                    r2 = r - 1;
                }
                val = characteristicMatrix[r1][c1].getDownSideDiagonalPixLeng() + 1;
                characteristicMatrix[r][c].setDownSideDiagonalPixLeng(val);

                if (resultMat.at<Vec3b>(r2, c2) == Vec3b(255, 255, 255)) {
                    while (resultMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
                        characteristicMatrix[r1][c1].setDownSideDiagonalPixLeng(val);
                        if (c1 % 2 == 0) {
                            ++r1;
                        }
                        --c1;
                    }
                }

            }
        }

    }

    for (int c = 2; c < mapWidth - 2; ++c) {
        for (int r = mapHeight - 2; r > 2; --r) {
            if (resultMat.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                //up side
                r1 = r + 1;
                r2 = r - 1;
                if (r % 2 == 0) {
                    c1 = c - 1;
                    c2 = c;
                }
                else {
                    c1 = c;
                    c2 = c + 1;
                }
                val = characteristicMatrix[r1][c1].getUpSideDiagonalPixLeng() + 1;
                characteristicMatrix[r][c].setUpSideDiagonalPixLeng(val);

                if (resultMat.at<Vec3b>(r2, c2) == Vec3b(255, 255, 255)) {
                    while (resultMat.at<Vec3b>(r1, c1) == Vec3b(0, 0, 0)) {
                        characteristicMatrix[r1][c1].setUpSideDiagonalPixLeng(val);
                        if (r1 % 2 == 0) {
                            --c1;
                        }
                        ++r1;
                    }
                }
            }
        }
    }
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

        int contourIndexWithMaxPoints = drawMapContours(contours, hierarchy);

         //начинаем сегментацию


        int mapHeight = src_gray.size().height;
        int mapWidth = src_gray.size().width;

        vector<vector<Characteristic>> characteristicMatrix(mapHeight,
            vector<Characteristic>(mapWidth, Characteristic()));

        Mat mapMat;
        calculateCharacteristic(src_gray, contours, characteristicMatrix,mapMat);
        

        // длины с порядком 3 всех точек
        int maxLength = 0;
        vector<int> lenghtFor3DirArr;
        vector<Point> mapPoints;

        int val = 0;

        for (int r = 0; r < mapHeight; ++r) {
            for (int c = 0; c < mapWidth; ++c) {
                if (mapMat.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                    mapPoints.push_back(Point(c, r));

                    val = characteristicMatrix[r][c].getCharacteristicByOrder(7);
                    lenghtFor3DirArr.push_back(val);
  
                    maxLength = std::max(maxLength, val);
                }
            }
        }


        vector<int> lengthDistribution(maxLength+1,0);
        cout << endl << lenghtFor3DirArr.size();
        for (int i = 0; i < lenghtFor3DirArr.size(); ++i) {
            lengthDistribution[lenghtFor3DirArr[i]]++;
        }

        cout << endl;
        for (int i = 0; i < lengthDistribution.size(); ++i) {
            cout << "num of " << i << " = " << lengthDistribution[i] << endl;
        }

        cout << "MistakeAfter";
        // Работа с гистограммой и вычислени порогов
        int numOfStepsLeadership = 0;
        const int numOfStepsLeadersipRequirement = 15;
        int total = 0;
        const int summationLength = 4;

        int indexOfEndOfmaxValSeq = 0, maxVal = summationLength-1;
        
        for (int i = 0; i < summationLength; ++i) {
            total += lengthDistribution[i];
        }
        maxVal = total;

        int i1 = summationLength;
        while (numOfStepsLeadership != numOfStepsLeadersipRequirement) {
            total -= lengthDistribution[i1-summationLength];
            total += lengthDistribution[i1];
            
            if (total >= maxVal) {
                maxVal = total;
                indexOfEndOfmaxValSeq = i1;
                numOfStepsLeadership = 0;
            }
            else {
                numOfStepsLeadership++;
            }
            ++i1;
        }

        // визуализация
        Mat mapMat2(src_gray.size(), CV_8UC3, Scalar(255, 255, 255));

        //Mat forVizual(src_gray.size(), CV_8UC3, Scalar(255, 255, 255));


        //const double num1 = 255.0 / maxLength;

        //for (int i = 0; i < mapPoints.size(); ++i) {
        //    val = num1 * lenghtFor3DirArr[i];
        //    //cout << "val: " << val;

        //    //cout << mapPoints[i] << endl;
        //    if (val < 20) {
        //        forVizual.at<Vec3b>(mapPoints[i]) = Vec3b(0, 255, 0);

        //    }
        //    else
        //    {
        //        forVizual.at<Vec3b>(mapPoints[i]) = Vec3b(val, val, val);

        //    }
        //}

        //imshow("new", forVizual);

        // пороги
        cout <<endl << "indexOfEndOfmaxValSeq = " << indexOfEndOfmaxValSeq;
        for (int i = 0; i < mapPoints.size(); ++i) {
            
            if (lenghtFor3DirArr[i] <= indexOfEndOfmaxValSeq) {
                mapMat2.at<Vec3b>(mapPoints[i]) = Vec3b(0, 255, 0);

            }
            else if(lenghtFor3DirArr[i] <= 2* indexOfEndOfmaxValSeq)
            {
                mapMat2.at<Vec3b>(mapPoints[i]) = Vec3b(0, 0, 255);
            }
            else {
                mapMat2.at<Vec3b>(mapPoints[i]) = Vec3b(255, 0, 0);

            }
        }

        //вернуть
        imwrite(identificationStagePath1, mapMat2);

        //namedWindow("MapAttempt1");
        //imshow("MapAttempt1", mapMat2);

        Mat mapMat3 = mapMat2.clone();
        vector<vector<Point> > uncertainties;
        vector<vector<Point>>  uncertaintiesNeighbors;

        for (int c = 0; c < mapWidth; ++c) {
            for (int r = 0; r < mapHeight; ++r) {

                if (mapMat3.at<Vec3b>(r, c) == Vec3b(0, 0, 255)) {
                    mapMat3.at<Vec3b>(r,c) = Vec3b(0, 0, 0);
                }
                else {
                    mapMat3.at<Vec3b>(r, c) = Vec3b(255, 255, 255);
                }
            }
        }

        /*namedWindow("MapAttempt3");
        imshow("MapAttempt3", mapMat2);*/

        vector<Point> processedArr;
        vector<Point> resultUncertanties;
        vector<Point> resultUncertantiesNeigh;

        Point processedPoint(-1, 0), tempPoint(0,0);
        int x=-1, y=-1;

        // вычленяем зоны неопределённости и их соседей

        for (int c = 0; c < mapWidth; ++c) {
            for (int r = 0; r < mapHeight; ++r) {
                if (mapMat3.at<Vec3b>(r, c) == Vec3b(0, 0, 0)) {
                    resultUncertanties = vector<Point>();
                    resultUncertantiesNeigh = vector<Point>();

                    processedArr.push_back(Point(c, r));
                    resultUncertanties.push_back(Point(c, r));

                    mapMat3.at<Vec3b>(r, c) = Vec3b(250, 250, 250);

                    while (!processedArr.empty()) {

                        processedPoint = processedArr.back();
                        processedArr.pop_back();

                        x = processedPoint.x;
                        y = processedPoint.y;

                        /*tempPoint = Point(x - 1, y - 1);

                        if (mapMat3.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                            processedArr.push_back(tempPoint);
                            mapMat3.at<Vec3b>(tempPoint) = Vec3b(250, 250, 250);
                            resultUncertanties.push_back(tempPoint);
                        }
                        else if (mapMat3.at<Vec3b>(tempPoint) != Vec3b(250, 250, 250)) {
                            resultUncertantiesNeigh.push_back(tempPoint);
                        }*/

                        tempPoint = Point(x - 1, y);

                        if (mapMat3.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                            processedArr.push_back(tempPoint);
                            mapMat3.at<Vec3b>(tempPoint) = Vec3b(250, 250, 250);
                            resultUncertanties.push_back(tempPoint);
                        }
                        else if (mapMat3.at<Vec3b>(tempPoint) != Vec3b(250, 250, 250)) {
                            resultUncertantiesNeigh.push_back(tempPoint);
                        }

                        /*tempPoint = Point(x - 1, y +1);

                        if (mapMat3.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                            processedArr.push_back(tempPoint);
                            mapMat3.at<Vec3b>(tempPoint) = Vec3b(250, 250, 250);
                            resultUncertanties.push_back(tempPoint);
                        }
                        else if (mapMat3.at<Vec3b>(tempPoint) != Vec3b(250, 250, 250)) {
                            resultUncertantiesNeigh.push_back(tempPoint);
                        }*/

                        tempPoint = Point(x , y + 1);

                        if (mapMat3.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                            processedArr.push_back(tempPoint);
                            mapMat3.at<Vec3b>(tempPoint) = Vec3b(250, 250, 250);
                            resultUncertanties.push_back(tempPoint);
                        }
                        else if (mapMat3.at<Vec3b>(tempPoint) != Vec3b(250, 250, 250)) {
                            resultUncertantiesNeigh.push_back(tempPoint);
                        }

                        /*tempPoint = Point(x + 1, y + 1);

                        if (mapMat3.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                            processedArr.push_back(tempPoint);
                            mapMat3.at<Vec3b>(tempPoint) = Vec3b(250, 250, 250);
                            resultUncertanties.push_back(tempPoint);
                        }
                        else if (mapMat3.at<Vec3b>(tempPoint) != Vec3b(250, 250, 250)) {
                            resultUncertantiesNeigh.push_back(tempPoint);
                        }*/

                        tempPoint = Point(x + 1, y );

                        if (mapMat3.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                            processedArr.push_back(tempPoint);
                            mapMat3.at<Vec3b>(tempPoint) = Vec3b(250, 250, 250);
                            resultUncertanties.push_back(tempPoint);
                        }
                        else if (mapMat3.at<Vec3b>(tempPoint) != Vec3b(250, 250, 250)) {
                            resultUncertantiesNeigh.push_back(tempPoint);
                        }

                        /*tempPoint = Point(x + 1, y - 1);

                        if (mapMat3.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                            processedArr.push_back(tempPoint);
                            mapMat3.at<Vec3b>(tempPoint) = Vec3b(250, 250, 250);
                            resultUncertanties.push_back(tempPoint);
                        }
                        else if (mapMat3.at<Vec3b>(tempPoint) != Vec3b(250, 250, 250)) {
                            resultUncertantiesNeigh.push_back(tempPoint);
                        }*/

                        tempPoint = Point(x, y - 1);

                        if (mapMat3.at<Vec3b>(tempPoint) == Vec3b(0, 0, 0)) {
                            processedArr.push_back(tempPoint);
                            mapMat3.at<Vec3b>(tempPoint) = Vec3b(250, 250, 250);
                            resultUncertanties.push_back(tempPoint);
                        }
                        else if(mapMat3.at<Vec3b>(tempPoint) != Vec3b(250, 250, 250)) {
                            resultUncertantiesNeigh.push_back(tempPoint);
                        }
                    }
                    uncertainties.push_back(resultUncertanties);
                    uncertaintiesNeighbors.push_back(resultUncertantiesNeigh);
                }
            }
        }

        vector<NeighborsCharacteristic> uncertantiesNeighborsCharact;
        NeighborsCharacteristic procNeighCharact;

        //смотрим, какие соседи

        for (int i = 0; i < uncertainties.size(); ++i) {
            procNeighCharact = NeighborsCharacteristic();
            processedArr = uncertaintiesNeighbors.at(i);
            
            for (int j = 0; j < processedArr.size(); ++j) {
                tempPoint = processedArr.at(j);

                if (mapMat2.at<Vec3b>(tempPoint) == Vec3b(255, 0, 0)) {
                    procNeighCharact.setIsNodeNeighbor(true);
                }
                else if (mapMat2.at<Vec3b>(tempPoint) == Vec3b(0, 255, 0)) {
                    procNeighCharact.setIsLineNeighbor(true);
                }

                if (procNeighCharact.getIsNodeNeighbor() && procNeighCharact.getIsLineNeighbor()) break;
            }
            uncertantiesNeighborsCharact.push_back(procNeighCharact);
        }


        // разрешаем неопределённости
        Vec3b color2;
        for (int i = 0; i< uncertainties.size(); ++i) {
            procNeighCharact = uncertantiesNeighborsCharact.at(i);
            processedArr = uncertainties.at(i);

            if (procNeighCharact.getIsLineNeighbor() && procNeighCharact.getIsNodeNeighbor()) {
                color2 = Vec3b(255, 0, 0);
            }
            else if(procNeighCharact.getIsLineNeighbor()) {
                color2 = Vec3b(0, 255, 0);
            }
            else if(procNeighCharact.getIsNodeNeighbor()){
                color2 = Vec3b(255, 0, 0);
            }

            for (int j = 0; j < processedArr.size(); ++j) {
                tempPoint = processedArr.at(j);

                mapMat2.at<Vec3b>(tempPoint) = color2;
            }
        }

        imwrite(identificationStagePath2, mapMat2);

        Mat matForFindingLines = mapMat2.clone();
        Mat matForFindingNodes = mapMat2.clone();
        // перекрашиваем, оставляем только узлы
        for (int c = 0; c < mapWidth; ++c) {
            for (int r = 0; r < mapHeight; ++r) {

                if (matForFindingNodes.at<Vec3b>(r, c) == Vec3b(255, 0, 0)) {
                    matForFindingNodes.at<Vec3b>(r, c) = Vec3b(0, 0, 0);
                }
                else {
                    matForFindingNodes.at<Vec3b>(r, c) = Vec3b(255, 255, 255);
                }
            }
        }
        
        Mat gMatForFindingNodes;
        cvtColor(matForFindingNodes, gMatForFindingNodes, COLOR_BGR2GRAY);

        findContours(gMatForFindingNodes, nodesContours, nodesContoursHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
        //findPolygons(matForFindingNodes, nodesContours);

        /*Mat matVis(matForFindingNodes.size(), CV_8UC3, Scalar(255, 255, 255));

        for (int i = 0; i < nodesContours.size(); ++i) {
            drawPolygon(matVis, nodesContours.at(i), Vec3b(0, 0, 0));
        }

        imshow("nodes", matVis);*/
        
        cout << endl << "findPolygonsNodesFinished";
        // перекрашиваем, оставляем только линии
        for (int c = 0; c < mapWidth; ++c) {
            for (int r = 0; r < mapHeight; ++r) {

                if (matForFindingLines.at<Vec3b>(r, c) == Vec3b(0, 255, 0)) {
                    matForFindingLines.at<Vec3b>(r, c) = Vec3b(0, 0, 0);
                }
                else {
                    matForFindingLines.at<Vec3b>(r, c) = Vec3b(255, 255, 255);
                }
            }
        }

        Mat gMatForFindingLines;
        cvtColor(matForFindingLines, gMatForFindingLines, COLOR_BGR2GRAY);

        findContours(gMatForFindingLines, linesContours, linesContoursHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
        //findPolygons(matForFindingLines, linesContours);
        cout << endl << "findLinesFinished";

        // открываем окно для выбора центральньго цзла
        chooseCentralNodeImage = src.clone();

        /*for (int i = 0; i < nodesContours.size(); ++i) {
            drawContours(chooseCentralNodeImage, nodesContours, i, Scalar(0,0,255), 3, LINE_8, nodesContoursHierarchy, 0);
        }*/

        namedWindow("chooseCentralNodeWindow", 1);

        cvSetMouseCallback("chooseCentralNodeWindow", mouseCallback,&chooseCentralNodeImage);


        //imshow("chooseCentralNodeWindow", chooseCentralNodeImage);

        char key = 0;
        while ((int)key != 27) {
            imshow("chooseCentralNodeWindow", chooseCentralNodeImage);
            key = waitKey(1);
        }
    }
}



