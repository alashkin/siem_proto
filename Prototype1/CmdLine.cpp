#include "stdafx.h"

#include "unicode.h"
#include "CmdLine.h"
#include <boost/program_options/parsers.hpp>
#include <boost/algorithm/string.hpp>
#include <windows.h>
#include <Shlwapi.h>
#include <stdexcept>
#include "utils.h"

namespace po = boost::program_options;

namespace helpers
{
	std::wstring GetCurrentDirectory()
	{
		std::vector<wchar_t> currDir(1024 * 32);

		if (!::GetCurrentDirectory(currDir.size(), &currDir[0]))
		{
			throw std::runtime_error(utils::FormatLastErrorA("GetCurrentDirectory"));
		}

		::PathRemoveBackslash(&currDir[0]);

		return &currDir[0];
	}
}

CCmdLine::CCmdLine() : m_optDescr("\r\nUsage:") 
{
};

CCmdLine::~CCmdLine()
{

}

void CCmdLine::Prepare(int argc, _TCHAR* argv[])
{
	std::wstring strLogLevel = L"Log level: none, ";
	for (unsigned int i = klog::severity_level::debug; i <= klog::severity_level::fatal; ++i)
	{
		strLogLevel += klog::GetStrFromLevel(klog::severity_level(i));

		if (i != klog::severity_level::fatal)
		{
			strLogLevel += L", ";
		}
	}

	strLogLevel += L" (default=debug)";

	m_optDescr.add_options()
		(loglevel, po::value<ansi::string>(), ATL::CW2A(strLogLevel.c_str()))
		(console, "Write log to console (default=off)")
		(reloadadschema, "Reload Active Directory Schema")
		(adschemafile, po::value<ansi::string>(), "Active Directory Schema file")
		(logfile, po::value<ansi::string>(), "Log filename, (default=project1.log) ")
		(bkmpath, po::value<ansi::string>(), "Bookmarks folder, 'fake' to disable bookmarks saving")
		(flushlog, po::value<ansi::string>(), "Flush log, (default=on)")
		(cfgfile, po::value<ansi::string>(), "JSON configuration filename");

	po::store(po::parse_command_line(argc, argv, m_optDescr), m_vm);
	po::notify(m_vm);
}

void CCmdLine::Parse(std::wstring* pstrOutput)
{
	if (m_vm.count(loglevel) == 0)
	{
		*pstrOutput += L"loglevel not specified, using \"debug\"\r\n";
		m_LogSeverityLevel = klog::severity_level::debug;
	}
	else if (m_vm.count(loglevel) != 1)
	{
		throw std::invalid_argument(loglevel);
	}
	else
	{
		std::wstring strLogLevel = boost::lexical_cast<std::wstring>(m_vm[loglevel].as<ansi::string>());
		if (boost::iequals(strLogLevel, L"none"))
		{
			m_LogSeverityLevel = static_cast<klog::severity_level>(klog::severity_level::debug - 1);
		}
		else
		{
			m_LogSeverityLevel = klog::GetLevelFromStr(strLogLevel);
		}
	}

	if (m_vm.count(logfile) == 0 && m_LogSeverityLevel != static_cast<decltype(m_LogSeverityLevel)>(klog::severity_level::debug - 1))
	{
		m_LogFilename += helpers::GetCurrentDirectory() + LR"(\project1.log)";

		*pstrOutput += L"logfile not specified, using " + m_LogFilename + L"\r\n";
	}
	else if (m_vm.count(logfile) != 1)
	{
		throw std::invalid_argument(logfile);
	}
	else
	{
		m_LogFilename = boost::lexical_cast<std::wstring>(m_vm[logfile].as<ansi::string>());
	}

	if (m_vm.count(bkmpath) == 1)
	{
		m_BkmPath = boost::lexical_cast<std::wstring>(m_vm[bkmpath].as<ansi::string>());

		while( m_BkmPath[m_BkmPath.size() - 1] == '\\')
		{
			m_BkmPath = m_BkmPath.substr(0, m_BkmPath.size() - 1);
		}
	}
	else
	{
		throw std::invalid_argument(ansi::string(bkmpath) + " not specified");
	}

	if (m_vm.count(cfgfile) != 1)
	{
		throw std::invalid_argument(ansi::string(cfgfile) + " not specified");
	}
	else
	{
		m_CfgFilename = ATL::CA2W(m_vm[cfgfile].as<ansi::string>().c_str());
	}

	m_UseConsole = false;
	if (m_vm.count(console) == 1)
	{
		m_UseConsole = true;
	}
	else if (m_vm.count(console) == 0)
	{
		*pstrOutput += L"Logging to console disabled\r\n";
	}
	else 
	{
		throw std::invalid_argument(console);
	}

	m_FlushLog = false;
	if (m_vm.count(flushlog) == 1)
	{
		std::wstring strFlushLog = boost::lexical_cast<std::wstring>(m_vm[flushlog].as<ansi::string>());
		if (boost::iequals(strFlushLog, L"on"))
		{
			m_FlushLog = true;
		}
		else if (!boost::iequals(strFlushLog, L"off"))
		{
			throw std::invalid_argument("Invalid flushlog value" + boost::lexical_cast<ansi::string>(strFlushLog));
		}
	}
	else if (m_vm.count(flushlog) == 0)
	{
		m_FlushLog = true;
		*pstrOutput += L"flushlog not specified, using ON\r\n";
	}
	else
	{
		throw std::invalid_argument(flushlog);
	}

	if(m_vm.count(adschemafile) == 0)
	{
		throw std::invalid_argument("adschemafile paramater must be specified");
	}

	m_ReloadADSchema = (m_vm.count(reloadadschema) != 0);
	m_ADSchemaFilename = boost::lexical_cast<std::wstring>(m_vm[adschemafile].as<ansi::string>());
}

void CCmdLine::PrintDescription(std::ostream& os)
{
	os << m_optDescr;
}
	
