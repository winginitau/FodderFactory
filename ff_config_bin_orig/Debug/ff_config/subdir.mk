################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/brendan/git/FodderFactory/ff_config/ff_config.cpp 

OBJS += \
./ff_config/ff_config.o 

CPP_DEPS += \
./ff_config/ff_config.d 


# Each subdirectory must supply rules for building sources it contributes
ff_config/ff_config.o: /home/brendan/git/FodderFactory/ff_config/ff_config.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/brendan/git/FodderFactory/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


