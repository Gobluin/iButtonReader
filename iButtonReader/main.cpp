//	Этот файл является частью проекта ПС "Конструктор".
//	Время создания: Втр Авг 28 03:54:40 2012

#include <iostream>
#include <stdio.h>

using namespace std;

#include "ds9097.h"
#include "ComPort.h"
int main( int argc, char ** argv )
{
	DS9097 device("/dev/usb/ttyUSB0");
	if( !device.Detect() )
	{
		cout<<"Error"<<endl;
		return 1;
	}
	cout<<"Detected"<<endl;
	if( !device.setSpeed(ComPort::eB115200) )
	{
		cout<<"speed error"<<endl;
		return 1;
	}
	cout<<"speed ok"<<endl;

	if( !device.setSpeed(ComPort::eB9600) )
	{
		cout<<"speed error"<<endl;
		return 1;
	}
	cout<<"speed ok"<<endl;
	if( !device.setSpeed(ComPort::eB115200) )
	{
		cout<<"speed error"<<endl;
		return 1;
	}
	cout<<"speed ok"<<endl;
	if( !device.Detect() )
	{
		cout<<"Error"<<endl;
		return 1;
	}
	cout<<"Detected"<<endl;
return 0;
}
