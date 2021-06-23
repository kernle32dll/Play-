#include "Iop_iLink.h"
#include "Log.h"

#define LOG_NAME ("iop_ilink")

using namespace Iop;

CiLink::CiLink(CIntc& intc)
    : m_intc(intc)
{
}

void CiLink::Reset()
{
}

uint32 CiLink::ReadRegister(uint32 address)
{
	uint32 result = 0;
	switch(address)
	{
	case REG_CTRL2:
		result = m_ctrl2;

		// If the device is powered (0x2), add
		// 0x80 to indicate that the device is ready.
		if(result & 0x2)
		{
			result |= 0x8;
		}
		break;
	case REG_PHYACCESS:
		result = m_phyreg_readResult;
		break;
	case REG_INTR0:
		result = m_intr0;
        m_intr0 = 0;
		break;
	case REG_INTR0_MASK:
		result = m_intr0_mask;
		break;
	case REG_INTR1:
		result = m_intr1;
        m_intr1 = 0;
		break;
	case REG_INTR1_MASK:
		result = m_intr1_mask;
		break;
	case REG_INTR2:
		result = m_intr2;
        m_intr2 = 0;
		break;
	case REG_INTR2_MASK:
		result = m_intr2_mask;
		break;
	case REG_DMA_CTRLSR0:
		result = m_dmaCtrlSR0;
		break;
	case REG_DMA_UNK_7C:
		result = m_unknown_7C;
		break;
	case REG_DMA_CTRLSR1:
		result = m_dmaCtrlSR1;
		break;
	}

	LogRead(address, result);
	return result;
}

void CiLink::WriteRegister(uint32 address, uint32 value)
{
	switch(address)
	{
	case REG_CTRL2:
		m_ctrl2 = value;
		break;
	case REG_PHYACCESS:
	{
		uint8 port = (value >> 24) & 0xF;

		if(value & PHY_WRITE)
		{
			uint8 portValue = (value >> 16) & 0xFF;
			m_phyreg[port] = portValue;

			// Bus reset
			if(port == 0x5 && ((portValue & 0x40) > 0))
			{
				m_intr0 |= 0x20000000;
				m_intc.AssertLine(CIntc::LINE_ILINK);
			}
		}
		else if(value & PHY_READ)
		{
			m_phyreg_readResult = m_phyreg[port];
		}

		// TODO: Dunno what this does, but OPL expects INTR0 to have this bit after a PHY access
//		m_intr0 |= 0x40000000;

		break;
	}
	case REG_INTR0:
		m_intr0 = value;
		break;
	case REG_INTR0_MASK:
		m_intr0_mask = value;
		break;
	case REG_INTR1:
		m_intr1 = value;
		break;
	case REG_INTR1_MASK:
		m_intr1_mask = value;
		break;
	case REG_INTR2:
		m_intr2 = value;
		break;
	case REG_INTR2_MASK:
		m_intr2_mask = value;
		break;
	case REG_DMA_CTRLSR0:
		m_dmaCtrlSR0 = value;
		break;
	case REG_DMA_UNK_7C:
		m_unknown_7C = value;
		break;
	case REG_DMA_CTRLSR1:
		m_dmaCtrlSR1 = value;
		break;
	}

	LogWrite(address, value);
}

void CiLink::LogWrite(uint32 address, uint32 value)
{
#define LOG_SET(registerId)                                                      \
	case registerId:                                                             \
		CLog::GetInstance().Print(LOG_NAME, #registerId " = 0x%08X\r\n", value); \
		break;

	if(address == REG_PHYACCESS)
	{
		uint8 port = (value >> 24) & 0xF;

		if(value & PHY_WRITE)
		{
			uint8 portValue = (value >> 16) & 0xF;
			CLog::GetInstance().Print(LOG_NAME, "REG_PHYACCESS = 0x%08X (write 0x%08X to port 0x%08X)\r\n", value, portValue, port);
		}
		else if(value & PHY_READ)
		{
			CLog::GetInstance().Print(LOG_NAME, "REG_PHYACCESS = 0x%08X (read from port 0x%08X)\r\n", value, port);
		}
		return;
	}

	switch(address)
	{
		LOG_SET(REG_CTRL2)
		LOG_SET(REG_INTR0)
		LOG_SET(REG_INTR0_MASK)
		LOG_SET(REG_INTR1)
		LOG_SET(REG_INTR1_MASK)
		LOG_SET(REG_INTR2)
		LOG_SET(REG_INTR2_MASK)
		LOG_SET(REG_DMA_CTRLSR0)
		LOG_SET(REG_DMA_UNK_7C)
		LOG_SET(REG_DMA_CTRLSR1)

	default:
		CLog::GetInstance().Warn(LOG_NAME, "Wrote 0x%08X to an unknown register 0x%08X.\r\n", value, address);
		break;
	}
}

void CiLink::LogRead(uint32 address, uint32 result)
{
#define LOG_GET(registerId)                                                            \
	case registerId:                                                                   \
		CLog::GetInstance().Print(LOG_NAME, "= " #registerId " (0x%08X)\r\n", result); \
		break;

	switch(address)
	{
		LOG_GET(REG_CTRL2)
		LOG_GET(REG_PHYACCESS)
		LOG_GET(REG_INTR0)
		LOG_GET(REG_INTR0_MASK)
		LOG_GET(REG_INTR1)
		LOG_GET(REG_INTR1_MASK)
		LOG_GET(REG_INTR2)
		LOG_GET(REG_INTR2_MASK)
		LOG_GET(REG_DMA_CTRLSR0)
		LOG_GET(REG_DMA_UNK_7C)
		LOG_GET(REG_DMA_CTRLSR1)

	default:
		CLog::GetInstance().Warn(LOG_NAME, "Read an unknown register 0x%08X.\r\n", address);
		break;
	}
}