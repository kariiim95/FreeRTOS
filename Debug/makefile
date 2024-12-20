################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./draft.obj" \
"./main.obj" \
"./tm4c123gh6pm_startup_ccs.obj" \
"./MCAL/ADC/adc.obj" \
"./MCAL/BUTTONS/buttons.obj" \
"./MCAL/GPIO/gpio.obj" \
"./MCAL/GPTM/GPTM.obj" \
"./MCAL/LEDS/leds.obj" \
"./MCAL/Temperatrue Sensor/lm35.obj" \
"./MCAL/UART/uart0.obj" \
"./Source/event_groups.obj" \
"./Source/list.obj" \
"./Source/queue.obj" \
"./Source/tasks.obj" \
"./Source/timers.obj" \
"./Source/portable/CCS/ARM_CM4F/port.obj" \
"./Source/portable/CCS/ARM_CM4F/portasm.obj" \
"./Source/portable/MemMang/heap_2.obj" \
"../tm4c123gh6pm.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include MCAL/ADC/subdir_vars.mk
-include MCAL/BUTTONS/subdir_vars.mk
-include MCAL/GPIO/subdir_vars.mk
-include MCAL/GPTM/subdir_vars.mk
-include MCAL/LEDS/subdir_vars.mk
-include MCAL/Temperatrue\ Sensor/subdir_vars.mk
-include MCAL/UART/subdir_vars.mk
-include Source/subdir_vars.mk
-include Source/portable/CCS/ARM_CM4F/subdir_vars.mk
-include Source/portable/MemMang/subdir_vars.mk
-include subdir_rules.mk
-include MCAL/ADC/subdir_rules.mk
-include MCAL/BUTTONS/subdir_rules.mk
-include MCAL/GPIO/subdir_rules.mk
-include MCAL/GPTM/subdir_rules.mk
-include MCAL/LEDS/subdir_rules.mk
-include MCAL/Temperatrue\ Sensor/subdir_rules.mk
-include MCAL/UART/subdir_rules.mk
-include Source/subdir_rules.mk
-include Source/portable/CCS/ARM_CM4F/subdir_rules.mk
-include Source/portable/MemMang/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Lab1.out 

EXE_OUTPUTS__QUOTED += \
"Lab1.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "Lab1.out"

# Tool invocations
Lab1.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi -z -m"Lab1.map" --heap_size=0 --stack_size=512 -i"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/lib" -i"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="Lab1_linkInfo.xml" --rom_model -o "Lab1.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "draft.obj" "main.obj" "tm4c123gh6pm_startup_ccs.obj" "MCAL\ADC\adc.obj" "MCAL\BUTTONS\buttons.obj" "MCAL\GPIO\gpio.obj" "MCAL\GPTM\GPTM.obj" "MCAL\LEDS\leds.obj" "MCAL\Temperatrue Sensor\lm35.obj" "MCAL\UART\uart0.obj" "Source\event_groups.obj" "Source\list.obj" "Source\queue.obj" "Source\tasks.obj" "Source\timers.obj" "Source\portable\CCS\ARM_CM4F\port.obj" "Source\portable\CCS\ARM_CM4F\portasm.obj" "Source\portable\MemMang\heap_2.obj" 
	-$(RM) "draft.d" "main.d" "tm4c123gh6pm_startup_ccs.d" "MCAL\ADC\adc.d" "MCAL\BUTTONS\buttons.d" "MCAL\GPIO\gpio.d" "MCAL\GPTM\GPTM.d" "MCAL\LEDS\leds.d" "MCAL\Temperatrue Sensor\lm35.d" "MCAL\UART\uart0.d" "Source\event_groups.d" "Source\list.d" "Source\queue.d" "Source\tasks.d" "Source\timers.d" "Source\portable\CCS\ARM_CM4F\port.d" "Source\portable\MemMang\heap_2.d" 
	-$(RM) "Source\portable\CCS\ARM_CM4F\portasm.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

