/*
 *
 */
#include "IWireDevices.h"
#include "crc.h"

// объ€вл€ем адреса и длинны, характерные дл€ DS1977
#define MEMORY_ADDRES 0x0000
#define READ_PASSWORD_ADDRES 0x7FC0
#define RW_PASSWORD_ADDRES 0x7FC8
#define PASSWORD_CONTROL_ADDRES 0x7FD0

#define SCRATCHPAD_LEN 64
#define MEMORY_LEN 32704
#define PASSWORD_LEN 8

//определ€ем команды DS1977
#define WRITE_SCRATCHPAD 0x0F
#define	READ_SCRATCHPAD 0xAA
#define	COPY_SCRATCHPAD 0x99
#define	READ_MEMORY 0x69
#define	VERIFY_PASSWORD 0xC3

int DS1977::WriteScratchpad( unsigned short addres, unsigned char* data, unsigned int dataLen)
{
	if ( ((addres&0x3F)+dataLen) > SCRATCHPAD_LEN )
		return 0;

	len = 0;
	// инициализируем таблетку
	if( !network->Reset() || (MatchRom() != 1) )
		return -1;

	// формируем команду на выполнение
	bufer[len++] = WRITE_SCRATCHPAD;
	bufer[len++] = addres&0xff;
	bufer[len++] = addres>>8;

	// копируем данные из буфеа
	for( unsigned int i = 0 ; i < dataLen ; i++ )
		bufer[len++] =  data[i];

	for( unsigned int i = 0 ; i < len ; i++ )
		if( network->WriteByte(bufer[i]) != 1 )
			return -1;

	// провер€ем полные ли данные
	if( ((addres&0x3F) + dataLen) != SCRATCHPAD_LEN )
	{
		if( !network->Reset() )
			return -1;
		return 1;
	}

	// расчитываем контрольную сумму
	unsigned short calculatedCrc = 0;

	for( int i = 0 ; i < len ; i++ )
	{
		calculatedCrc = Crc16(calculatedCrc , bufer[i]);
	}
	calculatedCrc ^= 0xFFFF;
	//читаем црц
	if( (network->ReadByte( crc16[0])!= 1) || (network->ReadByte( crc16[1])!= 1))
		return -1;

	if( (crc16[0] != (calculatedCrc&0xff)) || (crc16[1] != (calculatedCrc>>8)) )
		return 0;

	if( !network->Reset() )
		return -1;

	return 1;
}



int DS1977::ReadScratchpad(unsigned char* inBuf , unsigned int buferLen)
{
	// провер€ем входные данные
	if( (inBuf==0) || (buferLen==0) )
		return -1;

	// инициализируем таблетку
	if( !network->Reset() || (MatchRom() != 1) )
		return -1;

	// посылаем команду на чтение
	if( network->WriteByte(READ_SCRATCHPAD) != 1 )
		return -1;

	//читаем адрес и байт состо€ни€
	if( (network->ReadByte(scrAddres[0]) != 1) || (network->ReadByte(scrAddres[1]) != 1) || (network->ReadByte(esStatus) != 1))
		return -1;

	len = 0x3F - (((scrAddres[1]<<8)|scrAddres[0])&0x3F);

	// читаем до конца данных, чтобы проверить данные с контрольной суммой
	for( int i = 0 ; i < len ; ++i )
	{
		if( network->ReadByte( bufer[i]) != 1)
			return -1;
	}

	//читаем црц
	if( (network->ReadByte( crc16[0])!= 1) || (network->ReadByte( crc16[1])!= 1))
		return -1;

	// считаем црц полученных данных и сравниваем с полученным
	unsigned short calculatedCrc = 0;
	calculatedCrc = Crc16(calculatedCrc , scrAddres[0]);
	calculatedCrc = Crc16(calculatedCrc , scrAddres[1]);
	calculatedCrc = Crc16(calculatedCrc , esStatus);
	for( int i =0 ; i< len ; ++i)
		calculatedCrc = Crc16(calculatedCrc , bufer[i]);
	// црц приходит в инвертированном виде, поэтому ддл€ сравнени€ инвертируем полученное црц
	calculatedCrc ^= 0xFFFF;

	// сравниваем црц
	if( (crc16[0] != (calculatedCrc&0xff)) || (crc16[1] != (calculatedCrc>>8)) )
		return 0;

	//расчитываем реальное количество байт
	len = (esStatus&0x3F) - (((scrAddres[1]<<8)|scrAddres[0])&0x3F);
	//копируем прочитанные данные
	for( int i = 0 ; i< (buferLen < len ? buferLen : len) ; ++i )
		inBuf[i] = bufer[i];
	return (buferLen < len ? buferLen : len);
}

int DS1977::CopyScratchpad(unsigned char* , unsigned int)
{
	// инициализируем таблетку
	if( !network->Reset() || (MatchRom() != 1) )
		return -1;

	// заполн€ем пароль
	bzero( password , sizeof(password) );

	len = 0;

	bufer[ len++ ] = COPY_SCRATCHPAD;
	bufer[ len++ ] = scrAddres[0];
	bufer[ len++ ] = scrAddres[1];

	// копируем пароль
	for( int i = 0 ; i < sizeof(password) ; i++)
		bufer[ len++ ] = password[i];

	for( int i = 0 ; i< len-1 ;  ++i)
		if(network->WriteByte(bufer[i]) != 1 )
			return -1;


	return 1;
}



