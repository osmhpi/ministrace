#!/usr/bin/env bash

GCC="gcc"
SED="sed"

HEADER_FILE="unistd_64.h"

ensure_avail() {
   command=$1
   if ! which "${command}" > /dev/null; then
       echo "ERROR: the ${command} is not available on your system."
       exit 1
   fi

   echo "[OK] ${command} available."
}

get_header_locations() {
    RAW=$(LC_ALL=C "${GCC}" -v -E -xc - </dev/null 2>&1 | LC_ALL=C "${SED}" -ne '/starts here/,/End of/p')

    for word in $RAW; do
        if [[ $word == /* ]]; then
            echo "$word"
        fi
    done
}

ensure_avail gcc
ensure_avail sed

headers=$(get_header_locations)

CANDIDATES=""

for header in $headers; do
    echo "CHECKING include dir ${header}..."
    matches=$(find "${header}" -name "${HEADER_FILE}")

    if [ "${#matches}" != "0" ]; then
        echo "[OK] FOUND ${matches}"
        CANDIDATES+=" ${matches}"
    fi

done


if [ "${#CANDIDATES}"  = "0" ]; then
    echo "[ERR] none of the search directories contains ${HEADER_FILE}"
    exit 1
fi

success=0

echo "CAN: $CANDIDATES"

for candidate in $CANDIDATES; do
    OUTPUT=""

    if ! OUTPUT=$(python3 ./get_syscalls.py "${candidate}" 2>&1 ); then
        echo "[ERR] File ${candidate} is invalid."
    else
        echo "[OK] File ${candidate} is valid."
        success=1
        break
    fi
done

if [ $success -eq 0 ]; then
    echo "${OUTPUT}"
    echo "[ERR] could not extract syscalls: all sources failed"
    exit 1
else
    echo "$OUTPUT" > ./syscall_nr.h
fi
