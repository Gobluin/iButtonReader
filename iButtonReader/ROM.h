#ifndef ROM_H_
#define ROM_H_

/* ========= INCLUDES ========= */

/* ========= DEFINES ========= */
#define ROM_LENGTH 64

class ROM
{
	public:
		ROM();
		virtual ~ROM();

		unsigned char	Type(){ return rom.parsed.type; };
		unsigned char 	CRC(){ return rom.parsed.crc; };
		unsigned char*	Serial(){ return rom.parsed.serial;};

		// bit operations
		unsigned char operator[] ( unsigned int);
		ROM& operator& (unsigned long long);
		ROM& operator| (unsigned long long);


		// crc check function
		bool isValidCRC();
	protected :
		union
		{
			struct
			{
				unsigned char type;
				unsigned char serial[6];
				unsigned char crc;
			} parsed;
			unsigned long long full;
		} rom;
};

#endif /* ROM_H_ */
