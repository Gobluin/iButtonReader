#ifndef IWIRE_H_
#define IWIRE_H_

#define ROM_LENGTH 64





/*
 * Класс, описывающий основные параметры сети IWire
 */
class IWire
{
	public:
		enum Speed		{NormalSpeed , OverdriveSpeed};
		enum PowerMode	{NormalMode , PowerDeliveryMode};
};

typedef long long ROM;

class IWireMasterDevice
{
	public :
		IWireMasterDevice(){};
		virtual ~IWireMasterDevice(){};

		// IWire Basic operations
		virtual bool Reset() = 0;
		virtual bool Search() = 0;

		virtual bool ReadByte( unsigned char& ) = 0;
		virtual bool WriteByte( unsigned char) = 0;

		virtual bool ReadBit( unsigned char&) = 0;
		virtual bool WriteBit( unsigned char) = 0;

		// IWire Advansed operations
		virtual bool setPowerMode( IWire::PowerMode) = 0;
		virtual bool setSpeed(IWire::Speed) = 0;
		virtual bool ProgramPulse() = 0;

		virtual bool ReadBitPower(unsigned char&) = 0;
		virtual bool WriteBitPower(unsigned char) = 0;

};

class IWireSlaveDevice
{

};
#endif /* IWIRE_H_ */
