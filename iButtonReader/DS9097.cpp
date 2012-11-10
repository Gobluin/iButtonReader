#include "ds9097.h"
#include <unistd.h>
#include <string>
#include <stdexcept>

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

/*
 * Метод поиска элементов.
 * На выходе получаем количество устройств в сети, либо -1 в случае ошибки
 */

int DS9097::Search( DeviceList& _list)
{
	ROM rom;

	// отчищаем списки предыдущих данных
	_list.clear();
	foundedRom.clear();


	try
	{
		// ищем данные
		if( SearchStep(0 , true , rom) == SEARCH_ERROR)
			return SEARCH_ERROR;


		for( vector<ROM>::iterator i = foundedRom.begin(); i != foundedRom.end() ; ++i )
			_list.push_back( IWireSlaveDevice(this , *i) );
	}
	catch(...){throw;}

	return _list.size();
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


/**/
bool DS9097::setPowerMode( IWire::PowerMode newMode)
{
	if( powerMode == newMode)
		return true;

	return false;
}

/*
 * Установка скорости обмена данными в сети 1-wire
 */
bool DS9097::setSpeed( IWire::Speed newSpeed)
{
	if( speed == newSpeed)
		return true;

	commandLen = 0;
	if( currentMode != CommandMode )
		command[ commandLen++ ] = COMMAND_MODE;

	switch( newSpeed )
	{
		case IWire::NormalSpeed :
			if( !setSpeed(ComPort::eB9600) )
				return false;
			command[ commandLen++ ] = 0xA5;
		break;
		case IWire::OverdriveSpeed :
			if( !setSpeed(ComPort::eB115200) )
				return false;
			command[ commandLen++ ] = 0xA9;
		break;
	}

	port->Flush();
	if( port->Write( command , commandLen) != 1)
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




int DS9097::SearchStep( unsigned int bitPosition , bool newRequest , ROM findedRom)
{
	// если дошли до конца длинны ROM - записываем полученное значение в массив полученных РОМ-ов
	if( bitPosition == ROM_LENGTH)
	{
		if( !findedRom.isValidCRC() )
			return SEARCH_NODATA;
		foundedRom.push_back(findedRom);
		return SEARCH_OK;
	}

	// если взведён флаг нового запроса, то формируем новый запрос с полученными до битами
	// отправляем его, и получаем ответ
	if( newRequest)
	{
		if( !Reset() )
			return SEARCH_ERROR;

		commandLen = 0 ;
		// если текущее состояние не режим данных
		if( currentMode != DataMode)
			command[commandLen++] = DATA_MODE;

		// формируем запрос на поиск серийного номера
		command[commandLen++] = SEARCH_COMMAND;
		command[commandLen++] = COMMAND_MODE;
		command[commandLen++] = SEARCH_START;
		command[commandLen++] = DATA_MODE;
		FillRequestData(command+commandLen,findedRom , bitPosition);
		commandLen += 16;
		command[commandLen++] = COMMAND_MODE;
		command[commandLen++] = SEARCH_STOP;

		// посылаем запрос
		try
		{
			if(port->Write(command , commandLen) != 1)
				return SEARCH_ERROR;
			currentMode = CommandMode;

			if( port->Read(responce , 17) != 17)
				return SEARCH_ERROR;
		}
		catch(...)	{throw;}

		if(responce[0] != SEARCH_COMMAND)
			return SEARCH_ERROR;

	}

	posBit = ( responce[bitPosition/4+1]>>(2*(bitPosition%4)) ) & 0x3;
	try
	{
		if((ret = SearchStep( bitPosition+1 , false , findedRom |((posBit>>1)<<bitPosition) )) == SEARCH_ERROR )
			return SEARCH_ERROR;
	}
	catch(...){throw;}

	posBit = ( responce[bitPosition/4+1]>>(2*(bitPosition%4)) ) & 0x3;
	if( posBit & 0x1 )
	{
		try
		{
			return SearchStep( bitPosition+1 , true , findedRom |(((~posBit>>1)&0x1)<<bitPosition) );
		}
		catch(...){throw;}
	}
	return ret;
}


// метод заполнения данных в запрос
int DS9097::FillRequestData(unsigned char* data, ROM& rom, int bit)
{
	bzero( data , 16);

	for( int i = 0 ; i < bit ; i++)
		data[ i/4] |= ( rom[i] << (2*(i%4)+1) );

	return 1;
}
