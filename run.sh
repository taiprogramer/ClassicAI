#!/bin/sh

die() {
    echo $1
    exit
}

PROBLEM_DIR_NAME=$1
SOURCE_FILE="$PROBLEM_DIR_NAME"/main.c

test -f $SOURCE_FILE || die 'File not found'

make prog=$SOURCE_FILE > /dev/null && ./"$SOURCE_FILE".out

