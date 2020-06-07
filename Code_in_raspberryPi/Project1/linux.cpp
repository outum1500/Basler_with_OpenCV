///* --------------------------- Setting --------------------------- */
//
//#include <memory.h>
//#include <fstream>
//#include <stdlib.h>
//#include <string.h>
//#include <stdio.h>
//#include <conio.h>
//#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <pylon/PylonIncludes.h>
//
//#ifdef PYLON_WIN_BUILD
//#    include <pylon/PylonGUI.h>
//#endif
//
//#pragma warning(disable:4996)
//
//#define RUNNING_LED 17
//#define NG_LED 27
//#define OK_LED 22
//#define BTN 14
//
//using namespace cv;
//using namespace std;
//using namespace Pylon;
//
///* ---------------------------------------------------------------- */
//
//
//
///* --------------------------- function --------------------------- */
//
//void drawRotatedRectangle(Mat &image, RotatedRect rect);
//
//void getContours(Mat &source);
//
//void drawAngle(Mat &source, int i, int downIndex, int leftIndex);
//
//void areaOfRectangle(Point start, Point end, int iswidth);
//
//void getHorizontalLine();
//
//void setLabel(Mat &image, string str, vector <Point> contour);
//
//void readFileSetup();
//
//int readFile(String path, String *data);
//
//int parseStringValue(string str, string key);
//
//int capVideo(Mat *openCvImage);
//
//int parseString(string *lines, int cnt, string key);
//
//int getType();
//
//double getAngle(Point start, Point end);
//
//bool getLineType(Point a, Point b);
//
//Mat convertToBinary(Mat &source);
//
///* ---------------------------------------------------------------- */
//
//
//
///* --------------------------- Global variable --------------------------- */
//
//string g_path = "image4.jpg"; //ī�޶� ������ ��ü�Ǿ� ���� ����
//
//RNG g_rng(12345);
//
//vector <vector<Point>> g_contours;
//vector <Vec4i> g_hierarchy;
//vector <Point2f> g_approx;
//vector <vector<Point>> g_approxxxxxxxxx;
//Point g_li[2][2];
//
//int g_printWidthFlag = 0, g_printHeightFlag = 0;
//int u_angle = 0, u_circleArea = 0, u_rectangleArea = 0;  // ������� �ν��� �� ������ ��� ����
//int g_bigCircleIndex = 0, g_smallCircleIndex = 0;
//int errflag = 0;
//
//double g_dwidthDistance = 0, g_dheightDistance = 0;
//double g_dmm = 0;
//double g_drealRectangleLength = 320;
//
//struct sockaddr_in serv_addr;
//struct sockaddr_in clnt_addr;
//int serv_sock;
//int clnt_sock;
//int readn_sock;
//char socket_buf[2048];
//
///* ---------------------------------------------------------------- */
//
//
//
//
//void drawRotatedRectangle(Mat &image, RotatedRect rect) {
//
//	Point2f vertices[4];
//	rect.points(vertices);
//	for (int i = 0; i < 4; i++)
//		line(image, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255, 0), 2);
//
//}
//
//
//void getContours(Mat &source) {  // �ܰ����� ������ ��� �Լ�
//
//	int circle_area_average = 0;
//	int circle_big = 0, circle_small = 10000;
//
//	findContours(source, g_contours, g_hierarchy, RETR_LIST, CHAIN_APPROX_NONE);  // �ܰ��� ã��
//
//	g_approxxxxxxxxx.resize(g_contours.size());
//
//	for (int i = 0; i < g_contours.size(); i++) {
//		drawContours(source, g_contours, i, Scalar(0, 255, 255), 2, 8, g_hierarchy, 0, Point());  // �ܰ��� �׸���
//		approxPolyDP(Mat(g_contours[i]), g_approx, arcLength(Mat(g_contours[i]), true) * 0.01,
//			true);  // ������ ����1 (approx)
//		approxPolyDP(Mat(g_contours[i]), g_approxxxxxxxxx[i], arcLength(Mat(g_contours[i]), true) * 0.002,
//			true);  // ������ ����2 (approxxxxxxxxx)
//
//
//		if (fabs(contourArea(Mat(g_approxxxxxxxxx[i]))) < 2000) {  // ���� ��� �� if�� �ȿ� ���´�
//
//																   // ������ ��� ũ�⸦ ���ϱ� ���� ������ ��� ���Ѵ�
//			circle_area_average += fabs(contourArea(Mat(g_approxxxxxxxxx[i])));
//
//			// ������ ����� ������ ���� ���� ���� ũ�Ⱑ ũ�ٸ�?
//			if (circle_big < fabs(contourArea(Mat(g_approxxxxxxxxx[i])))) {
//				circle_big = fabs(contourArea(Mat(g_approxxxxxxxxx[i])));  // �ִ� ���� ũ�� ����
//				g_bigCircleIndex = i;  // �ε����� ����
//			}
//
//			// ������ ����� ������ ���� ���� ���� ũ�Ⱑ �۴ٸ�?
//			if (circle_small > fabs(contourArea(Mat(g_approxxxxxxxxx[i])))) {
//				circle_small = fabs(contourArea(Mat(g_approxxxxxxxxx[i])));  // �ּ� ���� ũ�� ����
//				g_smallCircleIndex = i;  // �ε����� ����
//			}
//		}
//
//		setLabel(source, to_string(i + 1), g_contours[i]);  // ���� ��ȣ �Ű��ֱ�
//	}
//
//
//	cout << "------------------------------------------------" << endl;
//	cout << "Average of circle area : " << circle_area_average / 64 << endl;
//	cout << "Biggest circle area : " << circle_big << endl;
//	cout << "Smallest circle area : " << circle_small << endl;
//	if (u_circleArea < circle_big) {
//		errflag = 1;
//		cout << "!!!!!!!! Warning !!!!!!!!" << endl;
//		cout << "|   circle is too big   |" << endl;
//		cout << "-------------------------" << endl;
//	}
//	cout << "------------------------------------------------" << endl << endl << endl;
//
//
//	for (int i = 0; i < g_approx.size(); i++) {
//		line(source, g_approx[i], g_approx[i], Scalar(255, 0, 255), 10);  // �������� �����
//																		  // cout << approx[i] << endl;
//	}
//
//	//cout << "contours count : " << g_contours.size() << endl;  // ī��Ʈ ���� ���
//
//}
//
//
//void drawAngle(Mat &source, int i, int downIndex, int leftIndex) {  // ���� �̹����� ǥ���ϴ� �Լ�
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
//
//	if (u_angle < getAngle(a, b)) {
//		errflag = 1;
//		cout << "!!!!!!!! Warning !!!!!!!!" << endl;
//		cout << "| Angle is too oblique  |" << endl;
//		cout << "-------------------------" << endl;
//
//	}
//
//}
//
//
//void areaOfRectangle(Point start, Point end, int iswidth) {
//
//
//	double dy = end.y - start.y;
//	double dx = end.x - start.x;
//
//	if (iswidth) {  // ���� ���̶�� �����
//
//					// ��Ʈ������ �ȼ� ��� ���� ũ�⸦ ���Ѵ�
//		if (!g_printWidthFlag) {
//			g_dwidthDistance = sqrt((dx * dx) + (dy * dy));
//			g_dmm = g_drealRectangleLength / g_dwidthDistance;
//
//			cout << "Width distance : " << g_dwidthDistance << endl;
//
//
//			g_printWidthFlag = 1;
//		}
//
//	}
//	else {  // ���� ���̶�� �����
//
//		if (!g_printHeightFlag) {
//			g_dheightDistance = sqrt((dx * dx) + (dy * dy));
//			double mm = g_drealRectangleLength / g_dheightDistance;
//
//			cout << "Height distance : " << g_dheightDistance << endl;
//
//			g_printHeightFlag = 1;
//		}
//
//	}
//
//
//}
//
//
//void getHorizontalLine() {  // �簢������ ���μ� ���ϴ� �Լ�
//	int j = 0;
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
//
//}
//
//
//void setLabel(Mat &image, string str, vector <Point> contour) {
//	int fontface = FONT_HERSHEY_SIMPLEX;
//	double scale = 0.5;
//	int thickness = 1;
//	int baseline = 0;
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
//void readFileSetup() {
//
//	int textFileRoute;
//	String lines[10];
//
//	cout << "Reading User File.." << endl;
//	textFileRoute = readFile("/home/pi/header_test/stdio.txt", lines);
//	u_angle = parseString(lines, textFileRoute, "over_angle");
//	u_circleArea = parseString(lines, textFileRoute, "circle_area");
//	u_rectangleArea = parseString(lines, textFileRoute, "rectangle_area");
//	cout << "Reading User File Success" << endl << endl;
//
//}
//
//
//void preProcessing(Mat &cam2) {
//
//	// Grab�� ���� ��ó��
//	cout << "Getting Camera Image.." << endl;
//	capVideo(&cam2);
//	//imshow("original", cam2);
//	//cam2 = cam2(Rect(500, 400, 700, 550));
//	cvtColor(cam2, cam2, COLOR_GRAY2RGB);
//	cout << "Getting Camera Image Success" << endl << endl << endl << endl << endl << endl << endl << endl;
//
//}
//
//
//void printResult(Mat &cam, RotatedRect &rect) {
//
//	cout << "center of rectangle : " << rect.center << endl;
//	cout << "circle count of rectangle : " << g_contours.size() - 1 << endl;
//	cout << "------------------------------------------------" << endl << endl << endl;
//	cout << "------------------------------------------------" << endl;
//	cout << "rectangle pixel area : " << g_dwidthDistance * g_dheightDistance << endl;
//	cout << "rectangle cm area : " << ((g_dwidthDistance * g_dmm) * (g_dheightDistance * g_dmm)) / 100 << endl;
//	if (u_rectangleArea < g_dwidthDistance * g_dheightDistance) {
//		errflag = 1;
//		cout << "!!!!!!!! Warning !!!!!!!!" << endl;
//		cout << "| rectangle is too big  |" << endl;
//		cout << "-------------------------" << endl;
//	}
//	cout << "1 pixel is " << g_dmm << "mm" << endl;
//	cout << "------------------------------------------------" << endl << endl << endl;
//
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
//
//int readFile(String path, String *data) {
//
//	int i = 0;
//
//	ifstream openFile(path.data());
//	if (openFile.is_open()) {
//		string line;
//		while (getline(openFile, line)) { data[i++] = line; };
//		openFile.close();
//	}
//	return i;
//}
//
//
//int parseStringValue(string str, string key) {
//	if (str.find(key) == string::npos) return 0;
//	return atoi(str.substr(str.find(key) + key.length() + 1).c_str());
//}
//
//
//int parseString(string *lines, int cnt, string key) {
//
//	for (int i = 0; i < cnt; i++) {
//		if (int value = parseStringValue(lines[i], key)) {
//			return value;
//		}
//	}
//	return 0;
//}
//
//
//int capVideo(Mat *openCvImage) {
//
//	try {
//		CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
//
//
//		camera.MaxNumBuffer = 5;
//
//
//		camera.StartGrabbing(1);
//
//
//		CGrabResultPtr ptrGrabResult;
//
//		CPylonImage image;
//		CImageFormatConverter fc;
//		fc.OutputPixelFormat = PixelType_Mono8;
//		fc.OutputBitAlignment = OutputBitAlignment_MsbAligned;
//
//		while (camera.IsGrabbing()) {
//
//			camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
//
//			if (ptrGrabResult->GrabSucceeded()) {
//				fc.Convert(image, ptrGrabResult);
//				//cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
//				//cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
//				const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
//				//cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << endl << endl;
//				//cout << "length : " << (uint32_t)pImageBuffer[1000] << endl;
//
//				*openCvImage = Mat(966, 1296, CV_8UC1, (uint32_t *)image.GetBuffer()).clone();
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
//	catch (const GenericException &e) {
//		// Error handling.
//		cerr << "An exception occurred." << endl
//			<< e.GetDescription() << endl;
//		*openCvImage = imread(g_path, CV_8UC1);
//		return 0;
//	}
//
//	//cout << "camera image return" << endl;
//	return 1;
//}
//
//
//int getType() {  // �簢�� ���� ���
//
//	for (size_t i = 0; i < g_contours.size(); i++) {
//		if (fabs(contourArea(Mat(g_approxxxxxxxxx[i]))) > 1000)   // ������ ����ũ�� �̻��̾�� �Ѵ�.
//		{
//			int size = g_approxxxxxxxxx[i].size();
//			if (size == 4 && isContourConvex(Mat(g_approx))) {  // �簢���ϰ��
//																// cout << "�簢�� " << endl;
//				return size;  // �簢�� �ϳ��� ó���ϵ��� ����
//			}
//		}
//	}
//
//	return 0;  // �簢�� ������� 0����
//
//}
//
//
//double getAngle(Point start, Point end)  // ���� ���ϴ� �Լ�
//{
//	double dy = end.y - start.y;
//	double dx = end.x - start.x;
//
//	double angle = abs(atan2(dy, dx) * (180.0 / CV_PI));
//
//	if (angle > 90) {
//		angle = 180 - angle;
//	}
//
//	return angle;
//}
//
//
//bool getLineType(Point a, Point b) {  // ���μ�, ���μ� �����ϴ� �Լ�. ���μ��� true, ���δ� false
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
//Mat convertToBinary(Mat &source) {  // ����ȭ �Լ�
//
//	Mat gray;
//	cvtColor(source, gray, COLOR_RGB2GRAY);
//	threshold(gray, gray, 80, 255, THRESH_BINARY_INV);  // ����ȭ
//	return gray;
//
//}
//
//
//Mat calculateImage(Mat &cam, RotatedRect &rect) {
//
//	Mat bin;
//	int downIndex;
//	int leftIndex;
//
//	bin = convertToBinary(cam);  // ����ȭ
//	getContours(bin);  // �������� �ܰ��� ����
//					   //cout << getType() << endl;  // �簢�� �ִ��� ���ϱ�
//	cout << "------------------------------------------------" << endl;
//	getHorizontalLine();  // ���μ� ���ϱ�
//	cout << "------------------------------------------------" << endl << endl << endl;
//
//
//	cout << "------------------------------------------------" << endl;
//	// �簢�� ���� �Ʒ��� �ִ� �� ã��
//	Point downPoint = g_approx[0];
//	for (int i = 1; i < g_approx.size(); i++) {
//		if (g_approx[i].y > downPoint.y) {
//			downPoint = g_approx[i];
//		}
//	}
//
//
//	for (int i = 0; i < 2; i++) {
//		for (int j = 0; j < 2; j++) {
//			if (downPoint == g_li[i][j]) {  // �簢�� ���� �Ʒ��ִ� ���� �����ϴ� ���μ� ã��
//											// line(cam, li[i][0], li[i][1], Scalar(255, 0, 255), 3);  // �簢�� �Ʒ��ٿ� �ٱ߱�
//				if (g_li[i][0].y > g_li[i][1].y) {  //���� �Ʒ� �� ��������
//					downIndex = 0;
//				}
//				else {
//					downIndex = 1;
//				}
//
//				if (g_li[!i][0].x < g_li[!i][1].x) {  //���� �Ʒ� �� ��������
//					leftIndex = 0;
//				}
//				else {
//					leftIndex = 1;
//				}
//
//				drawAngle(cam, i, downIndex, leftIndex);  // ���� ǥ��
//				i = 2;  // break
//				j = 2;  // break
//			}
//		}
//	}
//
//
//	rect = minAreaRect(g_approx);  // ���� �簢�� ���ϱ�
//	line(cam, rect.center, rect.center, Scalar(0, 255, 0), 5);  // ���� ���� �簢�� �߾ӿ� �����
//
//	cam = cam(Rect(rect.center.x / 2, rect.center.y / 2, rect.center.x, rect.center.y));
//	bin = bin(Rect(rect.center.x / 2, rect.center.y / 2, rect.center.x, rect.center.y));
//
//	drawContours(cam, g_contours, g_bigCircleIndex, Scalar(255, 0, 0), 1, 8, g_hierarchy, 0, Point());
//	drawContours(cam, g_contours, g_smallCircleIndex, Scalar(0, 0, 255), 1, 8, g_hierarchy, 0, Point());
//
//
//	return bin;
//}
//
//
//
//int main() {
//
//	Mat cam, bin;
//	RotatedRect rect;
//
//	PylonInitialize();
//
//
//	cout << "now press button" << endl;
//
//
//	while (true) {
//
//		int keynum = getch();
//
//		if (keynum == 97) {
//		
//			cout << "you pressed button" << endl;
//
//			// ����ڰ� �ۼ��� ������ �о��
//			readFileSetup();
//
//
//			// Grab�� ���� ��ó��
//			preProcessing(cam);
//
//
//			// ��ó�� �� ������ �̿��� �����
//			bin = calculateImage(cam, rect);
//
//			// ��� ���
//			printResult(cam, rect);
//			if (errflag == 0) {
//				cout << "no error" << endl;
//			}
//			imshow("cam", cam);
//			imshow("bin", bin);
//
//			waitKey(0);
//		}
//
//		else {
//			errflag = 0;
//		}
//
//
//	}
//
//	waitKey(0);
//
//}
