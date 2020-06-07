/* Code Rule * /
/*
Global Variable : g_myName
Local Variable : myName
Function Name : myName()
*/



/* --------------------------- Setting --------------------------- */
/* Header */
#include <memory.h>
#include <time.h>
#include <mysql/mysql.h>
#include <wiringPi.h>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <signal.h>
#include <sys/socket.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <pylon/PylonIncludes.h>


#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

/* Warning Disable */
#pragma warning(disable:4996)

/* Define */
#define RUNNING_LED 17
#define NG_LED 27
#define OK_LED 22
#define BTN 14

/* Namespace */
using namespace cv;
using namespace std;
using namespace Pylon;

/* ---------------------------------------------------------------- */



/* --------------------------- function --------------------------- */

MYSQL *mysql_connection_setup(struct connection_details mysql_details);

MYSQL_RES *mysql_perform_query(MYSQL *connection, char *sql_query);

const std::string currentDateTime(int istime);

void areaOfRectangle(Point start, Point end, int iswidth);

void drawRotatedRectangle(Mat &image, RotatedRect rect);

void getContours(Mat &source);

void drawAngle(Mat &source, int i, int downIndex, int leftIndex);

void getHorizontalLine();

void setLabel(Mat &image, string str, vector <Point> contour);

void readFileSetup();

void printResult(Mat &cam, RotatedRect &rect);

void sockTimeout(int signal);

void printLine();

void printEndLine();

void initTableValues();

int preProcessing(Mat &cam2);

int readFile();

int parseStringValue(string str, string key);

int parseString(string *lines, int cnt, string key);

int capVideo(Mat *openCvImage);

int getType();

int calculateImage(Mat &cam, Mat &bin, RotatedRect &rect);

double getAngle(Point start, Point end);

bool getLineType(Point a, Point b);

void sockGetText();

void fileSave();

int convertToBinary(Mat &source, Mat &bin);

/* ---------------------------------------------------------------- */



/* --------------------------- Global variable --------------------------- */

string g_path = "image4.jpg"; //카메라 오류시 대체되어 나올 사진
string filepath = "/home/pi/header_test/stdio.txt"; //텍스트 파일 경로
string lines[10];

RNG g_rng(12345);

vector <vector<Point>> g_contours;
vector <Vec4i> g_hierarchy;
vector <Point2f> g_approx;
vector <vector<Point>> g_approxxxxxxxxx;
Point g_li[2][2];

struct sockaddr_in serv_addr;
struct sockaddr_in clnt_addr;

int g_printWidthFlag = 0, g_printHeightFlag = 0;
int errFlag = 0, runningFlag = 0;
int u_angle = 0, u_circleArea = 0, u_rectangleArea = 0;
int u_circleAreaAverage = 0, u_circeError = 0;
int g_bigCircleIndex = 0, g_smallCircleIndex = 0;
int g_serv_sock, g_clnt_sock;

double g_dwidthDistance = 0, g_dheightDistance = 0;
double g_dmm = 0;
double g_drealRectangleLength = 320;

char g_socket_buf[2048];
bool g_sock_flag = true;
bool g_btnCalFlag;
bool g_startFlag = false;

bool g_db_flag = true;

char g_db_ip[20] = "192.168.0.120";

/* ---------------------------------------------------------------- */



/* --------------------------- MySQL 관련 구조체와 함수들 --------------------------- */
struct connection_details {
	char *server;
	char *user;
	char *password;
	char *database;
};

struct dworld_table_values {
	string u_date = currentDateTime(0);
	string u_time = currentDateTime(1);
	int u_product_num = 0;
	int u_size = 0;
	double u_angle = 0.0;
	bool u_error = false;
	string u_image = "";

	string fileTime = currentDateTime(2);
	string fileDate = currentDateTime(4);
};

dworld_table_values g_table_values;

