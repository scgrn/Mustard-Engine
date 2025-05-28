#!/bin/bash

CONFIG=$1
source project.cfg

if [ "$CONFIG" == "debug" ]; then
    bin/$PROJECT_NAME-Debug
elif [ "$CONFIG" == "release" ]; then
    bin/$PROJECT_NAME-Release
elif [ "$CONFIG" == "web-debug" ]; then
    if [[ -z "${EMCMAKE}" ]]; then
        source ~/emsdk/emsdk_env.sh
    fi
    emrun bin/$PROJECT_NAME-Web-Debug.html
elif [ "$CONFIG" == "web-release" ]; then
    if [[ -z "${EMCMAKE}" ]]; then
        source ~/emsdk/emsdk_env.sh
    fi
    emrun bin/$PROJECT_NAME-Web-Release.html
else
    echo usage: ./run.sh config
    echo
    echo Where config is one of:
    echo
    echo debug
    echo release
    echo web-debug
    echo web-release
    echo
fi

