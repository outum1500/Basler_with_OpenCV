#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#define SERVERIP   "192.168.0.113"
#define SERVERPORT 8080

#pragma warning(disable:4996)

using namespace std;
char buf[80]; //버퍼 안으로 넣음
char cstr[2048];
string line;
string STRING = "";

int main(int argc, char* argv[])
{
	int retval;
	// 윈속 초기화

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) exit(1);

	// 소켓 설정
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// 연결 요청
	retval = connect(sock, (SOCKADDR*)& serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) exit(1);

	// 서버와 데이터 통신

	while (1) {

		recv(sock, buf, 80, 0);
		cout << string(buf) << endl;

		line = "";
		STRING = "";

		memset(cstr, 0, sizeof(cstr));

		ifstream infile;
		infile.open("C:\\Users\\user\\Documents\\Visual Studio 2015\\Projects\\Project1\\Project1\\stdio.txt");

		while (!infile.eof()) { // To get you all the lines.
			getline(infile, line); // Saves the line in STRING.
			STRING += line + "\n";
		}
		infile.close();

		cout << STRING << endl;
		strcpy(cstr, STRING.c_str());

		cout << sizeof(cstr) << endl;
		send(sock, cstr, 2048, 0);

	}

	// 소켓 종료
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}