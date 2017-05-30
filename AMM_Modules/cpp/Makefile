all: amm_types amm_sim_manager amm_virtual_equipment amm_command_exec amm_physiology_manager

clean depend generated realclean check-syntax $(CUSTOM_TARGETS):
	@cd standalone && $(MAKE) -f Makefile.amm_types $(@)
	@cd standalone && $(MAKE) -f Makefile.amm_sim_manager $(@)
	@cd standalone && $(MAKE) -f Makefile.amm_virtual_equipment $(@)
	@cd standalone && $(MAKE) -f Makefile.amm_physiology_manager $(@)

.PHONY: amm_types
amm_types:
	@cd standalone && $(MAKE) -f Makefile.amm_types all

.PHONY: amm_sim_manager
amm_sim_manager: amm_types
	@cd standalone && $(MAKE) -f Makefile.amm_sim_manager all

.PHONY: amm_virtual_equipment
amm_virtual_equipment: amm_types
	@cd standalone && $(MAKE) -f Makefile.amm_virtual_equipment all
	
.PHONY: amm_command_exec
amm_command_exec: amm_types
	@cd standalone && $(MAKE) -f Makefile.amm_command_exec all	
	
.PHONY: amm_physiology_manager
 amm_physiology_manager: amm_types
	@cd standalone && $(MAKE) -f Makefile.amm_physiology_manager all

project_name_list:
	@echo amm_sim_manager
	@echo amm_virtual_equipment
	@echo amm_command_exec
	@echo amm_physiology_manager
	@echo amm_types
