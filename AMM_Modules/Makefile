# -*-Makefile-*-
#----------------------------------------------------------------------------
#       Macros
#----------------------------------------------------------------------------
CP            = cp -p
BTARGETDIR    = bin/

#----------------------------------------------------------------------------
#       Local targets
#----------------------------------------------------------------------------
all: amm_types amm_sim_manager amm_virtual_equipment amm_physiology_manager deploy

clean depend generated realclean check-syntax $(CUSTOM_TARGETS):
	@cd cpp/standalone && $(MAKE) -f Makefile.amm_types $(@)
	@cd cpp/standalone && $(MAKE) -f Makefile.amm_sim_manager $(@)
	@cd cpp/standalone && $(MAKE) -f Makefile.amm_virtual_equipment $(@)
	@cd cpp/standalone && $(MAKE) -f Makefile.amm_physiology_manager $(@)

.PHONY: amm_types
amm_types:
	@cd cpp/standalone && $(MAKE) -f Makefile.amm_types all

.PHONY: amm_sim_manager
amm_sim_manager: amm_types
	@cd cpp/standalone && $(MAKE) -f Makefile.amm_sim_manager all
	
.PHONY: amm_virtual_equipment
amm_virtual_equipment: amm_types
	@cd cpp/standalone && $(MAKE) -f Makefile.amm_virtual_equipment all

.PHONY: amm_physiology_manager
amm_physiology_manager: amm_types
	@cd cpp/standalone && $(MAKE) -f Makefile.amm_physiology_manager all

project_name_list:
	@echo amm_sim_manager
	@echo amm_virtual_equipment
	@echo amm_physiology_manager
	@echo amm_types

deploy:
	-$(CP) -r ospl.xml $(BTARGETDIR)
	