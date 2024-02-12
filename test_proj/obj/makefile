################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
test_proj.hex \

SECONDARY_LIST += \
test_proj.lst \

SECONDARY_SIZE += \
test_proj.siz \


# All Target
all: test_proj.elf secondary-outputs

# Tool invocations
test_proj.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T "C:\development\CH32V_projects\test_proj\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"test_proj.map" --specs=nano.specs --specs=nosys.specs -o "test_proj.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lprintf
	@	@
test_proj.hex: test_proj.elf
	@	riscv-none-embed-objcopy -O ihex "test_proj.elf"  "test_proj.hex"
	@	@
test_proj.lst: test_proj.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "test_proj.elf" > "test_proj.lst"
	@	@
test_proj.siz: test_proj.elf
	@	riscv-none-embed-size --format=berkeley "test_proj.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) test_proj.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
