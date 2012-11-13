/*
 *
 */

#ifndef IWIREELEMENTCREATOR_H_
#define IWIREELEMENTCREATOR_H_

/* ========= INCLUDES ========= */
#include "IWire.h"
#include "rom.h"

/* ========= DEFINES ========= */

class IWireElementCreator
{
	public:
		static IWireDevice* Create( ROM& , IWireNetwork& );
};

#endif /* IWIREELEMENTCREATOR_H_ */
