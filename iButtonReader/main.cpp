//	Этот файл является частью проекта ПС "Конструктор".
//	Время создания: Втр Авг 28 03:54:40 2012

#include <iostream>
#include <stdio.h>

using namespace std;

#include "ds9097.h"
#include "ComPort.h"
#include "IWireDevices.h"

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
	DeviceList foundedDevises;
	cout<<device.Search( foundedDevises )<<endl;
	
	unsigned char test[] = "1234                                                            ";
	DS1977 *dev = NULL;
	for( DeviceList::iterator i = foundedDevises.begin() ; i!= foundedDevises.end() ; ++i)
		if( (*i)->Type() == IWire::eDS1977 )
			dev = dynamic_cast<DS1977*>((*i));

	if( dev != NULL)
		if(dev->WriteScratchpad(0x0000 , test , 64) == 1)
			cout<<"Запись прошла успешно"<<endl;
		else
			cout<<"Ошибка при записи"<<endl;
return 0;
}
