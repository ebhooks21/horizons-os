#!/bin/bash

# Prepares program for submission
# $1 is the file name for the zip file
bzr add *.c *.asm *.h *.sh Makefile *.txt
bzr commit -m "program"
zip -r $1 .bzr
echo "File saved as : ${1}"
