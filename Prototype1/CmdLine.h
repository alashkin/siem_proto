#pragma once

#include "log.h"
#include "macroses.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <string>

class CCmdLine
{
private:
	bool m_UseConsole;
	klog::severity_level m_LogSeverityLevel;
	std::wstring m_LogFilename;
	std::wstring m_CfgFilename;
	std::wstring m_ADSchemaFilename;
	std::wstring m_BkmPath;
	bool m_FlushLog;
	bool m_ReloadADSchema;
	boost::program_options::options_description m_optDescr;
	boost::program_options::variables_map m_vm;
	DEFINE_CONST(loglevel);
	DEFINE_CONST(console);
	DEFINE_CONST(logfile);
	DEFINE_CONST(cfgfile);
	DEFINE_CONST(bkmpath);
	DEFINE_CONST(flushlog);
	DEFINE_CONST(adschemafile);
	DEFINE_CONST(reloadadschema);

public:
	DEFINE_GET(UseConsole);
	DEFINE_GET(LogSeverityLevel);
	DEFINE_GET(LogFilename);
	DEFINE_GET(CfgFilename);
	DEFINE_GET(BkmPath);
	DEFINE_GET(FlushLog);
	DEFINE_GET(ReloadADSchema);
	DEFINE_GET(ADSchemaFilename);

	void Prepare(int argc, _TCHAR* argv[]);
	void Parse(std::wstring* pstrOutput);
	void PrintDescription(std::ostream& os);

	CCmdLine();
	virtual ~CCmdLine();

};

