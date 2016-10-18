# invoke SourceDir generated makefile for fatsdraw.pem4f
fatsdraw.pem4f: .libraries,fatsdraw.pem4f
.libraries,fatsdraw.pem4f: package/cfg/fatsdraw_pem4f.xdl
	$(MAKE) -f D:\google\github\MSP432_data_Logger_V2/src/makefile.libs

clean::
	$(MAKE) -f D:\google\github\MSP432_data_Logger_V2/src/makefile.libs clean

