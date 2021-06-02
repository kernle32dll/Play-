#include "TTYDevice.h"
#include <Singleton.h>
#include "AppConfig.h"
#include "PathUtils.h"
#include "StringUtils.h"
#include "../../Log.h"

using namespace Iop::Ioman;

CTTYDevice::CTTYDevice()
{
}

Framework::CStream* CTTYDevice::GetFile(uint32 mode, const char* devicePath)
{
	auto stdoutPath = CAppConfig::GetBasePath() / "ps2_tty.txt";
	return new Framework::CStdStream(fopen(stdoutPath.string().c_str(), "ab"));
}

DirectoryIteratorPtr CTTYDevice::GetDirectory(const char*)
{
	throw std::runtime_error("Not supported.");
}