#include "Iop_Dev9.h"
#include "Log.h"

#define LOG_NAME ("iop_dev9")

using namespace Iop;

uint32 CDev9::ReadRegister(uint32 address)
{
	uint32 result = 0;
	switch(address)
	{
	case REG_POWER:
		result = m_Power;
		break;
	case REG_1460:
		result = m_reg1460;
		break;
	case REG_1462:
		result = m_reg1462;
		break;
	case REG_1464:
		result = m_reg1464;
		break;
	case REG_1466:
		result = m_reg1466;
		break;
	case REG_REV:
		//Report expansion bay
		result = 0x30;
		break;
	}
	LogRead(address, result);
	return result;
}

void CDev9::WriteRegister(uint32 address, uint32 value)
{
	switch(address)
	{
	case REG_1460:
		 m_reg1460 = value;
		break;
	case REG_1462:
		 m_reg1462 = value;
		break;
	case REG_1464:
		 m_reg1464 = value;
		break;
	case REG_1466:
		 m_reg1466 = value;
		break;
	case REG_POWER:
		m_Power = value;
	}
	LogWrite(address, value);
}

void CDev9::LogRead(uint32 address, uint32 result)
{
	switch(address)
	{
	case REG_1460:
		CLog::GetInstance().Print(LOG_NAME, "= REG_1460 (0x%08X)\r\n", result);
		break;
	case REG_1462:
		CLog::GetInstance().Print(LOG_NAME, "= REG_1462 (0x%08X)\r\n", result);
		break;
	case REG_1464:
		CLog::GetInstance().Print(LOG_NAME, "= REG_1464 (0x%08X)\r\n", result);
		break;
	case REG_1466:
		CLog::GetInstance().Print(LOG_NAME, "= REG_1466 (0x%08X)\r\n", result);
		break;
	case REG_POWER:
		CLog::GetInstance().Print(LOG_NAME, "= REG_POWER (0x%08X)\r\n", result);
		break;
	case REG_REV:
		CLog::GetInstance().Print(LOG_NAME, "= REG_REV (0x%08X)\r\n", result);
		break;
	default:
		CLog::GetInstance().Warn(LOG_NAME, "Read an unknown register 0x%08X.\r\n", address);
		break;
	}
}

void CDev9::LogWrite(uint32 address, uint32 value)
{
	switch(address)
	{
	case REG_1460:
		CLog::GetInstance().Print(LOG_NAME, "REG_1460 = 0x%08X\r\n", value);
		break;
	case REG_1462:
		CLog::GetInstance().Print(LOG_NAME, "REG_1462 = 0x%08X\r\n", value);
		break;
	case REG_1464:
		CLog::GetInstance().Print(LOG_NAME, "REG_1464 = 0x%08X\r\n", value);
		break;
	case REG_1466:
		CLog::GetInstance().Print(LOG_NAME, "REG_1466 = 0x%08X\r\n", value);
		break;
	case REG_POWER:
		CLog::GetInstance().Print(LOG_NAME, "REG_POWER = 0x%08X\r\n", value);
		break;
	default:
		CLog::GetInstance().Warn(LOG_NAME, "Wrote 0x%08X to an unknown register 0x%08X.\r\n", value, address);
		break;
	}
}
