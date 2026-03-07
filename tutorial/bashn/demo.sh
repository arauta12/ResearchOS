#! /bin/bash

# check if gcc installed!
lgcc --version > /dev/null 2> /dev/null

if [ $? -eq 0 ]; then
	echo "GCC installed!"
else
	echo "GCC not installed!"
	exit 1
fi
