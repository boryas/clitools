#!/usr/bin/env bash

RCLI_ROOT="$HOME/.rcli/clis/"

_rcli_completion() {
	COMPREPLY=()
	local cli_path=$RCLI_ROOT
	local cur=${COMP_WORDS[$COMP_CWORD]}
	local i=1
	while [ $i -lt $COMP_CWORD ]
	do
		local itr=${COMP_WORDS[$i]}
		cli_path+=$itr/
		((i+=1))
	done
	if ! [ -d $cli_path ];
	then
		return 0
	fi
	for cli in $cli_path/*
	do
		if [ -d $cli ]
		then
			cli=$(basename $cli)
			if [[ $cli == "$cur"* ]]
			then
				COMPREPLY+=($cli)
			fi
		fi
	done
	return 0
}

complete -F _rcli_completion rcli
