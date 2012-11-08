#include "rom.h"
#include "crc.h"
#include <iostream>

ROM::ROM()
{
	rom.full = 0;

}

ROM::~ROM()
{
	rom.full = 0;
}


unsigned char ROM::operator []( unsigned int bit)
{
	return ((rom.full>>bit)&0x1);
}

ROM& ROM::operator &( unsigned long long _data)
{
	rom.full &= _data;
	return *this;
}

ROM& ROM::operator |( unsigned long long _data)
{
	rom.full |= _data;
	return *this;
}


bool ROM::isValidCRC()
{
	unsigned char crc = 0;
	crc = Crc8(crc, rom.parsed.type);
	for( int i = 0; i< 6; i++)
		crc = Crc8( crc, rom.parsed.serial[i] );

	return crc == rom.parsed.crc;
}