const std::string currentDateTime(int istime) {
	time_t now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm tstruct;
	char time_buf[80];
	char date_buf[80];
	char nontime_buf[80];
	char nondate_buf[80];
	tstruct = *localtime(&now);
	strftime(time_buf, sizeof(time_buf), "%R:%S", &tstruct); // HH:mm:ss 형태의 스트링
	strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", &tstruct);
	strftime(nontime_buf, sizeof(nontime_buf), "%H%M%S", &tstruct);
	strftime(nondate_buf, sizeof(nondate_buf), "%Y%m%d", &tstruct);

	if (istime == 1) {
		return time_buf;
	}
	else if (istime == 2) {
		return nontime_buf;
	}
	else if (istime == 0) {
		return date_buf;
	}
	else if (istime == 4) {
		return nondate_buf;
	}
}

MYSQL *mysql_connection_setup(struct connection_details mysql_details) {

	MYSQL *connection = mysql_init(NULL);

	if (!mysql_real_connect(connection, mysql_details.server, mysql_details.user, mysql_details.password,
		mysql_details.database, 0, NULL, 0)) {

		printf("Connection error : %s\n", mysql_error(connection));
		g_db_flag = false;
		return connection;
	}
	printf("Connect DB");
	g_db_flag = true;
	return connection;
}

MYSQL_RES *mysql_perform_query(MYSQL *connection, char *sql_query) {

	if (mysql_query(connection, sql_query)) {

		printf("MYSQL query error : %s\n", mysql_error(connection));
		exit(1);

	}
	return mysql_use_result(connection);
}
/* -------------------------------------------------------------------------------- */





/* 오류 출력 함수 */
void showError(char why[30]) {

	errFlag = 1;
	digitalWrite(NG_LED, HIGH);
	digitalWrite(RUNNING_LED, LOW);
	cout << "!!!!!!!! Warning !!!!!!!!" << endl;
	cout << why << endl;
	cout << "-------------------------" << endl;
	g_table_values.u_error = true;

}


/* 검출된 타겟의 외접 사각형을 그리는 함수 */
void drawRotatedRectangle(Mat &image, RotatedRect rect) {
	Point2f vertices[4];
	rect.points(vertices);
	for (int i = 0; i < 4; i++)
		line(image, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255, 0), 2);
}


/* 이진화된 이미지의 외곽선을 추출하고, 결과를 바탕으로 그 크기와 꼭짓점을 추출하는 함수 */
void getContours(Mat &source) {  // 외곽선및 꼭지점 잡는 함수

	int circle_area_average = 0;
	int circle_big = 0, circle_small = 10000;

	findContours(source, g_contours, g_hierarchy, RETR_LIST, CHAIN_APPROX_NONE);  // 외곽선 찾기

	g_approxxxxxxxxx.resize(g_contours.size());


	for (int i = 0; i < g_contours.size(); i++) {
		drawContours(source, g_contours, i, Scalar(0, 255, 255), 2, 8, g_hierarchy, 0, Point());  // 외곽선 그리기
		approxPolyDP(Mat(g_contours[i]), g_approx, arcLength(Mat(g_contours[i]), true) * 0.01,
			true);  // 꼭지점 따기1 (approx)
		approxPolyDP(Mat(g_contours[i]), g_approxxxxxxxxx[i], arcLength(Mat(g_contours[i]), true) * 0.002,
			true);  // 꼭지점 따기2 (approxxxxxxxxx)


		if (fabs(contourArea(Mat(g_approxxxxxxxxx[i]))) < 2000) {  // 원일 경우 이 if문 안에 들어온다

																   // 원들의 평균 크기를 구하기 위해 면적을 모두 더한다
			circle_area_average += fabs(contourArea(Mat(g_approxxxxxxxxx[i])));

			// 기존에 저장된 원보다 새로 들어온 원의 크기가 크다면?
			if (circle_big < fabs(contourArea(Mat(g_approxxxxxxxxx[i])))) {
				circle_big = fabs(contourArea(Mat(g_approxxxxxxxxx[i])));  // 최대 원의 크기 저장
				g_bigCircleIndex = i;  // 인덱스도 저장
			}

			// 기존에 저장된 원보다 새로 들어온 원의 크기가 작다면?
			if (circle_small > fabs(contourArea(Mat(g_approxxxxxxxxx[i])))) {
				circle_small = fabs(contourArea(Mat(g_approxxxxxxxxx[i])));  // 최소 원의 크기 저장
				g_smallCircleIndex = i;  // 인덱스도 저장
			}
		}

		setLabel(source, to_string(i + 1), g_contours[i]);  // 원에 번호 매겨주기
	}


	printLine();
	cout << "Average of circle area : " << circle_area_average / 64 << endl;
	cout << "Biggest circle area : " << circle_big << endl;
	cout << "Smallest circle area : " << circle_small << endl;


	if (u_circleArea < circle_big) {
		showError("|   circle is too big   |");
	}

	if (g_contours.size() != 65) {
		showError("|   different contours    |");
	}

	if (!(((u_circleAreaAverage - u_circeError) < circle_area_average) &&
		((u_circleAreaAverage + u_circeError) > circle_area_average))) {
		showError("|   Circle Average Size Error   |");
	}


	printEndLine();
	for (int i = 0; i < g_approx.size(); i++)
		line(source, g_approx[i], g_approx[i], Scalar(255, 0, 255), 10);  // 꼭지점에 점찍기
}


