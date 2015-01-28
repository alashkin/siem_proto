// Prototype1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "macroses.h"
#include "CmdLine.h"
#include "Config.h"
#include "JSONConfig.h"
#include "Log.h"
#include "EventCollector1.h"
#include <fstream>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/locale.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include "unicode.h"
#include <memory>
#include "BkmConfig.h"
#include <iostream>
#include <boost/format.hpp>
#include "XMLQueryGen.h"
#include "XMLQueryGenBase.h"
#include "XMLQueryFromFile.h"
#include "static_containers.h"
#include "Dispatcher1.h"
#include <vector>
#include "EventProcessorSelector.h"
#include "EventUtils.h"
#include "ObjectMovedProcessor.h"
#include "ClearEventLogProcessor.h"
#include "ObjectDeletedProcessor.h"
#include "AttributeValuesParser.h"
#include "GroupTypeParser.h"
#include "AttributeValueParserBase.h"
#include "UACParser.h"
#include "SIEMWriter.h"
#include "DefaultAttributeParser.h"
#include "ADSchemaReader.h"
#include "ADSchemaSerializer.h"
#include "ADSchema.h"

namespace logging = boost::log;

static void LogCmdLine(int argc, _TCHAR* argv[]);
static void InitLog(const CCmdLine& cmdLine);

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		std::cout << "Project1. (C) Kapaai Group 2013." << std::endl;

		CCmdLine cmdLine;
		cmdLine.Prepare(argc, argv);

		try
		{
			std::wstring strResults;
			cmdLine.Parse(&strResults);

			if (!strResults.empty())
			{
				std::wcout << strResults;
			}
		}
		catch (std::exception& e)
		{
			std::wcout << L"Error: " << boost::lexical_cast<std::wstring>(e) << std::endl;

			cmdLine.PrintDescription(std::cout);

			throw;
		}

		InitLog(cmdLine);

		_LOG(info) << "Project1 started.";

		LogCmdLine(argc, argv);

		_LOG(info) << "Loading configuration from " << cmdLine.get_CfgFilename();

		CJSONConfig jsonCfg;
		try
		{
			jsonCfg.Init(cmdLine.get_CfgFilename());
			_LOG(info) << "Configuration loaded succesfully";
		}
		catch (std::exception& e)
		{
			_LOG(fatal) << L"Configuration error: " << boost::lexical_cast<std::wstring>(e);

			throw;
		}

		std::shared_ptr<IXMLQueryGen> queryGen;
		if (jsonCfg.GetQueryType() == CJSONConfig::qtEvents)
		{
			std::shared_ptr<CXMLQueryGen> xmlQueryGen = std::make_shared<CXMLQueryGen>();
			xmlQueryGen->Generate(jsonCfg.GetEventsToCollect(), jsonCfg.GetEventsToSuppress(), jsonCfg.GetMaxIDinQuery());
			queryGen = std::static_pointer_cast<IXMLQueryGen>(xmlQueryGen);
		}
		else
		{
			std::shared_ptr<CXMLQueryFromFile> xmlQueryGen = std::make_shared<CXMLQueryFromFile>();
			xmlQueryGen->Load(jsonCfg.GetQueryFile());
			queryGen = std::static_pointer_cast<IXMLQueryGen>(xmlQueryGen);
		}

		ADUtils::SchemaInfo si;

		CADSchemaSerializer::Params adssp;
		adssp.fileName = cmdLine.get_ADSchemaFilename();
		CADSchemaSerializer adss(adssp);

		if (cmdLine.get_ReloadADSchema())
		{
			// forcing read AD schema
			CADSchemaReader adsr(jsonCfg.GetSettings().adSchema.server,
								 jsonCfg.GetSettings().adSchema.domain_fqdn,
							  	 jsonCfg.GetSettings().adSchema.user,
								 jsonCfg.GetSettings().adSchema.password);
			
			adsr.Read(&si);
			adss.Write(si);
		}
		else
		{
			adss.Read(&si);
		}

		std::shared_ptr<CADSchema> ad_schema 
										= std::make_shared<CADSchema>(si);

		CDispatcher1::Params dispParams;

		auto& targets = jsonCfg.GetTargets();
		for (auto& target : targets)
		{
			CChangeGenerator1::Params generatorParams;

			CEventCollector1::Params eventCollectorParams;

			CBkmConfig bkmConfig(cmdLine.get_BkmPath());

			eventCollectorParams.bkmStg = bkmConfig.GetBkmStorage(target.server);
			eventCollectorParams.target = std::make_shared<Config::Target>(target);
			eventCollectorParams.xmlQueryGen = queryGen;

			generatorParams.eventCollector = std::make_shared<CEventCollector1>(eventCollectorParams);

			CEventProcessor2::Params eventProcessorParams;
			eventProcessorParams.max_event_tolerance = target.max_event_tolerance;
			eventProcessorParams.wait_for_event_min = target.wait_for_event_min;
			eventProcessorParams.wait_for_event_max = target.wait_for_event_max;

			std::shared_ptr<CEventProcessor2> evtProc1 = std::make_shared<CEventProcessor2>(eventProcessorParams);


			CEventProcessorSelector::Params::EIDProc eidproc1;
			eidproc1.eventIDs.insert(EventUtils::Consts::ID::EID_ObjectCreated);
			eidproc1.eventIDs.insert(EventUtils::Consts::ID::EID_ObjectModified);
			eidproc1.eventIDs.insert(EventUtils::Consts::ID::EID_ObjectUndeleted);
			eidproc1.proc = evtProc1;

			std::shared_ptr<CObjectMovedProcessor> evtProc2 = std::make_shared<CObjectMovedProcessor>();
			CEventProcessorSelector::Params::EIDProc eidproc2;
			eidproc2.eventIDs.insert(EventUtils::Consts::ID::EID_ObjectMoved);
			eidproc2.proc = evtProc2;

			std::shared_ptr<CEventLogClearedProcessor> evtProc3 = std::make_shared<CEventLogClearedProcessor>();
			CEventProcessorSelector::Params::EIDProc eidproc3;
			eidproc3.eventIDs.insert(EventUtils::Consts::ID::EID_EventLogCleared);
			eidproc3.proc = evtProc3;

			std::shared_ptr<CObjectDeletedProcessor> evtProc4 = std::make_shared<CObjectDeletedProcessor>();
			CEventProcessorSelector::Params::EIDProc eidproc4;
			eidproc4.eventIDs.insert(EventUtils::Consts::ID::EID_ObjectDeleted);
			eidproc4.proc = evtProc4;


			CEventProcessorSelector::Params epsParams;
			epsParams.processors.push_back(eidproc1);
			epsParams.processors.push_back(eidproc2);
			epsParams.processors.push_back(eidproc3);
			epsParams.processors.push_back(eidproc4);
			generatorParams.eventProcessorSelector = std::make_shared<CEventProcessorSelector>(epsParams);

			CAttributeValuesParser::Params avpParams;
			avpParams.parsers = std::make_shared<CAttributeValuesParser::parsers_map_t>();
			(*avpParams.parsers)[L"group"][L"groupType"] = std::make_shared<CGroupTypeParser>();
			(*avpParams.parsers)[L"user"][L"userAccountControl"] = std::make_shared<CUACParser>();
			avpParams.defaultParser = std::make_shared<CDefaultAttributeParser>(ad_schema);

			generatorParams.attributeValuesParser = std::make_shared<CAttributeValuesParser>(avpParams);
			std::shared_ptr<CChangeGenerator1> generator = std::make_shared<CChangeGenerator1>(generatorParams);

			dispParams.changeGenerators.push_back(generator);
		}

		CDispatcher1 disp(dispParams);
		disp.Start();
		_LOG(info) << "Running.Press Enter to stop";
		getchar();

		disp.Stop();
		_LOG(info) << "Stopped. Press Enter to close";
		getchar();

	}
	catch (std::exception& e)
	{
		_LOG(fatal) << boost::lexical_cast<std::wstring>(e);
	}

	return 0;
}

void LogCmdLine(int argc, _TCHAR* argv[])
{
	_LOG_PREPARE_REC();

	_LOG_OPEN_REC(info);
	_LOG_ADD_TO_REC() << "Command line: ";
	for (int i = 1; i < argc; ++i)
	{
		_LOG_ADD_TO_REC() << std::wstring(argv[i]) << " ";
	}

	_LOG_CLOSE_REC();
}

void InitLog(const CCmdLine& cmdLine)
{
	klog::InitLogging(cmdLine.get_LogFilename(), cmdLine.get_LogSeverityLevel(), cmdLine.get_FlushLog(), cmdLine.get_UseConsole());
}

