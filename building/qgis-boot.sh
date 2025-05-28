#!/usr/bin/bash

#xhost +
xhost +SI:localuser:test

function boot() {
	if [ $# -eq 0 ]; 
	then
		docker exec -it jingweiprinter qgis
	else
		docker exec -it jingweiprinter qgis "$@"
	fi
}

boot "$@"
