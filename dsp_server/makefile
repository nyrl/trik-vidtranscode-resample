
# your various installation directories
DEPOT?=/home/nyrl/trik/ti-ce3/
XDC_INSTALL_DIR?=$(DEPOT)/xdctools_3_25_00_48
CE_INSTALL_DIR?=$(DEPOT)/codec_engine_3_23_00_07
BIOS_INSTALL_DIR?=$(DEPOT)/bios_6_35_01_29
FC_INSTALL_DIR?=$(DEPOT)/framework_components_3_23_03_17
XDAIS_INSTALL_DIR?=$(DEPOT)/xdais_7_23_00_06
IPC_INSTALL_DIR?=$(DEPOT)/ipc_1_25_02_12
LINK_INSTALL_DIR?=$(DEPOT)/syslink_2_21_01_05
OSAL_INSTALL_DIR?=$(DEPOT)/osal_1_23_00_04
CMEM_INSTALL_DIR?=$(DEPOT)/linuxutils_3_23_00_01
CODEGEN_INSTALL_DIR?=$(DEPOT)/cgt6x_7_4_2

EXTRA_XDCPACKAGES_DIR?=$(PWD)/..

#uncomment this for verbose builds
#XDCOPTIONS=v

# If we're a subcomponent, allow larger product settings to override our
# defaults.  The variable $(RULES_MAKE) can be overridden on the 'make'
# command-line if the location below is not correct (for example, if you
# want to use a Rules.make from some other location.
RULES_MAKE := $(CE_INSTALL_DIR)/../Rules.make
ifneq ($(wildcard $(RULES_MAKE)),)
include $(RULES_MAKE)
endif

XDCARGS=CODEGEN_INSTALL_DIR=\"$(CODEGEN_INSTALL_DIR)\"

# Construct XDCPATH from the paths above - this is the path containing
# dependencies required for this Server
XDCPATH=$(XDC_INSTALL_DIR)/packages;$(CE_INSTALL_DIR)/packages;$(BIOS_INSTALL_DIR)/packages;$(LINK_INSTALL_DIR)/packages;$(IPC_INSTALL_DIR)/packages;$(XDAIS_INSTALL_DIR)/packages;$(FC_INSTALL_DIR)/packages;$(OSAL_INSTALL_DIR)/packages;$(CMEM_INSTALL_DIR)/packages;$(EXTRA_XDCPACKAGES_DIR)

TRACE_LEVEL=
TRACE_GROUPS=

# To enable RTSC trace support for Genserver builds, uncomment the two
# lines following this description. More detailed tracing can be enabled.
# See http://rtsc.eclipse.org/docs-tip/Trace_Support for full details.
#TRACE_LEVEL=1
#TRACE_GROUPS=genserver

all:
	"$(XDC_INSTALL_DIR)/xdc" XDC_TRACE_LEVEL=$(TRACE_LEVEL) XDC_TRACE_GROUPS=$(TRACE_GROUPS) XDCOPTIONS=$(XDCOPTIONS) XDCARGS="$(XDCARGS)" --xdcpath="$(XDCPATH)" release

clean:
	"$(XDC_INSTALL_DIR)/xdc" XDCOPTIONS=$(XDCOPTIONS) clean


