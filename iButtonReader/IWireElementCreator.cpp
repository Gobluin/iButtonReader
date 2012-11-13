#include "IWireElementCreator.h"
#include "IWireDevices.h"


IWireDevice* IWireElementCreator::Create(ROM & rom, IWireNetwork& net)
{
	switch( rom.Type() )
	{
		case 0x37:
			return new DS1977(net , rom);
		case 0x02:
			return new DS1991(net , rom);
		case 0x08:
			return new DS1992(net , rom);
		case 0x0C:
			return new DS1996(net , rom);
	}
	return new UnknownDevice(net , rom);
}
