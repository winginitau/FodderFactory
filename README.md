# FodderFactory
Cross platform hierarchical architecture for operation and deployment of a multi-senor, multi-actuator, multi-device, multi-hub, hardware agnostic, IoT network.

Supports generic configuration of end-to-end business applications to solve real-world problems. 

Pilot focus to direct development effort is an IoT system to operate an off-grid, radio connected fodder growing room which produces barley sprouts (fodder) for feeding to premium beef cattle.

The project is developed in partnership with the Headwaters Pastoral Company (http://headwaters.farm)

The overall system and architecture is evolving along with the control and compute components.

This README provides a description of the project and a working log of development. Documentation for re-using, build and deployment will be added when stable maturity is reached. Development and evolution is presently very dynamic. 

# Original Project Intent:

Develop an integrated control system (MCU based - currenlty prototyping on arduino) to:
 - (Done) Read multiple environment sensors (temprature, humidity, CO2, light levels)
 - (Dev) Monitor the VE.Direct enabled Victron Energy components
 - (Done) Switch via relay a range of devices - water pump, lights, water and air heaters, coolers, dehumidifiers etc
 - (Done, PID excluded for now) Provide rules and event based control logic (including PID controllers)
 - (Done) Aggregate all monitoring and event data to a datalogger for user analysis
 - (Done) Provide a basic UI via an LCD screen for viewing key data and making manual overrides and setting changes
 - (Dev) Provide a command and reporting interface via digital radio (900Mhz) to a base station for data upload and remote configuration and control

# Update 2017-10-29:

 - Branch v0.2dev running well on the factory.
 - Branch v0.3dev (and downstream) is the current dev focus.
 - Interest has been expressed in a generic model for an IoT fully connected deployment. 
 - Project will remain focssed on Fodder for now though with the architecture taking on more generic structure. 
 - Thanks heaps to:
 	- the arduino community
	- the StackOverflow community for much help in general and particularly on time/date approaches
	- jantje for sloeber arduino eclipse plugin. Use it - https://github.com/Sloeber/arduino-eclipse-plugin
	- Steve Marple for IniFile  (which I'm still working on) - https://github.com/stevemarple/IniFile

# Update 2018-02-04:

 - Tag 0.4b running on the factory
 - Completely rebuilt hardware components (properly housed components with a combination of Neutrik and Amphenol connectors)
 - Added a RaspberryPi 3B as local hub - to collect data, relay messages and commands, upload firmware to the MCU remotely)
 - Added a RaspberryPi 3B with touch screen as a remote visualisation and control station (python3 / kivvy)
 - Connected the two RPIs using an RDF900 radio modem with PPP transport (http://rfdesign.com.au)
 - Connected the control station "Controller" to the net over a sattelite ISP service with VPN to a cloud host
 - Continuing development on inter-device messaging, command and configuration grammar and services
 
# Disclaimers 
- Please excuse the kindergarten code - still revisiting C and C++ fundamentals after 25 years absence.
- Project still mostly in ANSI C. C++ is arriving slowing and painfully. 
- We shouldn't let architects get into the detail!

# Factory Operational System Components

As at Sepatember 2017:

	Highly customised older Fodder Solutions developed transportable growing room. See http://www.foddersolutions.com.au/system-specs/
		- 6x levels of growing racks each holding 6x growing trays
	Lighting
		- 240v fluro lights at one end (to green up the sprouts)
		INTENT: 
			- (Done) Stage 1: Switch lighting from the MCU (16 hrs on, 8 hrs off)
			- (Differred) Stage 2: Replace with LED lighting switchable from the MCU via realy
	Water
		- (Done) 2x watering branches (top 3 and bottom 3 racks) with misting sprinklers
		- (Replaced) Watering controlled by an old Toro watering timer and valve solenoids (2 stations in use - top and bottom)
		- (Done) 12v DC water pump
		- (Done) Water pumped from a header tank which includes a water heater via a 5 micron filtration canister and pre filter
		- (Done) Water fed via gravity from tank sources on the farm
		- (Replaced) Currently watering 12x times per day (max the toro can handle) for 1 minute per station
		INTENT: 
			- (Done) Replace toro with the MCU using relay switching
			- (Not Required) Reuse toro 24v AC supply for solenoid activation
			- (Done) Move to 25sec watering every 40 mins (per Fodder Solutions Reccommendation)
	Environment
		- (Redundant) An old caravan style air conditioner (currently inoperable)
		- (Still target) Target environ: 22 deg C, 50% RH
		INTENT:
			- (Done) Stage 1: Integrate interior and exterior temparature sensors and collect data
			- (Done and evolving) Stage 2: Integrate HVAC devices depending on need
	Energy:
		- (Done) 24v system
		- (Done) 4x Enersys 3GFM1500 batteries (1500AH @ 24v nom) 
		- (Done) 1 KW solar array. 6x 170W 36v nom. panels. (3x ~72v strings of 2x panels each in parrallel)
		- (Done) Victron BlueSolar 100/50 MPPT solar charger controller (see https://www.victronenergy.com/solar-charge-controllers/mppt-100-30)
		- (Done) Victron BMV-700 battery monitor (see https://www.victronenergy.com/battery-monitors/bmv-700)
		- (Done) Victron BP-220 battery protect isolator (see https://www.victronenergy.com/battery_protect/battery-protect)
		- (Done) Victron Orion 24/12-70 DC-DC converter (see https://www.victronenergy.com/upload/documents/Datasheet-Orion-DC-DC-converters-high-power,-non-isolated-EN.pdf)
		- (Problematic - replaced with alternate) Xantrex ProSine 1000i 12v inverter (feeding lights and toro currently) 
		INTENT:
			- (Dev) Integrate the BMV and MPPT to the MCU via VE.Direct protocol for integrated monitoring and reporting
			
			
		
	
	
  
