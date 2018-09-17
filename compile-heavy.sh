#!/bin/bash

DIR=`dirname "$0"`
HVCC=$DIR/Tools/hvcc/hvcc.py
FILENAME=`basename $MAIN_FILENAME`
TARGETS="c unity wwise bela fabric pdext vst2"

mkdir -p $BUILD_DIR
rm -f $BUILD_DIR/c/HvHeavy.cpp

python2.7 $HVCC $SOURCE_DIR/$FILENAME -g $TARGETS -n "$PATCHNAME" -p $SOURCE_DIR -o $BUILD_DIR

# hvcc always returns 0 so this doesn't work: if [ $? -eq 0 ]
if [ -f $BUILD_DIR/c/HvHeavy.cpp ]
then
    echo "Success!"
else
    echo "Heavy compilation failed" >&2
    exit 1
fi

cd $BUILD_DIR && zip -q patch.zip */*
cp $BUILD_DIR/patch.zip $TARGET_DIR/$PATCH_SEO_NAME.zip
