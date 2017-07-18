

# AMM Modules using DDS

This is a series of proof-of-concept AMM modules utilizing DDS.

Requirements:
* PrismTech OpenSplice DDS built and installed.  Suggested location: /usr/local/opensplice
* BioGears SDK built and installed.  Suggested location: /usr/local/physiology


```
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
```

This will build into the `bin` directory.  You will have a few binaries:

* amm_sim_manager - outputs ticks at 50hz
* amm_physiology_manager - BioGears module, publishes some node_paths
* amm_virtual_equipment - command line tool, accepts a node_path to listen for
* amm_command_executor - execute commands and interventions
* amm_rest_adapater - Accept HTTP requests and return data from the DDS bus


