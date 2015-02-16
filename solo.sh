#!/bin/bash

PATCH=$1
echo Generating patch $PATCH
echo '#include "../Libraries/OwlPatches/'$PATCH'Patch.hpp"' > Source/solopatch.h
echo 'REGISTER_PATCH('$PATCH'Patch, "'$PATCH'", 2, 2);' > Source/solopatch.cpp
# make solo
make Build/solo.bin && cp Build/solo.bin patches/$PATCH.bin
# make Build/solo.bin Build/solo.syx && cp Build/solo.bin patches/$PATCH.bin && cp Build/solo.syx patches/$PATCH.syx
