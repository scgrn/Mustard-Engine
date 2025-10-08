#!/bin/bash

cp -r ../../projectTemplate test-project
cd test-project
cat > project.cfg <<EOF
PROJECT_NAME=TestProject

MUSTARD_PATH=../../..
SDL2_PATH=/libs/SDL2-2.26.5/cmake
EOF

./build.sh all

#check all build files exist
files=(
    "bin/TestProject-Debug"
    "bin/TestProject-Release"

    "bin/web-debug/index.html"
    "bin/web-debug/TestProject-Web-Debug.data"
    "bin/web-debug/TestProject-Web-Debug.js"
    "bin/web-debug/TestProject-Web-Debug.wasm"

    "bin/web-release/index.html"
    "bin/web-release/TestProject-Web-Release.data"
    "bin/web-release/TestProject-Web-Release.js"
    "bin/web-release/TestProject-Web-Release.wasm"

    "dist/TestProject"
    "dist/TestProject.dat"
)

missing=false
for file in "${files[@]}"; do
    if [[ ! -e "$file" ]]; then
        echo "Missing: $file"
        missing=true
    fi
done

if $missing; then
    exit 1
fi

# ./run.sh release

cd ..
rm -r test-project

