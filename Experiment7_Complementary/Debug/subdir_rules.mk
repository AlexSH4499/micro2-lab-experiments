################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
MIL_LCD_lib.obj: C:/Users/reyna/Documents/Code\ Composer/Micro2\ Lab\ Workspace/micro2-lab-experiments/custom-libraries/MIL_LCD_lib.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv4 --code_state=32 --abi=ti_arm9_abi --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/reyna/Documents/Code Composer/Micro2 Lab Workspace/r-tiles-dev/custom-libraries" -g --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="MIL_LCD_lib.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv4 --code_state=32 --abi=ti_arm9_abi --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/reyna/Documents/Code Composer/Micro2 Lab Workspace/r-tiles-dev/custom-libraries" -g --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tivaUtils.obj: C:/Users/reyna/Documents/Code\ Composer/Micro2\ Lab\ Workspace/micro2-lab-experiments/custom-libraries/tivaUtils.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv4 --code_state=32 --abi=ti_arm9_abi --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/reyna/Documents/Code Composer/Micro2 Lab Workspace/r-tiles-dev/custom-libraries" -g --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tivaUtils.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


