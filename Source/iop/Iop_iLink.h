#pragma once

#include "Types.h"
#include "Iop_Intc.h"

namespace Iop
{
	class CiLink
	{
	public:
		CiLink(CIntc&);

		void Reset();

		uint32 ReadRegister(uint32);
		void WriteRegister(uint32, uint32);

		void LogRead(uint32, uint32);
		void LogWrite(uint32, uint32);

	private:
		enum
		{
			REG_CTRL2 = 0x1F808410,
			REG_PHYACCESS = 0x1F808414,
			REG_INTR0 = 0x1F808420,
			REG_INTR0_MASK = 0x1F808424,
			REG_INTR1 = 0x1F808428,
			REG_INTR1_MASK = 0x1F80842C,
			REG_INTR2 = 0x1F808430,
			REG_INTR2_MASK = 0x1F808434,
			REG_DMA_CTRLSR0 = 0x1F8084B8,
			REG_DMA_UNK_7C = 0x1F80847C,
			REG_DMA_CTRLSR1 = 0x1F808538,
		};

		enum
		{
			PHY_REGS_SIZE = 0x0F,
			PHY_WRITE = 0x40000000,
			PHY_READ = 0x80000000,
		};

		CIntc& m_intc;

		uint32 m_ctrl2 = 0;

		uint8 m_phyreg[PHY_REGS_SIZE];
		uint8 m_phyreg_readResult;

		uint32 m_intr0 = 0;
		uint32 m_intr0_mask = 0;
		uint32 m_intr1 = 0;
		uint32 m_intr1_mask = 0;
		uint32 m_intr2 = 0;
		uint32 m_intr2_mask = 0;

		uint32 m_dmaCtrlSR0 = 0;
		uint32 m_unknown_7C = 0;
		uint32 m_dmaCtrlSR1 = 0;
	};
}