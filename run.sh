#!/bin/sh

# TODO
# - [] Run with valgrind (memory checker)

die() {
	echo $1
	exit
}

# *.c -> *.c.out
compile() {
	make prog=$1 > /dev/null
}

# *.out
run() {
	./$1
}

help() {
	echo "-------------- Usage --------------"
	echo "./run.sh [FOLDER_NAME]\n"
	echo "Example:"
	echo "- ./run.sh WaterJug/"
}

test -z $1 && help && die

# remove "/" on folder name
PROBLEM_DIR_NAME=`echo $1 | sed 's/\///g'`

test -d $PROBLEM_DIR_NAME || die 'Folder not found'
echo "Welcome to $PROBLEM_DIR_NAME!\n"
echo "Which algorithm you want to run?"

i=0
for f in $PROBLEM_DIR_NAME/*.c; do
	echo "- [$i] $f"
	i=`expr $i + 1`
done

echo ""
ans=0
test $i -eq 1 || read -p "Select number [0-`expr $i - 1`]: " ans

i=0
for f in $PROBLEM_DIR_NAME/*.c; do
	test $i -eq $ans && compile $f && run $f.out && die
	i=`expr $i + 1`
done