/* 오차 각도와 기준선을 결과 이미지에 표시해주는 함수 */
void drawAngle(Mat &source, int i, int downIndex, int leftIndex) {

	Point a = g_li[i][downIndex];
	Point b = g_li[i][!downIndex];

	line(source, Point(0, a.y), Point(source.size().width, a.y), Scalar(0, 0, 255), 2, LINE_AA);  // 기준선 그리기

	if (a.x > b.x) {
		line(source, a, b, Scalar(0, 0, 255), 2);  // 기존 선 하이라이트
		putText(source, to_string(getAngle(a, b)), Point(a.x, b.y - 2), 1, 1, Scalar(255, 0, 0), 2);  // 각도 입력
	}
	else {
		line(source, a, g_li[!i][leftIndex], Scalar(0, 0, 255), 2);  // 기존 선 하이라이트
		putText(source, to_string(90 - getAngle(a, b)), Point(a.x, b.y - 2), 1, 1, Scalar(255, 0, 0), 2);  // 각도 입력
	}

	if (u_angle < getAngle(a, b)) {
		showError("| Angle is too oblique  |");
	}

	g_table_values.u_angle = getAngle(a, b);
}


/* 타겟의 픽셀 대비 실제 크기와 타겟의 가로, 세로 길이를 출력해주는 함수 */
void areaOfRectangle(Point start, Point end, int iswidth) {

	double dy = end.y - start.y;
	double dx = end.x - start.x;
	double mm;

	if (iswidth) {  // 가로 선이라면 여기로
		if (!g_printWidthFlag) {  // 매트릭스의 픽셀 대비 실제 크기를 구한다
			g_dwidthDistance = sqrt((dx * dx) + (dy * dy));
			g_dmm = g_drealRectangleLength / g_dwidthDistance;

			cout << "Width distance : " << g_dwidthDistance << endl;
			g_printWidthFlag = 1;
		}
	}
	else {  // 세로 선이라면 여기로
		if (!g_printHeightFlag) {
			g_dheightDistance = sqrt((dx * dx) + (dy * dy));
			mm = g_drealRectangleLength / g_dheightDistance;

			cout << "Height distance : " << g_dheightDistance << endl;
			g_printHeightFlag = 1;
		}
	}
}


/* 사각형에서 가로선인지 아닌지를 판단하고, 가로선의 양 끝 좌표를 저장해주는 함수 */
void getHorizontalLine() {
	int j = 0;

	for (int i = 1; i < g_approx.size(); i++)  // 가로선인지 구하기(1 ~ MAX-1)
	{
		if (getLineType(g_approx[i - 1], g_approx[i])) {  // 가로선인지 구하기(1 ~ MAX-1)
			g_li[j][0] = g_approx[i];  // 가로선이면 저장
			g_li[j][1] = g_approx[i - 1];  // 가로선이면 저장
			j++;  // 인덱스 ++
		}
	}

	if (getLineType(g_approx[0], g_approx[g_approx.size() - 1])) {  // 가로선인지 구하기(0, MAX)
		g_li[j][0] = g_approx[0];  // 가로선이면 저장
		g_li[j][1] = g_approx[g_approx.size() - 1];  // 가로선이면 저장
	}
}


