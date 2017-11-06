# FodderFactory
Cross platform IoT MCU system to operate an off-grid cattle fodder (barley sprouts) growing room.

The overall system is evolving along with the control components.

Project intent: Develop an integrated control system (MCU based - currenlty prototyping on arduino) to:
 - Read multiple environment sensors (temprature, humidity, CO2, light levels)
 - Monitor the VE.Direct enabled Victron Energy components
 - Switch via relay a range of devices - water pump, lights, water and air heaters, coolers, dehumidifiers etc
 - Provide rules and event based control logic (including PID controllers)
 - Aggregate all monitoring and event data to a datalogger for user analysis
 - Provide a basic UI via an LCD screen for viewing key data and making manual overrides and setting changes
 - Provide a command and reporting interface via digital radio (900Mhz) to a base station for data upload and remote configuration and control

# Update 2017-10-29:

 - Branch v0.2dev running well on the factory.
 - Branch v0.3dev (and downstream) is the current dev focus.
 - Interest has been expressed in a generic model for an IoT fully connected deploayment. 
 - Project will remain focssed on Fodder for now though with the architecture taking on more generic structure. 
 - Thanks heaps to:
 	- the arduino community
	- jantje for sloeber arduino eclipse plugin. Use it - https://github.com/Sloeber/arduino-eclipse-plugin
	- Steve Marple for IniFile  (which I'm still working on) - https://github.com/stevemarple/IniFile
	
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
			- Stage 1: Switch lighting from the MCU (16 hrs on, 8 hrs off)
			- Stage 2: Replace with LED lighting switchable from the MCU via realy
	Water
		- 2x watering branches (top 3 and bottom 3 racks) with misting sprinklers
		- Watering controlled by an old Toro watering timer and valve solenoids (2 stations in use - top and bottom)
		- 12v DC water pump
		- Water pumped from a header tank which includes a water heater via a 5 micron filtration canister and pre filter
		- Water fed via gravity from tank sources on the farm
		- Currently watering 12x times per day (max the toro can handle) for 1 minute per station
		INTENT: 
			- Replace toro with the MCU using relay switching
			- Reuse toro 24v AC supply for solenoid activation
			- Move to 25sec watering every 40 mins (per Fodder Solutions Reccommendation)
	Environment
		- An old caravan style air conditioner (currently inoperable)
		- Target environ: 22 deg C, 50% RH
		INTENT:
			- Stage 1: Integrate interior and exterior temparature sensors and collect data
			- Stage 2: Integrate HVAC devices depending on need
	Energy:
		- 24v system
		- 4x Enersys 3GFM1500 batteries (1500AH @ 24v nom) 
		- 1 KW solar array. 6x 170W 36v nom. panels. (3x ~72v strings of 2x panels each in parrallel)
		- Victron BlueSolar 100/50 MPPT solar charger controller (see https://www.victronenergy.com/solar-charge-controllers/mppt-100-30)
		- Victron BMV-700 battery monitor (see https://www.victronenergy.com/battery-monitors/bmv-700)
		- Victron BP-220 battery protect isolator (see https://www.victronenergy.com/battery_protect/battery-protect)
		- Victron Orion 24/12-70 DC-DC converter (see https://www.victronenergy.com/upload/documents/Datasheet-Orion-DC-DC-converters-high-power,-non-isolated-EN.pdf)
		- Xantrex ProSine 1000i 12v inverter (feeding lights and toro currently) 
		INTENT:
			- Integrate the BMV and MPPT to the MCU via VE.Direct protocol for integrated monitoring and reporting
			
			
		
	
	
  
