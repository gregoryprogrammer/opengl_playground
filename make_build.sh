#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ -z $DIR ]; then
    echo error: empty current dir
    exit 1
fi

OPTIONS=("DEBUG GCC" "DEBUG CLANG" "RELEASE GCC" "RELEASE CLANG" "quit")
select opt in "${OPTIONS[@]}"
do
    case $opt in
        ${OPTIONS[0]})
            echo "debug gcc"
            CC=gcc
            CXX=g++
            BUILD_DIR=build_debug_gcc
            BUILD_TYPE=Debug
            break
            ;;
        ${OPTIONS[1]})
            echo "debug clang"
            CC=clang
            CXX=clang++
            BUILD_DIR=build_debug_clang
            BUILD_TYPE=Debug
            break
            ;;
        ${OPTIONS[2]})
            echo "release gcc"
            CC=gcc
            CXX=g++
            BUILD_DIR=build_release_gcc
            BUILD_TYPE=Release
            break
            ;;
        ${OPTIONS[3]})
            echo "release clang"
            CC=clang
            CXX=clang++
            BUILD_DIR=build_release_clang
            BUILD_TYPE=Release
            break
            ;;
        "quit")
            exit 0
            ;;
        *)
            echo invalid option
            ;;
    esac
done

echo $CC $CXX $BUILD_DIR $BUILD_TYPE

reset
rm -rf "${DIR}/${BUILD_DIR}"
mkdir "${DIR}/${BUILD_DIR}"
cd "${BUILD_DIR}"
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
make
