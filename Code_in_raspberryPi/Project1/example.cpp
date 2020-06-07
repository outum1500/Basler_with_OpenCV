//#include <ctime>
//#include <fstream>
//#include <iostream>
//#include <unistd.h>
//#include <raspicam/raspicam.h>
//#include <wiringPi.h>
//
//using namespace std;
//
//const int ledPin = 17;//pin 11(GPIO 17)
//const int butPin = 14;//pin 16(GPIO 23)
//
//
//int main(int argc, char **argv)
//{
//
//	raspicam::RaspiCam Camera;
//
//	wiringPiSetupGpio();
//
//	pinMode(ledPin, OUTPUT);//LED를 위해 출력으로 설정 
//	pinMode(butPin, INPUT);//버튼을 위해 입력으로 설정
//	pullUpDnControl(butPin, PUD_UP);
//
//
//	cout << "now press button" << endl;
//
//
//	while (1)
//	{
//
//		int ret = digitalRead(butPin);
//		if (ret)
//		{
//			digitalWrite(ledPin, LOW);
//		}
//		else
//		{
//			digitalWrite(ledPin, HIGH);
//		}
//	}
//
//	return 0;
//}