#include "ds9097.h"
#include <unistd.h>
#include <string>

DS9097::DS9097( char* portName )
{
	port = new ComPort(portName);
	port->setSpeed( ComPort::eB9600);
	currentMode = DataMode;
	speed = IWire::NormalSpeed;
	powerMode = IWire::NormalMode;
}

DS9097::~DS9097()
{
	// TODO Auto-generated destructor stub
}

/*
 * Метод обнаружения устройства и синхронизации его с портом
 */
bool DS9097::Detect()
{
	if( !port->isOpened() )
		if(port->Open() != 1)
			return false;

	if( port->setSpeed( ComPort::eB9600) == -1 )
		return false;

	port->Break();
	usleep(2);
	port->Flush();

	command[0] = TIMING_COMMAND;
	if( port->Write(command , 1) != 1)
		return false;

	usleep(4);
	command[0] = 0x17;
	command[1] = 0x45;
	command[2] = 0x5B;
	command[3] = 0x0F;
	command[4] = 0x91;
	port->Flush();


	if(port->Write(command , 5) != 1)
		return false;
	if(port->Read(responce , 5) != 5)
		return false;

	if( (responce[0] != 0x16) || (responce[1] != 0x44) || (responce[2] != 0x5A) || (responce[3] != 0x00) || (responce[4] != 0x93) )
		return false;
	return true;
}

/*
 * Установить скорость передачи данных на соответствующее количество бит/с
 */
bool DS9097::setSpeed( ComPort::eSpeed newSpeed)
{
	unsigned char spd;
	commandLen =0 ;
	//формируем команду
	if( currentMode != DS9097::CommandMode)
		command[commandLen++] = COMMAND_MODE; // переключение в режим команд
	command[commandLen]= 0x71; // установка скорости
	switch(newSpeed)
	{
		case ComPort::eB9600:
			spd = 0x00;
		break;
		case ComPort::eB19200:
			spd = 0x02;
		break;
		case ComPort::eB57600:
			spd = 0x04;
		break;
		case ComPort::eB115200:
			spd = 0x06;
		break;
		default:
			return false;
	}
	command[commandLen++] |= spd;

	if( port->Write(command , commandLen) != 1)
		return false;

	currentMode = CommandMode;
	usleep(5);
	if( port->setSpeed(newSpeed) == -1)
		return false;
	usleep(5);
	port->Flush();

	command[0] = READ_SPEED_COMMAND; // команда чтения скорости

	if( port->Write(command , 1) != 1)
		return false;
	if(port->Read(responce, 1) != 1)
		return false;

	if(responce[0] != spd)
	{
		Detect();
		return false;
	}
	return true;
}



/* Сброс */
bool DS9097::Reset()
{
	commandLen = 0;
	// смотрим режим и добовляем переход в режим команд, если надо
	if(currentMode != CommandMode)
		command[commandLen++] = COMMAND_MODE;

	switch( speed )
	{
		case IWire::NormalSpeed:
			command[commandLen++] = 0xC1|0x4;
		break;
		case IWire::OverdriveSpeed:
			command[commandLen++] = 0xC1|0x8;
		break;
	}

	port->Flush();

	if(port->Write(command , commandLen) != 1)
		return false;

	currentMode = CommandMode;

	if(port->Read(responce , 1) != 1)
		return false;

	// responce format   11xvvvrr; x - undefined;
	// vvv - version: 010 - DS2480, 011 - DS2480B, 100 - future; rr - reset result: 00 - 1-Wire shorted, 01 - presence, 10, alarm presence , 11 - no presence.
	if( (responce[0] & 0xDC) != 0xCC)
		return false;

	responce[0] &= 0x3;
	if( (responce[0] != 1) &&(responce[0] != 2) )
		return false;

	return true;
}

