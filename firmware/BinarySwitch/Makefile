#****************************************************************************
#
# Copyright (c) 2001-2012
# Sigma Designs, Inc.
# All Rights Reserved
#
#---------------------------------------------------------------------------
#
# Description:
#    Makefile for the ZW0x0x development kit LED Dimmer
#    This makefile defines the targets built
#
#    To make a bootable program, you must first make a booloader specific for
#    your application:
#    > make leddimmer_ZW030x_EU BOOTLOADER=1
#    This will build the "bootloader_enhanced_ZW030x_EU.hex"
#
#    Then you can build your application prepared for booting:
#    > make leddimmer_ZW030x_EU BOOTLOADER_ENABLED=1
#    This will build the "leddimmer_enhanced_ZW030x_EU.hex", which includes
#    a firmware descriptor needed by the bootloader.
#    This specially built application relies heavily on the bootloader
#    prepared in advance, because it reuses some software modules in the bootloader
#    to reduce code memory consumption.
#
# Author:   Henrik Holm
#
# Last Changed By:  $Author: efh $
# Revision:         $Revision: 24605 $
# Last Changed:     $Date: 2013-02-26 15:25:18 +0100 (ti, 26 feb 2013) $
#
#***************************************************************************

# Find the current directory
# used as a base for all other paths.
BASEDIR=$(shell cd)

# The command to make dependencies from the headerfiles
# included by a file.
# INCDEP=python $(subst \,\\,$(TOOLSDIR)\IncDep\incdep.py)

# Find the product name from the subdirectoryname
BASEDIRWORDLIST=$(subst \, ,$(BASEDIR))
BASEDIRLASTWORDINDEX=$(words $(BASEDIRWORDLIST))
PRODUCTDIR=$(word $(BASEDIRLASTWORDINDEX),$(BASEDIRWORDLIST))

# Find the Z-Wave library location (if not forced by setting the environment variable ZWLIBROOT)
ifndef ZWLIBROOT
ZWLIBROOT:=$(subst \$(PRODUCTDIR),,$(subst \Product,\Z-Wave,$(BASEDIR)))
endif

# Library location when making release of application
ifdef SVNLIBRELEASEDIRBIN
ZWLIBROOT:=$(SVNLIBRELEASEDIRBIN)
endif

ifneq ($(BOOTLOADER),)
BOOTLOADER_SPEC:= BOOTLOADER=1
export BOOTLOADER_SPEC
endif

ifneq ($(BOOTLOADER_ENABLED),)
BOOTLOADER_ENABLED_SPEC:= BOOTLOADER_ENABLED=1
export BOOTLOADER_ENABLED_SPEC
endif

# Export the variables declared above to the other makefiles.
export BASEDIR ZWLIBROOT

BDIRS=build

default: ZW0x0x

all: default

# Standard make names
ZW020x_EU:  leddimmer_ZW020x_EU
ZW020x_US:  leddimmer_ZW020x_US
ZW020x_ANZ: leddimmer_ZW020x_ANZ
ZW020x_HK:  leddimmer_ZW020x_HK
ZW020x_MY:  leddimmer_ZW020x_MY
ZW020x_IN:  leddimmer_ZW020x_IN
ZW020x_RU:  leddimmer_ZW020x_RU
ZW020x_IL:  leddimmer_ZW020x_IL
ZW030x_EU:  leddimmer_ZW030x_EU
ZW030x_US:  leddimmer_ZW030x_US
ZW030x_ANZ: leddimmer_ZW030x_ANZ
ZW030x_HK:  leddimmer_ZW030x_HK
ZW030x_MY:  leddimmer_ZW030x_MY
ZW030x_IN:  leddimmer_ZW030x_IN
ZW030x_RU:  leddimmer_ZW030x_RU
ZW030x_IL:  leddimmer_ZW030x_IL

ZW0x0x_NOSEC:    leddimmer_ZW020x leddimmer_ZW030x leddimmer_OTA_ZW030x

ZW0x0x: ZW0x0x_NOSEC

ZW020x: leddimmer_ZW020x
leddimmer_ZW020x:\
 leddimmer_ZW020x_EU\
 leddimmer_ZW020x_US\
 leddimmer_ZW020x_ANZ\
 leddimmer_ZW020x_HK\
 leddimmer_ZW020x_MY\
 leddimmer_ZW020x_IN\
 leddimmer_ZW020x_RU\
 leddimmer_ZW020x_IL

ZW030x: leddimmer_ZW030x

