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

