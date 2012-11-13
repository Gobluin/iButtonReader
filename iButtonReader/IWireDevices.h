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

		int WriteScratchpad( unsigned short , unsigned char* , unsigned int);
	private:
		DS1977(IWireNetwork& net, ROM& _rom) : IWireDevice(net , _rom){};

		// devise level functions

		int ReadScratchpad( unsigned short , unsigned char* , unsigned int);
		int CopyScratchpad();
		int ReadMemory();
		int VeryfyPassword();

	private:
		unsigned char password[8];
		unsigned char bufer[BUFER_LEN];
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
