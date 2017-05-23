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

extern c_metaObject __AMM_Physiology_Data__load (c_base base);
extern const char * __AMM_Physiology_Data__keys (void);
extern const char * __AMM_Physiology_Data__name (void);
struct _AMM_Physiology_Data ;
extern  c_bool __AMM_Physiology_Data__copyIn(c_base base, struct AMM::Physiology::Data *from, struct _AMM_Physiology_Data *to);
extern  void __AMM_Physiology_Data__copyOut(void *_from, void *_to);
struct _AMM_Physiology_Data {
    c_string node_path;
    c_string unit;
    c_double dbl;
    c_string str;
};

extern c_metaObject __AMM_Physiology_HighFrequencyData__load (c_base base);
extern const char * __AMM_Physiology_HighFrequencyData__keys (void);
extern const char * __AMM_Physiology_HighFrequencyData__name (void);
struct _AMM_Physiology_HighFrequencyData ;
extern  c_bool __AMM_Physiology_HighFrequencyData__copyIn(c_base base, struct AMM::Physiology::HighFrequencyData *from, struct _AMM_Physiology_HighFrequencyData *to);
extern  void __AMM_Physiology_HighFrequencyData__copyOut(void *_from, void *_to);
struct _AMM_Physiology_HighFrequencyData {
    c_string node_path;
    c_string unit;
    c_double dbl;
    c_string str;
};

#endif
