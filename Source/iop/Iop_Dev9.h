#pragma once

#include "Types.h"

namespace Iop
{
	class CDev9
	{
	public:
		enum
		{
			ADDR_BEGIN = 0x1F801460,
			ADDR_END = 0x1F80147F
		};

		uint32 ReadRegister(uint32);
		void WriteRegister(uint32, uint32);

	private:
		enum
		{
			// TODO: Find out these - they are used by OPL
			// 0x1F801460
			// 0x1F801462
			// 0x1F801464
			REG_POWER = 0x1F80146C,
			REG_REV = 0x1F80146E,
		};

		void LogRead(uint32, uint32);
		void LogWrite(uint32, uint32);

		uint32 m_Power;
	};
}
