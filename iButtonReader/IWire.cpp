/*
 *
 */
#include "IWire.h"

/*
 *  �������� ������ � ���������� �������� ����������
 *  ���� ������ ROM ������. ������ ���������� � ����� �� ROM ������� �� ���� ����� ������������ ���������� �������
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
 * ���������� ������������� ROM �������.
 * ���������� ����������� ���������� � ��������� ��������������� � ������ � �������� ����������
 * !!!! �������� !!! ���� �� ���� ������������ ��������� ��������� - ��� ��� ������ ������������ ��������� �������,��������� �� ���� 1-wire. !!!!
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
 * ���������� �������������, ���� ����� ���� �����������������
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


