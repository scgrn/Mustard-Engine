#!/bin/bash

CONFIG=$1
source project.cfg

if [ "$CONFIG" == "debug" ]; then
    bin/$PROJECT_NAME-Debug
elif [ "$CONFIG" == "release" ]; then
    bin/$PROJECT_NAME-Release
else
    echo usage: ./run.sh config
    echo
    echo Where config is one of:
    echo
    echo debug
    echo release
    echo
fi

