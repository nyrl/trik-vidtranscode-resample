
# your various installation directories
DEPOT?=/home/nyrl/trik/ti-ce3/
XDC_INSTALL_DIR?=$(DEPOT)/xdctools_3_25_00_48
CE_INSTALL_DIR?=$(DEPOT)/codec_engine_3_23_00_07
XDAIS_INSTALL_DIR?=$(DEPOT)/xdais_7_23_00_06
CODEGEN_INSTALL_DIR?=$(DEPOT)/cgt6x_7_4_2

TRIK_LIBIMAGE?=$(PWD)/libimage
TRIK_INCPATH=$(PWD)/include;$(TRIK_LIBIMAGE)/include

#uncomment this for verbose builds
#XDCOPTIONS=v

CGTOOLS_C64P?=$(CODEGEN_INSTALL_DIR)
CGTOOLS_C674?=$(CODEGEN_INSTALL_DIR)
XDCARGS=CGTOOLS_C64P=\"$(CGTOOLS_C64P)\"  CGTOOLS_C674=\"$(CGTOOLS_C674)\"

XDCPATH=$(CE_INSTALL_DIR)/packages;$(XDAIS_INSTALL_DIR)/packages;$(TRIK_INCPATH)

all:
	"$(XDC_INSTALL_DIR)/xdc" XDCOPTIONS=$(XDCOPTIONS) XDCARGS="$(XDCARGS)" --xdcpath="$(XDCPATH)" release

clean:
	"$(XDC_INSTALL_DIR)/xdc" clean

