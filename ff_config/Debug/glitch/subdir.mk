################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/brendan/git/FodderFactory/glitch/OutputBuffer.cpp \
/home/brendan/git/FodderFactory/glitch/StringList.cpp \
/home/brendan/git/FodderFactory/glitch/out.cpp \
/home/brendan/git/FodderFactory/glitch/out_user_code.cpp 

OBJS += \
./glitch/OutputBuffer.o \
./glitch/StringList.o \
./glitch/out.o \
./glitch/out_user_code.o 

CPP_DEPS += \
./glitch/OutputBuffer.d \
./glitch/StringList.d \
./glitch/out.d \
./glitch/out_user_code.d 


# Each subdirectory must supply rules for building sources it contributes
glitch/OutputBuffer.o: /home/brendan/git/FodderFactory/glitch/OutputBuffer.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/brendan/git/FodderFactory/src" -I"/home/brendan/git/FodderFactory/glitch" -I"/home/brendan/git/FodderFactory/itch" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

glitch/StringList.o: /home/brendan/git/FodderFactory/glitch/StringList.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/brendan/git/FodderFactory/src" -I"/home/brendan/git/FodderFactory/glitch" -I"/home/brendan/git/FodderFactory/itch" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

glitch/out.o: /home/brendan/git/FodderFactory/glitch/out.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/brendan/git/FodderFactory/src" -I"/home/brendan/git/FodderFactory/glitch" -I"/home/brendan/git/FodderFactory/itch" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

glitch/out_user_code.o: /home/brendan/git/FodderFactory/glitch/out_user_code.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/brendan/git/FodderFactory/src" -I"/home/brendan/git/FodderFactory/glitch" -I"/home/brendan/git/FodderFactory/itch" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


