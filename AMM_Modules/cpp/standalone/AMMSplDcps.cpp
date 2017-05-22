#include "AMMSplDcps.h"
#include "ccpp_AMM.h"
#include "dds_type_aliases.h"

const char *
__AMM_Simulation_Tick__name(void)
{
    return (const char*)"AMM::Simulation::Tick";
}

const char *
__AMM_Simulation_Tick__keys(void)
{
    return (const char*)"frame";
}

const char *
__AMM_Physiology_Data__name(void)
{
    return (const char*)"AMM::Physiology::Data";
}

const char *
__AMM_Physiology_Data__keys(void)
{
    return (const char*)"node_path";
}

const char *
__AMM_Physiology_HighFrequencyData__name(void)
{
    return (const char*)"AMM::Physiology::HighFrequencyData";
}

const char *
__AMM_Physiology_HighFrequencyData__keys(void)
{
    return (const char*)"node_path";
}

#include <v_kernel.h>
#include <v_topic.h>
#include <os_stdlib.h>
#include <string.h>
#include <os_report.h>

c_bool
__AMM_Simulation_Tick__copyIn(
    c_base base,
    struct ::AMM::Simulation::Tick *from,
    struct _AMM_Simulation_Tick *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

    to->frame = (c_longlong)from->frame;
    to->time = (c_float)from->time;
    return result;
}

c_bool
__AMM_Physiology_Data__copyIn(
    c_base base,
    struct ::AMM::Physiology::Data *from,
    struct _AMM_Physiology_Data *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

#ifdef OSPL_BOUNDS_CHECK
    if(from->node_path){
        to->node_path = c_stringNew(base, from->node_path);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::Data.node_path' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->node_path = c_stringNew(base, from->node_path);
#endif
#ifdef OSPL_BOUNDS_CHECK
    if(from->unit){
        to->unit = c_stringNew(base, from->unit);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::Data.unit' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->unit = c_stringNew(base, from->unit);
#endif
    to->dbl = (c_double)from->dbl;
#ifdef OSPL_BOUNDS_CHECK
    if(from->str){
        to->str = c_stringNew(base, from->str);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::Data.str' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->str = c_stringNew(base, from->str);
#endif
    return result;
}

c_bool
__AMM_Physiology_HighFrequencyData__copyIn(
    c_base base,
    struct ::AMM::Physiology::HighFrequencyData *from,
    struct _AMM_Physiology_HighFrequencyData *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

#ifdef OSPL_BOUNDS_CHECK
    if(from->node_path){
        to->node_path = c_stringNew(base, from->node_path);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::HighFrequencyData.node_path' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->node_path = c_stringNew(base, from->node_path);
#endif
#ifdef OSPL_BOUNDS_CHECK
    if(from->unit){
        to->unit = c_stringNew(base, from->unit);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::HighFrequencyData.unit' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->unit = c_stringNew(base, from->unit);
#endif
    to->dbl = (c_double)from->dbl;
#ifdef OSPL_BOUNDS_CHECK
    if(from->str){
        to->str = c_stringNew(base, from->str);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::HighFrequencyData.str' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->str = c_stringNew(base, from->str);
#endif
    return result;
}

void
__AMM_Simulation_Tick__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Simulation_Tick *from = (struct _AMM_Simulation_Tick *)_from;
    struct ::AMM::Simulation::Tick *to = (struct ::AMM::Simulation::Tick *)_to;
    to->frame = (::DDS::LongLong)from->frame;
    to->time = (::DDS::Float)from->time;
}

void
__AMM_Physiology_Data__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Physiology_Data *from = (struct _AMM_Physiology_Data *)_from;
    struct ::AMM::Physiology::Data *to = (struct ::AMM::Physiology::Data *)_to;
    to->node_path = DDS::string_dup(from->node_path ? from->node_path : "");
    to->unit = DDS::string_dup(from->unit ? from->unit : "");
    to->dbl = (::DDS::Double)from->dbl;
    to->str = DDS::string_dup(from->str ? from->str : "");
}

void
__AMM_Physiology_HighFrequencyData__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Physiology_HighFrequencyData *from = (struct _AMM_Physiology_HighFrequencyData *)_from;
    struct ::AMM::Physiology::HighFrequencyData *to = (struct ::AMM::Physiology::HighFrequencyData *)_to;
    to->node_path = DDS::string_dup(from->node_path ? from->node_path : "");
    to->unit = DDS::string_dup(from->unit ? from->unit : "");
    to->dbl = (::DDS::Double)from->dbl;
    to->str = DDS::string_dup(from->str ? from->str : "");
}

