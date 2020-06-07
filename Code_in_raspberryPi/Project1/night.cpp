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
//int u_angle = 0, u_circleArea = 0, u_rectangleArea = 0;  // ������� �ν��� �� ������ ��� ����
//int u_circleAreaAverage = 0, u_circeError = 0;
//int g_bigCircleIndex = 0, g_smallCircleIndex = 0;
//
//double g_dwidthDistance = 0, g_dheightDistance = 0;
//double g_dmm = 0;
//double g_drealRectangleLength = 320;
//
//
//
///* ����� Ÿ���� ���� �簢���� �׸��� �Լ� */
//void drawRotatedRectangle(Mat& image, RotatedRect rect) {
//	Point2f vertices[4];
//	rect.points(vertices);
//	for (int i = 0; i < 4; i++)
//		line(image, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255, 0), 2);
//}
//
//
///* ����ȭ�� �̹����� �ܰ����� �����ϰ�, ����� �������� �� ũ��� �������� �����ϴ� �Լ� */
//void getContours(Mat& source) {  // �ܰ����� ������ ��� �Լ�
//
//	int circle_area_average = 0;
//	int circle_big = 0, circle_small = 10000;
//	Point start, end;
//
//	findContours(source, g_contours, g_hierarchy, RETR_LIST, CHAIN_APPROX_NONE);  // �ܰ��� ã��
//
//	g_approxxxxxxxxx.resize(g_contours.size());
//
//
//	for (int i = 0; i < g_contours.size(); i++) {
//		drawContours(source, g_contours, i, Scalar(0, 255, 255), 2, 8, g_hierarchy, 0, Point());  // �ܰ��� �׸���
//		approxPolyDP(Mat(g_contours[i]), g_approx, arcLength(Mat(g_contours[i]), true) * 0.01,
//			true);  // ������ ����1 (approx)
//		approxPolyDP(Mat(g_contours[i]), g_approxxxxxxxxx[i], arcLength(Mat(g_contours[i]), true) * 0.002,
//			true);  // ������ ����2 (approxxxxxxxxx)
//
//					//cout << "contour" << i << "'s center : " << g_ endl;
//		setLabel(source, to_string(i + 1), g_contours[i]);  // ���� ��ȣ �Ű��ֱ�
//
//		RotatedRect rect = minAreaRect(g_approx);  // ���� �簢�� ���ϱ�
//		line(source, rect.center, rect.center, Scalar(0, 255, 0), 5);  // ���� ���� �簢�� �߾ӿ� �����
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
//	//   line(source, g_approx[i], g_approx[i], Scalar(255, 0, 255), 10);  // �������� �����
//}
//
//
///* ���� ������ ���ؼ��� ��� �̹����� ǥ�����ִ� �Լ� */
//void drawAngle(Mat& source, int i, int downIndex, int leftIndex) {
//
//	Point a = g_li[i][downIndex];
//	Point b = g_li[i][!downIndex];
//
//	line(source, Point(0, a.y), Point(source.size().width, a.y), Scalar(0, 0, 255), 2, LINE_AA);  // ���ؼ� �׸���
//
//	if (a.x > b.x) {
//		line(source, a, b, Scalar(0, 0, 255), 2);  // ���� �� ���̶���Ʈ
//		putText(source, to_string(getAngle(a, b)), Point(a.x, b.y - 2), 1, 1, Scalar(255, 0, 0), 2);  // ���� �Է�
//	}
//	else {
//		line(source, a, g_li[!i][leftIndex], Scalar(0, 0, 255), 2);  // ���� �� ���̶���Ʈ
//		putText(source, to_string(90 - getAngle(a, b)), Point(a.x, b.y - 2), 1, 1, Scalar(255, 0, 0), 2);  // ���� �Է�
//	}
//}
//
//
///* Ÿ���� �ȼ� ��� ���� ũ��� Ÿ���� ����, ���� ���̸� ������ִ� �Լ� */
//void areaOfRectangle(Point start, Point end, int iswidth) {
//
//	double dy = end.y - start.y;
//	double dx = end.x - start.x;
//	double mm;
//
//	if (iswidth) {  // ���� ���̶�� �����
//		if (!g_printWidthFlag) {  // ��Ʈ������ �ȼ� ��� ���� ũ�⸦ ���Ѵ�
//			g_dwidthDistance = sqrt((dx * dx) + (dy * dy));
//			g_dmm = g_drealRectangleLength / g_dwidthDistance;
//
//			cout << "Width distance : " << g_dwidthDistance << endl;
//			g_printWidthFlag = 1;
//		}
//	}
//	else {  // ���� ���̶�� �����
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
///* �簢������ ���μ����� �ƴ����� �Ǵ��ϰ�, ���μ��� �� �� ��ǥ�� �������ִ� �Լ� */
//void getHorizontalLine() {
//	int j = 0;
//
//	for (int i = 1; i < g_approx.size(); i++)  // ���μ����� ���ϱ�(1 ~ MAX-1)
//	{
//		if (getLineType(g_approx[i - 1], g_approx[i])) {  // ���μ����� ���ϱ�(1 ~ MAX-1)
//			g_li[j][0] = g_approx[i];  // ���μ��̸� ����
//			g_li[j][1] = g_approx[i - 1];  // ���μ��̸� ����
//			j++;  // �ε��� ++
//		}
//	}
//
//	if (getLineType(g_approx[0], g_approx[g_approx.size() - 1])) {  // ���μ����� ���ϱ�(0, MAX)
//		g_li[j][0] = g_approx[0];  // ���μ��̸� ����
//		g_li[j][1] = g_approx[g_approx.size() - 1];  // ���μ��̸� ����
//	}
//}
//
//
///* ����� ������ ��ü�� �޾ƿ���, ��� �̹����� �� ��° ��ü���� �� ��ü���� �󺧸� ���ִ� �Լ� */
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
///* ���� ����� �ַܼ� ������ִ� �Լ� */
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
///* ���� ��ó�� �Լ� */
//int preProcessing(Mat& cam, Mat& copyCam1, Mat& copyCam2) {
//
//	// Grab�� ���� ��ó��
//	cout << "Getting Camera Image.." << endl;
//	if (!capVideo(&cam)) {  // ī�޶� ���������� ���ٸ�?
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
///* Basler ī�޶�κ��� ������ ȹ���� �� Mat�� �̹����� �ٲ��ִ� �Լ� */
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
///* Ÿ���� �ִ��� ������ �Ǵ����ִ� �Լ� */
//int getType() {
//
//	for (size_t i = 0; i < g_contours.size(); i++) {
//		if (fabs(contourArea(Mat(g_approxxxxxxxxx[i]))) > 5000)   // ������ ����ũ�� �̻��̾�� �Ѵ�.
//		{
//			int size = g_approxxxxxxxxx[i].size();
//			if (size == 4 && isContourConvex(Mat(g_approx))) {  // �簢���ϰ��
//																//cout << "�簢�� " << endl;
//				return 0;  // �簢�� �ϳ��� ó���ϵ��� ����
//			}
//		}
//	}
//
//	return 1;  // �簢�� ������� ����
//}
//
//
///* ��ó���� ������ �޾ƿ� �پ��� ������ ���ִ� �Լ� */
//int calculateImage(Mat& cam, Mat& bin, RotatedRect& rect) {
//	int downIndex;
//	int leftIndex;
//	int convertNum = convertToBinary(cam, bin);  // ����ȭ
//
//	if (convertNum)
//		return 2;
//
//	getContours(bin);  // ������ �� �ܰ��� ����
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
///* ������ �߽����� ���ؼ� ��ȯ���ִ� �Լ� */
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
///* ���μ����� ���μ����� �Ǵ����ִ� �Լ� (���μ� : true, ���μ� : false) */
//bool getLineType(Point a, Point b) {
//	if ((getAngle(b, a) < 45) || (getAngle(b, a) > 135)) {  // ������
//		areaOfRectangle(b, a, 1);
//		return true;  // ���μ� true
//	}
//	else {
//		areaOfRectangle(b, a, 0);
//		return false;  // ���μ� false
//	}
//}
//
//
///* ��⿡ ���� �ڵ����� ����ȭ threshold ���� �������ִ� �Լ� */
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
//	if (grayAverage <= 140 && grayAverage > 50)  // ��Ⱑ �ʹ� ���ٸ� ���
//		cout << "Brightness is too low, Target may not be found." << endl;
//	else if (grayAverage >= 250)  // ��Ⱑ �ʹ� ���ٸ� ���
//		cout << "Brightness is too high, Target may not be found." << endl;
//	else if (grayAverage <= 50) {
//		cout << "Brightness is too low, Can't find target." << endl;
//		errFlag = true;
//		return 1;
//	}
//
//
//	threshold(gray, gray, 60, 255, THRESH_BINARY_INV);  // ����ȭ
//
//	bin = gray;
//
//	return 0;
//}
//
//int main() {
//
//	PylonInitialize();  // Pylon �¾�
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
//		time_t     now = time(0); //���� �ð��� time_t Ÿ������ ����
//		struct tm  tstruct;
//		char       buff[80];
//		tstruct = *localtime(&now);
//		strftime(buff, sizeof(buff), "%Y-%m-%d.%X \n", &tstruct); // YYYY-MM-DD.HH:mm:ss ������ ��Ʈ��
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