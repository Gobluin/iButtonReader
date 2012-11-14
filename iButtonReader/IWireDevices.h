/*
 *
 */

#ifndef IWIREDEVICES_H_
#define IWIREDEVICES_H_

/* ========= INCLUDES ========= */
#include "IWire.h"
#include "rom.h"
#include "IWireElementCreator.h"

/* ========= DEFINES ========= */
#define BUFER_LEN 100

class DS1977 : public IWireDevice
{
	public:
		IWire::Type Type(){ return IWire::eDS1977;};

	private:
		DS1977(IWireNetwork& net, ROM& _rom) : IWireDevice(net , _rom){};

		// devise level functions
		int WriteScratchpad( unsigned short , unsigned char* , unsigned int);
		int ReadScratchpad( unsigned char* , unsigned int);
		int CopyScratchpad( unsigned char* , unsigned int);
		int ReadMemory();
		int VeryfyPassword();

	private:
		unsigned char	password[8];
		unsigned char	bufer[BUFER_LEN];
		unsigned int	len;
		unsigned char	esStatus;
		unsigned char	scrAddres[2];
		unsigned char	crc16[2];
		unsigned char	password[8];

		friend class IWireElementCreator;
};




class DS1991 : public IWireDevice
{
	public:
		IWire::Type Type(){ return IWire::eDS1991;};

	private:
		DS1991(IWireNetwork& net, ROM& _rom) : IWireDevice(net , _rom){};

		friend class IWireElementCreator;
};




class DS1992 : public IWireDevice
{
	public:
		IWire::Type Type(){ return IWire::eDS1992;};

	private:
		DS1992(IWireNetwork& net, ROM& _rom) : IWireDevice(net , _rom){};

		friend class IWireElementCreator;
};




class DS1996 : public IWireDevice
{
	public:
		IWire::Type Type(){ return IWire::eDS1996;};

	private:
		DS1996(IWireNetwork& net, ROM& _rom) : IWireDevice(net , _rom){};

		friend class IWireElementCreator;
};


class UnknownDevice : public IWireDevice
{
	public:
		IWire::Type Type(){ return IWire::eUnknownDevice;};

	private:
		UnknownDevice( IWireNetwork& net, ROM& _rom) : IWireDevice(net , _rom){};

		friend class IWireElementCreator;
};
#endif /* IWIREDEVICES_H_ */
