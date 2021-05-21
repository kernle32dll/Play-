#pragma once

#include "Iop_SifMan.h"
#include "../ee/SIF.h"
#include "IopBios.h"

namespace Iop
{
	class CSifManPs2 : public CSifMan
	{
	public:
		CSifManPs2(CIopBios&, CSIF&, uint8*, uint8*);
		virtual ~CSifManPs2() = default;

		void RegisterModule(uint32, CSifModule*) override;
		bool IsModuleRegistered(uint32) override;
		void UnregisterModule(uint32) override;
		void SendPacket(void*, uint32) override;
		void SetDmaBuffer(uint32, uint32) override;
		void SetCmdBuffer(uint32, uint32) override;
		void SendCallReply(uint32, const void*) override;
		void GetOtherData(uint32, uint32, uint32) override;
		void SetModuleResetHandler(const ModuleResetHandler&) override;
		void SetCustomCommandHandler(const CustomCommandHandler&) override;

		void CountTicks(uint32) override;

		uint32 SifDmaStat(uint32 transferId) override;
		uint32 SifSetDmaCallback(CMIPS& context, uint32 structAddr, uint32 count, uint32 callbackPtr, uint32 callbackParam) override;
		uint32 SifSetDma(uint32, uint32) override;

		uint8* GetEeRam() const;

	private:
		CIopBios& m_bios;
		CSIF& m_sif;
		uint8* m_eeRam;
		uint8* m_iopRam;

		std::pair<uint32, std::vector<SIFDMAREG>> m_buffer[32];
		std::pair<uint32, uint32> m_callback[32];
		uint32 m_delay;
		void executeQueuePosition(uint32);
	};
}
