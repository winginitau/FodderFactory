################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../DallasExamples.ino 

CPP_SRCS += \
../sloeber.ino.cpp 

LINK_OBJ += \
./sloeber.ino.cpp.o 

INO_DEPS += \
./DallasExamples.ino.d 

CPP_DEPS += \
./sloeber.ino.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
DallasExamples.o: ../DallasExamples.ino
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/home/brendan/eclipse/cpp-oxygen/eclipse//arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -flto -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10802 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/home/brendan/eclipse/cpp-oxygen/eclipse/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/cores/arduino" -I"/home/brendan/eclipse/cpp-oxygen/eclipse/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/variants/mega" -I"/home/brendan/Arduino/libraries/DallasTemperature" -I"/home/brendan/Arduino/libraries/OneWire" -I"/home/brendan/eclipse/cpp-oxygen/eclipse/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/libraries/Wire" -I"/home/brendan/eclipse/cpp-oxygen/eclipse/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/libraries/Wire/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '

sloeber.ino.cpp.o: ../sloeber.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/home/brendan/eclipse/cpp-oxygen/eclipse//arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -flto -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10802 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/home/brendan/eclipse/cpp-oxygen/eclipse/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/cores/arduino" -I"/home/brendan/eclipse/cpp-oxygen/eclipse/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/variants/mega" -I"/home/brendan/Arduino/libraries/DallasTemperature" -I"/home/brendan/Arduino/libraries/OneWire" -I"/home/brendan/eclipse/cpp-oxygen/eclipse/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/libraries/Wire" -I"/home/brendan/eclipse/cpp-oxygen/eclipse/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/libraries/Wire/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


