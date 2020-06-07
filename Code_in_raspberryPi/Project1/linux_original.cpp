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

string g_path = "image4.jpg"; //ī�޶� ������ ��ü�Ǿ� ���� ����
string filepath = "/home/pi/header_test/stdio.txt"; //�ؽ�Ʈ ���� ���
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



/* --------------------------- MySQL ���� ����ü�� �Լ��� --------------------------- */
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
	time_t now = time(0); //���� �ð��� time_t Ÿ������ ����
	struct tm tstruct;
	char time_buf[80];
	char date_buf[80];
	char nontime_buf[80];
	char nondate_buf[80];
	tstruct = *localtime(&now);
	strftime(time_buf, sizeof(time_buf), "%R:%S", &tstruct); // HH:mm:ss ������ ��Ʈ��
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





/* ���� ��� �Լ� */
void showError(char why[30]) {

	errFlag = 1;
	digitalWrite(NG_LED, HIGH);
	digitalWrite(RUNNING_LED, LOW);
	cout << "!!!!!!!! Warning !!!!!!!!" << endl;
	cout << why << endl;
	cout << "-------------------------" << endl;
	g_table_values.u_error = true;

}


/* ����� Ÿ���� ���� �簢���� �׸��� �Լ� */
void drawRotatedRectangle(Mat &image, RotatedRect rect) {
	Point2f vertices[4];
	rect.points(vertices);
	for (int i = 0; i < 4; i++)
		line(image, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255, 0), 2);
}


