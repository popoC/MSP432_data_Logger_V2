#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/tirtos_msp43x_2_16_01_14/packages;C:/ti/tirtos_msp43x_2_16_01_14/products/bios_6_45_02_31/packages;C:/ti/tirtos_msp43x_2_16_01_14/products/tidrivers_msp43x_2_16_01_13/packages;C:/ti/tirtos_msp43x_2_16_01_14/products/uia_2_00_05_50/packages;C:/ti/ccsv6/ccs_base;C:/Users/poa11/workspace_v6_1/MSP432_data_Logger_V1.0/.config
override XDCROOT = C:/ti/xdctools_3_32_00_06_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/tirtos_msp43x_2_16_01_14/packages;C:/ti/tirtos_msp43x_2_16_01_14/products/bios_6_45_02_31/packages;C:/ti/tirtos_msp43x_2_16_01_14/products/tidrivers_msp43x_2_16_01_13/packages;C:/ti/tirtos_msp43x_2_16_01_14/products/uia_2_00_05_50/packages;C:/ti/ccsv6/ccs_base;C:/Users/poa11/workspace_v6_1/MSP432_data_Logger_V1.0/.config;C:/ti/xdctools_3_32_00_06_core/packages;..
HOSTOS = Windows
endif
