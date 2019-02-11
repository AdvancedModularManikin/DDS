#pragma once

#include <plog/Log.h>
#include <plog/Formatters/FuncMessageFormatter.h>

#include <AMM/DDS_Manager.h>

using namespace AMM;

namespace plog {
    template<class Formatter>
    class DDS_Log_Appender : public IAppender {
    public:
        explicit DDS_Log_Appender(DDS_Manager *mgr)
                : m_mgr(mgr) {

        }

        virtual void write(const Record &record) {
            util::nstring message = record.getMessage();
            util::nstring severity = plog::severityToString(record.getSeverity());
            // util::nstring str = Formatter::format(record);
            m_mgr->PublishLogRecord(message, severity);
            //m_mgr->PublishLogRecord(str);
        }

    private:
        DDS_Manager *m_mgr;
    };
};

