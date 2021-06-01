#pragma once

#include <SifModule.h>
#include "Iop_Module.h"
#include "Iop_SifMan.h"

namespace Iop
{
	class CLibSd : public CModule, public CSifModule
	{
	public:
		enum MODULE_ID
		{
			MODULE_ID = 0x80000701,
		};

		CLibSd(CSifMan&);
		virtual ~CLibSd() = default;

		std::string GetId() const override;
		std::string GetFunctionName(unsigned int) const override;

		void Invoke(CMIPS&, unsigned int) override;
		bool Invoke(uint32, uint32*, uint32, uint32*, uint32, uint8*) override;

		static void TraceCall(CMIPS&, unsigned int);
	};
}