/* 검출된 컨투어 객체를 받아오고, 결과 이미지에 몇 번째 객체인지 각 객체마다 라벨링 해주는 함수 */
void setLabel(Mat &image, string str, vector <Point> contour) {
	int fontface = FONT_HERSHEY_SIMPLEX;
	int thickness = 1;
	int baseline = 0;
	double scale = 0.5;

	Size text = getTextSize(str, fontface, scale, thickness, &baseline);
	Rect r = boundingRect(contour);

	Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	rectangle(image, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(200, 200, 200), FILLED);
	putText(image, str, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}


/* 읽어온 사용자 설정 파일에 있는 값을 변수에 저장하는 함수 */
void readFileSetup() {
	int textFileRoute;

	cout << "Reading User File.." << endl;
	textFileRoute = readFile();
	u_angle = parseString(lines, textFileRoute, "over_angle");
	u_circleArea = parseString(lines, textFileRoute, "circle_area");
	u_rectangleArea = parseString(lines, textFileRoute, "rectangle_area");
	u_circleAreaAverage = parseString(lines, textFileRoute, "circle_area_average");
	u_circeError = parseString(lines, textFileRoute, "circe_error");

	for (int i = 0; i < textFileRoute; i++) {
		if (lines[i].find("ip") == string::npos) continue;
		strcpy(g_db_ip, (lines[i].substr(lines[i].find("ip") + string("ip").length() + 1)).c_str());
	}


	cout << "Reading User File Success" << endl << endl;
}


/* 최종 결과를 콘솔로 출력해주는 함수 */
void printResult(Mat &cam, RotatedRect &rect) {

	cout << "center of rectangle : " << rect.center << endl;
	cout << "circle count of rectangle : " << g_contours.size() - 1 << endl;
	printEndLine();
	printLine();
	cout << "rectangle pixel area : " << g_dwidthDistance * g_dheightDistance << endl;
	g_table_values.u_size = g_dwidthDistance * g_dheightDistance;  // DB에 저장
	cout << "rectangle cm area : " << ((g_dwidthDistance * g_dmm) * (g_dheightDistance * g_dmm)) / 100 << endl;
	if (u_rectangleArea < g_dwidthDistance * g_dheightDistance) {
		showError("| rectangle is too big  |");
	}
	cout << "1 pixel is " << g_dmm << "mm" << endl;
	printEndLine();


	String textCircleArea = "Standard Circle Area : " + to_string(u_circleArea);
	putText(cam, textCircleArea, Point(10, 20), 1, 1, Scalar(255, 0, 0), 0);
	String textRectangleArea = "Standard Rectangle Area : " + to_string(u_rectangleArea);
	putText(cam, textRectangleArea, Point(10, 40), 1, 1, Scalar(255, 0, 0), 0);
	String textAngle = "Standard Angle : " + to_string(u_angle);
	putText(cam, textAngle, Point(10, 60), 1, 1, Scalar(255, 0, 0), 0);
	String textCircleAreaAverage = "Standard Circle Average Area: " + to_string(u_circleAreaAverage);
	putText(cam, textCircleAreaAverage, Point(10, 80), 1, 1, Scalar(255, 0, 0), 0);
	String textCirceError = "Standard Circe Error : " + to_string(u_circeError);
	putText(cam, textCirceError, Point(10, 100), 1, 1, Scalar(255, 0, 0), 0);

}


/* PC와 Socket으로 연결되기까지 일정 시간을 대기하는 함수 */
void sockTimeout(int signal) {
	if ((g_clnt_sock == 0)) {  // 연결 안되면?
		cout << "timeout" << endl;
		g_sock_flag = false;
		close(g_clnt_sock);
		close(g_serv_sock);
	}
	else {  // 연결되면?
		g_sock_flag = true;
		digitalWrite(RUNNING_LED, HIGH);
	}
	cout << "Press button" << endl;
	g_startFlag = true;
}


/* PC에서 Socket으로 보내준 텍스트를 Raspberry Pi에 있는 텍스트 파일에 저장해주는 함수 */
void fileSave() {
	ofstream writeFile(filepath);
	if (writeFile.is_open()) {
		writeFile.write(g_socket_buf, 2048);
		writeFile.close();
	}
	cout << "file save complete!" << endl;
}


/* Socket 통신으로 PC에게 텍스트 파일 요청을 보내는 함수 */
void sockGetText() {
	send(g_clnt_sock, "10", sizeof("10"), 0);
	recv(g_clnt_sock, &g_socket_buf, 2048, MSG_WAITALL);
	cout << "sock read complete!" << endl;
	printf("%s\n", g_socket_buf);
}


/* 한줄 긋기 */
void printLine() {
	cout << "------------------------------------------------" << endl;
}


/* 굵게 한줄 긋기 */
void printEndLine() {
	cout << "================================================" << endl << endl << endl;
}


/* MySQL 연결 */
void connectMySQL() {
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char query[256];

	struct connection_details mysqlD;
	mysqlD.server = g_db_ip;
	mysqlD.user = "root";
	mysqlD.password = "1234";
	mysqlD.database = "dworld_rpi_connection";

	conn = mysql_connection_setup(mysqlD);

	if (!g_db_flag) {
		return;
	}

	g_table_values.u_product_num++;
	//cout << "Database connected" << endl;

	//query = "insert into dworld_table(u_date, u_time, u_product_num, u_size, u_angle, u_error, u_image) " + "values(" + g_table_values.u_date + ", " + g_table_values.u_time + ", " + g_table_values.u_product_num + ", " + g_table_values.u_size + ", " + g_table_values.u_angle + ", " + g_table_values.u_error + ", '" + g_table_values.u_image + "');";
	sprintf(query,
		"insert into dworld_table(u_date, u_time, u_product_num, u_size, u_angle, u_error, u_image) values(\"%s\",\"%s\",%d,%d,%lf,%d,\"%s\")",
		g_table_values.u_date.c_str(), g_table_values.u_time.c_str(), g_table_values.u_product_num,
		g_table_values.u_size,
		g_table_values.u_angle, g_table_values.u_error, g_table_values.u_image.c_str());
	//cout << query << endl;
	res = mysql_perform_query(conn, query);

	mysql_free_result(res);
	mysql_library_end();
	mysql_close(conn);

}


/* DB에 들어갈 데이터 값들을 모아놓은 구조체를 초기화해주는 함수 */
void initTableValues() {
	if (g_table_values.u_date.compare(currentDateTime(0)) != 0) {
		g_table_values.u_product_num = 1;
	}
	g_table_values.u_date = currentDateTime(0);
	g_table_values.u_time = currentDateTime(1);
	g_table_values.fileTime = currentDateTime(2);
	g_table_values.fileDate = currentDateTime(4);
	g_table_values.u_size = 0;
	g_table_values.u_angle = 0.000;
	g_table_values.u_error = false;
	g_table_values.u_image = "";
}


/* 영상 전처리 함수 */
int preProcessing(Mat &cam2) {

	// Grab한 영상 전처리
	if (!capVideo(&cam2)) {  // 카메라가 정상적으로 없다면?
		errFlag = 1;
		return 1;
	}
	cvtColor(cam2, cam2, COLOR_GRAY2RGB);
	cout << "Getting Camera Image Success" << endl << endl << endl << endl << endl << endl << endl << endl;

	return 0;
}


/* 사용자 설정 파일을 읽어오는 함수 */
int readFile() {
	int i = 0;

	ifstream openFile(filepath);
	if (openFile.is_open()) {
		string line;
		while (getline(openFile, line)) { lines[i++] = line; };
		openFile.close();
	}
	return i;
}


/* 사용자가 입력한 변수명을 문자열에서 찾아오는 함수 */
int parseStringValue(string str, string key) {
	if (str.find(key) == string::npos) return 0;
	return atoi(str.substr(str.find(key) + key.length() + 1).c_str());
}


/* 사용자가 입력한 변수명을 Raspberry Pi 파일 안에서 찾아오는 함수 */
int parseString(string *lines, int cnt, string key) {
	for (int i = 0; i < cnt; i++) {
		if (int value = parseStringValue(lines[i], key)) {  // value가 Null이 아니라면?
			return value;
		}
	}
	return 0;
}


/* Basler Camera에서 이미지를 Grab하여 받아오고, Mat형으로 변환해주는 함수 */
int capVideo(Mat *openCvImage) {

	try {
		CGrabResultPtr ptrGrabResult;
		CPylonImage image;
		CImageFormatConverter fc;
		CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());

		camera.MaxNumBuffer = 5;
		camera.StartGrabbing(1);
		fc.OutputPixelFormat = PixelType_Mono8;
		fc.OutputBitAlignment = OutputBitAlignment_MsbAligned;


		while (camera.IsGrabbing()) {

			camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

			if (ptrGrabResult->GrabSucceeded()) {
				fc.Convert(image, ptrGrabResult);
				const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();

				*openCvImage = Mat(966, 1296, CV_8UC1, (uint32_t *)image.GetBuffer()).clone();

#ifdef PYLON_WIN_BUILD
				// Display the grabbed image.
				//Pylon::DisplayImage(1, ptrGrabResult);
#endif
			}
			else {
				cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription()
					<< endl;
			}
		}
	}
	catch (const GenericException &e) {
		// Error handling.
		cerr << "An exception occurred." << endl
			<< e.GetDescription() << endl;
		//*openCvImage = imread(g_path, CV_8UC1);
		errFlag = 1;
		return 0;
	}

	cout << "camera image return" << endl;
	return 1;
}