leddimmer_ZW030x:\
 leddimmer_ZW030x_EU\
 leddimmer_ZW030x_US\
 leddimmer_ZW030x_ANZ\
 leddimmer_ZW030x_HK\
 leddimmer_ZW030x_MY\
 leddimmer_ZW030x_IN\
 leddimmer_ZW030x_RU\
 leddimmer_ZW030x_IL

bootloader_ZW030x:\
 bootloader_ZW030x_EU\
 bootloader_ZW030x_US\
 bootloader_ZW030x_ANZ\
 bootloader_ZW030x_HK\
 bootloader_ZW030x_MY\
 bootloader_ZW030x_IN\
 bootloader_ZW030x_RU\
 bootloader_ZW030x_IL

leddimmer_OTA_ZW030x:\
 leddimmer_OTA_ZW030x_EU\
 leddimmer_OTA_ZW030x_US\
 leddimmer_OTA_ZW030x_ANZ\
 leddimmer_OTA_ZW030x_HK\
 leddimmer_OTA_ZW030x_MY\
 leddimmer_OTA_ZW030x_IN\
 leddimmer_OTA_ZW030x_RU\
 leddimmer_OTA_ZW030x_IL

##***************************************************************************/
# Include secure targets (if any)
-include Makefile.SecureTargets

##***************************************************************************/
# Include test targets (if any)
-include Makefile.TestTargets
-include Makefile.Testdefines

dirs: $(BDIRS)

MAKEOPTS=--no-print-directory -C

# The rules below this point are used to build the downloadables
# by invoking specific makefiles. They all got "depend" as a
# prerequisite to make sure that the headerfile dependencies are
# up to date before starting the build.

# Makes the dependency lines from included headerfiles.

## ZW020x target
leddimmer_ZW020x_EU:MAKELINE=CHIP=ZW020x  COUNTRY=EU RF=EU
leddimmer_ZW020x_EU:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW020x_US:MAKELINE=CHIP=ZW020x  COUNTRY=US RF=US
leddimmer_ZW020x_US:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW020x_HK:MAKELINE=CHIP=ZW020x  COUNTRY=HK RF=HK
leddimmer_ZW020x_HK:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW020x_ANZ:MAKELINE=CHIP=ZW020x  COUNTRY=ANZ RF=ANZ
leddimmer_ZW020x_ANZ:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW020x_MY:MAKELINE=CHIP=ZW020x  COUNTRY=MY RF=MY
leddimmer_ZW020x_MY:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW020x_IN:MAKELINE=CHIP=ZW020x  COUNTRY=IN RF=IN
leddimmer_ZW020x_IN:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW020x_RU:MAKELINE=CHIP=ZW020x  COUNTRY=RU RF=RU
leddimmer_ZW020x_RU:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW020x_IL:MAKELINE=CHIP=ZW020x  COUNTRY=IL RF=IL
leddimmer_ZW020x_IL:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

## ZW030x target

leddimmer_ZW030x_EU:MAKELINE=CHIP=ZW030x  COUNTRY=EU RF=EU
leddimmer_ZW030x_EU:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW030x_US:MAKELINE=CHIP=ZW030x  COUNTRY=US RF=US
leddimmer_ZW030x_US:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW030x_HK:MAKELINE=CHIP=ZW030x  COUNTRY=HK RF=HK
leddimmer_ZW030x_HK:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW030x_ANZ:MAKELINE=CHIP=ZW030x  COUNTRY=ANZ  RF=ANZ
leddimmer_ZW030x_ANZ:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW030x_MY:MAKELINE=CHIP=ZW030x  COUNTRY=MY  RF=MY
leddimmer_ZW030x_MY:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW030x_IN:MAKELINE=CHIP=ZW030x  COUNTRY=IN  RF=IN
leddimmer_ZW030x_IN:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW030x_RU:MAKELINE=CHIP=ZW030x  COUNTRY=RU  RF=RU
leddimmer_ZW030x_RU:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_ZW030x_IL:MAKELINE=CHIP=ZW030x  COUNTRY=IL  RF=IL
leddimmer_ZW030x_IL:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

## ZW030x bootloader targets

bootloader_ZW030x_EU:MAKELINE=CHIP=ZW030x  COUNTRY=EU RF=EU BOOTLOADER=1
bootloader_ZW030x_EU:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

bootloader_ZW030x_US:MAKELINE=CHIP=ZW030x  COUNTRY=US RF=US BOOTLOADER=1
bootloader_ZW030x_US:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

bootloader_ZW030x_HK:MAKELINE=CHIP=ZW030x  COUNTRY=HK RF=HK BOOTLOADER=1
bootloader_ZW030x_HK:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

bootloader_ZW030x_ANZ:MAKELINE=CHIP=ZW030x  COUNTRY=ANZ  RF=ANZ BOOTLOADER=1
bootloader_ZW030x_ANZ:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

