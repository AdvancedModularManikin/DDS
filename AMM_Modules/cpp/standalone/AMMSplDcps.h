#ifndef AMMSPLTYPES_H
#define AMMSPLTYPES_H

#include "ccpp_AMM.h"

#include <c_base.h>
#include <c_misc.h>
#include <c_sync.h>
#include <c_collection.h>
#include <c_field.h>

extern c_metaObject __AMM_AMM__load (c_base base);

extern c_metaObject __AMM_AMM_Simulation__load (c_base base);

extern c_metaObject __AMM_Simulation_Tick__load (c_base base);
extern const char * __AMM_Simulation_Tick__keys (void);
extern const char * __AMM_Simulation_Tick__name (void);
struct _AMM_Simulation_Tick ;
extern  c_bool __AMM_Simulation_Tick__copyIn(c_base base, struct AMM::Simulation::Tick *from, struct _AMM_Simulation_Tick *to);
extern  void __AMM_Simulation_Tick__copyOut(void *_from, void *_to);
struct _AMM_Simulation_Tick {
    c_longlong frame;
    c_float time;
};

extern c_metaObject __AMM_AMM_Physiology__load (c_base base);

extern c_metaObject __AMM_Physiology_Node__load (c_base base);
extern const char * __AMM_Physiology_Node__keys (void);
extern const char * __AMM_Physiology_Node__name (void);
struct _AMM_Physiology_Node ;
extern  c_bool __AMM_Physiology_Node__copyIn(c_base base, struct AMM::Physiology::Node *from, struct _AMM_Physiology_Node *to);
extern  void __AMM_Physiology_Node__copyOut(void *_from, void *_to);
struct _AMM_Physiology_Node {
    c_longlong frame;
    c_string nodepath;
    c_string unit;
    c_double dbl;
    c_string str;
};

extern c_metaObject __AMM_Physiology_HighFrequencyNode__load (c_base base);
extern const char * __AMM_Physiology_HighFrequencyNode__keys (void);
extern const char * __AMM_Physiology_HighFrequencyNode__name (void);
struct _AMM_Physiology_HighFrequencyNode ;
extern  c_bool __AMM_Physiology_HighFrequencyNode__copyIn(c_base base, struct AMM::Physiology::HighFrequencyNode *from, struct _AMM_Physiology_HighFrequencyNode *to);
extern  void __AMM_Physiology_HighFrequencyNode__copyOut(void *_from, void *_to);
struct _AMM_Physiology_HighFrequencyNode {
    c_longlong frame;
    c_string nodepath;
    c_string unit;
    c_double dbl;
    c_string str;
};

extern c_metaObject __AMM_AMM_PatientAction__load (c_base base);

extern c_metaObject __AMM_AMM_PatientAction_BioGears__load (c_base base);

extern c_metaObject __AMM_PatientAction_BioGears_Command__load (c_base base);
extern const char * __AMM_PatientAction_BioGears_Command__keys (void);
extern const char * __AMM_PatientAction_BioGears_Command__name (void);
struct _AMM_PatientAction_BioGears_Command ;
extern  c_bool __AMM_PatientAction_BioGears_Command__copyIn(c_base base, struct AMM::PatientAction::BioGears::Command *from, struct _AMM_PatientAction_BioGears_Command *to);
extern  void __AMM_PatientAction_BioGears_Command__copyOut(void *_from, void *_to);
struct _AMM_PatientAction_BioGears_Command {
    c_string message;
};

extern c_metaObject __AMM_AMM_Performance__load (c_base base);

extern c_metaObject __AMM_Performance_xAPIModifier__load (c_base base);
extern const char * __AMM_Performance_xAPIModifier__keys (void);
extern const char * __AMM_Performance_xAPIModifier__name (void);
struct _AMM_Performance_xAPIModifier ;
extern  c_bool __AMM_Performance_xAPIModifier__copyIn(c_base base, struct AMM::Performance::xAPIModifier *from, struct _AMM_Performance_xAPIModifier *to);
extern  void __AMM_Performance_xAPIModifier__copyOut(void *_from, void *_to);
struct _AMM_Performance_xAPIModifier {
    c_string key;
    c_string value;
};

extern c_metaObject __AMM_Performance_xAPI_Modifiers__load (c_base base);
typedef c_sequence _AMM_Performance_xAPI_Modifiers;

extern c_metaObject __AMM_Performance_Statement__load (c_base base);
extern const char * __AMM_Performance_Statement__keys (void);
extern const char * __AMM_Performance_Statement__name (void);
struct _AMM_Performance_Statement ;
extern  c_bool __AMM_Performance_Statement__copyIn(c_base base, struct AMM::Performance::Statement *from, struct _AMM_Performance_Statement *to);
extern  void __AMM_Performance_Statement__copyOut(void *_from, void *_to);
struct _AMM_Performance_Statement {
    c_string noun;
    _AMM_Performance_xAPI_Modifiers noun_modifiers;
    c_string verb;
    _AMM_Performance_xAPI_Modifiers verb_modifiers;
    c_string obj;
    _AMM_Performance_xAPI_Modifiers obj_modifiers;
};

extern c_metaObject __AMM_AMM_Sys__load (c_base base);

extern c_metaObject __AMM_AMM_Sys_Log__load (c_base base);

extern c_metaObject __AMM_Sys_Log_info__load (c_base base);
extern const char * __AMM_Sys_Log_info__keys (void);
extern const char * __AMM_Sys_Log_info__name (void);
struct _AMM_Sys_Log_info ;
extern  c_bool __AMM_Sys_Log_info__copyIn(c_base base, struct AMM::Sys::Log::info *from, struct _AMM_Sys_Log_info *to);
extern  void __AMM_Sys_Log_info__copyOut(void *_from, void *_to);
struct _AMM_Sys_Log_info {
    c_string message;
};

extern c_metaObject __AMM_Sys_Log_warning__load (c_base base);
extern const char * __AMM_Sys_Log_warning__keys (void);
extern const char * __AMM_Sys_Log_warning__name (void);
struct _AMM_Sys_Log_warning ;
extern  c_bool __AMM_Sys_Log_warning__copyIn(c_base base, struct AMM::Sys::Log::warning *from, struct _AMM_Sys_Log_warning *to);
extern  void __AMM_Sys_Log_warning__copyOut(void *_from, void *_to);
struct _AMM_Sys_Log_warning {
    c_string message;
};

extern c_metaObject __AMM_Sys_Log_error__load (c_base base);
extern const char * __AMM_Sys_Log_error__keys (void);
extern const char * __AMM_Sys_Log_error__name (void);
struct _AMM_Sys_Log_error ;
extern  c_bool __AMM_Sys_Log_error__copyIn(c_base base, struct AMM::Sys::Log::error *from, struct _AMM_Sys_Log_error *to);
extern  void __AMM_Sys_Log_error__copyOut(void *_from, void *_to);
struct _AMM_Sys_Log_error {
    c_string message;
};

extern c_metaObject __AMM_Sys_Log_debug__load (c_base base);
extern const char * __AMM_Sys_Log_debug__keys (void);
extern const char * __AMM_Sys_Log_debug__name (void);
struct _AMM_Sys_Log_debug ;
extern  c_bool __AMM_Sys_Log_debug__copyIn(c_base base, struct AMM::Sys::Log::debug *from, struct _AMM_Sys_Log_debug *to);
extern  void __AMM_Sys_Log_debug__copyOut(void *_from, void *_to);
struct _AMM_Sys_Log_debug {
    c_string message;
};

#endif
