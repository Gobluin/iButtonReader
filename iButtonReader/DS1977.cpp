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

	unsigned int len = 0;

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
	if( (network->ReadByte( bufer[0])!= 1) || (network->ReadByte( bufer[1])!= 1))
		return -1;

	if( (bufer[0] != (calculatedCrc&0xff)) || (bufer[1] != (calculatedCrc>>8)) )
		return 0;

	if( !network->Reset() )
		return -1;

	return 1;
}

int DS1977::ReadScratchpad( unsigned short , unsigned char* , unsigned int)
{

	return 1;
}



