#include "Iop_OsdSnd.h"
#include "Iop_SifManPs2.h"
#include "../Log.h"

using namespace Iop;

#define LOG_NAME "iop_osdsnd"

COsdSnd::COsdSnd(uint8* ram, CSifMan& sifMan, CSysmem& sysMem, CSpu2& spu2)
    : m_ram(ram)
    , m_sifMan(sifMan)
    , m_sysMem(sysMem)
    , m_spu2(spu2)
{
	sifMan.RegisterModule(MODULE_ID, this);
	m_core = 0;
}

std::string COsdSnd::GetId() const
{
	return "osdsnd";
}

std::string COsdSnd::GetFunctionName(unsigned int functionId) const
{
	switch(functionId)
	{
	default:
		return "unknown";
		break;
	}
}

void COsdSnd::Invoke(CMIPS& context, unsigned int functionId)
{
	switch(functionId)
	{
	default:
		CLog::GetInstance().Warn(LOG_NAME, "Unknown function (%d) called. PC = (%08X).\r\n",
		                         functionId, context.m_State.nPC);
		break;
	}
}

bool COsdSnd::Invoke(uint32 method, uint32* args, uint32 argsSize, uint32* ret, uint32 retSize, uint8* ram)
{
	switch(method)
	{
	case 0x00000001:
		SpuInit(args, argsSize, ret, retSize, ram);
		break;
	case 0x00000002:
		// spuSetCore
		CLog::GetInstance().Warn(LOG_NAME, "spuSetCore(core = %d).\r\n", args[1]);
		m_core = args[1];

		ret[0] = 0;
		break;
	case 0x00000005:
	{
		// spuSetKey
		CLog::GetInstance().Warn(LOG_NAME, "spuSetKey(on = %d, channelFlag = 0x%08X).\r\n", args[1], args[2]);

		Spu2::CCore* core = m_spu2.GetCore(m_core);

		if(args[1] == 1)
		{
			core->WriteRegister(Spu2::CCore::A_KON_HI, args[2] & 0x0000FFFF);
			core->WriteRegister(Spu2::CCore::A_KON_LO, (args[2] & 0x0000FFFF) << 16);
		}
		else
		{
			core->WriteRegister(Spu2::CCore::A_KOFF_HI, args[2] & 0x0000FFFF);
			core->WriteRegister(Spu2::CCore::A_KOFF_LO, (args[2] & 0x0000FFFF) << 16);
		}

		ret[0] = 0;
		break;
	}
	case 0x0000000E:
	{
		// spuSetTransferMode
		CLog::GetInstance().Warn(LOG_NAME, "spuSetTransferMode(mode = %d).\r\n", args[1]);

		Spu2::CCore* core = m_spu2.GetCore(m_core);
		core->WriteRegister(Spu2::CCore::A_TS_MODE, 0); // TODO - needs mapping?

		ret[0] = 0;
		break;
	}
	case 0x00000010:
	{
		// spuSetTransferStartAddr
		CLog::GetInstance().Warn(LOG_NAME, "spuSetTransferStartAddr(addr = 0x%08X).\r\n", args[1]);

		Spu2::CCore* core = m_spu2.GetCore(m_core);
		core->GetSpuBase().SetTransferAddress(args[1]);

		// subtract ram size!

		CLog::GetInstance().Warn(LOG_NAME, "HI (addr = 0x%08X).\r\n", GetAddressHi(core->GetSpuBase().GetTransferAddress()));
		CLog::GetInstance().Warn(LOG_NAME, "LO (addr = 0x%08X).\r\n", GetAddressLo(core->GetSpuBase().GetTransferAddress()));

		//		core->WriteRegister(Spu2::CCore::A_TSA_HI, 0);
		//		core->WriteRegister(Spu2::CCore::A_TSA_LO, 0);

		ret[0] = 0;
		break;
	}
	case 0x00000011:
		// spuWrite
		CLog::GetInstance().Warn(LOG_NAME, "spuWrite(addr = 0x%08X, size = 0x%08X).\r\n", args[1], args[2]);
		ret[0] = 0;
		break;
	case 0x00000012:
	{
		// spuWrite0
		CLog::GetInstance().Warn(LOG_NAME, "spuWrite0(size = 0x%08X).\r\n", args[1]);

		Spu2::CCore* core = m_spu2.GetCore(m_core);
		//        core->GetSpuBase().WriteWord(0);

		ret[0] = 0;
		break;
	}
	case 0x00000100:
		// spuInitMalloc
		CLog::GetInstance().Warn(LOG_NAME, "spuInitMalloc(addr = 0x%08X).\r\n", argsSize, retSize);
		ret[0] = 0;
		break;
	case 0x00000101:
	{
		// spuMalloc
		uint32 size = args[1];
		if(size > 0x1faff0)
		{
			// It's only possible to allocate a maximum of 0x1faff0 at a time
			size = 0x1faff0;
		}

		CLog::GetInstance().Warn(LOG_NAME, "spuMalloc(size = 0x%08X).\r\n", size);
		ret[0] = 0;
		break;
	}
	case 0x00007128:
	{
		// spuSetCommonAttr
		CLog::GetInstance().Warn(LOG_NAME, "spuSetCommonAttr(argSize = %d, retSize = %d).\r\n", argsSize, retSize);
		auto moduleData = reinterpret_cast<CommonAttributes*>(args);

		CLog::GetInstance().Warn(LOG_NAME, "  mask = 0x%08X.\r\n", moduleData->mask);
		CLog::GetInstance().Warn(LOG_NAME, "  vol left = 0x%08X.\r\n", moduleData->vol.left);
		CLog::GetInstance().Warn(LOG_NAME, "  vol right = 0x%08X.\r\n", moduleData->vol.right);
		CLog::GetInstance().Warn(LOG_NAME, "  vol current left = 0x%08X.\r\n", moduleData->vol_current.left);
		CLog::GetInstance().Warn(LOG_NAME, "  vol current right = 0x%08X.\r\n", moduleData->vol_current.right);
		CLog::GetInstance().Warn(LOG_NAME, "  vol mode left = 0x%08X.\r\n", moduleData->vol_mode.left);
		CLog::GetInstance().Warn(LOG_NAME, "  vol mode right = 0x%08X.\r\n", moduleData->vol_mode.right);

		Spu2::CCore* core = m_spu2.GetCore(m_core);

		if(moduleData->mask & SPU_COMMON_MASTER_VOL_LEFT)
		{
			core->WriteRegister(Spu2::CCore::P_MVOLL, moduleData->vol.left);
		}

		if(moduleData->mask & SPU_COMMON_MASTER_VOL_RIGHT)
		{
			core->WriteRegister(Spu2::CCore::P_MVOLR, moduleData->vol.right);
		}

		ret[0] = 0;
		break;
	}
	case 0x00007240:
	{
		// spuSetVoiceAttr
		CLog::GetInstance().Warn(LOG_NAME, "spuSetVoiceAttr:\r\n");

		auto moduleData = reinterpret_cast<VoiceAttributes*>(args);

		CLog::GetInstance().Warn(LOG_NAME, "  mask = 0x%08X.\r\n", moduleData->mask);
		CLog::GetInstance().Warn(LOG_NAME, "  voice = 0x%08X.\r\n", moduleData->voice);
		CLog::GetInstance().Warn(LOG_NAME, "  pitch = %d.\r\n", moduleData->pitch);
		CLog::GetInstance().Warn(LOG_NAME, "  vol left = 0x%08X.\r\n", moduleData->vol.left);
		CLog::GetInstance().Warn(LOG_NAME, "  vol right = 0x%08X.\r\n", moduleData->vol.right);
		CLog::GetInstance().Warn(LOG_NAME, "  sample addr = 0x%08X.\r\n", moduleData->sample_addr);
		CLog::GetInstance().Warn(LOG_NAME, "  loop addr = 0x%08X.\r\n", moduleData->loop_addr);

		Spu2::CCore* core = m_spu2.GetCore(m_core);

		for(int i = 0; i < CSpuBase::MAX_CHANNEL; i++)
		{
			if(moduleData->voice & 1 >> i)
			{
				if(moduleData->mask & SPU_VOICE_VOL_LEFT)
				{
					core->WriteRegisterChannel(i, Spu2::CCore::VP_VOLL, moduleData->vol.left);
				}
				if(moduleData->mask & SPU_VOICE_VOL_RIGHT)
				{
					core->WriteRegisterChannel(i, Spu2::CCore::VP_VOLR, moduleData->vol.right);
				}
				if(moduleData->mask & SPU_VOICE_PITCH)
				{
					core->WriteRegisterChannel(i, Spu2::CCore::VP_PITCH, moduleData->pitch);
				}
				if(moduleData->mask & SPU_VOICE_SAMPLE_ADDR)
				{
					core->WriteRegisterChannel(i, Spu2::CCore::VA_SSA_HI, moduleData->sample_addr);
					core->WriteRegisterChannel(i, Spu2::CCore::VA_SSA_LO, moduleData->sample_addr);
				}
				if(moduleData->mask & SPU_VOICE_LOOP_ADDR)
				{
					core->WriteRegisterChannel(i, Spu2::CCore::VA_LSAX_HI, moduleData->loop_addr);
					core->WriteRegisterChannel(i, Spu2::CCore::VA_LSAX_LO, moduleData->loop_addr);
				}
			}
		}

		ret[0] = 0;
		break;
	}
	default:
		CLog::GetInstance().Warn(LOG_NAME, "Unknown RPC method invoked (0x%08X).\r\n", method);
		break;
	}
	return true;
}

