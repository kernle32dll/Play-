#include <cstring>
#include "Iop_SifManPs2.h"
#include "../Ps2Const.h"
#include "../Log.h"

#define LOG_NAME ("iop_sifmanps2")

using namespace Iop;

CSifManPs2::CSifManPs2(CIopBios& bios, CSIF& sif, uint8* eeRam, uint8* iopRam)
    : m_bios(bios)
    , m_sif(sif)
    , m_eeRam(eeRam)
    , m_iopRam(iopRam)
{
	for(auto& buf : m_buffer)
	{
		buf.first = -1;
		buf.second = std::vector<SIFDMAREG>();
	}
	for(auto& buf : m_callback)
	{
		buf.first = -1;
		buf.second = -1;
	}
}

void CSifManPs2::CountTicks(uint32 ticks)
{
	m_delay -= std::min(m_delay, ticks);
	if(m_delay == 0)
	{
		for(auto& buf : m_buffer)
		{
			auto position = buf.first;

			if(position != -1)
			{
				executeQueuePosition(position);
			}
		}
	}
}
void CSifManPs2::executeQueuePosition(uint32 position)
{
	std::pair<uint32, std::vector<SIFDMAREG>>& buf = m_buffer[position];

	for(auto& dmaReg : buf.second)
	{
		CLog::GetInstance().Print(LOG_NAME, "DMA reg -> srcAddr = 0x%08X, dstAddr = 0x%08X, size = %d, flags = 0x%08X;\r\n",
		                          dmaReg.srcAddr, dmaReg.dstAddr, dmaReg.size, dmaReg.flags);

		uint8* src = m_iopRam + dmaReg.srcAddr;
		uint8* dst = m_eeRam + (dmaReg.dstAddr & (PS2::EE_RAM_SIZE - 1));
		memcpy(dst, src, dmaReg.size);
	}

	// Reset dma regs
	buf.first = -1;
	buf.second = std::vector<SIFDMAREG>();

	CLog::GetInstance().Print(LOG_NAME, "DMA request done (%d);\r\n",
	                          position + 1);

	// -------

	auto callback = m_callback[position];
	auto callbackPtr = callback.first;
	auto callbackParam = callback.second;

	if(callbackPtr != -1)
	{
		CLog::GetInstance().Print(LOG_NAME, "Firing callback(callbackPtr = 0x%08X, callbackParam = 0x%08X);\r\n",
		                          callbackPtr, callbackParam);
		m_bios.TriggerCallback(callbackPtr, callbackParam);

		m_callback[position].first = -1;
		m_callback[position].second = -1;
	}
}

void CSifManPs2::RegisterModule(uint32 id, CSifModule* module)
{
	m_sif.RegisterModule(id, module);
}

bool CSifManPs2::IsModuleRegistered(uint32 id)
{
	return m_sif.IsModuleRegistered(id);
}

void CSifManPs2::UnregisterModule(uint32 id)
{
	m_sif.UnregisterModule(id);
}

void CSifManPs2::SendPacket(void* packet, uint32 size)
{
	m_sif.SendPacket(packet, size);
}

void CSifManPs2::SetDmaBuffer(uint32 bufferAddress, uint32 size)
{
	m_sif.SetDmaBuffer(bufferAddress, size);
}

void CSifManPs2::SetCmdBuffer(uint32 bufferAddress, uint32 size)
{
	m_sif.SetCmdBuffer(bufferAddress, size);
}

void CSifManPs2::SendCallReply(uint32 serverId, const void* returnData)
{
	m_sif.SendCallReply(serverId, returnData);
}

void CSifManPs2::GetOtherData(uint32 dst, uint32 src, uint32 size)
{
	uint8* srcPtr = m_eeRam + (src & (PS2::EE_RAM_SIZE - 1));
	uint8* dstPtr = m_iopRam + dst;
	memcpy(dstPtr, srcPtr, size);
}

void CSifManPs2::SetModuleResetHandler(const ModuleResetHandler& moduleResetHandler)
{
	m_sif.SetModuleResetHandler(moduleResetHandler);
}

void CSifManPs2::SetCustomCommandHandler(const CustomCommandHandler& customCommandHandler)
{
	m_sif.SetCustomCommandHandler(customCommandHandler);
}

uint32 CSifManPs2::SifDmaStat(uint32 transferId)
{
	CLog::GetInstance().Print(LOG_NAME, "SifDmaStat(transferId = %X);\r\n",
	                          transferId);

	if (transferId <= 0) {
        return -1;
	}

	auto test = m_buffer[transferId - 1];

	// Still working on the request
	if(test.first != -1)
	{
		CLog::GetInstance().Print(LOG_NAME, "Still working... (transferId = %X);\r\n",
		                          transferId);
		return 0;
	}

	return -1;
}

uint32 CSifManPs2::SifSetDma(uint32 structAddr, uint32 count)
{
	CLog::GetInstance().Print(LOG_NAME, "SifSetDma(structAddr = 0x%08X, count = %d);\r\n",
	                          structAddr, count);

	if(structAddr == 0)
	{
		return 0;
	}

	uint32 possiblePos = -1;
	for(int i = 0; i < 32; i++)
	{
		if(m_buffer[i].first == -1)
		{
			possiblePos = i;
			break;
		}
	}

	// No space in queue anymore
	if(possiblePos == -1)
	{
		return 0;
	}

	std::vector<SIFDMAREG> regs;

	auto dmaRegs = reinterpret_cast<const SIFDMAREG*>(m_iopRam + structAddr);
	for(unsigned int i = 0; i < count; i++)
	{
		regs.push_back(dmaRegs[i]);
	}

	m_buffer[possiblePos].first = possiblePos;
	m_buffer[possiblePos].second = regs;
	m_callback[possiblePos].first = -1;
	m_callback[possiblePos].second = -1;

	executeQueuePosition(possiblePos);

	return possiblePos + 1;
}

uint32 CSifManPs2::SifSetDmaCallback(CMIPS& context, uint32 structAddr, uint32 count, uint32 callbackPtr, uint32 callbackParam)
{
	CLog::GetInstance().Print(LOG_NAME, "SifSetDmaCallback(structAddr = 0x%08X, count = %d, callbackPtr = 0x%08X, callbackParam = 0x%08X);\r\n",
	                          structAddr, count, callbackPtr, callbackParam);

	if(structAddr == 0)
	{
		return 0;
	}

	uint32 possiblePos = -1;
	for(int i = 0; i < 32; i++)
	{
		if(m_buffer[i].first == -1)
		{
			possiblePos = i;
			break;
		}
	}

	// No space in queue anymore
	if(possiblePos == -1)
	{
		return 0;
	}

	std::vector<SIFDMAREG> regs;

	auto dmaRegs = reinterpret_cast<const SIFDMAREG*>(m_iopRam + structAddr);
	for(unsigned int i = 0; i < count; i++)
	{
		regs.push_back(dmaRegs[i]);
	}

	m_buffer[possiblePos].first = possiblePos;
	m_buffer[possiblePos].second = regs;
	m_callback[possiblePos].first = callbackPtr;
	m_callback[possiblePos].second = callbackParam;

	m_delay = 10000000;

	return possiblePos + 1;
}

uint8* CSifManPs2::GetEeRam() const
{
	return m_eeRam;
}
