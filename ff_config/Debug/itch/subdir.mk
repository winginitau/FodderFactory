################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/brendan/git/FodderFactory/itch/NodeMap.cpp \
/home/brendan/git/FodderFactory/itch/Parser.cpp \
/home/brendan/git/FodderFactory/itch/TokenList.cpp \
/home/brendan/git/FodderFactory/itch/itch.cpp \
/home/brendan/git/FodderFactory/itch/itch_hal.cpp 

OBJS += \
./itch/NodeMap.o \
./itch/Parser.o \
./itch/TokenList.o \
./itch/itch.o \
./itch/itch_hal.o 

CPP_DEPS += \
./itch/NodeMap.d \
./itch/Parser.d \
./itch/TokenList.d \
./itch/itch.d \
./itch/itch_hal.d 


# Each subdirectory must supply rules for building sources it contributes
itch/NodeMap.o: /home/brendan/git/FodderFactory/itch/NodeMap.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/brendan/git/FodderFactory/src" -I"/home/brendan/git/FodderFactory/glitch" -I"/home/brendan/git/FodderFactory/itch" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

itch/Parser.o: /home/brendan/git/FodderFactory/itch/Parser.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/brendan/git/FodderFactory/src" -I"/home/brendan/git/FodderFactory/glitch" -I"/home/brendan/git/FodderFactory/itch" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

itch/TokenList.o: /home/brendan/git/FodderFactory/itch/TokenList.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/brendan/git/FodderFactory/src" -I"/home/brendan/git/FodderFactory/glitch" -I"/home/brendan/git/FodderFactory/itch" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

itch/itch.o: /home/brendan/git/FodderFactory/itch/itch.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/brendan/git/FodderFactory/src" -I"/home/brendan/git/FodderFactory/glitch" -I"/home/brendan/git/FodderFactory/itch" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

itch/itch_hal.o: /home/brendan/git/FodderFactory/itch/itch_hal.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/brendan/git/FodderFactory/src" -I"/home/brendan/git/FodderFactory/glitch" -I"/home/brendan/git/FodderFactory/itch" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


