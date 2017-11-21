
# AMM Modules using DDS

This is a series of proof-of-concept AMM modules utilizing DDS.  These can be used as a reference to build your own AMM-compliant modules.

#### Requirements:
* [FastRTPS](https://github.com/eProsima/Fast-RTPS) built and installed. 
   * We depend on fastrtpsgen so be sure to enable Java with -DBUILD_JAVA=ON. Gradle and the JDK must already be installed on your system.
   
   ```
   git clone https://github.com/eProsima/Fast-RTPS
   cd Fast-RTPS
   mkdir build
   cd build
   cmake -DTHIRDPARTY=ON -DBUILD_JAVA=ON .. 
   make
   make install
   ```

    
* [BioGears](https://github.com/BioGearsEngine/Engine) or [Pulse](https://gitlab.kitware.com/physiology/engine) Physiology Engine SDK built and installed. 

* [libboost](http://www.boost.org) installed. On debian : sudo apt install libboost-all-dev 

#### Optional:
* [RapidJSON](https://github.com/miloyip/rapidjson) - JSON parser/generator for C++ (required for REST adapter) 
* [Pistache](https://github.com/oktal/pistache) - Simple C++ REST framework (required for REST adapter)

### Build Instructions

```
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
```

This will build into the `bin` directory.  You will have a few binaries:

* amm_sim_manager - outputs ticks at 50hz, start/pause/stop simulation
* amm_physiology_manager - interface for physiology engine, publishes some node_paths
* amm_virtual_equipment - command line tool, accepts a node_path to listen for
* amm_command_executor - command line tool, execute commands and interventions
* amm_heartrate_led - [BLT](https://github.com/AdvancedModularManikin/development-kit/wiki/AMMDK-Overview) test to flash LED at heart rate
* amm_rest_adapter - Accept HTTP requests and return data from the DDS bus
* amm_tcp_bridge - Connector from the DDS bus to TCP, including a UDP auto-discovery server.  Used for Unity3D connectivity.


### REST Adapter routes
The REST adapter exposes the following routes:
```
/nodes            - retrieve the current state of all node paths
/node/<name>      - retrieve a single node_path
/command/<action> - issue a command
```

#### Examples: 
```
http://localhost:9080/node/Cardiovascular_HeartRate
http://localhost:9080/command/LEG_HEMORRHAGE
```
