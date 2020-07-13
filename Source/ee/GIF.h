#pragma once

#include "Types.h"
#include "zip/ZipArchiveWriter.h"
#include "zip/ZipArchiveReader.h"
#include "../gs/GSHandler.h"
#include "../Profiler.h"

class CGIF
{
public:
	// Control register
	// 0x10003000 - 0x1000300F
	struct CTRL : public convertible<uint32>
	{
		unsigned int reset : 1;
		unsigned int unused0 : 2;
		unsigned int tempStop : 1;
		unsigned int unused1 : 28;
	};
	static_assert(sizeof(CTRL) == 4, "Size of CTRL must be 4 bytes.");

	// Mode of operation
	// 0x10003010 - 0x1000301F
	struct MODE : public convertible<uint32>
	{
		unsigned int maskP3 : 1;
		unsigned int unused : 1;
		unsigned int intMode : 1;
		unsigned int unused0 : 29;
	};
	static_assert(sizeof(MODE) == 4, "Size of MODE must be 4 bytes.");

	// Status register
	// 0x10003020 - 0x1000302F
	struct STATUS : public convertible<uint32>
	{
		unsigned int p3MaskedByMode : 1;
		unsigned int p3MaskedByVIF : 1;
		unsigned int intMode : 1;
		unsigned int tempStop : 1;
		unsigned int unused0 : 1;
		unsigned int path3Something : 1;
		unsigned int path3queued : 1;
		unsigned int path2queued : 1;
		unsigned int path1queued : 1;
		unsigned int outputPath : 1;
		unsigned int activePath : 2;
		unsigned int transferDir : 1;
		unsigned int unused1 : 11;
		unsigned int fifoData : 5;
		unsigned int unused2 : 3;
	};
	static_assert(sizeof(STATUS) == 4, "Size of STATUS must be 4 bytes.");

	enum REGISTER
	{
        GIF_CTRL = 0x10003000,
        GIF_MODE = 0x10003010,
        GIF_STAT = 0x10003020
	};

	enum
	{
		REGS_START = 0x10003000,
		REGS_END = 0x100030B0,
	};

	struct TAG
	{
		unsigned int loops : 15;
		unsigned int eop : 1;
		unsigned int reserved0 : 16;
		unsigned int reserved1 : 14;
		unsigned int pre : 1;
		unsigned int prim : 11;
		unsigned int cmd : 2;
		unsigned int nreg : 4;
		uint64 regs;
	};
	static_assert(sizeof(TAG) == 0x10, "Size of TAG must be 16 bytes.");

	CGIF(CGSHandler*&, uint8*, uint8*);
	virtual ~CGIF() = default;

	void Reset();
	uint32 ReceiveDMA(uint32, uint32, uint32, bool);

	uint32 ProcessSinglePacket(const uint8*, uint32, uint32, uint32, const CGsPacketMetadata&);
	uint32 ProcessMultiplePackets(const uint8*, uint32, uint32, uint32, const CGsPacketMetadata&);

	uint32 GetRegister(uint32);
	void SetRegister(uint32, uint32);

	CGSHandler* GetGsHandler();

	void SetPath3Masked(bool);

	void LoadState(Framework::CZipArchiveReader&);
	void SaveState(Framework::CZipArchiveWriter&);

private:
	enum SIGNAL_STATE
	{
		SIGNAL_STATE_NONE,
		SIGNAL_STATE_ENCOUNTERED,
		SIGNAL_STATE_PENDING,
	};

	uint32 ProcessPacked(const uint8*, uint32, uint32);
	uint32 ProcessRegList(const uint8*, uint32, uint32);
	uint32 ProcessImage(const uint8*, uint32, uint32, uint32);

	void DisassembleGet(uint32);
	void DisassembleSet(uint32, uint32);

	uint16 m_loops = 0;
	uint8 m_cmd = 0;
	uint8 m_regs = 0;
	uint8 m_regsTemp = 0;
	uint64 m_regList = 0;
	bool m_eop = false;
	uint32 m_qtemp;
	SIGNAL_STATE m_signalState = SIGNAL_STATE_NONE;
	uint8* m_ram;
	uint8* m_spr;
	CGSHandler*& m_gs;
    STATUS m_Stat;

	CProfiler::ZoneHandle m_gifProfilerZone = 0;
};
