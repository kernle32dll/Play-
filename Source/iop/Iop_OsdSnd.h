#pragma once

#include <string>
#include <map>
#include <regex>
#include "filesystem_def.h"
#include "StdStream.h"
#include "Iop_Module.h"
#include "Iop_SifManPs2.h"
#include "Iop_Sysmem.h"
#include "Iop_SpuBase.h"
#include "Iop_Spu2.h"

#define SPU_VOICE_VOL_LEFT (1 << 0)
#define SPU_VOICE_VOL_RIGHT (1 << 1)
#define SPU_VOICE_PITCH (1 << 4)

namespace Iop
{
	class CSysmem;

	class COsdSnd : public CModule, CSifModule
	{
	public:
		COsdSnd(uint8*, CSifMan&, CSysmem&, CSpu2&);
		virtual ~COsdSnd() = default;

		std::string GetId() const override;
		std::string GetFunctionName(unsigned int) const override;
		void Invoke(CMIPS&, unsigned int) override;

		void LoadState(Framework::CZipArchiveReader&) override;
		void SaveState(Framework::CZipArchiveWriter&) const override;

		bool Invoke(uint32, uint32*, uint32, uint32*, uint32, uint8*) override;

		enum MODULE_ID
		{
			MODULE_ID = 0x80000601,
		};

	private:
		uint8* m_ram;
		CSifMan& m_sifMan;
		CSysmem& m_sysMem;
		CSpu2& m_spu2;

		uint8 m_core;

		struct Volume
		{
			int16 left;
			int16 right;
		};

		struct VoiceAttributes
		{
			uint32 voice;
			uint32 mask;
			Volume vol;
			Volume vol_mode;
			Volume vol_current;
			uint16 pitch;
			uint16 note;
			uint16 sample_note;
			int16 envelopevalue_current;
			uint32* sample_addr;
			uint32* loop_addr;
			int32 attack_mode;
			int32 sustain_mode;
			int32 release_mode;
			uint16 attack_rate;
			uint16 decay_rate;
			uint16 sustain_rate;
			uint16 release_rate;
			uint16 sustain_level;
			uint16 adsr1;
			uint16 adsr2;
		};
		static_assert(sizeof(VoiceAttributes) == 80, "VoiceAttributes must be 80 bytes long.");
	};

	typedef std::shared_ptr<COsdSnd> COsdSndPtr;
}