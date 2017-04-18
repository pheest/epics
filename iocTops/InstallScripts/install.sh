#!/bin/bash 

#	Author:  Peter Heesterman (Tessella plc). Date: 13 Aug 2015.
#	Written for CCFE (Culham Centre for Fusion Energy).

#
# Usage:
# sudo ./install.sh

# check if user has right permissions
if [ "$(id -u)" != "0" ]; then
	echo "Sorry, you are not root. Please try again using sudo."
	exit 1

fi

# terminate script after first line that fails
set -e

# get installation directory from command line argument
DEFAULT_INSTALL_PATH="/usr/local/epics"
if [ -z "$*" ]; then INSTALL_PATH=$DEFAULT_INSTALL_PATH; else INSTALL_PATH=$1;fi

# dependencies

# asyn
if [ ! -d $INSTALL_PATH/support/areaDetector ]; 
then
    ./epics_areadetector_2-6_install.sh $INSTALL_PATH
fi

#pyepics
if [ ! -d $INSTALL_PATH/extensions/src/pyepics ]; 
then
    ./epics_pyepics_3-2-5_install.sh $INSTALL_PATH
fi

if [ ! -d $EPICS_ROOT/extensions/src/medm ];
then
	./epics_medm_3-1-11_install.sh $EPICS_ROOT
fi
