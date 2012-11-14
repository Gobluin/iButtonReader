/*
 *
 */
#include "IWireDevices.h"
#include "crc.h"

// ��������� ������ � ������, ����������� ��� DS1977
#define MEMORY_ADDRES 0x0000
#define READ_PASSWORD_ADDRES 0x7FC0
#define RW_PASSWORD_ADDRES 0x7FC8
#define PASSWORD_CONTROL_ADDRES 0x7FD0

#define SCRATCHPAD_LEN 64
#define MEMORY_LEN 32704
#define PASSWORD_LEN 8

//���������� ������� DS1977
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
	// �������������� ��������
	if( !network->Reset() || (MatchRom() != 1) )
		return -1;

	// ��������� ������� �� ����������
	bufer[len++] = WRITE_SCRATCHPAD;
	bufer[len++] = addres&0xff;
	bufer[len++] = addres>>8;

	// �������� ������ �� �����
	for( unsigned int i = 0 ; i < dataLen ; i++ )
		bufer[len++] =  data[i];

	for( unsigned int i = 0 ; i < len ; i++ )
		if( network->WriteByte(bufer[i]) != 1 )
			return -1;

	// ��������� ������ �� ������
	if( ((addres&0x3F) + dataLen) != SCRATCHPAD_LEN )
	{
		if( !network->Reset() )
			return -1;
		return 1;
	}

	// ����������� ����������� �����
	unsigned short calculatedCrc = 0;

	for( int i = 0 ; i < len ; i++ )
	{
		calculatedCrc = Crc16(calculatedCrc , bufer[i]);
	}
	calculatedCrc ^= 0xFFFF;
	//������ ���
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
	// ��������� ������� ������
	if( (inBuf==0) || (buferLen==0) )
		return -1;

	// �������������� ��������
	if( !network->Reset() || (MatchRom() != 1) )
		return -1;

	// �������� ������� �� ������
	if( network->WriteByte(READ_SCRATCHPAD) != 1 )
		return -1;

	//������ ����� � ���� ���������
	if( (network->ReadByte(scrAddres[0]) != 1) || (network->ReadByte(scrAddres[1]) != 1) || (network->ReadByte(esStatus) != 1))
		return -1;

	len = 0x3F - (((scrAddres[1]<<8)|scrAddres[0])&0x3F);

	// ������ �� ����� ������, ����� ��������� ������ � ����������� ������
	for( int i = 0 ; i < len ; ++i )
	{
		if( network->ReadByte( bufer[i]) != 1)
			return -1;
	}

	//������ ���
	if( (network->ReadByte( crc16[0])!= 1) || (network->ReadByte( crc16[1])!= 1))
		return -1;

	// ������� ��� ���������� ������ � ���������� � ����������
	unsigned short calculatedCrc = 0;
	calculatedCrc = Crc16(calculatedCrc , scrAddres[0]);
	calculatedCrc = Crc16(calculatedCrc , scrAddres[1]);
	calculatedCrc = Crc16(calculatedCrc , esStatus);
	for( int i =0 ; i< len ; ++i)
		calculatedCrc = Crc16(calculatedCrc , bufer[i]);
	// ��� �������� � ��������������� ����, ������� ���� ��������� ����������� ���������� ���
	calculatedCrc ^= 0xFFFF;

	// ���������� ���
	if( (crc16[0] != (calculatedCrc&0xff)) || (crc16[1] != (calculatedCrc>>8)) )
		return 0;

	//����������� �������� ���������� ����
	len = (esStatus&0x3F) - (((scrAddres[1]<<8)|scrAddres[0])&0x3F);
	//�������� ����������� ������
	for( int i = 0 ; i< (buferLen < len ? buferLen : len) ; ++i )
		inBuf[i] = bufer[i];
	return (buferLen < len ? buferLen : len);
}

int DS1977::CopyScratchpad(unsigned char* , unsigned int)
{
	// �������������� ��������
	if( !network->Reset() || (MatchRom() != 1) )
		return -1;

	// ��������� ������
	bzero( password , sizeof(password) );

	len = 0;

	bufer[ len++ ] = COPY_SCRATCHPAD;
	bufer[ len++ ] = scrAddres[0];
	bufer[ len++ ] = scrAddres[1];

	// �������� ������
	for( int i = 0 ; i < sizeof(password) ; i++)
		bufer[ len++ ] = password[i];

	for( int i = 0 ; i< len-1 ;  ++i)
		if(network->WriteByte(bufer[i]) != 1 )
			return -1;


	return 1;
}