/* ����ȭ�� �̹����� �ܰ����� �����ϰ�, ����� �������� �� ũ��� �������� �����ϴ� �Լ� */
void getContours(Mat &source) {  // �ܰ����� ������ ��� �Լ�

	int circle_area_average = 0;
	int circle_big = 0, circle_small = 10000;

	findContours(source, g_contours, g_hierarchy, RETR_LIST, CHAIN_APPROX_NONE);  // �ܰ��� ã��

	g_approxxxxxxxxx.resize(g_contours.size());


	for (int i = 0; i < g_contours.size(); i++) {
		drawContours(source, g_contours, i, Scalar(0, 255, 255), 2, 8, g_hierarchy, 0, Point());  // �ܰ��� �׸���
		approxPolyDP(Mat(g_contours[i]), g_approx, arcLength(Mat(g_contours[i]), true) * 0.01,
			true);  // ������ ����1 (approx)
		approxPolyDP(Mat(g_contours[i]), g_approxxxxxxxxx[i], arcLength(Mat(g_contours[i]), true) * 0.002,
			true);  // ������ ����2 (approxxxxxxxxx)


		if (fabs(contourArea(Mat(g_approxxxxxxxxx[i]))) < 2000) {  // ���� ��� �� if�� �ȿ� ���´�

																   // ������ ��� ũ�⸦ ���ϱ� ���� ������ ��� ���Ѵ�
			circle_area_average += fabs(contourArea(Mat(g_approxxxxxxxxx[i])));

			// ������ ����� ������ ���� ���� ���� ũ�Ⱑ ũ�ٸ�?
			if (circle_big < fabs(contourArea(Mat(g_approxxxxxxxxx[i])))) {
				circle_big = fabs(contourArea(Mat(g_approxxxxxxxxx[i])));  // �ִ� ���� ũ�� ����
				g_bigCircleIndex = i;  // �ε����� ����
			}

			// ������ ����� ������ ���� ���� ���� ũ�Ⱑ �۴ٸ�?
			if (circle_small > fabs(contourArea(Mat(g_approxxxxxxxxx[i])))) {
				circle_small = fabs(contourArea(Mat(g_approxxxxxxxxx[i])));  // �ּ� ���� ũ�� ����
				g_smallCircleIndex = i;  // �ε����� ����
			}
		}

		setLabel(source, to_string(i + 1), g_contours[i]);  // ���� ��ȣ �Ű��ֱ�
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
		line(source, g_approx[i], g_approx[i], Scalar(255, 0, 255), 10);  // �������� �����
}


/* ���� ������ ���ؼ��� ��� �̹����� ǥ�����ִ� �Լ� */
void drawAngle(Mat &source, int i, int downIndex, int leftIndex) {

	Point a = g_li[i][downIndex];
	Point b = g_li[i][!downIndex];

	line(source, Point(0, a.y), Point(source.size().width, a.y), Scalar(0, 0, 255), 2, LINE_AA);  // ���ؼ� �׸���

	if (a.x > b.x) {
		line(source, a, b, Scalar(0, 0, 255), 2);  // ���� �� ���̶���Ʈ
		putText(source, to_string(getAngle(a, b)), Point(a.x, b.y - 2), 1, 1, Scalar(255, 0, 0), 2);  // ���� �Է�
	}
	else {
		line(source, a, g_li[!i][leftIndex], Scalar(0, 0, 255), 2);  // ���� �� ���̶���Ʈ
		putText(source, to_string(90 - getAngle(a, b)), Point(a.x, b.y - 2), 1, 1, Scalar(255, 0, 0), 2);  // ���� �Է�
	}

	if (u_angle < getAngle(a, b)) {
		showError("| Angle is too oblique  |");
	}

	g_table_values.u_angle = getAngle(a, b);
}


/* Ÿ���� �ȼ� ��� ���� ũ��� Ÿ���� ����, ���� ���̸� ������ִ� �Լ� */
void areaOfRectangle(Point start, Point end, int iswidth) {

	double dy = end.y - start.y;
	double dx = end.x - start.x;
	double mm;

	if (iswidth) {  // ���� ���̶�� �����
		if (!g_printWidthFlag) {  // ��Ʈ������ �ȼ� ��� ���� ũ�⸦ ���Ѵ�
			g_dwidthDistance = sqrt((dx * dx) + (dy * dy));
			g_dmm = g_drealRectangleLength / g_dwidthDistance;

			cout << "Width distance : " << g_dwidthDistance << endl;
			g_printWidthFlag = 1;
		}
	}
	else {  // ���� ���̶�� �����
		if (!g_printHeightFlag) {
			g_dheightDistance = sqrt((dx * dx) + (dy * dy));
			mm = g_drealRectangleLength / g_dheightDistance;

			cout << "Height distance : " << g_dheightDistance << endl;
			g_printHeightFlag = 1;
		}
	}
}


/* �簢������ ���μ����� �ƴ����� �Ǵ��ϰ�, ���μ��� �� �� ��ǥ�� �������ִ� �Լ� */
void getHorizontalLine() {
	int j = 0;

	for (int i = 1; i < g_approx.size(); i++)  // ���μ����� ���ϱ�(1 ~ MAX-1)
	{
		if (getLineType(g_approx[i - 1], g_approx[i])) {  // ���μ����� ���ϱ�(1 ~ MAX-1)
			g_li[j][0] = g_approx[i];  // ���μ��̸� ����
			g_li[j][1] = g_approx[i - 1];  // ���μ��̸� ����
			j++;  // �ε��� ++
		}
	}

	if (getLineType(g_approx[0], g_approx[g_approx.size() - 1])) {  // ���μ����� ���ϱ�(0, MAX)
		g_li[j][0] = g_approx[0];  // ���μ��̸� ����
		g_li[j][1] = g_approx[g_approx.size() - 1];  // ���μ��̸� ����
	}
}


/* ����� ������ ��ü�� �޾ƿ���, ��� �̹����� �� ��° ��ü���� �� ��ü���� �󺧸� ���ִ� �Լ� */
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


/* �о�� ����� ���� ���Ͽ� �ִ� ���� ������ �����ϴ� �Լ� */
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


/* ���� ����� �ַܼ� ������ִ� �Լ� */
void printResult(Mat &cam, RotatedRect &rect) {

	cout << "center of rectangle : " << rect.center << endl;
	cout << "circle count of rectangle : " << g_contours.size() - 1 << endl;
	printEndLine();
	printLine();
	cout << "rectangle pixel area : " << g_dwidthDistance * g_dheightDistance << endl;
	g_table_values.u_size = g_dwidthDistance * g_dheightDistance;  // DB�� ����
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


/* PC�� Socket���� ����Ǳ���� ���� �ð��� ����ϴ� �Լ� */
void sockTimeout(int signal) {
	if ((g_clnt_sock == 0)) {  // ���� �ȵǸ�?
		cout << "timeout" << endl;
		g_sock_flag = false;
		close(g_clnt_sock);
		close(g_serv_sock);
	}
	else {  // ����Ǹ�?
		g_sock_flag = true;
		digitalWrite(RUNNING_LED, HIGH);
	}
	cout << "Press button" << endl;
	g_startFlag = true;
}


/* PC���� Socket���� ������ �ؽ�Ʈ�� Raspberry Pi�� �ִ� �ؽ�Ʈ ���Ͽ� �������ִ� �Լ� */
void fileSave() {
	ofstream writeFile(filepath);
	if (writeFile.is_open()) {
		writeFile.write(g_socket_buf, 2048);
		writeFile.close();
	}
	cout << "file save complete!" << endl;
}


/* Socket ������� PC���� �ؽ�Ʈ ���� ��û�� ������ �Լ� */
void sockGetText() {
	send(g_clnt_sock, "10", sizeof("10"), 0);
	recv(g_clnt_sock, &g_socket_buf, 2048, MSG_WAITALL);
	cout << "sock read complete!" << endl;
	printf("%s\n", g_socket_buf);
}


/* ���� �߱� */
void printLine() {
	cout << "------------------------------------------------" << endl;
}


/* ���� ���� �߱� */
void printEndLine() {
	cout << "================================================" << endl << endl << endl;
}


/* MySQL ���� */
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


/* DB�� �� ������ ������ ��Ƴ��� ����ü�� �ʱ�ȭ���ִ� �Լ� */
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


/* ���� ��ó�� �Լ� */
int preProcessing(Mat &cam2) {

	// Grab�� ���� ��ó��
	if (!capVideo(&cam2)) {  // ī�޶� ���������� ���ٸ�?
		errFlag = 1;
		return 1;
	}
	cvtColor(cam2, cam2, COLOR_GRAY2RGB);
	cout << "Getting Camera Image Success" << endl << endl << endl << endl << endl << endl << endl << endl;

	return 0;
}


/* ����� ���� ������ �о���� �Լ� */
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


/* ����ڰ� �Է��� �������� ���ڿ����� ã�ƿ��� �Լ� */
int parseStringValue(string str, string key) {
	if (str.find(key) == string::npos) return 0;
	return atoi(str.substr(str.find(key) + key.length() + 1).c_str());
}


/* ����ڰ� �Է��� �������� Raspberry Pi ���� �ȿ��� ã�ƿ��� �Լ� */
int parseString(string *lines, int cnt, string key) {
	for (int i = 0; i < cnt; i++) {
		if (int value = parseStringValue(lines[i], key)) {  // value�� Null�� �ƴ϶��?
			return value;
		}
	}
	return 0;
}


/* Basler Camera���� �̹����� Grab�Ͽ� �޾ƿ���, Mat������ ��ȯ���ִ� �Լ� */
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


/* Ÿ���� �ִ��� ������ �Ǵ����ִ� �Լ� */
int getType() {

	for (size_t i = 0; i < g_contours.size(); i++) {
		if (fabs(contourArea(Mat(g_approxxxxxxxxx[i]))) > 5000)   // ������ ����ũ�� �̻��̾�� �Ѵ�.
		{
			int size = g_approxxxxxxxxx[i].size();
			if (size == 4 && isContourConvex(Mat(g_approx))) {  // �簢���ϰ��
																//cout << "�簢�� " << endl;
				return 0;  // �簢�� �ϳ��� ó���ϵ��� ����
			}
		}
	}

	return 1;  // �簢�� ������� ����
}


/* ��ó���� ������ �޾ƿ� �پ��� ������ ���ִ� �Լ� */
int calculateImage(Mat &cam, Mat &bin, RotatedRect &rect) {
	int downIndex;
	int leftIndex;

	int convertNum = convertToBinary(cam, bin);  // ����ȭ

	if (convertNum) {
		return 2;
	}


	getContours(bin);  // ������ �� �ܰ��� ����


	if (getType()) {  // �簢���� ������?
					  /* ���� */
		errFlag = 1;
		digitalWrite(NG_LED, HIGH);
		return 1;
	}


	printLine();
	getHorizontalLine();  // ���μ� ���ϱ�
	printEndLine();


	printLine();
	// �簢�� ���� �Ʒ��� �ִ� �� ã��
	Point downPoint = g_approx[0];
	for (int i = 1; i < g_approx.size(); i++) {
		if (g_approx[i].y > downPoint.y)
			downPoint = g_approx[i];
	}


	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (downPoint == g_li[i][j]) {  // �簢�� ���� �Ʒ��ִ� ���� �����ϴ� ���μ� ã��
				downIndex = g_li[i][0].y > g_li[i][1].y ? 0 : 1;  // ���� �Ʒ� �� ��������
				leftIndex = g_li[!i][0].x < g_li[!i][1].x ? 0 : 1;  // ���� �Ʒ� �� ��������

				drawAngle(cam, i, downIndex, leftIndex);  // ���� ǥ��
				i = 2;  // break
				j = 2;  // break
			}
		}
	}


	rect = minAreaRect(g_approx);  // ���� �簢�� ���ϱ�
	line(cam, rect.center, rect.center, Scalar(0, 255, 0), 5);  // ���� ���� �簢�� �߾ӿ� �����

	drawContours(cam, g_contours, g_bigCircleIndex, Scalar(255, 0, 0), 1, 8, g_hierarchy, 0, Point());
	drawContours(cam, g_contours, g_smallCircleIndex, Scalar(0, 0, 255), 1, 8, g_hierarchy, 0, Point());

	cam = cam(Rect(rect.center.x - 280, rect.center.y - 280, 560, 560));
	bin = bin(Rect(rect.center.x - 280, rect.center.y - 280, 560, 560));


	cout << "1 : " << g_bigCircleIndex << endl;
	cout << "2 : " << g_smallCircleIndex << endl;

	return 0;
}


