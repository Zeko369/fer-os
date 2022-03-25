#!/bin/sh

echo "1" > status.txt
rm -f obrada.txt
touch obrada.txt

rm *.o 2> /dev/null
rm *.zip 2> /dev/null
