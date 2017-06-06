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
__AMM_Physiology_Node__name(void)
{
    return (const char*)"AMM::Physiology::Node";
}

const char *
__AMM_Physiology_Node__keys(void)
{
    return (const char*)"frame";
}

const char *
__AMM_Physiology_HighFrequencyNode__name(void)
{
    return (const char*)"AMM::Physiology::HighFrequencyNode";
}

const char *
__AMM_Physiology_HighFrequencyNode__keys(void)
{
    return (const char*)"frame";
}

const char *
__AMM_PatientAction_BioGears_Command__name(void)
{
    return (const char*)"AMM::PatientAction::BioGears::Command";
}

const char *
__AMM_PatientAction_BioGears_Command__keys(void)
{
    return (const char*)"message";
}

const char *
__AMM_Performance_xAPIModifier__name(void)
{
    return (const char*)"AMM::Performance::xAPIModifier";
}

const char *
__AMM_Performance_xAPIModifier__keys(void)
{
    return (const char*)"key";
}

const char *
__AMM_Performance_Statement__name(void)
{
    return (const char*)"AMM::Performance::Statement";
}

const char *
__AMM_Performance_Statement__keys(void)
{
    return (const char*)"noun";
}

const char *
__AMM_Sys_Log_info__name(void)
{
    return (const char*)"AMM::Sys::Log::info";
}

const char *
__AMM_Sys_Log_info__keys(void)
{
    return (const char*)"";
}

const char *
__AMM_Sys_Log_warning__name(void)
{
    return (const char*)"AMM::Sys::Log::warning";
}

const char *
__AMM_Sys_Log_warning__keys(void)
{
    return (const char*)"";
}

const char *
__AMM_Sys_Log_error__name(void)
{
    return (const char*)"AMM::Sys::Log::error";
}

const char *
__AMM_Sys_Log_error__keys(void)
{
    return (const char*)"";
}

const char *
__AMM_Sys_Log_debug__name(void)
{
    return (const char*)"AMM::Sys::Log::debug";
}

