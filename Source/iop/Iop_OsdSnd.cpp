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
		// spuInit
		CLog::GetInstance().Warn(LOG_NAME, "spuInit (argSize = %d, retSize = %d).\r\n", argsSize, retSize);
		ret[0] = 0;
		break;
	case 0x00000002:
		// spuSetCore
		CLog::GetInstance().Warn(LOG_NAME, "spuSetCore (core = %d).\r\n", args[1]);
		m_core = args[1];

		ret[0] = 0;
		break;
	case 0x00000005:
	{
		// spuSetKey
		CLog::GetInstance().Warn(LOG_NAME, "spuSetKey (on = %d, channelFlag = 0x%08X).\r\n", args[1], args[2]);

		CSpuBase core = m_spu2.GetCore(m_core)->GetSpuBase();

		if(args[1] == 1)
		{
			core.SendKeyOn(args[2]);
		}
		else
		{
			core.SendKeyOff(args[2]);
		}

		ret[0] = 0;
		break;
	}
	case 0x0000000E:
		// spuSetTransferMode
		CLog::GetInstance().Warn(LOG_NAME, "spuSetTransferMode (argSize = %d, retSize = %d).\r\n", argsSize, retSize);
		ret[0] = 0;
		break;
	case 0x00000010:
	{
		// spuSetTransferStartAddr
		CLog::GetInstance().Warn(LOG_NAME, "spuSetTransferStartAddr (addr = 0x%08X).\r\n", args[1]);

		Spu2::CCore* core = m_spu2.GetCore(m_core);
//		core->GetSpuBase().SetTransferAddress(args[1]);

		ret[0] = 0;
		break;
	}
	case 0x00000011:
		// spuWrite
		CLog::GetInstance().Warn(LOG_NAME, "spuWrite (addr = 0x%08X, size = %d).\r\n", args[1], args[2]);
		ret[0] = 0;
		break;
	case 0x00000012:
		// spuWrite0
		CLog::GetInstance().Warn(LOG_NAME, "spuWrite0 (argSize = %d, retSize = %d).\r\n", argsSize, retSize);
		ret[0] = 0;
		break;
	case 0x00000100:
		// spuInitMalloc
		CLog::GetInstance().Warn(LOG_NAME, "spuInitMalloc (argSize = %d, retSize = %d).\r\n", argsSize, retSize);
		ret[0] = 0;
		break;
	case 0x00000101:
		// spuMalloc
		CLog::GetInstance().Warn(LOG_NAME, "spuMalloc (argSize = %d, retSize = %d).\r\n", argsSize, retSize);
		ret[0] = 0;
		break;
	case 0x00007128:
		// spuSetCommonAttr
		CLog::GetInstance().Warn(LOG_NAME, "spuSetCommonAttr (argSize = %d, retSize = %d).\r\n", argsSize, retSize);
		ret[0] = 0;
		break;
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
		CSpuBase::CHANNEL& channel = core->GetSpuBase().GetChannel(moduleData->voice);

		if(moduleData->mask & SPU_VOICE_PITCH)
		{
			channel.pitch = moduleData->pitch;
		}
		if(moduleData->mask & SPU_VOICE_VOL_LEFT)
		{
			channel.volumeLeft.volume.volume = moduleData->vol.left;
		}
		if(moduleData->mask & SPU_VOICE_VOL_RIGHT)
		{
			channel.volumeRight.volume.volume = moduleData->vol.right;
		}

		//        channel.address = *moduleData->sample_addr;

		ret[0] = 0;
		break;
	}
	default:
		CLog::GetInstance().Warn(LOG_NAME, "Unknown RPC method invoked (0x%08X).\r\n", method);
		break;
	}
	return true;
}

void COsdSnd::LoadState(Framework::CZipArchiveReader&)
{
}

void COsdSnd::SaveState(Framework::CZipArchiveWriter&) const
{
}
