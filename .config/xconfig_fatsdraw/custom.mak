## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em4f linker.cmd package/cfg/fatsdraw_pem4f.oem4f

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/fatsdraw_pem4f.xdl
	$(SED) 's"^\"\(package/cfg/fatsdraw_pem4fcfg.cmd\)\"$""\"C:/Users/poa11/workspace_v6_1/MSP432_data_Logger_V1.0/.config/xconfig_fatsdraw/\1\""' package/cfg/fatsdraw_pem4f.xdl > $@
	-$(SETDATE) -r:max package/cfg/fatsdraw_pem4f.h compiler.opt compiler.opt.defs