const char *
__AMM_Sys_Log_debug__keys(void)
{
    return (const char*)"";
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
__AMM_Physiology_Node__copyIn(
    c_base base,
    struct ::AMM::Physiology::Node *from,
    struct _AMM_Physiology_Node *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

    to->frame = (c_longlong)from->frame;
#ifdef OSPL_BOUNDS_CHECK
    if(from->nodepath){
        to->nodepath = c_stringNew(base, from->nodepath);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::Node.nodepath' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->nodepath = c_stringNew(base, from->nodepath);
#endif
#ifdef OSPL_BOUNDS_CHECK
    if(from->unit){
        to->unit = c_stringNew(base, from->unit);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::Node.unit' of type 'c_string' is NULL.");
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
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::Node.str' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->str = c_stringNew(base, from->str);
#endif
    return result;
}

c_bool
__AMM_Physiology_HighFrequencyNode__copyIn(
    c_base base,
    struct ::AMM::Physiology::HighFrequencyNode *from,
    struct _AMM_Physiology_HighFrequencyNode *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

    to->frame = (c_longlong)from->frame;
#ifdef OSPL_BOUNDS_CHECK
    if(from->nodepath){
        to->nodepath = c_stringNew(base, from->nodepath);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::HighFrequencyNode.nodepath' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->nodepath = c_stringNew(base, from->nodepath);
#endif
#ifdef OSPL_BOUNDS_CHECK
    if(from->unit){
        to->unit = c_stringNew(base, from->unit);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::HighFrequencyNode.unit' of type 'c_string' is NULL.");
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
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Physiology::HighFrequencyNode.str' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->str = c_stringNew(base, from->str);
#endif
    return result;
}

c_bool
__AMM_PatientAction_BioGears_Command__copyIn(
    c_base base,
    struct ::AMM::PatientAction::BioGears::Command *from,
    struct _AMM_PatientAction_BioGears_Command *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

#ifdef OSPL_BOUNDS_CHECK
    if(from->message){
        to->message = c_stringNew(base, from->message);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::PatientAction::BioGears::Command.message' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->message = c_stringNew(base, from->message);
#endif
    return result;
}

c_bool
__AMM_Performance_xAPIModifier__copyIn(
    c_base base,
    struct ::AMM::Performance::xAPIModifier *from,
    struct _AMM_Performance_xAPIModifier *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

#ifdef OSPL_BOUNDS_CHECK
    if(from->key){
        to->key = c_stringNew(base, from->key);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Performance::xAPIModifier.key' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->key = c_stringNew(base, from->key);
#endif
#ifdef OSPL_BOUNDS_CHECK
    if(from->value){
        to->value = c_stringNew(base, from->value);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Performance::xAPIModifier.value' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->value = c_stringNew(base, from->value);
#endif
    return result;
}

c_bool
__AMM_Performance_xAPI_Modifiers__copyIn(
    c_base base,
    ::AMM::Performance::xAPI_Modifiers *from,
    _AMM_Performance_xAPI_Modifiers *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

/* Code generated by /home/dds/OvernightTests/tmp.Zi75hUiMaK/build/src/tools/idlpp/code/idl_genCorbaCxxCopyin.c at line 2411 */

    static c_type type0 = NULL;
    c_type subtype0 = NULL;
    c_long length0;
    struct _AMM_Performance_xAPIModifier *dest0;

    if (type0 == NULL) {
        subtype0 = c_type(c_metaResolve (c_metaObject(base), "AMM::Performance::xAPIModifier"));
        type0 = c_metaSequenceTypeNew(c_metaObject(base),"C_SEQUENCE<AMM::Performance::xAPIModifier>",subtype0,0);
        c_free(subtype0);
    }
    length0 = (c_long)(*from).length();
#ifdef OSPL_BOUNDS_CHECK
    dest0 = (struct _AMM_Performance_xAPIModifier *)c_newSequence(c_collectionType(type0), length0);
    {
    /* Code generated by /home/dds/OvernightTests/tmp.Zi75hUiMaK/build/src/tools/idlpp/code/idl_genCorbaCxxCopyin.c at line 1928 */
        unsigned int i0;
        for (i0 = 0; (i0 < (unsigned int)length0) && result; i0++) {
            extern c_bool __AMM_Performance_xAPIModifier__copyIn(c_base base,
                AMM::Performance::xAPIModifier *From,
                struct _AMM_Performance_xAPIModifier *To);

            result = __AMM_Performance_xAPIModifier__copyIn(base, &(*from)[i0], (struct _AMM_Performance_xAPIModifier *)&dest0[i0]);
        }
    }
    *to = (_AMM_Performance_xAPI_Modifiers)dest0;
#else
    dest0 = (struct _AMM_Performance_xAPIModifier *)c_newSequence(c_collectionType(type0), length0);
    {
    /* Code generated by /home/dds/OvernightTests/tmp.Zi75hUiMaK/build/src/tools/idlpp/code/idl_genCorbaCxxCopyin.c at line 1928 */
        unsigned int i0;
        for (i0 = 0; (i0 < (unsigned int)length0) && result; i0++) {
            extern c_bool __AMM_Performance_xAPIModifier__copyIn(c_base base,
                AMM::Performance::xAPIModifier *From,
                struct _AMM_Performance_xAPIModifier *To);

            result = __AMM_Performance_xAPIModifier__copyIn(base, &(*from)[i0], (struct _AMM_Performance_xAPIModifier *)&dest0[i0]);
        }
    }
    *to = (_AMM_Performance_xAPI_Modifiers)dest0;
#endif
    return result;
}

c_bool
__AMM_Performance_Statement__copyIn(
    c_base base,
    struct ::AMM::Performance::Statement *from,
    struct _AMM_Performance_Statement *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

#ifdef OSPL_BOUNDS_CHECK
    if(from->noun){
        to->noun = c_stringNew(base, from->noun);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Performance::Statement.noun' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->noun = c_stringNew(base, from->noun);
#endif
    if(result){
        extern c_bool __AMM_Performance_xAPI_Modifiers__copyIn(c_base, ::AMM::Performance::xAPI_Modifiers *, _AMM_Performance_xAPI_Modifiers *);
        result = __AMM_Performance_xAPI_Modifiers__copyIn(base, &from->noun_modifiers, &to->noun_modifiers);
    }
#ifdef OSPL_BOUNDS_CHECK
    if(from->verb){
        to->verb = c_stringNew(base, from->verb);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Performance::Statement.verb' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->verb = c_stringNew(base, from->verb);
#endif
    if(result){
        extern c_bool __AMM_Performance_xAPI_Modifiers__copyIn(c_base, ::AMM::Performance::xAPI_Modifiers *, _AMM_Performance_xAPI_Modifiers *);
        result = __AMM_Performance_xAPI_Modifiers__copyIn(base, &from->verb_modifiers, &to->verb_modifiers);
    }
#ifdef OSPL_BOUNDS_CHECK
    if(from->obj){
        to->obj = c_stringNew(base, from->obj);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Performance::Statement.obj' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->obj = c_stringNew(base, from->obj);
#endif
    if(result){
        extern c_bool __AMM_Performance_xAPI_Modifiers__copyIn(c_base, ::AMM::Performance::xAPI_Modifiers *, _AMM_Performance_xAPI_Modifiers *);
        result = __AMM_Performance_xAPI_Modifiers__copyIn(base, &from->obj_modifiers, &to->obj_modifiers);
    }
    return result;
}

c_bool
__AMM_Sys_Log_info__copyIn(
    c_base base,
    struct ::AMM::Sys::Log::info *from,
    struct _AMM_Sys_Log_info *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

#ifdef OSPL_BOUNDS_CHECK
    if(from->message){
        to->message = c_stringNew(base, from->message);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Sys::Log::info.message' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->message = c_stringNew(base, from->message);
#endif
    return result;
}

c_bool
__AMM_Sys_Log_warning__copyIn(
    c_base base,
    struct ::AMM::Sys::Log::warning *from,
    struct _AMM_Sys_Log_warning *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

#ifdef OSPL_BOUNDS_CHECK
    if(from->message){
        to->message = c_stringNew(base, from->message);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Sys::Log::warning.message' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->message = c_stringNew(base, from->message);
#endif
    return result;
}

c_bool
__AMM_Sys_Log_error__copyIn(
    c_base base,
    struct ::AMM::Sys::Log::error *from,
    struct _AMM_Sys_Log_error *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

#ifdef OSPL_BOUNDS_CHECK
    if(from->message){
        to->message = c_stringNew(base, from->message);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Sys::Log::error.message' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->message = c_stringNew(base, from->message);
#endif
    return result;
}

c_bool
__AMM_Sys_Log_debug__copyIn(
    c_base base,
    struct ::AMM::Sys::Log::debug *from,
    struct _AMM_Sys_Log_debug *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

#ifdef OSPL_BOUNDS_CHECK
    if(from->message){
        to->message = c_stringNew(base, from->message);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'AMM::Sys::Log::debug.message' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->message = c_stringNew(base, from->message);
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
__AMM_Physiology_Node__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Physiology_Node *from = (struct _AMM_Physiology_Node *)_from;
    struct ::AMM::Physiology::Node *to = (struct ::AMM::Physiology::Node *)_to;
    to->frame = (::DDS::LongLong)from->frame;
    to->nodepath = DDS::string_dup(from->nodepath ? from->nodepath : "");
    to->unit = DDS::string_dup(from->unit ? from->unit : "");
    to->dbl = (::DDS::Double)from->dbl;
    to->str = DDS::string_dup(from->str ? from->str : "");
}

void
__AMM_Physiology_HighFrequencyNode__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Physiology_HighFrequencyNode *from = (struct _AMM_Physiology_HighFrequencyNode *)_from;
    struct ::AMM::Physiology::HighFrequencyNode *to = (struct ::AMM::Physiology::HighFrequencyNode *)_to;
    to->frame = (::DDS::LongLong)from->frame;
    to->nodepath = DDS::string_dup(from->nodepath ? from->nodepath : "");
    to->unit = DDS::string_dup(from->unit ? from->unit : "");
    to->dbl = (::DDS::Double)from->dbl;
    to->str = DDS::string_dup(from->str ? from->str : "");
}

void
__AMM_PatientAction_BioGears_Command__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_PatientAction_BioGears_Command *from = (struct _AMM_PatientAction_BioGears_Command *)_from;
    struct ::AMM::PatientAction::BioGears::Command *to = (struct ::AMM::PatientAction::BioGears::Command *)_to;
    to->message = DDS::string_dup(from->message ? from->message : "");
}

void
__AMM_Performance_xAPIModifier__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Performance_xAPIModifier *from = (struct _AMM_Performance_xAPIModifier *)_from;
    struct ::AMM::Performance::xAPIModifier *to = (struct ::AMM::Performance::xAPIModifier *)_to;
    to->key = DDS::string_dup(from->key ? from->key : "");
    to->value = DDS::string_dup(from->value ? from->value : "");
}

void
__AMM_Performance_xAPI_Modifiers__copyOut(
    void *_from,
    void *_to)
{
    _AMM_Performance_xAPI_Modifiers *from = (_AMM_Performance_xAPI_Modifiers *)_from;
    ::AMM::Performance::xAPI_Modifiers *to = (::AMM::Performance::xAPI_Modifiers *)_to;
    long size0;
    struct _AMM_Performance_xAPIModifier *src0 = (struct _AMM_Performance_xAPIModifier *)(*from);

    size0 = c_arraySize(c_sequence(src0));
    (*to).length(size0);
    {
        long i0;
        for (i0 = 0; i0 < size0; i0++) {
            extern void __AMM_Performance_xAPIModifier__copyOut(void *from, void *to);
            __AMM_Performance_xAPIModifier__copyOut((void *)&src0[i0], (void *)&(*to)[i0]);
        }
    }
}

void
__AMM_Performance_Statement__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Performance_Statement *from = (struct _AMM_Performance_Statement *)_from;
    struct ::AMM::Performance::Statement *to = (struct ::AMM::Performance::Statement *)_to;
    to->noun = DDS::string_dup(from->noun ? from->noun : "");
    {
        extern void __AMM_Performance_xAPI_Modifiers__copyOut(void *, void *);
        __AMM_Performance_xAPI_Modifiers__copyOut((void *)&from->noun_modifiers, (void *)&to->noun_modifiers);
    }
    to->verb = DDS::string_dup(from->verb ? from->verb : "");
    {
        extern void __AMM_Performance_xAPI_Modifiers__copyOut(void *, void *);
        __AMM_Performance_xAPI_Modifiers__copyOut((void *)&from->verb_modifiers, (void *)&to->verb_modifiers);
    }
    to->obj = DDS::string_dup(from->obj ? from->obj : "");
    {
        extern void __AMM_Performance_xAPI_Modifiers__copyOut(void *, void *);
        __AMM_Performance_xAPI_Modifiers__copyOut((void *)&from->obj_modifiers, (void *)&to->obj_modifiers);
    }
}

void
__AMM_Sys_Log_info__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Sys_Log_info *from = (struct _AMM_Sys_Log_info *)_from;
    struct ::AMM::Sys::Log::info *to = (struct ::AMM::Sys::Log::info *)_to;
    to->message = DDS::string_dup(from->message ? from->message : "");
}

void
__AMM_Sys_Log_warning__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Sys_Log_warning *from = (struct _AMM_Sys_Log_warning *)_from;
    struct ::AMM::Sys::Log::warning *to = (struct ::AMM::Sys::Log::warning *)_to;
    to->message = DDS::string_dup(from->message ? from->message : "");
}

void
__AMM_Sys_Log_error__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Sys_Log_error *from = (struct _AMM_Sys_Log_error *)_from;
    struct ::AMM::Sys::Log::error *to = (struct ::AMM::Sys::Log::error *)_to;
    to->message = DDS::string_dup(from->message ? from->message : "");
}

void
__AMM_Sys_Log_debug__copyOut(
    void *_from,
    void *_to)
{
    struct _AMM_Sys_Log_debug *from = (struct _AMM_Sys_Log_debug *)_from;
    struct ::AMM::Sys::Log::debug *to = (struct ::AMM::Sys::Log::debug *)_to;
    to->message = DDS::string_dup(from->message ? from->message : "");
}