bootloader_ZW030x_MY:MAKELINE=CHIP=ZW030x  COUNTRY=MY  RF=MY BOOTLOADER=1
bootloader_ZW030x_MY:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

bootloader_ZW030x_IN:MAKELINE=CHIP=ZW030x  COUNTRY=IN  RF=IN BOOTLOADER=1
bootloader_ZW030x_IN:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

bootloader_ZW030x_RU:MAKELINE=CHIP=ZW030x  COUNTRY=RU  RF=RU BOOTLOADER=1
bootloader_ZW030x_RU:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

bootloader_ZW030x_IL:MAKELINE=CHIP=ZW030x  COUNTRY=IL  RF=IL BOOTLOADER=1
bootloader_ZW030x_IL:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

## ZW030x bootloader enabled targets

leddimmer_OTA_ZW030x_EU: bootloader_ZW030x_EU
leddimmer_OTA_ZW030x_EU:MAKELINE=CHIP=ZW030x  COUNTRY=EU RF=EU BOOTLOADER_ENABLED=1
leddimmer_OTA_ZW030x_EU:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_OTA_ZW030x_US: bootloader_ZW030x_US
leddimmer_OTA_ZW030x_US:MAKELINE=CHIP=ZW030x  COUNTRY=US RF=US BOOTLOADER_ENABLED=1
leddimmer_OTA_ZW030x_US:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_OTA_ZW030x_HK: bootloader_ZW030x_HK
leddimmer_OTA_ZW030x_HK:MAKELINE=CHIP=ZW030x  COUNTRY=HK RF=HK BOOTLOADER_ENABLED=1
leddimmer_OTA_ZW030x_HK:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_OTA_ZW030x_ANZ: bootloader_ZW030x_ANZ
leddimmer_OTA_ZW030x_ANZ:MAKELINE=CHIP=ZW030x  COUNTRY=ANZ  RF=ANZ BOOTLOADER_ENABLED=1
leddimmer_OTA_ZW030x_ANZ:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_OTA_ZW030x_MY: bootloader_ZW030x_MY
leddimmer_OTA_ZW030x_MY:MAKELINE=CHIP=ZW030x  COUNTRY=MY  RF=MY BOOTLOADER_ENABLED=1
leddimmer_OTA_ZW030x_MY:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_OTA_ZW030x_IN: bootloader_ZW030x_IN
leddimmer_OTA_ZW030x_IN:MAKELINE=CHIP=ZW030x  COUNTRY=IN  RF=IN BOOTLOADER_ENABLED=1
leddimmer_OTA_ZW030x_IN:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_OTA_ZW030x_RU: bootloader_ZW030x_RU
leddimmer_OTA_ZW030x_RU:MAKELINE=CHIP=ZW030x  COUNTRY=RU  RF=RU BOOTLOADER_ENABLED=1
leddimmer_OTA_ZW030x_RU:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

leddimmer_OTA_ZW030x_IL: bootloader_ZW030x_IL
leddimmer_OTA_ZW030x_IL:MAKELINE=CHIP=ZW030x  COUNTRY=IL  RF=IL BOOTLOADER_ENABLED=1
leddimmer_OTA_ZW030x_IL:
	$(MAKE) depend $(MAKEOPTS) . $(MAKELINE) -f Makefile.common
	$(MAKE) $(MAKEOPTS) . $(MAKELINE) -f Makefile.common

clean:
ifeq ($(OS), Windows_NT)
	@cmd /c "if exist build\NUL rd build /s /q"
else
	@deltree /Y $(subst \,\\,build)
endif

# Catch all targets which we do not handle and tell world that
%:
	@echo Could not find rule for target '$@'
	@echo Some of the known targets are :
	@echo   leddimmer_ZW020x     : ZW020x targets
	@echo   leddimmer_ZW030x     : ZW030x targets


.PHONY: dirs clean depend all default world ZW0x0x ZW030x ZW020x leddimmer_ZW020x leddimmer_ZW030x leddimmer_ZW0x0x\
 ZW020x_EU ZW020x_US ZW020x_ANZ ZW020x_HK ZW020x_MY ZW020x_IN ZW020x_RU ZW020x_IL ZW030x_EU ZW030x_US ZW030x_ANZ ZW030x_HK ZW030x_MY ZW030x_IN ZW030x_RU ZW030x_IL\
 ZW0x0x_NOSEC\
 bootloader_ZW030x\
 leddimmer_OTA_ZW030x\
 Makefile.Testdefines Makefile.TestTargets Makefile.SecureTargets
