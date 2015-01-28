#pragma once

#include <boost/log/common.hpp>
#include <boost/lexical_cast.hpp>
#include <string>

#define _LOG(lvl) BOOST_LOG_SEV(klog::global_logger, klog::severity_level::lvl)
#define _LOG_PREPARE_REC() boost::log::record __log_rec;boost::log::record_ostream __log_strm; 
#define _LOG_OPEN_REC(lvl) __log_rec = klog::global_logger.open_record(boost::log::keywords::severity = klog::lvl); if( __log_rec) __log_strm.attach_record( __log_rec);  
#define _LOG_ADD_TO_REC() if( __log_rec) __log_strm
#define _LOG_CLOSE_REC() if( __log_rec) { __log_strm.flush(); klog::global_logger.push_record(boost::move(__log_rec)); };

#define _LOG_REC() __log_strm

#define _LOG_REC_TYPE boost::log::record_ostream

namespace klog
{
	enum severity_level
	{
		debug,
		info,
		warning,
		error,
		fatal
	};

	void InitLogging(const std::wstring& filename, severity_level lvl, bool auto_flush, bool console_logging);

	severity_level GetLevelFromStr(const std::wstring& str);
	std::wstring GetStrFromLevel(severity_level lvl);

	// меня здесь нет
	extern boost::log::sources::wseverity_logger< severity_level > global_logger;
}

