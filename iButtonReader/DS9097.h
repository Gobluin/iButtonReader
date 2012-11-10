#ifndef DS9097_H_
#define DS9097_H_

// INCLUDES
#include <vector>
#include "ComPort.h"
#include "IWire.h"
#include "rom.h"

// DEFINES
#define TIMING_COMMAND 0xC1
#define COMMAND_MODE 0xE3
#define DATA_MODE 0xE1
#define READ_SPEED_COMMAND 0x0F

#define SEARCH_COMMAND 0xF0
#define SEARCH_START 0xB5
#define SEARCH_STOP 0xA5
#define SEARCH_OK 1
#define SEARCH_NODATA 0
#define SEARCH_ERROR -1



using namespace std;
class DS9097 : public IWireNetwork
{
	public:
		enum Mode{ DataMode = 0xE1 , CommandMode = 0xE3};

	public:
		DS9097(char* portName = "/dev/ttyS0" );
		~DS9097();

		bool Detect();
		bool setSpeed(ComPort::eSpeed);

		bool Reset();
		int Search( DeviceList& );

		bool ReadByte( unsigned char& );
		bool WriteByte( unsigned char);

		bool ReadBit( unsigned char&);
		bool WriteBit( unsigned char);

		bool setPowerMode( IWire::PowerMode){return false;};
		bool setSpeed(IWire::Speed){return false;};
		bool ProgramPulse(){return false;};

		bool ReadBitPower(unsigned char&){return false;};
		bool WriteBitPower(unsigned char){return false;};


	protected:
		unsigned char TouchByte( unsigned char ){return false;};

		int	SearchStep(unsigned int , bool , ROM);
		int FillRequestData(unsigned char* , ROM& , int);

	private:
		ComPort 			*port;			// порт, к которому подсоеденён DS9097
		Mode				currentMode;	// режим, в котором работает DS9097

		IWire::Speed		speed;
		IWire::PowerMode	powerMode;
		unsigned char 		command[100];
		unsigned char 		responce[100];
		unsigned int  		commandLen;

		vector<ROM>			foundedRom;
		DeviceList		devices;
		unsigned long long	posBit;
		int					ret;
};

#endif /* DS9097_H_ */
