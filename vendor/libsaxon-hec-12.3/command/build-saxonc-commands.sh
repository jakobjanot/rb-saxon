#!/bin/sh

#Build file for SaxonC on C++
parent_path=$( cd "$(dirname "$0")" ; pwd -P )
library_dir=${1-$(pwd -P)}/../libs/nix
#cd "$parent_path"
gcc -m64 -fPIC -I../Saxon.C.API/graalvm  -c ../Saxon.C.API/SaxonCGlue.c -o SaxonCGlue.o $1

gcc -m64 -fPIC -I../Saxon.C.API/graalvm  Transform.c  -o transform -ldl -lc SaxonCGlue.o -L$library_dir -lsaxon-hec-12.3 $1

gcc  -m64 -fPIC -I../Saxon.C.API/graalvm Query.c -o query -ldl -lc SaxonCGlue.o -L$library_dir -lsaxon-hec-12.3 $1

gcc  -m64 -fPIC -I../Saxon.C.API/graalvm Validate.c -o validate -ldl -lc SaxonCGlue.o -L$library_dir -lsaxon-hec-12.3 $1

gcc  -m64 -fPIC -I../Saxon.C.API/graalvm Gizmo.c -o gizmo -ldl -lc SaxonCGlue.o -L$library_dir -lsaxon-hec-12.3 $1

