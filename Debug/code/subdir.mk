################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/Car_Control.c \
../code/Mahony_Icm20602.c \
../code/Menu.c \
../code/Motor_Control.c \
../code/PID.c \
../code/image.c 

OBJS += \
./code/Car_Control.o \
./code/Mahony_Icm20602.o \
./code/Menu.o \
./code/Motor_Control.o \
./code/PID.o \
./code/image.o 

COMPILED_SRCS += \
./code/Car_Control.src \
./code/Mahony_Icm20602.src \
./code/Menu.src \
./code/Motor_Control.src \
./code/PID.src \
./code/image.src 

C_DEPS += \
./code/Car_Control.d \
./code/Mahony_Icm20602.d \
./code/Menu.d \
./code/Motor_Control.d \
./code/PID.d \
./code/image.d 


# Each subdirectory must supply rules for building sources it contributes
code/%.src: ../code/%.c code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc26xb "-fD:/zhinengche/dulun/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -o "$@"  "$<"  -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

code/%.o: ./code/%.src code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


