################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GEMInfor.cpp \
../src/GEMInputHandler.cpp \
../src/GEMRawFileDecoder.cpp \
../src/GEM_Analysis.cpp 

OBJS += \
./src/GEMInfor.o \
./src/GEMInputHandler.o \
./src/GEMRawFileDecoder.o \
./src/GEM_Analysis.o 

CPP_DEPS += \
./src/GEMInfor.d \
./src/GEMInputHandler.d \
./src/GEMRawFileDecoder.d \
./src/GEM_Analysis.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/newdriver/Source/ROOT_CERN/root_v5.34.36/include" -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


