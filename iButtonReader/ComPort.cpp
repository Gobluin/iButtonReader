/*
 * ComPort.cpp
 *
 *  Created on: 14.09.2012
 *      Author: Shrike
 */

#include "ComPort.h"
#include <cstring>

using namespace std;

ComPort::ComPort(char*_file , ComPort::eSpeed _speed)
{
	port = -1;
	speed = _speed;
	file = new char[ strlen(_file) ];
	memcpy( file , _file , strlen(_file) );
}

ComPort::~ComPort()
{
	if( port != -1 )
		Close();
	port = -1;
	delete [] file;
}


int ComPort::Open()
{
	if( port != -1)
		return 0;

	port = open( file , O_RDWR|O_NONBLOCK);
	if( port == -1)
		return -1;

	if( tcgetattr(port, &portOptions) == -1)
	{
		close(port);
		port = -1;
		return -1;
	}

	setSpeedToOptions( portOptions , speed);

	portOptions.c_iflag &= ~(BRKINT|ICRNL|IGNCR|INLCR|INPCK|ISTRIP|IXON|IXOFF|PARMRK);
	portOptions.c_iflag |= IGNBRK|IGNPAR;
	portOptions.c_oflag &= ~(OPOST);
	portOptions.c_cflag &= ~(CRTSCTS|CSIZE|HUPCL|PARENB);
	portOptions.c_cflag |= (CLOCAL|CS8|CREAD);
	portOptions.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHONL|ICANON|IEXTEN|ISIG);
	portOptions.c_cc[VMIN] = 0;
	portOptions.c_cc[VTIME] = 3;

	if( tcsetattr(port, TCSAFLUSH, &portOptions) == -1)
	{
		close(port);
		port = -1;
		return -1;
	}

	tcflush( port ,TCIOFLUSH);
	return 1;
}



/*
 * Закрыть порт
 */
void ComPort::Close()
{
	if( port == -1)
		return;

	close(port);
	port = -1;
}



/*
 * Установка скорости
 */
int ComPort::setSpeed( eSpeed _speed)
{

	if( port == -1)
		return -1;

	if( speed == _speed)
		return 0;

	speed = _speed;
	setSpeedToOptions(portOptions , speed);
	if( tcsetattr(port, TCSAFLUSH, &portOptions) == -1)
	{
		close(port);
		port = -1;
		return -1;
	}
	tcflush( port ,TCIOFLUSH);
	return 1;
}


/*
 * Метод чтения данных из порта
 */
int ComPort::Read( unsigned char *bufer , unsigned int buferSize)
{
	if( bufer == 0)
		return -1;

	unsigned int count;

	for( count = 0; count< buferSize ; ++count)
	{
		FD_ZERO(&selectDescriptors);
		FD_SET(port , &selectDescriptors);

		timeout.tv_sec =0;
		timeout.tv_usec = 100000;

		if(select(port+1,&selectDescriptors,NULL,NULL,&timeout) != 0 )
		{
			if( read( port , &bufer[count] ,1) != 1)
			{
				close(port);
				port = -1;
				return -1;
			}
		}
		else
			return count;
	}
	return count;
}

int ComPort::Write(unsigned char *data , unsigned int dataSize)
{
	unsigned int sended;
	if( (sended = write( port , data , dataSize)) == -1)
	{
		close(port);
		port = -1;
		return -1;
	}

	tcdrain( port );
	return sended == dataSize ? 1 : 0;
}

bool ComPort::isOpened()
{
	return (port != -1);
}

void ComPort::Flush()
{
	if( port != -1)
		tcflush( port ,TCIOFLUSH);
}

void ComPort::Break()
{
	if( port != -1)
		tcsendbreak(port, 0);
}

void ComPort::setSpeedToOptions(termios& _option, eSpeed& _speed)
{
	int spd = B9600;
	switch( _speed )
	{
		case ComPort::eB9600:
			spd = B9600;
		break;
		case ComPort::eB57600:
			spd = B57600;
		break;
		case ComPort::eB19200:
			spd = B19200;
		break;
		case ComPort::eB115200:
			spd = B115200;
		break;
	}
	cfsetospeed(&_option, spd);
	cfsetispeed(&_option, spd);
}
