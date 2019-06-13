#pragma once

#include <AMM/DDS_Manager.h>
#include <boost/algorithm/string.hpp>
#include <plog/Formatters/FuncMessageFormatter.h>
#include <plog/Log.h>

#include <codecvt>
#include <locale>
#include <string>

using namespace AMM;

namespace plog
{
template <class Formatter>
class DDS_Log_Appender : public IAppender
{
  public:
    explicit DDS_Log_Appender(DDS_Manager* mgr)
        : m_mgr(mgr)
    {
    }

    virtual void write(const Record& record)
    {
        std::ostringstream message;
        const char * cSeverity = plog::severityToString(record.getSeverity());        
		message << record.getMessage();        
        std::string severity = std::string(cSeverity);
        const std::string &smessage = message.str();
        const std::string &sseverity = severity;
        
        if (boost::starts_with(smessage, "Serial debug:"))
        {
            return;
        }

        try
        {
            m_mgr->PublishLogRecord(smessage, sseverity);
        }
        catch (std::exception& e)
        {
            LOG_ERROR << e.what();
        }
    }

  private:
    DDS_Manager* m_mgr;
};
};