/* 타겟이 있는지 없는지 판단해주는 함수 */
int getType() {

	for (size_t i = 0; i < g_contours.size(); i++) {
		if (fabs(contourArea(Mat(g_approxxxxxxxxx[i]))) > 5000)   // 면적이 일정크기 이상이어야 한다.
		{
			int size = g_approxxxxxxxxx[i].size();
			if (size == 4 && isContourConvex(Mat(g_approx))) {  // 사각형일경우
																//cout << "사각형 " << endl;
				return 0;  // 사각형 하나만 처리하도록 설정
			}
		}
	}

	return 1;  // 사각형 없을경우 오류
}


/* 전처리된 영상을 받아와 다양한 연산을 해주는 함수 */
int calculateImage(Mat &cam, Mat &bin, RotatedRect &rect) {
	int downIndex;
	int leftIndex;

	int convertNum = convertToBinary(cam, bin);  // 이진화

	if (convertNum) {
		return 2;
	}


	getContours(bin);  // 꼭지점 및 외곽선 따기


	if (getType()) {  // 사각형이 없으면?
					  /* 오류 */
		errFlag = 1;
		digitalWrite(NG_LED, HIGH);
		return 1;
	}


	printLine();
	getHorizontalLine();  // 가로선 구하기
	printEndLine();


	printLine();
	// 사각형 제일 아래에 있는 점 찾기
	Point downPoint = g_approx[0];
	for (int i = 1; i < g_approx.size(); i++) {
		if (g_approx[i].y > downPoint.y)
			downPoint = g_approx[i];
	}


	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (downPoint == g_li[i][j]) {  // 사각형 제일 아래있는 점을 포함하는 가로선 찾기
				downIndex = g_li[i][0].y > g_li[i][1].y ? 0 : 1;  // 제일 아래 점 가져오기
				leftIndex = g_li[!i][0].x < g_li[!i][1].x ? 0 : 1;  // 제일 아래 점 가져오기

				drawAngle(cam, i, downIndex, leftIndex);  // 각도 표기
				i = 2;  // break
				j = 2;  // break
			}
		}
	}


	rect = minAreaRect(g_approx);  // 외접 사각형 구하기
	line(cam, rect.center, rect.center, Scalar(0, 255, 0), 5);  // 구한 외접 사각형 중앙에 점찍기

	drawContours(cam, g_contours, g_bigCircleIndex, Scalar(255, 0, 0), 1, 8, g_hierarchy, 0, Point());
	drawContours(cam, g_contours, g_smallCircleIndex, Scalar(0, 0, 255), 1, 8, g_hierarchy, 0, Point());

	cam = cam(Rect(rect.center.x - 280, rect.center.y - 280, 560, 560));
	bin = bin(Rect(rect.center.x - 280, rect.center.y - 280, 560, 560));


	cout << "1 : " << g_bigCircleIndex << endl;
	cout << "2 : " << g_smallCircleIndex << endl;

	return 0;
}


