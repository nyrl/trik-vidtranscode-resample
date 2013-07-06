
# your various installation directories
DEPOT_DSP?=/opt/trik-dsp

XDC_INSTALL_DIR?=$(DEPOT_DSP)/xdctools_3_24_07_73
CE_INSTALL_DIR?=$(DEPOT_DSP)/codec_engine_3_23_00_07
XDAIS_INSTALL_DIR?=$(DEPOT_DSP)/xdais_7_23_00_06
CODEGEN_INSTALL_DIR?=$(DEPOT_DSP)/cgt_c6000_7.4.2

#uncomment this for verbose builds
XDCOPTIONS=v

CGTOOLS_C64P?=
CGTOOLS_C674?=$(CODEGEN_INSTALL_DIR)
XDCARGS=CGTOOLS_C64P=\"$(CGTOOLS_C64P)\"  CGTOOLS_C674=\"$(CGTOOLS_C674)\"

XDCPATH=$(CE_INSTALL_DIR)/packages;$(XDAIS_INSTALL_DIR)/packages

all:
	"$(XDC_INSTALL_DIR)/xdc" XDCOPTIONS=$(XDCOPTIONS) XDCARGS="$(XDCARGS)" --xdcpath="$(XDCPATH)" release

clean:
	"$(XDC_INSTALL_DIR)/xdc" clean

