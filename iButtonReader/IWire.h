#ifndef IWIRE_H_
#define IWIRE_H_

// INCLUDES
#include "rom.h"
#include <list>

/* ========= DEFINES ========= */
#define READ_ROM 0x33
#define MATCH_ROM 0x55
#define SKIP_ROM 0xCC
#define RESUME_ROM 0xA5
#define OVERDRIVE_SKIP_ROM 0x3C
#define OVERDRIVE_MATCH_ROM 0x69

class IWireDevice;

typedef list<IWireDevice*> DeviceList;

/*
 * Класс, описывающий основные параметры сети IWire
 */
class IWire
{
	public:
		enum Speed		{NormalSpeed , OverdriveSpeed};
		enum PowerMode	{NormalMode , PowerDeliveryMode};
		enum Type		{eDS1977 , eDS1991 , eDS1992 , eDS1996 , eUnknownDevice};
};



class IWireNetwork
{
	public :
		IWireNetwork(){};
		virtual ~IWireNetwork(){};

		// IWire Basic operations
		virtual bool Reset() = 0;
		virtual int	 Search( DeviceList& ) = 0;

		virtual bool ReadByte( unsigned char& ) = 0;
		virtual bool WriteByte( unsigned char) = 0;

		virtual bool ReadBit( unsigned char&) = 0;
		virtual bool WriteBit( unsigned char) = 0;

		// IWire Advansed operations
		virtual bool setPowerMode( IWire::PowerMode) = 0;
		virtual bool setSpeed(IWire::Speed) = 0;
		virtual bool ProgramPulse() = 0;

		virtual bool ReadBitPower(unsigned char&) = 0;
		virtual bool WriteBitPower(unsigned char) = 0;
};


/*контролируемое устройство в сети 1-Wire*/
class IWireDevice
{
	public:
		virtual ~IWireDevice(){};

		//Общие функции всех устройств
		virtual IWire::Type Type() = 0;
	protected:
		IWireNetwork 	*network;
		ROM				rom;

	protected:
		IWireDevice(IWireNetwork& _net, ROM& _rom): network(&_net) , rom(_rom) {};
		//Device Rom Function
		int		MatchRom( );
		int		SkipRom( );
		int		Resume( );
		int		OverdriveSkip( ){return 0;};
		int 	OverdriveMatch( ){return 0;};
};

#endif /* IWIRE_H_ */
