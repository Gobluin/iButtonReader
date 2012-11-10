//	Этот файл является частью проекта ПС "Конструктор".
//	Время создания: Втр Авг 28 03:54:40 2012

#include <iostream>
#include <stdio.h>

using namespace std;

#include "ds9097.h"
#include "ComPort.h"

#include <stdio.h>

int main( int argc, char ** argv )
{
	DS9097 device("/dev/ttyS1");
	if( !device.Detect() )
	{
		cout<<"Error"<<endl;
		return 1;
	}
	cout<<"Detected"<<endl;

	if( !device.Reset() )
	{
		cout<<"No devices in iWire"<<endl;
		return 1;
	}
	SlaveDeviceList foundedDevises;
	cout<<device.Search( foundedDevises )<<endl;
	
return 0;
}
