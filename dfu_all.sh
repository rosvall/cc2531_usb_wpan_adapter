#!/bin/sh -ex

IFS='="'
dfu-util -l | grep -o 'serial=.*' | while read _ _ SERIAL
do
	$@ -S $SERIAL
done
