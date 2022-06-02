#!/bin/env sh
PROGRAM_NAME='txtToHTML.c'
BINARY_NAME='txtToHTML'

musl-clang -O3 -Wall -static $PROGRAM_NAME -o "./bin/"$BINARY_NAME

