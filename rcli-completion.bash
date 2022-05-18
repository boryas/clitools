#!/usr/bin/env bash

RCLI_ROOT="$HOME/.rcli/clis/"

_rcli_completion() {
	COMPREPLY=()
	local cli_path=$RCLI_ROOT
	local i=1
	while [ $i -lt $COMP_CWORD ]
	do
		cli_path+=${COMP_WORDS[$i]}/
		#COMPREPLY+=("I:$i")
		((i+=1))
	done
	#COMPREPLY+=("PATH:$cli_path")
	#COMPREPLY+=("CWORD:$COMP_CWORD")
	for cli in $cli_path/*
	do
		if [ -d $cli ]
		then
			cli=$(basename $cli)
			COMPREPLY+=($cli)
		fi
	done
}

complete -F _rcli_completion rcli