/* 각도를 구해서 반환해주는 함수 */
double getAngle(Point start, Point end) {
	double dy = end.y - start.y;
	double dx = end.x - start.x;

	double angle = abs(atan2(dy, dx) * (180.0 / CV_PI));

	if (angle > 90) {
		angle = 180 - angle;
	}

	return angle;
}


/* 가로선인지 세로선인지 판단해주는 함수 (가로선 : true, 세로선 : false) */
bool getLineType(Point a, Point b) {
	if ((getAngle(b, a) < 45) || (getAngle(b, a) > 135)) {  // 각도비교
		areaOfRectangle(b, a, 1);
		return true;  // 가로선 true
	}
	else {
		areaOfRectangle(b, a, 0);
		return false;  // 세로선 false
	}
}


/* 밝기에 따라 자동으로 이진화 threshold 값을 조절해주는 함수 */
int convertToBinary(Mat &source, Mat &bin) {

	int graySum = 0, loopNum = 0, grayAverage = 0;
	Mat gray;

	cvtColor(source, gray, COLOR_RGB2GRAY);

	for (int i = 0; i < gray.rows; i += 10) {
		for (int j = 0; j < gray.cols; j += 10) {
			graySum += gray.at<uchar>(i, j);
			loopNum++;
		}
	}

	grayAverage = graySum / loopNum;
	cout << "Gray value average is " << grayAverage << endl;

	if (grayAverage <= 140 && grayAverage > 50)  // 밝기가 너무 낮다면 경고
		cout << "Brightness is too low, Target may not be found." << endl;
	else if (grayAverage >= 250)  // 밝기가 너무 높다면 경고
		cout << "Brightness is too high, Target may not be found." << endl;
	else if (grayAverage <= 50) {
		cout << "Brightness is too low, Can't find target." << endl;
		errFlag = true;
		digitalWrite(NG_LED, HIGH);
		return 1;
	}


	threshold(gray, gray, grayAverage / 2, 255, THRESH_BINARY_INV);  // 이진화

	bin = gray;

	return 0;
}


