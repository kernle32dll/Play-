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
			// These have no documented name
			REG_1460 = 0x1F801460,
			REG_1462 = 0x1F801462,
			REG_1464 = 0x1F801464,
			REG_1466 = 0x1F801466,

			REG_POWER = 0x1F80146C,
			REG_REV = 0x1F80146E,
		};

		void LogRead(uint32, uint32);
		void LogWrite(uint32, uint32);

		uint32 m_reg1460;
		uint32 m_reg1462;
		uint32 m_reg1464;
		uint32 m_reg1466;
		uint32 m_Power;
	};
}