void COsdSnd::SpuInit(uint32* args, uint32 argsSize, uint32* ret, uint32 retSize, uint8* ram)
{
	CLog::GetInstance().Warn(LOG_NAME, "spuInit().\r\n");

	m_core = 0;

	Spu2::CCore* core = m_spu2.GetCore(0);
	core->WriteRegister(Spu2::CCore::P_MMIX, 0xFFF);
	core->WriteRegister(Spu2::CCore::A_EEA_HI, 0xE);
	core->WriteRegister(Spu2::CCore::S_VMIXL_HI, 0xFFFF);
	core->WriteRegister(Spu2::CCore::S_VMIXL_LO, 0xFF);
	core->WriteRegister(Spu2::CCore::S_VMIXEL_HI, 0xFFFF);
	core->WriteRegister(Spu2::CCore::S_VMIXEL_LO, 0xFF);
	core->WriteRegister(Spu2::CCore::S_VMIXR_HI, 0xFFFF);
	core->WriteRegister(Spu2::CCore::S_VMIXR_LO, 0xFF);
	core->WriteRegister(Spu2::CCore::S_VMIXER_HI, 0xFFFF);
	core->WriteRegister(Spu2::CCore::S_VMIXER_LO, 0xFF);
	core->WriteRegister(Spu2::CCore::CORE_ATTR, 0xC000);

	// TODO: (only on) core0 correct?
	core->WriteRegister(Spu2::CCore::P_MVOLL, 0);
	core->WriteRegister(Spu2::CCore::P_MVOLR, 0);
	core->WriteRegister(Spu2::CCore::P_EVOLL, 0);
	core->WriteRegister(Spu2::CCore::P_EVOLR, 0);
	core->WriteRegister(0x1F900768, 0); // Input volume
	core->WriteRegister(0x1F90076A, 0);
	core->WriteRegister(Spu2::CCore::P_BVOLL, 0);
	core->WriteRegister(Spu2::CCore::P_BVOLR, 0);
	core->WriteRegister(0x1F900788, 0);
	core->WriteRegister(0x1F90078A, 0);
	core->WriteRegister(0x1F90078C, 0);
	core->WriteRegister(0x1F90078E, 0);
	core->WriteRegister(0x1F900790, 0x7FFF);
	core->WriteRegister(0x1F900792, 0x7FFF);
	core->WriteRegister(0x1F900794, 0);

	core = m_spu2.GetCore(1);
	core->WriteRegister(Spu2::CCore::P_MMIX, 0xFFF);
	core->WriteRegister(Spu2::CCore::A_EEA_HI, 0xF);
	core->WriteRegister(Spu2::CCore::S_VMIXL_HI, 0xFFFF);
	core->WriteRegister(Spu2::CCore::S_VMIXL_LO, 0xFF);
	core->WriteRegister(Spu2::CCore::S_VMIXEL_HI, 0xFFFF);
	core->WriteRegister(Spu2::CCore::S_VMIXEL_LO, 0xFF);
	core->WriteRegister(Spu2::CCore::S_VMIXR_HI, 0xFFFF);
	core->WriteRegister(Spu2::CCore::S_VMIXR_LO, 0xFF);
	core->WriteRegister(Spu2::CCore::S_VMIXER_HI, 0xFFFF);
	core->WriteRegister(Spu2::CCore::S_VMIXER_LO, 0xFF);
	core->WriteRegister(Spu2::CCore::CORE_ATTR, 0xC001);

	m_spu2.WriteRegister(0x1F9007C0, 0x200); // C_SPDIF_OUT
	m_spu2.WriteRegister(0x1F9007C6, 0x900); // C_SPDIF_MODE
	m_spu2.WriteRegister(0x1F9007CA, 0x8); // C_SPDIF_MODE

	ret[0] = 0;
}

void COsdSnd::LoadState(Framework::CZipArchiveReader&)
{
}

void COsdSnd::SaveState(Framework::CZipArchiveWriter&) const
{
}

uint16 COsdSnd::GetAddressLo(uint32 address)
{
	return static_cast<uint16>((address >> 1) & 0xFFFF);
}

uint16 COsdSnd::GetAddressHi(uint32 address)
{
	return static_cast<uint16>((address >> (16 + 1)) & 0xFFFF);
}
