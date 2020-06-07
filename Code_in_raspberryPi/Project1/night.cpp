//#include <memory.h>
//#include <time.h>
//#include <fstream>
//#include <stdlib.h>
//#include <string.h>
//#include <stdio.h>
//#include <sstream>
//#include <signal.h>
//#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <pylon/PylonIncludes.h>
//
//
//#ifdef PYLON_WIN_BUILD
//#    include <pylon/PylonGUI.h>
//#endif
//
///* Warning Disable */
//#pragma warning(disable:4996)
//
///* Namespace */
//using namespace cv;
//using namespace std;
//using namespace Pylon;
//
///* ---------------------------------------------------------------- */
//
//
//
///* --------------------------- function --------------------------- */
//Rect roi(Point center, Point start, Point end);
//void getCenter(Point start, Point end, Mat& cam);
//void areaOfRectangle(Point start, Point end, int iswidth);
//
//void drawRotatedRectangle(Mat& image, RotatedRect rect);
//
//void getContours(Mat& source);
//
//void drawAngle(Mat& source, int i, int downIndex, int leftIndex);
//
//void getHorizontalLine();
//
//void setLabel(Mat& image, string str, vector <Point> contour);
//
//void printResult(Mat& cam, RotatedRect& rect);
//
//int preProcessing(Mat& cam2);
//
//int capVideo(Mat* openCvImage);
//
//int getType();
//
//int calculateImage(Mat& cam, Mat& bin, RotatedRect& rect);
//
//double getAngle(Point start, Point end);
//
//bool getLineType(Point a, Point b);
//
//int convertToBinary(Mat& source, Mat& bin);
//
//Mat viewImg;
//Rect ViewRect;
//
///* ---------------------------------------------------------------- */
//
//
//
///* --------------------------- Global variable --------------------------- */
//
//RNG g_rng(12345);
//
//vector <vector<Point>> g_contours;
//vector <Vec4i> g_hierarchy;
//vector <Point2f> g_approx;
//vector <vector<Point>> g_approxxxxxxxxx;
//Point g_li[2][2];
//
//Mat cam(100, 100, CV_8UC1), bin(100, 100, CV_8UC1);
//
//int g_printWidthFlag = 0, g_printHeightFlag = 0;
//int errFlag = 0, runningFlag = 0;
//int u_angle = 0, u_circleArea = 0, u_rectangleArea = 0;  // 오차라고 인식할 값 기준을 담는 변수
//int u_circleAreaAverage = 0, u_circeError = 0;
//int g_bigCircleIndex = 0, g_smallCircleIndex = 0;
//
//double g_dwidthDistance = 0, g_dheightDistance = 0;
//double g_dmm = 0;
//double g_drealRectangleLength = 320;
//
//
//
///* 검출된 타겟의 외접 사각형을 그리는 함수 */
//void drawRotatedRectangle(Mat& image, RotatedRect rect) {
//	Point2f vertices[4];
//	rect.points(vertices);
//	for (int i = 0; i < 4; i++)
//		line(image, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255, 0), 2);
//}
//
//
///* 이진화된 이미지의 외곽선을 추출하고, 결과를 바탕으로 그 크기와 꼭짓점을 추출하는 함수 */
//void getContours(Mat& source) {  // 외곽선및 꼭지점 잡는 함수
//
//	int circle_area_average = 0;
//	int circle_big = 0, circle_small = 10000;
//	Point start, end;
//
//	findContours(source, g_contours, g_hierarchy, RETR_LIST, CHAIN_APPROX_NONE);  // 외곽선 찾기
//
//	g_approxxxxxxxxx.resize(g_contours.size());
//
//
//	for (int i = 0; i < g_contours.size(); i++) {
//		drawContours(source, g_contours, i, Scalar(0, 255, 255), 2, 8, g_hierarchy, 0, Point());  // 외곽선 그리기
//		approxPolyDP(Mat(g_contours[i]), g_approx, arcLength(Mat(g_contours[i]), true) * 0.01,
//			true);  // 꼭지점 따기1 (approx)
//		approxPolyDP(Mat(g_contours[i]), g_approxxxxxxxxx[i], arcLength(Mat(g_contours[i]), true) * 0.002,
//			true);  // 꼭지점 따기2 (approxxxxxxxxx)
//
//					//cout << "contour" << i << "'s center : " << g_ endl;
//		setLabel(source, to_string(i + 1), g_contours[i]);  // 원에 번호 매겨주기
//
//		RotatedRect rect = minAreaRect(g_approx);  // 외접 사각형 구하기
//		line(source, rect.center, rect.center, Scalar(0, 255, 0), 5);  // 구한 외접 사각형 중앙에 점찍기
//
//		if (i == 0) {
//			start = rect.center;
//			cout << start << endl;
//		}
//		else {
//			end = rect.center;
//			cout << end << endl;
//		}
//	}
//
//	getAngle(start, end);
//	getCenter(start, end, source);
//
//
//
//	//for (int i = 0; i < g_approx.size(); i++)
//	//   line(source, g_approx[i], g_approx[i], Scalar(255, 0, 255), 10);  // 꼭지점에 점찍기
//}
//
//
///* 오차 각도와 기준선을 결과 이미지에 표시해주는 함수 */
//void drawAngle(Mat& source, int i, int downIndex, int leftIndex) {
//
//	Point a = g_li[i][downIndex];
//	Point b = g_li[i][!downIndex];
//
//	line(source, Point(0, a.y), Point(source.size().width, a.y), Scalar(0, 0, 255), 2, LINE_AA);  // 기준선 그리기
//
//	if (a.x > b.x) {
//		line(source, a, b, Scalar(0, 0, 255), 2);  // 기존 선 하이라이트
//		putText(source, to_string(getAngle(a, b)), Point(a.x, b.y - 2), 1, 1, Scalar(255, 0, 0), 2);  // 각도 입력
//	}
//	else {
//		line(source, a, g_li[!i][leftIndex], Scalar(0, 0, 255), 2);  // 기존 선 하이라이트
//		putText(source, to_string(90 - getAngle(a, b)), Point(a.x, b.y - 2), 1, 1, Scalar(255, 0, 0), 2);  // 각도 입력
//	}
//}
//
//
///* 타겟의 픽셀 대비 실제 크기와 타겟의 가로, 세로 길이를 출력해주는 함수 */
//void areaOfRectangle(Point start, Point end, int iswidth) {
//
//	double dy = end.y - start.y;
//	double dx = end.x - start.x;
//	double mm;
//
//	if (iswidth) {  // 가로 선이라면 여기로
//		if (!g_printWidthFlag) {  // 매트릭스의 픽셀 대비 실제 크기를 구한다
//			g_dwidthDistance = sqrt((dx * dx) + (dy * dy));
//			g_dmm = g_drealRectangleLength / g_dwidthDistance;
//
//			cout << "Width distance : " << g_dwidthDistance << endl;
//			g_printWidthFlag = 1;
//		}
//	}
//	else {  // 세로 선이라면 여기로
//		if (!g_printHeightFlag) {
//			g_dheightDistance = sqrt((dx * dx) + (dy * dy));
//			mm = g_drealRectangleLength / g_dheightDistance;
//
//			cout << "Height distance : " << g_dheightDistance << endl;
//			g_printHeightFlag = 1;
//		}
//	}
//}
//
//
///* 사각형에서 가로선인지 아닌지를 판단하고, 가로선의 양 끝 좌표를 저장해주는 함수 */
//void getHorizontalLine() {
//	int j = 0;
//
//	for (int i = 1; i < g_approx.size(); i++)  // 가로선인지 구하기(1 ~ MAX-1)
//	{
//		if (getLineType(g_approx[i - 1], g_approx[i])) {  // 가로선인지 구하기(1 ~ MAX-1)
//			g_li[j][0] = g_approx[i];  // 가로선이면 저장
//			g_li[j][1] = g_approx[i - 1];  // 가로선이면 저장
//			j++;  // 인덱스 ++
//		}
//	}
//
//	if (getLineType(g_approx[0], g_approx[g_approx.size() - 1])) {  // 가로선인지 구하기(0, MAX)
//		g_li[j][0] = g_approx[0];  // 가로선이면 저장
//		g_li[j][1] = g_approx[g_approx.size() - 1];  // 가로선이면 저장
//	}
//}
//
//
///* 검출된 컨투어 객체를 받아오고, 결과 이미지에 몇 번째 객체인지 각 객체마다 라벨링 해주는 함수 */
//void setLabel(Mat& image, string str, vector <Point> contour) {
//	int fontface = FONT_HERSHEY_SIMPLEX;
//	int thickness = 1;
//	int baseline = 0;
//	double scale = 0.5;
//
//	Size text = getTextSize(str, fontface, scale, thickness, &baseline);
//	Rect r = boundingRect(contour);
//
//	Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
//	rectangle(image, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(200, 200, 200), FILLED);
//	putText(image, str, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
//}
//
//
///* 최종 결과를 콘솔로 출력해주는 함수 */
//void printResult(Mat& cam, RotatedRect& rect) {
//
//	cout << "center of rectangle : " << rect.center << endl;
//	cout << "circle count of rectangle : " << g_contours.size() - 1 << endl;
//	cout << "rectangle pixel area : " << g_dwidthDistance * g_dheightDistance << endl;
//	cout << "rectangle cm area : " << ((g_dwidthDistance * g_dmm) * (g_dheightDistance * g_dmm)) / 100 << endl;
//	cout << "1 pixel is " << g_dmm << "mm" << endl;
//
//	String textCircleArea = "Standard Circle Area : " + to_string(u_circleArea);
//	putText(cam, textCircleArea, Point(10, 20), 1, 1, Scalar(255, 0, 0), 0);
//	String textRectangleArea = "Standard Rectangle Area : " + to_string(u_rectangleArea);
//	putText(cam, textRectangleArea, Point(10, 40), 1, 1, Scalar(255, 0, 0), 0);
//	String textAngle = "Standard Angle : " + to_string(u_angle);
//	putText(cam, textAngle, Point(10, 60), 1, 1, Scalar(255, 0, 0), 0);
//
//}
//
///* 영상 전처리 함수 */
//int preProcessing(Mat& cam, Mat& copyCam1, Mat& copyCam2) {
//
//	// Grab한 영상 전처리
//	cout << "Getting Camera Image.." << endl;
//	if (!capVideo(&cam)) {  // 카메라가 정상적으로 없다면?
//		errFlag = 1;
//		return 1;
//	}
//	//imshow("original", cam2);
//	//cam2 = cam2(Rect(500, 400, 700, 550));
//	cvtColor(cam, cam, COLOR_GRAY2RGB);
//	cout << "Getting Camera Image Success" << endl << endl << endl << endl << endl << endl << endl << endl;
//
//
//	cam.copyTo(copyCam1);
//	cam.copyTo(copyCam2);
//
//	blur(copyCam1, copyCam1, Size(101, 101));
//	blur(copyCam2, copyCam2, Size(3, 3));
//	addWeighted(copyCam1, -1, copyCam2, 1, 128, cam);
//
//
//	return 0;
//}
//
//
///* Basler 카메라로부터 영상을 획득한 후 Mat형 이미지로 바꿔주는 함수 */
//int capVideo(Mat* openCvImage) {
//
//	try {
//		CGrabResultPtr ptrGrabResult;
//		CPylonImage image;
//		CImageFormatConverter fc;
//		CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
//
//		camera.MaxNumBuffer = 5;
//		camera.StartGrabbing(1);
//		fc.OutputPixelFormat = PixelType_Mono8;
//		fc.OutputBitAlignment = OutputBitAlignment_MsbAligned;
//
//
//		while (camera.IsGrabbing()) {
//
//			camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
//
//			if (ptrGrabResult->GrabSucceeded()) {
//				fc.Convert(image, ptrGrabResult);
//				const uint8_t* pImageBuffer = (uint8_t*)ptrGrabResult->GetBuffer();
//				*openCvImage = Mat(966, 1296, CV_8UC1, (uint32_t*)image.GetBuffer()).clone();
//
//
//#ifdef PYLON_WIN_BUILD
//				// Display the grabbed image.
//				//Pylon::DisplayImage(1, ptrGrabResult);
//#endif
//			}
//			else {
//				cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription()
//					<< endl;
//			}
//		}
//	}
//	catch (const GenericException & e) {
//		// Error handling.
//		cerr << "An exception occurred." << endl
//			<< e.GetDescription() << endl;
//		//*openCvImage = imread(g_path, CV_8UC1);
//		errFlag = 1;
//		return 0;
//	}
//
//	//cout << "camera image return" << endl;
//	return 1;
//}
//
//
///* 타겟이 있는지 없는지 판단해주는 함수 */
//int getType() {
//
//	for (size_t i = 0; i < g_contours.size(); i++) {
//		if (fabs(contourArea(Mat(g_approxxxxxxxxx[i]))) > 5000)   // 면적이 일정크기 이상이어야 한다.
//		{
//			int size = g_approxxxxxxxxx[i].size();
//			if (size == 4 && isContourConvex(Mat(g_approx))) {  // 사각형일경우
//																//cout << "사각형 " << endl;
//				return 0;  // 사각형 하나만 처리하도록 설정
//			}
//		}
//	}
//
//	return 1;  // 사각형 없을경우 오류
//}
//
//
///* 전처리된 영상을 받아와 다양한 연산을 해주는 함수 */
//int calculateImage(Mat& cam, Mat& bin, RotatedRect& rect) {
//	int downIndex;
//	int leftIndex;
//	int convertNum = convertToBinary(cam, bin);  // 이진화
//
//	if (convertNum)
//		return 2;
//
//	getContours(bin);  // 꼭지점 및 외곽선 따기
//
//	return 0;
//}
//
//void getCenter(Point start, Point end, Mat& cam) {
//	Point center;
//	center.x = (start.x + end.x) / 2;
//	center.y = (start.y + end.y) / 2;
//	cout << "center is " << center << endl;
//
//	ViewRect = roi(center, start, end);
//}
///* 각도와 중심점을 구해서 반환해주는 함수 */
//double getAngle(Point start, Point end) {
//	double num = (double)(start.y - end.y) / (double)(start.x - end.x);
//	double angle = atan(num) * (180.0 / CV_PI);
//
//	if (angle < 0)
//		angle += 180;
//
//	return angle;
//}
//
//Rect roi(Point center, Point start, Point end) {
//	Rect rect;
//
//	if (start.y < end.y) {
//		rect.y = start.y - 20;
//		rect.height = end.y - rect.y + 40;
//		cout << "height is " << rect.height << endl;
//	}
//	else if (start.y > end.y) {
//		rect.y = end.y - 20;
//		rect.height = start.y - rect.y + 40;
//		cout << "height is " << rect.height << endl;
//	}
//
//	if (start.x < end.x) {
//		rect.x = start.x - 20;
//		rect.width = end.x - rect.x + 40;
//		cout << "width is " << rect.width << endl;
//	}
//	else if (start.x > end.x) {
//		rect.x = end.x - 20;
//		rect.width = start.x - rect.x + 40;
//		cout << "width is " << rect.width << endl;
//	}
//
//	return rect;
//}
//
//
///* 가로선인지 세로선인지 판단해주는 함수 (가로선 : true, 세로선 : false) */
//bool getLineType(Point a, Point b) {
//	if ((getAngle(b, a) < 45) || (getAngle(b, a) > 135)) {  // 각도비교
//		areaOfRectangle(b, a, 1);
//		return true;  // 가로선 true
//	}
//	else {
//		areaOfRectangle(b, a, 0);
//		return false;  // 세로선 false
//	}
//}
//
//
///* 밝기에 따라 자동으로 이진화 threshold 값을 조절해주는 함수 */
//int convertToBinary(Mat& source, Mat& bin) {
//
//	int graySum = 0, loopNum = 0, grayAverage = 0;
//	Mat gray;
//
//	cvtColor(source, gray, COLOR_RGB2GRAY);
//
//	for (int i = 0; i < gray.rows; i += 10) {
//		for (int j = 0; j < gray.cols; j += 10) {
//			graySum += gray.at<uchar>(i, j);
//			loopNum++;
//		}
//	}
//
//	grayAverage = graySum / loopNum;
//	cout << "Gray value average is " << grayAverage << endl;
//
//	if (grayAverage <= 140 && grayAverage > 50)  // 밝기가 너무 낮다면 경고
//		cout << "Brightness is too low, Target may not be found." << endl;
//	else if (grayAverage >= 250)  // 밝기가 너무 높다면 경고
//		cout << "Brightness is too high, Target may not be found." << endl;
//	else if (grayAverage <= 50) {
//		cout << "Brightness is too low, Can't find target." << endl;
//		errFlag = true;
//		return 1;
//	}
//
//
//	threshold(gray, gray, 60, 255, THRESH_BINARY_INV);  // 이진화
//
//	bin = gray;
//
//	return 0;
//}
//
//int main() {
//
//	PylonInitialize();  // Pylon 셋업
//	clock_t start, end;
//
//	while (true) {
//		Mat copyCam1, copyCam2;
//		RotatedRect rect;
//
//		double result;
//
//		start = clock();
//		int resultPreProcessing = preProcessing(cam, copyCam1, copyCam2);
//		int resultCalculate = calculateImage(cam, bin, rect);
//
//		end = clock();
//		cout << "time is " << end - start << endl;
//
//
//		time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
//		struct tm  tstruct;
//		char       buff[80];
//		tstruct = *localtime(&now);
//		strftime(buff, sizeof(buff), "%Y-%m-%d.%X \n", &tstruct); // YYYY-MM-DD.HH:mm:ss 형태의 스트링
//
//		ofstream writeFile("/home/pi/night_test/log.txt", ios::app);
//		if (writeFile.is_open()) {
//			writeFile.write(buff, strlen(buff));
//			writeFile.close();
//		}
//		viewImg = cam(ViewRect).clone();
//		imshow("aaa", viewImg);
//		waitKey(10);
//	}
//
//}