

# AMM Modules using DDS

This is a series of proof-of-concept AMM modules utilizing DDS.

Requirements:
* PrismTech OpenSplice DDS built and installed.  Suggested location: /usr/local/OpenSplice
* BioGears SDK built and installed.  Suggested location: /usr/local/BioGears

run `make` at the root of AMM_Modules to build three executables in the bin dir:

* amm_virtual_equipment - command line tool, accepts a node_path to listen for
* amm_physiology_manager - BioGears module, publishes some node_paths
* amm_sim_manager - outputs ticks at 50hz


