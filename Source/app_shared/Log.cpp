#include "Log.h"
#include <chrono>
#include <set>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <string>
#include "AppConfig.h"
#include "PathUtils.h"
#include "StdStreamUtils.h"

#define LOG_PATH "logs"

#define PREF_LOG_SHOWPRINTS "log.showprints"

#if LOGGING_ENABLED

// clang-format off
static const std::set<std::string, std::less<>> g_allowedLogs =
{
	//"iop_mcserv",
};
// clang-format on

namespace
{
	std::string GetTimestampPrefix()
	{
		const auto now = std::chrono::system_clock::now();
		const auto nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
		const auto subSecondMicros = std::chrono::duration_cast<std::chrono::microseconds>(now - nowSeconds).count();
		const auto nowTimeT = std::chrono::system_clock::to_time_t(now);
		std::tm timeInfo{};
#ifdef _WIN32
		localtime_s(&timeInfo, &nowTimeT);
#else
		localtime_r(&nowTimeT, &timeInfo);
#endif
		char buffer[48];
		if(std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S", &timeInfo) == 0)
		{
			return {};
		}
		std::snprintf(buffer + std::char_traits<char>::length(buffer), sizeof(buffer) - std::char_traits<char>::length(buffer), ".%06lld] ", static_cast<long long>(subSecondMicros));
		return buffer;
	}

	void WriteLogLine(Framework::CStdStream& logStream, const char* format, va_list args)
	{
		const auto timestampPrefix(GetTimestampPrefix());
		if(!timestampPrefix.empty())
		{
			std::fputs(timestampPrefix.c_str(), logStream);
		}
		std::vfprintf(logStream, format, args);
		logStream.Flush();
	}
}

CLog::CLog()
{
	m_logBasePath = CAppConfig::GetInstance().GetBasePath() / LOG_PATH;
	Framework::PathUtils::EnsurePathExists(m_logBasePath);
	CAppConfig::GetInstance().RegisterPreferenceBoolean(PREF_LOG_SHOWPRINTS, false);
	m_showPrints = CAppConfig::GetInstance().GetPreferenceBoolean(PREF_LOG_SHOWPRINTS);
}

void CLog::Print(const char* logName, const char* format, ...)
{
	if(!m_showPrints && !g_allowedLogs.count(logName)) return;
	auto& logStream(GetLog(logName));
	va_list args;
	va_start(args, format);
	WriteLogLine(logStream, format, args);
	va_end(args);
}

void CLog::Warn(const char* logName, const char* format, ...)
{
	auto& logStream(GetLog(logName));
	va_list args;
	va_start(args, format);
	WriteLogLine(logStream, format, args);
	va_end(args);
}

Framework::CStdStream& CLog::GetLog(const char* logName)
{
	auto logIterator(m_logs.find(logName));
	if(logIterator == std::end(m_logs))
	{
		auto logPath = m_logBasePath / (std::string(logName) + ".log");
		auto logStream = Framework::CreateOutputStdStream(logPath.native());
		m_logs[logName] = std::move(logStream);
		logIterator = m_logs.find(logName);
	}
	return logIterator->second;
}

#endif