/* 프로그램 종료 함수 */
void quitMain(int signal) {

	digitalWrite(RUNNING_LED, LOW);
	digitalWrite(NG_LED, LOW);
	digitalWrite(OK_LED, LOW);

	close(g_clnt_sock);
	close(g_serv_sock);

	cout << "exit(ctrl + c)" << endl;

	exit(1);
}


int main() {

	Mat cam, bin;
	RotatedRect rect;
	clock_t start, end;
	double result;


	PylonInitialize();  // Pylon 셋업
	wiringPiSetupGpio();  // 라즈베리파이 버튼, LED 처리를 위한 GPIO 라이브러리 셋업


	/* -------------------- 핀모드 설정, 초기화 -------------------- */
	pinMode(RUNNING_LED, OUTPUT);  // LED를 위해 출력으로 설정
	pinMode(NG_LED, OUTPUT);
	pinMode(OK_LED, OUTPUT);
	pinMode(BTN, INPUT);  // 버튼을 위해 입력으로 설정
	pullUpDnControl(BTN, PUD_UP);

	digitalWrite(RUNNING_LED, LOW);
	digitalWrite(NG_LED, LOW);
	digitalWrite(OK_LED, LOW);
	/* -------------------- 핀모드 설정, 초기화 끝 -------------------- */




	/* -------------------------------- 소켓 연결 --------------------------------*/

	signal(SIGALRM, sockTimeout);
	signal(SIGINT, quitMain);

	g_serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	if (g_serv_sock == -1)
		cout << ("socket() error") << endl;

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(8080);

	if (bind(g_serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		cout << ("bind() error") << endl;

	if (listen(g_serv_sock, 5) == -1)
		cout << ("listen() error") << endl;

	alarm(5);

	socklen_t clnt_addr_sz = sizeof(clnt_addr);
	g_clnt_sock = accept(g_serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_sz);

	cout << "accept" << endl;

	/* -------------------------------- 소켓 연결 끝 --------------------------------*/






	runningFlag = 1;

	while (!g_startFlag);


	while (true) {

		errFlag = 0;  // 검사가 모두 끝난 이후엔 에러플래그 0으로 초기화

		int btnflag = digitalRead(BTN);

		if (btnflag) {  // 버튼 안눌렸을 때
			if (cam.empty() == 0) {  // 최근에 검사를 한 기록이 있다면 최근에 검사했던 결과를 띄워준다
				imshow("cam", cam);  // 이미지가 비어있지 않다면 계속 이미지를 띄워준다
				imshow("bin", bin);

				if (waitKey(1) == 97) {  // A키 누르면 종료
					break;
				}
			}

			g_btnCalFlag = true;
			digitalWrite(RUNNING_LED, HIGH);
		}
		else if ((!btnflag) && g_btnCalFlag) {  // 버튼을 눌렀다면?

			start = clock(); // 수행 시간 측정 시작


			initTableValues();


			digitalWrite(RUNNING_LED, LOW);  // 버튼 누르라는 신호의 LED를 끈다
			digitalWrite(OK_LED, LOW);
			digitalWrite(NG_LED, LOW);
			cout << "You pressed the button" << endl;


			if (g_sock_flag) {  // PC랑 연결되어 있다면?
				sockGetText();
				fileSave();
			}


			// 사용자가 작성한 파일을 읽어옴
			readFileSetup();


			try {
				int resultPreProcessing = preProcessing(cam);
				int resultCalculate = calculateImage(cam, bin, rect);
				string err;

				if (resultPreProcessing) {
					err = "Camera not found";
					throw err;
				}
				else if (resultCalculate == 1) {
					err = "Target not found";
					cam.release();
					throw err;
				}
				else if (resultCalculate == 2) {
					err = "Brightness";
					cam.release();
					throw err;
				}
			}
			catch (string err) {
				cout << err << endl;
				cout << "Press button" << endl;
				continue;
			}



			// 결과 출력
			printResult(cam, rect);


			if (errFlag == 0)  // 결함이 아무것도 없다면?
				digitalWrite(OK_LED, HIGH);
			else
				digitalWrite(NG_LED, HIGH);


			//imshow("cam", cam);
			//imshow("bin", bin);


			if (g_table_values.u_error || g_db_flag) {  // 에러가 있다면 이미지 경로 저장
				string imagePath = "/home/pi/mount_test/" + g_table_values.fileDate + g_table_values.fileTime + ".jpg";
				imwrite(imagePath, cam);
				g_table_values.u_image = "C:/Users/user/Desktop/dworld/winform_image/" + g_table_values.fileDate
					+ g_table_values.fileTime + ".jpg";
				cout << "saving image at " << g_table_values.fileDate << g_table_values.fileTime << endl;
			}

			//waitKey(1);





			g_printHeightFlag = 0;
			g_printWidthFlag = 0;
			cout << "Press button" << endl;

			g_btnCalFlag = false;

			connectMySQL();  // DB로 값을 보냄




			end = clock();  // 시간 측정 끝
			result = (double)(end - start);  // 결과 출력
			cout << result / 1000 << "millisecond" << endl;
			cout << result / 1000000 << "seconds" << endl;
		}

	}

	digitalWrite(RUNNING_LED, LOW);
	digitalWrite(NG_LED, LOW);
	digitalWrite(OK_LED, LOW);

	waitKey(0);

	close(g_clnt_sock);
	close(g_serv_sock);

}