bool DS9097::Search()
{
	commandLen = 0;

	if( currentMode != DataMode )
		command[commandLen++] = DATA_MODE;
	command[commandLen++] = SEARCH_COMMAND;

	if( port->Write( command , commandLen) )
		return false;
	currentMode = CommandMode;
	return true;
}
/*
 * Метод чтения данных из сети 1-Wire
 */
bool DS9097::ReadByte( unsigned char& dest)
{
	commandLen = 0;
	if(currentMode != DataMode)
		command[commandLen++] =DATA_MODE;

	command[commandLen++] = 0xFF;

	port->Flush();
	if( port->Write(command, commandLen) != 1)
		return false;

	currentMode = DataMode;

	if(port->Read(responce , 1) != 1)
		return false;

	dest = responce[0];
	return true;
}

/*
 * Метод записи байта в сеть 1-Wire
 */
bool DS9097::WriteByte(unsigned char sndByte)
{
	commandLen = 0;
	if(currentMode != DataMode)
		command[commandLen++] = DATA_MODE;

	command[commandLen++] = sndByte;
	if(sndByte == CommandMode)
		command[commandLen++] = sndByte;

	// посылаем команду
	port->Flush();
	if(port->Write(command , commandLen) != 1)
		return false;

	currentMode = DataMode;

	if(port->Read(responce , 1) != 1)
		return false;

	if( responce[0] != sndByte)
	{
		Detect();
		return false;
	}

	return true;
}

/*
 * Метод отправки бита в сеть 1-Wire
 */
bool DS9097::WriteBit( unsigned char wBit)
{
	commandLen = 0;

	if(currentMode != CommandMode)
		command[commandLen++] = COMMAND_MODE;

	switch(speed)
	{
		case IWire::NormalSpeed :
			command[commandLen ] = 0x81 | 0x04;
		break;
		case IWire::OverdriveSpeed:
			command[commandLen ] = 0x81 | 0x08;
		break;
	}

	command[commandLen++] |= (wBit&0x1)<<4;

	port->Flush();
	if(port->Write(command , commandLen) != 1)
		return false;

	currentMode = CommandMode;

	if(port->Read(responce , 1) != 1)
		return false;

	if( (responce[0]&0xFC) != (command[commandLen -1] & 0xFC) )
		return false;

	if( ((wBit&0x1<<1)|(wBit&0x1)) != (responce[0]&0xFC) )
		return false;
	return true;
}

/*
 * Метод чтения бита из сети 1-Wire
 */
bool DS9097::ReadBit( unsigned char &rBit )
{
	commandLen = 0;

	if(currentMode != CommandMode)
		command[commandLen++] = COMMAND_MODE;

	switch(speed)
	{
		case IWire::NormalSpeed :
			command[commandLen ] = 0x81 | 0x04;
		break;
		case IWire::OverdriveSpeed:
			command[commandLen ] = 0x81 | 0x08;
		break;
	}
	// команда на чтение данных
	command[commandLen++] |= (0x1<<4);

	port->Flush();
	if(port->Write(command , commandLen) != 1)
		return false;

	currentMode = CommandMode;

	if(port->Read(responce , 1) != 1)
		return false;

	if( (responce[0]&0xFC) != (command[commandLen -1] & 0xFC) )
		return false;

	if( (responce[0]&0x3) == 0 )
	{
		rBit = 0;
		return true;
	}

	if( (responce[0]&0x3) == 0x3 )
	{
		rBit = 1;
		return true;
	}
	return false;
}




int DS9097::SearchStep( unsigned int bitPosition , bool newSerch , ROM findedRom)
{
	if( bitPosition == ROM_LENGTH)
	{
		foundedRom.pushBack(findedRom);
		return SEARCH_OK;
	}

	commandLen = 0 ;
	if( currentMode != DataMode)
		command[commandLen++] = DATA_MODE;

	command[commandLen++] = SEARCH_COMMAND;
	command[commandLen++] = COMMAND_MODE;
	command[commandLen++] =
}
