################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
MIL_LCD_lib.obj: //fs.ece.uprm.edu/students/s402120683/My\ Documents/Micro\ 2\ files/micro2-repository-workspace/micro2-lab-experiments/custom-libraries/MIL_LCD_lib.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="X:/My Documents/Micro 2 files/micro2-repository-workspace/micro2-lab-experiments/custom-libraries" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/reyna/Documents/Code Composer/Micro2 Lab Workspace/micro2-lab-experiments/custom-libraries" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="MIL_LCD_lib.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="X:/My Documents/Micro 2 files/micro2-repository-workspace/micro2-lab-experiments/custom-libraries" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/reyna/Documents/Code Composer/Micro2 Lab Workspace/micro2-lab-experiments/custom-libraries" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tivaUtils.obj: //fs.ece.uprm.edu/students/s402120683/My\ Documents/Micro\ 2\ files/micro2-repository-workspace/micro2-lab-experiments/custom-libraries/tivaUtils.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="X:/My Documents/Micro 2 files/micro2-repository-workspace/micro2-lab-experiments/custom-libraries" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/reyna/Documents/Code Composer/Micro2 Lab Workspace/micro2-lab-experiments/custom-libraries" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="tivaUtils.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tm4c123gh6pm_startup_ccs.obj: ../tm4c123gh6pm_startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="X:/My Documents/Micro 2 files/micro2-repository-workspace/micro2-lab-experiments/custom-libraries" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/reyna/Documents/Code Composer/Micro2 Lab Workspace/micro2-lab-experiments/custom-libraries" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=full --preproc_with_compile --preproc_dependency="tm4c123gh6pm_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

