#!/bin/bash

SRC=$PWD # current working directory
PATCH=$1 # first command line argument to script is the patch name
shift;
TARGET="$@" # second argument is target, e.g. run
OWLPROGRAM=~/owl/OwlProgram

usage(){
	echo "\

	^^  OWL  ^^	

	Build an OWL patch and upload it to the device.
	You must be in the folder containing the patch.
	$ owl.sh PATCH [TARGET=run] [any other make options/target]

	You need to set the path to OwlProgram in the configuration file
"
}
[ -z "$PATCH" ] && { usage; exit 1; }
[ -z "$TARGET" ] && TARGET=run;

make -C $OWLPROGRAM PATCHNAME=$PATCH PATCHSOURCE=$SRC $TARGET
echo make -C $OWLPROGRAM PATCHNAME=$PATCH PATCHSOURCE=$SRC $TARGET

