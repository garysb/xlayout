#!/bin/sh
# This little script just does all the commands needed to make and install xlayout
make clean
# Compile the file itself
make
# Check the file was compiled correctly
#if [ -e "xlayout" ]
#then
#	make install
#else
#	echo "I am sorry to say that the program didnt compile for some reason"
#	echo "Please report this to stroppytux@gmail.com with any additional"
#	echo "information."
#fi