/* ������ ���ؼ� ��ȯ���ִ� �Լ� */
double getAngle(Point start, Point end) {
	double dy = end.y - start.y;
	double dx = end.x - start.x;

	double angle = abs(atan2(dy, dx) * (180.0 / CV_PI));

	if (angle > 90) {
		angle = 180 - angle;
	}

	return angle;
}


/* ���μ����� ���μ����� �Ǵ����ִ� �Լ� (���μ� : true, ���μ� : false) */
bool getLineType(Point a, Point b) {
	if ((getAngle(b, a) < 45) || (getAngle(b, a) > 135)) {  // ������
		areaOfRectangle(b, a, 1);
		return true;  // ���μ� true
	}
	else {
		areaOfRectangle(b, a, 0);
		return false;  // ���μ� false
	}
}


/* ��⿡ ���� �ڵ����� ����ȭ threshold ���� �������ִ� �Լ� */
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

	if (grayAverage <= 140 && grayAverage > 50)  // ��Ⱑ �ʹ� ���ٸ� ���
		cout << "Brightness is too low, Target may not be found." << endl;
	else if (grayAverage >= 250)  // ��Ⱑ �ʹ� ���ٸ� ���
		cout << "Brightness is too high, Target may not be found." << endl;
	else if (grayAverage <= 50) {
		cout << "Brightness is too low, Can't find target." << endl;
		errFlag = true;
		digitalWrite(NG_LED, HIGH);
		return 1;
	}


	threshold(gray, gray, grayAverage / 2, 255, THRESH_BINARY_INV);  // ����ȭ

	bin = gray;

	return 0;
}


