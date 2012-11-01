/*
 * Класс реализующий алгоритм поиска упровляемых устройств на канале 1-wire
 */

#ifndef IWIRESEARCH_H_
#define IWIRESEARCH_H_

/* ========= INCLUDES ========= */
#include "IWire.h"
#include <vector>
#include <string>
#include <stdexception>

/* ========= DEFINES ========= */
#define ROM_LEN	64
#define SEARCH_OK 1
#define SEARCH_NODATA 0
#define SEARCH_ERROR -1



using namespace std;
class IWireSearch
{
	public:
		IWireSearch();
		virtual ~IWireSearch();

		void SearchDevices( IWireMasterDevice& );

	private:
		int SearchStep(IWireMasterDevice&,unsigned int,  bool , ROM);

	private:
		// переменные для поиска данных
		vector<ROM>		foundDevices;
		unsigned int	cycle;
		unsigned char	firstBit;
		unsigned char	secondBit;
		int 			tmp;
};

#endif /* IWIRESEARCH_H_ */
