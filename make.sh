#!/bin/env sh
PROGRAM_NAME='txtToHTML.c'
BINARY_NAME='txtToHTML'


#musl-clang -O3 -Wall -static "./src/$PROGRAM_NAME" -o "./bin/$BINARY_NAME"
clang -O3 -Wall "./src/$PROGRAM_NAME" -o "./bin/$BINARY_NAME"

