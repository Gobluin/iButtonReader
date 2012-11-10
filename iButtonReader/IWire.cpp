/*
 *
 */
#include "IWire.h"

/*
 *  Получить доступ к внутренним функциям устройства
 *  путём подачи ROM адреса. Только устройство с таким же ROM адресом на шине будет обрабатывать дальнейшие функции
 */
int IWireDevice::MatchRom( )
{
	try
	{
		if( (!network->Reset())  || (!network->setSpeed(IWire::NormalSpeed)) )
			return -1;

		if( (!network->WriteByte(MATCH_ROM)) || (!network->WriteByte(rom.Type())) )
			return -1;

		for(int i=0 ; i<6 ; i++)
		{
			if( !network->WriteByte(rom.Serial()[i]) )
				return -1;
		}

		if( !network->WriteByte(rom.CRC()) )
			return -1;
	}
	catch( ... ){throw;}

	return 1;
}

/*
 * Пропустить инициализацию ROM функций.
 * Пропускаем инициализию устройства и переходим непосредственно к работе с функиями устройства
 * !!!! ВНИМАНИЕ !!! если на шине присутствует несколько устройств - все они начнут одновременно выполнять функции,посланные по шине 1-wire. !!!!
 */
int IWireDevice::SkipRom( )
{
	try
	{
		if( (!network->Reset()) || (!network->setSpeed(IWire::NormalSpeed)) )
			return -1;

		if( !network->WriteByte(SKIP_ROM) )
			return -1;
	}
	catch( ... ){ throw;}

	return 1;
}

/*
 * Пропустить инициализацию, если ранее были инициализированны
 */
int IWireDevice::Resume( )
{
	try
	{
		if( (!network->Reset()) || (!network->setSpeed(IWire::NormalSpeed)) )
			return -1;

		if( !network->WriteByte(RESUME_ROM) )
			return -1;
	}
	catch( ... ){throw;}
	return 1;
}


