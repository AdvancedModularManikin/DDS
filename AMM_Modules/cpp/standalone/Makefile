
all: amm_types amm_sim_manager amm_virtual_equipment amm_physiology_manager amm_command_exec

clean depend generated realclean check-syntax $(CUSTOM_TARGETS):
	@$(MAKE) -f Makefile.amm_types $(@)
	@$(MAKE) -f Makefile.amm_sim_manager $(@)
	@$(MAKE) -f Makefile.amm_virtual_equipment $(@)
	@$(MAKE) -f Makefile.amm_lcd_heartrate $(@)
	@$(MAKE) -f Makefile.amm_command_exec $(@)
	@$(MAKE) -f Makefile.amm_physiology_manager $(@)

.PHONY: amm_types
amm_types:
	@$(MAKE) -f Makefile.amm_types all

.PHONY: amm_sim_manager
amm_sim_manager: amm_types
	@$(MAKE) -f Makefile.amm_sim_manager all

.PHONY: amm_virtual_equipment
amm_virtual_equipment: amm_types
	@$(MAKE) -f Makefile.amm_virtual_equipment all

.PHONY: amm_rest_adapter
amm_rest_adapter: amm_types
	@$(MAKE) -f Makefile.amm_rest_adapter all

.PHONY: amm_lcd_heartrate
amm_lcd_heartrate: amm_types
	@$(MAKE) -f Makefile.amm_lcd_heartrate all

.PHONY: amm_command_exec
amm_command_exec: amm_types
	@$(MAKE) -f Makefile.amm_command_exec all
		
.PHONY: amm_physiology_manager
amm_physiology_manager: amm_types
	@$(MAKE) -f Makefile.amm_physiology_manager all

project_name_list:
	@echo amm_sim_manager
	@echo amm_virtual_equipment
	@echo amm_command_exec
	@echo amm_physiology_manager
	@echo amm_types