/* ���α׷� ���� �Լ� */
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


	PylonInitialize();  // Pylon �¾�
	wiringPiSetupGpio();  // ��������� ��ư, LED ó���� ���� GPIO ���̺귯�� �¾�


	/* -------------------- �ɸ�� ����, �ʱ�ȭ -------------------- */
	pinMode(RUNNING_LED, OUTPUT);  // LED�� ���� ������� ����
	pinMode(NG_LED, OUTPUT);
	pinMode(OK_LED, OUTPUT);
	pinMode(BTN, INPUT);  // ��ư�� ���� �Է����� ����
	pullUpDnControl(BTN, PUD_UP);

	digitalWrite(RUNNING_LED, LOW);
	digitalWrite(NG_LED, LOW);
	digitalWrite(OK_LED, LOW);
	/* -------------------- �ɸ�� ����, �ʱ�ȭ �� -------------------- */




	/* -------------------------------- ���� ���� --------------------------------*/

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

	/* -------------------------------- ���� ���� �� --------------------------------*/






	runningFlag = 1;

	while (!g_startFlag);


	while (true) {

		errFlag = 0;  // �˻簡 ��� ���� ���Ŀ� �����÷��� 0���� �ʱ�ȭ

		int btnflag = digitalRead(BTN);

		if (btnflag) {  // ��ư �ȴ����� ��
			if (cam.empty() == 0) {  // �ֱٿ� �˻縦 �� ����� �ִٸ� �ֱٿ� �˻��ߴ� ����� ����ش�
				imshow("cam", cam);  // �̹����� ������� �ʴٸ� ��� �̹����� ����ش�
				imshow("bin", bin);

				if (waitKey(1) == 97) {  // AŰ ������ ����
					break;
				}
			}

			g_btnCalFlag = true;
			digitalWrite(RUNNING_LED, HIGH);
		}
		else if ((!btnflag) && g_btnCalFlag) {  // ��ư�� �����ٸ�?

			start = clock(); // ���� �ð� ���� ����


			initTableValues();


			digitalWrite(RUNNING_LED, LOW);  // ��ư ������� ��ȣ�� LED�� ����
			digitalWrite(OK_LED, LOW);
			digitalWrite(NG_LED, LOW);
			cout << "You pressed the button" << endl;


			if (g_sock_flag) {  // PC�� ����Ǿ� �ִٸ�?
				sockGetText();
				fileSave();
			}


			// ����ڰ� �ۼ��� ������ �о��
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



			// ��� ���
			printResult(cam, rect);


			if (errFlag == 0)  // ������ �ƹ��͵� ���ٸ�?
				digitalWrite(OK_LED, HIGH);
			else
				digitalWrite(NG_LED, HIGH);


			//imshow("cam", cam);
			//imshow("bin", bin);


			if (g_table_values.u_error || g_db_flag) {  // ������ �ִٸ� �̹��� ��� ����
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

			connectMySQL();  // DB�� ���� ����




			end = clock();  // �ð� ���� ��
			result = (double)(end - start);  // ��� ���
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