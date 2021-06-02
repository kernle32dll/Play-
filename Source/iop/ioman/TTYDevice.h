#pragma once

#include <vector>
#include "../Ioman_Device.h"

namespace Iop::Ioman
{
	class CTTYDevice : public CDevice
	{
	public:
		CTTYDevice();

		Framework::CStream* GetFile(uint32, const char*) override;
		DirectoryIteratorPtr GetDirectory(const char*) override;
	};
}
