#include "IWireSearch.h"

IWireSearch::IWireSearch()
{
	// TODO Auto-generated constructor stub

}

IWireSearch::~IWireSearch() {
	// TODO Auto-generated destructor stub
}



void IWireSearch::SearchDevices( IWireMasterDevice& device)
{
	// �������� ��������� ����������
	foundDevices.clear();

	switch( SearchStep(device , 0 , true , 0) )
	{
		case SEARCH_OK:
			cout<<"Found devises: count "<< foundDevices.size()<<endl;
		break;
		case SEARCH_NODATA:
			cout<<"No devises found"<<endl;;
		break;
		case SEARCH_ERROR:
			cout<<" Error during searching"<<endl;
		break;
	}
}


int IWireSearch::SearchStep( IWireMasterDevice& device , unsigned int level,  bool fromBegin, ROM detectedRom)
{

	// ������� �� �������� �� �� �����
	if( level == ROM_LEN)
	{
		foundDevices.push_back( detectedRom );
		return SEARCH_OK;
	}

	// �������, ���� �� �������� � ������
	if( fromBegin )
	{
		// ���������� ��� ���������� � �������� ������� ������
		if( !device.Reset() || !device.Search())
			return SEARCH_ERROR;	// ������������ � �������

		//�������� �� ��� ������������ �����, � �������� �� � ����������, ����� ������� �� ��������������� ������� � �������� ������
		// ������� ������������� ������ - �������
		for( cycle = 0 ; cycle < level ; cycle++ )
		{
			if( device.ReadBit(firstBit) || device.ReadBit(secondBit) || !device.WriteBit( (detectedRom>>cycle)&0x1 ) )
				return SEARCH_ERROR;
		}
	}

	// ������ �������� ���� ����� level
	if( device.ReadBit(firstBit) || device.ReadBit(secondBit) )
		return SEARCH_ERROR;

	switch( secondBit|(firstBit<<1) )
	{
		case 0x3:	// � ������ ������� ��� ���������
			return SEARCH_NODATA;
		case 0x1:	// � ������ ������� � ���� ��������� ����
			return SearchStep( device , level+1, false , detectedRom );
		case 0x2:	// � ������ ������� � ���� ��������� �������
			return SearchStep( device , level+1, false , detectedRom|(1<<level) );
		case 0x0:	// � ������ ������� � ������� � ����
			// ��� �� ����� �����
			if( (tmp = SearchStep( device , level+1, false , detectedRom )) != OK_STEP)
				return tmp;
			// ��� �� ����� �������
			return SearchStep( device , level+1, true , detectedRom|(1<<level) );
		break;
	}
	return -1;
}
