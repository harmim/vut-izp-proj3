#!/usr/bin/env bash

file='../proj3.c'
binary='../proj3'

# Output settings
TEXT_BOLD=`tput bold`
TEXT_GREEN=`tput setaf 2`
TEXT_RED=`tput setaf 1`
TEXT_RESET=`tput sgr0`

# IO file names
function errorFileName () {
	printf "test${1}-error"
}
function inFileName () {
	printf "test${1}-in"
}
function outFileName () {
	printf "test${1}-out"
}
function myFileName () {
	printf "test${1}-my"
}

# Test initialization
function initTest () {
	testNumber=$1
	testError=$(errorFileName $testNumber)
	testOut=$(outFileName $testNumber)
	testMy=$(myFileName $testNumber)
}

# Test evaluation
function testName () {
	testNumber=$1
	printf "Test ${testNumber}.. "
}
function ok () {
	printf "${TEXT_GREEN}OK${TEXT_RESET}\n"
}
function fail () {
	printf "${TEXT_RED}FAIL${TEXT_RESET}\n"
}

function isOk () {
	local testNumber=$1
	local testResult=$2
		local testError=$3

	if [ -e ${testOut} ] 
	then
	
			`diff -q ${testOut} ${testMy} > /dev/null`
			local diffResult=$?


			echo $1 "isOK   Result:" $testResult " diff:" $diffResult
			testName $testNumber
			if [ $testResult == 0 ] && [ $diffResult == 0 ]
			then
		ok
			else
		fail
			fi
	
	else

			echo $1 "isFail Result:" $2 " error:" $3

			testName $testNumber
			if [ $testResult != 0 ] && [ -s $testError ]
			then
		ok
			else
		fail
			fi
	fi

}


function testMem () {
	local testNumber=$1
	local testResult=$2

		echo $1

		cat tmp_error | grep "=="

		if [ $testResult == 0 ]
		then
		fail
		else
		ok

	
	fi

}


# Cleaning
if [ "$1" == "-c" ]
then
	rm -f test*-my test*-error
	exit 0
fi

# File exists
if [ -e $file ]
then
	printf "${TEXT_BOLD}Testing file ${file}${TEXT_RESET}\n"
else
	printf "${TEXT_RED}Cannot run test without file ${file}.${TEXT_RESET}\n"
	exit 1
fi

# Compile source
printf "Compile source code...\n"
`gcc -std=c99 -Wall -Wextra -pedantic -g -DNDEBUG ${file} -o ${binary} -lm`

no=0
#           1             2            3              4                 5                   6  7                  
for par in "test1-in 20" "test2-in 8" "test3-in 100" "test4-in blabla" "test5-in 3 blabla" " " "blablafile.txt 1" "test8-in" "test9-in" "test10-in" "test11-in" "test12-in" "test13-in" "test14-in" "test15-in" "test16-in" "test17-in" "test18-in" "test19-in"  "test20-in" "test21-in"
do

	# Test
	no=$((no+1))

	initTest ${no}
	`./${binary} ${par} > ${testMy} 2> ${testError}`
	isOk $testNumber $? $testError

done

# Valgrind installed
`valgrind --version > /dev/null 2>&1`
valgrindResult=$?
if [ "$1" == "-m" ] && [ $valgrindResult == 0 ]
then
	printf "Memmory leaks will be checked...\n"

	no=0
	#           1             2            3              4                 5                   6  7
	for par in "test1-in 20" "test2-in 8" "test3-in 100" "test4-in blabla" "test5-in 3 blabla" " " "blablafile.txt 1" "test8-in" "test9-in" "test10-in" "test11-in" "test12-in" "test13-in" "test14-in" "test15-in" "test16-in" "test17-in" "test18-in" "test19-in"  "test20-in" "test21-in"
	do

		# Test
		no=$((no+1))

		initTest ${no}
		`valgrind -q --leak-check=full --show-leak-kinds=all ./${binary} ${par} >/dev/null 2>tmp_error`
		grep -q "==" "tmp_error"
		testMem $testNumber $?

	done

else
	if [ "$1" == "-m" ]
	then
		printf "${TEXT_RED}Valgrind not installed. Cannot check memmory leaks.${TEXT_RESET}\n"
	fi
fi

printf "\n"