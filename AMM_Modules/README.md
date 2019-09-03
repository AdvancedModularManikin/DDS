# DEPRECATED

This repository is the pre-AMM 1.0 standard repo for AMM core modules.  New libraries will be released soon which supercede this version and include improved building and development processes.  This will be updated when the new repositories are ready.


# AMM Modules using DDS

This is a series of proof-of-concept AMM modules utilizing DDS.  These can be used as a reference to build your own AMM-compliant modules.

#### Requirements:
* [FastRTPS](https://github.com/eProsima/Fast-RTPS) built and installed, version 1.7.x or higher.  Versions newer than 1.9 require additional dependancies which must be installed separately.  FastCDR versions should match the version of FastRTPS you choose: please see the FastRTPS repo for instructions.
   
   ```
   git clone https://github.com/eProsima/Fast-RTPS
   cd Fast-RTPS
   mkdir build
   cd build
   cmake -DTHIRDPARTY=ON -DBUILD_JAVA=OFF .. 
   make
   make install
   ```

    
* [BioGears](https://github.com/BioGearsEngine/Engine) or [Pulse](https://gitlab.kitware.com/physiology/engine) Physiology Engine SDK built and installed. 
* [libboost](http://www.boost.org) 1.5.1 or higher installed. On Debian : sudo apt install libboost-all-dev 
* [libtinyxml2](http://www.grinninglizard.com/tinyxml2/) installed.  On Debian: sudo apt install libtinyxml2-dev

#### Optional:
* [RapidJSON](https://github.com/miloyip/rapidjson) - JSON parser/generator for C++ (required for REST adapter) 
* [Pistache](https://github.com/oktal/pistache) - Simple C++ REST framework (required for REST adapter)
* [sqlite](https://www.sqlite.org/index.html) - Persistent, light-weight storage.  On Debian: sudo apt install sqlite3 libsqlite3-dev sqlite libsqlite-dev 

### Build Instructions

```
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
```

This will build into the `bin` directory.  You will have a few binaries:

**CORE Software**:
* amm_module_manager - Maintains track of all modules and their capabilities
* amm_sim_manager - outputs ticks at 50hz, start/pause/stop simulation
* amm_physiology_manager - interface for physiology engine, publishes some node_paths
* amm_rest_adapter - Accept HTTP requests and return data from the DDS bus
* amm_tcp_bridge - Connector from the DDS bus to TCP, including a UDP auto-discovery server.  Used for Unity3D connectivity.

**Example modules**:
* amm_virtual_equipment - command line tool, accepts a node_path to listen for
* amm_command_executor - command line tool, execute commands and interventions

**Example hardware modules**:
* amm_heartrate_led - [BLT](https://github.com/AdvancedModularManikin/development-kit/wiki/AMMDK-Overview) test to flash LED at heart rate
* amm_ivc_module
* amm_fluid_manager
* amm_arduino_bridge - Connector to devices (Arduino, etc) that communicate via serial / USB. 

### REST Adapter routes
The REST adapter exposes the following routes:
```
/nodes            - retrieve the current state of all node paths
/node/<name>      - retrieve a single node_path
/command/<action> - issue a command
/actions	  - retrieve a list of all available actions
/states		  - retrieve a list of all available starting states / scenarios
/patients	  - retrieve a list of all available patients
/modules      - retrieve a list of all connected modules and their statuses/capabilities
/module/<id>  - retrieve a single module's status, configuration and capabilities
```

#### Examples: 
```
http://localhost:9080/node/Cardiovascular_HeartRate
http://localhost:9080/command/LEG_HEMORRHAGE
```
