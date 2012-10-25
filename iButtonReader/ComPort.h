#ifndef COMPORT_H_
#define COMPORT_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>

class ComPort
{
	public:
		enum eSpeed{ eB9600 , eB19200 , eB57600 , eB115200 };
	public:
		ComPort( char* _file= "/dev/ttyS0" , ComPort::eSpeed _speed = ComPort::eB9600);
		virtual ~ComPort();

		int Open();
		void Close();
		int setSpeed( eSpeed );
		int Read( unsigned char* , unsigned int);
		int Write( unsigned char* , unsigned int);
		bool isOpened();
		void Flush();
		void Break();

	private:
		char 			*file;
		int 			port;
		termios			portOptions;
		fd_set			selectDescriptors;
		timeval 		timeout;
		eSpeed			speed;

	private:
		void setSpeedToOptions( termios& , eSpeed&);
};

#endif /* COMPORT_H_ */
